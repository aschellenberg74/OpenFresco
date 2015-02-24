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

// $Revision$
// $Date$
// $URL$

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 08/13
// Revision: A
//
// Description: This file contains the implementation of the ECSCRAMNetGT class.

#include "ECSCRAMNetGT.h"


ECSCRAMNetGT::ECSCRAMNetGT(int tag, int memoffset, int numdof)
    : ExperimentalControl(tag),
    memOffset(memoffset), numDOF(numdof),
    memPtrBASE(0), memPtrOPF(0),
    newTarget(0), switchPC(0), atTarget(0),
    ctrlDisp(0), ctrlVel(0), ctrlAccel(0), ctrlForce(0), ctrlTime(0),
    daqDisp(0), daqVel(0), daqAccel(0), daqForce(0), daqTime(0)
{
    // initialize a handle to a specific SCRAMNet GT device/unit
    int unit = 0;
    int rValue = scgtOpen(unit, &gtHandle);
    if (rValue != SCGT_SUCCESS)  {
        opserr << "ECSCRAMNetGT::ECSCRAMNetGT() - scgtOpen():"
            << " could not open driver to device/unit #"
            << unit << ".\n";
        exit(rValue);
    }
    
    // get the pointer to the base memory address
    memPtrBASE = (int*) scgtMapMem(&gtHandle);
    if (memPtrBASE == NULL)  {
        opserr << "ECSCRAMNetGT::ECSCRAMNetGT() - scgtMapMem():"
            << " could not map memory into address space.\n";
        exit(OF_ReturnType_failed);
    }
    
    // get device information to check amount of mapped memory
    rValue = scgtGetDeviceInfo(&gtHandle, &deviceInfo);
    if (rValue != SCGT_SUCCESS)  {
        opserr << "ECSCRAMNetGT::ECSCRAMNetGT() - scgtGetDeviceInfo():"
            << " could not retrive device information.\n";
        exit(rValue);
    }
    if (int(deviceInfo.mappedMemSize/4) < (3 + 10*numDOF))  {
        opserr << "ECSCRAMNetGT::ECSCRAMNetGT() - "
            << " mapped memory size too small for required data.\n";
        exit(OF_ReturnType_failed);
    }
    
    // set OpenFresco nodeID (469D = node1, xPCtarget = node2)
    int nodeID = 3;
    rValue = scgtSetState(&gtHandle, SCGT_NODE_ID, nodeID);
    if (rValue != SCGT_SUCCESS)  {
        opserr << "ECSCRAMNetGT::ECSCRAMNetGT() - scgtSetState():"
            << " could not change nodeID to 3.\n";
        exit(rValue);
    }
    
    // set to write me last
    rValue = scgtSetState(&gtHandle, SCGT_WRITE_ME_LAST, 0);
    if (rValue != SCGT_SUCCESS)  {
        opserr << "ECSCRAMNetGT::ECSCRAMNetGT() - scgtSetState():"
            << " could not change to write me last.\n";
        exit(rValue);
    }
    
    // set the transaction mode for memory access to Longword (32-bit)
    // Long_mode = 0 (32-bit), Word_mode = 1 (16-bit), Byte_mode = 2 (8-bit)
    // | no swap 32-bit | 00 | Q(31:24) | Q(23:16) | Q(15:8)  | Q(7:0)   |
    // | 16 bit         | 01 | Q(15:8)  | Q(7:0)   | Q(31:24) | Q(23:16) |
    // | 8 bit          | 10 | Q(7:0)   | Q(15:8)  | Q(23:16) | Q(31:24) |
    rValue = scgtSetState(&gtHandle, SCGT_WORD_SWAP_ENABLE, 0);
    if (rValue != SCGT_SUCCESS)  {
        opserr << "ECSCRAMNetGT::ECSCRAMNetGT() - scgtSetState():"
            << " could not disable word swap.\n";
        exit(rValue);
    }
    rValue = scgtSetState(&gtHandle, SCGT_BYTE_SWAP_ENABLE, 0);
    if (rValue != SCGT_SUCCESS)  {
        opserr << "ECSCRAMNetGT::ECSCRAMNetGT() - scgtSetState():"
            << " could not disable byte swap.\n";
        exit(rValue);
    }
    
    // set state so node can receive and transmit data
    rValue = scgtSetState(&gtHandle, SCGT_RECEIVE_ENABLE, 1);
    if (rValue != SCGT_SUCCESS)  {
        opserr << "ECSCRAMNetGT::ECSCRAMNetGT() - scgtSetState():"
            << " could not change node to receive data.\n";
        exit(rValue);
    }
    rValue = scgtSetState(&gtHandle, SCGT_TRANSMIT_ENABLE, 1);
    if (rValue != SCGT_SUCCESS)  {
        opserr << "ECSCRAMNetGT::ECSCRAMNetGT() - scgtSetState():"
            << " could not change node to transmit data.\n";
        exit(rValue);
    }
    
    // get address for OpenFresco memory on SCRAMNet board
    // memOffset is given in bytes, so divide by 4
    memPtrOPF = (float*) (memPtrBASE + (memOffset/4));
    float *memPtr = memPtrOPF;
    
    // setup pointers to state flags
    newTarget = (int*) memPtr;  memPtr++;
    switchPC  = (int*) memPtr;  memPtr++;
    atTarget  = (int*) memPtr;  memPtr++;
    
    // setup pointers to control memory locations
    ctrlDisp  = memPtr;  memPtr += numDOF;
    ctrlVel   = memPtr;  memPtr += numDOF;
    ctrlAccel = memPtr;  memPtr += numDOF;
    ctrlForce = memPtr;  memPtr += numDOF;
    ctrlTime  = memPtr;  memPtr += numDOF;
    
    // setup pointers to daq memory locations
    daqDisp  = memPtr;  memPtr += numDOF;
    daqVel   = memPtr;  memPtr += numDOF;
    daqAccel = memPtr;  memPtr += numDOF;
    daqForce = memPtr;  memPtr += numDOF;
    daqTime  = memPtr;
    
    // initialize everything to zero
    newTarget[0] = 0;
    switchPC[0]  = 0;
    atTarget[0]  = 0;
    for (int i=0; i<numDOF; i++)  {
        ctrlDisp[i]  = 0.0;  daqDisp[i]  = 0.0;
        ctrlVel[i]   = 0.0;  daqVel[i]   = 0.0;
        ctrlAccel[i] = 0.0;  daqAccel[i] = 0.0;
        ctrlForce[i] = 0.0;  daqForce[i] = 0.0;
        ctrlTime[i]  = 0.0;  daqTime[i]  = 0.0;
    }
    
    opserr << "******************************************\n";
    opserr << "* The SCRANNet GT memory has been mapped *\n";
    opserr << "******************************************\n";
    opserr << endln;
}


