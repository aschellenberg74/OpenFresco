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
// ESInvertedVBraceJntOff2d class.

#include "ESInvertedVBraceJntOff2d.h"

#include <ExperimentalControl.h>

#include <elementAPI.h>

#include <math.h>


void* OPF_ESInvertedVBraceJntOff2d()
{
    // pointer to experimental setup that will be returned
    ExperimentalSetup* theSetup = 0;
    
    if (OPS_GetNumRemainingInputArgs() < 10) {
        opserr << "WARNING invalid number of arguments\n";
        opserr << "Want: expSetup InvertedVBraceJntOff tag <-control ctrlTag> "
            << "La1 La2 La3 L1 L2 L3 L4 L5 L6 "
            << "<-nlGeom> <-posAct1 pos> <-phiLocX phi>\n";
        return 0;
    }
    
    // setup tag
    int tag;
    int numdata = 1;
    if (OPS_GetIntInput(&numdata, &tag) != 0) {
        opserr << "WARNING invalid expSetup InvertedVBraceJntOff tag\n";
        return 0;
    }
    
    // control tag (optional)
    ExperimentalControl* theControl = 0;
    const char* type = OPS_GetString();
    if (strcmp(type, "-control") == 0 || strcmp(type, "-ctrl") == 0) {
        int ctrlTag;
        numdata = 1;
        if (OPS_GetIntInput(&numdata, &ctrlTag) < 0) {
            opserr << "WARNING invalid ctrlTag\n";
            opserr << "expSetup InvertedVBraceJntOff " << tag << endln;
            return 0;
        }
        theControl = OPF_getExperimentalControl(ctrlTag);
        if (theControl == 0) {
            opserr << "WARNING experimental control not found\n";
            opserr << "expControl: " << ctrlTag << endln;
            opserr << "expSetup InvertedVBraceJntOff " << tag << endln;
            return 0;
        }
    }
    else {
        // move current arg back by one
        OPS_ResetCurrentInputArg(-1);
    }
    
    // La1, La2, La3
    double La[3];
    numdata = 3;
    if (OPS_GetDoubleInput(&numdata, La) != 0) {
        opserr << "WARNING invalid La1, La2, or La3\n";
        opserr << "expSetup InvertedVBraceJntOff " << tag << endln;
        return 0;
    }
    
    // L1, L2, L3, L4, L5, L6
    double L[6];
    numdata = 6;
    if (OPS_GetDoubleInput(&numdata, L) != 0) {
        opserr << "WARNING invalid L1, L2, L3, L4, L5, or L6\n";
        opserr << "expSetup InvertedVBraceJntOff " << tag << endln;
        return 0;
    }
    
    // optional parameters
    int nlGeom = 0;
    char posAct0[6] = { 'l','e','f','t','\0' };
    double phiLocX = 0.0;
    while (OPS_GetNumRemainingInputArgs() > 0) {
        // nlGeom
        type = OPS_GetString();
        if (strcmp(type, "-nlGeom") == 0) {
            nlGeom = 1;
        }
        // posAct1
        else if (strcmp(type, "-posAct1") == 0) {
            const char* pos = OPS_GetString();
            if (strcmp(pos, "left") == 0 || strcmp(pos, "l") == 0)
                strcpy(posAct0, "left");
            else if (strcmp(pos, "right") == 0 || strcmp(pos, "r") == 0)
                strcpy(posAct0, "right");
        }
        // phiLocX
        else if (strcmp(type, "-phiLocX") == 0) {
            numdata = 1;
            if (OPS_GetDoubleInput(&numdata, &phiLocX) != 0) {
                opserr << "WARNING invalid phiLocX\n";
                opserr << "expSetup InvertedVBraceJntOff " << tag << endln;
                return 0;
            }
        }
    }
    
    // parsing was successful, allocate the setup
    theSetup = new ESInvertedVBraceJntOff2d(tag, La[0], La[1], La[2],
        L[0], L[1], L[2], L[3], L[4], L[5], theControl, nlGeom, posAct0, phiLocX);
    if (theSetup == 0) {
        opserr << "WARNING could not create experimental setup of type ESInvertedVBraceJntOff2d\n";
        return 0;
    }
    
    return theSetup;
}


ESInvertedVBraceJntOff2d::ESInvertedVBraceJntOff2d(int tag,
    double actLength0, double actLength1, double actLength2,
    double rigidLength0, double rigidLength1, double rigidLength2,
    double rigidLength3, double rigidLength4, double rigidLength5,
    ExperimentalControl* control,
    int nlgeom, const char *posact0, double philocx)
    : ExperimentalSetup(tag, control),
    La0(actLength0), La1(actLength1), La2(actLength2),
    L0(rigidLength0), L1(rigidLength1), L2(rigidLength2),
    L3(rigidLength3), L4(rigidLength4), L5(rigidLength5),
    nlGeom(nlgeom), phiLocX(philocx), rotLocX(3,3)
{
    strcpy(posAct0,posact0);

    // call setup method
    this->setup();

    for (int i=0; i<3; i++)
        firstWarning[i] = true;
}


ESInvertedVBraceJntOff2d::ESInvertedVBraceJntOff2d(const ESInvertedVBraceJntOff2d& es)
    : ExperimentalSetup(es),
    rotLocX(3,3)
{
    La0     = es.La0;
    La1     = es.La1;
    La2     = es.La2;
    L0      = es.L0;
    L1      = es.L1;
    L2      = es.L2;
    L3      = es.L3;
    L4      = es.L4;
    L5      = es.L5;
    nlGeom  = es.nlGeom;
    phiLocX = es.phiLocX;
    strcpy(posAct0,es.posAct0);

    // call setup method
    this->setup();

    for (int i=0; i<3; i++)
        firstWarning[i] = true;
}


