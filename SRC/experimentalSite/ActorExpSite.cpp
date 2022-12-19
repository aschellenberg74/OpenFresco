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

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 09/06
// Revision: A
//
// Description: This file contains the implementation of ActorExpSite.

#include "ActorExpSite.h"

#include <Channel.h>
#include <TCP_Socket.h>
#include <TCP_SocketSSL.h>
#include <UDP_Socket.h>
#include <elementAPI.h>


void* OPF_ActorExpSite()
{
    // pointer to experimental site that will be returned
    ExperimentalSite* theSite = 0;
    
    int numArgs = OPS_GetNumRemainingInputArgs();
    if (4 > numArgs || numArgs > 6) {
        opserr << "WARNING invalid number of arguments\n";
        opserr << "Want: expSite ActorSite tag -setup setupTag ipPort <-udp> <-ssl>\n"
            << "  or: expSite ActorSite tag -control ctrlTag ipPort <-udp> <-ssl>\n";
        return 0;
    }
    
    // site tag
    int tag;
    int numdata = 1;
    if (OPS_GetIntInput(&numdata, &tag) != 0) {
        opserr << "WARNING invalid expSite ActorSite tag\n";
        return 0;
    }
    
    // experimental setup or experimental control
    ExperimentalSetup* theSetup = 0;
    ExperimentalControl* theControl = 0;
    const char* type = OPS_GetString();
    if (strcmp(type, "-setup") == 0) {
        int setupTag;
        numdata = 1;
        if (OPS_GetIntInput(&numdata, &setupTag) != 0) {
            opserr << "WARNING invalid setupTag\n";
            opserr << "expSite ActorSite " << tag << endln;
            return 0;
        }
        theSetup = OPF_getExperimentalSetup(setupTag);
        if (theSetup == 0) {
            opserr << "WARNING experimental setup not found\n";
            opserr << "expSetup: " << setupTag << endln;
            opserr << "expSite ActorSite " << tag << endln;
            return 0;
        }
    }
    else if (strcmp(type, "-control") == 0) {
        int ctrlTag;
        numdata = 1;
        if (OPS_GetIntInput(&numdata, &ctrlTag) != 0) {
            opserr << "WARNING invalid ctrlTag\n";
            opserr << "expSite ActorSite " << tag << endln;
            return 0;
        }
        theControl = OPF_getExperimentalControl(ctrlTag);
        if (theControl == 0) {
            opserr << "WARNING experimental control not found\n";
            opserr << "expControl: " << ctrlTag << endln;
            opserr << "expSite ActorSite " << tag << endln;
            return 0;
        }
    }
    
    // ip port
    int ipPort;
    numdata = 1;
    if (OPS_GetIntInput(&numdata, &ipPort) < 0) {
        opserr << "WARNING invalid ipPort\n";
        opserr << "expSite ActorSite " << tag << endln;
        return 0;
    }
    
    // optional parameters
    int ssl = 0, udp = 0;
    int noDelay = 0;
    while (OPS_GetNumRemainingInputArgs() > 0) {
        type = OPS_GetString();
        if (strcmp(type, "-ssl") == 0) {
            ssl = 1; udp = 0;
        }
        else if (strcmp(type, "-udp") == 0) {
            udp = 1; ssl = 0;
        }
        else if (strcmp(type, "-noDelay") == 0) {
            noDelay = 1;
        }
    }
    
    // parsing was successful, setup the connection and allocate the site
    Channel* theChannel = 0;
    if (ssl) {
        theChannel = new TCP_SocketSSL(ipPort, true, noDelay);
        if (theChannel != 0) {
            opserr << "\nSSL Channel successfully created: "
                << "Waiting for ShadowExpSite...\n";
        }
        else {
            opserr << "WARNING could not create SSL channel\n";
            opserr << "expSite ActorSite " << tag << endln;
            return 0;
        }
    }
    else if (udp) {
        theChannel = new UDP_Socket(ipPort, true);
        if (theChannel != 0) {
            opserr << "\nUDP Channel successfully created: "
                << "Waiting for ShadowExpSite...\n";
        }
        else {
            opserr << "WARNING could not create UDP channel\n";
            opserr << "expSite ActorSite " << tag << endln;
            return 0;
        }
    }
    else {
        theChannel = new TCP_Socket(ipPort, true, noDelay);
        if (theChannel != 0) {
            opserr << "\nTCP Channel successfully created: "
                << "Waiting for ShadowExpSite...\n";
        }
        else {
            opserr << "WARNING could not create TCP channel\n";
            opserr << "expSite ActorSite " << tag << endln;
            return 0;
        }
    }
    
    // parsing was successful, allocate the site
    if (theControl == 0)
        theSite = new ActorExpSite(tag, theSetup, *theChannel);
    else if (theSetup == 0)
        theSite = new ActorExpSite(tag, theControl, *theChannel);
    
    if (theSite == 0) {
        opserr << "WARNING could not create experimental site of type ActorSite\n";
        return 0;
    }
    
    return theSite;
}