ECSCRAMNetGT::ECSCRAMNetGT(const ECSCRAMNetGT &ec)
    : ExperimentalControl(ec),
    memOffset(ec.memOffset), numDOF(ec.numDOF),
    memPtrBASE(0), memPtrOPF(0),
    newTarget(0), switchPC(0), atTarget(0),
    ctrlDisp(0), ctrlVel(0), ctrlAccel(0), ctrlForce(0), ctrlTime(0),
    daqDisp(0), daqVel(0), daqAccel(0), daqForce(0), daqTime(0)
{
    memPtrBASE = ec.memPtrBASE;
    memPtrOPF  = ec.memPtrOPF;
    newTarget  = ec.newTarget;
    switchPC   = ec.switchPC;
    atTarget   = ec.atTarget;
    
    ctrlDisp  = ec.ctrlDisp;
    ctrlVel   = ec.ctrlVel;
    ctrlAccel = ec.ctrlAccel;
    ctrlForce = ec.ctrlForce;
    ctrlTime  = ec.ctrlTime;
    
    daqDisp  = ec.daqDisp;
    daqVel   = ec.daqVel;
    daqAccel = ec.daqAccel;
    daqForce = ec.daqForce;
    daqTime  = ec.daqTime;
}


ECSCRAMNetGT::~ECSCRAMNetGT()
{
    // stop predictor-corrector
    newTarget[0] = -1;
    
    // unmap the SCRAMNet physical memory
    scgtUnmapMem(&gtHandle);
    
    // close the handle to the SCRAMNet GT device/unit
    int rValue = scgtClose(&gtHandle);
    if (rValue != SCGT_SUCCESS)  {
        opserr << "ECSCRAMNetGT::~ECSCRAMNetGT() - scgtClose():"
            << " could not close driver.\n";
        exit(rValue);
    }
    
    opserr << endln;
    opserr << "********************************************\n";
    opserr << "* The SCRANNet GT memory has been unmapped *\n";
    opserr << "********************************************\n";
    opserr << endln;
}


