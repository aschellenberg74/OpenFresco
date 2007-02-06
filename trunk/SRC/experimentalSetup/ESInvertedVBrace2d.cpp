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
// ESInvertedVBrace2d class.

#include "ESInvertedVBrace2d.h"

#include <math.h>


ESInvertedVBrace2d::ESInvertedVBrace2d(int tag,
    double actLength0, double actLength1, double actLength2,
    double rigidLength0, double rigidLength1,
    ExperimentalControl* control,
    int nlgeom, char *posact0, double philocx)
    : ExperimentalSetup(tag, control),
    La0(actLength0), La1(actLength1), La2(actLength2),
    L0(rigidLength0), L1(rigidLength1),
    nlGeom(nlgeom), phiLocX(philocx), rotLocX(3,3)
{
    strcpy(posAct0,posact0);

    // call setup method
    this->setup();

    for (int i=0; i<3; i++)
        firstWarning[i] = true;
}


ESInvertedVBrace2d::ESInvertedVBrace2d(const ESInvertedVBrace2d& es)
    : ExperimentalSetup(es),
    rotLocX(3,3)
{
    La0     = es.La0;
    La1     = es.La1;
    La2     = es.La2;
    L0      = es.L0;
    L1      = es.L1;
    nlGeom  = es.nlGeom;
    phiLocX = es.phiLocX;
    strcpy(posAct0,es.posAct0);

    // call setup method
    this->setup();

    for (int i=0; i<3; i++)
        firstWarning[i] = true;
}


ESInvertedVBrace2d::~ESInvertedVBrace2d()
{
    // does nothing
}


int ESInvertedVBrace2d::setSize(ID sizeT, ID sizeO)
{
    // check sizeTrial and sizeOut
    // for ESInvertedVBrace2d object
    
    // a component of sizeT must be equal to 3
    // and a component of sizeO must be equal
    // to 6 if it is non-zero.
    
    int i;
    for(i=0; i<OF_Resp_Force; i++) {
        if((sizeT[i] != 0 && sizeT[i] != 3) ||
            (sizeO[i] != 0 && sizeO[i] != 3)) {
            opserr << "ESInvertedVBrace2d::setSize - wrong sizeTrial/Out\n"; 
            opserr << "see User Manual.\n";
            opserr << "sizeT = " << sizeT;
            opserr << "sizeO = " << sizeO;
            return OF_ReturnType_failed;
        }
    }
    if((sizeT[OF_Resp_Force] != 0 && sizeT[OF_Resp_Force] != 3) ||
        (sizeO[OF_Resp_Force] != 0 && sizeO[OF_Resp_Force] != 6)) {
        opserr << "ESInvertedVBrace2d::setSize - wrong sizeTrial/Out\n"; 
        opserr << "see User Manual.\n";
        opserr << "sizeT = " << sizeT;
        opserr << "sizeO = " << sizeO;
        return OF_ReturnType_failed;
    }
    if((sizeT[OF_Resp_Time] != 0 && sizeT[OF_Resp_Time] != 1) ||
        (sizeO[OF_Resp_Time] != 0 && sizeO[OF_Resp_Time] != 1)) {
        opserr << "ESInvertedVBrace2d::setSize - wrong sizeTrial/Out\n"; 
        opserr << "see User Manual.\n";
        opserr << "sizeT = " << sizeT;
        opserr << "sizeO = " << sizeO;
        return OF_ReturnType_failed;
    }
    
    return OF_ReturnType_completed;
}


int ESInvertedVBrace2d::commitState()
{
    return theControl->commitState();
}


