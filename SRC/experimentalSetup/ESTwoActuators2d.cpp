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
    double actLength0, double actLength1,
    double rigidLength,
    ExperimentalControl* control,
    int nlgeom, double philocx)
    : ExperimentalSetup(tag, control),
    La0(actLength0), La1(actLength1), L(rigidLength),
    nlGeom(nlgeom), phiLocX(philocx), rotLocX(3,3)
{
    // call setup method
    this->setup();
}


ESTwoActuators2d::ESTwoActuators2d(const ESTwoActuators2d& es)
    : ExperimentalSetup(es),
    rotLocX(3,3)
{
    La0     = es.La0;
    La1     = es.La1;
    L       = es.L;
    nlGeom  = es.nlGeom;
    phiLocX = es.phiLocX;

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
    
    // initialize rotation matrix
    rotLocX.Zero();
    double pi = acos(-1.0);
    rotLocX(0,0) = cos(phiLocX/180.0*pi); rotLocX(0,1) = -sin(phiLocX/180.0*pi);
    rotLocX(1,0) = sin(phiLocX/180.0*pi); rotLocX(1,1) =  cos(phiLocX/180.0*pi);
    rotLocX(2,2) = 1.0;

    return OF_ReturnType_completed;
}


int ESTwoActuators2d::transfTrialResponse(const Vector* disp, 
    const Vector* vel,
    const Vector* accel,
    const Vector* force,
    const Vector* time)
{
    // transform data
    if (disp != 0) {
        this->transfTrialDisp(disp);
        for (int i=0; i<(*sizeCtrl)[OF_Resp_Disp]; i++)
            (*cDisp)[i] *= (*cDispFact)[i];
    }
    if (disp != 0 && vel != 0) {
        this->transfTrialVel(disp,vel);
        for (int i=0; i<(*sizeCtrl)[OF_Resp_Vel]; i++)
            (*cVel)[i] *= (*cVelFact)[i];
    }
    if (disp != 0 && vel != 0 && accel != 0) {
        this->transfTrialAccel(disp,vel,accel);
        for (int i=0; i<(*sizeCtrl)[OF_Resp_Accel]; i++)
            (*cAccel)[i] *= (*cAccelFact)[i];
    }
    if (force != 0) {
        this->transfTrialForce(force);
        for (int i=0; i<(*sizeCtrl)[OF_Resp_Force]; i++)
            (*cForce)[i] *= (*cForceFact)[i];
    }
    if (time != 0) {
        this->transfTrialTime(time);
        for (int i=0; i<(*sizeCtrl)[OF_Resp_Time]; i++)
            (*cTime)[i] *= (*cTimeFact)[i];
    }
    
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
    s << " actLength1 : " << La0 << endln;
    s << " actLength2 : " << La1 << endln;
    s << " rigidLength: " << L << endln;
    s << " nlGeom     : " << nlGeom << endln;
    s << " phiLocX    : " << phiLocX << endln;
    if(theControl != 0)  {
        s << "\tExperimentalControl tag: " << theControl->getTag();
        s << *theControl;
    }
}


int ESTwoActuators2d::transfTrialDisp(const Vector* disp)
{  
    // rotate direction
    static Vector d(3);
    d = rotLocX*(*disp);

    // linear geometry
    if (nlGeom == 0)  {
        // actuator 0
        (*cDisp)(0) = d(0);
        // actuator 1
        (*cDisp)(1) = d(0) - L*d(2);
    }
    // nonlinear geometry
    else if (nlGeom == 1)  {
        // actuator 0
        (*cDisp)(0) = d(0);
        // actuator 1
        (*cDisp)(1) = pow(pow(d(0)-L*sin(d(2))+La1,2.0)+pow(L*cos(d(2))-L,2.0),0.5)-La1;
    }
    
    return OF_ReturnType_completed;
}


int ESTwoActuators2d::transfTrialVel(const Vector* vel)
{  
    return OF_ReturnType_completed;
}