ESInvertedVBraceJntOff2d::~ESInvertedVBraceJntOff2d()
{
    // does nothing
}


int ESInvertedVBraceJntOff2d::setup()
{
    // setup the trial/out vectors
    sizeTrial->Zero();
    sizeOut->Zero();
    for (int i=0; i<OF_Resp_Force; i++)  {
        (*sizeTrial)(i) = 3;
        (*sizeOut)(i) = 3;
    }
    (*sizeTrial)(OF_Resp_Force) = 3;
    (*sizeOut)(OF_Resp_Force) = 6;
    (*sizeTrial)(OF_Resp_Time) = 1;
    (*sizeOut)(OF_Resp_Time) = 1;
    
    this->setTrialOutSize();
    
    // setup the ctrl/daq vectors
    sizeCtrl->Zero();
    sizeDaq->Zero();
    for (int i=0; i<OF_Resp_Force; i++)  {
        (*sizeCtrl)(i) = 3;
        (*sizeDaq)(i) = 3;
    }
    (*sizeCtrl)(OF_Resp_Force) = 3;
    (*sizeDaq)(OF_Resp_Force) = 6;
    (*sizeCtrl)(OF_Resp_Time) = 1;
    (*sizeDaq)(OF_Resp_Time) = 1;
    
    this->setCtrlDaqSize();
    
    // initialize rotation matrix
    rotLocX.Zero();
    double pi = acos(-1.0);
    rotLocX(0,0) = cos(phiLocX/180.0*pi); rotLocX(0,1) = -sin(phiLocX/180.0*pi);
    rotLocX(1,0) = sin(phiLocX/180.0*pi); rotLocX(1,1) =  cos(phiLocX/180.0*pi);
    rotLocX(2,2) = 1.0;
    
    return OF_ReturnType_completed;
}


