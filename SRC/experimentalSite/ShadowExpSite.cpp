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
// Description: This file contains the implementation of ShadowExpSite.

#include "ShadowExpSite.h"

#include <Channel.h>
#include <TCP_Socket.h>
#include <TCP_SocketSSL.h>
#include <UDP_Socket.h>
#include <elementAPI.h>


void* OPF_ShadowExpSite()
{
    // pointer to experimental site that will be returned
    ExperimentalSite* theSite = 0;
    
    int numArgs = OPS_GetNumRemainingInputArgs();
    if (3 > numArgs || numArgs > 7) {
        opserr << "WARNING invalid number of arguments\n";
        opserr << "Want: expSite ShadowSite tag <-setup setupTag> ipAddr ipPort <-udp> <-ssl> <-dataSize size>\n";
        return 0;
    }
    
    // site tag
    int tag;
    int numdata = 1;
    if (OPS_GetIntInput(&numdata, &tag) != 0) {
        opserr << "WARNING invalid expSite ShadowSite tag\n";
        return 0;
    }
    
    // experimental setup (optional)
    ExperimentalSetup* theSetup = 0;
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
    } else {
        // move current arg back by one
        OPS_ResetCurrentInputArg(-1);
    }
    
    // ip address
    char* ipAddr;
    type = OPS_GetString();
    ipAddr = new char[strlen(type) + 1];
    strcpy(ipAddr, type);
    
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
    int dataSize = OF_Network_dataSize;
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
        else if (strcmp(type, "-dataSize") == 0) {
            if (OPS_GetIntInput(&numdata, &dataSize) < 0) {
                opserr << "WARNING invalid ShadowSite dataSize\n";
                opserr << "expSite ShadowSite " << tag << endln;
                return 0;
            }
        }
    }
    
    // parsing was successful, setup the connection and allocate the site
    Channel* theChannel = 0;
    if (ssl) {
        theChannel = new TCP_SocketSSL(ipPort, ipAddr, true, noDelay);
        if (!theChannel) {
            opserr << "WARNING could not create SSL channel\n";
            opserr << "expSite ShadowSite " << tag << endln;
            return 0;
        }
    }
    else if (udp) {
        theChannel = new UDP_Socket(ipPort, ipAddr, true);
        if (!theChannel) {
            opserr << "WARNING could not create UDP channel\n";
            opserr << "expSite ShadowSite " << tag << endln;
            return 0;
        }
    }
    else {
        theChannel = new TCP_Socket(ipPort, ipAddr, true, noDelay);
        if (!theChannel) {
            opserr << "WARNING could not create TCP channel\n";
            opserr << "expSite ShadowSite " << tag << endln;
            return 0;
        }
    }
    
    // parsing was successful, allocate the site
    if (theSetup == 0)
        theSite = new ShadowExpSite(tag, *theChannel, dataSize);
    else
        theSite = new ShadowExpSite(tag, theSetup, *theChannel, dataSize);
    
    if (theSite == 0) {
        opserr << "WARNING could not create experimental site of type ShadowSite\n";
        return 0;
    }
    
    // cleanup dynamic memory
    if (ipAddr != 0)
        delete[] ipAddr;
    
    return theSite;
}


ShadowExpSite::ShadowExpSite(int tag,
    Channel& theChannel, int datasize,
    FEM_ObjectBroker *theObjectBroker)
    : ExperimentalSite(tag, (ExperimentalSetup*)0),
    Shadow(theChannel, *theObjectBroker), dataSize(datasize),
    sendV(OF_Network_dataSize), recvV(OF_Network_dataSize),
    bDisp(0), bVel(0), bAccel(0), bForce(0), bTime(0),
    rDisp(0), rVel(0), rAccel(0), rForce(0), rTime(0)
{
    sendV(0) = OF_RemoteTest_open;
    sendV(1) = tag;
    sendV(2) = atof(OPF_VERSION);
    this->sendVector(sendV);
    this->recvVector(recvV);
    
    if (recvV(2) != atof(OPF_VERSION))  {
        opserr << "ShadowExpSite::ShadowExpSite() - OpenFresco Version "
            << "mismatch:\nShadowExpSite Version " << atof(OPF_VERSION)
            << " != ActorExpSite Version " << recvV(2) << endln;
        exit(OF_ReturnType_failed);
    }
    
    opserr << "\nConnected to ActorExpSite "
        << recvV(1) << endln;
}


