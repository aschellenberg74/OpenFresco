/* ****************************************************************** **
**    OpenFRESCO - Open Framework                                     **
**                 for Experimental Setup and Control                 **
**                                                                    **
**                                                                    **
** Copyright (c) 2006, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited. See    **
** file 'COPYRIGHT_UCB' in main directory for information on usage    **
** and redistribution, and for a DISCLAIMER OF ALL WARRANTIES.        **
**                                                                    **
** Developed by:                                                      **
**   Andreas Schellenberg (andreas.schellenberg@gmx.net)              **
**   Yoshikazu Takahashi (yos@catfish.dpri.kyoto-u.ac.jp)             **
**   Gregory L. Fenves (fenves@berkeley.edu)                          **
**   Stephen A. Mahin (mahin@berkeley.edu)                            **
**                                                                    **
** ****************************************************************** */

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 01/07
// Revision: A
//
// Description: This file contains the implementation of the
// ECLabVIEW class.

#include "ECLabVIEW.h"
#include <ExperimentalCP.h>

#include <Message.h>
#include <TCP_Socket.h>
#include <elementAPI.h>


void* OPF_ECLabVIEW()
{
    // pointer to experimental control that will be returned
    ExperimentalControl* theControl = 0;
    
    if (OPS_GetNumRemainingInputArgs() < 7) {
        opserr << "WARNING invalid number of arguments\n";
        opserr << "Want: expControl LabVIEW tag ipAddr ipPort -trialCP cpTags -outCP cpTags "
            << "<-ctrlFilters (5 filterTag)> <-daqFilters (5 filterTag)>\n";
        return 0;
    }
    
    // control tag
    int tag;
    int numdata = 1;
    if (OPS_GetIntInput(&numdata, &tag) != 0) {
        opserr << "WARNING invalid expControl LabVIEW tag\n";
        return 0;
    }
    
    // ip address
    char* ipAddr;
    const char* type = OPS_GetString();
    ipAddr = new char[strlen(type) + 1];
    strcpy(ipAddr, type);
    
    // ip port
    int ipPort;
    numdata = 1;
    if (OPS_GetIntInput(&numdata, &ipPort) < 0) {
        opserr << "WARNING invalid ipPort\n";
        opserr << "expControl LabVIEW " << tag << endln;
        return 0;
    }
    
    // trialCPs
    type = OPS_GetString();
    if (strcmp(type, "-trialCP") != 0) {
        opserr << "WARNING expecting -trialCP cpTags\n";
        opserr << "expControl LabVIEW " << tag << endln;
        return 0;
    }
    ID cpTags(32);
    int numTrialCPs = 0;
    while (OPS_GetNumRemainingInputArgs() > 0) {
        int cpTag;
        numdata = 1;
        int numArgs = OPS_GetNumRemainingInputArgs();
        if (OPS_GetIntInput(&numdata, &cpTag) < 0) {
            if (numArgs > OPS_GetNumRemainingInputArgs()) {
                // move current arg back by one
                OPS_ResetCurrentInputArg(-1);
            }
            break;
        }
        cpTags(numTrialCPs++) = cpTag;
    }
    if (numTrialCPs == 0) {
        opserr << "WARNING no trialCPTags specified\n";
        opserr << "expControl LabVIEW " << tag << endln;
        return 0;
    }
    cpTags.resize(numTrialCPs);
    // create the array to hold the trial control points
    ExperimentalCP** trialCPs = new ExperimentalCP * [numTrialCPs];
    if (trialCPs == 0) {
        opserr << "WARNING out of memory\n";
        opserr << "expControl LabVIEW " << tag << endln;
        return 0;
    }
    // populate array with trial control points
    for (int i = 0; i < numTrialCPs; i++) {
        trialCPs[i] = 0;
        trialCPs[i] = OPF_getExperimentalCP(cpTags(i));
        if (trialCPs[i] == 0) {
            opserr << "WARNING experimental control point not found\n";
            opserr << "expControlPoint " << cpTags(i) << endln;
            opserr << "expControl LabVIEW " << tag << endln;
            return 0;
        }
    }
    
    // outCPs
    type = OPS_GetString();
    if (strcmp(type, "-outCP") != 0) {
        opserr << "WARNING expecting -outCP cpTags\n";
        opserr << "expControl LabVIEW " << tag << endln;
        return 0;
    }
    cpTags.resize(32); cpTags.Zero();
    int numOutCPs = 0;
    while (OPS_GetNumRemainingInputArgs() > 0) {
        int cpTag;
        numdata = 1;
        int numArgs = OPS_GetNumRemainingInputArgs();
        if (OPS_GetIntInput(&numdata, &cpTag) < 0) {
            if (numArgs > OPS_GetNumRemainingInputArgs()) {
                // move current arg back by one
                OPS_ResetCurrentInputArg(-1);
            }
            break;
        }
        cpTags(numOutCPs++) = cpTag;
    }
    if (numOutCPs == 0) {
        opserr << "WARNING no outCPTags specified\n";
        opserr << "expControl LabVIEW " << tag << endln;
        return 0;
    }
    cpTags.resize(numOutCPs);
    // create the array to hold the output control points
    ExperimentalCP** outCPs = new ExperimentalCP * [numOutCPs];
    if (outCPs == 0) {
        opserr << "WARNING out of memory\n";
        opserr << "expControl LabVIEW " << tag << endln;
        return 0;
    }
    // populate array with output control points
    for (int i = 0; i < numOutCPs; i++) {
        outCPs[i] = 0;
        outCPs[i] = OPF_getExperimentalCP(cpTags(i));
        if (outCPs[i] == 0) {
            opserr << "WARNING experimental control point not found\n";
            opserr << "expControlPoint " << cpTags(i) << endln;
            opserr << "expControl LabVIEW " << tag << endln;
            return 0;
        }
    }
    
    // parsing was successful, allocate the control
    theControl = new ECLabVIEW(tag, numTrialCPs, trialCPs,
        numOutCPs, outCPs, ipAddr, ipPort);
    if (theControl == 0) {
        opserr << "WARNING could not create experimental control of type ECLabVIEW\n";
        return 0;
    }
    
    // cleanup dynamic memory
    //if (trialCPs != 0)
    //    delete[] trialCPs;
    //if (outCPs != 0)
    //    delete[] outCPs;
    
    return theControl;
}


