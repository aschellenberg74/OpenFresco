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

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 09/06
// Revision: A
//
// Description: This file contains the implementation of the
// ESTwoActuators2d class.

#include "ESTwoActuators2d.h"

#include <math.h>


ESTwoActuators2d::ESTwoActuators2d(int tag,
    int nlGeomFlag,
    double actLength0, double actLength1,
    double rigidLength,
    ExperimentalControl* control)
    : ExperimentalSetup(tag, control), nlFlag(nlGeomFlag),
    La0(actLength0), La1(actLength1), L(rigidLength)
{
    // call setup method
    this->setup();
}


ESTwoActuators2d::ESTwoActuators2d(const ESTwoActuators2d& es)
    : ExperimentalSetup(es)
{
    nlFlag = es.nlFlag;
    La0    = es.La0;
    La1    = es.La1;
    L      = es.L;

    // call setup method
    this->setup();
}


ESTwoActuators2d::~ESTwoActuators2d()
{
    // does nothing
}


int ESTwoActuators2d::setSize(ID sizeT, ID sizeO)
{
    // check sizeTrial and sizeOut
    // for ESTwoActuators2d object
    
    // a component of sizeT/sizeO must be equal
    // to 3 if it is non-zero.
    
    int i;
    for(i=0; i<OF_Resp_Time; i++) {
        if((sizeT[i] != 0 && sizeT[i] != 3) ||
            (sizeO[i] != 0 && sizeO[i] != 3)) {
            opserr << "ESTwoActuators2d::setSize - wrong sizeTrial/Out\n"; 
            opserr << "see User Manual.\n";
            opserr << "sizeT = " << sizeT;
            opserr << "sizeO = " << sizeO;
            return OF_ReturnType_failed;
        }
    }
    if((sizeT[OF_Resp_Time] != 0 && sizeT[OF_Resp_Time] != 1) ||
        (sizeO[OF_Resp_Time] != 0 && sizeO[OF_Resp_Time] != 1)) {
        opserr << "ESTwoActuators2d::setSize - wrong sizeTrial/Out\n"; 
        opserr << "see User Manual.\n";
        opserr << "sizeT = " << sizeT;
        opserr << "sizeO = " << sizeO;
        return OF_ReturnType_failed;
    }
    
    return OF_ReturnType_completed;
}


int ESTwoActuators2d::commitState()
{
    return theControl->commitState();
}


int ESTwoActuators2d::setup()
{
    // setup for ctrl/daq vectors of ESTwoActuators2d
    sizeCtrl->Zero();
    sizeDaq->Zero();
    for(int i=0; i<OF_Resp_Time; i++) {
        (*sizeCtrl)[i] = 2;
        (*sizeDaq)[i] = 2;
    }
    (*sizeCtrl)[OF_Resp_Time] = 1;
    (*sizeDaq)[OF_Resp_Time] = 1;
    
    this->setCtrlDaqSize();
    
    return OF_ReturnType_completed;
}


ExperimentalSetup* ESTwoActuators2d::getCopy()
{
    ESTwoActuators2d *theCopy = new ESTwoActuators2d(*this);
    
    return theCopy;
}


void ESTwoActuators2d::Print(OPS_Stream &s, int flag)
{
    s << "ExperimentalSetup: " << this->getTag(); 
    s << " type: ESTwoActuators2d\n";
    s << " nlGeomFlag : " << nlFlag << endln;
    s << " actLength0 : " << La0 << endln;
    s << " actLength1 : " << La1 << endln;
    s << " rigidLength: " << L << endln;
    if(theControl != 0)  {
        s << "\tExperimentalControl tag: " << theControl->getTag();
        s << *theControl;
    }
}


int ESTwoActuators2d::transfTrialDisp(const Vector* disp)
{  
    // linear geometry
    if (nlFlag == 0)  {
        // actuator 0
        (*cDisp)(0) = -(*disp)(1);
        // actuator 1
        (*cDisp)(1) = -(*disp)(1) - L*(*disp)(2);
    }
    // nonlinear geometry
    else if (nlFlag == 1)  {
        opserr << "ESTwoActuators2d::transfTrialDisp() - "
            << "nonlinear geometry not implemented yet";
        return OF_ReturnType_failed;
    }
    
    return OF_ReturnType_completed;
}


