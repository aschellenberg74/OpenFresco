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
// $Source$

// Written: Yoshi (yos@catfish.dpri.kyoto-u.ac.jp)
// Created: 09/06
// Revision: A
//
// Description: This file contains the implementation of RemoteExpSite.

#include <ID.h>
#include <Vector.h>
#include <ArrayOfTaggedObjects.h>

#include <ExperimentalCP.h>
#include <ExperimentalSetup.h>
#include <RemoteExpSite.h>


RemoteExpSite::RemoteExpSite(int tag,
    Channel& theChannel,
    int dataSize,
    FEM_ObjectBroker *theObjectBroker)
    : ExperimentalSite(tag, (ExperimentalSetup*)0),
    Shadow(theChannel, *theObjectBroker),
    msgData(2), sendDataSize(dataSize), recvDataSize(dataSize),
    sendV(0), recvV(0),
    bDisp(0), bVel(0), bAccel(0), bForce(0), bTime(0),
    rDisp(0), rVel(0), rAccel(0), rForce(0), rTime(0)
{
    msgData(0) = OF_RemoteTest_open;
    this->sendID(msgData);
    this->recvID(msgData);
    
    opserr << "\nConnected to ActorExpSite.\n";
}


RemoteExpSite::RemoteExpSite(int tag, 
    ExperimentalSetup *setup,
    Channel &theChannel,
    int dataSize,
    FEM_ObjectBroker *theObjectBroker)
    : ExperimentalSite(tag, setup),
    Shadow(theChannel, *theObjectBroker),
    msgData(2), sendDataSize(dataSize), recvDataSize(dataSize),
    sendV(0), recvV(0),
    bDisp(0), bVel(0), bAccel(0), bForce(0), bTime(0),
    rDisp(0), rVel(0), rAccel(0), rForce(0), rTime(0)
{
    if(theSetup == 0) {
        opserr << "FATAL RemoteExpSite::RemoteExpSite - "
            << "if you want to use it without an ExperimentalSetup, "
            << "use another constructor."
            << endln;
        exit(OF_ReturnType_failed);
    }
    msgData(0) = OF_RemoteTest_open;
    this->sendID(msgData);
    this->recvID(msgData);
    
    opserr << "\nConnected to ActorExpSite.\n";
}


RemoteExpSite::RemoteExpSite(const RemoteExpSite& es)
    : ExperimentalSite(es), Shadow(es),
    msgData(2), sendDataSize(0), recvDataSize(0),
    sendV(0), recvV(0),
    bDisp(0), bVel(0), bAccel(0), bForce(0), bTime(0),
    rDisp(0), rVel(0), rAccel(0), rForce(0), rTime(0)
{
    sendDataSize = es.sendDataSize;
    recvDataSize = es.recvDataSize;
    
    sendV = new Vector((es.sendV)->Size());
    recvV = new Vector((es.recvV)->Size());
}


RemoteExpSite::~RemoteExpSite()
{
    if(sendV != 0)
        delete sendV;
    if(recvV != 0)
        delete recvV;
    
    if(bDisp != 0)
        delete bDisp;
    if(bVel != 0)
        delete bVel;
    if(bAccel != 0)
        delete bAccel;
    if(bForce != 0)
        delete bForce;
    if(bTime != 0)
        delete bTime;
    if(rDisp != 0)
        delete rDisp;
    if(rVel != 0)
        delete rVel;
    if(rAccel != 0)
        delete rAccel;
    if(rForce != 0)
        delete rForce;
    if(rTime != 0)
        delete rTime;
    
    msgData(0) = OF_RemoteTest_DIE;
    this->sendID(msgData);
    this->recvID(msgData);
    opserr << "\nDisconnected from ActorExpSite.\n";
}


