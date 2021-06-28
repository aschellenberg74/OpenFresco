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

// Written: Vesna Terzic (vesna@berkeley.edu)
// Created: 01/08
// Revision: A
//
// Description: This file contains the implementation of the
// ESFourActuators3d class.

#include "ESFourActuators3d.h"

#include <ExperimentalControl.h>

#include <elementAPI.h>

#include <math.h>
#include <stdlib.h>


void* OPF_ESFourActuators3d()
{
    // pointer to experimental setup that will be returned
    ExperimentalSetup* theSetup = 0;
    
    if (OPS_GetNumRemainingInputArgs() < 10) {
        opserr << "WARNING invalid number of arguments\n";
        opserr << "Want: expSetup FourActuators tag <-control ctrlTag> "
            << "L1 L2 L3 L4 a1 a2 a3 a4 h h1 h2 arlN arlS LrodN LrodS Hbeam "
            << " <-nlGeom> <-phiLocX phi>\n";
        return 0;
    }
    
    // setup tag
    int tag;
    int numdata = 1;
    if (OPS_GetIntInput(&numdata, &tag) != 0) {
        opserr << "WARNING invalid expSetup FourActuators tag\n";
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
            opserr << "expSetup FourActuators " << tag << endln;
            return 0;
        }
        theControl = OPF_getExperimentalControl(ctrlTag);
        if (theControl == 0) {
            opserr << "WARNING experimental control not found\n";
            opserr << "expControl: " << ctrlTag << endln;
            opserr << "expSetup FourActuators " << tag << endln;
            return 0;
        }
    }
    else {
        // move current arg back by one
        OPS_ResetCurrentInputArg(-1);
    }
    
    // L1, L2, L3, L4
    double L[4];
    numdata = 4;
    if (OPS_GetDoubleInput(&numdata, L) != 0) {
        opserr << "WARNING invalid L1, L2, L3, or L4\n";
        opserr << "expSetup FourActuators " << tag << endln;
        return 0;
    }
    
    // a1, a2, a3, a4
    double a[4];
    numdata = 4;
    if (OPS_GetDoubleInput(&numdata, a) != 0) {
        opserr << "WARNING invalid a1, a2, a3, or a4\n";
        opserr << "expSetup FourActuators " << tag << endln;
        return 0;
    }
    
    // h, h1, h2
    double h[3];
    numdata = 3;
    if (OPS_GetDoubleInput(&numdata, h) != 0) {
        opserr << "WARNING invalid h, h1, or h2\n";
        opserr << "expSetup FourActuators " << tag << endln;
        return 0;
    }
    
    // arlN, arlS
    double arl[2];
    numdata = 2;
    if (OPS_GetDoubleInput(&numdata, arl) != 0) {
        opserr << "WARNING invalid arlN or arlS\n";
        opserr << "expSetup FourActuators " << tag << endln;
        return 0;
    }
    
    // LrodN, LrodS
    double Lrod[2];
    numdata = 2;
    if (OPS_GetDoubleInput(&numdata, Lrod) != 0) {
        opserr << "WARNING invalid LrodN or LrodS\n";
        opserr << "expSetup FourActuators " << tag << endln;
        return 0;
    }
    
    // Hbeam
    double Hbeam;
    numdata = 1;
    if (OPS_GetDoubleInput(&numdata, &Hbeam) != 0) {
        opserr << "WARNING invalid Hbeam\n";
        opserr << "expSetup FourActuators " << tag << endln;
        return 0;
    }
    
    // optional parameters
    int nlGeom = 0;
    double phiLocX = 0.0;
    while (OPS_GetNumRemainingInputArgs() > 0) {
        // nlGeom
        type = OPS_GetString();
        if (strcmp(type, "-nlGeom") == 0) {
            nlGeom = 1;
        }
        // phiLocX
        else if (strcmp(type, "-phiLocX") == 0) {
            numdata = 1;
            if (OPS_GetDoubleInput(&numdata, &phiLocX) != 0) {
                opserr << "WARNING invalid phiLocX\n";
                opserr << "expSetup FourActuators " << tag << endln;
                return 0;
            }
        }
    }
    
    // parsing was successful, allocate the setup
    theSetup = new ESFourActuators3d(tag, L[0], L[1], L[2], L[3],
        a[0], a[1], a[2], a[3], h[0], h[1], h[2], arl[0], arl[1],
        Lrod[0], Lrod[1], Hbeam, theControl, nlGeom, phiLocX);
    if (theSetup == 0) {
        opserr << "WARNING could not create experimental setup of type ESFourActuators3d\n";
        return 0;
    }
    
    return theSetup;
}