int ESInvertedVBraceJntOff2d::transfTrialResponse(const Vector* disp, 
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


ExperimentalSetup* ESInvertedVBraceJntOff2d::getCopy()
{
    ESInvertedVBraceJntOff2d *theCopy = new ESInvertedVBraceJntOff2d(*this);
    
    return theCopy;
}


void ESInvertedVBraceJntOff2d::Print(OPS_Stream &s, int flag)
{
    s << "ExperimentalSetup: " << this->getTag(); 
    s << " type: ESInvertedVBraceJntOff2d\n";
    s << " actLength1  : " << La0 << endln;
    s << " actLength2  : " << La1 << endln;
    s << " actLength3  : " << La2 << endln;
    s << " rigidLength1: " << L0 << endln;
    s << " rigidLength2: " << L1 << endln;
    s << " rigidLength3: " << L2 << endln;
    s << " rigidLength4: " << L3 << endln;
    s << " rigidLength5: " << L4 << endln;
    s << " rigidLength6: " << L5 << endln;
    s << " nlGeom      : " << nlGeom << endln;
    s << " posAct1     : " << posAct0 << endln;
    s << " phiLocX     : " << phiLocX << endln;
    if (theControl != 0)  {
        s << "\tExperimentalControl tag: " << theControl->getTag();
        s << *theControl;
    }}


int ESInvertedVBraceJntOff2d::transfTrialDisp(const Vector* disp)
{  
    // rotate direction
    static Vector d(3);
    d = rotLocX*(*disp);

    // linear geometry, horizontal actuator left
    if (nlGeom == 0 && strcmp(posAct0,"left") == 0)  {
        // actuator 0
        (*cDisp)(0) = d(0);
        // actuator 1
        (*cDisp)(1) = d(1) - L1*d(2);
        // actuator 2
        (*cDisp)(2) = d(1) + L2*d(2);
    }
    // linear geometry, horizontal actuator right
    else if (nlGeom == 0 && strcmp(posAct0,"right") == 0)  {
        // actuator 0
        (*cDisp)(0) = -d(0);
        // actuator 1
        (*cDisp)(1) = d(1) - L1*d(2);
        // actuator 2
        (*cDisp)(2) = d(1) + L2*d(2);
    }
    // nonlinear geometry, horizontal actuator left
    else if (nlGeom == 1 && strcmp(posAct0,"left") == 0)  {
        double R0 = sqrt(L1*L1 + L4*L4);
        double R1 = sqrt(L2*L2 + L5*L5);
        double alpha0 = atan2(L4,L1);
        double alpha1 = atan2(L5,L2);
        
        // actuator 0
        (*cDisp)(0) = pow(pow(d(0)-(L0+L1)*cos(d(2))+L0+L1+La0,2.0)+pow(d(1)-(L0+L1)*sin(d(2)),2.0),0.5)-La0;
        // actuator 1
        (*cDisp)(1) = pow(pow(d(0)-R0*cos(alpha0+d(2))+L1,2.0)+pow(d(1)-R0*sin(alpha0+d(2))+La1+L4,2.0),0.5)-La1;
        // actuator 2
        (*cDisp)(2) = pow(pow(d(0)+R1*cos(-alpha1+d(2))-L2,2.0)+pow(d(1)+R1*sin(-alpha1+d(2))+La2+L5,2.0),0.5)-La2;
    }
    // nonlinear geometry, horizontal actuator right
    else if (nlGeom == 1 && strcmp(posAct0,"right") == 0)  {
        double R0 = sqrt(L1*L1 + L4*L4);
        double R1 = sqrt(L2*L2 + L5*L5);
        double alpha0 = atan2(L4,L1);        
        double alpha1 = atan2(L5,L2);
        
        // actuator 0
        (*cDisp)(0) = pow(pow(d(0)+(L2+L3)*cos(d(2))-L2-L3-La0,2.0)+pow(d(1)+(L2+L3)*sin(d(2)),2.0),0.5)-La0;
        // actuator 1
        (*cDisp)(1) = pow(pow(d(0)-R0*cos(alpha0+d(2))+L1,2.0)+pow(d(1)-R0*sin(alpha0+d(2))+La1+L4,2.0),0.5)-La1;
        // actuator 2
        (*cDisp)(2) = pow(pow(d(0)+R1*cos(-alpha1+d(2))-L2,2.0)+pow(d(1)+R1*sin(-alpha1+d(2))+La2+L5,2.0),0.5)-La2;
    }
    
    return OF_ReturnType_completed;
}


int ESInvertedVBraceJntOff2d::transfTrialVel(const Vector* vel)
{
    return OF_ReturnType_completed;
}


int ESInvertedVBraceJntOff2d::transfTrialVel(const Vector* disp,
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
        (*cVel)(1) = v(1) - L1*v(2);
        // actuator 2
        (*cVel)(2) = v(1) + L2*v(2);
    }
    // linear geometry, horizontal actuator right
    else if (nlGeom == 0 && strcmp(posAct0,"right") == 0)  {
        // actuator 0
        (*cVel)(0) = -v(0);
        // actuator 1
        (*cVel)(1) = v(1) - L1*v(2);
        // actuator 2
        (*cVel)(2) = v(1) + L2*v(2);
    }
    // nonlinear geometry, horizontal actuator left
    else if (nlGeom == 1 && strcmp(posAct0,"left") == 0)  {
        double R0 = sqrt(L1*L1 + L4*L4);
        double R1 = sqrt(L2*L2 + L5*L5);
        double alpha0 = atan2(L4,L1);
        double alpha1 = atan2(L5,L2);
        
        // actuator 0
        (*cVel)(0) = 0.5*(2.0*(d(0)-(L0+L1)*cos(d(2))+L0+L1+La0)*(v(0)+(L0+L1)*sin(d(2))*v(2))+2.0*(d(1)-(L0+L1)*sin(d(2)))*(v(1)-(L0+L1)*cos(d(2))*v(2)))/pow(pow(d(0)-(L0+L1)*cos(d(2))+L0+L1+La0,2.0)+pow(d(1)-(L0+L1)*sin(d(2)),2.0),0.5);
        // actuator 1
        (*cVel)(1) = 0.5*(2.0*(d(0)-R0*cos(alpha0+d(2))+L1)*(v(0)+R0*sin(alpha0+d(2))*v(2))+2.0*(d(1)-R0*sin(alpha0+d(2))+La1+L4)*(v(1)-R0*cos(alpha0+d(2))*v(2)))/pow(pow(d(0)-R0*cos(alpha0+d(2))+L1,2.0)+pow(d(1)-R0*sin(alpha0+d(2))+La1+L4,2.0),0.5);
        // actuator 2
        (*cVel)(2) = 0.5*(2.0*(d(0)+R1*cos(-alpha1+d(2))-L2)*(v(0)-R1*sin(-alpha1+d(2))*v(2))+2.0*(d(1)+R1*sin(-alpha1+d(2))+La2+L5)*(v(1)+R1*cos(-alpha1+d(2))*v(2)))/pow(pow(d(0)+R1*cos(-alpha1+d(2))-L2,2.0)+pow(d(1)+R1*sin(-alpha1+d(2))+La2+L5,2.0),0.5);
    }
    // nonlinear geometry, horizontal actuator right
    else if (nlGeom == 1 && strcmp(posAct0,"right") == 0)  {
        double R0 = sqrt(L1*L1 + L4*L4);
        double R1 = sqrt(L2*L2 + L5*L5);
        double alpha0 = atan2(L4,L1);        
        double alpha1 = atan2(L5,L2);
        
        // actuator 0
        (*cVel)(0) = 0.5*(2.0*(d(0)+(L2+L3)*cos(d(2))-L2-L3-La0)*(v(0)-(L2+L3)*sin(d(2))*v(2))+2.0*(d(1)+(L2+L3)*sin(d(2)))*(v(1)+(L2+L3)*cos(d(2))*v(2)))/pow(pow(d(0)+(L2+L3)*cos(d(2))-L2-L3-La0,2.0)+pow(d(1)+(L2+L3)*sin(d(2)),2.0),0.5);
        // actuator 1
        (*cVel)(1) = 0.5*(2.0*(d(0)-R0*cos(alpha0+d(2))+L1)*(v(0)+R0*sin(alpha0+d(2))*v(2))+2.0*(d(1)-R0*sin(alpha0+d(2))+La1+L4)*(v(1)-R0*cos(alpha0+d(2))*v(2)))/pow(pow(d(0)-R0*cos(alpha0+d(2))+L1,2.0)+pow(d(1)-R0*sin(alpha0+d(2))+La1+L4,2.0),0.5);
        // actuator 2
        (*cVel)(2) = 0.5*(2.0*(d(0)+R1*cos(-alpha1+d(2))-L2)*(v(0)-R1*sin(-alpha1+d(2))*v(2))+2.0*(d(1)+R1*sin(-alpha1+d(2))+La2+L5)*(v(1)+R1*cos(-alpha1+d(2))*v(2)))/pow(pow(d(0)+R1*cos(-alpha1+d(2))-L2,2.0)+pow(d(1)+R1*sin(-alpha1+d(2))+La2+L5,2.0),0.5);
    }
    
    return OF_ReturnType_completed;
}


