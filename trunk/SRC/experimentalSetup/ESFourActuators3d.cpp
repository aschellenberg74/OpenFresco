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

// $Revision: 83 $
// $Date: 2007-07-30 21:06:42 -0700 (Mon, 30 Jul 2007) $
// $URL: $

// Written: Vesna Terzic (vesna@berkeley.edu)
// Created: 01/08
// Revision: A
//
// Description: This file contains the implementation of the
// ESFourActuators3d class.

#include "ESFourActuators3d.h"

#include <math.h>
#include <stdlib.h>


ESFourActuators3d::ESFourActuators3d(int tag,
	double actLength1, double actLength2, double actLength3, double actLength4,
	double rigidLength1, double rigidLength2, double rigidLength3, double rigidLength4,
	double rigidHeight, double rigidHeight1, double rigidHeight2,
	double rigidLinkN, double rigidLinkS, double rodLengthN, double rodLengthS,
	ExperimentalControl* control,
	int nlgeom, double philocx)
    : ExperimentalSetup(tag, control),
    L1(actLength1), L2(actLength2), L3(actLength3), L4(actLength4),
    a1(rigidLength1), a2(rigidLength2), a3(rigidLength3), a4(rigidLength4),
	h(rigidHeight), h1(rigidHeight1), h2(rigidHeight2),
	arlN(rigidLinkN), arlS(rigidLinkS), LrodN(rodLengthN), LrodS(rodLengthS),
    nlGeom(nlgeom), phiLocX(philocx), rotLocX(4,4), D(4), d(4)
{
	uxbeam = 0.0;
	uybeam = 0.0;
	upS = 0.0;
	upN = 0.0;
	theta1 = 0.0;
	theta2 = 0.0;
	theta3 = 0.0;
	theta4 = 0.0;
	theta11 = 0.0;
	theta22 = 0.0;
	theta33 = 0.0;
	theta44 = 0.0;
    
    // call setup method
    this->setup();
    
    for (int i=0; i<3; i++)
        firstWarning[i] = true;
}


ESFourActuators3d::ESFourActuators3d(const ESFourActuators3d& es)
    : ExperimentalSetup(es),
    rotLocX(4,4), D(4), d(4)
{
    L1      = es.L1;
    L2      = es.L2;
    L3      = es.L3;
	L4      = es.L4;
    a1      = es.a1;
    a2      = es.a2;
	a3      = es.a3;
	a4      = es.a4;
	h       = es.h;
	h1      = es.h1;
	h2      = es.h2;
	arlN    = es.arlN;
	arlS    = es.arlS;
	LrodN   = es.LrodN;
	LrodS   = es.LrodS;
    nlGeom  = es.nlGeom;
	phiLocX = es.phiLocX;
    
    // call setup method
    this->setup();
    
    for (int i=0; i<3; i++)
        firstWarning[i] = true;
}


ESFourActuators3d::~ESFourActuators3d()
{
    // does nothing
}


int ESFourActuators3d::setSize(ID sizeT, ID sizeO)
{
    // check sizeTrial and sizeOut
    // for ESFourActuators3d object
    
    // a component of sizeT/sizeO must be equal
    // to 6 if it is non-zero.
    for (int i=0; i<OF_Resp_Time; i++) {
        if ((sizeT[i] != 0 && sizeT[i] != 6) ||
            (sizeO[i] != 0 && sizeO[i] != 6)) {
            opserr << "ESFourActuators3d::setSize - wrong sizeTrial/Out\n"; 
            opserr << "see User Manual.\n";
            opserr << "sizeT = " << sizeT;
            opserr << "sizeO = " << sizeO;
            return OF_ReturnType_failed;
        }
    }
    if ((sizeT[OF_Resp_Time] != 0 && sizeT[OF_Resp_Time] != 1) ||
        (sizeO[OF_Resp_Time] != 0 && sizeO[OF_Resp_Time] != 1)) {
        opserr << "ESFourActuators3d::setSize - wrong sizeTrial/Out\n"; 
        opserr << "see User Manual.\n";
        opserr << "sizeT = " << sizeT;
        opserr << "sizeO = " << sizeO;
        return OF_ReturnType_failed;
    }
    
    return OF_ReturnType_completed;
}


int ESFourActuators3d::commitState()
{
    return theControl->commitState();
}