ESFourActuators3d::ESFourActuators3d(int tag,
	double actLength1, double actLength2, double actLength3, double actLength4,
	double rigidLength1, double rigidLength2, double rigidLength3, double rigidLength4,
	double rigidHeight, double rigidHeight1, double rigidHeight2,
	double rigidLinkN, double rigidLinkS, double rodLengthN, double rodLengthS,
    double spreaderBeamHeight, ExperimentalControl* control, int nlgeom, double philocx)
    : ExperimentalSetup(tag, control),
    L1(actLength1), L2(actLength2), L3(actLength3), L4(actLength4),
    a1(rigidLength1), a2(rigidLength2), a3(rigidLength3), a4(rigidLength4),
	h(rigidHeight), h1(rigidHeight1), h2(rigidHeight2),
	arlN(rigidLinkN), arlS(rigidLinkS), LrodN(rodLengthN), LrodS(rodLengthS), Hbeam(spreaderBeamHeight),
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
	ha3 = 0.0;
    ha4 = 0.0;
    ax3 = 0.0;
    ay3 = 0.0;
    ax4 = 0.0;
    ay4 = 0.0;
    
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
	Hbeam   = es.Hbeam;
    nlGeom  = es.nlGeom;
	phiLocX = es.phiLocX;
    
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
	ha3 = 0.0;
    ha4 = 0.0;
    ax3 = 0.0;
    ay3 = 0.0;
    ax4 = 0.0;
    ay4 = 0.0;
    
    // call setup method
    this->setup();
    
    for (int i=0; i<3; i++)
        firstWarning[i] = true;
}


ESFourActuators3d::~ESFourActuators3d()
{
    // does nothing
}