int ESInvertedVBraceJntOff2d::transfTrialAccel(const Vector* accel)
{
    return OF_ReturnType_completed;
}


int ESInvertedVBraceJntOff2d::transfTrialAccel(const Vector* disp,
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
        (*cAccel)(1) = a(1) - L1*a(2);
        // actuator 2
        (*cAccel)(2) = a(1) + L2*a(2);
    }
    // linear geometry, horizontal actuator right
    else if (nlGeom == 0 && strcmp(posAct0,"right") == 0)  {
        // actuator 0
        (*cAccel)(0) = -a(0);
        // actuator 1
        (*cAccel)(1) = a(1) - L1*a(2);
        // actuator 2
        (*cAccel)(2) = a(1) + L2*a(2);
    }
    // nonlinear geometry, horizontal actuator left
    else if (nlGeom == 1 && strcmp(posAct0,"left") == 0)  {
        double R0 = sqrt(L1*L1 + L4*L4);
        double R1 = sqrt(L2*L2 + L5*L5);
        double alpha0 = atan2(L4,L1);
        double alpha1 = atan2(L5,L2);
        
        // actuator 0
        (*cAccel)(0) = -0.25*pow(2.0*(d(0)-(L0+L1)*cos(d(2))+L0+L1+La0)*(v(0)+(L0+L1)*sin(d(2))*v(2))+2.0*(d(1)-(L0+L1)*sin(d(2)))*(v(1)-(L0+L1)*cos(d(2))*v(2)),2.0)/pow(pow(d(0)-(L0+L1)*cos(d(2))+L0+L1+La0,2.0)+pow(d(1)-(L0+L1)*sin(d(2)),2.0),1.5)+0.5*(2.0*pow(v(0)+(L0+L1)*sin(d(2))*v(2),2.0)+2.0*(d(0)-(L0+L1)*cos(d(2))+L0+L1+La0)*(a(0)+(L0+L1)*cos(d(2))*pow(v(2),2.0)+(L0+L1)*sin(d(2))*a(2))+2.0*pow(v(1)-(L0+L1)*cos(d(2))*v(2),2.0)+2.0*(d(1)-(L0+L1)*sin(d(2)))*(a(1)+(L0+L1)*sin(d(2))*pow(v(2),2.0)-(L0+L1)*cos(d(2))*a(2)))/pow(pow(d(0)-(L0+L1)*cos(d(2))+L0+L1+La0,2.0)+pow(d(1)-(L0+L1)*sin(d(2)),2.0),0.5);
        // actuator 1
        (*cAccel)(1) = -0.25*pow(2.0*(d(0)-R0*cos(alpha0+d(2))+L1)*(v(0)+R0*sin(alpha0+d(2))*v(2))+2.0*(d(1)-R0*sin(alpha0+d(2))+La1+L4)*(v(1)-R0*cos(alpha0+d(2))*v(2)),2.0)/pow(pow(d(0)-R0*cos(alpha0+d(2))+L1,2.0)+pow(d(1)-R0*sin(alpha0+d(2))+La1+L4,2.0),1.5)+0.5*(2.0*pow(v(0)+R0*sin(alpha0+d(2))*v(2),2.0)+2.0*(d(0)-R0*cos(alpha0+d(2))+L1)*(a(0)+R0*cos(alpha0+d(2))*pow(v(2),2.0)+R0*sin(alpha0+d(2))*a(2))+2.0*pow(v(1)-R0*cos(alpha0+d(2))*v(2),2.0)+2.0*(d(1)-R0*sin(alpha0+d(2))+La1+L4)*(a(1)+R0*sin(alpha0+d(2))*pow(v(2),2.0)-R0*cos(alpha0+d(2))*a(2)))/pow(pow(d(0)-R0*cos(alpha0+d(2))+L1,2.0)+pow(d(1)-R0*sin(alpha0+d(2))+La1+L4,2.0),0.5);
        // actuator 2
        (*cAccel)(2) = -0.25*pow(2.0*(d(0)+R1*cos(-alpha1+d(2))-L2)*(v(0)-R1*sin(-alpha1+d(2))*v(2))+2.0*(d(1)+R1*sin(-alpha1+d(2))+La2+L5)*(v(1)+R1*cos(-alpha1+d(2))*v(2)),2.0)/pow(pow(d(0)+R1*cos(-alpha1+d(2))-L2,2.0)+pow(d(1)+R1*sin(-alpha1+d(2))+La2+L5,2.0),1.5)+0.5*(2.0*pow(v(0)-R1*sin(-alpha1+d(2))*v(2),2.0)+2.0*(d(0)+R1*cos(-alpha1+d(2))-L2)*(a(0)-R1*cos(-alpha1+d(2))*pow(v(2),2.0)-R1*sin(-alpha1+d(2))*a(2))+2.0*pow(v(1)+R1*cos(-alpha1+d(2))*v(2),2.0)+2.0*(d(1)+R1*sin(-alpha1+d(2))+La2+L5)*(a(1)-R1*sin(-alpha1+d(2))*pow(v(2),2.0)+R1*cos(-alpha1+d(2))*a(2)))/pow(pow(d(0)+R1*cos(-alpha1+d(2))-L2,2.0)+pow(d(1)+R1*sin(-alpha1+d(2))+La2+L5,2.0),0.5);
    }
    // nonlinear geometry, horizontal actuator right
    else if (nlGeom == 1 && strcmp(posAct0,"right") == 0)  {
        double R0 = sqrt(L1*L1 + L4*L4);
        double R1 = sqrt(L2*L2 + L5*L5);
        double alpha0 = atan2(L4,L1);        
        double alpha1 = atan2(L5,L2);
        
        // actuator 0
        (*cAccel)(0) = -0.25*pow(2.0*(d(0)+(L2+L3)*cos(d(2))-L2-L3-La0)*(v(0)-(L2+L3)*sin(d(2))*v(2))+2.0*(d(1)+(L2+L3)*sin(d(2)))*(v(1)+(L2+L3)*cos(d(2))*v(2)),2.0)/pow(pow(d(0)+(L2+L3)*cos(d(2))-L2-L3-La0,2.0)+pow(d(1)+(L2+L3)*sin(d(2)),2.0),1.5)+0.5*(2.0*pow(v(0)-(L2+L3)*sin(d(2))*v(2),2.0)+2.0*(d(0)+(L2+L3)*cos(d(2))-L2-L3-La0)*(a(0)-(L2+L3)*cos(d(2))*pow(v(2),2.0)-(L2+L3)*sin(d(2))*a(2))+2.0*pow(v(1)+(L2+L3)*cos(d(2))*v(2),2.0)+2.0*(d(1)+(L2+L3)*sin(d(2)))*(a(1)-(L2+L3)*sin(d(2))*pow(v(2),2.0)+(L2+L3)*cos(d(2))*a(2)))/pow(pow(d(0)+(L2+L3)*cos(d(2))-L2-L3-La0,2.0)+pow(d(1)+(L2+L3)*sin(d(2)),2.0),0.5);
        // actuator 1
        (*cAccel)(1) = -0.25*pow(2.0*(d(0)-R0*cos(alpha0+d(2))+L1)*(v(0)+R0*sin(alpha0+d(2))*v(2))+2.0*(d(1)-R0*sin(alpha0+d(2))+La1+L4)*(v(1)-R0*cos(alpha0+d(2))*v(2)),2.0)/pow(pow(d(0)-R0*cos(alpha0+d(2))+L1,2.0)+pow(d(1)-R0*sin(alpha0+d(2))+La1+L4,2.0),1.5)+0.5*(2.0*pow(v(0)+R0*sin(alpha0+d(2))*v(2),2.0)+2.0*(d(0)-R0*cos(alpha0+d(2))+L1)*(a(0)+R0*cos(alpha0+d(2))*pow(v(2),2.0)+R0*sin(alpha0+d(2))*a(2))+2.0*pow(v(1)-R0*cos(alpha0+d(2))*v(2),2.0)+2.0*(d(1)-R0*sin(alpha0+d(2))+La1+L4)*(a(1)+R0*sin(alpha0+d(2))*pow(v(2),2.0)-R0*cos(alpha0+d(2))*a(2)))/pow(pow(d(0)-R0*cos(alpha0+d(2))+L1,2.0)+pow(d(1)-R0*sin(alpha0+d(2))+La1+L4,2.0),0.5);
        // actuator 2
        (*cAccel)(2) = -0.25*pow(2.0*(d(0)+R1*cos(-alpha1+d(2))-L2)*(v(0)-R1*sin(-alpha1+d(2))*v(2))+2.0*(d(1)+R1*sin(-alpha1+d(2))+La2+L5)*(v(1)+R1*cos(-alpha1+d(2))*v(2)),2.0)/pow(pow(d(0)+R1*cos(-alpha1+d(2))-L2,2.0)+pow(d(1)+R1*sin(-alpha1+d(2))+La2+L5,2.0),1.5)+0.5*(2.0*pow(v(0)-R1*sin(-alpha1+d(2))*v(2),2.0)+2.0*(d(0)+R1*cos(-alpha1+d(2))-L2)*(a(0)-R1*cos(-alpha1+d(2))*pow(v(2),2.0)-R1*sin(-alpha1+d(2))*a(2))+2.0*pow(v(1)+R1*cos(-alpha1+d(2))*v(2),2.0)+2.0*(d(1)+R1*sin(-alpha1+d(2))+La2+L5)*(a(1)-R1*sin(-alpha1+d(2))*pow(v(2),2.0)+R1*cos(-alpha1+d(2))*a(2)))/pow(pow(d(0)+R1*cos(-alpha1+d(2))-L2,2.0)+pow(d(1)+R1*sin(-alpha1+d(2))+La2+L5,2.0),0.5);
    }
    
    return OF_ReturnType_completed;
}