ShadowExpSite::ShadowExpSite(int tag, 
    ExperimentalSetup *setup,
    Channel &theChannel, int datasize,
    FEM_ObjectBroker *theObjectBroker)
    : ExperimentalSite(tag, setup),
    Shadow(theChannel, *theObjectBroker), dataSize(datasize),
    sendV(OF_Network_dataSize), recvV(OF_Network_dataSize),
    bDisp(0), bVel(0), bAccel(0), bForce(0), bTime(0),
    rDisp(0), rVel(0), rAccel(0), rForce(0), rTime(0)
{
    if (theSetup == 0)  {
        opserr << "ShadowExpSite::ShadowExpSite() - "
            << "if you want to use it without an ExperimentalSetup, "
            << "use another constructor.\n";
        exit(OF_ReturnType_failed);
    }
    
    sendV(0) = OF_RemoteTest_open;
    sendV(1) = tag;
    sendV(2) = atof(OPF_VERSION);
    this->sendVector(sendV);
    this->recvVector(recvV);
    
    if (recvV(2) != atof(OPF_VERSION))  {
        opserr << "ShadowExpSite::ShadowExpSite() - OpenFresco Version "
            << "mismatch:\nShadowExpSite Version " << atof(OPF_VERSION)
            << " != ActorExpSite Version " << recvV(2) << endln;
        exit(OF_ReturnType_failed);
    }
    
    opserr << "\nConnected to ActorExpSite "
        << recvV(1) << endln;
}


ShadowExpSite::ShadowExpSite(const ShadowExpSite& es)
    : ExperimentalSite(es), Shadow(es), dataSize(0),
    sendV(OF_Network_dataSize), recvV(OF_Network_dataSize),
    bDisp(0), bVel(0), bAccel(0), bForce(0), bTime(0),
    rDisp(0), rVel(0), rAccel(0), rForce(0), rTime(0)
{
    dataSize = es.dataSize;
    sendV.resize(dataSize);
    recvV.resize(dataSize);
}


ShadowExpSite::~ShadowExpSite()
{
    if (bDisp != 0)
        delete bDisp;
    if (bVel != 0)
        delete bVel;
    if (bAccel != 0)
        delete bAccel;
    if (bForce != 0)
        delete bForce;
    if (bTime != 0)
        delete bTime;
    if (rDisp != 0)
        delete rDisp;
    if (rVel != 0)
        delete rVel;
    if (rAccel != 0)
        delete rAccel;
    if (rForce != 0)
        delete rForce;
    if (rTime != 0)
        delete rTime;
    
    sendV(0) = OF_RemoteTest_DIE;
    sendV(1) = this->getTag();
    this->sendVector(sendV);
    this->recvVector(recvV);
    
    opserr << "\nDisconnected from ActorExpSite "
        << recvV(1) << endln << endln;
}


int ShadowExpSite::setup()
{    
    sendV(0) = OF_RemoteTest_setup;
    sendV(1) = dataSize;
    this->sendVector(sendV);
    
    if (theSetup != 0)  {
        // send sizeCtrl
        this->sendID(theSetup->getCtrlSize());
        // send sizeDaq
        this->sendID(theSetup->getDaqSize());
    } else  {
        // send sizeTrial
        this->sendID(*sizeTrial);
        // send sizeOut
        this->sendID(*sizeOut);
    }
    
    return OF_ReturnType_completed;
}