int RemoteExpSite::setSize(ID sizeT, ID sizeO) 
{
    this->ExperimentalSite::setSize(sizeT, sizeO);
    
    if(theSetup != 0) {
        theSetup->setSize(sizeT, sizeO);
        
        int nCtrl = 0, nDaq = 0;
        for(int i=0; i<OF_Resp_All; i++) {
            nCtrl += getCtrlSize(i);
            nDaq += getDaqSize(i);
        }
        // 4 is a magic number....
        if(sendDataSize < nCtrl*4) sendDataSize = nCtrl*4;
        if(recvDataSize < nDaq*4) recvDataSize = nDaq*4;
    } else {
        int nInput = 0, nOutput = 0;
        for(int i=0; i<OF_Resp_All; i++) {
            nInput += sizeT[i];
            nOutput += sizeO[i];
        }
        // 4 is a magic number....
        if(sendDataSize < nInput*4) sendDataSize = nInput*4;
        if(recvDataSize < nOutput*4) recvDataSize = nOutput*4;
    }
    opserr << "sendDataSize = " << sendDataSize << ", recvDataSize = " << recvDataSize << endln;
    // setting of channel Vectors
    sendV = new Vector(sendDataSize);
    recvV = new Vector(recvDataSize);
    
    // send experimental setup to remote
    this->setup();
    
    return OF_ReturnType_completed;
}


int RemoteExpSite::setup()
{
    //if(theSetup != 0) {
    //    theSetup->setup();
    //}
    
    msgData(0) = OF_RemoteTest_setup;
    this->sendID(msgData);
    this->recvID(msgData);
    if(msgData(0) != OF_ReturnType_accepted) {
        opserr << "Failed in RemoteExpSite::setup()";
        msgData(0) = OF_RemoteTest_DIE;
        this->sendID(msgData);
        this->recvID(msgData);
        exit(OF_ReturnType_failed);
    }
    
    if(theSetup != 0) {
        // send sizeCtrl
        this->sendID(theSetup->getCtrlSize());
        this->recvID(msgData);
        if(msgData(0) != OF_ReturnType_completed) {
            opserr << "Failed in RemoteExpSite::setup()";
            msgData(0) = OF_RemoteTest_DIE;
            this->sendID(msgData);
            this->recvID(msgData);
            exit(OF_ReturnType_failed);
        }
        
        // send sizeDaq
        this->sendID(theSetup->getDaqSize());
        this->recvID(msgData);
        if(msgData(0) != OF_ReturnType_completed) {
            opserr << "Failed in RemoteExpSite::setup()";
            msgData(0) = OF_RemoteTest_DIE;
            this->sendID(msgData);
            this->recvID(msgData);
            exit(OF_ReturnType_failed);
        }
    } else {
        // send sizeTrial
        this->sendID(*sizeTrial);
        this->recvID(msgData);
        if(msgData(0) != OF_ReturnType_completed) {
            opserr << "Failed in RemoteExpSite::setup()";
            msgData(0) = OF_RemoteTest_DIE;
            this->sendID(msgData);
            this->recvID(msgData);
            exit(OF_ReturnType_failed);
        }
        
        // send sizeOut
        this->sendID(*sizeOut);
        this->recvID(msgData);
        if(msgData(0) != OF_ReturnType_completed) {
            opserr << "Failed in RemoteExpSite::setup()";
            msgData(0) = OF_RemoteTest_DIE;
            this->sendID(msgData);
            this->recvID(msgData);
            exit(OF_ReturnType_failed);
        }
    }
    
    // receive result
    this->recvID(msgData);
    opserr << "msgData at setup : " << msgData << endln;
    if(msgData(0) != OF_ReturnType_completed) {
        opserr << "\nFailed in RemoteExpSite::setup()";
        msgData(0) = OF_RemoteTest_DIE;
        this->sendID(msgData);
        this->recvID(msgData);
        exit(1);
    }

    return OF_ReturnType_completed;
}


