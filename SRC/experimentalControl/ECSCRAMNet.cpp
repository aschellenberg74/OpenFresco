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
**   Andreas Schellenberg (andreas.schellenberg@gmail.com)            **
**   Yoshikazu Takahashi (yos@catfish.dpri.kyoto-u.ac.jp)             **
**   Gregory L. Fenves (fenves@berkeley.edu)                          **
**   Stephen A. Mahin (mahin@berkeley.edu)                            **
**                                                                    **
** ****************************************************************** */

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 11/06
// Revision: A
//
// Description: This file contains the implementation of the ECSCRAMNet class.

#include "ECSCRAMNet.h"
#include <ExperimentalCP.h>

#include <elementAPI.h>

extern "C" {
#include <scrplus.h>
}


void* OPF_ECSCRAMNet()
{
    // pointer to experimental control that will be returned
    ExperimentalControl* theControl = 0;
    
    if (OPS_GetNumRemainingInputArgs() < 3) {
        opserr << "WARNING invalid number of arguments\n";
        opserr << "Want: expControl SCRAMNet tag -nodeID id memOffset -trialCP cpTags -outCP cpTags "
            << "<-ctrlFilters (5 filterTag)> <-daqFilters (5 filterTag)>\n";
        return 0;
    }
    
    // control tag
    int tag;
    int numdata = 1;
    if (OPS_GetIntInput(&numdata, &tag) != 0) {
        opserr << "WARNING invalid expControl SCRAMNet tag\n";
        return 0;
    }
    
    // OpenFresco SCRAMNet GT node ID
    int nodeID;
    const char* type = OPS_GetString();
    if (strcmp(type, "-nodeID") != 0) {
        opserr << "WARNING expecting -nodeID id\n";
        opserr << "expControl SCRAMNet " << tag << endln;
        return 0;
    }
    numdata = 1;
    if (OPS_GetIntInput(&numdata, &nodeID) != 0) {
        opserr << "WARNING invalid nodeID\n";
        opserr << "expControl SCRAMNet " << tag << endln;
        return 0;
    }
    
    // memory offset in bytes
    int memOffset;
    numdata = 1;
    if (OPS_GetIntInput(&numdata, &memOffset) != 0) {
        opserr << "WARNING invalid memOffset\n";
        opserr << "expControl SCRAMNet " << tag << endln;
        return 0;
    }
    
    // trialCPs
    type = OPS_GetString();
    if (strcmp(type, "-trialCP") != 0) {
        opserr << "WARNING expecting -trialCP cpTags\n";
        opserr << "expControl SCRAMNet " << tag << endln;
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
        opserr << "expControl SCRAMNet " << tag << endln;
        return 0;
    }
    cpTags.resize(numTrialCPs);
    // create the array to hold the trial control points
    ExperimentalCP** trialCPs = new ExperimentalCP * [numTrialCPs];
    if (trialCPs == 0) {
        opserr << "WARNING out of memory\n";
        opserr << "expControl SCRAMNet " << tag << endln;
        return 0;
    }
    // populate array with trial control points
    for (int i = 0; i < numTrialCPs; i++) {
        trialCPs[i] = 0;
        trialCPs[i] = OPF_getExperimentalCP(cpTags(i));
        if (trialCPs[i] == 0) {
            opserr << "WARNING experimental control point not found\n";
            opserr << "expControlPoint " << cpTags(i) << endln;
            opserr << "expControl SCRAMNet " << tag << endln;
            return 0;
        }
    }
    
    // outCPs
    type = OPS_GetString();
    if (strcmp(type, "-outCP") != 0) {
        opserr << "WARNING expecting -outCP cpTags\n";
        opserr << "expControl SCRAMNet " << tag << endln;
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
        opserr << "expControl SCRAMNet " << tag << endln;
        return 0;
    }
    cpTags.resize(numOutCPs);
    // create the array to hold the output control points
    ExperimentalCP** outCPs = new ExperimentalCP * [numOutCPs];
    if (outCPs == 0) {
        opserr << "WARNING out of memory\n";
        opserr << "expControl SCRAMNet " << tag << endln;
        return 0;
    }
    // populate array with output control points
    for (int i = 0; i < numOutCPs; i++) {
        outCPs[i] = 0;
        outCPs[i] = OPF_getExperimentalCP(cpTags(i));
        if (outCPs[i] == 0) {
            opserr << "WARNING experimental control point not found\n";
            opserr << "expControlPoint " << cpTags(i) << endln;
            opserr << "expControl SCRAMNet " << tag << endln;
            return 0;
        }
    }
    
    // parsing was successful, allocate the control
    theControl = new ECSCRAMNet(tag, numTrialCPs, trialCPs,
        numOutCPs, outCPs, memOffset, nodeID);
    if (theControl == 0) {
        opserr << "WARNING could not create experimental control of type ECSCRAMNet\n";
        return 0;
    }
    
    // cleanup dynamic memory
    //if (trialCPs != 0)
    //    delete[] trialCPs;
    //if (outCPs != 0)
    //    delete[] outCPs;
    
    return theControl;
}


