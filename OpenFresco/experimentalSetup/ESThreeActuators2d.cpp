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
// $Source$

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 09/06
// Revision: A
//
// Description: This file contains the implementation of the
// ESThreeActuators2d class.

#include "ESThreeActuators2d.h"

#include <math.h>


ESThreeActuators2d::ESThreeActuators2d(int tag,
    int nlGeomFlag,
    double actLength0, double actLength1, double actLength2,
    double rigidLength0, double rigidLength1,
    ExperimentalControl* control)
    : ExperimentalSetup(tag, control), nlFlag(nlGeomFlag),
    La0(actLength0), La1(actLength1), La2(actLength2),
    L0(rigidLength0), L1(rigidLength1)
{
    // call setup method
    this->setup();
}


ESThreeActuators2d::ESThreeActuators2d(const ESThreeActuators2d& es)
    : ExperimentalSetup(es)
{
    nlFlag = es.nlFlag;
    La0    = es.La0;
    La1    = es.La1;
    La2    = es.La2;
    L0     = es.L0;
    L1     = es.L1;

    // call setup method
    this->setup();
}


ESThreeActuators2d::~ESThreeActuators2d()
{
	// does nothing
}


int ESThreeActuators2d::setSize(ID sizeT, ID sizeO)
{
    // check sizeTrial and sizeOut
    // for ESThreeActuators2d object
    
    // a component of sizeT/sizeO must be equal
    // to 3 if it is non-zero.
    
    int i;
    for(i=0; i<OF_Resp_Time; i++) {
        if((sizeT[i] != 0 && sizeT[i] != 3) ||
            (sizeO[i] != 0 && sizeO[i] != 3)) {
            opserr << "ESThreeActuators2d::setSize - wrong sizeTrial/Out\n"; 
            opserr << "see User Manual.\n";
            opserr << "sizeT = " << sizeT;
            opserr << "sizeO = " << sizeO;
            return OF_ReturnType_failed;
        }
    }
    if((sizeT[OF_Resp_Time] != 0 && sizeT[OF_Resp_Time] != 1) ||
        (sizeO[OF_Resp_Time] != 0 && sizeO[OF_Resp_Time] != 1)) {
        opserr << "ESThreeActuators2d::setSize - wrong sizeTrial/Out\n"; 
        opserr << "see User Manual.\n";
        opserr << "sizeT = " << sizeT;
        opserr << "sizeO = " << sizeO;
        return OF_ReturnType_failed;
    }
    
    return OF_ReturnType_completed;
}


int ESThreeActuators2d::commitState()
{
    return theControl->commitState();
}


int ESThreeActuators2d::setup()
{
    // setup for ctrl/daq vectors of ESThreeActuators2d
    sizeCtrl->Zero();
    sizeDaq->Zero();
    for(int i=0; i<OF_Resp_Time; i++) {
        (*sizeCtrl)[i] = 3;
        (*sizeDaq)[i] = 3;
    }
    (*sizeCtrl)[OF_Resp_Time] = 1;
    (*sizeDaq)[OF_Resp_Time] = 1;
    
    this->setCtrlDaqSize();
    
    return OF_ReturnType_completed;
}


ExperimentalSetup* ESThreeActuators2d::getCopy()
{
	ESThreeActuators2d *theCopy = new ESThreeActuators2d(*this);
	
	return theCopy;
}


void ESThreeActuators2d::Print(OPS_Stream &s, int flag)
{
	s << "ExperimentalSetup: " << this->getTag(); 
	s << " type: ESThreeActuators2d\n";
    s << " nlGeomFlag  : " << nlFlag << endln;
    s << " actLength0  : " << La0 << endln;
    s << " actLength1  : " << La1 << endln;
    s << " actLength2  : " << La2 << endln;
	s << " rigidLength0: " << L0 << endln;
	s << " rigidLength1: " << L1 << endln;
    if(theControl != 0)  {
        s << "\tExperimentalControl tag: " << theControl->getTag();
        s << *theControl;
    }
}