ECLabVIEW::ECLabVIEW(int tag, int nTrialCPs, ExperimentalCP** trialcps,
    int nOutCPs, ExperimentalCP** outcps, char* ipaddress, int ipport)
    : ExperimentalControl(tag),
    numTrialCPs(nTrialCPs), trialCPs(0), numOutCPs(nOutCPs), outCPs(0),
    ipAddress(ipaddress), ipPort(ipport),
    theSocket(0), sData(0), sendData(0), rData(0), recvData(0),
    numCtrlSignals(0), numDaqSignals(0),
    ctrlDisp(0), ctrlForce(0), daqDisp(0), daqForce(0),
    trialSigOffset(0), ctrlSigOffset(0), daqSigOffset(0),
    gotRelativeTrial(1)
{
    // open log file
    logFile = fopen("ECLabVIEW.log", "w");
    if (logFile == 0) {
        opserr << "ECLabVIEW::ECLabVIEW() - "
            << "fopen: could not open log file.\n";
        exit(OF_ReturnType_failed);
    }
    
    // get trial and output control points
    if (trialcps == 0 || outcps == 0) {
        opserr << "ECLabVIEW::ECLabVIEW() - "
            << "null trialCPs or outCPs array passed.\n";
        exit(OF_ReturnType_failed);
    }
    trialCPs = trialcps;
    outCPs = outcps;
    
    // get total number of control and daq signals and
    // check if any signals use relative trial reference
    for (int i = 0; i < numTrialCPs; i++) {
        int numSignals = trialCPs[i]->getNumSignal();
        numCtrlSignals += numSignals;
        
        int j = 0;
        ID isRel = trialCPs[i]->getTrialSigRefType();
        while (gotRelativeTrial && j < numSignals)
            gotRelativeTrial = !isRel(j++);
    }
    for (int i = 0; i < numOutCPs; i++) {
        int numSignals = outCPs[i]->getNumSignal();
        numDaqSignals += numSignals;
    }
    
    // setup the connection
    theSocket = new TCP_Socket(ipPort, ipAddress);
    if (theSocket->setUpConnection() != 0) {
        opserr << "ECLabVIEW::ECLabVIEW() - "
            << "failed to setup TCP connection to LabVIEW.\n";
        delete theSocket;
        exit(OF_ReturnType_failed);
    }
    
    opserr << "****************************************************************\n";
    opserr << "* The TCP/IP socket with address: " << ipAddress << endln;
    opserr << "* and port: " << ipPort << " has been opened\n";
    opserr << "****************************************************************\n";
    opserr << endln;
    
    // allocate memory for the send messages
    const int dataSize = 512;
    sData = new char[dataSize];
    sendData = new Message(sData, dataSize);
    
    // allocate memory for the receive messages
    rData = new char[dataSize];
    recvData = new Message(rData, dataSize);
    
    // open a session with LabVIEW
    sprintf(sData, "open-session\tOpenFresco\n");
    fprintf(logFile, "%s", sData);
    delete sendData;  sendData = new Message(sData, (int)strlen(sData));  // needed because of bug in LabVIEW-plugin
    theSocket->sendMsg(0, 0, *sendData, 0);
    theSocket->recvMsgUnknownSize(0, 0, *recvData, 0);
    fprintf(logFile, "%s", rData);
    
    if (strcmp(strtok(rData, "\t"), "OK") != 0) {
        opserr << "ECLabVIEW::ECLabVIEW() - "
            << "failed to open a session with LabVIEW.\n";
        opserr << rData << endln;
        delete theSocket;
        exit(OF_ReturnType_failed);
    }
    
    // send parameters (needed for NEES-SAM & MiniMost -> remove later)
    sprintf(sData, "set-parameter\tOPFTransaction\tnstep\t1\n");
    fprintf(logFile, "%s", sData);
    delete sendData;  sendData = new Message(sData, (int)strlen(sData));  // needed because of bug in LabVIEW-plugin
    theSocket->sendMsg(0, 0, *sendData, 0);
    theSocket->recvMsgUnknownSize(0, 0, *recvData, 0);
    fprintf(logFile, "%s", rData);
    
    if (strcmp(strtok(rData, "\t"), "OK") != 0) {
        opserr << "ECLabVIEW::ECLabVIEW() - "
            << "failed to set parameter with LabVIEW.\n";
        opserr << rData << endln;
        delete theSocket;
        exit(OF_ReturnType_failed);
    }
}