int ECSCRAMNetGT::setup()
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
        scgtUnmapMem(&gtHandle);
        scgtClose(&gtHandle);
        exit(OF_ReturnType_failed);
    }
    
    do  {
        rValue += this->control();
        rValue += this->acquire();
        
        int i;
        opserr << "****************************************************************\n";
        opserr << "* Initial values of DAQ are:\n";
        opserr << "*\n";
        opserr << "* dspDaq = [";
        for (i=0; i<(*sizeDaq)(OF_Resp_Disp); i++)
            opserr << " " << daqDisp[i];
        opserr << " ]\n";
        opserr << "* frcDaq = [";
        for (i=0; i<(*sizeDaq)(OF_Resp_Force); i++)
            opserr << " " << daqForce[i];
        opserr << " ]\n";
        opserr << "*\n";
        opserr << "* Press 'Enter' to start the test or\n";
        opserr << "* 'r' to repeat the measurement or\n";
        opserr << "* 'c' to cancel the initialization\n";
        opserr << "****************************************************************\n";
        opserr << endln;
        c = getchar();
        if (c == 'c')  {
            getchar();
            scgtUnmapMem(&gtHandle);
            scgtClose(&gtHandle);
            exit(OF_ReturnType_failed);
        } else if (c == 'r')  {
            getchar();
        }
    } while (c == 'r');
    
    opserr << "*****************\n";
    opserr << "* Running...... *\n";
    opserr << "*****************\n";
    opserr << endln;
    
    return rValue;
}


int ECSCRAMNetGT::setSize(ID sizeT, ID sizeO)
{
    // check sizeTrial and sizeOut
    // for ECSCRAMNetGT object
    
    // ECSCRAMNetGT objects can only use 
    // trial response vectors with size <= numDOF and
    // output response vectors with size <= numDOF
    // check these are available in sizeT/sizeO.
    if (sizeT(OF_Resp_Disp) > numDOF || sizeT(OF_Resp_Vel) > numDOF ||
        sizeT(OF_Resp_Accel) > numDOF || sizeT(OF_Resp_Force) > numDOF ||
        sizeT(OF_Resp_Time) > numDOF ||
        sizeO(OF_Resp_Disp) > numDOF || sizeO(OF_Resp_Vel) > numDOF ||
        sizeO(OF_Resp_Accel) > numDOF || sizeO(OF_Resp_Force) > numDOF ||
        sizeO(OF_Resp_Time) > numDOF)  {
        opserr << "ECSCRAMNetGT::setSize() - wrong sizeTrial/Out\n"; 
        opserr << "see User Manual.\n";
        scgtUnmapMem(&gtHandle);
        scgtClose(&gtHandle);
        exit(OF_ReturnType_failed);
    }
    
    (*sizeCtrl) = sizeT;
    (*sizeDaq)  = sizeO;
    
    return OF_ReturnType_completed;
}