int ESFourActuators3d::setup()
{
    // setup for ctrl/daq vectors of ESFourActuators3d
    sizeCtrl->Zero();
    (*sizeCtrl)[OF_Resp_Disp] = 4;
    (*sizeCtrl)[OF_Resp_Time] = 1;

    sizeDaq->Zero();
    (*sizeDaq)[OF_Resp_Force] = 6;
    (*sizeDaq)[OF_Resp_Time]  = 1;
    
    this->setCtrlDaqSize();

    // initialize rotation matrix
    rotLocX.Zero();
    double pi = acos(-1.0);
    rotLocX(0,0) =  cos(phiLocX/180.0*pi); rotLocX(0,1) = sin(phiLocX/180.0*pi);
    rotLocX(1,0) = -sin(phiLocX/180.0*pi); rotLocX(1,1) = cos(phiLocX/180.0*pi);
	rotLocX(2,2) =  cos(phiLocX/180.0*pi); rotLocX(2,3) = sin(phiLocX/180.0*pi);
    rotLocX(3,2) = -sin(phiLocX/180.0*pi); rotLocX(3,3) = cos(phiLocX/180.0*pi);

	D.Zero();
	d.Zero();

    return OF_ReturnType_completed;
}


int ESFourActuators3d::transfTrialResponse(const Vector* disp, 
    const Vector* vel,
    const Vector* accel,
    const Vector* force,
    const Vector* time)
{
    // transform data
    if (disp != 0) {
        this->transfTrialDisp(disp);
    }
    if (vel != 0) {
        this->transfTrialVel(vel);
    }
    if (accel != 0) {
        this->transfTrialAccel(accel);
    }
    if (force != 0) {
        this->transfTrialForce(force);
    }
    if (time != 0) {
        this->transfTrialTime(time);
    }
    
    return OF_ReturnType_completed;
}


int ESFourActuators3d::transfDaqResponse(Vector* disp,
    Vector* vel,
    Vector* accel,
    Vector* force,
    Vector* time)
{
    // transform data
    if (disp != 0) {
        this->transfDaqDisp(disp);
    }
    if (vel != 0) {
        this->transfDaqVel(vel);
    }
    if (accel != 0) {
        this->transfDaqAccel(accel);
    }
    if (force != 0) {
        this->transfDaqForce(force);
    }
    if (time != 0) {
        this->transfDaqTime(time);
    }
    
    return OF_ReturnType_completed;
}


ExperimentalSetup* ESFourActuators3d::getCopy()
{
	ESFourActuators3d *theCopy = new ESFourActuators3d(*this);
    
    return theCopy;
}


void ESFourActuators3d::Print(OPS_Stream &s, int flag)
{
    s << "ExperimentalSetup: " << this->getTag(); 
    s << " type: ESFourActuators3d\n";
    s << " actLength1  : " << L1      << endln;
    s << " actLength2  : " << L2      << endln;
    s << " actLength3  : " << L3      << endln;
	s << " actLength4  : " << L4      << endln;
    s << " rigidLength1: " << a1      << endln;
    s << " rigidLength2: " << a2      << endln;
	s << " rigidLength3: " << a3      << endln;
	s << " rigidLength4: " << a4      << endln;
	s << " rigidHeight : " << h       << endln;
	s << " rigidHeight1: " << h1      << endln;
	s << " rigidHeight2: " << h2      << endln;
	s << " rigidLinkN  : " << arlN    << endln;
	s << " rigidLinkS  : " << arlS    << endln;
	s << " rodLengthN  : " << LrodN   << endln;
	s << " rodLengthS  : " << LrodS   << endln;
    s << " nlGeom      : " << nlGeom  << endln;
    s << " phiLocX     : " << phiLocX << endln;
    if (theControl != 0)  {
        s << "\tExperimentalControl tag: " << theControl->getTag();
        s << *theControl;
    }
}


