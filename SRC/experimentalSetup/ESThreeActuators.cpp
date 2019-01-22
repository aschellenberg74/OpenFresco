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

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 09/06
// Revision: A
//
// Description: This file contains the implementation of the
// ESThreeActuators class.

#include "ESThreeActuators.h"

#include <math.h>

const int numDOF = 3;

ESThreeActuators::ESThreeActuators(int tag,
    const ID &dof, int sizet, int sizeo,
    double actLength0, double actLength1, double actLength2,
    double rigidLength0, double rigidLength1,
    ExperimentalControl* control,
    int nlgeom, char *posact0)
    : ExperimentalSetup(tag, control),
    DOF(dof), sizeT(sizet), sizeO(sizeo),
    La0(actLength0), La1(actLength1), La2(actLength2),
    L0(rigidLength0), L1(rigidLength1), nlGeom(nlgeom)
{
    // check if DOF array has correct size
    if (DOF.Size() != numDOF)  {
        opserr << "ESThreeActuators::ESThreeActuators()"
            << " - DOF ID has wrong size != 3\n";
        exit(OF_ReturnType_failed);
    }
    
    // check DOFs for valid values
    for (int i = 0; i<numDOF; i++)  {
        if (DOF(i) < 0 || DOF(i) >= sizeT ||
            DOF(i) >= sizeO)  {
            opserr << "ESThreeActuators::ESThreeActuators()"
                << " - DOF ID out of bound:"
                << DOF(i) << endln;
            exit(OF_ReturnType_failed);
        }
    }
    
    strcpy(posAct0, posact0);
    
    // call setup method
    this->setup();
    
    for (int i=0; i<3; i++)
        firstWarning[i] = true;
}


ESThreeActuators::ESThreeActuators(const ESThreeActuators& es)
    : ExperimentalSetup(es)
{
    DOF =   es.DOF;
    sizeT = es.sizeT;
    sizeO = es.sizeO;
    La0     = es.La0;
    La1     = es.La1;
    La2     = es.La2;
    L0      = es.L0;
    L1      = es.L1;
    nlGeom  = es.nlGeom;
    strcpy(posAct0,es.posAct0);
    
    // call setup method
    this->setup();
    
    for (int i=0; i<3; i++)
        firstWarning[i] = true;
}


ESThreeActuators::~ESThreeActuators()
{
    // invoke the destructor on any objects created by the object
    // that the object still holds a pointer to
}


int ESThreeActuators::setup()
{
    // setup the trial/out vectors
    sizeTrial->Zero();
    sizeOut->Zero();
    for (int i=0; i<OF_Resp_Time; i++)  {
        (*sizeTrial)(i) = sizeT;
        (*sizeOut)(i) = sizeO;
    }
    (*sizeTrial)(OF_Resp_Time) = 1;
    (*sizeOut)(OF_Resp_Time) = 1;
    
    this->setTrialOutSize();
    
    // setup the ctrl/daq vectors
    sizeCtrl->Zero();
    sizeDaq->Zero();
    for (int i=0; i<OF_Resp_Time; i++)  {
        (*sizeCtrl)(i) = numDOF;
        (*sizeDaq)(i) = numDOF;
    }
    (*sizeCtrl)(OF_Resp_Time) = 1;
    (*sizeDaq)(OF_Resp_Time) = 1;
    
    this->setCtrlDaqSize();
    
    return OF_ReturnType_completed;
}


