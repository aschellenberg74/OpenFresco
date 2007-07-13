/* ****************************************************************** **
**    OpenFRESCO - Open Framework                                     **
**                 for Experimental Setup and Control                 **
**                                                                    **
**                                                                    **
** Copyright (c) 2006, Yoshikazu Takahashi, Kyoto University          **
** All rights reserved.                                               **
**                                                                    **
** Licensed under the modified BSD License (the "License");           **
** you may not use this file except in compliance with the License.   **
** You may obtain a copy of the License in main directory.            **
** Unless required by applicable law or agreed to in writing,         **
** software distributed under the License is distributed on an        **
** "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,       **
** either express or implied. See the License for the specific        **
** language governing permissions and limitations under the License.  **
**                                                                    **
** Developed by:                                                      **
**   Yoshikazu Takahashi (yos@catfish.dpri.kyoto-u.ac.jp)             **
**   Andreas Schellenberg (andreas.schellenberg@gmx.net)              **
**   Gregory L. Fenves (fenves@berkeley.edu)                          **
**                                                                    **
** ****************************************************************** */

// $Revision$
// $Date$
// $URL: $

// Written: Yoshi (yos@catfish.dpri.kyoto-u.ac.jp)
// Created: 09/06
// Revision: A
//
// Description: This file contains the implementation of ActorExpSite.

#include <ID.h>
#include <Vector.h>
#include <Matrix.h>
#include <stdlib.h>

#include <ExperimentalCP.h>
#include <ExperimentalSetup.h>
#include <ExperimentalControl.h>
#include <ActorExpSite.h>


ActorExpSite::ActorExpSite(int tag, 
    ExperimentalSetup *setup,
    Channel &theChannel,
    FEM_ObjectBroker *theObjectBroker)
    : ExperimentalSite(tag, setup), 
    Actor(theChannel, *theObjectBroker, 0),
    theControl(0), msgData(2), dataSize(0),
    sendV(OF_Network_dataSize), recvV(OF_Network_dataSize)
{ 
    if (theSetup == 0) {
        opserr << "ActorExpSite::ActorExpSite() - "
            << "if you want to use it without an ExperimentalSetup, "
            << "use another constructor"
            << endln;
        exit(OF_ReturnType_failed);
    }
}


ActorExpSite::ActorExpSite(int tag, 
    ExperimentalControl *control,
    Channel &theChannel,
    FEM_ObjectBroker *theObjectBroker)
    : ExperimentalSite(tag, (ExperimentalSetup*)0), 
    Actor(theChannel, *theObjectBroker, 0),
    theControl(control), msgData(2), dataSize(0),
    sendV(OF_Network_dataSize), recvV(OF_Network_dataSize)
{ 
    if (theControl == 0) {
        opserr << "ActorExpSite::ActorExpSite() - "
            << "if you want to use it without an ExperimentalControl, "
            << "use another constructor"
            << endln;
        exit(OF_ReturnType_failed);
    }
}


ActorExpSite::ActorExpSite(const ActorExpSite& es)
    : ExperimentalSite(es), Actor(es), 
    theControl(es.theControl), msgData(2), dataSize(0),
    sendV(OF_Network_dataSize), recvV(OF_Network_dataSize)
{  
    dataSize = es.dataSize;
    
    sendV.resize(dataSize);
    recvV.resize(dataSize);
}


ActorExpSite::~ActorExpSite()
{
    if (theControl != 0) 
        delete theControl;
}


int ActorExpSite::run()
{
    bool exitYet = false;
    int ndim;
    while (exitYet == false) {
        this->recvVector(recvV);
        int action = (int)recvV(0);
        
        // opserr << "action = " << action << endln;
        switch(action) {
        case OF_RemoteTest_open:
            opserr << "\nConnected to RemoteExpSite "
                << recvV(1) << endln;
            sendV(0) = OF_ReturnType_completed;
            sendV(1) = this->getTag();
            this->sendVector(sendV);
            break;
        case OF_RemoteTest_setup:
            dataSize = (int)recvV(1);
            this->setup();
            break;
        case OF_RemoteTest_setTrialResponse:
            ndim = 1;
            if (tDisp != 0) {
                tDisp->Extract(recvV, ndim);
                ndim += getTrialSize(OF_Resp_Disp);
            }
            if (tVel != 0) {
                tVel->Extract(recvV, ndim);
                ndim += getTrialSize(OF_Resp_Vel);
            }
            if (tAccel != 0) {
                tAccel->Extract(recvV, ndim);
                ndim += getTrialSize(OF_Resp_Accel);
            }
            if (tForce != 0) {
                tForce->Extract(recvV, ndim);
                ndim += getTrialSize(OF_Resp_Force);
            }
            if (tTime != 0) {
                tTime->Extract(recvV, ndim);
            }            
            this->setTrialResponse(tDisp, tVel, tAccel, tForce, tTime);
            break;
        case OF_RemoteTest_commitState:
            this->commitState();
            break;
        case OF_RemoteTest_getDaqResponse:
            this->checkDaqResponse();
            this->setSendDaqResponse();
            this->sendVector(sendV);
            break;
        case OF_RemoteTest_DIE:
            opserr << "\nDisconnected from RemoteExpSite "
                << recvV(1) << endln << endln;
            sendV(0) = OF_ReturnType_received;
            sendV(1) = this->getTag();
            this->sendVector(sendV);
            exitYet = true;
            break;
        default:
            opserr << "ActorExpSite::run() - invalid action "
                << action << " received" << endln;
            recvV(0) = OF_ReturnType_failed;
        }
    }
    
    return OF_ReturnType_completed;
}