int ESFourActuators3d::transfTrialDisp(const Vector* disp)
{	
    // transform yz coordinate system to local xy system
    //Vector D(4);
    D(0) = -(*disp)(1)*(*cDispFact)(0);
    D(1) = -(*disp)(3)*(*cDispFact)(1);
    D(2) = -(*disp)(4)*(*cDispFact)(2);
    D(3) = -(*disp)(2)*(*cDispFact)(3);
    
    // transform displacements (ux, uy, phix, phiy) to local coordinate system
    //Vector d(4);
    d = rotLocX*D;
    
    // rigid body displacements due to vector d
    double pi = acos(-1.0);
    double alpha = 0.0;
    double beta = 0.0;
    
    if (d(2) == 0) {
        alpha = (d(3)/fabs(d(3)))*pi/2.0;
        beta = -(d(3)/fabs(d(3)))*d(3);
    }
    else {
        alpha = atan(d(3)/d(2));
        beta = -(d(2)/fabs(d(2)))*sqrt(pow(d(2),2.0)+pow(d(3),2.0));
    }
    double gama = -alpha;
    Matrix A(3,3), B(3,3), C(3,3), V(3,4), V1(3,1), V2(3,1);
    A.Zero();
    A(0,0) =  cos(gama); A(0,1) = sin(gama);
    A(1,0) = -sin(gama); A(1,1) = cos(gama);
    A(2,2) =  1.0;
    
    B.Zero();
    B(0,0) =  1.0;
    B(1,1) =  cos(beta); B(1,2) = sin(beta);
    B(2,1) = -sin(beta); B(2,2) = cos(beta);
    
    C.Zero();
    C(0,0) =  cos(alpha); C(0,1) = sin(alpha);
    C(1,0) = -sin(alpha); C(1,1) = cos(alpha);
    C(2,2) =  1.0;
    
    V.Zero();
    V(0,0) = -a1; V(0,2) = -a3; 
    V(1,1) = -a2; V(1,3) = -a4;
    V(2,2) =  h;  V(2,3) =  h;
    
    Matrix R(3,3), Vr(3,4), Vr1(3,1), Vr2(3,1);
    R.Zero(); Vr.Zero(); Vr1.Zero(); Vr2.Zero();
    R = A*B*C;
    Vr1(2,0) = h1; Vr2(2,0) = h2; 
    Vr = R*V; Vr1 = R*V1; Vr2 = R*V2;
    
    uxbeam = D(0)+Vr1(0,0)*cos(phiLocX/180.0*pi)-Vr1(1,0)*sin(phiLocX/180.0*pi);
    uybeam = D(1)+Vr2(0,0)*sin(phiLocX/180.0*pi)+Vr2(1,0)*cos(phiLocX/180.0*pi);
    upN    = uybeam-sin(-D(2))*arlN;
    upS    = uybeam-sin(-D(2))*arlS;
    
    // linear geometry
    if (nlGeom == 0 )  {
        opserr << "WARNING ESFourActuators3d::transfDaqForce() - " 
            << "Linear geometry case is not implemented yet\n";
    }
    
    // nonlinear geometry
    else if (nlGeom == 1)  {
        // actuator 1
        double u11 = L1+d(0)+a1+Vr(0,0);
        double u12 = d(1)+Vr(1,0);
        double u13 = Vr(2,0);
        (*cDisp)(0) = sqrt(pow(u11,2.0)+pow(u12,2.0)+pow(u13,2.0))-L1;
        theta1  = phiLocX*pi/180.0+atan(u12/u11);
        theta11 = atan(u13/sqrt(pow(u11,2.0)+pow(u12,2.0))); 
        // actuator 2
        double u21 = d(0)+Vr(0,1);
        double u22 = L2+d(1)+a2+Vr(1,1);
        double u23 = Vr(2,1);
        (*cDisp)(1) = sqrt(pow(u21,2.0)+pow(u22,2.0)+pow(u23,2.0))-L2;
        //(*cDisp)(1) = -1.0*(sqrt(pow(u21,2.0)+pow(u22,2.0)+pow(u23,2.0))-L2);
        theta2  = (pi/2.0-phiLocX*pi/180.0)+atan(u21/u22);
        theta22 = atan(u23/sqrt(pow(u21,2.0)+pow(u22,2.0))); 
        // actuator 3
        double u31 = L3+d(0)+a3+Vr(0,2);
        double u32 = d(1)+Vr(1,2);
        double u33 = Vr(2,2)-h;
        (*cDisp)(2) = sqrt(pow(u31,2.0)+pow(u32,2.0)+pow(u33,2.0))-L3;
        theta3  = phiLocX*pi/180.0+atan(u32/u31);
        theta33 = atan(u33/sqrt(pow(u31,2.0)+pow(u32,2.0))); 
        // actuator 4
        double u41 = d(0)+Vr(0,3);
        double u42 = L4+d(1)+a4+Vr(1,3);
        double u43 = Vr(2,3)-h;
        (*cDisp)(3) = sqrt(pow(u41,2.0)+pow(u42,2.0)+pow(u43,2.0))-L4;
        //(*cDisp)(3) = -1.0*(sqrt(pow(u41,2.0)+pow(u42,2.0)+pow(u43,2.0))-L4);
        theta4  = (pi/2.0-phiLocX*pi/180.0)+atan(u41/u42);
        theta44 = atan(u43/sqrt(pow(u41,2.0)+pow(u42,2.0))); 
    }
    
    return OF_ReturnType_completed;
}