ActorExpSite::ActorExpSite(int tag,
    ExperimentalSetup *setup,
    Channel &theChannel,
    FEM_ObjectBroker *theObjectBroker)
    : ExperimentalSite(tag, setup), 
    Actor(theChannel, *theObjectBroker, 0),
    theControl(0), dataSize(0),
    sendV(OF_Network_dataSize), recvV(OF_Network_dataSize)
{ 
    if (theSetup == 0)  {
        opserr << "ActorExpSite::ActorExpSite() - "
            << "if you want to use it without an ExperimentalSetup, "
            << "use another constructor.\n";
        exit(OF_ReturnType_failed);
    }
}


ActorExpSite::ActorExpSite(int tag, 
    ExperimentalControl *control,
    Channel &theChannel,
    FEM_ObjectBroker *theObjectBroker)
    : ExperimentalSite(tag, (ExperimentalSetup*)0), 
    Actor(theChannel, *theObjectBroker, 0),
    theControl(control), dataSize(0),
    sendV(OF_Network_dataSize), recvV(OF_Network_dataSize)
{ 
    if (theControl == 0)  {
        opserr << "ActorExpSite::ActorExpSite() - "
            << "if you want to use it without an ExperimentalControl, "
            << "use another constructor.\n";
        exit(OF_ReturnType_failed);
    }
}


ActorExpSite::ActorExpSite(const ActorExpSite& es)
    : ExperimentalSite(es), Actor(es), 
    theControl(0), dataSize(0),
    sendV(OF_Network_dataSize), recvV(OF_Network_dataSize)
{  
    if (es.theControl != 0)  {
        theControl = (es.theControl)->getCopy();
        if (theControl == 0)  {
            opserr << "ActorExpSite::ActorExpSite() - "
                << "failed to get a copy of control system.\n";
            exit(OF_ReturnType_failed);
        }
        theControl->setup();
    }
    
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
    this->runTill(OF_RemoteTest_DIE);

    return OF_ReturnType_completed;
}


int ActorExpSite::runTill(int exitWhen)
{
    bool exitYet = false;
    int ndim;
    while (exitYet == false)  {
        this->recvVector(recvV);
        int action = (int)recvV(0);
        
        switch (action)  {
        case OF_RemoteTest_open:
            opserr << "\nConnected to ShadowExpSite "
                << recvV(1) << endln;
            sendV(0) = OF_ReturnType_completed;
            sendV(1) = this->getTag();
            sendV(2) = atof(OPF_VERSION);
            this->sendVector(sendV);
            if (recvV(2) != atof(OPF_VERSION))  {
                opserr << "ActorExpSite::run() - OpenFresco Version "
                    << "mismatch:\nActorExpSite Version " << atof(OPF_VERSION)
                    << " != ShadowExpSite Version " << recvV(2) << endln;
                exit(OF_ReturnType_failed);
            }
            if (exitWhen == action)
                exitYet = true;
            break;
        case OF_RemoteTest_setup:
            dataSize = (int)recvV(1);
            this->setup();
            if (exitWhen == action)
                exitYet = true;
            break;
        case OF_RemoteTest_setTrialResponse:
            ndim = 1;
            if (tDisp != 0)  {
                tDisp->Extract(recvV, ndim);
                ndim += getTrialSize(OF_Resp_Disp);
            }
            if (tVel != 0)  {
                tVel->Extract(recvV, ndim);
                ndim += getTrialSize(OF_Resp_Vel);
            }
            if (tAccel != 0)  {
                tAccel->Extract(recvV, ndim);
                ndim += getTrialSize(OF_Resp_Accel);
            }
            if (tForce != 0)  {
                tForce->Extract(recvV, ndim);
                ndim += getTrialSize(OF_Resp_Force);
            }
            if (tTime != 0)  {
                tTime->Extract(recvV, ndim);
            }            
            this->setTrialResponse(tDisp, tVel, tAccel, tForce, tTime);
            if (exitWhen == action)
                exitYet = true;
            break;
        case OF_RemoteTest_commitState:
            if (tTime != 0)  {
                ndim = 1
                    + getTrialSize(OF_Resp_Disp)
                    + getTrialSize(OF_Resp_Vel)
                    + getTrialSize(OF_Resp_Accel)
                    + getTrialSize(OF_Resp_Force);
                tTime->Extract(recvV, ndim);
            }
            this->commitState();
            if (exitWhen == action)
                exitYet = true;
            // check if we need to force a server shutdown
            if (exitWhen == OF_RemoteTest_shutdown)
                exitYet = true;
            break;
        case OF_RemoteTest_getDaqResponse:
            this->checkDaqResponse();
            this->setSendDaqResponse();
            this->sendVector(sendV);
            if (exitWhen == action)
                exitYet = true;
            break;
        case OF_RemoteTest_DIE:
            opserr << "\nDisconnected from ShadowExpSite "
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
            break;
        }
    }
    
    return OF_ReturnType_completed;
}


int ActorExpSite::setup()
{
    this->recvID(*sizeTrial);
    this->recvID(*sizeOut);
    
    this->setSize(*sizeTrial, *sizeOut);
    
    return OF_ReturnType_completed;
}