int ShadowExpSite::setSize(ID sizeT, ID sizeO) 
{
    // call the base class method
    this->ExperimentalSite::setSize(sizeT, sizeO);
    
    int usrDataSize = dataSize;

    if (theSetup != 0)  {
        theSetup->checkSize(sizeT, sizeO);
        int nCtrl = 0, nDaq = 0;
        for (int i=0; i<OF_Resp_All; i++)  {
            nCtrl += getCtrlSize(i);
            nDaq  += getDaqSize(i);
        }
        if (dataSize < 1+nCtrl) dataSize = 1+nCtrl;
        if (dataSize < nDaq)    dataSize = nDaq;
    } else  {
        int nInput = 0, nOutput = 0;
        for (int i=0; i<OF_Resp_All; i++)  {
            nInput  += sizeT(i);
            nOutput += sizeO(i);
        }
        if (dataSize < 1+nInput) dataSize = 1+nInput;
        if (dataSize < nOutput)  dataSize = nOutput;
    }
    
    // warning message if user-provided dataSize was too small
    if (dataSize > usrDataSize)  {
        opserr << "\nWARNING ShadowExpSite::setSize() - "
            << "dataSize increased to " << dataSize << ".\n";
    }
    
    // send experimental setup to ActorExpSite
    this->setup();
    
    // resize channel Vectors
    sendV.resize(dataSize);
    recvV.resize(dataSize);
    
    return OF_ReturnType_completed;
}


int ShadowExpSite::setTrialResponse(const Vector* disp, 
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
    if (theSetup != 0)  {
        // transform trial response
        rValue = theSetup->transfTrialResponse(tDisp, tVel, tAccel, tForce, tTime);
        if (rValue != OF_ReturnType_completed)  {
            opserr << "ShadowExpSite::setTrialResponse() - "
                << "failed to set trial response at the setup.\n";
            exit(OF_ReturnType_failed);
        }
        
        if (bDisp == 0)  {
            if (getCtrlSize(OF_Resp_Disp) != 0)
                bDisp = new Vector(getCtrlSize(OF_Resp_Disp));
            if (getCtrlSize(OF_Resp_Vel) != 0)
                bVel = new Vector(getCtrlSize(OF_Resp_Vel));
            if (getCtrlSize(OF_Resp_Accel) != 0)
                bAccel = new Vector(getCtrlSize(OF_Resp_Accel));
            if (getCtrlSize(OF_Resp_Force) != 0)
                bForce = new Vector(getCtrlSize(OF_Resp_Force));
            if (getCtrlSize(OF_Resp_Time) != 0)
                bTime = new Vector(getCtrlSize(OF_Resp_Time));
        }
        
        // get trial response from the setup
        rValue = theSetup->getTrialResponse(bDisp, bVel, bAccel, bForce, bTime);
        if (rValue != OF_ReturnType_completed)  {
            opserr << "ShadowExpSite::setTrialResponse() - "
                << "failed to get trial response from the setup.\n";
            exit(OF_ReturnType_failed);
        }
        
        int ndim = 1, size;
        sendV.Zero();
        size = getCtrlSize(OF_Resp_Disp);
        if (size != 0)  {
            sendV.Assemble(*bDisp, ndim);
            ndim += size;
        }
        size = getCtrlSize(OF_Resp_Vel);
        if (size != 0)  {
            sendV.Assemble(*bVel, ndim);
            ndim += size;
        }
        size = getCtrlSize(OF_Resp_Accel);
        if (size != 0)  {
            sendV.Assemble(*bAccel, ndim);
            ndim += size;
        }
        size = getCtrlSize(OF_Resp_Force);
        if (size != 0)  {
            sendV.Assemble(*bForce, ndim);
            ndim += size;
        }
        size = getCtrlSize(OF_Resp_Time);
        if (size != 0)  {
            sendV.Assemble(*bTime, ndim);
        }
    } else  {
        int ndim = 1, size;
        sendV.Zero();
        size = getTrialSize(OF_Resp_Disp);
        if (size != 0)  {
            sendV.Assemble(*tDisp, ndim);
            ndim += size;
        }
        size = getTrialSize(OF_Resp_Vel);
        if (size != 0)  {
            sendV.Assemble(*tVel, ndim);
            ndim += size;
        }
        size = getTrialSize(OF_Resp_Accel);
        if (size != 0)  {
            sendV.Assemble(*tAccel, ndim);
            ndim += size;
        }
        size = getTrialSize(OF_Resp_Force);
        if (size != 0)  {
            sendV.Assemble(*tForce, ndim);
            ndim += size;
        }
        size = getTrialSize(OF_Resp_Time);
        if (size != 0)  {
            sendV.Assemble(*tTime, ndim);
        }
    }
    
    // set trial response
    sendV(0) = OF_RemoteTest_setTrialResponse;
    this->sendVector(sendV);
    
    return OF_ReturnType_completed;
}