int ESInvertedVBraceJntOff2d::transfTrialForce(const Vector* force)
{  
    // rotate direction
    static Vector f(3);
    f = rotLocX*(*force);

    // linear geometry, horizontal actuator left
    if (nlGeom == 0 && strcmp(posAct0,"left") == 0)  {
        // actuator 0
        (*cForce)(0) = f(0);
        // actuator 1
        (*cForce)(1) = 1.0/(L1+L2)*(L2*f(1) - f(2));
        // actuator 2
        (*cForce)(2) = 1.0/(L1+L2)*(L1*f(1) + f(2));
    }
    // linear geometry, horizontal actuator right
    else if (nlGeom == 0 && strcmp(posAct0,"right") == 0)  {
        // actuator 0
        (*cForce)(0) = -f(0);
        // actuator 1
        (*cForce)(1) = 1.0/(L1+L2)*(L2*f(1) - f(2));
        // actuator 2
        (*cForce)(2) = 1.0/(L1+L2)*(L1*f(1) + f(2));
    }
    // nonlinear geometry, horizontal actuator left
    else if (nlGeom == 1 && strcmp(posAct0,"left") == 0)  {
        if (firstWarning[0] == true)  {
            opserr << "WARNING ESInvertedVBraceJntOff2d::transfTrialForce() - "
                << "nonlinear geometry with horizontal actuator left "
                << "not implemented yet. Using linear geometry instead.\n\n";
            firstWarning[0] = false;
        }
        // actuator 0
        (*cForce)(0) = f(0);
        // actuator 1
        (*cForce)(1) = 1.0/(L1+L2)*(L2*f(1) - f(2));
        // actuator 2
        (*cForce)(2) = 1.0/(L1+L2)*(L1*f(1) + f(2));
    }
    // nonlinear geometry, horizontal actuator right
    else if (nlGeom == 1 && strcmp(posAct0,"right") == 0)  {
        if (firstWarning[0] == true)  {
            opserr << "WARNING ESInvertedVBraceJntOff2d::transfTrialForce() - "
                << "nonlinear geometry with horizontal actuator right "
                << "not implemented yet. Using linear geometry instead.\n\n";
            firstWarning[0] = false;
        }
        // actuator 0
        (*cForce)(0) = -f(0);
        // actuator 1
        (*cForce)(1) = 1.0/(L1+L2)*(L2*f(1) - f(2));
        // actuator 2
        (*cForce)(2) = 1.0/(L1+L2)*(L1*f(1) + f(2));
    }
    
    return OF_ReturnType_completed;
}