int RemoteExpSite::setTrialResponse(const Vector* disp, 
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
    if(theSetup != 0) {
        // set trial response to the setup
        rValue = theSetup->transfTrialResponse(tDisp, tVel, tAccel, tForce, tTime);
        if(rValue != OF_ReturnType_completed) {
            opserr << "Fail to set trial response to the setup.";
            exit(OF_ReturnType_failed);
        }
        
        if(bDisp == 0) {
            if(getCtrlSize(OF_Resp_Disp) != 0)
                bDisp = new Vector(getCtrlSize(OF_Resp_Disp));
            if(getCtrlSize(OF_Resp_Vel) != 0)
                bVel = new Vector(getCtrlSize(OF_Resp_Vel));
            if(getCtrlSize(OF_Resp_Accel) != 0)
                bAccel = new Vector(getCtrlSize(OF_Resp_Accel));
            if(getCtrlSize(OF_Resp_Force) != 0)
                bForce = new Vector(getCtrlSize(OF_Resp_Force));
            if(getCtrlSize(OF_Resp_Time) != 0)
                bTime = new Vector(getCtrlSize(OF_Resp_Time));
        }
        // get trial response from the setup
        rValue = theSetup->getTrialResponse(bDisp, bVel, bAccel, bForce, bTime);
        if(rValue != OF_ReturnType_completed) {
            opserr << "Fail to get trial response to the setup.";
            exit(OF_ReturnType_failed);
        }
        
        int ndim = 0, size;
        sendV->Zero();
        size = getCtrlSize(OF_Resp_Disp);
        if(size != 0) {
            sendV->Assemble(*bDisp, 0);
            ndim += size;
        }
        size = getCtrlSize(OF_Resp_Vel);
        if(size != 0) {
            sendV->Assemble(*bVel, ndim);
            ndim += size;
        }
        size = getCtrlSize(OF_Resp_Accel);
        if(size != 0) {
            sendV->Assemble(*bAccel, ndim);
            ndim += size;
        }
        size = getCtrlSize(OF_Resp_Force);
        if(size != 0) {
            sendV->Assemble(*bForce, ndim);
            ndim += size;
        }
        size = getCtrlSize(OF_Resp_Time);
        if(size != 0) {
            sendV->Assemble(*bTime, ndim);
        }
    } else {
        int ndim = 0, size;
        sendV->Zero();
        
        size = getTrialSize(OF_Resp_Disp);
        if(size != 0) {
            sendV->Assemble(*tDisp, 0);
            ndim += size;
        }
        size = getTrialSize(OF_Resp_Vel);
        if(size != 0) {
            sendV->Assemble(*tVel, ndim);
            ndim += size;
        }
        size = getTrialSize(OF_Resp_Accel);
        if(size != 0) {
            sendV->Assemble(*tAccel, ndim);
            ndim += size;
        }
        size = getTrialSize(OF_Resp_Force);
        if(size != 0) {
            sendV->Assemble(*tForce, ndim);
            ndim += size;
        }
        size = getTrialSize(OF_Resp_Time);
        if(size != 0) {
            sendV->Assemble(*tTime, ndim);
        }
    }
    
    //set trial response
    msgData(0) = OF_RemoteTest_setTrialResponse;
    this->sendID(msgData);
    
    //  opserr << "sendV = " << *sendV;
    this->sendVector(*sendV);
    
    // receive result
    this->recvID(msgData);
    if(msgData(0) != OF_ReturnType_completed) {
        opserr << "Failed in RemoteExpSite::setTrialResponse()";
        msgData(0) = OF_RemoteTest_DIE;
        this->sendID(msgData);
        this->recvID(msgData);
        exit(OF_ReturnType_failed);
    }
    
    return OF_ReturnType_completed;
}


