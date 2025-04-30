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
// Created: 09/24
// Revision: A
//
// Description: This file contains the implementation of the ECSpeedgoat class.

#include "ECSpeedgoat.h"
#include <ExperimentalCP.h>

#include <elementAPI.h>

#include <Channel.h>
#include <TCP_Socket.h>
#include <UDP_Socket.h>


void* OPF_ECSpeedgoat()
{
    // pointer to experimental control that will be returned
    ExperimentalControl* theControl = 0;
    
    if (OPS_GetNumRemainingInputArgs() < 7) {
        opserr << "WARNING invalid number of arguments\n";
        opserr << "Want: expControl Speedgoat tag ipAddr ipPort <-udp> "
            << "-trialCP cpTags -outCP cpTags "
            << "<-ctrlFilters (5 filterTag)> <-daqFilters (5 filterTag)>\n";
        return 0;
    }
    
    // control tag
    int tag;
    int numdata = 1;
    if (OPS_GetIntInput(&numdata, &tag) != 0) {
        opserr << "WARNING invalid expControl Speedgoat tag\n";
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
        opserr << "expControl Speedgoat " << tag << endln;
        return 0;
    }
    
    // optional parameters and control points
    int udp = 0;
    int numTrialCPs = 0, numOutCPs = 0;
    ID cpTags(32);
    ExperimentalCP** trialCPs = 0;
    ExperimentalCP** outCPs = 0;
    while (OPS_GetNumRemainingInputArgs() > 0) {
        type = OPS_GetString();
        if (strcmp(type, "-udp") == 0) {
            udp = 1;
        }
        else if (strcmp(type, "-trialCP") == 0) {
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
                opserr << "expControl Speedgoat " << tag << endln;
                return 0;
            }
            cpTags.resize(numTrialCPs);
            // create the array to hold the trial control points
            trialCPs = new ExperimentalCP * [numTrialCPs];
            if (trialCPs == 0) {
                opserr << "WARNING out of memory\n";
                opserr << "expControl Speedgoat " << tag << endln;
                return 0;
            }
            // populate array with trial control points
            for (int i = 0; i < numTrialCPs; i++) {
                trialCPs[i] = 0;
                trialCPs[i] = OPF_getExperimentalCP(cpTags(i));
                if (trialCPs[i] == 0) {
                    opserr << "WARNING experimental control point not found\n";
                    opserr << "expControlPoint " << cpTags(i) << endln;
                    opserr << "expControl Speedgoat " << tag << endln;
                    return 0;
                }
            }
        }
        else if (strcmp(type, "-outCP") == 0) {
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
                opserr << "expControl Speedgoat " << tag << endln;
                return 0;
            }
            cpTags.resize(numOutCPs);
            // create the array to hold the output control points
            outCPs = new ExperimentalCP * [numOutCPs];
            if (outCPs == 0) {
                opserr << "WARNING out of memory\n";
                opserr << "expControl Speedgoat " << tag << endln;
                return 0;
            }
            // populate array with output control points
            for (int i = 0; i < numOutCPs; i++) {
                outCPs[i] = 0;
                outCPs[i] = OPF_getExperimentalCP(cpTags(i));
                if (outCPs[i] == 0) {
                    opserr << "WARNING experimental control point not found\n";
                    opserr << "expControlPoint " << cpTags(i) << endln;
                    opserr << "expControl Speedgoat " << tag << endln;
                    return 0;
                }
            }
        }
    }
    
    // parsing was successful, allocate the control
    theControl = new ECSpeedgoat(tag, numTrialCPs, trialCPs,
        numOutCPs, outCPs, ipAddr, ipPort, udp);
    if (theControl == 0) {
        opserr << "WARNING could not create experimental control of type Speedgoat\n";
        return 0;
    }
    
    // cleanup dynamic memory
    //if (trialCPs != 0)
    //    delete[] trialCPs;
    //if (outCPs != 0)
    //    delete[] outCPs;
    
    return theControl;
}