int ESInvertedVBrace2d::setup()
{
    // setup for ctrl/daq vectors of ESInvertedVBrace2d
    sizeCtrl->Zero();
    sizeDaq->Zero();
    for(int i=0; i<OF_Resp_Force; i++) {
        (*sizeCtrl)[i] = 3;
        (*sizeDaq)[i] = 3;
    }
    (*sizeCtrl)[OF_Resp_Force] = 3;
    (*sizeDaq)[OF_Resp_Force] = 6;
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


int ESInvertedVBrace2d::transfTrialResponse(const Vector* disp, 
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


ExperimentalSetup* ESInvertedVBrace2d::getCopy()
{
    ESInvertedVBrace2d *theCopy = new ESInvertedVBrace2d(*this);
    
    return theCopy;
}


void ESInvertedVBrace2d::Print(OPS_Stream &s, int flag)
{
    s << "ExperimentalSetup: " << this->getTag(); 
    s << " type: ESInvertedVBrace2d\n";
    s << " actLength1  : " << La0 << endln;
    s << " actLength2  : " << La1 << endln;
    s << " actLength3  : " << La2 << endln;
    s << " rigidLength1: " << L0 << endln;
    s << " rigidLength2: " << L1 << endln;
    s << " nlGeom      : " << nlGeom << endln;
    s << " posAct1     : " << posAct0 << endln;
    s << " phiLocX     : " << phiLocX << endln;
    if(theControl != 0)  {
        s << "\tExperimentalControl tag: " << theControl->getTag();
        s << *theControl;
    }
}


int ESInvertedVBrace2d::transfTrialDisp(const Vector* disp)
{  
    // rotate direction
    static Vector d(3);
    d = rotLocX*(*disp);

    // linear geometry, horizontal actuator left
    if (nlGeom == 0 && strcmp(posAct0,"left") == 0)  {
        // actuator 0
        (*cDisp)(0) = d(0);
        // actuator 1
        (*cDisp)(1) = d(1) - L0*d(2);
        // actuator 2
        (*cDisp)(2) = d(1) + L1*d(2);
    }
    // linear geometry, horizontal actuator right
    else if (nlGeom == 0 && strcmp(posAct0,"right") == 0)  {
        // actuator 0
        (*cDisp)(0) = -d(0);
        // actuator 1
        (*cDisp)(1) = d(1) - L0*d(2);
        // actuator 2
        (*cDisp)(2) = d(1) + L1*d(2);
    }
    // nonlinear geometry, horizontal actuator left
    else if (nlGeom == 1 && strcmp(posAct0,"left") == 0)  {
        // actuator 0
        (*cDisp)(0) = pow(pow(d(0)+L0*(1.0-cos(d(2)))+La0,2.0)+pow(d(1)-L0*sin(d(2)),2.0),0.5)-La0;
        // actuator 1
        (*cDisp)(1) = pow(pow(d(0)+L0*(1.0-cos(d(2))),2.0)+pow(d(1)-L0*sin(d(2))+La1,2.0),0.5)-La1;
        // actuator 2
        (*cDisp)(2) = pow(pow(d(0)-L1*(1.0-cos(d(2))),2.0)+pow(d(1)+L1*sin(d(2))+La2,2.0),0.5)-La2;
    }
    // nonlinear geometry, horizontal actuator right
    else if (nlGeom == 1 && strcmp(posAct0,"right") == 0)  {
        // actuator 0
        (*cDisp)(0) = pow(pow(d(0)-L1*(1.0-cos(d(2)))-La0,2.0)+pow(d(1)+L1*sin(d(2)),2.0),0.5)-La0;
        // actuator 1
        (*cDisp)(1) = pow(pow(d(0)+L0*(1.0-cos(d(2))),2.0)+pow(d(1)-L0*sin(d(2))+La1,2.0),0.5)-La1;
        // actuator 2
        (*cDisp)(2) = pow(pow(d(0)-L1*(1.0-cos(d(2))),2.0)+pow(d(1)+L1*sin(d(2))+La2,2.0),0.5)-La2;        
    }
    
    return OF_ReturnType_completed;
}


int ESInvertedVBrace2d::transfTrialVel(const Vector* vel)
{
    return OF_ReturnType_completed;
}


int ESInvertedVBrace2d::transfTrialVel(const Vector* disp,
    const Vector* vel)
{  
    // rotate direction
    static Vector d(3), v(3);
    d = rotLocX*(*disp);
    v = rotLocX*(*vel);

    // linear geometry, horizontal actuator left
    if (nlGeom == 0 && strcmp(posAct0,"left") == 0)  {
        // actuator 0
        (*cVel)(0) = v(0);
        // actuator 1
        (*cVel)(1) = v(1) - L0*v(2);
        // actuator 2
        (*cVel)(2) = v(1) + L1*v(2);
    }
    // linear geometry, horizontal actuator right
    else if (nlGeom == 0 && strcmp(posAct0,"right") == 0)  {
        // actuator 0
        (*cVel)(0) = -v(0);
        // actuator 1
        (*cVel)(1) = v(1) - L0*v(2);
        // actuator 2
        (*cVel)(2) = v(1) + L1*v(2);
    }
    // nonlinear geometry, horizontal actuator left
    else if (nlGeom == 1 && strcmp(posAct0,"left") == 0)  {
        // actuator 0
        (*cVel)(0) = 0.5*(2.0*(d(0)+L0*(1.0-cos(d(2)))+La0)*(v(0)+L0*sin(d(2))*v(2))+2.0*(d(1)-L0*sin(d(2)))*(v(1)-L0*cos(d(2))*v(2)))/pow(pow(d(0)+L0*(1.0-cos(d(2)))+La0,2.0)+pow(d(1)-L0*sin(d(2)),2.0),0.5);
        // actuator 1
        (*cVel)(1) = 0.5*(2.0*(d(0)+L0*(1.0-cos(d(2))))*(v(0)+L0*sin(d(2))*v(2))+2.0*(d(1)-L0*sin(d(2))+La1)*(v(1)-L0*cos(d(2))*v(2)))/pow(pow(d(0)+L0*(1.0-cos(d(2))),2.0)+pow(d(1)-L0*sin(d(2))+La1,2.0),0.5);
        // actuator 2
        (*cVel)(2) = 0.5*(2.0*(d(0)-L1*(1.0-cos(d(2))))*(v(0)-L1*sin(d(2))*v(2))+2.0*(d(1)+L1*sin(d(2))+La2)*(v(1)+L1*cos(d(2))*v(2)))/pow(pow(d(0)-L1*(1.0-cos(d(2))),2.0)+pow(d(1)+L1*sin(d(2))+La2,2.0),0.5);
    }
    // nonlinear geometry, horizontal actuator right
    else if (nlGeom == 1 && strcmp(posAct0,"right") == 0)  {
        // actuator 0
        (*cVel)(0) = 0.5*(2.0*(d(0)-L1*(1.0-cos(d(2)))-La0)*(v(0)-L1*sin(d(2))*v(2))+2.0*(d(1)+L1*sin(d(2)))*(v(1)+L1*cos(d(2))*v(2)))/pow(pow(d(0)-L1*(1.0-cos(d(2)))-La0,2.0)+pow(d(1)+L1*sin(d(2)),2.0),0.5);
        // actuator 1
        (*cVel)(1) = 0.5*(2*(d(0)+L0*(1.0-cos(d(2))))*(v(0)+L0*sin(d(2))*v(2))+2*(d(1)-L0*sin(d(2))+La1)*(v(1)-L0*cos(d(2))*v(2)))/pow(pow(d(0)+L0*(1.0-cos(d(2))),2)+pow(d(1)-L0*sin(d(2))+La1,2),0.5);
        // actuator 2
        (*cVel)(2) = 0.5*(2.0*(d(0)-L1*(1.0-cos(d(2))))*(v(0)-L1*sin(d(2))*v(2))+2.0*(d(1)+L1*sin(d(2))+La2)*(v(1)+L1*cos(d(2))*v(2)))/pow(pow(d(0)-L1*(1.0-cos(d(2))),2.0)+pow(d(1)+L1*sin(d(2))+La2,2.0),0.5);
    }
    
    return OF_ReturnType_completed;
}


int ESInvertedVBrace2d::transfTrialAccel(const Vector* accel)
{
    return OF_ReturnType_completed;
}


int ESInvertedVBrace2d::transfTrialAccel(const Vector* disp,
    const Vector* vel,
    const Vector* accel)
{  
    // rotate direction
    static Vector d(3), v(3), a(3);
    d = rotLocX*(*disp);
    v = rotLocX*(*vel);
    a = rotLocX*(*accel);

    // linear geometry, horizontal actuator left
    if (nlGeom == 0 && strcmp(posAct0,"left") == 0)  {
        // actuator 0
        (*cAccel)(0) = a(0);
        // actuator 1
        (*cAccel)(1) = a(1) - L0*a(2);
        // actuator 2
        (*cAccel)(2) = a(1) + L1*a(2);
    }
    // linear geometry, horizontal actuator right
    else if (nlGeom == 0 && strcmp(posAct0,"right") == 0)  {
        // actuator 0
        (*cAccel)(0) = -a(0);
        // actuator 1
        (*cAccel)(1) = a(1) - L0*a(2);
        // actuator 2
        (*cAccel)(2) = a(1) + L1*a(2);
    }
    // nonlinear geometry, horizontal actuator left
    else if (nlGeom == 1 && strcmp(posAct0,"left") == 0)  {
        // actuator 0
        (*cAccel)(0) = -0.25*pow(2.0*(d(0)+L0*(1.0-cos(d(2)))+La0)*(v(0)+L0*sin(d(2))*v(2))+2.0*(d(1)-L0*sin(d(2)))*(v(1)-L0*cos(d(2))*v(2)),2.0)/pow(pow(d(0)+L0*(1.0-cos(d(2)))+La0,2.0)+pow(d(1)-L0*sin(d(2)),2.0),1.5)+0.5*(2.0*pow(v(0)+L0*sin(d(2))*v(2),2.0)+2.0*(d(0)+L0*(1.0-cos(d(2)))+La0)*(a(0)+L0*cos(d(2))*pow(v(2),2.0)+L0*sin(d(2))*a(2))+2.0*pow(v(1)-L0*cos(d(2))*v(2),2.0)+2.0*(d(1)-L0*sin(d(2)))*(a(1)+L0*sin(d(2))*pow(v(2),2.0)-L0*cos(d(2))*a(2)))/pow(pow(d(0)+L0*(1.0-cos(d(2)))+La0,2.0)+pow(d(1)-L0*sin(d(2)),2.0),0.5);
        // actuator 1
        (*cAccel)(1) = -0.25*pow(2.0*(d(0)+L0*(1.0-cos(d(2))))*(v(0)+L0*sin(d(2))*v(2))+2.0*(d(1)-L0*sin(d(2))+La1)*(v(1)-L0*cos(d(2))*v(2)),2.0)/pow(pow(d(0)+L0*(1.0-cos(d(2))),2.0)+pow(d(1)-L0*sin(d(2))+La1,2.0),1.5)+0.5*(2.0*pow(v(0)+L0*sin(d(2))*v(2),2.0)+2.0*(d(0)+L0*(1.0-cos(d(2))))*(a(0)+L0*cos(d(2))*pow(v(2),2.0)+L0*sin(d(2))*a(2))+2.0*pow(v(1)-L0*cos(d(2))*v(2),2.0)+2.0*(d(1)-L0*sin(d(2))+La1)*(a(1)+L0*sin(d(2))*pow(v(2),2.0)-L0*cos(d(2))*a(2)))/pow(pow(d(0)+L0*(1.0-cos(d(2))),2.0)+pow(d(1)-L0*sin(d(2))+La1,2.0),0.5);
        // actuator 2
        (*cAccel)(2) = -0.25*pow(2.0*(d(0)-L1*(1.0-cos(d(2))))*(v(0)-L1*sin(d(2))*v(2))+2.0*(d(1)+L1*sin(d(2))+La2)*(v(1)+L1*cos(d(2))*v(2)),2.0)/pow(pow(d(0)-L1*(1.0-cos(d(2))),2.0)+pow(d(1)+L1*sin(d(2))+La2,2.0),1.5)+0.5*(2.0*pow(v(0)-L1*sin(d(2))*v(2),2.0)+2.0*(d(0)-L1*(1.0-cos(d(2))))*(a(0)-L1*cos(d(2))*pow(v(2),2.0)-L1*sin(d(2))*a(2))+2.0*pow(v(1)+L1*cos(d(2))*v(2),2.0)+2.0*(d(1)+L1*sin(d(2))+La2)*(a(1)-L1*sin(d(2))*pow(v(2),2.0)+L1*cos(d(2))*a(2)))/pow(pow(d(0)-L1*(1.0-cos(d(2))),2.0)+pow(d(1)+L1*sin(d(2))+La2,2.0),0.5);
    }
    // nonlinear geometry, horizontal actuator right
    else if (nlGeom == 1 && strcmp(posAct0,"right") == 0)  {
        // actuator 0
        (*cAccel)(0) = -0.25*pow(2.0*(d(0)-L1*(1.0-cos(d(2)))-La0)*(v(0)-L1*sin(d(2))*v(2))+2.0*(d(1)+L1*sin(d(2)))*(v(1)+L1*cos(d(2))*v(2)),2.0)/pow(pow(d(0)-L1*(1.0-cos(d(2)))-La0,2.0)+pow(d(1)+L1*sin(d(2)),2.0),1.5)+0.5*(2.0*pow(v(0)-L1*sin(d(2))*v(2),2.0)+2.0*(d(0)-L1*(1.0-cos(d(2)))-La0)*(a(0)-L1*cos(d(2))*pow(v(2),2.0)-L1*sin(d(2))*a(2))+2.0*pow(v(1)+L1*cos(d(2))*v(2),2.0)+2.0*(d(1)+L1*sin(d(2)))*(a(1)-L1*sin(d(2))*pow(v(2),2.0)+L1*cos(d(2))*a(2)))/pow(pow(d(0)-L1*(1.0-cos(d(2)))-La0,2.0)+pow(d(1)+L1*sin(d(2)),2.0),0.5);
        // actuator 1
        (*cAccel)(1) = -0.25*pow(2.0*(d(0)+L0*(1.0-cos(d(2))))*(v(0)+L0*sin(d(2))*v(2))+2.0*(d(1)-L0*sin(d(2))+La1)*(v(1)-L0*cos(d(2))*v(2)),2.0)/pow(pow(d(0)+L0*(1.0-cos(d(2))),2.0)+pow(d(1)-L0*sin(d(2))+La1,2.0),1.5)+0.5*(2.0*pow(v(0)+L0*sin(d(2))*v(2),2.0)+2.0*(d(0)+L0*(1.0-cos(d(2))))*(a(0)+L0*cos(d(2))*pow(v(2),2.0)+L0*sin(d(2))*a(2))+2.0*pow(v(1)-L0*cos(d(2))*v(2),2.0)+2.0*(d(1)-L0*sin(d(2))+La1)*(a(1)+L0*sin(d(2))*pow(v(2),2.0)-L0*cos(d(2))*a(2)))/pow(pow(d(0)+L0*(1.0-cos(d(2))),2.0)+pow(d(1)-L0*sin(d(2))+La1,2.0),0.5);
        // actuator 2
        (*cAccel)(2) = -0.25*pow(2.0*(d(0)-L1*(1.0-cos(d(2))))*(v(0)-L1*sin(d(2))*v(2))+2.0*(d(1)+L1*sin(d(2))+La2)*(v(1)+L1*cos(d(2))*v(2)),2.0)/pow(pow(d(0)-L1*(1.0-cos(d(2))),2.0)+pow(d(1)+L1*sin(d(2))+La2,2.0),1.5)+0.5*(2.0*pow(v(0)-L1*sin(d(2))*v(2),2.0)+2.0*(d(0)-L1*(1.0-cos(d(2))))*(a(0)-L1*cos(d(2))*pow(v(2),2.0)-L1*sin(d(2))*a(2))+2.0*pow(v(1)+L1*cos(d(2))*v(2),2.0)+2.0*(d(1)+L1*sin(d(2))+La2)*(a(1)-L1*sin(d(2))*pow(v(2),2.0)+L1*cos(d(2))*a(2)))/pow(pow(d(0)-L1*(1.0-cos(d(2))),2.0)+pow(d(1)+L1*sin(d(2))+La2,2.0),0.5);
    }
    
    return OF_ReturnType_completed;
}


int ESInvertedVBrace2d::transfTrialForce(const Vector* force)
{  
    // rotate direction
    static Vector f(3);
    f = rotLocX*(*force);

    // linear geometry, horizontal actuator left
    if (nlGeom == 0 && strcmp(posAct0,"left") == 0)  {
        // actuator 0
        (*cForce)(0) = f(0);
        // actuator 1
        (*cForce)(1) = 1.0/(L0+L1)*(L1*f(1) - f(2));
        // actuator 2
        (*cForce)(2) = 1.0/(L0+L1)*(L0*f(1) + f(2));
    }
    // linear geometry, horizontal actuator right
    else if (nlGeom == 0 && strcmp(posAct0,"right") == 0)  {
        // actuator 0
        (*cForce)(0) = -f(0);
        // actuator 1
        (*cForce)(1) = 1.0/(L0+L1)*(L1*f(1) - f(2));
        // actuator 2
        (*cForce)(2) = 1.0/(L0+L1)*(L0*f(1) + f(2));
    }
    // nonlinear geometry, horizontal actuator left
    else if (nlGeom == 1 && strcmp(posAct0,"left") == 0)  {
        if (firstWarning[0] == true)  {
            opserr << "WARNING ESInvertedVBrace2d::transfTrialForce() - "
                << "nonlinear geometry with horizontal actuator left "
                << "not implemented yet. Using linear geometry instead.\n\n";
            firstWarning[0] = false;
        }
        // actuator 0
        (*cForce)(0) = f(0);
        // actuator 1
        (*cForce)(1) = 1.0/(L0+L1)*(L1*f(1) - f(2));
        // actuator 2
        (*cForce)(2) = 1.0/(L0+L1)*(L0*f(1) + f(2));
    }
    // nonlinear geometry, horizontal actuator right
    else if (nlGeom == 1 && strcmp(posAct0,"right") == 0)  {
        if (firstWarning[0] == true)  {
            opserr << "WARNING ESInvertedVBrace2d::transfTrialForce() - "
                << "nonlinear geometry with horizontal actuator right "
                << "not implemented yet. Using linear geometry instead.\n\n";
            firstWarning[0] = false;
        }
        // actuator 0
        (*cForce)(0) = -f(0);
        // actuator 1
        (*cForce)(1) = 1.0/(L0+L1)*(L1*f(1) - f(2));
        // actuator 2
        (*cForce)(2) = 1.0/(L0+L1)*(L0*f(1) + f(2));
    }
    
    return OF_ReturnType_completed;
}


int ESInvertedVBrace2d::transfTrialTime(const Vector* time)
{
    *cTime = *time;
    
    return OF_ReturnType_completed;
}


int ESInvertedVBrace2d::transfDaqDisp(Vector* disp)
{
    // linear geometry, horizontal actuator left
    if (nlGeom == 0 && strcmp(posAct0,"left") == 0)  {
        (*disp)(0) = (*dDisp)(0);
        (*disp)(1) = 1.0/(L0+L1)*(L1*(*dDisp)(1) + L0*(*dDisp)(2));
        (*disp)(2) = 1.0/(L0+L1)*(-(*dDisp)(1) + (*dDisp)(2));
    }
    // linear geometry, horizontal actuator right
    else if (nlGeom == 0 && strcmp(posAct0,"right") == 0)  {
        (*disp)(0) = -(*dDisp)(0);
        (*disp)(1) = 1.0/(L0+L1)*(L1*(*dDisp)(1) + L0*(*dDisp)(2));
        (*disp)(2) = 1.0/(L0+L1)*(-(*dDisp)(1) + (*dDisp)(2));
    }
    // nonlinear geometry, horizontal actuator left
    else if (nlGeom == 1 && strcmp(posAct0,"left") == 0)  {
        Vector F(2), theta(2), dTheta(2);
        Matrix DF(2,2);
        int iter = 0;
        int maxIter = 15;
        double tol = 1E-9;
        
        double d0 = La0 + (*dDisp)(0);
        double d1 = La1 + (*dDisp)(1);
        double d2 = La2 + (*dDisp)(2);
        
        theta(0) = (*dDisp)(0)/La1;
        theta(1) = (*dDisp)(0)/La2;
        
        do  {
            F(0) = pow(d0,2.0) - pow(d1*sin(theta(0))+La0,2.0) - pow(d1*cos(theta(0))-La1,2.0);
            F(1) = pow(L0+L1,2.0) - pow(d2*sin(theta(1))+L0+L1-d1*sin(theta(0)),2.0) - pow(d2*cos(theta(1))-d1*cos(theta(0)),2.0);
            
            DF(0,0) = -2.0*d1*(La0*cos(theta(0))+La1*sin(theta(0)));
            DF(0,1) = 0.0;
            DF(1,0) = 2.0*d1*((L0+L1)*cos(theta(0))-d2*sin(theta(0)-theta(1)));
            DF(1,1) = 2.0*d2*(-(L0+L1)*cos(theta(1))+d1*sin(theta(0)-theta(1)));
            
            // Newton's method
            dTheta = F/DF;
            theta -= dTheta;
            iter++;
        }  while ((dTheta.Norm() >= tol) && (iter <= maxIter));
        
        // issue warning if iteration did not converge
        if (iter >= maxIter)   {
            opserr << "WARNING ESInvertedVBrace2d::transfDaqDisp() - "
                << "did not find the angle theta after "
                << iter << " iterations and norm: " << dTheta.Norm() << endln;
        }

        (*disp)(2) = atan((d2*cos(theta(1))-d1*cos(theta(0)))/(d2*sin(theta(1))+L0+L1-d1*sin(theta(0))));
        (*disp)(0) = d1*sin(theta(0))+L0*cos((*disp)(2))-L0;
        (*disp)(1) = d1*cos(theta(0))+L0*sin((*disp)(2))-La1;
    }
    // nonlinear geometry, horizontal actuator right
    else if (nlGeom == 1 && strcmp(posAct0,"right") == 0)  {
        Vector F(2), theta(2), dTheta(2);
        Matrix DF(2,2);
        int iter = 0;
        int maxIter = 15;
        double tol = 1E-9;
        
        double d0 = La0 + (*dDisp)(0);
        double d1 = La1 + (*dDisp)(1);
        double d2 = La2 + (*dDisp)(2);
        
        theta(0) = (*dDisp)(0)/La1;
        theta(1) = (*dDisp)(0)/La2;
        
        do  {
            F(0) = pow(L0+L1,2.0) - pow(-d2*sin(theta(1))+L0+L1+d1*sin(theta(0)),2.0) - pow(d2*cos(theta(1))-d1*cos(theta(0)),2.0);
            F(1) = pow(d0,2.0) - pow(-d2*sin(theta(1))-La0,2.0) - pow(d2*cos(theta(1))-La1,2.0);
            
            DF(0,0) = 2.0*d1*(-(L0+L1)*cos(theta(0))-d2*sin(theta(0)-theta(1)));
            DF(0,1) = 2.0*d2*((L0+L1)*cos(theta(1))+d1*sin(theta(0)-theta(1)));
            DF(1,0) = 0.0;
            DF(1,1) = -2.0*d2*(La0*cos(theta(1))+La1*sin(theta(1)));
            
            // Newton’s method
            dTheta = F/DF;
            theta -= dTheta;
            iter++;
        }  while ((dTheta.Norm() >= tol) && (iter <= maxIter));
        
        // issue warning if iteration did not converge
        if (iter >= maxIter)   {
            opserr << "WARNING ESInvertedVBrace2d::transfDaqDisp() - "
                << "did not find the angle theta after "
                << iter << " iterations and norm: " << dTheta.Norm() << endln;
        }

        (*disp)(2) = atan((d2*cos(theta(1))-d1*cos(theta(0)))/(-d2*sin(theta(1))+L0+L1+d1*sin(theta(0))));
        (*disp)(0) = -d1*sin(theta(0))+L0*cos((*disp)(2))-L0;
        (*disp)(1) = d1*cos(theta(0))+L0*sin((*disp)(2))-La1;
    }
    
    // rotate direction if necessary
    if (phiLocX != 0.0)  {
        (*disp) = rotLocX^(*disp);
    }

    return OF_ReturnType_completed;
}


int ESInvertedVBrace2d::transfDaqVel(Vector* vel)
{
    // linear geometry, horizontal actuator left
    if (nlGeom == 0 && strcmp(posAct0,"left") == 0)  {
        (*vel)(0) = (*dVel)(0);
        (*vel)(1) = 1.0/(L0+L1)*(L1*(*dVel)(1) + L0*(*dVel)(2));
        (*vel)(2) = 1.0/(L0+L1)*(-(*dVel)(1) + (*dVel)(2));
    }
    // linear geometry, horizontal actuator right
    else if (nlGeom == 0 && strcmp(posAct0,"right") == 0)  {
        (*vel)(0) = -(*dVel)(0);
        (*vel)(1) = 1.0/(L0+L1)*(L1*(*dVel)(1) + L0*(*dVel)(2));
        (*vel)(2) = 1.0/(L0+L1)*(-(*dVel)(1) + (*dVel)(2));
    }
    // nonlinear geometry, horizontal actuator left
    else if (nlGeom == 1 && strcmp(posAct0,"left") == 0)  {
        if (firstWarning[1] == true)  {
            opserr << "WARNING ESInvertedVBrace2d::transfDaqVel() - "
                << "nonlinear geometry with horizontal actuator left "
                << "not implemented yet. Using linear geometry instead.\n\n";
            firstWarning[1] = false;
        }
        (*vel)(0) = (*dVel)(0);
        (*vel)(1) = 1.0/(L0+L1)*(L1*(*dVel)(1) + L0*(*dVel)(2));
        (*vel)(2) = 1.0/(L0+L1)*(-(*dVel)(1) + (*dVel)(2));
    }
    // nonlinear geometry, horizontal actuator right
    else if (nlGeom == 1 && strcmp(posAct0,"right") == 0)  {
        if (firstWarning[1] == true)  {
            opserr << "WARNING ESInvertedVBrace2d::transfDaqVel() - "
                << "nonlinear geometry with horizontal actuator right "
                << "not implemented yet. Using linear geometry instead.\n\n";
            firstWarning[1] = false;
        }
        (*vel)(0) = -(*dVel)(0);
        (*vel)(1) = 1.0/(L0+L1)*(L1*(*dVel)(1) + L0*(*dVel)(2));
        (*vel)(2) = 1.0/(L0+L1)*(-(*dVel)(1) + (*dVel)(2));
    }
    
    // rotate direction if necessary
    if (phiLocX != 0.0)  {
        (*vel) = rotLocX^(*vel);
    }

    return OF_ReturnType_completed;
}


int ESInvertedVBrace2d::transfDaqAccel(Vector* accel)
{
    // linear geometry, horizontal actuator left
    if (nlGeom == 0 && strcmp(posAct0,"left") == 0)  {
        (*accel)(0) = (*dAccel)(0);
        (*accel)(1) = 1.0/(L0+L1)*(L1*(*dAccel)(1) + L0*(*dAccel)(2));
        (*accel)(2) = 1.0/(L0+L1)*(-(*dAccel)(1) + (*dAccel)(2));
    }
    // linear geometry, horizontal actuator right
    else if (nlGeom == 0 && strcmp(posAct0,"right") == 0)  {
        (*accel)(0) = -(*dAccel)(0);
        (*accel)(1) = 1.0/(L0+L1)*(L1*(*dAccel)(1) + L0*(*dAccel)(2));
        (*accel)(2) = 1.0/(L0+L1)*(-(*dAccel)(1) + (*dAccel)(2));
    }
    // nonlinear geometry, horizontal actuator left
    else if (nlGeom == 1 && strcmp(posAct0,"left") == 0)  {
        if (firstWarning[2] == true)  {
            opserr << "WARNING ESInvertedVBrace2d::transfDaqAccel() - "
                << "nonlinear geometry with horizontal actuator left "
                << "not implemented yet. Using linear geometry instead.\n\n";
            firstWarning[2] = false;
        }
        (*accel)(0) = (*dAccel)(0);
        (*accel)(1) = 1.0/(L0+L1)*(L1*(*dAccel)(1) + L0*(*dAccel)(2));
        (*accel)(2) = 1.0/(L0+L1)*(-(*dAccel)(1) + (*dAccel)(2));
    }
    // nonlinear geometry, horizontal actuator right
    else if (nlGeom == 1 && strcmp(posAct0,"right") == 0)  {
        if (firstWarning[2] == true)  {
            opserr << "WARNING ESInvertedVBrace2d::transfDaqAccel() - "
                << "nonlinear geometry with horizontal actuator right "
                << "not implemented yet. Using linear geometry instead.\n\n";
            firstWarning[2] = false;
        }
        (*accel)(0) = -(*dAccel)(0);
        (*accel)(1) = 1.0/(L0+L1)*(L1*(*dAccel)(1) + L0*(*dAccel)(2));
        (*accel)(2) = 1.0/(L0+L1)*(-(*dAccel)(1) + (*dAccel)(2));
    }
    
    // rotate direction if necessary
    if (phiLocX != 0.0)  {
        (*accel) = rotLocX^(*accel);
    }

    return OF_ReturnType_completed;
}


int ESInvertedVBrace2d::transfDaqForce(Vector* force)
{
    *force = *dForce;
    
    return OF_ReturnType_completed;
}


int ESInvertedVBrace2d::transfDaqTime(Vector* time)
{  
    *time = *dTime;
    
    return OF_ReturnType_completed;
}
