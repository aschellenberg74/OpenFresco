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
// $URL$

#ifndef ESFourActuators3d_h
#define ESFourActuators3d_h

// Written: Vesna Terzic (vesna@berkeley.edu)
// Created: 01/07
// Revision: A
//
// Description: This file contains the class definition for
// ESFourActuators3d. ESFourActuators3d is an experimental
// setup class with four actuators which control the two
// translational and the two rotational degrees of freedom of
// a specimen in 3D. The axial and torsional dof are ignored.

#include "ExperimentalSetup.h"

#include <Matrix.h>

class ESFourActuators3d : public ExperimentalSetup
{
public:
    // constructors
    ESFourActuators3d(int tag,
		double actLength1, double actLength2, double actLength3, double actLength4,
		double rigidLength1, double rigidLength2, double rigidLength3, double rigidLength4,
		double rigidHeight, double rigidHeight1, double rigidHeight2,
		double rigidLinkN, double rigidLinkS, double rodLengthN, double rodLengthS,
        double spreaderBeamHeight, ExperimentalControl* control = 0,
		int nlGeom = 0, double phiLocX = 45.0);            
    ESFourActuators3d(const ESFourActuators3d& es);
    
    // destructor
    virtual ~ESFourActuators3d();
    
    // method to get class type
    const char *getClassType() const {return "ESFourActuators3d";};
    
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
    double L1;          // length of actuator 1
    double L2;          // length of actuator 2
    double L3;          // length of actuator 3
	double L4;          // length of actuator 4
    double a1;          // rigid link length 1
	double a2;          // rigid link length 2
	double a3;          // rigid link length 3
	double a4;          // rigid link length 4
	double h;			// rigid link height between actuators
	double h1;			// rigid link height between lower actuator and pin connection
	double h2;			// rigid link height between lower actuator and lower beam flange
	double arlN;		// rigid link at NORTH rod - top pin to lower beam flange
	double arlS;		// rigid link at SOUTH rod - top pin to lower beam flange
	double LrodN;		// pin to pin length of the north side rod
	double LrodS;		// pin to pin length of the south side rod
	double Hbeam;       // spreader beam height
    int nlGeom;         // non-linear geometry (0: linear, 1: nonlinear)
	double phiLocX;     // angle of actuator 1 w.r.t reaction wall [deg]
    
	double uxbeam;
	double uybeam;
	double upN;
	double upS;
	double theta1;
	double theta2;
	double theta3;
	double theta4;
	double theta11;
	double theta22;
	double theta33;
	double theta44;
	double ha3;
	double ha4;
	double ax3;
	double ay3;
	double ax4;
	double ay4;
    
    Matrix rotLocX;     // rotation matrix
	Vector D;			// displacement vector in x,y axis
	Vector d;			// displacement vector in 1,2 axis
    
    bool firstWarning[8];
};

#endif