int ESInvertedVBraceJntOff2d::transfTrialTime(const Vector* time)
{
    *cTime = *time;
    
    return OF_ReturnType_completed;
}

int ESInvertedVBraceJntOff2d::transfDaqDisp(Vector* disp)
{ 
    // linear geometry, horizontal actuator left
    if (nlGeom == 0 && strcmp(posAct0,"left") == 0)  {
        (*disp)(0) = (*dDisp)(0);
        (*disp)(1) = 1.0/(L1+L2)*(L2*(*dDisp)(1) + L1*(*dDisp)(2));
        (*disp)(2) = 1.0/(L1+L2)*(-(*dDisp)(1) + (*dDisp)(2));
    }
    // linear geometry, horizontal actuator right
    else if (nlGeom == 0 && strcmp(posAct0,"right") == 0)  {
        (*disp)(0) = -(*dDisp)(0);
        (*disp)(1) = 1.0/(L1+L2)*(L2*(*dDisp)(1) + L1*(*dDisp)(2));
        (*disp)(2) = 1.0/(L1+L2)*(-(*dDisp)(1) + (*dDisp)(2));
    }
    // nonlinear geometry, horizontal actuator left
    else if (nlGeom == 1 && strcmp(posAct0,"left") == 0)  {
        Vector F(3), theta(3), dTheta(3);
        Matrix DF(3,3);
        int iter = 0;
        int maxIter = 15;
        double tol = 1E-9;
        
        double d0 = La0 + (*dDisp)(0);
        double d1 = La1 + (*dDisp)(1);
        double d2 = La2 + (*dDisp)(2);
        
        theta(0) = (*dDisp)(1)/La0;
        theta(1) = (*dDisp)(0)/La1;
        theta(2) = (*dDisp)(0)/La2;
        
        do  {
            F(0) = L0*L0 + L4*L4 - pow(d0*cos(theta(0))-La0-L0-d1*sin(theta(1)),2.0) - pow(d0*sin(theta(0))+La1+L4-d1*cos(theta(1)),2.0);
            F(1) = pow(L1+L2,2.0) + pow(L4-L5,2.0) - pow(d2*sin(theta(2))+L1+L2-d1*sin(theta(1)),2.0) - pow(d2*cos(theta(2))+La1+L4-L5-La2-d1*cos(theta(1)),2.0);
            F(2) = pow(L0+L1+L2,2.0) + L5*L5 - pow(d0*cos(theta(0))-La0-L0-d2*sin(theta(2))-L1-L2,2.0) - pow(d0*sin(theta(0))+La2+L5-d2*cos(theta(2)),2.0);
            
            DF(0,0) = 2.0*d0*(-(L4+La1)*cos(theta(0))+d1*cos(theta(1)+theta(0))-(L0+La0)*sin(theta(0)));
            DF(0,1) = 2.0*d1*(-(L0+La0)*cos(theta(1))+d0*cos(theta(1)+theta(0))-(L4+La1)*sin(theta(1)));
            DF(0,2) = 0.0;
            DF(1,0) = 0.0;
            DF(1,1) = 2.0*d1*((L1+L2)*cos(theta(1))-d2*sin(theta(1)-theta(2))-(L4-L5+La1-La2)*sin(theta(1)));
            DF(1,2) = 2.0*d2*(-(L1+L2)*cos(theta(2))+d1*sin(theta(1)-theta(2))+(L4-L5+La1-La2)*sin(theta(2)));
            DF(2,0) = 2.0*d0*(-(L5+La2)*cos(theta(0))+d2*cos(theta(2)+theta(0))-(L0+L1+L2+La0)*sin(theta(0)));
            DF(2,1) = 0.0;
            DF(2,2) = 2.0*d2*(-(L0+L1+L2+La0)*cos(theta(2))+d0*cos(theta(2)+theta(0))-(L5+La2)*sin(theta(2)));
            
            // Newton’s method
            dTheta = F/DF;
            theta -= dTheta;
            iter++;
        }  while ((dTheta.Norm() >= tol) && (iter <= maxIter));
        
        // issue warning if iteration did not converge
        if (iter >= maxIter)   {
            opserr << "WARNING ESInvertedVBraceJntOff2d::transfDaqDisp() - "
                << "did not find the angle theta after "
                << iter << " iterations and norm: " << dTheta.Norm() << endln;
        }

        (*disp)(2) = atan2(d2*cos(theta(2))+La1+L4-L5-La2-d1*cos(theta(1)),d2*sin(theta(2))+L1+L2-d1*sin(theta(1))) - atan2(L4-L5,L1+L2);
        double R0 = sqrt(L1*L1 + L4*L4);
        double beta0 = atan2(L4,L1) + (*disp)(2);
        (*disp)(0) = d1*sin(theta(1)) + R0*cos(beta0) - L1;
        (*disp)(1) = d1*cos(theta(1)) + R0*sin(beta0) - La1 - L4;
    }
    // nonlinear geometry, horizontal actuator right
    else if (nlGeom == 1 && strcmp(posAct0,"right") == 0)  {
        Vector F(3), theta(3), dTheta(3);
        Matrix DF(3,3);
        int iter = 0;
        int maxIter = 15;
        double tol = 1E-9;
        
        double d0 = La0 + (*dDisp)(0);
        double d1 = La1 + (*dDisp)(1);
        double d2 = La2 + (*dDisp)(2);
        
        theta(0) = (*dDisp)(2)/La0;
        theta(1) = (*dDisp)(0)/La1;
        theta(2) = (*dDisp)(0)/La2;
        
        do  {
            F(0) = pow(L1+L2+L3,2.0) + L4*L4 - pow(-d0*cos(theta(0))+La0+L1+L2+L3+d1*sin(theta(1)),2.0) - pow(d0*sin(theta(0))+La1+L4-d1*cos(theta(1)),2.0);
            F(1) = pow(L1+L2,2.0) + pow(L4-L5,2.0) - pow(-d2*sin(theta(2))+L1+L2+d1*sin(theta(1)),2.0) - pow(d2*cos(theta(2))+La1+L4-L5-La2-d1*cos(theta(1)),2.0);
            F(2) = L3*L3 + L5*L5 - pow(-d0*cos(theta(0))+La0+L3+d2*sin(theta(2)),2.0) - pow(d0*sin(theta(0))+La2+L5-d2*cos(theta(2)),2.0);
            
            DF(0,0) = 2.0*d0*(-(L4+La1)*cos(theta(0))+d1*cos(theta(1)+theta(0))-(L1+L2+L3+La0)*sin(theta(0)));
            DF(0,1) = 2.0*d1*(-(L1+L2+L3+La0)*cos(theta(1))+d0*cos(theta(1)+theta(0))-(L4+La1)*sin(theta(1)));
            DF(0,2) = 0.0;
            DF(1,0) = 0.0;
            DF(1,1) = 2.0*d1*(-(L1+L2)*cos(theta(1))-d2*sin(theta(1)-theta(2))-(L4-L5+La1-La2)*sin(theta(1)));
            DF(1,2) = 2.0*d2*((L1+L2)*cos(theta(2))+d1*sin(theta(1)-theta(2))+(L4-L5+La1-La2)*sin(theta(2)));
            DF(2,0) = 2.0*d0*(-(L5+La2)*cos(theta(0))+d2*cos(theta(2)+theta(0))-(L3+La0)*sin(theta(0)));
            DF(2,1) = 0.0;
            DF(2,2) = 2.0*d2*(-(L3+La0)*cos(theta(2))+d0*cos(theta(2)+theta(0))-(L5+La2)*sin(theta(2)));
            
            // Newton’s method
            dTheta = F/DF;
            theta -= dTheta;
            iter++;
        }  while ((dTheta.Norm() >= tol) && (iter <= maxIter));
        
        // issue warning if iteration did not converge
        if (iter >= maxIter)   {
            opserr << "WARNING ESInvertedVBraceJntOff2d::transfDaqDisp() - "
                << "did not find the angle theta after "
                << iter << " iterations and norm: " << dTheta.Norm() << endln;
        }

        (*disp)(2) = atan2(d2*cos(theta(2))+La1+L4-L5-La2-d1*cos(theta(1)),-d2*sin(theta(2))+L1+L2+d1*sin(theta(1))) - atan2(L4-L5,L1+L2);
        double R0 = sqrt(L1*L1 + L4*L4);
        double beta0 = atan2(L4,L1) + (*disp)(2);
        (*disp)(0) = -d1*sin(theta(1)) + R0*cos(beta0) - L1;
        (*disp)(1) = d1*cos(theta(1)) + R0*sin(beta0) - La1 - L4;
    }
        
    // rotate direction if necessary
    if (phiLocX != 0.0)  {
        (*disp) = rotLocX^(*disp);
    }

    return OF_ReturnType_completed;
}