ECSpeedgoat::ECSpeedgoat(int tag, int nTrialCPs, ExperimentalCP** trialcps,
    int nOutCPs, ExperimentalCP** outcps, char* ipaddress, int ipport, int _udp)
    : ExperimentalControl(tag),
    numTrialCPs(nTrialCPs), trialCPs(0), numOutCPs(nOutCPs), outCPs(0),
    ipAddress(ipaddress), ipPort(ipport), udp(_udp),
    dataSize(OF_Network_dataSize), theChannel(0),
    sData(0), sendData(0), rData(0), recvData(0),
    newTarget(0), switchPC(0), atTarget(0),
    numCtrlSignals(0), numDaqSignals(0), ctrlSignal(0), daqSignal(0),
    trialSigOffset(0), ctrlSigOffset(0), daqSigOffset(0),
    gotRelativeTrial(0)
{
    // get trial and output control points
    if (trialcps == 0 || outcps == 0) {
        opserr << "ECSpeedgoat::ECSpeedgoat() - "
            << "null trialCPs or outCPs array passed.\n";
        exit(OF_ReturnType_failed);
    }
    trialCPs = trialcps;
    outCPs = outcps;
    
    // get total number of control and daq signals
    for (int i = 0; i < numTrialCPs; i++) {
        int numSignals = trialCPs[i]->getNumSignal();
        numCtrlSignals += numSignals;
    }
    for (int i = 0; i < numOutCPs; i++) {
        int numSignals = outCPs[i]->getNumSignal();
        numDaqSignals += numSignals;
    }
    
    opserr << "****************************************************************\n";
    opserr << "* Make sure that offset values of controller are set to ZERO,  *\n";
    opserr << "* then start the Simulink model on the Speedgoat target        *\n";
    opserr << "*                                                              *\n";
    opserr << "* Press 'Enter' to proceed or 'c' to cancel the initialization *\n";
    opserr << "****************************************************************\n";
    int c = getchar();
    if (c == 'c') {
        getchar();
        this->~ECSpeedgoat();
        exit(OF_ReturnType_failed);
    }
    
    // setup the connection to the Speedgoat target
    if (udp)
        theChannel = new UDP_Socket(ipPort, ipAddress, false);
    else
        theChannel = new TCP_Socket(ipPort, ipAddress);
    if (!theChannel) {
        opserr << "ECSpeedgoat::ECSpeedgoat() - "
            << "failed to create channel.\n";
        exit(OF_ReturnType_failed);
    }
    if (theChannel->setUpConnection() != 0) {
        opserr << "ECSpeedgoat::ECSpeedgoat() - "
            << "failed to setup connection to Speedgoat target.\n";
        delete theChannel;
        exit(OF_ReturnType_failed);
    }
    
    opserr << "****************************************************************\n";
    opserr << "* The channel to Speedgoat with address: " << ipAddress << endln;
    opserr << "* and port: " << ipPort << " has been opened\n";
    opserr << "****************************************************************\n";
    opserr << endln;
    
    // allocate memory for the send vectors
    sData = new double[dataSize];
    sendData = new Vector(sData, dataSize);
    sendData->Zero();
    
    // allocate memory for the receive vectors
    rData = new double[dataSize];
    recvData = new Vector(rData, dataSize);
    recvData->Zero();
}


ECSpeedgoat::ECSpeedgoat(const ECSpeedgoat& ec)
    : ExperimentalControl(ec),
    dataSize(OF_Network_dataSize), theChannel(0),
    sData(0), sendData(0), rData(0), recvData(0),
    newTarget(0), switchPC(0), atTarget(0),
    numCtrlSignals(0), numDaqSignals(0), ctrlSignal(0), daqSignal(0),
    trialSigOffset(0), ctrlSigOffset(0), daqSigOffset(0),
    gotRelativeTrial(0)
{
    numTrialCPs = ec.numTrialCPs;
    trialCPs = ec.trialCPs;
    numOutCPs = ec.numOutCPs;
    outCPs = ec.outCPs;
    
    // use the existing channel which is set up
    ipAddress = ec.ipAddress;
    ipPort = ec.ipPort;
    udp = ec.udp;
    theChannel = ec.theChannel;
    
    // allocate memory for the send vectors
    sData = new double[dataSize];
    sendData = new Vector(sData, dataSize);
    sendData->Zero();
    
    // allocate memory for the receive vectors
    rData = new double[dataSize];
    recvData = new Vector(rData, dataSize);
    recvData->Zero();
    
    newTarget = ec.newTarget;
    switchPC = ec.switchPC;
    atTarget = ec.atTarget;
    
    numCtrlSignals = ec.numCtrlSignals;
    numDaqSignals = ec.numDaqSignals;
    gotRelativeTrial = ec.gotRelativeTrial;
}


