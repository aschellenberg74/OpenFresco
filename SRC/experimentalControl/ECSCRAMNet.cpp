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

// $Revision: $
// $Date: $
// $URL: $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 11/06
// Revision: A
//
// Description: This file contains the implementation of the ECSCRAMNet class.

#include "ECSCRAMNet.h"

extern "C" {
#include <scrplus.h>
}


ECSCRAMNet::ECSCRAMNet(int tag, int memoffset, int numactch)
    : ExperimentalControl(tag),
    memOffset(memoffset), numActCh(numactch),
    cDisp(0), cVel(0), cAccel(0), cForce(0), cTime(0),
    dDisp(0), dVel(0), dAccel(0), dForce(0), dTime(0)
{
#ifdef _WIN32
    // map the SCRAMNet control status registers (CSRs)
	// and the SCRAMNet physical memory
	rValue = sp_scram_init();
    if (rValue != 0) {
        opserr << "ECSCRAMNet::ECSCRAMNet() - sp_scram_init:";
		if (rValue == -4)
            opserr << " could not open driver" << endln;
		if (rValue == -3)
            opserr << " could not map memory" << endln;
		if (rValue == -2)
            opserr << " could not map CSRs" << endln;
		if (rValue == -1)
            opserr << " could not read registry" << endln;
        exit(rValue);
    }
#else
    // map the SCRAMNet control status registers (CSRs)
    rValue = scr_reg_mm(MAP);
    if (rValue != 0) {
        opserr << "ECSCRAMNet::ECSCRAMNet() - scr_reg_mm(MAP):"
            << " could not map CSRs" << endln;
        exit(rValue);
    }

    // map the SCRAMNet physical memory
    rValue = scr_mem_mm(MAP);
    if (rValue != 0) {
        opserr << "ECSCRAMNet::ECSCRAMNet() - scr_mem_mm(MAP):"
            << " could not map memory" << endln;
        scr_reg_mm(UNMAP);
        exit(rValue);
    }
#endif

    // get the pointer to the base memory address
    memPtrBASE = (int*) get_base_mem();

    // get address for OpenFresco memory on SCRAMNet board
	// memOffset is given in bytes, so divide by 4
    memPtrOPF = (double*) (memPtrBASE + (memOffset/4));
    double *memPtr = memPtrOPF;

    // setup pointers to state flags
    newTarget = memPtr;
	memPtr++; newTarget[0] = 0.0;
    atTarget  = memPtr;
    memPtr++; atTarget[0] = 0.0;
    switchPC  = memPtr;
    memPtr++; switchPC[0] = 0.0;

    // setup pointers to control memory locations
    cDisp  = new Vector(memPtr, numActCh); 
    memPtr += numActCh; cDisp->Zero();
    cVel   = new Vector(memPtr, numActCh);
    memPtr += numActCh; cVel->Zero();
    cAccel = new Vector(memPtr, numActCh); 
    memPtr += numActCh; cAccel->Zero();
    cForce = new Vector(memPtr, numActCh); 
    memPtr += numActCh; cForce->Zero();
    cTime  = new Vector(memPtr, numActCh); 
    memPtr += numActCh; cTime->Zero();

	// setup pointers to daq memory locations
    dDisp  = new Vector(memPtr, numActCh);
    memPtr += numActCh; dDisp->Zero();
    dVel   = new Vector(memPtr, numActCh);
    memPtr += numActCh; dVel->Zero();
    dAccel = new Vector(memPtr, numActCh);
    memPtr += numActCh; dAccel->Zero();
    dForce = new Vector(memPtr, numActCh);
    memPtr += numActCh; dForce->Zero();
    dTime  = new Vector(memPtr, numActCh);
    memPtr += numActCh; dTime->Zero();

    opserr << "************************************************\n";
    opserr << "* The SCRANNet csr and memory have been mapped *\n";
    opserr << "************************************************\n";
    opserr << endln;    
}


ECSCRAMNet::ECSCRAMNet(const ECSCRAMNet &ec)
    : ExperimentalControl(ec),
    memOffset(ec.memOffset), numActCh(ec.numActCh)
{
    
}