int RemoteExpSite::checkDaqResponse()
{
    if(daqFlag == false) {
        msgData(0) = OF_RemoteTest_getDaqResponse;
        this->sendID(msgData);
        
        // receive result
        this->recvID(msgData);
        if(msgData(0) != OF_ReturnType_accepted) {
            opserr << "Failed in RemoteExpSite::getDaqResopnse()";
            msgData(0) = OF_RemoteTest_DIE;
            this->sendID(msgData);
            this->recvID(msgData);
            exit(OF_ReturnType_failed);
        }
        
        if(rDisp == 0) {
            if(getDaqSize(OF_Resp_Disp) != 0)
                rDisp = new Vector(getDaqSize(OF_Resp_Disp));
            if(getDaqSize(OF_Resp_Vel) != 0)
                rVel = new Vector(getDaqSize(OF_Resp_Vel));
            if(getDaqSize(OF_Resp_Accel) != 0)
                rAccel = new Vector(getDaqSize(OF_Resp_Accel));
            if(getDaqSize(OF_Resp_Force) != 0)
                rForce = new Vector(getDaqSize(OF_Resp_Force));
            if(getDaqSize(OF_Resp_Time) != 0)
                rTime = new Vector(getDaqSize(OF_Resp_Time));
        }
        this->recvVector(*recvV);
        //    opserr << "recvV = " << recvV;
        
        // receive result
        this->recvID(msgData);
        if(msgData(0) != OF_ReturnType_completed) {
            opserr << "Failed in RemoteExpSite::getDaqResponse()";
            msgData(0) = OF_RemoteTest_DIE;
            this->sendID(msgData);
            this->recvID(msgData);
            exit(OF_ReturnType_failed);
        }
        
        int ndim = 0;
        if(rDisp != 0) {
            rDisp->Extract(*recvV, 0);
            ndim += getDaqSize(OF_Resp_Disp);
        }
        if(rVel != 0) {
            rVel->Extract(*recvV, ndim);
            ndim += getDaqSize(OF_Resp_Vel);
        }
        if(rAccel != 0) {
            rAccel->Extract(*recvV, ndim);
            ndim += getDaqSize(OF_Resp_Accel);
        }
        if(rForce != 0) {
            rForce->Extract(*recvV, ndim);
            ndim += getDaqSize(OF_Resp_Force);
        }
        if(rTime != 0) {
            rTime->Extract(*recvV, ndim);
        }
        
        if(theSetup != 0) {
            // set daq response into ExperimentalSetup
            theSetup->setDaqResponse(rDisp, rVel, rAccel, rForce, rTime);
            
            // transform daq response into output response
            theSetup->transfDaqResponse(Disp, Vel, Accel, Force, Time);
        } else {
            if(Disp != 0) 
                *Disp = *rDisp;
            if(Vel != 0) 
                *Vel = *rVel;
            if(Accel != 0) 
                *Accel = *rAccel;
            if(Force != 0) 
                *Force = *rForce;
            if(Time != 0) 
                *Time = *rTime;
        }
        this->ExperimentalSite::setDaqResponse(Disp, Vel, Accel, Force, Time); 
        
        // set daq flag
        daqFlag = true;
    }
    
    return OF_ReturnType_completed;
}


int RemoteExpSite::commitState()
{
    msgData(0) = OF_RemoteTest_commitState;
    this->sendID(msgData);
    
    // receive result
    this->recvID(msgData);
    if(msgData(0) != OF_ReturnType_completed) {
        opserr << "Failed in RemoteExpSite::commitState()";
        msgData(0) = OF_RemoteTest_DIE;
        this->sendID(msgData);
        this->recvID(msgData);
        exit(1);
    }
    
    return OF_ReturnType_completed;
}


ExperimentalSite* RemoteExpSite::getCopy()
{
    RemoteExpSite *theCopy = new RemoteExpSite(*this);

    return theCopy;
}


void RemoteExpSite::Print(OPS_Stream &s, int flag)
{
    s << "ExperimentalSite: " << this->getTag(); 
    s << " type: RemoteExpSite\n";
    if(theSetup != 0) {
        s << "\tExperimentalSetup tag: " << theSetup->getTag()
            << endln;
        s << *theSetup;
    }
}

