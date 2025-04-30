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

#ifndef ExpForceControl_h
#define ExpForceControl_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 09/24
// Revision: A
//
// Description: This file contains the class definition for ExpForceControl.
// ExpForceControl is a class which imposes the experimental control trial
// force values at a particular dof at a node.

#include <NodalLoad.h>

class Node;
class Vector;

#define LOAD_TAG_ExpForceControl 101


class ExpForceControl : public NodalLoad
{
public:
    // constructors    
    ExpForceControl(int tag, int nodeTag, int ndof,
        double *ctrlForce, double forceFact = 1.0);
    
    // destructor
    ~ExpForceControl();
    
    //double getValue();
    void applyLoad(double loadFactor);
    //bool isHomogeneous() const;
    
    int sendSelf(int commitTag, Channel &theChannel);
    int recvSelf(int commitTag, Channel &theChannel,
        FEM_ObjectBroker &theBroker);
    
    void Print(OPS_Stream &s, int flag = 0);
    
protected:

private:
    int nodeTag;        // tag of node where load is applied
    int nodeDOF;        // nodal dof where load is applied
    double *ctrlForce;  // pointer to control force
    double forceFact;	// control force factor

    Node* theNode;         // pointer to node where load is applied
    Vector* theNodalLoad;  // vector for setting nodal loads
};

#endif