int ESTwoActuators2d::transfTrialVel(const Vector* disp,
    const Vector* vel)
{  
    // rotate direction
    static Vector d(3), v(3);
    d = rotLocX*(*disp);
    v = rotLocX*(*vel);

    // linear geometry
    if (nlGeom == 0)  {
        // actuator 0
        (*cVel)(0) = v(0);
        // actuator 1
        (*cVel)(1) = v(0) - L*v(2);
    }
    // nonlinear geometry
    else if (nlGeom == 1)  {
        // actuator 0
        (*cVel)(0) = v(0);
        // actuator 1
        (*cVel)(1) = 0.5*(2.0*(d(0)-L*sin(d(2))+La1)*(v(0)-L*cos(d(2))*v(2))-2.0*(L*cos(d(2))-L)*L*sin(d(2))*v(2))/pow(pow(d(0)-L*sin(d(2))+La1,2.0)+pow(L*cos(d(2))-L,2.0),0.5);
    }
    
    return OF_ReturnType_completed;
}


int ESTwoActuators2d::transfTrialAccel(const Vector* accel)
{
    return OF_ReturnType_completed;
}


int ESTwoActuators2d::transfTrialAccel(const Vector* disp,
    const Vector* vel,
    const Vector* accel)
{  
    // rotate direction
    static Vector d(3), v(3), a(3);
    d = rotLocX*(*disp);
    v = rotLocX*(*vel);
    a = rotLocX*(*accel);

    // linear geometry
    if (nlGeom == 0)  {
        // actuator 0
        (*cAccel)(0) = a(0);
        // actuator 1
        (*cAccel)(1) = a(0) - L*a(2);
    }
    // nonlinear geometry
    else if (nlGeom == 1)  {
        // actuator 0
        (*cAccel)(0) = a(0);
        // actuator 1
        (*cAccel)(1) = -0.25*pow(2.0*(d(0)-L*sin(d(2))+La1)*(v(0)-L*cos(d(2))*v(2))-2.0*(L*cos(d(2))-L)*L*sin(d(2))*v(2),2.0)/pow(pow(d(0)-L*sin(d(2))+La1,2.0)+pow(L*cos(d(2))-L,2.0),1.5)+0.5*(2.0*pow(v(0)-L*cos(d(2))*v(2),2.0)+2.0*(d(0)-L*sin(d(2))+La1)*(a(0)+L*sin(d(2))*pow(v(2),2.0)-L*cos(d(2))*a(2))+2.0*pow(L*sin(d(2))*v(2),2.0)-2.0*(L*cos(d(2))-L)*L*cos(d(2))*pow(v(2),2.0)-2.0*(L*cos(d(2))-L)*L*sin(d(2))*a(2))/pow(pow(d(0)-L*sin(d(2))+La1,2.0)+pow(L*cos(d(2))-L,2.0),0.5);
    }
    
    return OF_ReturnType_completed;
}