int ESThreeActuators::transfTrialResponse(const Vector* disp, 
    const Vector* vel,
    const Vector* accel,
    const Vector* force,
    const Vector* time)
{
    // transform data
    if (disp != 0)  {
        for (int i=0; i<(*sizeTrial)(OF_Resp_Disp); i++)
            (*tDisp)(i) = (*disp)(i) * (*tDispFact)(i);
        this->transfTrialDisp(tDisp);
        for (int i=0; i<(*sizeCtrl)(OF_Resp_Disp); i++)
            (*cDisp)(i) *= (*cDispFact)(i);
    }
    if (disp != 0 && vel != 0)  {
        for (int i=0; i<(*sizeTrial)(OF_Resp_Vel); i++)
            (*tVel)(i) = (*vel)(i) * (*tVelFact)(i);
        this->transfTrialVel(tDisp,tVel);
        for (int i=0; i<(*sizeCtrl)(OF_Resp_Vel); i++)
            (*cVel)(i) *= (*cVelFact)(i);
    }
    if (disp != 0 && vel != 0 && accel != 0)  {
        for (int i=0; i<(*sizeTrial)(OF_Resp_Accel); i++)
            (*tAccel)(i) = (*accel)(i) * (*tAccelFact)(i);
        this->transfTrialAccel(tDisp,tVel,tAccel);
        for (int i=0; i<(*sizeCtrl)(OF_Resp_Accel); i++)
            (*cAccel)(i) *= (*cAccelFact)(i);
    }
    if (force != 0)  {
        for (int i=0; i<(*sizeTrial)(OF_Resp_Force); i++)
            (*tForce)(i) = (*force)(i) * (*tForceFact)(i);
        this->transfTrialForce(tForce);
        for (int i=0; i<(*sizeCtrl)(OF_Resp_Force); i++)
            (*cForce)(i) *= (*cForceFact)(i);
    }
    if (time != 0)  {
        for (int i=0; i<(*sizeTrial)(OF_Resp_Time); i++)
            (*tTime)(i) = (*time)(i) * (*tTimeFact)(i);
        this->transfTrialTime(tTime);
        for (int i=0; i<(*sizeCtrl)(OF_Resp_Time); i++)
            (*cTime)(i) *= (*cTimeFact)(i);
    }
    
    return OF_ReturnType_completed;
}


ExperimentalSetup* ESThreeActuators::getCopy()
{
    ESThreeActuators *theCopy = new ESThreeActuators(*this);
    
    return theCopy;
}


void ESThreeActuators::Print(OPS_Stream &s, int flag)
{
    s << "ExperimentalSetup: " << this->getTag(); 
    s << " type: ESThreeActuators\n";
    s << " dof         : " << DOF << endln;
    s << " actLength1  : " << La0 << endln;
    s << " actLength2  : " << La1 << endln;
    s << " actLength3  : " << La2 << endln;
    s << " rigidLength1: " << L0 << endln;
    s << " rigidLength2: " << L1 << endln;
    s << " nlGeom      : " << nlGeom << endln;
    s << " posAct1     : " << posAct0 << endln;
    if (theControl != 0)  {
        s << "\tExperimentalControl tag: " << theControl->getTag();
        s << *theControl;
    }
}


