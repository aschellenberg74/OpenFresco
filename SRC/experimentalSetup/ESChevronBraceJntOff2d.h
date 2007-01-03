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

#ifndef ESChevronBraceJntOff2d_h
#define ESChevronBraceJntOff2d_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 09/06
// Revision: A
//
// Description: This file contains the class definition for
// ESChevronBraceJntOff2d. ESChevronBraceJntOff2d is an experimental
// setup class with three actuators which control the specimen
// deformations and two load cells which measure the six  support
// reactions (resisting forces). The setup accounts for the rigid joint
// offsets between the actuators.

#include "ExperimentalSetup.h"
#include <ExperimentalControl.h>

class ESChevronBraceJntOff2d : public ExperimentalSetup
{
public:
    // constructors
    ESChevronBraceJntOff2d(int tag,
        int nlGeomFlag,
        double actLength0, double actLength1, double actLength2,
        double rigidLength0, double rigidLength1, double rigidLength2,
        double rigidLength3, double rigidLength4, double rigidLength5,
        ExperimentalControl* control = 0);
    ESChevronBraceJntOff2d(const ESChevronBraceJntOff2d& es);
    
    // destructor
    virtual ~ESChevronBraceJntOff2d();
    
    // public methods
    virtual int setSize(ID sizeT, ID sizeO);
    virtual int commitState();
    virtual int setup();
    
    // public methods to transform the responses
    virtual int transfTrialResponse(const Vector* disp,
        const Vector* vel,
        const Vector* accel,
        const Vector* force,
        const Vector* time);
    virtual int transfDaqResponse(Vector* disp,
        Vector* vel,
        Vector* accel,
        Vector* force,
        Vector* time);

    virtual ExperimentalSetup *getCopy(void);
    
    // public methods for output
    void Print(OPS_Stream &s, int flag = 0);
    
protected:	
    // protected tranformation methods 
    virtual int transfTrialDisp(const Vector* disp);
    virtual int transfTrialVel(const Vector* vel);
    virtual int transfTrialAccel(const Vector* accel);
    virtual int transfTrialForce(const Vector* force);
    virtual int transfTrialTime(const Vector* time);
    
    virtual int transfDaqDisp(Vector* disp);
    virtual int transfDaqVel(Vector* vel);
    virtual int transfDaqAccel(Vector* accel);
    virtual int transfDaqForce(Vector* force);
    virtual int transfDaqTime(Vector* time);
    
private:
    // private tranformation methods
    virtual int transfTrialVel(const Vector* disp,
        const Vector* vel);
    virtual int transfTrialAccel(const Vector* disp,
        const Vector* vel,
        const Vector* accel);

    int nlFlag;     // non-linear geometry flag
    double La0;     // length of actuator 0
    double La1;     // length of actuator 1
    double La2;     // length of actuator 2
    double L0;      // rigid link length 0
    double L1;      // rigid link length 1
    double L2;      // rigid link length 2
    double L3;      // rigid link length 3
    double L4;      // rigid link length 4
    double L5;      // rigid link length 5
};

#endif