int ESFourActuators3d::transfTrialVel(const Vector* vel)
{
    return OF_ReturnType_completed;
}


int ESFourActuators3d::transfTrialAccel(const Vector* accel)
{
    return OF_ReturnType_completed;
}


int ESFourActuators3d::transfTrialForce(const Vector* force)
{   
    return OF_ReturnType_completed;
}


int ESFourActuators3d::transfTrialTime(const Vector* time)
{
    *cTime = *time;
    
    return OF_ReturnType_completed;
}


int ESFourActuators3d::transfDaqDisp(Vector* disp)
{
    return OF_ReturnType_completed;
}


int ESFourActuators3d::transfDaqVel(Vector* vel)
{
    return OF_ReturnType_completed;
}


int ESFourActuators3d::transfDaqAccel(Vector* accel)
{
    return OF_ReturnType_completed;
}


int ESFourActuators3d::transfDaqForce(Vector* force)
{
    // linear geometry
    if (nlGeom == 0 )  {
        opserr << "WARNING ESFourActuators3d::transfDaqForce() - "
            << "Linear geometry case is not implemented yet\n";
    }
    // nonlinear geometry
    else if (nlGeom == 1 )  {

        //lateral forces that come from rods
        double deltaFxN = -(uxbeam/LrodN)*(-1.0)*(*dForce)(0);
        double deltaFxS = -(uxbeam/LrodS)*(-1.0)*(*dForce)(1);
        double deltaFx  = deltaFxN+deltaFxS;

        double alphaN   = atan(upN/LrodN);
        double alphaS   = atan(upS/LrodS);
        double phiN     = -D(2)-alphaN;
        double phiS     = -D(2)-alphaS;
        double deltaFyN = (-1.0)*(*dForce)(0)*tan(phiN)*cos(D(2))+(-1.0)*(*dForce)(0)*sin(D(2));
        double deltaFyS = (-1.0)*(*dForce)(1)*tan(phiS)*cos(D(2))+(-1.0)*(*dForce)(1)*sin(D(2));
        double deltaFy  = deltaFyN+deltaFyS;

        // Lateral forces that come from actuators
        double F1x = (*dForce)(2)*cos(theta1)*cos(theta11);
        double F1y = (*dForce)(2)*sin(theta1)*cos(theta11);
        double F2x = -(*dForce)(3)*cos(theta2)*cos(theta22);
        double F2y = (*dForce)(3)*sin(theta2)*cos(theta22);
        double F3x = (*dForce)(4)*cos(theta3)*cos(theta33);
        double F3y = (*dForce)(4)*sin(theta3)*cos(theta33);
        double F4x = -(*dForce)(5)*cos(theta4)*cos(theta44);
        double F4y = (*dForce)(5)*sin(theta4)*cos(theta44);

        // Total lateral forces
        double Fx = F1x+F2x+F3x+F4x+deltaFx;
        double Fy = F1y+F2y+F3y+F4y+deltaFy;
        double Mx = -(F3y+F4y)*h-deltaFy*(h2+6.09375);
        double My = (F3x+F4x)*h+deltaFx*h1;

        //(*force)(0) = ((*dForce)(0)+(*dForce)(1))*(*dForceFact)(0);
        (*force)(0) = 0.0;
        (*force)(1) = -Fx*(*dForceFact)(1);
        (*force)(2) = -My*(*dForceFact)(2);
        (*force)(3) = -Fy*(*dForceFact)(3);
        (*force)(4) = -Mx*(*dForceFact)(4);
        (*force)(5) = 0.0;
    }

    return OF_ReturnType_completed;
}


int ESFourActuators3d::transfDaqTime(Vector* time)
{  
    *time = *dTime;
    
    return OF_ReturnType_completed;
}