int ESInvertedVBraceJntOff2d::transfDaqVel(Vector* vel)
{
    // linear geometry, horizontal actuator left
    if (nlGeom == 0 && strcmp(posAct0,"left") == 0)  {
        (*vel)(0) = (*dVel)(0);
        (*vel)(1) = 1.0/(L1+L2)*(L2*(*dVel)(1) + L1*(*dVel)(2));
        (*vel)(2) = 1.0/(L1+L2)*(-(*dVel)(1) + (*dVel)(2));
    }
    // linear geometry, horizontal actuator right
    else if (nlGeom == 0 && strcmp(posAct0,"right") == 0)  {
        (*vel)(0) = -(*dVel)(0);
        (*vel)(1) = 1.0/(L1+L2)*(L2*(*dVel)(1) + L1*(*dVel)(2));
        (*vel)(2) = 1.0/(L1+L2)*(-(*dVel)(1) + (*dVel)(2));
    }
    // nonlinear geometry, horizontal actuator left
    else if (nlGeom == 1 && strcmp(posAct0,"left") == 0)  {
        if (firstWarning[1] == true)  {
            opserr << "WARNING ESInvertedVBraceJntOff2d::transfDaqVel() - "
                << "nonlinear geometry with horizontal actuator left "
                << "not implemented yet. Using linear geometry instead.\n\n";
            firstWarning[1] = false;
        }
        (*vel)(0) = (*dVel)(0);
        (*vel)(1) = 1.0/(L1+L2)*(L2*(*dVel)(1) + L1*(*dVel)(2));
        (*vel)(2) = 1.0/(L1+L2)*(-(*dVel)(1) + (*dVel)(2));
    }
    // nonlinear geometry, horizontal actuator right
    else if (nlGeom == 1 && strcmp(posAct0,"right") == 0)  {
        if (firstWarning[1] == true)  {
            opserr << "WARNING ESInvertedVBraceJntOff2d::transfDaqVel() - "
                << "nonlinear geometry with horizontal actuator right "
                << "not implemented yet. Using linear geometry instead.\n\n";
            firstWarning[1] = false;
        }
        (*vel)(0) = -(*dVel)(0);
        (*vel)(1) = 1.0/(L1+L2)*(L2*(*dVel)(1) + L1*(*dVel)(2));
        (*vel)(2) = 1.0/(L1+L2)*(-(*dVel)(1) + (*dVel)(2));
    }
    
    // rotate direction if necessary
    if (phiLocX != 0.0)  {
        (*vel) = rotLocX^(*vel);
    }

    return OF_ReturnType_completed;
}