ECSCRAMNet::ECSCRAMNet(int tag, int nTrialCPs, ExperimentalCP** trialcps,
    int nOutCPs, ExperimentalCP** outcps, int memoffset, unsigned short nodeid)
    : ExperimentalControl(tag),
    numTrialCPs(nTrialCPs), trialCPs(0), numOutCPs(nOutCPs), outCPs(0),
    memOffset(memoffset), nodeID(nodeid),
    memPtrBASE(0), memPtrOPF(0), scrCtrlSig(0), scrDaqSig(0),
    newTarget(0), switchPC(0), atTarget(0),
    numCtrlSignals(0), numDaqSignals(0), ctrlSignal(0), daqSignal(0),
    trialSigOffset(0), ctrlSigOffset(0), daqSigOffset(0),
    gotRelativeTrial(1), flag(0)
{
    // get trial and output control points
    if (trialcps == 0 || outcps == 0) {
        opserr << "ECSCRAMNetGT::ECSCRAMNetGT() - "
            << "null trialCPs or outCPs array passed.\n";
        exit(OF_ReturnType_failed);
    }
    trialCPs = trialcps;
    outCPs = outcps;
    
    // get total number of control and daq signals and
    // check if any signals use relative trial reference
    // (do this here instead of in setup() method so we
    // can check that we have enough memory on SCRAMNet
    // and then map the memory required for OpenFresco)
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
    
#ifdef _WIN32
    // map the SCRAMNet control status registers (CSRs)
    // and the SCRAMNet physical memory
    int rValue = sp_scram_init();
    if (rValue != 0) {
        opserr << "ECSCRAMNet::ECSCRAMNet() - sp_scram_init():";
        if (rValue == -4)
            opserr << " could not open driver.\n";
        if (rValue == -3)
            opserr << " could not map memory.\n";
        if (rValue == -2)
            opserr << " could not map CSRs.\n";
        if (rValue == -1)
            opserr << " could not read registry.\n";
        exit(rValue);
    }
#else
    // map the SCRAMNet control status registers (CSRs)
    rValue = scr_reg_mm(MAP);
    if (rValue != 0) {
        opserr << "ECSCRAMNet::ECSCRAMNet() - scr_reg_mm(MAP):"
            << " could not map CSRs.\n";
        exit(rValue);
    }
    
    // map the SCRAMNet physical memory
    rValue = scr_mem_mm(MAP);
    if (rValue != 0) {
        opserr << "ECSCRAMNet::ECSCRAMNet() - scr_mem_mm(MAP):"
            << " could not map memory.\n";
        scr_reg_mm(UNMAP);
        exit(rValue);
    }
#endif
    
    // set OpenFresco nodeID
    // (typically: OpenFresco = node10, xPCtarget = node20, controller = node30)
    scr_csr_write(SCR_CSR3, nodeID);
    
    // set the transaction mode for memory access to Longword (32-bit)
    // Long_mode = 0 (32-bit), Word_mode = 1 (16-bit), Byte_mode = 2 (8-bit)
    // | no swap 32-bit | 00 | Q(31:24) | Q(23:16) | Q(15:8)  | Q(7:0)   |
    // | 16 bit         | 01 | Q(15:8)  | Q(7:0)   | Q(31:24) | Q(23:16) |
    // | 8 bit          | 10 | Q(7:0)   | Q(15:8)  | Q(23:16) | Q(31:24) |
    if (sp_stm_mm(Long_mode) != TRUE) {
        opserr << "ECSCRAMNet::ECSCRAMNet() - sp_stm_mm():"
            << " could not set Longword memory access mode.\n";
    }
    
    // set CSR0 bits so node can receive and transmit data
    unsigned short csrValue = 0x8003;
    scr_csr_write(SCR_CSR0, csrValue);
    
    // get the pointer to the base memory address
    memPtrBASE = (int*)get_base_mem();
    
    // get address for OpenFresco memory on SCRAMNet GT board
    // memOffset is given in bytes, so divide by 4
    memPtrOPF = (float*)(memPtrBASE + (memOffset / 4));
    float* memPtr = memPtrOPF;
    
    // setup scr pointers to newTarget flag
    newTarget = (int*)memPtr;  memPtr++;
    newTarget[0] = 0;
    
    // setup scr pointers to control signals
    scrCtrlSig = memPtr;  memPtr += numCtrlSignals;
    for (int i = 0; i < numCtrlSignals; i++)
        scrCtrlSig[i] = 0.0;
    
    // setup scr pointers to switchPC and atTarget flags
    switchPC = (int*)memPtr;  memPtr++;
    atTarget = (int*)memPtr;  memPtr++;
    
    // setup scr pointers to daq signals
    scrDaqSig = memPtr;
    
    opserr << "************************************************\n";
    opserr << "* The SCRANNet csr and memory have been mapped *\n";
    opserr << "************************************************\n";
    opserr << endln;
}