int ECSCRAMNetGT::setTrialResponse(const Vector* disp,
    const Vector* vel,
    const Vector* accel,
    const Vector* force,
    const Vector* time)
{
    int i, rValue = 0;
    if (disp != 0)  {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Disp); i++)  {
            ctrlDisp[i] = float((*disp)(i));
            if (theCtrlFilters[OF_Resp_Disp] != 0)
                ctrlDisp[i] = float(theCtrlFilters[OF_Resp_Disp]->filtering(ctrlDisp[i]));
        }
    }
    if (vel != 0)  {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Vel); i++)  {
            ctrlVel[i] = float((*vel)(i));
            if (theCtrlFilters[OF_Resp_Vel] != 0)
                ctrlVel[i] = float(theCtrlFilters[OF_Resp_Vel]->filtering(ctrlVel[i]));
        }
    }
    if (accel != 0)  {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Accel); i++)  {
            ctrlAccel[i] = float((*accel)(i));
            if (theCtrlFilters[OF_Resp_Accel] != 0)
                ctrlAccel[i] = float(theCtrlFilters[OF_Resp_Accel]->filtering(ctrlAccel[i]));
        }
    }
    if (force != 0)  {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Force); i++)  {
            ctrlForce[i] = float((*force)(i));
            if (theCtrlFilters[OF_Resp_Force] != 0)
                ctrlForce[i] = float(theCtrlFilters[OF_Resp_Force]->filtering(ctrlForce[i]));
        }
    }
    if (time != 0)  {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Time); i++)  {
            ctrlTime[i] = float((*time)(i));
            if (theCtrlFilters[OF_Resp_Time] != 0)
                ctrlTime[i] = float(theCtrlFilters[OF_Resp_Time]->filtering(ctrlTime[i]));
        }
    }
    
    rValue = this->control();
    
    return rValue;
}


int ECSCRAMNetGT::getDaqResponse(Vector* disp,
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
                daqDisp[i] = float(theDaqFilters[OF_Resp_Disp]->filtering(daqDisp[i]));
            (*disp)(i) = daqDisp[i];
        }
    }
    if (vel != 0)  {
        for (i=0; i<(*sizeDaq)(OF_Resp_Vel); i++)  {
            if (theDaqFilters[OF_Resp_Vel] != 0)
                daqVel[i] = float(theDaqFilters[OF_Resp_Vel]->filtering(daqVel[i]));
            (*vel)(i) = daqVel[i];
        }
    }
    if (accel != 0)  {
        for (i=0; i<(*sizeDaq)(OF_Resp_Accel); i++)  {
            if (theDaqFilters[OF_Resp_Accel] != 0)
                daqAccel[i] = float(theDaqFilters[OF_Resp_Accel]->filtering(daqAccel[i]));
            (*accel)(i) = daqAccel[i];
        }
    }
    if (force != 0)  {
        for (i=0; i<(*sizeDaq)(OF_Resp_Force); i++)  {
            if (theDaqFilters[OF_Resp_Force] != 0)
                daqForce[i] = float(theDaqFilters[OF_Resp_Force]->filtering(daqForce[i]));
            (*force)(i) = daqForce[i];
        }
    }
    if (time != 0)  {
        for (i=0; i<(*sizeDaq)(OF_Resp_Time); i++)  {
            if (theDaqFilters[OF_Resp_Time] != 0)
                daqTime[i] = float(theDaqFilters[OF_Resp_Time]->filtering(daqTime[i]));
            (*time)(i) = daqTime[i];
        }
    }
    
    return OF_ReturnType_completed;
}


int ECSCRAMNetGT::commitState()
{
    return OF_ReturnType_completed;
}


ExperimentalControl *ECSCRAMNetGT::getCopy()
{
    return new ECSCRAMNetGT(*this);
}