int ActorExpSite::setSize(ID sizeT, ID sizeO)
{
    // call the base class method
    this->ExperimentalSite::setSize(sizeT, sizeO);
    
    if (theSetup != 0)  {
        theSetup->checkSize(sizeT, sizeO);
    } else if (theControl != 0)  {
        theControl->setSize(sizeT, sizeO);
        theControl->setup();
    }
    
    int nTrial = 0, nOutput = 0;
    for (int i=0; i<OF_Resp_All; i++)  {
        nTrial += sizeT(i);
        nOutput += sizeO(i);
    }
    
    // resize channel Vectors
    sendV.resize(dataSize);
    recvV.resize(dataSize);
    
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
    if (theSetup != 0)  {
        rValue = theSetup->setTrialResponse(tDisp, tVel, tAccel, tForce, tTime);
        if (rValue != OF_ReturnType_completed)  {
            opserr << "ActorExpSite::setTrialResponse() - "
                << "failed to set trial response at the setup.\n";
            exit(OF_ReturnType_failed);
        }
        
        // get daq response from the setup
        rValue = theSetup->getDaqResponse(oDisp, oVel, oAccel, oForce, oTime);
        if (rValue != OF_ReturnType_completed)  {
            opserr << "ActorExpSite::setTrialResponse() - "
                << "failed to get daq response from the setup.\n";
            exit(OF_ReturnType_failed);
        }
    
    // set trial response at the control
    } else if (theControl != 0)  {
        rValue = theControl->setTrialResponse(tDisp, tVel, tAccel, tForce, tTime);
        if (rValue != OF_ReturnType_completed)  {
            opserr << "ActorExpSite::setTrialResponse() - "
                << "failed to set trial response at the control.\n";
            exit(OF_ReturnType_failed);
        }
        
        // get daq response from the control
        rValue = theControl->getDaqResponse(oDisp, oVel, oAccel, oForce, oTime);
        if (rValue != OF_ReturnType_completed)  {
            opserr << "ActorExpSite::setTrialResponse() - "
                << "failed to get daq response from the control.\n";
            exit(OF_ReturnType_failed);
        }
    }
    
    // save data
    this->ExperimentalSite::setDaqResponse(oDisp, oVel, oAccel, oForce, oTime); 
    
    // set daq flag
    daqFlag = true;
    
    return OF_ReturnType_completed;
}


int ActorExpSite::checkDaqResponse()
{
    if (daqFlag == false)  {
        int rValue;
        // get daq response from the setup
        if (theSetup != 0)  {
            rValue = theSetup->getDaqResponse(oDisp, oVel, oAccel, oForce, oTime);
            if (rValue != OF_ReturnType_completed)  {
                opserr << "ActorExpSite::checkDaqResponse() - "
                    << "failed to get daq response from the setup.\n";
                exit(OF_ReturnType_failed);
            }
        
        // get daq response from the control
        } else if (theControl != 0)  {
            rValue = theControl->getDaqResponse(oDisp, oVel, oAccel, oForce, oTime);
            if (rValue != OF_ReturnType_completed)  {
                opserr << "ActorExpSite::checkDaqResponse() - "
                    << "failed to get daq response from the control.\n";
                exit(OF_ReturnType_failed);
            }
        }
        
        // save data
        this->ExperimentalSite::setDaqResponse(oDisp, oVel, oAccel, oForce, oTime);
        
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
    if (size != 0)  {
        sendV.Assemble(*oDisp, ndim);
        ndim += size;
    }
    size = getOutSize(OF_Resp_Vel);
    if (size != 0)  {
        sendV.Assemble(*oVel, ndim);
        ndim += size;
    }
    size = getOutSize(OF_Resp_Accel);
    if (size != 0)  {
        sendV.Assemble(*oAccel, ndim);
        ndim += size;
    }
    size = getOutSize(OF_Resp_Force);
    if (size != 0)  {
        sendV.Assemble(*oForce, ndim);
        ndim += size;
    }
    size = getOutSize(OF_Resp_Time);
    if (size != 0)  {
        sendV.Assemble(*oTime, ndim);
    }
    
    return OF_ReturnType_completed;
}


int ActorExpSite::commitState(Vector* time)
{
    int rValue = 0;
    
    // update the trial time vector
    if (time != 0 && tTime != 0)
        *tTime = *time;
    
    // first commit the control
    if (theControl != 0)  {
        rValue += theControl->commitState();
        if (rValue != OF_ReturnType_completed)  {
            opserr << "ActorExpSite::commitState() - "
                << "failed to commit state for the control.\n";
            exit(OF_ReturnType_failed);
        }
    }
    
    // then commit base class
    rValue += this->ExperimentalSite::commitState();
    
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
    if (theSetup != 0)  {
        s << "\tExperimentalSetup tag: " << theSetup->getTag() << endln;
        s << *theSetup;
    } else if (theControl != 0)  {
        s << "\tExperimentalControl tag: " << theControl->getTag() << endln;
        s << *theControl;
    }
}