int ESThreeActuators2d::transfTrialDisp(const Vector* disp)
{  
    // linear geometry, horizontal actuator left
    if (nlFlag == 0)  {
        // actuator 0
        (*cDisp)(0) = -(*disp)(1);
        // actuator 1
        (*cDisp)(1) = (*disp)(0) - L0*(*disp)(2);
        // actuator 2
        (*cDisp)(2) = (*disp)(0) + L1*(*disp)(2);
    }
    // linear geometry, horizontal actuator right
    else if (nlFlag == 1)  {
        // actuator 0
        (*cDisp)(0) = (*disp)(1);
        // actuator 1
        (*cDisp)(1) = (*disp)(0) - L0*(*disp)(2);
        // actuator 2
        (*cDisp)(2) = (*disp)(0) + L1*(*disp)(2);
    }
    // nonlinear geometry, horizontal actuator left
    else if (nlFlag == 2)  {
        opserr << "ESThreeActuators2d::transfTrialDisp() - "
            << "nonlinear geometry with horizontal actuator left "
            << "not implemented yet";
        return OF_ReturnType_failed;
    }
    // nonlinear geometry, horizontal actuator right
    else if (nlFlag == 3)  {
        opserr << "ESThreeActuators2d::transfTrialDisp() - "
            << "nonlinear geometry with horizontal actuator right "
            << "not implemented yet";
        return OF_ReturnType_failed;
    }
    
    return OF_ReturnType_completed;
}


int ESThreeActuators2d::transfTrialVel(const Vector* vel)
{  
    // linear geometry, horizontal actuator left
    if (nlFlag == 0)  {
        // actuator 0
        (*cVel)(0) = -(*vel)(1);
        // actuator 1
        (*cVel)(1) = (*vel)(0) - L0*(*vel)(2);
        // actuator 2
        (*cVel)(2) = (*vel)(0) + L1*(*vel)(2);
    }
    // linear geometry, horizontal actuator right
    else if (nlFlag == 1)  {
        // actuator 0
        (*cVel)(0) = (*vel)(1);
        // actuator 1
        (*cVel)(1) = (*vel)(0) - L0*(*vel)(2);
        // actuator 2
        (*cVel)(2) = (*vel)(0) + L1*(*vel)(2);
    }
    // nonlinear geometry, horizontal actuator left
    else if (nlFlag == 2)  {
        opserr << "ESThreeActuators2d::transfTrialVel() - "
            << "nonlinear geometry with horizontal actuator left "
            << "not implemented yet";
        return OF_ReturnType_failed;
    }
    // nonlinear geometry, horizontal actuator right
    else if (nlFlag == 3)  {
        opserr << "ESThreeActuators2d::transfTrialVel() - "
            << "nonlinear geometry with horizontal actuator right "
            << "not implemented yet";
        return OF_ReturnType_failed;
    }
    
    return OF_ReturnType_completed;
}


int ESThreeActuators2d::transfTrialAccel(const Vector* accel)
{  
    // linear geometry, horizontal actuator left
    if (nlFlag == 0)  {
        // actuator 0
        (*cAccel)(0) = -(*accel)(1);
        // actuator 1
        (*cAccel)(1) = (*accel)(0) - L0*(*accel)(2);
        // actuator 2
        (*cAccel)(2) = (*accel)(0) + L1*(*accel)(2);
    }
    // linear geometry, horizontal actuator right
    else if (nlFlag == 1)  {
        // actuator 0
        (*cAccel)(0) = (*accel)(1);
        // actuator 1
        (*cAccel)(1) = (*accel)(0) - L0*(*accel)(2);
        // actuator 2
        (*cAccel)(2) = (*accel)(0) + L1*(*accel)(2);
    }
    // nonlinear geometry, horizontal actuator left
    else if (nlFlag == 2)  {
        opserr << "ESThreeActuators2d::transfTrialAccel() - "
            << "nonlinear geometry with horizontal actuator left "
            << "not implemented yet";
        return OF_ReturnType_failed;
    }
    // nonlinear geometry, horizontal actuator right
    else if (nlFlag == 3)  {
        opserr << "ESThreeActuators2d::transfTrialAccel() - "
            << "nonlinear geometry with horizontal actuator right "
            << "not implemented yet";
        return OF_ReturnType_failed;
    }
    
    return OF_ReturnType_completed;
}