Response* ECSCRAMNetGT::setResponse(const char **argv, int argc,
    OPS_Stream &output)
{
    int i;
    char outputData[15];
    Response *theResponse = 0;
    
    output.tag("ExpControlOutput");
    output.attr("ctrlType",this->getClassType());
    output.attr("ctrlTag",this->getTag());
    
    // ctrl displacements
    if (ctrlDisp != 0 && (
        strcmp(argv[0],"ctrlDisp") == 0 ||
        strcmp(argv[0],"ctrlDisplacement") == 0 ||
        strcmp(argv[0],"ctrlDisplacements") == 0))
    {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Disp); i++)  {
            sprintf(outputData,"ctrlDisp%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 1,
            Vector((*sizeCtrl)(OF_Resp_Disp)));
    }
    
    // ctrl velocities
    if (ctrlVel != 0 && (
        strcmp(argv[0],"ctrlVel") == 0 ||
        strcmp(argv[0],"ctrlVelocity") == 0 ||
        strcmp(argv[0],"ctrlVelocities") == 0))
    {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Vel); i++)  {
            sprintf(outputData,"ctrlVel%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 2,
            Vector((*sizeCtrl)(OF_Resp_Vel)));
    }
    
    // ctrl accelerations
    if (ctrlAccel != 0 && (
        strcmp(argv[0],"ctrlAccel") == 0 ||
        strcmp(argv[0],"ctrlAcceleration") == 0 ||
        strcmp(argv[0],"ctrlAccelerations") == 0))
    {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Accel); i++)  {
            sprintf(outputData,"ctrlAccel%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 3,
            Vector((*sizeCtrl)(OF_Resp_Accel)));
    }
    
    // ctrl forces
    if (ctrlForce != 0 && (
        strcmp(argv[0],"ctrlForce") == 0 ||
        strcmp(argv[0],"ctrlForces") == 0))
    {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Force); i++)  {
            sprintf(outputData,"ctrlForce%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 4,
            Vector((*sizeCtrl)(OF_Resp_Force)));
    }
    
    // ctrl times
    if (ctrlTime != 0 && (
        strcmp(argv[0],"ctrlTime") == 0 ||
        strcmp(argv[0],"ctrlTimes") == 0))
    {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Time); i++)  {
            sprintf(outputData,"ctrlTime%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 5,
            Vector((*sizeCtrl)(OF_Resp_Time)));
    }
    
    // daq displacements
    if (daqDisp != 0 && (
        strcmp(argv[0],"daqDisp") == 0 ||
        strcmp(argv[0],"daqDisplacement") == 0 ||
        strcmp(argv[0],"daqDisplacements") == 0))
    {
        for (i=0; i<(*sizeDaq)(OF_Resp_Disp); i++)  {
            sprintf(outputData,"daqDisp%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 6,
            Vector((*sizeDaq)(OF_Resp_Disp)));
    }
    
    // daq velocities
    if (daqVel != 0 && (
        strcmp(argv[0],"daqVel") == 0 ||
        strcmp(argv[0],"daqVelocity") == 0 ||
        strcmp(argv[0],"daqVelocities") == 0))
    {
        for (i=0; i<(*sizeDaq)(OF_Resp_Vel); i++)  {
            sprintf(outputData,"daqVel%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 7,
            Vector((*sizeDaq)(OF_Resp_Vel)));
    }
    
    // daq accelerations
    if (daqAccel != 0 && (
        strcmp(argv[0],"daqAccel") == 0 ||
        strcmp(argv[0],"daqAcceleration") == 0 ||
        strcmp(argv[0],"daqAccelerations") == 0))
    {
        for (i=0; i<(*sizeDaq)(OF_Resp_Accel); i++)  {
            sprintf(outputData,"daqAccel%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 8,
            Vector((*sizeDaq)(OF_Resp_Accel)));
    }
    
    // daq forces
    if (daqForce != 0 && (
        strcmp(argv[0],"daqForce") == 0 ||
        strcmp(argv[0],"daqForces") == 0))
    {
        for (i=0; i<(*sizeDaq)(OF_Resp_Force); i++)  {
            sprintf(outputData,"daqForce%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 9,
            Vector((*sizeDaq)(OF_Resp_Force)));
    }
    
    // daq times
    if (daqTime != 0 && (
        strcmp(argv[0],"daqTime") == 0 ||
        strcmp(argv[0],"daqTimes") == 0))
    {
        for (i=0; i<(*sizeDaq)(OF_Resp_Time); i++)  {
            sprintf(outputData,"daqTime%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 10,
            Vector((*sizeDaq)(OF_Resp_Time)));
    }
    
    output.endTag();
    
    return theResponse;
}


int ECSCRAMNetGT::getResponse(int responseID, Information &info)
{
    Vector resp(0);
    
    switch (responseID)  {
    case 1:  // ctrl displacements
        resp.setData((double*)ctrlDisp,(*sizeCtrl)(OF_Resp_Disp));
        return info.setVector(resp);
        
    case 2:  // ctrl velocities
        resp.setData((double*)ctrlVel,(*sizeCtrl)(OF_Resp_Vel));
        return info.setVector(resp);
        
    case 3:  // ctrl accelerations
        resp.setData((double*)ctrlAccel,(*sizeCtrl)(OF_Resp_Accel));
        return info.setVector(resp);
        
    case 4:  // ctrl forces
        resp.setData((double*)ctrlForce,(*sizeCtrl)(OF_Resp_Force));
        return info.setVector(resp);
        
    case 5:  // ctrl times
        resp.setData((double*)ctrlTime,(*sizeCtrl)(OF_Resp_Time));
        return info.setVector(resp);
        
    case 6:  // daq displacements
        resp.setData((double*)daqDisp,(*sizeDaq)(OF_Resp_Disp));
        return info.setVector(resp);
        
    case 7:  // daq velocities
        resp.setData((double*)daqVel,(*sizeDaq)(OF_Resp_Vel));
        return info.setVector(resp);
        
    case 8:  // daq accelerations
        resp.setData((double*)daqAccel,(*sizeDaq)(OF_Resp_Accel));
        return info.setVector(resp);
        
    case 9:  // daq forces
        resp.setData((double*)daqForce,(*sizeDaq)(OF_Resp_Force));
        return info.setVector(resp);
        
    case 10:  // daq times
        resp.setData((double*)daqTime,(*sizeDaq)(OF_Resp_Time));
        return info.setVector(resp);
        
    default:
        return -1;
    }
}


void ECSCRAMNetGT::Print(OPS_Stream &s, int flag)
{
    s << "****************************************************************\n";
    s << "* ExperimentalControl: " << this->getTag() << endln; 
    s << "*   type: ECSCRAMNetGT\n";
    s << "*   memOffset: " << memOffset << endln;
    s << "*   numDOF: " << numDOF << endln;
    s << "*   ctrlFilters:";
    for (int i=0; i<OF_Resp_All; i++)  {
        if (theCtrlFilters[i] != 0)
            s << " " << theCtrlFilters[i]->getTag();
        else
            s << " 0";
    }
    s << "\n*   daqFilters:";
    for (int i=0; i<OF_Resp_All; i++)  {
        if (theDaqFilters[i] != 0)
            s << " " << theDaqFilters[i]->getTag();
        else
            s << " 0";
    }
    s << endln;
    s << "****************************************************************\n";
    s << endln;
}


int ECSCRAMNetGT::control()
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


int ECSCRAMNetGT::acquire()
{
    // wait until target is reached
    flag = 0;
    while (flag != 1)  {
        // read atTarget flag
        flag = atTarget[0];
    }
    
    return OF_ReturnType_completed;
}