ECSCRAMNet::ECSCRAMNet(const ECSCRAMNet& ec)
    : ExperimentalControl(ec),
    memOffset(ec.memOffset), nodeID(ec.nodeID),
    memPtrBASE(0), memPtrOPF(0), scrCtrlSig(0), scrDaqSig(0),
    newTarget(0), switchPC(0), atTarget(0),
    numCtrlSignals(0), numDaqSignals(0), ctrlSignal(0), daqSignal(0),
    trialSigOffset(0), ctrlSigOffset(0), daqSigOffset(0),
    gotRelativeTrial(1), flag(0)
{
    numTrialCPs = ec.numTrialCPs;
    trialCPs = ec.trialCPs;
    numOutCPs = ec.numOutCPs;
    outCPs = ec.outCPs;
    
    memPtrBASE = ec.memPtrBASE;
    memPtrOPF = ec.memPtrOPF;
    scrCtrlSig = ec.scrCtrlSig;
    scrDaqSig = ec.scrDaqSig;
    newTarget = ec.newTarget;
    switchPC = ec.switchPC;
    atTarget = ec.atTarget;
    
    numCtrlSignals = ec.numCtrlSignals;
    numDaqSignals = ec.numDaqSignals;
    gotRelativeTrial = ec.gotRelativeTrial;
}


ECSCRAMNet::~ECSCRAMNet()
{
    // stop the target application on the xPC Target
    newTarget[0] = -1;
    this->ExperimentalControl::sleep(10);
    
    // set everything back to zero
    newTarget[0] = 0;
    for (int i = 0; i < numCtrlSignals; i++)
        ctrlSignal[i] = 0.0;
    
    // unmap the SCRAMNet control status registers (CSRs)
    int rValue = scr_reg_mm(UNMAP);
    if (rValue != 0) {
        opserr << "ECSCRAMNet::~ECSCRAMNet() - scr_reg_mm(UNMAP):"
            << " could not unmap CSRs.\n";
        exit(rValue);
    }
    
    // unmap the SCRAMNet physical memory
    rValue = scr_mem_mm(UNMAP);
    if (rValue != 0) {
        opserr << "ECSCRAMNet::~ECSCRAMNet() - scr_mem_mm(UNMAP):"
            << " could not unmap memory.\n";
        exit(rValue);
    }
    
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
    opserr << "**************************************************\n";
    opserr << "* The SCRANNet csr and memory have been unmapped *\n";
    opserr << "**************************************************\n";
    opserr << endln;
}