int ESTwoActuators2d::transfTrialForce(const Vector* force)
{  
    // rotate direction
    static Vector f(3);
    f = rotLocX*(*force);

    // linear geometry
    if (nlGeom == 0)  {
        // actuator 0
        (*cForce)(0) = f(0) + 1.0/L*f(2);
        // actuator 1
        (*cForce)(1) = -1.0/L*f(2);
    }
    // nonlinear geometry
    else if (nlGeom == 1)  {
        if (firstWarning[0] == true)  {
            opserr << "ESTwoActuators2d::transfTrialForce() - "
                << "nonlinear geometry not implemented yet. "
                << "Using linear geometry instead.\n\n";
            firstWarning[0] = false;
        }
        // actuator 0
        (*cForce)(0) = f(0) + 1.0/L*f(2);
        // actuator 1
        (*cForce)(1) = -1.0/L*f(2);
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
    if (nlGeom == 0)  {
        (*disp)(0) = (*dDisp)(0);
        (*disp)(1) = 0.0;
        (*disp)(2) = 1.0/L*((*dDisp)(0) - (*dDisp)(1));
    }
    // nonlinear geometry
    else if (nlGeom == 1)  {
        double d0 = La0 + (*dDisp)(0);
        double d1 = La1 + (*dDisp)(1);

        (*disp)(0) = (*dDisp)(0);
        (*disp)(1) = 0.0;
        (*disp)(2) = atan(d0/L) - acos((d1*d1-2*L*L-d0*d0)/(-2*L*pow(L*L+d0*d0,0.5)));
    }

    // rotate direction if necessary
    if (phiLocX != 0.0)  {
        (*disp) = rotLocX^(*disp);
    }
    
    return OF_ReturnType_completed;
}


int ESTwoActuators2d::transfDaqVel(Vector* vel)
{
    // linear geometry
    if (nlGeom == 0)  {
        (*vel)(0) = (*dVel)(0);
        (*vel)(1) = 0.0;
        (*vel)(2) = 1.0/L*((*dVel)(0) - (*dVel)(1));
    }
    // nonlinear geometry
    else if (nlGeom == 1)  {
        if (firstWarning[1] == true)  {
            opserr << "ESTwoActuators2d::transfDaqVel() - "
                << "nonlinear geometry not implemented yet. "
                << "Using linear geometry instead.\n\n";
            firstWarning[1] = false;
        }
        (*vel)(0) = (*dVel)(0);
        (*vel)(1) = 0.0;
        (*vel)(2) = 1.0/L*((*dVel)(0) - (*dVel)(1));
    }
    
    // rotate direction if necessary
    if (phiLocX != 0.0)  {
        (*vel) = rotLocX^(*vel);
    }

    return OF_ReturnType_completed;
}


int ESTwoActuators2d::transfDaqAccel(Vector* accel)
{
    // linear geometry
    if (nlGeom == 0)  {
        (*accel)(0) = (*dAccel)(0);
        (*accel)(1) = 0.0;
        (*accel)(2) = 1.0/L*((*dAccel)(0) - (*dAccel)(1));
    }
    // nonlinear geometry
    else if (nlGeom == 1)  {
        if (firstWarning[2] == true)  {
            opserr << "ESTwoActuators2d::transfDaqAccel() - "
                << "nonlinear geometry not implemented yet. "
                << "Using linear geometry instead.\n\n";
            firstWarning[2] = false;
        }
        (*accel)(0) = (*dAccel)(0);
        (*accel)(1) = 0.0;
        (*accel)(2) = 1.0/L*((*dAccel)(0) - (*dAccel)(1));
    }
    
    // rotate direction if necessary
    if (phiLocX != 0.0)  {
        (*accel) = rotLocX^(*accel);
    }

    return OF_ReturnType_completed;
}


int ESTwoActuators2d::transfDaqForce(Vector* force)
{
    // linear geometry
    if (nlGeom == 0)  {
        (*force)(0) = (*dForce)(0) + (*dForce)(1);
        (*force)(1) = 0.0;
        (*force)(2) = -L*(*dForce)(1);
    }
    // nonlinear geometry
    else if (nlGeom == 1)  {
        double d0 = La0 + (*dDisp)(0);
        double d1 = La1 + (*dDisp)(1);

        double disp2 = atan(d0/L) - acos((d1*d1-2*L*L-d0*d0)/(-2*L*pow(L*L+d0*d0,0.5)));
        double theta1 = asin(L*(1.0-cos(disp2))/d1);
        
        double fx1 = (*dForce)(1)*cos(theta1);
        double fy1 = (*dForce)(1)*sin(theta1);

        (*force)(0) = (*dForce)(0) + fx1;
        (*force)(1) = 0.0;
        (*force)(2) = -fx1*L*cos(disp2) - fy1*L*sin(disp2);
    }
    
    // rotate direction if necessary
    if (phiLocX != 0.0)  {
        (*force) = rotLocX^(*force);
    }

    return OF_ReturnType_completed;
}


int ESTwoActuators2d::transfDaqTime(Vector* time)
{  
    *time = *dTime;
    
    return OF_ReturnType_completed;
}