int ESTwoActuators2d::transfTrialVel(const Vector* vel)
{  
    // linear geometry
    if (nlFlag == 0)  {
        // actuator 0
        (*cVel)(0) = -(*vel)(1);
        // actuator 1
        (*cVel)(1) = -(*vel)(1) - L*(*vel)(2);
    }
    // nonlinear geometry
    else if (nlFlag == 1)  {
        opserr << "ESTwoActuators2d::transfTrialVel() - "
            << "nonlinear geometry not implemented yet";
        return OF_ReturnType_failed;
    }
    
    return OF_ReturnType_completed;
}


int ESTwoActuators2d::transfTrialAccel(const Vector* accel)
{  
    // linear geometry
    if (nlFlag == 0)  {
        // actuator 0
        (*cAccel)(0) = -(*accel)(1);
        // actuator 1
        (*cAccel)(1) = -(*accel)(1) - L*(*accel)(2);
    }
    // nonlinear geometry
    else if (nlFlag == 1)  {
        opserr << "ESTwoActuators2d::transfTrialAccel() - "
            << "nonlinear geometry not implemented yet";
        return OF_ReturnType_failed;
    }
    
    return OF_ReturnType_completed;
}


int ESTwoActuators2d::transfTrialForce(const Vector* force)
{  
    // linear geometry
    if (nlFlag == 0)  {
        // actuator 0
        (*cForce)(0) = -(*force)(1) + 1.0/L*(*force)(2);
        // actuator 1
        (*cForce)(1) = -1.0/L*(*force)(2);
    }
    // nonlinear geometry
    else if (nlFlag == 1)  {
        opserr << "ESTwoActuators2d::transfTrialForce() - "
            << "nonlinear geometry not implemented yet";
        return OF_ReturnType_failed;
    }
    
    return OF_ReturnType_completed;
}


int ESTwoActuators2d::transfTrialTime(const Vector* time)
{
    *cTime = *time;
    
    return OF_ReturnType_completed;
}


int ESTwoActuators2d::transfDaqDisp(Vector* disp)
{
    // linear geometry
    if (nlFlag == 0)  {
        (*disp)(0) = 0.0;
        (*disp)(1) = -(*dDisp)(0);
        (*disp)(2) = 1.0/L*((*dDisp)(0) - (*dDisp)(1));
    }
    // nonlinear geometry
    else if (nlFlag == 1)  {
        opserr << "ESTwoActuators2d::transfDaqDisp() - "
            << "nonlinear geometry not implemented yet";
        return OF_ReturnType_failed;
    }
    
    return OF_ReturnType_completed;
}


int ESTwoActuators2d::transfDaqVel(Vector* vel)
{
    // linear geometry
    if (nlFlag == 0)  {
        (*vel)(0) = 0.0;
        (*vel)(1) = -(*dVel)(0);
        (*vel)(2) = 1.0/L*((*dVel)(0) - (*dVel)(1));
    }
    // nonlinear geometry
    else if (nlFlag == 1)  {
        opserr << "ESTwoActuators2d::transfDaqVel() - "
            << "nonlinear geometry not implemented yet";
        return OF_ReturnType_failed;
    }
    
    return OF_ReturnType_completed;
}


int ESTwoActuators2d::transfDaqAccel(Vector* accel)
{
    // linear geometry
    if (nlFlag == 0)  {
        (*accel)(0) = 0.0;
        (*accel)(1) = -(*dAccel)(0);
        (*accel)(2) = 1.0/L*((*dAccel)(0) - (*dAccel)(1));
    }
    // nonlinear geometry
    else if (nlFlag == 1)  {
        opserr << "ESTwoActuators2d::transfDaqAccel() - "
            << "nonlinear geometry not implemented yet";
        return OF_ReturnType_failed;
    }
    
    return OF_ReturnType_completed;
}


int ESTwoActuators2d::transfDaqForce(Vector* force)
{
    // linear geometry
    if (nlFlag == 0)  {
        (*force)(0) = 0.0;
        (*force)(1) = -(*dForce)(0) - (*dForce)(1);
        (*force)(2) = -L*(*dForce)(1);
    }
    // nonlinear geometry
    else if (nlFlag == 1)  {
        opserr << "ESTwoActuators2d::transfDaqForce() - "
            << "nonlinear geometry not implemented yet";
        return OF_ReturnType_failed;
    }
    
    return OF_ReturnType_completed;
}


int ESTwoActuators2d::transfDaqTime(Vector* time)
{  
    *time = *dTime;
    
    return OF_ReturnType_completed;
}
