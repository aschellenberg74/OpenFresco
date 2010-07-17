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

// $Revision: 76 $
// $Date: 2007-07-03 00:29:57 -0700 (Tue, 03 Jul 2007) $
// $URL: $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 07/07
// Revision: A
//
// Description: This file contains the implementation of ExpControlSP.

#include "ExpControlSP.h"
#include <Domain.h>
#include <Node.h>
#include <Vector.h>
#include <LoadPattern.h>
#include <Channel.h>
#include <FEM_ObjectBroker.h>
#include <ID.h>


ExpControlSP::ExpControlSP(int node, int ndof,
    double *ctrldisp, double dispfact,
	double *ctrlvel, double velfact,
	double *ctrlaccel, double accelfact)
    : SP_Constraint(node, ndof, CNSTRNT_TAG_ExpControlSP),
    ctrlDisp(ctrldisp), ctrlVel(ctrlvel), ctrlAccel(ctrlaccel),
	dispFact(dispfact), velFact(velfact), accelFact(accelfact),
    theNode(0), theNodeResponse(0)
{
    // does nothing else
}


ExpControlSP::~ExpControlSP()
{
    if (theNodeResponse != 0)
        delete theNodeResponse;
}


double ExpControlSP::getValue()
{
    if (ctrlDisp == 0 || dispFact == 0.0)
        return 0.0;
    
    return dispFact * (*ctrlDisp);
}


int ExpControlSP::applyConstraint(double loadFactor)
{
    // on first call
    if (theNode == 0 || theNodeResponse == 0)  {
        Domain *theDomain = this->getDomain();
        
        theNode = theDomain->getNode(nodeTag);
        if (theNode == 0)
            return -1;
        
        theNodeResponse = new Vector(theNode->getNumberDOF());
        if (theNodeResponse == 0)
            return -2;
    }
    
    // set the responses at the node
    // disp response is the responsibility of constraint handler
    //*theNodeResponse = theNode->getTrialDisp();
    //(*theNodeResponse)(dofNumber) = dispFact * (*ctrlDisp);
    //theNode->setTrialDisp(*theNodeResponse);
    
    if (ctrlVel != 0 && velFact != 0.0)  {
        *theNodeResponse = theNode->getTrialVel();
        (*theNodeResponse)(dofNumber) = velFact * (*ctrlVel);
        theNode->setTrialVel(*theNodeResponse);
    }
    
    if (ctrlAccel != 0 && accelFact != 0.0)  {
        *theNodeResponse = theNode->getTrialAccel();
        (*theNodeResponse)(dofNumber) = accelFact * (*ctrlAccel);
        theNode->setTrialAccel(*theNodeResponse);
    }
    
    return 0;
}


bool ExpControlSP::isHomogeneous() const
{
    return false;
}


int ExpControlSP::sendSelf(int cTag, Channel &theChannel)
{
    // has not been implemented yet.....
    return 0;
}


int ExpControlSP::recvSelf(int cTag, Channel &theChannel, 
    FEM_ObjectBroker &theBroker)
{
    // has not been implemented yet.....
    return 0;
}


void ExpControlSP::Print(OPS_Stream &s, int flag) 
{
    s << "ExpControlSP: " << this->getTag();
    s << "  Node: " << this->getNodeTag();
    s << "  DOF: " << this->getDOF_Number() << endln;
}