int ShadowExpSite::checkDaqResponse()
{
    if (daqFlag == false)  {
        sendV(0) = OF_RemoteTest_getDaqResponse;
        this->sendVector(sendV);
        
        if (rDisp == 0)  {
            if (getDaqSize(OF_Resp_Disp) != 0)
                rDisp = new Vector(getDaqSize(OF_Resp_Disp));
            if (getDaqSize(OF_Resp_Vel) != 0)
                rVel = new Vector(getDaqSize(OF_Resp_Vel));
            if (getDaqSize(OF_Resp_Accel) != 0)
                rAccel = new Vector(getDaqSize(OF_Resp_Accel));
            if (getDaqSize(OF_Resp_Force) != 0)
                rForce = new Vector(getDaqSize(OF_Resp_Force));
            if (getDaqSize(OF_Resp_Time) != 0)
                rTime = new Vector(getDaqSize(OF_Resp_Time));
        }
        this->recvVector(recvV);
        
        int ndim = 0;
        if (rDisp != 0)  {
            rDisp->Extract(recvV, 0);
            ndim += getDaqSize(OF_Resp_Disp);
        }
        if (rVel != 0)  {
            rVel->Extract(recvV, ndim);
            ndim += getDaqSize(OF_Resp_Vel);
        }
        if (rAccel != 0)  {
            rAccel->Extract(recvV, ndim);
            ndim += getDaqSize(OF_Resp_Accel);
        }
        if (rForce != 0)  {
            rForce->Extract(recvV, ndim);
            ndim += getDaqSize(OF_Resp_Force);
        }
        if (rTime != 0)  {
            rTime->Extract(recvV, ndim);
        }
        
        if (theSetup != 0)  {
            // set daq response at the setup
            theSetup->setDaqResponse(rDisp, rVel, rAccel, rForce, rTime);
            // transform daq response
            theSetup->transfDaqResponse(oDisp, oVel, oAccel, oForce, oTime);
        } else  {
            if (oDisp != 0) 
                *oDisp = *rDisp;
            if (oVel != 0) 
                *oVel = *rVel;
            if (oAccel != 0) 
                *oAccel = *rAccel;
            if (oForce != 0) 
                *oForce = *rForce;
            if (oTime != 0) 
                *oTime = *rTime;
        }
        
        // save data in basic sys
        this->ExperimentalSite::setDaqResponse(oDisp, oVel, oAccel, oForce, oTime); 
        
        // set daq flag
        daqFlag = true;
    }
    
    return OF_ReturnType_completed;
}


int ShadowExpSite::commitState(Vector* time)
{
    int rValue = 0;
    
    // update the trial time vector
    if (time != 0 && tTime != 0)  {
        *tTime = *time;
        int ndim = 1
            + getTrialSize(OF_Resp_Disp)
            + getTrialSize(OF_Resp_Vel)
            + getTrialSize(OF_Resp_Accel)
            + getTrialSize(OF_Resp_Force);
        sendV.Assemble(*tTime, ndim);
    }
    
    // first commit the ActorExpSite
    sendV(0) = OF_RemoteTest_commitState;
    this->sendVector(sendV);
    
    // then commit base class
    rValue += this->ExperimentalSite::commitState();
    
    return rValue;
}


ExperimentalSite* ShadowExpSite::getCopy()
{
    ShadowExpSite *theCopy = new ShadowExpSite(*this);
    
    return theCopy;
}


void ShadowExpSite::Print(OPS_Stream &s, int flag)
{
    s << "ExperimentalSite: " << this->getTag(); 
    s << " type: ShadowExpSite\n";
    if (theSetup != 0)  {
        s << "\tExperimentalSetup tag: " << theSetup->getTag() << endln;
        s << *theSetup;
    }
}
