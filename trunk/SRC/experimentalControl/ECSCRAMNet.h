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

// $Revision$
// $Date$
// $URL$

#ifndef ECSCRAMNet_h
#define ECSCRAMNet_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 11/06
// Revision: A
//
// Description: This file contains the class definition for ECSCRAMNet.
// ECSCRAMNet is a controller class for communicating with a shared
// common RAM network (SCRAMNet).

#include "ExperimentalControl.h"

class ECSCRAMNet : public ExperimentalControl
{
public:
    // constructors
    ECSCRAMNet(int tag, int memOffset, int numDOF);
    ECSCRAMNet(const ECSCRAMNet &ec);
    
    // destructor
    virtual ~ECSCRAMNet();
    
    // method to get class type
    const char *getClassType() const {return "ECSCRAMNet";};
    
    // public methods to set and to get response
    virtual int setup();
    virtual int setSize(ID sizeT, ID sizeO);
    
    virtual int setTrialResponse(const Vector* disp,
        const Vector* vel,
        const Vector* accel,
        const Vector* force,
        const Vector* time);
    virtual int getDaqResponse(Vector* disp,
        Vector* vel,
        Vector* accel,
        Vector* force,
        Vector* time);
    
    virtual int commitState();
    
    virtual ExperimentalControl *getCopy();
    
    // public methods for experimental control recorder
    virtual Response *setResponse(const char **argv, int argc,
        OPS_Stream &output);
    virtual int getResponse(int responseID, Information &info);
    
    // public methods for output
    void Print(OPS_Stream &s, int flag = 0);
    
protected:
    // protected methods to set and to get response
    virtual int control();
    virtual int acquire();
    
private:
    void sleep(const clock_t wait);
    
    const int memOffset;            // memory offset in bytes from SCRAMNet base address
    const int numDOF;               // number of degrees-of-freedom in control system
    
    const int *memPtrBASE;          // pointer to SCRAMNet base memory address
    float *memPtrOPF;               // pointer to OpenFresco base memory address
    
    unsigned int *newTarget, *switchPC, *atTarget;
    float *ctrlDisp, *ctrlVel, *ctrlAccel, *ctrlForce, *ctrlTime;
    float *daqDisp, *daqVel, *daqAccel, *daqForce, *daqTime;
    
    unsigned int flag;              // flag to check states of Simulink model
};

#endif