ECSCRAMNet::~ECSCRAMNet()
{
    // delete memory of control vectors
    if (cDisp != 0)
        delete cDisp;
    if (cVel != 0)
        delete cVel;
    if (cAccel != 0)
        delete cAccel;
    if (cForce != 0)
        delete cForce;
    if (cTime != 0)
        delete cTime;
    
    // delete memory of daq vectors
    if (dDisp != 0)
        delete dDisp;
    if (dVel != 0)
        delete dVel;
    if (dAccel != 0)
        delete dAccel;
    if (dForce != 0)
        delete dForce;
    if (dTime != 0)
        delete dTime;
    
    // unmap the SCRAMNet control status registers (CSRs)
    rValue = scr_reg_mm(UNMAP);
    if (rValue != 0) {
        opserr << "ECSCRAMNet::~ECSCRAMNet() - scr_reg_mm(UNMAP):"
            << " could not unmap CSRs" << endln;
        exit(rValue);
    }

    // unmap the SCRAMNet physical memory
    rValue = scr_mem_mm(UNMAP);
    if (rValue != 0) {
        opserr << "ECSCRAMNet::~ECSCRAMNet() - scr_mem_mm(UNMAP):"
            << " could not unmap memory" << endln;
        exit(rValue);
    }
    
    opserr << "**************************************************\n";
    opserr << "* The SCRANNet csr and memory have been unmapped *\n";
    opserr << "**************************************************\n";
    opserr << endln;
}


int ECSCRAMNet::setSize(ID sizeT, ID sizeO)
{
    // check sizeTrial and sizeOut
    // for ECSCRAMNet object
    
    // ECSCRAMNet objects can only use 
    // trial response vectors with size <= numActCh and
    // output response vectors with size <= numActCh
    // check these are available in sizeT/sizeO.
    if (sizeT[OF_Resp_Disp] > numActCh || sizeT[OF_Resp_Vel] > numActCh ||
        sizeT[OF_Resp_Accel] > numActCh || sizeT[OF_Resp_Force] > numActCh ||
        sizeT[OF_Resp_Time] > numActCh ||
        sizeO[OF_Resp_Disp] > numActCh || sizeO[OF_Resp_Vel] > numActCh ||
        sizeO[OF_Resp_Accel] > numActCh || sizeO[OF_Resp_Force] > numActCh ||
        sizeO[OF_Resp_Time] > numActCh) {
        opserr << "ECSCRAMNet::setSize() - wrong sizeTrial/Out\n"; 
        opserr << "see User Manual.\n";
        scr_reg_mm(UNMAP);
        scr_mem_mm(UNMAP);
        return OF_ReturnType_failed;
    }
    
    (*sizeCtrl) = sizeT;
    (*sizeDaq)  = sizeO;
    
    return OF_ReturnType_completed;
}


int ECSCRAMNet::setup()
{
    // print experimental control information
    this->Print(opserr);
    
    opserr << "****************************************************************\n";
    opserr << "* Make sure that offset values of controller are set to ZERO   *\n";
    opserr << "*                                                              *\n";
    opserr << "* Press 'Enter' to proceed or 'c' to cancel the initialization *\n";
    opserr << "****************************************************************\n";
    opserr << endln;
    int c = getchar();
    if (c == 'c')  {
        getchar();
        scr_reg_mm(UNMAP);
        scr_mem_mm(UNMAP);
        exit(OF_ReturnType_failed);
    }
    
    do  {
        this->control();
        this->acquire();
        
        int i;
        opserr << "**************************************\n";
        opserr << "* Initial values of DAQ are:         *\n";
        opserr << "*                                    *\n";
        opserr << "* dspDaq = [";
        for (i=0; i<(*sizeDaq)[OF_Resp_Disp]; i++)
            opserr << " " << (*dDisp)(i);
        opserr << " ]\n";
        opserr << "* frcDaq = [";
        for (i=0; i<(*sizeDaq)[OF_Resp_Force]; i++)
            opserr << " " << (*dForce)(i);
        opserr << " ]\n";
        opserr << "*                                    *\n";
        opserr << "* Press 'Enter' to start the test or *\n";
        opserr << "* 'r' to repeat the measurement or   *\n";
        opserr << "* 'c' to cancel the initialization   *\n";
        opserr << "**************************************\n";
        opserr << endln;
        c = getchar();
        if (c == 'c')  {
            getchar();
            scr_reg_mm(UNMAP);
            scr_mem_mm(UNMAP);
            exit(OF_ReturnType_failed);
        } else if (c == 'r')  {
            getchar();
        }
    } while (c == 'r');
        
    opserr << "******************\n";
    opserr << "* Running......  *\n";
    opserr << "******************\n";
    opserr << endln;
    
    return OF_ReturnType_completed;
}