int ESThreeActuators2d::transfTrialForce(const Vector* force)
{  
    // linear geometry, horizontal actuator left
    if (nlFlag == 0)  {
        // actuator 0
        (*cForce)(0) = -(*force)(1);
        // actuator 1
        (*cForce)(1) = 1.0/(L0+L1)*(L1*(*force)(0) - (*force)(2));
        // actuator 2
        (*cForce)(2) = 1.0/(L0+L1)*(L0*(*force)(0) + (*force)(2));
    }
    // linear geometry, horizontal actuator right
    else if (nlFlag == 1)  {
        // actuator 0
        (*cForce)(0) = (*force)(1);
        // actuator 1
        (*cForce)(1) = 1.0/(L0+L1)*(L1*(*force)(0) - (*force)(2));
        // actuator 2
        (*cForce)(2) = 1.0/(L0+L1)*(L0*(*force)(0) + (*force)(2));
    }
    // nonlinear geometry, horizontal actuator left
    else if (nlFlag == 2)  {
        opserr << "ESThreeActuators2d::transfTrialForce() - "
            << "nonlinear geometry with horizontal actuator left "
            << "not implemented yet";
        return OF_ReturnType_failed;
    }
    // nonlinear geometry, horizontal actuator right
    else if (nlFlag == 3)  {
        opserr << "ESThreeActuators2d::transfTrialForce() - "
            << "nonlinear geometry with horizontal actuator right "
            << "not implemented yet";
        return OF_ReturnType_failed;
    }
    
    return OF_ReturnType_completed;
}


int ESThreeActuators2d::transfTrialTime(const Vector* time)
{
    *cTime = *time;
    
    return OF_ReturnType_completed;
}


int ESThreeActuators2d::transfDaqDisp(Vector* disp)
{
    // linear geometry, horizontal actuator left
    if (nlFlag == 0)  {
        (*disp)(0) = 1.0/(L0+L1)*(L1*(*dDisp)(1) + L0*(*dDisp)(2));
        (*disp)(1) = -(*dDisp)(0);
        (*disp)(2) = 1.0/(L0+L1)*(-(*dDisp)(1) + (*dDisp)(2));
    }
    // linear geometry, horizontal actuator right
    else if (nlFlag == 1)  {
        (*disp)(0) = 1.0/(L0+L1)*(L1*(*dDisp)(1) + L0*(*dDisp)(2));
        (*disp)(1) = (*dDisp)(0);
        (*disp)(2) = 1.0/(L0+L1)*(-(*dDisp)(1) + (*dDisp)(2));
    }
    // nonlinear geometry, horizontal actuator left
    else if (nlFlag == 2)  {
        opserr << "ESThreeActuators2d::transfDaqDisp() - "
            << "nonlinear geometry with horizontal actuator left "
            << "not implemented yet";
        return OF_ReturnType_failed;
    }
    // nonlinear geometry, horizontal actuator right
    else if (nlFlag == 3)  {
        opserr << "ESThreeActuators2d::transfDaqDisp() - "
            << "nonlinear geometry with horizontal actuator right "
            << "not implemented yet";
        return OF_ReturnType_failed;
    }
    
    return OF_ReturnType_completed;
}