ECLabVIEW::ECLabVIEW(const ECLabVIEW& ec)
    : ExperimentalControl(ec),
    theSocket(0), sData(0), sendData(0), rData(0), recvData(0),
    numCtrlSignals(0), numDaqSignals(0),
    ctrlDisp(0), ctrlForce(0), daqDisp(0), daqForce(0),
    trialSigOffset(0), ctrlSigOffset(0), daqSigOffset(0),
    gotRelativeTrial(1)
{
    numTrialCPs = ec.numTrialCPs;
    trialCPs = ec.trialCPs;
    numOutCPs = ec.numOutCPs;
    outCPs = ec.outCPs;
    
    logFile = ec.logFile;
    
    // use the existing socket which is set up
    ipAddress = ec.ipAddress;
    ipPort = ec.ipPort;
    theSocket = ec.theSocket;
    
    // allocate memory for the send messages
    const int dataSize = 512;
    sData = new char[dataSize];
    sendData = new Message(sData, dataSize);
    
    // allocate memory for the receive messages
    rData = new char[dataSize];
    recvData = new Message(rData, dataSize);
    
    numCtrlSignals = ec.numCtrlSignals;
    numDaqSignals = ec.numDaqSignals;
    gotRelativeTrial = ec.gotRelativeTrial;
}


ECLabVIEW::~ECLabVIEW()
{
    // delete memory of ctrl vectors
    if (ctrlDisp != 0)
        delete ctrlDisp;
    if (ctrlForce != 0)
        delete ctrlForce;
    
    // delete memory of daq vectors
    if (daqDisp != 0)
        delete daqDisp;
    if (daqForce != 0)
        delete daqForce;
    
    // delete memory of string
    if (ipAddress != 0)
        delete[] ipAddress;
    
    // close the session with LabVIEW
    sprintf(sData, "close-session\tOpenFresco\n");
    fprintf(logFile, "%s", sData);
    delete sendData;  sendData = new Message(sData, (int)strlen(sData));  // needed because of bug in LabVIEW-plugin
    theSocket->sendMsg(0, 0, *sendData, 0);
    theSocket->recvMsgUnknownSize(0, 0, *recvData, 0);
    fprintf(logFile, "%s", rData);
    
    if (strcmp(strtok(rData, "\t"), "OK") != 0) {
        opserr << "ECLabVIEW::~ECLabVIEW() - "
            << "failed to close the current session with LabVIEW.\n";
        opserr << rData << endln;
    }
    
    // close connection by destroying theSocket
    if (sendData != 0)
        delete sendData;
    if (sData != 0)
        delete[] sData;
    if (recvData != 0)
        delete recvData;
    if (rData != 0)
        delete[] rData;
    if (theSocket != 0)
        delete theSocket;
    
    // close output file
    fclose(logFile);
    
    // control points are not copies, so do not clean them up here
    //int i;
    //if (trialCPs != 0) {
    //    for (i=0; i<numTrialCPs; i++) {
    //        if (trialCPs[i] != 0)
    //            delete trialCPs[i];
    //    }
    //    delete [] trialCPs;
    //}
    //if (outCPs != 0) {
    //    for (i=0; i<numOutCPs; i++) {
    //        if (outCPs[i] != 0)
    //            delete outCPs[i];
    //    }
    //    delete [] outCPs;
    //}
    
    opserr << endln;
    opserr << "********************************************\n";
    opserr << "* The session with LabVIEW has been closed *\n";
    opserr << "********************************************\n";
    opserr << endln;
}


