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
// $URL: $

#ifndef ECdSpace_h
#define ECdSpace_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 09/06
// Revision: A
//
// Description: This file contains the class definition for ECdSpace.
// ECdSpace is a controller class for communicating with a dSpace
// digital signal processor.

#include "ExperimentalControl.h"

#include <clib32.h>

class ECdSpace : public ExperimentalControl
{
public:
    // constructors
    ECdSpace(int tag, int pcType, char *boardName);	
    ECdSpace(const ECdSpace &ec);
    
    // destructor
    virtual ~ECdSpace();
    
    // public methods to set and to get response
    virtual int setSize(ID sizeT, ID sizeO);
    virtual int setup();
    
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
    
    // public methods for output
    void Print(OPS_Stream &s, int flag = 0);    
    
protected:
    // protected methods to set and to get response
    virtual int control();
    virtual int acquire();
    
private:
    void sleep(const clock_t wait);
    
    int error, boardState, simState, pcType;
    char *boardName;

    unsigned int board_index;
    board_spec_tp board_spec;

    unsigned int newTarget, switchPC, atTarget;
    double *targDisp, *targVel, *targAccel;
    double *measDisp, *measForce;
    
    UInt32 simStateId;
    UInt32 newTargetId, switchPCId, atTargetId;
    UInt32 targDispId, targVelId, targAccelId;
    UInt32 measDispId, measForceId;
};

#endif