int ESThreeActuators2d::transfDaqVel(Vector* vel)
{
    // linear geometry, horizontal actuator left
    if (nlFlag == 0)  {
        (*vel)(0) = 1.0/(L0+L1)*(L1*(*dVel)(1) + L0*(*dVel)(2));
        (*vel)(1) = -(*dVel)(0);
        (*vel)(2) = 1.0/(L0+L1)*(-(*dVel)(1) + (*dVel)(2));
    }
    // linear geometry, horizontal actuator right
    else if (nlFlag == 1)  {
        (*vel)(0) = 1.0/(L0+L1)*(L1*(*dVel)(1) + L0*(*dVel)(2));
        (*vel)(1) = (*dVel)(0);
        (*vel)(2) = 1.0/(L0+L1)*(-(*dVel)(1) + (*dVel)(2));
    }
    // nonlinear geometry, horizontal actuator left
    else if (nlFlag == 2)  {
        opserr << "ESThreeActuators2d::transfDaqVel() - "
            << "nonlinear geometry with horizontal actuator left "
            << "not implemented yet";
        return OF_ReturnType_failed;
    }
    // nonlinear geometry, horizontal actuator right
    else if (nlFlag == 3)  {
        opserr << "ESThreeActuators2d::transfDaqVel() - "
            << "nonlinear geometry with horizontal actuator right "
            << "not implemented yet";
        return OF_ReturnType_failed;
    }
    
    return OF_ReturnType_completed;
}


int ESThreeActuators2d::transfDaqAccel(Vector* accel)
{
    // linear geometry, horizontal actuator left
    if (nlFlag == 0)  {
        (*accel)(0) = 1.0/(L0+L1)*(L1*(*dAccel)(1) + L0*(*dAccel)(2));
        (*accel)(1) = -(*dAccel)(0);
        (*accel)(2) = 1.0/(L0+L1)*(-(*dAccel)(1) + (*dAccel)(2));
    }
    // linear geometry, horizontal actuator right
    else if (nlFlag == 1)  {
        (*accel)(0) = 1.0/(L0+L1)*(L1*(*dAccel)(1) + L0*(*dAccel)(2));
        (*accel)(1) = (*dAccel)(0);
        (*accel)(2) = 1.0/(L0+L1)*(-(*dAccel)(1) + (*dAccel)(2));
    }
    // nonlinear geometry, horizontal actuator left
    else if (nlFlag == 2)  {
        opserr << "ESThreeActuators2d::transfDaqAccel() - "
            << "nonlinear geometry with horizontal actuator left "
            << "not implemented yet";
        return OF_ReturnType_failed;
    }
    // nonlinear geometry, horizontal actuator right
    else if (nlFlag == 3)  {
        opserr << "ESThreeActuators2d::transfDaqAccel() - "
            << "nonlinear geometry with horizontal actuator right "
            << "not implemented yet";
        return OF_ReturnType_failed;
    }
    
    return OF_ReturnType_completed;
}


int ESThreeActuators2d::transfDaqForce(Vector* force)
{
    // linear geometry, horizontal actuator left
    if (nlFlag == 0)  {
        (*force)(0) = (*dForce)(1) + (*dForce)(2);
        (*force)(1) = -(*dForce)(0);
        (*force)(2) = -L0*(*dForce)(1) + L1*(*dForce)(2);
    }
    // linear geometry, horizontal actuator right
    else if (nlFlag == 1)  {
        (*force)(0) = (*dForce)(1) + (*dForce)(2);
        (*force)(1) = (*dForce)(0);
        (*force)(2) = -L0*(*dForce)(1) + L1*(*dForce)(2);
    }
    // nonlinear geometry, horizontal actuator left
    else if (nlFlag == 2)  {
        opserr << "ESThreeActuators2d::transfDaqForce() - "
            << "nonlinear geometry with horizontal actuator left "
            << "not implemented yet";
        return OF_ReturnType_failed;
    }
    // nonlinear geometry, horizontal actuator right
    else if (nlFlag == 3)  {
        opserr << "ESThreeActuators2d::transfDaqForce() - "
            << "nonlinear geometry with horizontal actuator right "
            << "not implemented yet";
        return OF_ReturnType_failed;
    }
    
    return OF_ReturnType_completed;
}


int ESThreeActuators2d::transfDaqTime(Vector* time)
{  
    *time = *dTime;
    
    return OF_ReturnType_completed;
}