int ECSCRAMNet::setup()
{
    int rValue = 0;
    
    // resize signal vectors
    ctrlSignal.resize(numCtrlSignals);
    ctrlSignal.Zero();
    daqSignal.resize(numDaqSignals);
    daqSignal.Zero();
    
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
    opserr << "* Make sure that offset values of controller are set to ZERO,  *\n";
    opserr << "* then start the Simulink model by typing tg.start in Matlab   *\n";
    opserr << "*                                                              *\n";
    opserr << "* Press 'Enter' to proceed or 'c' to cancel the initialization *\n";
    opserr << "****************************************************************\n";
    opserr << endln;
    int c = getchar();
    if (c == 'c') {
        getchar();
        this->~ECSCRAMNet();
        exit(OF_ReturnType_failed);
    }
    
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
                                ctrlSigOffset(kT) = daqSignal(kO);
                                ctrlSignal(kT) = ctrlSigOffset(kT);
                            }
                            kO++;
                        }
                    }
                }
                kT++;
            }
        }
        
        // send control signals with offsets to controller
        for (int i = 0; i < numCtrlSignals; i++)
            scrCtrlSig[i] = (float)ctrlSignal(i);
        
        // determine daq signal offsets
        int kO = 0;
        for (int i = 0; i < numOutCPs; i++) {
            // get output control point parameters
            int numSignals = outCPs[i]->getNumSignal();
            ID isRelDaq = outCPs[i]->getDaqSigRefType();
            // loop through all the output control point signals
            for (int j = 0; j < numSignals; j++) {
                if (isRelDaq(j))
                    daqSigOffset(kO) = -daqSignal(kO);
                kO++;
            }
        }
        
        opserr << "****************************************************************\n";
        opserr << "* Initial values of DAQ are:\n";
        for (int i = 0; i < numDaqSignals; i++)
            opserr << "*   daqSig" << i + 1 << " = " << daqSignal(i) << endln;
        opserr << "*\n";
        for (int i = 0; i < numDaqSignals; i++) {
            if (daqSigOffset(i) != 0)
                opserr << "*   daqSigOffset" << i + 1 << " = " << daqSigOffset(i) << endln;
        }
        opserr << "*\n";
        for (int i = 0; i < numCtrlSignals; i++) {
            if (ctrlSigOffset(i) != 0)
                opserr << "*   ctrlSigOffset" << i + 1 << " = " << ctrlSigOffset(i) << endln;
        }
        opserr << "*\n";
        opserr << "* Press 'Enter' to start the test or\n";
        opserr << "* 'r' to repeat the measurement or\n";
        opserr << "* 'c' to cancel the initialization\n";
        opserr << "****************************************************************\n";
        opserr << endln;
        c = getchar();
        if (c == 'c') {
            getchar();
            this->~ECSCRAMNet();
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


int ECSCRAMNet::setSize(ID sizeT, ID sizeO)
{
    // check sizeTrial and sizeOut against sizes
    // specified in the control points
    // ECSCRAMNet objects can use:
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
            opserr << "ECSCRAMNet::setSize() - wrong sizeTrial/Out\n";
            opserr << "see User Manual.\n";
            this->~ECSCRAMNet();
            exit(OF_ReturnType_failed);
        }
    }
    // finally assign sizes
    (*sizeCtrl) = maxdofT;
    (*sizeDaq) = maxdofO;
    
    return OF_ReturnType_completed;
}


int ECSCRAMNet::setTrialResponse(
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
        
        // loop through all the trial control point dofs
        for (int j = 0; j < numSignals; j++) {
            // assemble the control signal array
            if (rsp(j) == OF_Resp_Disp && disp != 0)
                ctrlSignal(k) = (*disp)(dof(j));
            else if (rsp(j) == OF_Resp_Force && force != 0)
                ctrlSignal(k) = (*force)(dof(j));
            else if (rsp(j) == OF_Resp_Time && time != 0)
                ctrlSignal(k) = (*time)(dof(j));
            else if (rsp(j) == OF_Resp_Vel && vel != 0)
                ctrlSignal(k) = (*vel)(dof(j));
            else if (rsp(j) == OF_Resp_Accel && accel != 0)
                ctrlSignal(k) = (*accel)(dof(j));
            
            // get initial trial signal offset
            if (gotRelativeTrial == 0 && ctrlSignal(k) != 0) {
                trialSigOffset(k) = -ctrlSignal(k);
                
                opserr << endln;
                opserr << "****************************************************************\n";
                opserr << "* Initial signal values of FEA are:\n";
                for (int i = 0; i < numCtrlSignals; i++)
                    opserr << "*   trialSig" << i + 1 << " = " << ctrlSignal(i) << endln;
                opserr << "*\n";
                for (int i = 0; i < numCtrlSignals; i++) {
                    if (trialSigOffset(i) != 0)
                        opserr << "*   trialSigOffset" << i + 1 << " = " << trialSigOffset(i) << endln;
                }
                opserr << "*\n";
                opserr << "****************************************************************\n";
                opserr << endln;
            }
            
            // apply trial signal offset if not zero
            if (trialSigOffset(k) != 0)
                ctrlSignal(k) += trialSigOffset(k);
            
            // filter control signal if a filter exists
            if (theCtrlFilters[rsp(j)] != 0)
                ctrlSignal(k) = theCtrlFilters[rsp(j)]->filtering(ctrlSignal(k));
            
            // apply control signal offset if not zero
            if (ctrlSigOffset(k) != 0)
                ctrlSignal(k) += ctrlSigOffset(k);
            
            // increment counter
            k++;
        }
    }
    
    // set flag that relative trial signals have been obtained
    if (gotRelativeTrial == 0)
        gotRelativeTrial = 1;
    
    // send control signal array to controller
    k += this->control();
    
    return (k - numCtrlSignals);
}


