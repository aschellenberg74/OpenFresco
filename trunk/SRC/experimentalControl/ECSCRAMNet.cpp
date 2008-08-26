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
    memPtrBASE(0), memPtrOPF(0),
    newTarget(0), switchPC(0), atTarget(0),
    targDisp(0), targVel(0), targAccel(0), targForce(0), targTime(0),
    measDisp(0), measVel(0), measAccel(0), measForce(0), measTime(0)
{
#ifdef _WIN32
    // map the SCRAMNet control status registers (CSRs)
	// and the SCRAMNet physical memory
	int rValue = sp_scram_init();
    if (rValue != 0)  {
        opserr << "ECSCRAMNet::ECSCRAMNet() - sp_scram_init:";
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
    if (rValue != 0)  {
        opserr << "ECSCRAMNet::ECSCRAMNet() - scr_reg_mm(MAP):"
            << " could not map CSRs.\n";
        exit(rValue);
    }

    // map the SCRAMNet physical memory
    rValue = scr_mem_mm(MAP);
    if (rValue != 0)  {
        opserr << "ECSCRAMNet::ECSCRAMNet() - scr_mem_mm(MAP):"
            << " could not map memory.\n";
        scr_reg_mm(UNMAP);
        exit(rValue);
    }
#endif

    // set CSR0 bits so node can receive and transmit data
    unsigned short csrValue = 0x8003;
    scr_csr_write(SCR_CSR0,csrValue);

    // get the pointer to the base memory address
    memPtrBASE = (int*) get_base_mem();

    // get address for OpenFresco memory on SCRAMNet board
	// memOffset is given in bytes, so divide by 4
    memPtrOPF = (float*) (memPtrBASE + (memOffset/4));
    float *memPtr = memPtrOPF;

    // setup pointers to state flags
    newTarget = (unsigned int*) memPtr;  memPtr++;
    switchPC  = (unsigned int*) memPtr;  memPtr++;
    atTarget  = (unsigned int*) memPtr;  memPtr++;

    // setup pointers to control memory locations
    targDisp  = memPtr;  memPtr += numActCh;
    targVel   = memPtr;  memPtr += numActCh;
    targAccel = memPtr;  memPtr += numActCh;
    targForce = memPtr;  memPtr += numActCh;
    targTime  = memPtr;  memPtr += numActCh;

	// setup pointers to daq memory locations
    measDisp  = memPtr;  memPtr += numActCh;
    measVel   = memPtr;  memPtr += numActCh;
    measAccel = memPtr;  memPtr += numActCh;
    measForce = memPtr;  memPtr += numActCh;
    measTime  = memPtr;  memPtr += numActCh;

    // initialize everything to zero
	newTarget[0] = 0;
    switchPC[0]  = 0;
    atTarget[0]  = 0;
    for (int i=0; i<numActCh; i++)  {
        targDisp[i]  = 0.0;  measDisp[i]  = 0.0;
        targVel[i]   = 0.0;  measVel[i]   = 0.0;
        targAccel[i] = 0.0;  measAccel[i] = 0.0;
        targForce[i] = 0.0;  measForce[i] = 0.0;
        targTime[i]  = 0.0;  measTime[i]  = 0.0;
    }

    opserr << "************************************************\n";
    opserr << "* The SCRANNet csr and memory have been mapped *\n";
    opserr << "************************************************\n";
    opserr << endln;    
}


ECSCRAMNet::ECSCRAMNet(const ECSCRAMNet &ec)
    : ExperimentalControl(ec),
    memOffset(ec.memOffset), numActCh(ec.numActCh)
{
    // temporarily does nothing
}


ECSCRAMNet::~ECSCRAMNet()
{
    // unmap the SCRAMNet control status registers (CSRs)
    int rValue = scr_reg_mm(UNMAP);
    if (rValue != 0)  {
        opserr << "ECSCRAMNet::~ECSCRAMNet() - scr_reg_mm(UNMAP):"
            << " could not unmap CSRs.\n";
        exit(rValue);
    }

    // unmap the SCRAMNet physical memory
    rValue = scr_mem_mm(UNMAP);
    if (rValue != 0)  {
        opserr << "ECSCRAMNet::~ECSCRAMNet() - scr_mem_mm(UNMAP):"
            << " could not unmap memory.\n";
        exit(rValue);
    }
    
    opserr << endln;
    opserr << "**************************************************\n";
    opserr << "* The SCRANNet csr and memory have been unmapped *\n";
    opserr << "**************************************************\n";
    opserr << endln;
}


int ECSCRAMNet::setup()
{
    int rValue = 0;
    
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
        rValue += this->control();
        rValue += this->acquire();
        
        int i;
        opserr << "**************************************\n";
        opserr << "* Initial values of DAQ are:         *\n";
        opserr << "*                                    *\n";
        opserr << "* dspDaq = [";
        for (i=0; i<(*sizeDaq)(OF_Resp_Disp); i++)
            opserr << " " << measDisp[i];
        opserr << " ]\n";
        opserr << "* frcDaq = [";
        for (i=0; i<(*sizeDaq)(OF_Resp_Force); i++)
            opserr << " " << measForce[i];
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
    
    return rValue;
}


int ECSCRAMNet::setSize(ID sizeT, ID sizeO)
{
    // check sizeTrial and sizeOut
    // for ECSCRAMNet object
    
    // ECSCRAMNet objects can only use 
    // trial response vectors with size <= numActCh and
    // output response vectors with size <= numActCh
    // check these are available in sizeT/sizeO.
    if (sizeT(OF_Resp_Disp) > numActCh || sizeT(OF_Resp_Vel) > numActCh ||
        sizeT(OF_Resp_Accel) > numActCh || sizeT(OF_Resp_Force) > numActCh ||
        sizeT(OF_Resp_Time) > numActCh ||
        sizeO(OF_Resp_Disp) > numActCh || sizeO(OF_Resp_Vel) > numActCh ||
        sizeO(OF_Resp_Accel) > numActCh || sizeO(OF_Resp_Force) > numActCh ||
        sizeO(OF_Resp_Time) > numActCh)  {
        opserr << "ECSCRAMNet::setSize() - wrong sizeTrial/Out\n"; 
        opserr << "see User Manual.\n";
        scr_reg_mm(UNMAP);
        scr_mem_mm(UNMAP);
        exit(OF_ReturnType_failed);
    }
    
    (*sizeCtrl) = sizeT;
    (*sizeDaq)  = sizeO;
    
    return OF_ReturnType_completed;
}


int ECSCRAMNet::setTrialResponse(const Vector* disp,
    const Vector* vel,
    const Vector* accel,
    const Vector* force,
    const Vector* time)
{
    int i, rValue = 0;
    if (disp != 0)  {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Disp); i++)  {
            targDisp[i] = float((*disp)(i));
            if (theCtrlFilters[OF_Resp_Disp] != 0)
                targDisp[i] = float(theCtrlFilters[OF_Resp_Disp]->filtering(targDisp[i]));
        }
    }
    if (vel != 0)  {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Vel); i++)  {
            targVel[i] = float((*vel)(i));
            if (theCtrlFilters[OF_Resp_Vel] != 0)
                targVel[i] = float(theCtrlFilters[OF_Resp_Vel]->filtering(targVel[i]));
        }
    }
    if (accel != 0)  {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Accel); i++)  {
            targAccel[i] = float((*accel)(i));
            if (theCtrlFilters[OF_Resp_Accel] != 0)
                targAccel[i] = float(theCtrlFilters[OF_Resp_Accel]->filtering(targAccel[i]));
        }
    }
    if (force != 0)  {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Force); i++)  {
            targForce[i] = float((*force)(i));
            if (theCtrlFilters[OF_Resp_Force] != 0)
                targForce[i] = float(theCtrlFilters[OF_Resp_Force]->filtering(targForce[i]));
        }
    }
    if (time != 0)  {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Time); i++)  {
            targTime[i] = float((*time)(i));
            if (theCtrlFilters[OF_Resp_Time] != 0)
                targTime[i] = float(theCtrlFilters[OF_Resp_Time]->filtering(targTime[i]));
        }
    }
    
    rValue = this->control();
    
    return rValue;
}


