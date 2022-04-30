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

#ifndef ESThreeActuators2d_h
#define ESThreeActuators2d_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 09/06
// Revision: A
//
// Description: This file contains the class definition for
// ESThreeActuators2d. ESThreeActuators2d is an experimental
// setup class with three actuators which control the two
// translational and the rotational degree of freedom of a specimen.

#include "ExperimentalSetup.h"

#include <Matrix.h>

class ESThreeActuators2d : public ExperimentalSetup
{
public:
    // constructors
    ESThreeActuators2d(int tag,
        double actLength0, double actLength1, double actLength2,
        double rigidLength0, double rigidLength1,
        ExperimentalControl* control = 0,
        int nlGeom = 0, const char *posAct0 = "left", double phiLocX = 0.0);
    ESThreeActuators2d(const ESThreeActuators2d& es);
    
    // destructor
    virtual ~ESThreeActuators2d();
    
    // method to get class type
    const char *getClassType() const {return "ESThreeActuators2d";};
    
    // public methods
    virtual int setup();
    virtual int transfTrialResponse(const Vector* disp,
        const Vector* vel,
        const Vector* accel,
        const Vector* force,
        const Vector* time);
    
    virtual ExperimentalSetup *getCopy();
    
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
    
    double La0;         // length of actuator 0
    double La1;         // length of actuator 1
    double La2;         // length of actuator 2
    double L0;          // rigid link length 0
    double L1;          // rigid link length 1
    int nlGeom;         // non-linear geometry (0: linear, 1: nonlinear)
    char posAct0[6];    // position of actuator 0 (left, right)
    double phiLocX;     // angle of local x axis w.r.t rigid link [deg]
    
    Matrix rotLocX;     // rotation matrix
    
    bool firstWarning[3];
};

#endif