int ECLabVIEW::setup()
{
    int rValue = 0;
    
    if (ctrlDisp != 0)
        delete ctrlDisp;
    if (ctrlForce != 0)
        delete ctrlForce;
    
    if ((*sizeCtrl)(OF_Resp_Disp) != 0) {
        ctrlDisp = new Vector((*sizeCtrl)(OF_Resp_Disp));
        ctrlDisp->Zero();
    }
    if ((*sizeCtrl)(OF_Resp_Force) != 0) {
        ctrlForce = new Vector((*sizeCtrl)(OF_Resp_Force));
        ctrlForce->Zero();
    }
    
    if (daqDisp != 0)
        delete daqDisp;
    if (daqForce != 0)
        delete daqForce;
    
    if ((*sizeDaq)(OF_Resp_Disp) != 0) {
        daqDisp = new Vector((*sizeDaq)(OF_Resp_Disp));
        daqDisp->Zero();
    }
    if ((*sizeDaq)(OF_Resp_Force) != 0) {
        daqForce = new Vector((*sizeDaq)(OF_Resp_Force));
        daqForce->Zero();
    }
    
    // resize signal offset vectors
    trialSigOffset.resize(numCtrlSignals);
    trialSigOffset.Zero();
    ctrlSigOffset.resize(numCtrlSignals);
    ctrlSigOffset.Zero();
    daqSigOffset.resize(numDaqSignals);
    daqSigOffset.Zero();
    
    // print experimental control information
    this->Print(opserr);
    
    opserr << "****************************************************************\n";
    opserr << "* Make sure that offset values of controller are set to ZERO   *\n";
    opserr << "*                                                              *\n";
    opserr << "* Press 'Enter' to proceed or 'c' to cancel the initialization *\n";
    opserr << "****************************************************************\n";
    opserr << endln;
    int c = getchar();
    if (c == 'c') {
        getchar();
        sprintf(sData, "close-session\tOpenFresco\n");
        fprintf(logFile, "%s", sData);
        delete sendData;  sendData = new Message(sData, (int)strlen(sData));  // needed because of bug in LabVIEW-plugin
        theSocket->sendMsg(0, 0, *sendData, 0);
        delete theSocket;
        exit(OF_ReturnType_failed);
    }
    
    do {
        rValue += this->control();
        rValue += this->acquire();
        
        int i;
        opserr << "****************************************************************\n";
        opserr << "* Initial values of DAQ are:\n";
        opserr << "*\n";
        opserr << "* dspDaq = [";
        for (i = 0; i < (*sizeDaq)(OF_Resp_Disp); i++)
            opserr << " " << (*daqDisp)(i);
        opserr << " ]\n";
        opserr << "* frcDaq = [";
        for (i = 0; i < (*sizeDaq)(OF_Resp_Force); i++)
            opserr << " " << (*daqForce)(i);
        opserr << " ]\n";
        opserr << "*\n";
        opserr << "* Press 'Enter' to start the test or\n";
        opserr << "* 'r' to repeat the measurement or\n";
        opserr << "* 'c' to cancel the initialization\n";
        opserr << "****************************************************************\n";
        opserr << endln;
        c = getchar();
        if (c == 'c') {
            getchar();
            sprintf(sData, "close-session\tOpenFresco\n");
            fprintf(logFile, "%s", sData);
            delete sendData;  sendData = new Message(sData, (int)strlen(sData));  // needed because of bug in LabVIEW-plugin
            theSocket->sendMsg(0, 0, *sendData, 0);
            delete theSocket;
            exit(OF_ReturnType_failed);
        }
        else if (c == 'r') {
            getchar();
        }
    } while (c == 'r');
    
    opserr << "*****************\n";
    opserr << "* Running...... *\n";
    opserr << "*****************\n";
    opserr << endln;
    
    return rValue;
}


int ECLabVIEW::setSize(ID sizeT, ID sizeO)
{
    // check sizeTrial and sizeOut
    // for ECLabVIEW object
    
    // ECLabVIEW objects can only use 
    // disp and force for trial and disp and force for output
    // check these are available in sizeT/sizeO.
    int sizeTDisp = 0, sizeTForce = 0;
    int sizeODisp = 0, sizeOForce = 0;
    for (int i = 0; i < numTrialCPs; i++) {
        sizeTDisp += (trialCPs[i]->getSizeRspType())(OF_Resp_Disp);
        sizeTForce += (trialCPs[i]->getSizeRspType())(OF_Resp_Force);
    }
    for (int i = 0; i < numOutCPs; i++) {
        sizeODisp += (outCPs[i]->getSizeRspType())(OF_Resp_Disp);
        sizeOForce += (outCPs[i]->getSizeRspType())(OF_Resp_Force);
    }
    if ((sizeTDisp != 0 && sizeTDisp != sizeT(OF_Resp_Disp)) ||
        (sizeTForce != 0 && sizeTForce != sizeT(OF_Resp_Force)) ||
        (sizeODisp != 0 && sizeODisp != sizeO(OF_Resp_Disp)) ||
        (sizeOForce != 0 && sizeOForce != sizeO(OF_Resp_Force))) {
        opserr << "ECLabVIEW::setSize() - wrong sizeTrial/Out\n";
        opserr << "see User Manual.\n";
        sprintf(sData, "close-session\tOpenFresco\n");
        delete sendData;  sendData = new Message(sData, (int)strlen(sData));  // needed because of bug in LabVIEW-plugin
        theSocket->sendMsg(0, 0, *sendData, 0);
        delete theSocket;
        exit(OF_ReturnType_failed);
    }
    // finally assign sizes
    (*sizeCtrl) = sizeT;
    (*sizeDaq) = sizeO;
    
    return OF_ReturnType_completed;
}