int ECSCRAMNet::setTrialResponse(const Vector* disp,
    const Vector* vel,
    const Vector* accel,
    const Vector* force,
    const Vector* time)
{
    int i;
    if (disp != 0) {
        for (i=0; i<(*sizeCtrl)[OF_Resp_Disp]; i++) {
            (*cDisp)(i) = (*disp)(i);
            if (theFilter != 0)
                (*cDisp)(i) = theFilter->filtering((*cDisp)(i));
        }
    }
    if (vel != 0) {
        for (i=0; i<(*sizeCtrl)[OF_Resp_Vel]; i++)
            (*cVel)(i) = (*vel)(i);
    }
    if (accel != 0) {
        for (i=0; i<(*sizeCtrl)[OF_Resp_Accel]; i++)
            (*cAccel)(i) = (*accel)(i);
    }
    if (force != 0) {
        for (i=0; i<(*sizeCtrl)[OF_Resp_Force]; i++)
            (*cForce)(i) = (*force)(i);
    }
    if (time != 0) {
        for (i=0; i<(*sizeCtrl)[OF_Resp_Time]; i++)
            (*cTime)(i) = (*time)(i);
    }
    
    this->control();
    
    return OF_ReturnType_completed;
}


int ECSCRAMNet::getDaqResponse(Vector* disp,
    Vector* vel,
    Vector* accel,
    Vector* force,
    Vector* time)
{
    this->acquire();
    
    int i;
    if (disp != 0) {
        for (i=0; i<(*sizeDaq)[OF_Resp_Disp]; i++)
            (*disp)(i) = (*dDisp)(i);
    }
    if (vel != 0) {
        for (i=0; i<(*sizeDaq)[OF_Resp_Vel]; i++)
            (*vel)(i) = (*dVel)(i);
    }
    if (accel != 0) {
        for (i=0; i<(*sizeDaq)[OF_Resp_Accel]; i++)
            (*accel)(i) = (*dAccel)(i);
    }
    if (force != 0) {
        for (i=0; i<(*sizeDaq)[OF_Resp_Force]; i++)
            (*force)(i) = (*dForce)(i);
    }
    if (time != 0) {
        for (i=0; i<(*sizeDaq)[OF_Resp_Time]; i++)
            (*time)(i) = (*dTime)(i);
    }
    
    return OF_ReturnType_completed;
}


int ECSCRAMNet::commitState()
{	
    return OF_ReturnType_completed;
}


ExperimentalControl *ECSCRAMNet::getCopy()
{
    return new ECSCRAMNet(*this);
}


void ECSCRAMNet::Print(OPS_Stream &s, int flag)
{
    s << "****************************************************************\n";
    s << "* ExperimentalControl: " << this->getTag() << endln; 
    s << "* type: ECSCRAMNet\n";
    s << "*   memOffset: " << memOffset << endln;
    s << "*   numActCh: " << numActCh << endln;
    if (theFilter != 0) {
        s << "*\tFilter: " << *theFilter << endln;
    }
    s << "****************************************************************\n";
    s << endln;
}


int ECSCRAMNet::control()
{
    // set newTarget flag
    newTarget[0] = 1.0;
    
	// wait until switchPC flag has changed as well
	while (switchPC[0] != 1.0) {}
    
    // reset newTarget flag
    newTarget[0] = 0.0;
    
	// wait until switchPC flag has changed as well
	while (switchPC[0] != 0.0) {}
    
    return OF_ReturnType_completed;
}


int ECSCRAMNet::acquire()
{
    // wait until target is reached
    while (atTarget[0] != 1.0) {}
    
    return OF_ReturnType_completed;
}