ECSpeedgoat::~ECSpeedgoat()
{
    // send termination to Speedgoat (twice for reliability)
    (*newTarget) = -1;
    theChannel->sendVector(0, 0, *sendData, 0);
    theChannel->sendVector(0, 0, *sendData, 0);

    // delete memory of signal arrays
    if (ctrlSignal != 0)
        delete ctrlSignal;
    if (daqSignal != 0)
        delete daqSignal;

    // delete memory of string
    if (ipAddress != 0)
        delete[] ipAddress;

    // close connection by destroying theChannel
    if (sendData != 0)
        delete sendData;
    if (sData != 0)
        delete[] sData;
    if (recvData != 0)
        delete recvData;
    if (rData != 0)
        delete[] rData;
    if (theChannel != 0)
        delete theChannel;

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

    opserr << "*************************************************\n";
    opserr << "* The connection with Speedgoat has been closed *\n";
    opserr << "*************************************************\n";
    opserr << endln;
}


int ECSpeedgoat::setup()
{
    int rValue = 0;
    
    // delete memory of signal arrays
    if (ctrlSignal != 0)
        delete ctrlSignal;
    if (daqSignal != 0)
        delete daqSignal;
    
    // create control signal array
    newTarget = &sData[0];
    ctrlSignal = new Vector(&sData[1], numCtrlSignals);
    sendData->Zero();
    
    // create daq signal array
    switchPC = &rData[0];
    atTarget = &rData[1];
    daqSignal = new Vector(&rData[2], numDaqSignals);
    recvData->Zero();
    
    // resize signal offset vectors
    trialSigOffset.resize(numCtrlSignals);
    trialSigOffset.Zero();
    ctrlSigOffset.resize(numCtrlSignals);
    ctrlSigOffset.Zero();
    daqSigOffset.resize(numDaqSignals);
    daqSigOffset.Zero();
    
    // print experimental control information
    this->Print(opserr, 1);
    
    int c;
    do {
        // get daq signal array from controller/daq
        rValue += this->acquire();
        
        // determine control signal offsets
        int kT = 0;
        for (int iT = 0; iT < numTrialCPs; iT++) {
            // get trial control point parameters
            int numSigT = trialCPs[iT]->getNumSignal();
            ID dofT = trialCPs[iT]->getDOF();
            ID rspT = trialCPs[iT]->getRspType();
            ID isRelCtrl = trialCPs[iT]->getCtrlSigRefType();
            // loop through all the trial control point signals
            for (int jT = 0; jT < numSigT; jT++) {
                if (isRelCtrl(jT)) {
                    // now search through ouput control points to
                    // find the signal with the same DOF and rspType
                    int kO = 0;
                    for (int iO = 0; iO < numOutCPs; iO++) {
                        int numSigO = outCPs[iO]->getNumSignal();
                        ID dofO = outCPs[iO]->getDOF();
                        ID rspO = outCPs[iO]->getRspType();
                        // loop through all the output control point signals
                        for (int jO = 0; jO < numSigO; jO++) {
                            if (dofT(jT) == dofO(jO) && rspT(jT) == rspO(jO)) {
                                ctrlSigOffset(kT) = (*daqSignal)(kO);
                                (*ctrlSignal)(kT) = ctrlSigOffset(kT);
                            }
                            kO++;
                        }
                    }
                }
                kT++;
            }
        }
        
        // send control signals with offsets to controller
        theChannel->sendVector(0, 0, *sendData, 0);
        theChannel->sendVector(0, 0, *sendData, 0);
        
        // determine daq signal offsets
        int kO = 0;
        for (int i = 0; i < numOutCPs; i++) {
            // get output control point parameters
            int numSignals = outCPs[i]->getNumSignal();
            ID isRelDaq = outCPs[i]->getDaqSigRefType();
            // loop through all the output control point signals
            for (int j = 0; j < numSignals; j++) {
                if (isRelDaq(j))
                    daqSigOffset(kO) = -(*daqSignal)(kO);
                kO++;
            }
        }
        
        opserr << "****************************************************************\n";
        opserr << "* Initial signal values of DAQ are:\n";
        for (int i = 0; i < numDaqSignals; i++)
            opserr << "*   daqSig" << i + 1 << " = " << (*daqSignal)(i) << endln;
        opserr << "*\n";
        if (daqSigOffset != 0.0) {
            for (int i = 0; i < numDaqSignals; i++)
                opserr << "*   daqSigOffset" << i + 1 << " = " << daqSigOffset(i) << endln;
            opserr << "*\n";
        }
        if (ctrlSigOffset != 0.0) {
            for (int i = 0; i < numCtrlSignals; i++)
                opserr << "*   ctrlSigOffset" << i + 1 << " = " << ctrlSigOffset(i) << endln;
            opserr << "*\n";
        }
        opserr << "* Press 'Enter' to start the test or\n";
        opserr << "* 'r' to repeat the measurement or\n";
        opserr << "* 'c' to cancel the initialization\n";
        opserr << "****************************************************************\n";
        c = getchar();
        if (c == 'c') {
            getchar();
            this->~ECSpeedgoat();
            exit(OF_ReturnType_failed);
        }
        else if (c == 'r') {
            getchar();
        }
    } while (c == 'r');
    
    // exit initialization state and start predictor-corrector
    rValue += this->control();
    
    opserr << "*****************\n";
    opserr << "* Running...... *\n";
    opserr << "*****************\n";
    opserr << endln;
    
    return rValue;
}


