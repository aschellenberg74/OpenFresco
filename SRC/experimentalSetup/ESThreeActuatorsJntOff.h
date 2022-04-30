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

#ifndef ESThreeActuatorsJntOff_h
#define ESThreeActuatorsJntOff_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 09/06
// Revision: A
//
// Description: This file contains the class definition for
// ESThreeActuatorsJntOff. ESThreeActuatorsJntOff is an experimental setup
// class with three actuators which control two translational
// and one rotational degree of freedom of a specimen.

#include "ExperimentalSetup.h"

#include <Matrix.h>

class ESThreeActuatorsJntOff : public ExperimentalSetup
{
public:
    // constructors
    ESThreeActuatorsJntOff(int tag,
        const ID &DOF, int sizeT, int sizeO,
        double actLength0, double actLength1, double actLength2,
        double rigidLength0, double rigidLength1,
        double rigidLength2, double rigidLength3,
        double rigidLength4, double rigidLength5,
        double rigidLength6,
        ExperimentalControl* control = 0,
        int nlGeom = 0, const char *posAct0 = "left");
    ESThreeActuatorsJntOff(const ESThreeActuatorsJntOff& es);
    
    // destructor
    virtual ~ESThreeActuatorsJntOff();
    
    // method to get class type
    const char *getClassType() const {return "ESThreeActuatorsJntOff";};
    
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
    
    ID DOF;             // array of DOF
    int sizeT;          // trial size of disp, vel, accel, force
    int sizeO;          // output size of disp, vel, accel, force
    double La0;         // length of actuator 0
    double La1;         // length of actuator 1
    double La2;         // length of actuator 2
    double L0;          // horizontal rigid link length 0
    double L1;          // horizontal rigid link length 1
    double L2;          // horizontal rigid link length 2
    double L3;          // horizontal rigid link length 3
    double L4;          // vertical rigid link length 4
    double L5;          // vertical rigid link length 5
    double L6;          // vertical rigid link length 6
    int nlGeom;         // non-linear geometry (0: linear, 1: nonlinear)
    char posAct0[6];    // position of actuator 0 (left, right)
    
    bool firstWarning[3];
};

#endif