int ESFourActuators3d::setup()
{
    // setup the trial/out vectors
    sizeTrial->Zero();
    sizeOut->Zero();
    for (int i=0; i<OF_Resp_Time; i++)  {
        (*sizeTrial)(i) = 6;
        (*sizeOut)(i) = 6;
    }
    (*sizeTrial)(OF_Resp_Time) = 1;
    (*sizeOut)(OF_Resp_Time) = 1;
    
    this->setTrialOutSize();
    
    // setup the ctrl/daq vectors
    sizeCtrl->Zero();
    (*sizeCtrl)(OF_Resp_Disp) = 4;
    (*sizeCtrl)(OF_Resp_Time) = 1;
    
    sizeDaq->Zero();
    (*sizeDaq)(OF_Resp_Force) = 6;
    (*sizeDaq)(OF_Resp_Time)  = 1;
    
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


ExperimentalSetup* ESFourActuators3d::getCopy()
{
	ESFourActuators3d *theCopy = new ESFourActuators3d(*this);
    
    return theCopy;
}


void ESFourActuators3d::Print(OPS_Stream &s, int flag)
{
    s << "ExperimentalSetup: " << this->getTag(); 
    s << " type: ESFourActuators3d\n";
    s << " actLength1        : " << L1      << endln;
    s << " actLength2        : " << L2      << endln;
    s << " actLength3        : " << L3      << endln;
	s << " actLength4        : " << L4      << endln;
    s << " rigidLength1      : " << a1      << endln;
    s << " rigidLength2      : " << a2      << endln;
	s << " rigidLength3      : " << a3      << endln;
	s << " rigidLength4		 : " << a4      << endln;
	s << " rigidHeight		 : " << h       << endln;
	s << " rigidHeight1		 : " << h1      << endln;
	s << " rigidHeight2		 : " << h2      << endln;
	s << " rigidLinkN		 : " << arlN    << endln;
	s << " rigidLinkS        : " << arlS    << endln;
	s << " rodLengthN        : " << LrodN   << endln;
	s << " rodLengthS        : " << LrodS   << endln;
	s << " spreaderBeamHeight: " << Hbeam   << endln;
    s << " nlGeom            : " << nlGeom  << endln;
    s << " phiLocX           : " << phiLocX << endln;
    if (theControl != 0)  {
        s << "\tExperimentalControl tag: " << theControl->getTag();
        s << *theControl;
    }
}


int ESFourActuators3d::transfTrialDisp(const Vector* disp)
{	
	// transform yz coordinate system to local xy system
    
	// experiment starts from step=s; offsets are ux(s), uy(s), phix(s), phiy(s)	
	//at s=0
	double off1 = 0.0;
	double off2 = 0.0;
	double off3 = 0.0;
	double off4 = 0.0;
	
	// at s=574
	//double off1 = -0.081285543843571;
	//double off2 = -0.228852075001237;
	//double off3 =  0.004015970000000;
	//double off4 = -0.001927180000000;
    
	// at s=3775
	//double off1 =  0.120421628091371;
	//double off2 =  0.072533585780559;
	//double off3 = -0.001318810000000;
	//double off4 =  0.002186960000000;
    
	D(0) = -(*disp)(1) + off1;
	D(1) = -(*disp)(3) + off2;
	D(2) = -(*disp)(4) + off3;
	D(3) = -(*disp)(2) + off4;
	
    // transform displacements (ux, uy, phix, phiy) to local coordinate system
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
    double gamma = -alpha;
    Matrix A(3,3), B(3,3), C(3,3), V(3,4), V1(3,1), V2(3,1);
    A.Zero();
    A(0,0) =  cos(gamma); A(0,1) = sin(gamma);
    A(1,0) = -sin(gamma); A(1,1) = cos(gamma);
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
    V1.Zero();
    V1(2,0) = h1;
    V2.Zero();
    V2(2,0) = h2;
    
	Matrix R(3,3), Vr(3,4), Vr1(3,1), Vr2(3,1);
	R.Zero(); Vr.Zero(); Vr1.Zero(); Vr2.Zero();
	R = A*B*C;
	Vr = R*V;
	Vr1 = R*V1;
	Vr2 = R*V2;
    
	uxbeam = D(0)+Vr1(0,0)*cos(phiLocX/180.0*pi)-Vr1(1,0)*sin(phiLocX/180.0*pi);
	uybeam = D(1)+Vr2(0,0)*sin(phiLocX/180.0*pi)+Vr2(1,0)*cos(phiLocX/180.0*pi);
	upN    = uybeam-sin(-D(2))*arlN;
	upS    = uybeam-sin(-D(2))*arlS;
    
	ha3 = Vr(2,2);
    ha4 = Vr(2,3);
    ax3 = Vr(0,2)*cos(phiLocX/180.0*pi)-Vr(1,2)*sin(phiLocX/180.0*pi)-D(0);
    ay3 = Vr(0,2)*sin(phiLocX/180.0*pi)+Vr(1,2)*cos(phiLocX/180.0*pi)-D(1);
    ax4 = Vr(0,3)*cos(phiLocX/180.0*pi)-Vr(1,3)*sin(phiLocX/180.0*pi)-D(0);
    ay4 = Vr(0,3)*sin(phiLocX/180.0*pi)+Vr(1,3)*cos(phiLocX/180.0*pi)-D(1);
    
    // linear geometry
    if (nlGeom == 0)  {
        if (firstWarning[0] == true)  {
		    opserr << "WARNING ESFourActuators3d::transfTrialDisp() - " 
	            << "Linear geometry case is not implemented yet."
                << "Using nonlinear geometry instead.\n\n";
            firstWarning[0] = false;
        }
		// actuator displacements: experiment starts from step=s; offset is ui(s); i=1,2,3,4
		//at s=0
		double off_u1 = 0.0;
		double off_u2 = 0.0;
		double off_u3 = 0.0;
		double off_u4 = 0.0;
		// at s=574
		//double off_u1 = -0.219054540850067;
		//double off_u2 = -0.104074893045748;
		//double off_u3 = -0.295721108038151;
		//double off_u4 = -0.130830156667088;
		// at s=3775
		//double off_u1 =  0.136511926047248;
		//double off_u2 = -0.033762397285614;
		//double off_u3 =  0.181755536616677;
		//double off_u4 = -0.044892820188551;
        
        // actuator 1
		double u11 = L1+d(0)+a1+Vr(0,0);
		double u12 = d(1)+Vr(1,0);
		double u13 = Vr(2,0);
        (*cDisp)(0) = sqrt(pow(u11,2.0)+pow(u12,2.0)+pow(u13,2.0))-L1-off_u1;
		theta1  = phiLocX*pi/180.0+atan(u12/u11);
		theta11 = atan(u13/sqrt(pow(u11,2.0)+pow(u12,2.0))); 
        // actuator 2
		double u21 = d(0)+Vr(0,1);
		double u22 = L2+d(1)+a2+Vr(1,1);
		double u23 = Vr(2,1);
        (*cDisp)(1) = sqrt(pow(u21,2.0)+pow(u22,2.0)+pow(u23,2.0))-L2-off_u2;
		theta2  = (pi/2.0-phiLocX*pi/180.0)+atan(u21/u22);
		theta22 = atan(u23/sqrt(pow(u21,2.0)+pow(u22,2.0))); 
        // actuator 3
		double u31 = L3+d(0)+a3+Vr(0,2);
		double u32 = d(1)+Vr(1,2);
		double u33 = Vr(2,2)-h;
        (*cDisp)(2) = sqrt(pow(u31,2.0)+pow(u32,2.0)+pow(u33,2.0))-L3-off_u3;
		theta3  = phiLocX*pi/180.0+atan(u32/u31);
		theta33 = atan(u33/sqrt(pow(u31,2.0)+pow(u32,2.0))); 
        // actuator 4
		double u41 = d(0)+Vr(0,3);
		double u42 = L4+d(1)+a4+Vr(1,3);
		double u43 = Vr(2,3)-h;
        (*cDisp)(3) = sqrt(pow(u41,2.0)+pow(u42,2.0)+pow(u43,2.0))-L4-off_u4;
		theta4  = (pi/2.0-phiLocX*pi/180.0)+atan(u41/u42);
		theta44 = atan(u43/sqrt(pow(u41,2.0)+pow(u42,2.0)));
    }
    
    // nonlinear geometry
    else if (nlGeom == 1)  {
		// actuator displacements: experiment starts from step=s; offset is ui(s); i=1,2,3,4
		//at s=0
		double off_u1 = 0.0;
		double off_u2 = 0.0;
		double off_u3 = 0.0;
		double off_u4 = 0.0;
		// at s=574
		//double off_u1 = -0.219054540850067;
		//double off_u2 = -0.104074893045748;
		//double off_u3 = -0.295721108038151;
		//double off_u4 = -0.130830156667088;
		// at s=3775
		//double off_u1 =  0.136511926047248;
		//double off_u2 = -0.033762397285614;
		//double off_u3 =  0.181755536616677;
		//double off_u4 = -0.044892820188551;
        
        // actuator 1
		double u11 = L1+d(0)+a1+Vr(0,0);
		double u12 = d(1)+Vr(1,0);
		double u13 = Vr(2,0);
        (*cDisp)(0) = sqrt(pow(u11,2.0)+pow(u12,2.0)+pow(u13,2.0))-L1-off_u1;
		theta1  = phiLocX*pi/180.0+atan(u12/u11);
		theta11 = atan(u13/sqrt(pow(u11,2.0)+pow(u12,2.0))); 
        // actuator 2
		double u21 = d(0)+Vr(0,1);
		double u22 = L2+d(1)+a2+Vr(1,1);
		double u23 = Vr(2,1);
        (*cDisp)(1) = sqrt(pow(u21,2.0)+pow(u22,2.0)+pow(u23,2.0))-L2-off_u2;
		theta2  = (pi/2.0-phiLocX*pi/180.0)+atan(u21/u22);
		theta22 = atan(u23/sqrt(pow(u21,2.0)+pow(u22,2.0))); 
        // actuator 3
		double u31 = L3+d(0)+a3+Vr(0,2);
		double u32 = d(1)+Vr(1,2);
		double u33 = Vr(2,2)-h;
        (*cDisp)(2) = sqrt(pow(u31,2.0)+pow(u32,2.0)+pow(u33,2.0))-L3-off_u3;
		theta3  = phiLocX*pi/180.0+atan(u32/u31);
		theta33 = atan(u33/sqrt(pow(u31,2.0)+pow(u32,2.0))); 
        // actuator 4
		double u41 = d(0)+Vr(0,3);
		double u42 = L4+d(1)+a4+Vr(1,3);
		double u43 = Vr(2,3)-h;
        (*cDisp)(3) = sqrt(pow(u41,2.0)+pow(u42,2.0)+pow(u43,2.0))-L4-off_u4;
		theta4  = (pi/2.0-phiLocX*pi/180.0)+atan(u41/u42);
		theta44 = atan(u43/sqrt(pow(u41,2.0)+pow(u42,2.0)));
    }
    
    return OF_ReturnType_completed;
}


int ESFourActuators3d::transfTrialVel(const Vector* vel)
{
    if (firstWarning[1] == true)  {
        opserr << "WARNING ESFourActuators3d::transfTrialVel() - " 
            << "not implemented yet.\n\n";
        firstWarning[1] = false;
    }
    
    return OF_ReturnType_completed;
}



int ESFourActuators3d::transfTrialAccel(const Vector* accel)
{
    if (firstWarning[2] == true)  {
        opserr << "WARNING ESFourActuators3d::transfTrialAccel() - " 
            << "not implemented yet.\n\n";
        firstWarning[2] = false;
    }
    
    return OF_ReturnType_completed;
}


int ESFourActuators3d::transfTrialForce(const Vector* force)
{   
    if (firstWarning[3] == true)  {
        opserr << "WARNING ESFourActuators3d::transfTrialForce() - " 
            << "not implemented yet.\n\n";
        firstWarning[3] = false;
    }
    
    return OF_ReturnType_completed;
}


int ESFourActuators3d::transfTrialTime(const Vector* time)
{
    *cTime = *time;
    
    return OF_ReturnType_completed;
}


int ESFourActuators3d::transfDaqDisp(Vector* disp)
{
    if (firstWarning[4] == true)  {
        opserr << "WARNING ESFourActuators3d::transfDaqDisp() - " 
            << "not implemented yet.\n\n";
        firstWarning[4] = false;
    }
    
    return OF_ReturnType_completed;
}


int ESFourActuators3d::transfDaqVel(Vector* vel)
{
    if (firstWarning[5] == true)  {
        opserr << "WARNING ESFourActuators3d::transfDaqVel() - " 
            << "not implemented yet.\n\n";
        firstWarning[5] = false;
    }
    
    return OF_ReturnType_completed;
}


int ESFourActuators3d::transfDaqAccel(Vector* accel)
{
    if (firstWarning[6] == true)  {
        opserr << "WARNING ESFourActuators3d::transfDaqAccel() - " 
            << "not implemented yet.\n\n";
        firstWarning[6] = false;
    }
    
    return OF_ReturnType_completed;
}


int ESFourActuators3d::transfDaqForce(Vector* force)
{
    // linear geometry
    if (nlGeom == 0)  {
        if (firstWarning[7] == true)  {
		    opserr << "WARNING ESFourActuators3d::transfDaqForce() - " 
	            << "Linear geometry case is not implemented yet."
                << "Using nonlinear geometry instead.\n\n";
            firstWarning[7] = false;
        }
		// lateral forces that come from the rods
		double deltaFxN = -(uxbeam/LrodN)*(-1.0)*(*dForce)(0)*cos(D(2));
		double deltaFxS = -(uxbeam/LrodS)*(-1.0)*(*dForce)(1)*cos(D(2));
		double deltaFx  = deltaFxN+deltaFxS;
        
		double deltaFyN = (-1.0)*(*dForce)(0)*sin(D(2));
		double deltaFyS = (-1.0)*(*dForce)(1)*sin(D(2));
		double deltaFy  = deltaFyN+deltaFyS;
		
		// moments that come from the rods
		double FzN = (*dForce)(0)*cos(-D(2));
		double FzS = (*dForce)(1)*cos(-D(2));
		double Fz  = FzN+FzS;
		double MyV = -Fz*(uxbeam-D(0));
		double MyR = MyV+deltaFx*h1;
		double MxR = Fz*(uybeam+0.5*Hbeam*sin(-D(2))-D(1))-deltaFy*(h2+0.5*Hbeam);
        
		// lateral forces that come from actuators
		double F1x = (*dForce)(2)*cos(theta1)*cos(theta11);
		double F1y = (*dForce)(2)*sin(theta1)*cos(theta11);
		double F2x = -(*dForce)(3)*cos(theta2)*cos(theta22);
		double F2y = (*dForce)(3)*sin(theta2)*cos(theta22);
		double F3x = (*dForce)(4)*cos(theta3)*cos(theta33);
		double F3y = (*dForce)(4)*sin(theta3)*cos(theta33);
		double F3z = (*dForce)(4)*sin(theta33);
		double F4x = -(*dForce)(5)*cos(theta4)*cos(theta44);
		double F4y = (*dForce)(5)*sin(theta4)*cos(theta44);
		double F4z = (*dForce)(5)*sin(theta44);
        
		// moments that come from actuators
		double MxA = -(F3y*ha3+F4y*ha4)+F3z*ay3+F4z*ay4;
		double MyA = (F3x*ha3+F4x*ha4)-(F3z*ax3+F4z*ax4);
        
		// total lateral forces and moments
		double Fx = F1x+F2x+F3x+F4x+deltaFx;
		double Fy = F1y+F2y+F3y+F4y+deltaFy;
		double Mx = MxA+MxR;
		double My = MyA+MyR;
        
        // assign forces and moments
        (*force)(0) = 0;
        (*force)(1) = -Fx;
		(*force)(2) = -My;
        (*force)(3) = -Fy;
        (*force)(4) = -Mx;
		(*force)(5) = 0;
    }
    // nonlinear geometry
    else if (nlGeom == 1)  {
		// lateral forces that come from the rods
		double deltaFxN = -(uxbeam/LrodN)*(-1.0)*(*dForce)(0)*cos(D(2));
		double deltaFxS = -(uxbeam/LrodS)*(-1.0)*(*dForce)(1)*cos(D(2));
		double deltaFx  = deltaFxN+deltaFxS;
        
		double deltaFyN = (-1.0)*(*dForce)(0)*sin(D(2));
		double deltaFyS = (-1.0)*(*dForce)(1)*sin(D(2));
		double deltaFy  = deltaFyN+deltaFyS;
		
		// moments that come from the rods
		double FzN = (*dForce)(0)*cos(-D(2));
		double FzS = (*dForce)(1)*cos(-D(2));
		double Fz  = FzN+FzS;
		double MyV = -Fz*(uxbeam-D(0));
		double MyR = MyV+deltaFx*h1;
		double MxR = Fz*(uybeam+0.5*Hbeam*sin(-D(2))-D(1))-deltaFy*(h2+0.5*Hbeam);
        
		// lateral forces that come from actuators
		double F1x = (*dForce)(2)*cos(theta1)*cos(theta11);
		double F1y = (*dForce)(2)*sin(theta1)*cos(theta11);
		double F2x = -(*dForce)(3)*cos(theta2)*cos(theta22);
		double F2y = (*dForce)(3)*sin(theta2)*cos(theta22);
		double F3x = (*dForce)(4)*cos(theta3)*cos(theta33);
		double F3y = (*dForce)(4)*sin(theta3)*cos(theta33);
		double F3z = (*dForce)(4)*sin(theta33);
		double F4x = -(*dForce)(5)*cos(theta4)*cos(theta44);
		double F4y = (*dForce)(5)*sin(theta4)*cos(theta44);
		double F4z = (*dForce)(5)*sin(theta44);
        
		// moments that come from actuators
		double MxA = -(F3y*ha3+F4y*ha4)+F3z*ay3+F4z*ay4;
		double MyA = (F3x*ha3+F4x*ha4)-(F3z*ax3+F4z*ax4);
        
		// total lateral forces and moments
		double Fx = F1x+F2x+F3x+F4x+deltaFx;
		double Fy = F1y+F2y+F3y+F4y+deltaFy;
		double Mx = MxA+MxR;
		double My = MyA+MyR;
        
        // assign forces and moments
        (*force)(0) = 0;
        (*force)(1) = -Fx;
		(*force)(2) = -My;
        (*force)(3) = -Fy;
        (*force)(4) = -Mx;
		(*force)(5) = 0;
    }
    
    return OF_ReturnType_completed;
}


int ESFourActuators3d::transfDaqTime(Vector* time)
{  
    *time = *dTime;
    
    return OF_ReturnType_completed;
}