int ECLabVIEW::setTrialResponse(
    const Vector* disp,
    const Vector* vel,
    const Vector* accel,
    const Vector* force,
    const Vector* time)
{
    int i, rValue = 0;
    if (disp != 0) {
        *ctrlDisp = *disp;
        if (theCtrlFilters[OF_Resp_Disp] != 0) {
            for (i = 0; i < (*sizeCtrl)(OF_Resp_Disp); i++)
                (*ctrlDisp)(i) = theCtrlFilters[OF_Resp_Disp]->filtering((*ctrlDisp)(i));
        }
    }
    if (force != 0) {
        *ctrlForce = *force;
        if (theCtrlFilters[OF_Resp_Force] != 0) {
            for (i = 0; i < (*sizeCtrl)(OF_Resp_Force); i++)
                (*ctrlForce)(i) = theCtrlFilters[OF_Resp_Force]->filtering((*ctrlForce)(i));
        }
    }
    
    rValue = this->control();
    
    return rValue;
}


int ECLabVIEW::getDaqResponse(
    Vector* disp,
    Vector* vel,
    Vector* accel,
    Vector* force,
    Vector* time)
{
    this->acquire();
    
    int i;
    if (disp != 0) {
        if (theDaqFilters[OF_Resp_Disp] != 0) {
            for (i = 0; i < (*sizeDaq)(OF_Resp_Disp); i++)
                (*daqDisp)(i) = theDaqFilters[OF_Resp_Disp]->filtering((*daqDisp)(i));
        }
        *disp = *daqDisp;
    }
    if (force != 0) {
        if (theDaqFilters[OF_Resp_Force] != 0) {
            for (i = 0; i < (*sizeDaq)(OF_Resp_Force); i++)
                (*daqForce)(i) = theDaqFilters[OF_Resp_Force]->filtering((*daqForce)(i));
        }
        *force = *daqForce;
    }
    
    return OF_ReturnType_completed;
}


int ECLabVIEW::commitState()
{
    return OF_ReturnType_completed;
}


ExperimentalControl* ECLabVIEW::getCopy()
{
    return new ECLabVIEW(*this);
}


Response* ECLabVIEW::setResponse(const char** argv, int argc,
    OPS_Stream& output)
{
    int i;
    char outputData[15];
    Response* theResponse = 0;
    
    output.tag("ExpControlOutput");
    output.attr("ctrlType", this->getClassType());
    output.attr("ctrlTag", this->getTag());
    
    // ctrl displacements
    if (ctrlDisp != 0 && (
        strcmp(argv[0], "ctrlDisp") == 0 ||
        strcmp(argv[0], "ctrlDisplacement") == 0 ||
        strcmp(argv[0], "ctrlDisplacements") == 0))
    {
        for (i = 0; i < (*sizeCtrl)(OF_Resp_Disp); i++) {
            sprintf(outputData, "ctrlDisp%d", i + 1);
            output.tag("ResponseType", outputData);
        }
        theResponse = new ExpControlResponse(this, 1, *ctrlDisp);
    }
    
    // ctrl forces
    else if (ctrlForce != 0 && (
        strcmp(argv[0], "ctrlForce") == 0 ||
        strcmp(argv[0], "ctrlForces") == 0))
    {
        for (i = 0; i < (*sizeCtrl)(OF_Resp_Force); i++) {
            sprintf(outputData, "ctrlForce%d", i + 1);
            output.tag("ResponseType", outputData);
        }
        theResponse = new ExpControlResponse(this, 2, *ctrlForce);
    }
    
    // daq displacements
    else if (daqDisp != 0 && (
        strcmp(argv[0], "daqDisp") == 0 ||
        strcmp(argv[0], "daqDisplacement") == 0 ||
        strcmp(argv[0], "daqDisplacements") == 0))
    {
        for (i = 0; i < (*sizeDaq)(OF_Resp_Disp); i++) {
            sprintf(outputData, "daqDisp%d", i + 1);
            output.tag("ResponseType", outputData);
        }
        theResponse = new ExpControlResponse(this, 3, *daqDisp);
    }
    
    // daq forces
    else if (daqForce != 0 && (
        strcmp(argv[0], "daqForce") == 0 ||
        strcmp(argv[0], "daqForces") == 0))
    {
        for (i = 0; i < (*sizeDaq)(OF_Resp_Force); i++) {
            sprintf(outputData, "daqForce%d", i + 1);
            output.tag("ResponseType", outputData);
        }
        theResponse = new ExpControlResponse(this, 4, *daqForce);
    }
    
    output.endTag();
    
    return theResponse;
}


int ECLabVIEW::getResponse(int responseID, Information& info)
{
    switch (responseID) {
    case 1:  // ctrl displacements
        return info.setVector(*ctrlDisp);
    
    case 2:  // ctrl forces
        return info.setVector(*ctrlForce);
    
    case 3:  // daq displacements
        return info.setVector(*daqDisp);
    
    case 4:  // daq forces
        return info.setVector(*daqForce);
    
    default:
        return -1;
    }
}


