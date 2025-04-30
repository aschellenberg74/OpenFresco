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
// Created: 09/24
// Revision: A
//
// Description: This file contains the implementation of ExpForceControl.

#include "ExpForceControl.h"
#include <Domain.h>
#include <Node.h>
#include <Vector.h>
#include <LoadPattern.h>
#include <Channel.h>
#include <FEM_ObjectBroker.h>
#include <ID.h>


ExpForceControl::ExpForceControl(int tag, int nodetag,
    int ndof, double *ctrlforce, double forcefact)
    : NodalLoad(tag, nodetag, LOAD_TAG_ExpForceControl),
    nodeTag(nodetag), nodeDOF(ndof),
    ctrlForce(ctrlforce), forceFact(forcefact),
    theNode(0), theNodalLoad(0)
{
    // does nothing else
}


ExpForceControl::~ExpForceControl()
{
    // does nothing else
}


void ExpForceControl::applyLoad(double loadFactor)
{
    // on first call
    if (theNode == 0)  {
        Domain *theDomain = this->getDomain();
        
        theNode = theDomain->getNode(nodeTag);
        if (theNode == 0)
            return;
    }
    
    theNodalLoad->resize(theNode->getNumberDOF());
    theNodalLoad->Zero();
    (*theNodalLoad)(nodeDOF) = forceFact * (*ctrlForce);
    theNode->addUnbalancedLoad(*theNodalLoad);
    
    return;
}


int ExpForceControl::sendSelf(int cTag, Channel &theChannel)
{
    // has not been implemented yet.....
    return 0;
}


int ExpForceControl::recvSelf(int cTag, Channel &theChannel, 
    FEM_ObjectBroker &theBroker)
{
    // has not been implemented yet.....
    return 0;
}


void ExpForceControl::Print(OPS_Stream &s, int flag) 
{
    s << "ExpForceControl: " << this->getTag();
    s << "  Node: " << nodeTag;
    s << "  DOF: " << nodeDOF << endln;
}