int ECSpeedgoat::setSize(ID sizeT, ID sizeO)
{
    // check sizeTrial and sizeOut against sizes
    // specified in the control points
    // ECSpeedgoat objects can use:
    //     disp, vel, accel, force and time for trial and
    //     disp, vel, accel, force and time for output
    
    // get maximum dof IDs for each trial response quantity
    ID maxdofT(OF_Resp_All);
    for (int i = 0; i < numTrialCPs; i++) {
        // get trial control point parameters
        int numSignals = trialCPs[i]->getNumSignal();
        ID dof = trialCPs[i]->getDOF();
        ID rsp = trialCPs[i]->getRspType();
        
        // loop through all the trial control point signals
        for (int j = 0; j < numSignals; j++) {
            dof(j)++;  // switch to 1-based indexing
            maxdofT(rsp(j)) = dof(j) > maxdofT(rsp(j)) ? dof(j) : maxdofT(rsp(j));
        }
    }
    // get maximum dof IDs for each output response quantity
    ID maxdofO(OF_Resp_All);
    for (int i = 0; i < numOutCPs; i++) {
        // get output control point parameters
        int numSignals = outCPs[i]->getNumSignal();
        ID dof = outCPs[i]->getDOF();
        ID rsp = outCPs[i]->getRspType();
        
        // loop through all the output control point signals
        for (int j = 0; j < numSignals; j++) {
            dof(j)++;  // switch to 1-based indexing
            maxdofO(rsp(j)) = dof(j) > maxdofO(rsp(j)) ? dof(j) : maxdofO(rsp(j));
        }
    }
    // now check if dof IDs are within limits
    for (int i = 0; i < OF_Resp_All; i++) {
        if ((maxdofT(i) != 0 && maxdofT(i) > sizeT(i)) ||
            (maxdofO(i) != 0 && maxdofO(i) > sizeO(i))) {
            opserr << "ECSpeedgoat::setSize() - wrong sizeTrial/Out\n";
            opserr << "see User Manual.\n";
            this->~ECSpeedgoat();
            exit(OF_ReturnType_failed);
        }
    }
    // finally assign sizes
    (*sizeCtrl) = maxdofT;
    (*sizeDaq) = maxdofO;
    
    return OF_ReturnType_completed;
}