int ECSCRAMNet::getDaqResponse(
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
                daqSignal(k) += daqSigOffset(k);
            
            // filter daq signal if a filter exists
            if (theDaqFilters[rsp(j)] != 0)
                daqSignal(k) = theDaqFilters[rsp(j)]->filtering(daqSignal(k));
            
            // populate the daq response vectors
            if (rsp(j) == OF_Resp_Disp && disp != 0)
                (*disp)(dof(j)) = daqSignal(k);
            else if (rsp(j) == OF_Resp_Force && force != 0)
                (*force)(dof(j)) = daqSignal(k);
            else if (rsp(j) == OF_Resp_Time && time != 0)
                (*time)(dof(j)) = daqSignal(k);
            else if (rsp(j) == OF_Resp_Vel && vel != 0)
                (*vel)(dof(j)) = daqSignal(k);
            else if (rsp(j) == OF_Resp_Accel && accel != 0)
                (*accel)(dof(j)) = daqSignal(k);
            
            // increment counter
            k++;
        }
    }
    
    return (k + rValue - numDaqSignals);
}


int ECSCRAMNet::commitState()
{
    return OF_ReturnType_completed;
}


ExperimentalControl* ECSCRAMNet::getCopy()
{
    return new ECSCRAMNet(*this);
}


Response* ECSCRAMNet::setResponse(const char** argv, int argc,
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


int ECSCRAMNet::getResponse(int responseID, Information& info)
{
    switch (responseID) {
    case 1:  // ctrl signals
        return info.setVector(ctrlSignal);
    
    case 2:  // daq signals
        return info.setVector(daqSignal);
    
    default:
        return OF_ReturnType_failed;
    }
}


void ECSCRAMNet::Print(OPS_Stream& s, int flag)
{
    s << "****************************************************************\n";
    s << "* ExperimentalControl: " << this->getTag() << endln;
    s << "*   type: ECSCRAMNet\n";
    s << "*   nodeID: " << int(nodeID) << endln;
    s << "*   memOffset: " << memOffset << endln;
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


int ECSCRAMNet::control()
{
    // send control signals
    for (int i = 0; i < numCtrlSignals; i++)
        scrCtrlSig[i] = (float)ctrlSignal(i);
    
    // set newTarget flag
    newTarget[0] = 1;
    
    // wait until switchPC flag has changed as well
    flag = 0;
    while (flag != 1) {
        // read switchPC flag
        flag = switchPC[0];
    }
    
    // reset newTarget flag
    newTarget[0] = 0;
    
    // wait until switchPC flag has changed as well
    flag = 1;
    while (flag != 0) {
        // read switchPC flag
        flag = switchPC[0];
    }
    
    return OF_ReturnType_completed;
}


int ECSCRAMNet::acquire()
{
    // wait until target is reached
    flag = 0;
    while (flag != 1) {
        // read atTarget flag
        flag = atTarget[0];
    }
    
    // read measured signals at target
    for (int i = 0; i < numDaqSignals; i++)
        daqSignal(i) = (double)scrDaqSig[i];
    
    return OF_ReturnType_completed;
}