int ESThreeActuators::transfTrialDisp(const Vector* disp)
{
    // extract directions
    static Vector d(numDOF);
    for (int i=0; i<numDOF; i++)  {
        d(i) = (*disp)(DOF(i));
    }
    
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


int ESThreeActuators::transfTrialVel(const Vector* vel)
{
    return OF_ReturnType_completed;
}


int ESThreeActuators::transfTrialVel(const Vector* disp,
    const Vector* vel)
{
    // extract directions
    static Vector d(numDOF), v(numDOF);
    for (int i=0; i<numDOF; i++)  {
        d(i) = (*disp)(DOF(i));
        v(i) = (*vel)(DOF(i));
    }
    
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


int ESThreeActuators::transfTrialAccel(const Vector* accel)
{
    return OF_ReturnType_completed;
}


int ESThreeActuators::transfTrialAccel(const Vector* disp,
    const Vector* vel,
    const Vector* accel)
{
    // extract directions
    static Vector d(numDOF), v(numDOF), a(numDOF);
    for (int i=0; i<numDOF; i++)  {
        d(i) = (*disp)(DOF(i));
        v(i) = (*vel)(DOF(i));
        a(i) = (*accel)(DOF(i));
    }
    
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


int ESThreeActuators::transfTrialForce(const Vector* force)
{
    // extract directions
    static Vector f(numDOF);
    for (int i=0; i<numDOF; i++)  {
        f(i) = (*force)(DOF(i));
    }
    
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
            opserr << "WARNING ESThreeActuators::transfTrialForce() - "
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
            opserr << "WARNING ESThreeActuators::transfTrialForce() - "
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


int ESThreeActuators::transfTrialTime(const Vector* time)
{
    *cTime = *time;
    
    return OF_ReturnType_completed;
}


int ESThreeActuators::transfDaqDisp(Vector* disp)
{
    static Vector d(numDOF);
    
    // linear geometry, horizontal actuator left
    if (nlGeom == 0 && strcmp(posAct0,"left") == 0)  {
        d(0) = (*dDisp)(0);
        d(1) = 1.0/(L0+L1)*(L1*(*dDisp)(1) + L0*(*dDisp)(2));
        d(2) = 1.0/(L0+L1)*(-(*dDisp)(1) + (*dDisp)(2));
    }
    // linear geometry, horizontal actuator right
    else if (nlGeom == 0 && strcmp(posAct0,"right") == 0)  {
        d(0) = -(*dDisp)(0);
        d(1) = 1.0/(L0+L1)*(L1*(*dDisp)(1) + L0*(*dDisp)(2));
        d(2) = 1.0/(L0+L1)*(-(*dDisp)(1) + (*dDisp)(2));
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
            F(1) = pow(L0+L1,2.0) - pow(d2*sin(theta(1))+L0+L1-d1*sin(theta(0)),2.0) - pow(d2*cos(theta(1))+La1-La2-d1*cos(theta(0)),2.0);
            
            DF(0,0) = 2.0*d1*(-La0*cos(theta(0))-La1*sin(theta(0)));
            DF(0,1) = 0.0;
            DF(1,0) = 2.0*d1*((L0+L1)*cos(theta(0))-d2*sin(theta(0)-theta(1))-(La1-La2)*sin(theta(0)));
            DF(1,1) = 2.0*d2*(-(L0+L1)*cos(theta(1))+d1*sin(theta(0)-theta(1))+(La1-La2)*sin(theta(1)));
            
            // Newton's method
            dTheta = F/DF;
            theta -= dTheta;
            iter++;
        }  while ((dTheta.Norm() >= tol) && (iter <= maxIter));
        
        // issue warning if iteration did not converge
        if (iter >= maxIter)   {
            opserr << "WARNING ESThreeActuators::transfDaqDisp() - "
                << "did not find the angle theta after "
                << iter << " iterations and norm: " << dTheta.Norm() << endln;
        }
        
        d(2) = atan2(d2*cos(theta(1))+La1-La2-d1*cos(theta(0)),d2*sin(theta(1))+L0+L1-d1*sin(theta(0)));
        d(0) = d1*sin(theta(0))+L0*cos(d(2))-L0;
        d(1) = d1*cos(theta(0))+L0*sin(d(2))-La1;
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
            F(0) = pow(L0+L1,2.0) - pow(-d2*sin(theta(1))+L0+L1+d1*sin(theta(0)),2.0) - pow(d2*cos(theta(1))+La1-La2-d1*cos(theta(0)),2.0);
            F(1) = pow(d0,2.0) - pow(-d2*sin(theta(1))-La0,2.0) - pow(d2*cos(theta(1))-La2,2.0);
            
            DF(0,0) = 2.0*d1*(-(L0+L1)*cos(theta(0))-d2*sin(theta(0)-theta(1))-(La1-La2)*sin(theta(0)));
            DF(0,1) = 2.0*d2*((L0+L1)*cos(theta(1))+d1*sin(theta(0)-theta(1))+(La1-La2)*sin(theta(1)));
            DF(1,0) = 0.0;
            DF(1,1) = 2.0*d2*(-La0*cos(theta(1))-La2*sin(theta(1)));
            
            // Newton’s method
            dTheta = F/DF;
            theta -= dTheta;
            iter++;
        }  while ((dTheta.Norm() >= tol) && (iter <= maxIter));
        
        // issue warning if iteration did not converge
        if (iter >= maxIter)   {
            opserr << "WARNING ESThreeActuators::transfDaqDisp() - "
                << "did not find the angle theta after "
                << iter << " iterations and norm: " << dTheta.Norm() << endln;
        }
        
        d(2) = atan2(d2*cos(theta(1))+La1-La2-d1*cos(theta(0)),-d2*sin(theta(1))+L0+L1+d1*sin(theta(0)));
        d(0) = -d1*sin(theta(0))+L0*cos(d(2))-L0;
        d(1) = d1*cos(theta(0))+L0*sin(d(2))-La1;
    }
    
    // assemble directions
    for (int i=0; i<numDOF; i++)  {
        (*disp)(DOF(i)) = d(i);
    }
    
    return OF_ReturnType_completed;
}


int ESThreeActuators::transfDaqVel(Vector* vel)
{
    static Vector v(numDOF);
    
    // linear geometry, horizontal actuator left
    if (nlGeom == 0 && strcmp(posAct0,"left") == 0)  {
        v(0) = (*dVel)(0);
        v(1) = 1.0/(L0+L1)*(L1*(*dVel)(1) + L0*(*dVel)(2));
        v(2) = 1.0/(L0+L1)*(-(*dVel)(1) + (*dVel)(2));
    }
    // linear geometry, horizontal actuator right
    else if (nlGeom == 0 && strcmp(posAct0,"right") == 0)  {
        v(0) = -(*dVel)(0);
        v(1) = 1.0/(L0+L1)*(L1*(*dVel)(1) + L0*(*dVel)(2));
        v(2) = 1.0/(L0+L1)*(-(*dVel)(1) + (*dVel)(2));
    }
    // nonlinear geometry, horizontal actuator left
    else if (nlGeom == 1 && strcmp(posAct0,"left") == 0)  {
        if (firstWarning[1] == true)  {
            opserr << "WARNING ESThreeActuators::transfDaqVel() - "
                << "nonlinear geometry with horizontal actuator left "
                << "not implemented yet. Using linear geometry instead.\n\n";
            firstWarning[1] = false;
        }
        v(0) = (*dVel)(0);
        v(1) = 1.0/(L0+L1)*(L1*(*dVel)(1) + L0*(*dVel)(2));
        v(2) = 1.0/(L0+L1)*(-(*dVel)(1) + (*dVel)(2));
    }
    // nonlinear geometry, horizontal actuator right
    else if (nlGeom == 1 && strcmp(posAct0,"right") == 0)  {
        if (firstWarning[1] == true)  {
            opserr << "WARNING ESThreeActuators::transfDaqVel() - "
                << "nonlinear geometry with horizontal actuator right "
                << "not implemented yet. Using linear geometry instead.\n\n";
            firstWarning[1] = false;
        }
        v(0) = -(*dVel)(0);
        v(1) = 1.0/(L0+L1)*(L1*(*dVel)(1) + L0*(*dVel)(2));
        v(2) = 1.0/(L0+L1)*(-(*dVel)(1) + (*dVel)(2));
    }
    
    // assemble directions
    for (int i=0; i<numDOF; i++)  {
        (*vel)(DOF(i)) = v(i);
    }
    
    return OF_ReturnType_completed;
}


int ESThreeActuators::transfDaqAccel(Vector* accel)
{
    static Vector a(numDOF);
    
    // linear geometry, horizontal actuator left
    if (nlGeom == 0 && strcmp(posAct0,"left") == 0)  {
        a(0) = (*dAccel)(0);
        a(1) = 1.0/(L0+L1)*(L1*(*dAccel)(1) + L0*(*dAccel)(2));
        a(2) = 1.0/(L0+L1)*(-(*dAccel)(1) + (*dAccel)(2));
    }
    // linear geometry, horizontal actuator right
    else if (nlGeom == 0 && strcmp(posAct0,"right") == 0)  {
        a(0) = -(*dAccel)(0);
        a(1) = 1.0/(L0+L1)*(L1*(*dAccel)(1) + L0*(*dAccel)(2));
        a(2) = 1.0/(L0+L1)*(-(*dAccel)(1) + (*dAccel)(2));
    }
    // nonlinear geometry, horizontal actuator left
    else if (nlGeom == 1 && strcmp(posAct0,"left") == 0)  {
        if (firstWarning[2] == true)  {
            opserr << "WARNING ESThreeActuators::transfDaqAccel() - "
                << "nonlinear geometry with horizontal actuator left "
                << "not implemented yet. Using linear geometry instead.\n\n";
            firstWarning[2] = false;
        }
        a(0) = (*dAccel)(0);
        a(1) = 1.0/(L0+L1)*(L1*(*dAccel)(1) + L0*(*dAccel)(2));
        a(2) = 1.0/(L0+L1)*(-(*dAccel)(1) + (*dAccel)(2));
    }
    // nonlinear geometry, horizontal actuator right
    else if (nlGeom == 1 && strcmp(posAct0,"right") == 0)  {
        if (firstWarning[2] == true)  {
            opserr << "WARNING ESThreeActuators::transfDaqAccel() - "
                << "nonlinear geometry with horizontal actuator right "
                << "not implemented yet. Using linear geometry instead.\n\n";
            firstWarning[2] = false;
        }
        a(0) = -(*dAccel)(0);
        a(1) = 1.0/(L0+L1)*(L1*(*dAccel)(1) + L0*(*dAccel)(2));
        a(2) = 1.0/(L0+L1)*(-(*dAccel)(1) + (*dAccel)(2));
    }
    
    // assemble directions
    for (int i=0; i<numDOF; i++)  {
        (*accel)(DOF(i)) = a(i);
    }
    
    return OF_ReturnType_completed;
}


int ESThreeActuators::transfDaqForce(Vector* force)
{
    static Vector f(numDOF);
    
    // linear geometry, horizontal actuator left
    if (nlGeom == 0 && strcmp(posAct0,"left") == 0)  {
        f(0) = (*dForce)(0);
        f(1) = (*dForce)(1) + (*dForce)(2);
        f(2) = -L0*(*dForce)(1) + L1*(*dForce)(2);
    }
    // linear geometry, horizontal actuator right
    else if (nlGeom == 0 && strcmp(posAct0,"right") == 0)  {
        f(0) = -(*dForce)(0);
        f(1) = (*dForce)(1) + (*dForce)(2);
        f(2) = -L0*(*dForce)(1) + L1*(*dForce)(2);
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
            F(1) = pow(L0+L1,2.0) - pow(d2*sin(theta(1))+L0+L1-d1*sin(theta(0)),2.0) - pow(d2*cos(theta(1))+La1-La2-d1*cos(theta(0)),2.0);
            
            DF(0,0) = 2.0*d1*(-La0*cos(theta(0))-La1*sin(theta(0)));
            DF(0,1) = 0.0;
            DF(1,0) = 2.0*d1*((L0+L1)*cos(theta(0))-d2*sin(theta(0)-theta(1))-(La1-La2)*sin(theta(0)));
            DF(1,1) = 2.0*d2*(-(L0+L1)*cos(theta(1))+d1*sin(theta(0)-theta(1))+(La1-La2)*sin(theta(1)));
            
            // Newton's method
            dTheta = F/DF;
            theta -= dTheta;
            iter++;
        }  while ((dTheta.Norm() >= tol) && (iter <= maxIter));
        
        // issue warning if iteration did not converge
        if (iter >= maxIter)   {
            opserr << "WARNING ESThreeActuators::transfDaqForce() - "
                << "did not find the angle theta after "
                << iter << " iterations and norm: " << dTheta.Norm() << endln;
        }
        
        double disp2 = atan2(d2*cos(theta(1))+La1-La2-d1*cos(theta(0)),d2*sin(theta(1))+L0+L1-d1*sin(theta(0)));
        
        static Vector fx(3), fy(3);
        fx(0) = (*dForce)(0)*(d1*sin(theta(0))+La0)/d0;
        fx(1) = (*dForce)(1)*sin(theta(0));
        fx(2) = (*dForce)(2)*sin(theta(1));
        fy(0) = (*dForce)(0)*(d1*cos(theta(0))-La1)/d0;
        fy(1) = (*dForce)(1)*cos(theta(0));
        fy(2) = (*dForce)(2)*cos(theta(1));
        
        f(0) = fx(0) + fx(1) + fx(2);
        f(1) = fy(0) + fy(1) + fy(2);
        f(2) = ((fx(0)+fx(1))*L0 - fx(2)*L1)*sin(disp2) - ((fy(0)+fy(1))*L0 - fy(2)*L1)*cos(disp2);
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
            F(0) = pow(L0+L1,2.0) - pow(-d2*sin(theta(1))+L0+L1+d1*sin(theta(0)),2.0) - pow(d2*cos(theta(1))+La1-La2-d1*cos(theta(0)),2.0);
            F(1) = pow(d0,2.0) - pow(-d2*sin(theta(1))-La0,2.0) - pow(d2*cos(theta(1))-La2,2.0);
            
            DF(0,0) = 2.0*d1*(-(L0+L1)*cos(theta(0))-d2*sin(theta(0)-theta(1))-(La1-La2)*sin(theta(0)));
            DF(0,1) = 2.0*d2*((L0+L1)*cos(theta(1))+d1*sin(theta(0)-theta(1))+(La1-La2)*sin(theta(1)));
            DF(1,0) = 0.0;
            DF(1,1) = 2.0*d2*(-La0*cos(theta(1))-La2*sin(theta(1)));
            
            // Newton’s method
            dTheta = F/DF;
            theta -= dTheta;
            iter++;
        }  while ((dTheta.Norm() >= tol) && (iter <= maxIter));
        
        // issue warning if iteration did not converge
        if (iter >= maxIter)   {
            opserr << "WARNING ESThreeActuators::transfDaqForce() - "
                << "did not find the angle theta after "
                << iter << " iterations and norm: " << dTheta.Norm() << endln;
        }
        
        double disp2 = atan2(d2*cos(theta(1))+La1-La2-d1*cos(theta(0)),-d2*sin(theta(1))+L0+L1+d1*sin(theta(0)));
        
        static Vector fx(3), fy(3);
        fx(0) = (*dForce)(0)*(d2*sin(theta(1))+La0)/d0;
        fx(1) = (*dForce)(1)*sin(theta(0));
        fx(2) = (*dForce)(2)*sin(theta(1));
        fy(0) = (*dForce)(0)*(d2*cos(theta(1))-La1)/d0;
        fy(1) = (*dForce)(1)*cos(theta(0));
        fy(2) = (*dForce)(2)*cos(theta(1));
        
        f(0) = -fx(0) - fx(1) - fx(2);
        f(1) = fy(0) + fy(1) + fy(2);
        f(2) = ((fx(0)+fx(2))*L1 - fx(1)*L0)*sin(disp2) + ((fy(0)+fy(2))*L1 - fy(1)*L0)*cos(disp2);
    }
    
    // assemble directions
    for (int i=0; i<numDOF; i++)  {
        (*force)(DOF(i)) = f(i);
    }
    
    return OF_ReturnType_completed;
}


int ESThreeActuators::transfDaqTime(Vector* time)
{  
    *time = *dTime;
    
    return OF_ReturnType_completed;
}