int ECSpeedgoat::setTrialResponse(
    const Vector* disp,
    const Vector* vel,
    const Vector* accel,
    const Vector* force,
    const Vector* time)
{
    // loop through all the trial control points
    int k = 0;
    for (int i = 0; i < numTrialCPs; i++) {
        // get trial control point parameters
        int numSignals = trialCPs[i]->getNumSignal();
        ID dof = trialCPs[i]->getDOF();
        ID rsp = trialCPs[i]->getRspType();
        ID isRelTrial = trialCPs[i]->getTrialSigRefType();
        
        // loop through all the trial control point dofs
        for (int j = 0; j < numSignals; j++) {
            // assemble the control signal array
            if (rsp(j) == OF_Resp_Disp && disp != 0)
                (*ctrlSignal)(k) = (*disp)(dof(j));
            else if (rsp(j) == OF_Resp_Force && force != 0)
                (*ctrlSignal)(k) = (*force)(dof(j));
            else if (rsp(j) == OF_Resp_Time && time != 0)
                (*ctrlSignal)(k) = (*time)(dof(j));
            else if (rsp(j) == OF_Resp_Vel && vel != 0)
                (*ctrlSignal)(k) = (*vel)(dof(j));
            else if (rsp(j) == OF_Resp_Accel && accel != 0)
                (*ctrlSignal)(k) = (*accel)(dof(j));
            
            // get initial trial signal offset
            if (gotRelativeTrial == 0 && isRelTrial(j))
                trialSigOffset(k) = -(*ctrlSignal)(k);
            
            // apply trial signal offset if not zero
            if (trialSigOffset(k) != 0.0)
                (*ctrlSignal)(k) += trialSigOffset(k);
            
            // filter control signal if a filter exists
            if (theCtrlFilters[rsp(j)] != 0)
                (*ctrlSignal)(k) = theCtrlFilters[rsp(j)]->filtering((*ctrlSignal)(k));
            
            // apply control signal offset if not zero
            if (ctrlSigOffset(k) != 0.0)
                (*ctrlSignal)(k) += ctrlSigOffset(k);
            
            // increment counter
            k++;
        }
    }
    
    // print relative trial signal information once
    if (gotRelativeTrial == 0) {
        if (trialSigOffset != 0.0) {
            opserr << endln;
            opserr << "****************************************************************\n";
            opserr << "* Initial signal values of FEA are:\n";
            for (int i = 0; i < numCtrlSignals; i++) {
                if (trialSigOffset(i) != 0.0)
                    opserr << "*   trialSigOffset" << i + 1 << " = " << trialSigOffset(i) << endln;
            }
            opserr << "****************************************************************\n";
        }
        // now set flag that relative trial signals have been obtained
        gotRelativeTrial = 1;
    }
    
    // send control signal array to controller
    k += this->control();
    
    return (k - numCtrlSignals);
}


int ECSpeedgoat::getDaqResponse(
    Vector* disp,
    Vector* vel,
    Vector* accel,
    Vector* force,
    Vector* time)
{
    // get daq signal array from controller/daq
    int rValue = this->acquire();
    
    // loop through all the output control points
    int k = 0;
    for (int i = 0; i < numOutCPs; i++) {
        // get output control point parameters
        int numSignals = outCPs[i]->getNumSignal();
        ID dof = outCPs[i]->getDOF();
        ID rsp = outCPs[i]->getRspType();
        
        // loop through all the output control point dofs
        for (int j = 0; j < numSignals; j++) {
            // apply daq signal offset if not zero
            if (daqSigOffset(k) != 0)
                (*daqSignal)(k) += daqSigOffset(k);
            
            // filter daq signal if a filter exists
            if (theDaqFilters[rsp(j)] != 0)
                (*daqSignal)(k) = theDaqFilters[rsp(j)]->filtering((*daqSignal)(k));
            
            // populate the daq response vectors
            if (rsp(j) == OF_Resp_Disp && disp != 0)
                (*disp)(dof(j)) = (*daqSignal)(k);
            else if (rsp(j) == OF_Resp_Force && force != 0)
                (*force)(dof(j)) = (*daqSignal)(k);
            else if (rsp(j) == OF_Resp_Time && time != 0)
                (*time)(dof(j)) = (*daqSignal)(k);
            else if (rsp(j) == OF_Resp_Vel && vel != 0)
                (*vel)(dof(j)) = (*daqSignal)(k);
            else if (rsp(j) == OF_Resp_Accel && accel != 0)
                (*accel)(dof(j)) = (*daqSignal)(k);
            
            // increment counter
            k++;
        }
    }
    
    return (k + rValue - numDaqSignals);
}


