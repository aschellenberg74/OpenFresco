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

#ifndef ExpDispControl_h
#define ExpDispControl_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 07/07
// Revision: A
//
// Description: This file contains the class definition for ExpDispControl.
// ExpDispControl is a class which imposes the experimental control trial
// response values at a particular dof at a node.

#include <SP_Constraint.h>

class Node;
class Vector;

#define CNSTRNT_TAG_ExpDispControl 101


class ExpDispControl : public SP_Constraint
{
public:
    // constructors    
    ExpDispControl(int nodeTag, int ndof,
        double *ctrlDisp, double dispFact = 1.0,
		double *ctrlVel = 0, double velFact = 1.0,
		double *ctrlAccel = 0, double accelFact = 1.0);
    
    // destructor
    ~ExpDispControl();
    
    double getValue();
    int applyConstraint(double loadFactor);
    bool isHomogeneous() const;
    
    int sendSelf(int commitTag, Channel &theChannel);
    int recvSelf(int commitTag, Channel &theChannel,
        FEM_ObjectBroker &theBroker);
    
    void Print(OPS_Stream &s, int flag = 0);
    
protected:

private:
    double *ctrlDisp;           // pointer to control displacement
    double *ctrlVel;            // pointer to control velocity
    double *ctrlAccel;          // pointer to control acceleration
    
    double dispFact;			// control displacement factor
    double velFact;				// control velocity factor
    double accelFact;			// control acceleration factor
    
	Node   *theNode;            // pointer to node being constrained
    Vector *theNodeResponse;    // vector for setting nodal response
};

#endif