int ESInvertedVBraceJntOff2d::transfDaqAccel(Vector* accel)
{
    // linear geometry, horizontal actuator left
    if (nlGeom == 0 && strcmp(posAct0,"left") == 0)  {
        (*accel)(0) = (*dAccel)(0);
        (*accel)(1) = 1.0/(L1+L2)*(L2*(*dAccel)(1) + L1*(*dAccel)(2));
        (*accel)(2) = 1.0/(L1+L2)*(-(*dAccel)(1) + (*dAccel)(2));
    }
    // linear geometry, horizontal actuator right
    else if (nlGeom == 0 && strcmp(posAct0,"right") == 0)  {
        (*accel)(0) = -(*dAccel)(0);
        (*accel)(1) = 1.0/(L1+L2) * (L2*(*dAccel)(1) + L1*(*dAccel)(2));
        (*accel)(2) = 1.0/(L1+L2) * (-(*dAccel)(1) + (*dAccel)(2));
    }
    // nonlinear geometry, horizontal actuator left
    else if (nlGeom == 1 && strcmp(posAct0,"left") == 0)  {
        if (firstWarning[2] == true)  {
            opserr << "WARNING ESInvertedVBraceJntOff2d::transfDaqAccel() - "
                << "nonlinear geometry with horizontal actuator left "
                << "not implemented yet. Using linear geometry instead.\n\n";
            firstWarning[2] = false;
        }
        (*accel)(0) = (*dAccel)(0);
        (*accel)(1) = 1.0/(L1+L2)*(L2*(*dAccel)(1) + L1*(*dAccel)(2));
        (*accel)(2) = 1.0/(L1+L2)*(-(*dAccel)(1) + (*dAccel)(2));
    }
    // nonlinear geometry, horizontal actuator right
    else if (nlGeom == 1 && strcmp(posAct0,"right") == 0)  {
        if (firstWarning[2] == true)  {
            opserr << "WARNING ESInvertedVBraceJntOff2d::transfDaqAccel() - "
                << "nonlinear geometry with horizontal actuator right "
                << "not implemented yet. Using linear geometry instead.\n\n";
            firstWarning[2] = false;
        }
        (*accel)(0) = -(*dAccel)(0);
        (*accel)(1) = 1.0/(L1+L2) * (L2*(*dAccel)(1) + L1*(*dAccel)(2));
        (*accel)(2) = 1.0/(L1+L2) * (-(*dAccel)(1) + (*dAccel)(2));
    }
    
    // rotate direction if necessary
    if (phiLocX != 0.0)  {
        (*accel) = rotLocX^(*accel);
    }

    return OF_ReturnType_completed;
}


int ESInvertedVBraceJntOff2d::transfDaqForce(Vector* force)
{
    *force = *dForce;
    
    return OF_ReturnType_completed;
}


int ESInvertedVBraceJntOff2d::transfDaqTime(Vector* time)
{  
    *time = *dTime;
    
    return OF_ReturnType_completed;
}