void ECLabVIEW::Print(OPS_Stream& s, int flag)
{
    s << "****************************************************************\n";
    s << "* ExperimentalControl: " << this->getTag() << endln;
    s << "*   type: ECLabVIEW\n";
    s << "*   ipAddress: " << ipAddress << ", ipPort: " << ipPort << endln;
    s << "*   trialCP tag(s):";
    for (int i = 0; i < numTrialCPs; i++)
        s << " " << trialCPs[i]->getTag();
    s << "\n*   outCP tag(s):";
    for (int i = 0; i < numOutCPs; i++)
        s << " " << outCPs[i]->getTag();
    s << "\n*   ctrlFilter tags:";
    for (int i = 0; i < OF_Resp_All; i++) {
        if (theCtrlFilters[i] != 0)
            s << " " << theCtrlFilters[i]->getTag();
        else
            s << " 0";
    }
    s << "\n*   daqFilter tags:";
    for (int i = 0; i < OF_Resp_All; i++) {
        if (theDaqFilters[i] != 0)
            s << " " << theDaqFilters[i]->getTag();
        else
            s << " 0";
    }
    s << "\n****************************************************************\n\n";
}


int ECLabVIEW::control()
{
    // get current local time to setup transaction ID
    time(&rawtime);
    ptm = localtime(&rawtime);
    sprintf(OPFTransactionID, "OPFTransaction%4d%02d%02d%02d%02d%02d",
        1900 + ptm->tm_year, ptm->tm_mon, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    
    // propose ctrl values
    int dID = 0, fID = 0;
    sprintf(sData, "propose\t%s", OPFTransactionID);
    
    // loop through all the trial control points
    for (int i = 0; i < numTrialCPs; i++) {
        // append trial control point name
        if (i == 0)
            sprintf(sData, "%s\tMDL-00-01", sData);
        //sprintf(sData,"%s\tCPNode%02d",sData,trialCPs[i]->getNodeTag());
        else
            sprintf(sData, "%s\tcontrol-point\tCPNode%02d", sData, trialCPs[i]->getNodeTag());
        
        // get trial control point parameters
        int ndm = trialCPs[i]->getNodeNDM();
        int numSignals = trialCPs[i]->getNumSignal();
        ID dof = trialCPs[i]->getDOF();
        ID rsp = trialCPs[i]->getRspType();
        Vector fact = trialCPs[i]->getFactor();
        
        if (trialCPs[i]->hasLimits()) {
            // loop through all the trial control point parameters
            // and check if commands are within given limits
            double parameter; int c;
            for (int j = 0; j < numSignals; j++) {
                // append GeomType
                if (dof(j) == 0 || dof(j) == 3) {
                    sprintf(sData, "%s\tx", sData);
                }
                else if (dof(j) == 1 || dof(j) == 4) {
                    sprintf(sData, "%s\ty", sData);
                }
                else if (dof(j) == 2 || dof(j) == 5) {
                    sprintf(sData, "%s\tz", sData);
                }
                else {
                    opserr << "ECLabVIEW::control() - "
                        << "requested dof is not supported.\n";
                    exit(OF_ReturnType_failed);
                }
                // append ParameterType
                if (dof(j) < ndm && rsp(j) == OF_Resp_Disp) {
                    parameter = fact(j) * (*ctrlDisp)(dID);
                    sprintf(sData, "%s\tdisplacement", sData);
                    dID++;
                }
                else if (dof(j) < ndm && rsp(j) == OF_Resp_Force) {
                    parameter = fact(j) * (*ctrlForce)(fID);
                    sprintf(sData, "%s\tforce", sData);
                    fID++;
                }
                else if (dof(j) >= ndm && rsp(j) == OF_Resp_Disp) {
                    parameter = fact(j) * (*ctrlDisp)(dID);
                    sprintf(sData, "%s\trotation", sData);
                    dID++;
                }
                else if (dof(j) >= ndm && rsp(j) == OF_Resp_Force) {
                    parameter = fact(j) * (*ctrlForce)(fID);
                    sprintf(sData, "%s\tmoment", sData);
                    fID++;
                }
                else {
                    opserr << "ECLabVIEW::control() - "
                        << "requested response type is not supported.\n";
                    exit(OF_ReturnType_failed);
                }
                // check if parameter is within limits and append
                Vector lowerLim = trialCPs[i]->getLowerLimit();
                Vector upperLim = trialCPs[i]->getUpperLimit();
                if (parameter < lowerLim(j) || parameter > upperLim(j)) {
                    opserr << "****************************************************************\n";
                    opserr << "* WARNING - Control command exceeds the limits:\n";
                    opserr << "*\n";
                    opserr << "* Limits = [" << lowerLim(j) << "," << upperLim(j) << "]";
                    opserr << " -> Command = " << parameter << endln;
                    opserr << "*\n";
                    opserr << "* Press 'Enter' to continue the test or\n";
                    opserr << "* 's' to saturate the command at the limits or\n";
                    opserr << "* 'c' to cancel the test\n";
                    opserr << "****************************************************************\n";
                    opserr << endln;
                    c = getchar();
                    if (c == 'c') {
                        getchar();
                        sprintf(sData, "close-session\tOpenFresco\n");
                        fprintf(logFile, "%s", sData);
                        delete sendData;  sendData = new Message(sData, (int)strlen(sData));  // needed because of bug in LabVIEW-plugin
                        theSocket->sendMsg(0, 0, *sendData, 0);
                        delete theSocket;
                        exit(OF_ReturnType_failed);
                    }
                    else if (c == 's') {
                        getchar();
                        parameter = (parameter < lowerLim(j)) ? lowerLim(j) : upperLim(j);
                    }
                }
                sprintf(sData, "%s\t%.10E", sData, parameter);
            }
        }
        else {
            // loop through all the trial control point parameters
            // without checking if commands are within limits
            for (int j = 0; j < numSignals; j++) {
                // append GeomType
                if (dof(j) == 0 || dof(j) == 3) {
                    sprintf(sData, "%s\tx", sData);
                }
                else if (dof(j) == 1 || dof(j) == 4) {
                    sprintf(sData, "%s\ty", sData);
                }
                else if (dof(j) == 2 || dof(j) == 5) {
                    sprintf(sData, "%s\tz", sData);
                }
                else {
                    opserr << "ECLabVIEW::control() - "
                        << "requested dof is not supported.\n";
                    exit(OF_ReturnType_failed);
                }
                // append ParameterType and Parameter
                if (dof(j) < ndm && rsp(j) == OF_Resp_Disp) {
                    sprintf(sData, "%s\tdisplacement\t%.10E", sData, fact(j) * (*ctrlDisp)(dID));
                    dID++;
                }
                else if (dof(j) < ndm && rsp(j) == OF_Resp_Force) {
                    sprintf(sData, "%s\tforce\t%.10E", sData, fact(j) * (*ctrlForce)(fID));
                    fID++;
                }
                else if (dof(j) >= ndm && rsp(j) == OF_Resp_Disp) {
                    sprintf(sData, "%s\trotation\t%.10E", sData, fact(j) * (*ctrlDisp)(dID));
                    dID++;
                }
                else if (dof(j) >= ndm && rsp(j) == OF_Resp_Force) {
                    sprintf(sData, "%s\tmoment\t%.10E", sData, fact(j) * (*ctrlForce)(fID));
                    fID++;
                }
                else {
                    opserr << "ECLabVIEW::control() - "
                        << "requested response type is not supported.\n";
                    exit(OF_ReturnType_failed);
                }
            }
        }
    }
    sprintf(sData, "%s\n", sData);
    fprintf(logFile, "%s", sData);
    delete sendData;  sendData = new Message(sData, (int)strlen(sData));  // needed because of bug in LabVIEW-plugin
    theSocket->sendMsg(0, 0, *sendData, 0);
    theSocket->recvMsgUnknownSize(0, 0, *recvData, 0);
    fprintf(logFile, "%s", rData);
    if (strcmp(strtok(rData, "\t"), "OK") != 0) {
        opserr << "ECLabVIEW::control() - "
            << "proposed control values were not accepted.\n";
        opserr << rData << endln;
        exit(OF_ReturnType_failed);
    }
    
    // execute ctrl values
    sprintf(sData, "execute\t%s\n", OPFTransactionID);
    fprintf(logFile, "%s", sData);
    delete sendData;  sendData = new Message(sData, (int)strlen(sData));  // needed because of bug in LabVIEW-plugin
    theSocket->sendMsg(0, 0, *sendData, 0);
    theSocket->recvMsgUnknownSize(0, 0, *recvData, 0);
    fprintf(logFile, "%s", rData);
    if (strcmp(strtok(rData, "\t"), "OK") != 0) {
        opserr << "ECLabVIEW::control() - "
            << "failed to execute proposed control values.\n";
        opserr << rData << endln;
        exit(OF_ReturnType_failed);
    }
    
    return OF_ReturnType_completed;
}


int ECLabVIEW::acquire()
{
    char* tokenPtr;
    char cpName[17];
    char GeomType[2];
    char ParamType[13];
    double Parameter;
    
    // send acquisition request
    sprintf(sData, "get-control-point\t%s", OPFTransactionID);
    for (int i = 0; i < numOutCPs; i++) {
        // append output control point name
        sprintf(sData, "%s\tMDL-00-01", sData);
        //sprintf(sData,"%s\tCPNode%02d",sData,outCPs[i]->getNodeTag());
    }
    sprintf(sData, "%s\n", sData);
    fprintf(logFile, "%s", sData);
    delete sendData;  sendData = new Message(sData, (int)strlen(sData));  // needed because of bug in LabVIEW-plugin
    theSocket->sendMsg(0, 0, *sendData, 0);
    
    // receive output control point daq values
    int direction = 0, response = 0;
    int dID = 0, fID = 0;
    for (int i = 0; i < numOutCPs; i++) {
        // disaggregate received data
        theSocket->recvMsgUnknownSize(0, 0, *recvData, 0);
        fprintf(logFile, "%s", rData);
        if (strcmp(strtok(rData, "\t"), "OK") != 0) {
            opserr << "ECLabVIEW::acquire() - "
                << "failed to acquire control-point "
                << outCPs[i]->getTag() << " values.\n";
            opserr << rData << endln;
            exit(OF_ReturnType_failed);
        }
        tokenPtr = strtok(NULL, "\t");
        tokenPtr = strtok(NULL, "\t");
        if (strcmp(tokenPtr, OPFTransactionID) != 0) {
            opserr << "ECLabVIEW::acquire() - "
                << "received wrong OPFTransactionID\n"
                << " want: " << OPFTransactionID
                << " but got: " << tokenPtr << endln;
            exit(OF_ReturnType_failed);
        }
        tokenPtr = strtok(NULL, "\t");
        sprintf(cpName, "MDL-00-01");
        //sprintf(cpName,"CPNode%02d",outCPs[i]->getNodeTag());
        if (strcmp(tokenPtr, cpName) != 0) {
            opserr << "ECLabVIEW::acquire() - "
                << "received wrong control-point\n"
                << " want: " << cpName
                << " but got: " << tokenPtr << endln;
            exit(OF_ReturnType_failed);
        }
        tokenPtr = strtok(NULL, "\t");
        
        // get output control point parameters
        int ndf = trialCPs[i]->getNodeNDF();
        int numSignals = outCPs[i]->getNumSignal();
        ID dof = outCPs[i]->getDOF();
        ID rsp = outCPs[i]->getRspType();
        Vector fact = outCPs[i]->getFactor();
        ID sizeRespType = outCPs[i]->getSizeRspType();
        int sizeDisp = 0, sizeForce = 0;
        
        while (tokenPtr != NULL) {
            strcpy(GeomType, tokenPtr);
            strcpy(ParamType, strtok(NULL, "\t"));
            Parameter = atof(strtok(NULL, "\t"));
            if (strcmp(GeomType, "x") == 0) {
                if (strcmp(ParamType, "displacement") == 0) {
                    direction = 0;
                    response = OF_Resp_Disp;
                }
                else if (strcmp(ParamType, "force") == 0) {
                    direction = 0;
                    response = OF_Resp_Force;
                }
                else if (strcmp(ParamType, "rotation") == 0) {
                    direction = 3;
                    response = OF_Resp_Disp;
                }
                else if (strcmp(ParamType, "moment") == 0) {
                    direction = 3;
                    response = OF_Resp_Force;
                }
            }
            else if (strcmp(GeomType, "y") == 0) {
                if (strcmp(ParamType, "displacement") == 0) {
                    direction = 1;
                    response = OF_Resp_Disp;
                }
                else if (strcmp(ParamType, "force") == 0) {
                    direction = 1;
                    response = OF_Resp_Force;
                }
                else if (strcmp(ParamType, "rotation") == 0) {
                    direction = 4;
                    response = OF_Resp_Disp;
                }
                else if (strcmp(ParamType, "moment") == 0) {
                    direction = 4;
                    response = OF_Resp_Force;
                }
            }
            else if (strcmp(GeomType, "z") == 0) {
                if (strcmp(ParamType, "displacement") == 0) {
                    direction = 2;
                    response = OF_Resp_Disp;
                }
                else if (strcmp(ParamType, "force") == 0) {
                    direction = 2;
                    response = OF_Resp_Force;
                }
                else if (strcmp(ParamType, "rotation") == 0) {
                    direction = ndf - 1;
                    response = OF_Resp_Disp;
                }
                else if (strcmp(ParamType, "moment") == 0) {
                    direction = ndf - 1;
                    response = OF_Resp_Force;
                }
            }
            if (direction > ndf) {
                opserr << "ECLabVIEW::acquire() - "
                    << "received wrong direction\n"
                    << " direction <= " << ndf
                    << " but got: " << direction + 1 << endln;
                exit(OF_ReturnType_failed);
            }
            
            // assemble displacement and force daq vectors
            if (response == OF_Resp_Disp) {
                int id = dID;
                for (int j = 0; j < numSignals; j++) {
                    if (rsp(j) == response) {
                        if (dof(j) == direction) {
                            (*daqDisp)(id) = fact(j) * Parameter;
                            sizeDisp++;
                        }
                        id++;
                    }
                }
            }
            else if (response == OF_Resp_Force) {
                int id = fID;
                for (int j = 0; j < numSignals; j++) {
                    if (rsp(j) == response) {
                        if (dof(j) == direction) {
                            (*daqForce)(id) = fact(j) * Parameter;
                            sizeForce++;
                        }
                        id++;
                    }
                }
            }
            tokenPtr = strtok(NULL, "\t");
        }
        
        // check if received number of parameters is correct
        if (sizeDisp != sizeRespType(0) || sizeForce != sizeRespType(3)) {
            opserr << "ECLabVIEW::acquire() - "
                << "received wrong number of displacement or force parameters\n"
                << " want: " << sizeRespType(0) << " displacements"
                << "and " << sizeRespType(3) << " forces.\n";
            exit(OF_ReturnType_failed);
        }
        dID += sizeRespType(0);
        fID += sizeRespType(3);
    }
    
    return OF_ReturnType_completed;
}