int ActorExpSite::setSize(ID sizeT, ID sizeO)
{
    this->ExperimentalSite::setSize(sizeT, sizeO);
    
    if (theSetup != 0) {
        theSetup->setSize(sizeT, sizeO);
    } else if (theControl != 0) {
        theControl->setSize(sizeT, sizeO);
        theControl->setup();
    }
    
    int nTrial = 0, nOutput = 0;
    for (int i=0; i<OF_Resp_All; i++) {
        nTrial += sizeT[i];
        nOutput += sizeO[i];
    }
    
    // resize channel Vectors
    sendV.resize(dataSize);
    recvV.resize(dataSize);
    
    return OF_ReturnType_completed;
}


int ActorExpSite::setup()
{
    this->recvID(*sizeTrial);    
    this->recvID(*sizeOut);
        
    this->setSize(*sizeTrial, *sizeOut);
        
    return OF_ReturnType_completed;
}


int ActorExpSite::setTrialResponse(const Vector* disp,
    const Vector* vel,
    const Vector* accel,
    const Vector* force,
    const Vector* time)
{
    // save data in basic sys
    this->ExperimentalSite::setTrialResponse(disp, vel, accel, force, time);
    
    // set daq flag
    daqFlag = false;
    
    int rValue;
    // set trial response at the setup
    if (theSetup != 0) {
        rValue = theSetup->setTrialResponse(tDisp, tVel, tAccel, tForce, tTime);
        if (rValue != OF_ReturnType_completed) {
            opserr << "ActorExpSite::setTrialResponse() - "
                << "failed to set trial response at the setup";
            exit(OF_ReturnType_failed);
        }
        
        // transform daq response into output response
        theSetup->transfDaqResponse(Disp, Vel, Accel, Force, Time);
        
    } else if (theControl != 0) {
        theControl->setTrialResponse(tDisp, tVel, tAccel, tForce, tTime);
        
        theControl->getDaqResponse(Disp, Vel, Accel, Force, Time);
    }
    this->ExperimentalSite::setDaqResponse(Disp, Vel, Accel, Force, Time); 
    
    // set daq flag
    daqFlag = true;
    
    return OF_ReturnType_completed;
}


int ActorExpSite::checkDaqResponse()
{
    // don't use arguments in this method. only save daq data into sendV
    if (daqFlag == false) {
        if (theSetup != 0) {
            theSetup->getDaqResponse(Disp, Vel, Accel, Force, Time);
        } else if (theControl != 0) {
            theControl->getDaqResponse(Disp, Vel, Accel, Force, Time);
        }
        this->ExperimentalSite::setDaqResponse(Disp, Vel, Accel, Force, Time);
        // set daq flag
        daqFlag = true;
    }
    
    return OF_ReturnType_completed;
}


int ActorExpSite::setSendDaqResponse()
{
    int ndim = 0, size;
    sendV.Zero();
    size = getOutSize(OF_Resp_Disp);
    if (size != 0) {
        sendV.Assemble(*Disp, ndim);
        ndim += size;
    }
    size = getOutSize(OF_Resp_Vel);
    if (size != 0) {
        sendV.Assemble(*Vel, ndim);
        ndim += size;
    }
    size = getOutSize(OF_Resp_Accel);
    if (size != 0) {
        sendV.Assemble(*Accel, ndim);
        ndim += size;
    }
    size = getOutSize(OF_Resp_Force);
    if (size != 0) {
        sendV.Assemble(*Force, ndim);
        ndim += size;
    }
    size = getOutSize(OF_Resp_Time);
    if (size != 0) {
        sendV.Assemble(*Time, ndim);
    }
    
    return OF_ReturnType_completed;
}


int ActorExpSite::commitState()
{
    int rValue;
    if (theSetup != 0) {
        rValue = theSetup->commitState();
    } else if (theControl != 0) {
        rValue = theControl->commitState();
    }
    
    return rValue;
}


ExperimentalSite* ActorExpSite::getCopy()
{
    ActorExpSite *theCopy = new ActorExpSite(*this);
    
    return theCopy;
}


void ActorExpSite::Print(OPS_Stream &s, int flag)
{
    s << "ActorExpSite: " << this->getTag(); 
    if (theSetup != 0) {
        s << "\tExperimentalSetup tag: " << theSetup->getTag()
            << endln;
        s << *theSetup;
    } else if (theControl != 0) {
        s << "\tExperimentalControl tag: " << theControl->getTag()
            << endln;
        s << *theControl;
    }
}
