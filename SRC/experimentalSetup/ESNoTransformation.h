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

#ifndef ESNoTransformation_h
#define ESNoTransformation_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 09/06
// Revision: A
//
// Description: This file contains the class definition for
// ESNoTransformation. ESNoTransformation is an experimental
// setup class with up to six actuators which are set to control
// any of the basic degrees of freedom of a specimen.

#include "ExperimentalSetup.h"

class ESNoTransformation : public ExperimentalSetup
{
public:
    // constructors
    ESNoTransformation(int tag,
        const ID &DOF, int sizeT, int sizeO,
        ExperimentalControl* control = 0);
    ESNoTransformation(const ESNoTransformation& es);
    
    // destructor
    virtual ~ESNoTransformation();
    
    // method to get class type
    const char *getClassType() const {return "ESNoTransformation";};
    
    // public methods
    virtual int setup();
    
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
    int numDOF;  // number of DOF
    ID *DOF;     // array of DOF
    int sizeT;   // trial size of disp, vel, accel, force
    int sizeO;   // output size of disp, vel, accel, force
};

#endif