int ECSpeedgoat::commitState()
{
    return OF_ReturnType_completed;
}


ExperimentalControl* ECSpeedgoat::getCopy()
{
    return new ECSpeedgoat(*this);
}


Response* ECSpeedgoat::setResponse(const char** argv, int argc,
    OPS_Stream& output)
{
    int i;
    char outputData[15];
    Response* theResponse = 0;
    
    output.tag("ExpControlOutput");
    output.attr("ctrlType", this->getClassType());
    output.attr("ctrlTag", this->getTag());
    
    // ctrl signals
    if (ctrlSignal != 0 && (
        strcmp(argv[0], "ctrlSig") == 0 ||
        strcmp(argv[0], "ctrlSignal") == 0 ||
        strcmp(argv[0], "ctrlSignals") == 0))
    {
        for (i = 0; i < numCtrlSignals; i++) {
            sprintf(outputData, "ctrlSignal%d", i + 1);
            output.tag("ResponseType", outputData);
        }
        theResponse = new ExpControlResponse(this, 1,
            Vector(numCtrlSignals));
    }
    
    // daq signals
    else if (daqSignal != 0 && (
        strcmp(argv[0], "daqSig") == 0 ||
        strcmp(argv[0], "daqSignal") == 0 ||
        strcmp(argv[0], "daqSignals") == 0))
    {
        for (i = 0; i < numDaqSignals; i++) {
            sprintf(outputData, "daqSignal%d", i + 1);
            output.tag("ResponseType", outputData);
        }
        theResponse = new ExpControlResponse(this, 2,
            Vector(numDaqSignals));
    }
    output.endTag();
    
    return theResponse;
}


int ECSpeedgoat::getResponse(int responseID, Information& info)
{
    switch (responseID) {
    case 1:  // ctrl signals
        return info.setVector(*ctrlSignal);
    
    case 2:  // daq signals
        return info.setVector(*daqSignal);
    
    default:
        return OF_ReturnType_failed;
    }
}


void ECSpeedgoat::Print(OPS_Stream& s, int flag)
{
    s << "****************************************************************\n";
    s << "* ExperimentalControl: " << this->getTag() << endln;
    s << "*   type: ECSpeedgoat\n";
    if (udp)
        s << "*   channel: UDP, ";
    else
        s << "*   channel: TCP, ";
    s << "ipAddress: " << ipAddress << ", ipPort: " << ipPort << endln;
    s << "*   ctrlFilter tags:";
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
    s << endln;
    if (flag == 1) {
        // print experimental control point information
        for (int i = 0; i < numTrialCPs; i++)
            trialCPs[i]->Print(opserr);
        for (int i = 0; i < numOutCPs; i++)
            outCPs[i]->Print(opserr);
    }
    s << "****************************************************************\n";
    s << endln;
}


int ECSpeedgoat::control()
{
    // send control signals and set newTarget flag
    (*newTarget) = 1;
    theChannel->sendVector(0, 0, *sendData, 0);
    theChannel->sendVector(0, 0, *sendData, 0);

    // wait until switchPC flag has changed as well
    (*switchPC) = 0;
    while ((*switchPC) != 1) {
        theChannel->recvVector(0, 0, *recvData, 0);
    }
    
    // reset newTarget flag
    (*newTarget) = 0;
    theChannel->sendVector(0, 0, *sendData, 0);
    theChannel->sendVector(0, 0, *sendData, 0);
    
    // wait until switchPC flag has changed back
    (*switchPC) = 1;
    while ((*switchPC) != 0) {
        theChannel->recvVector(0, 0, *recvData, 0);
    }
    
    return OF_ReturnType_completed;
}


int ECSpeedgoat::acquire()
{
    // read measured signals until target is reached
    (*atTarget) = 0;
    while ((*atTarget) != 1) {
        theChannel->recvVector(0, 0, *recvData, 0);
    }
    
    return OF_ReturnType_completed;
}