int ECSCRAMNet::getDaqResponse(Vector* disp,
    Vector* vel,
    Vector* accel,
    Vector* force,
    Vector* time)
{
    this->acquire();
    
    int i;
    if (disp != 0)  {
        for (i=0; i<(*sizeDaq)(OF_Resp_Disp); i++)  {
            if (theDaqFilters[OF_Resp_Disp] != 0)
                measDisp[i] = float(theDaqFilters[OF_Resp_Disp]->filtering(measDisp[i]));
            (*disp)(i) = measDisp[i];
        }
    }
    if (vel != 0)  {
        for (i=0; i<(*sizeDaq)(OF_Resp_Vel); i++)  {
            if (theDaqFilters[OF_Resp_Vel] != 0)
                measVel[i] = float(theDaqFilters[OF_Resp_Vel]->filtering(measVel[i]));
            (*vel)(i) = measVel[i];
        }
    }
    if (accel != 0)  {
        for (i=0; i<(*sizeDaq)(OF_Resp_Accel); i++)  {
            if (theDaqFilters[OF_Resp_Accel] != 0)
                measAccel[i] = float(theDaqFilters[OF_Resp_Accel]->filtering(measAccel[i]));
            (*accel)(i) = measAccel[i];
        }
    }
    if (force != 0)  {
        for (i=0; i<(*sizeDaq)(OF_Resp_Force); i++)  {
            if (theDaqFilters[OF_Resp_Force] != 0)
                measForce[i] = float(theDaqFilters[OF_Resp_Force]->filtering(measForce[i]));
            (*force)(i) = measForce[i];
        }
    }
    if (time != 0)  {
        for (i=0; i<(*sizeDaq)(OF_Resp_Time); i++)  {
            if (theDaqFilters[OF_Resp_Time] != 0)
                measTime[i] = float(theDaqFilters[OF_Resp_Time]->filtering(measTime[i]));
            (*time)(i) = measTime[i];
        }
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
    if (theCtrlFilters != 0)
        s << "*   ctrlFilter: " << *theCtrlFilters << endln;
    if (theDaqFilters != 0)
        s << "*   daqFilter: " << *theDaqFilters << endln;
    s << "****************************************************************\n";
    s << endln;
}


int ECSCRAMNet::control()
{    
    // set newTarget flag
    newTarget[0] = 1;

    // wait until switchPC flag has changed as well
    flag = 0;
	while (flag != 1)  {
        // read switchPC flag
        flag = switchPC[0];
    }
    
    // reset newTarget flag
    newTarget[0] = 0;
    
	// wait until switchPC flag has changed as well
    flag = 1;
	while (flag != 0)  {
        // read switchPC flag
        flag = switchPC[0];
    }
    
    return OF_ReturnType_completed;
}


int ECSCRAMNet::acquire()
{
    // wait until target is reached
    flag = 0;
    while (flag != 1)  {
        // read atTarget flag
        flag = atTarget[0];
    }
    
    return OF_ReturnType_completed;
}


void ECSCRAMNet::sleep(const clock_t wait)
{
    clock_t goal;
    goal = wait + clock();
    while (goal>clock());
}
