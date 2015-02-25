/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision$
// $Date$
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/LoadControl.cpp,v $
                                                                        
                                                                        
// 
// Written: fmk 
// Created: 07/98
// Revision: A
//
// Description: This file contains the class definition for LoadControl.
// LoadControl is an algorithmic class for perfroming a static analysis
// using a load control integration scheme.
//
// What: "@(#) LoadControl.h, revA"



#include <LoadControl.h>
#include <AnalysisModel.h>
#include <LinearSOE.h>
#include <Vector.h>
#include <Channel.h>
#include <FE_Element.h>
#include <FE_EleIter.h>
#include <Node.h>
#include <DOF_Group.h>
#include <DOF_GrpIter.h>
#include <LoadPattern.h>
#include <LoadPatternIter.h>
#include <Domain.h>


LoadControl::LoadControl(double dLambda, int numIncr, double min, double max)
:StaticIntegrator(INTEGRATOR_TAGS_LoadControl),
 deltaLambda(dLambda), 
 specNumIncrStep(numIncr), numIncrLastStep(numIncr),
 dLambdaMin(min), dLambdaMax(max), gradNumber(0), sensitivityFlag(0)
{
  // to avoid divide-by-zero error on first update() ensure numIncr != 0
  if (numIncr == 0) {
    opserr << "WARNING LoadControl::LoadControl() - numIncr set to 0, 1 assumed\n";
    specNumIncrStep = 1.0;
    numIncrLastStep = 1.0;
  }
}


LoadControl::~LoadControl()
{
    
}

int 
LoadControl::newStep(void)
{
    AnalysisModel *theModel = this->getAnalysisModel();    
    if (theModel == 0) {
	opserr << "LoadControl::newStep() - no associated AnalysisModel\n";
	return -1;
    }

    // determine delta lambda for this step based on dLambda and #iter of last step
    double factor = specNumIncrStep/numIncrLastStep;
    deltaLambda *=factor;

    if (deltaLambda < dLambdaMin)
      deltaLambda = dLambdaMin;
    else if (deltaLambda > dLambdaMax)
      deltaLambda = dLambdaMax;
    
    double currentLambda = theModel->getCurrentDomainTime();

    currentLambda += deltaLambda;
    theModel->applyLoadDomain(currentLambda);

    numIncrLastStep = 0;
    
    return 0;
}
    
int
LoadControl::update(const Vector &deltaU)
{
    AnalysisModel *myModel = this->getAnalysisModel();
    LinearSOE *theSOE = this->getLinearSOE();
    if (myModel == 0 || theSOE == 0) {
	opserr << "WARNING LoadControl::update() ";
	opserr << "No AnalysisModel or LinearSOE has been set\n";
	return -1;
    }

    myModel->incrDisp(deltaU);    
    if (myModel->updateDomain() < 0) {
      opserr << "LoadControl::update - model failed to update for new dU\n";
      return -1;
    }

    // Set deltaU for the convergence test
    theSOE->setX(deltaU);

    numIncrLastStep++;

    return 0;
}


int
LoadControl::setDeltaLambda(double newValue)
{
  // we set the #incr at last step = #incr so get newValue incr
  numIncrLastStep = specNumIncrStep;
  deltaLambda = newValue;
  return 0;
}


int
LoadControl::sendSelf(int cTag,
		      Channel &theChannel)
{
  Vector data(5);
  data(0) = deltaLambda;
  data(1) = specNumIncrStep;
  data(2) = numIncrLastStep;
  data(3) = dLambdaMin;
  data(4) = dLambdaMax;
  if (theChannel.sendVector(this->getDbTag(), cTag, data) < 0) {
      opserr << "LoadControl::sendSelf() - failed to send the Vector\n";
      return -1;
  }
  return 0;
}


int
LoadControl::recvSelf(int cTag,
		      Channel &theChannel, FEM_ObjectBroker &theBroker)
{
  Vector data(5);
  if (theChannel.recvVector(this->getDbTag(), cTag, data) < 0) {
      opserr << "LoadControl::sendSelf() - failed to send the Vector\n";
      deltaLambda = 0;
      return -1;
  }      
  deltaLambda = data(0);
  specNumIncrStep = data(1);
  numIncrLastStep = data(2);
  dLambdaMin = data(3);
  dLambdaMax = data(4);
  return 0;
}



void
LoadControl::Print(OPS_Stream &s, int flag)
{
    AnalysisModel *theModel = this->getAnalysisModel();
    if (theModel != 0) {
	double currentLambda = theModel->getCurrentDomainTime();
	s << "\t LoadControl - currentLambda: " << currentLambda;
	s << "  deltaLambda: " << deltaLambda << endln;
    } else 
	s << "\t LoadControl - no associated AnalysisModel\n";
    
}

int
LoadControl::formEleResidual(FE_Element* theEle)
{
    if(sensitivityFlag == 0) {  // no sensitivity
	this->StaticIntegrator::formEleResidual(theEle);
    } else {
	theEle->zeroResidual();
	theEle->addResistingForceSensitivity(gradNumber);
    }
    return 0;
}

int
LoadControl::formIndependentSensitivityRHS()
{
    return 0;
}

int
LoadControl::formSensitivityRHS(int passedGradNumber)
{
    sensitivityFlag = 1;
    
    // Set a couple of data members
    gradNumber = passedGradNumber;

    // get model
    AnalysisModel* theAnalysisModel = this->getAnalysisModel();
    LinearSOE* theSOE = this->getLinearSOE();

    // Loop through elements
    FE_Element *elePtr;
    FE_EleIter &theEles = theAnalysisModel->getFEs();    
    while((elePtr = theEles()) != 0) {
	theSOE->addB(  elePtr->getResidual(this),  elePtr->getID()  );
    }

    // Loop through the loadPatterns and add the dPext/dh contributions
    static Vector oneDimVectorWithOne(1);
    oneDimVectorWithOne(0) = 1.0;
    static ID oneDimID(1);

    Node *aNode;
    DOF_Group *aDofGroup;
    int nodeNumber, dofNumber, relevantID, i, sizeRandomLoads, numRandomLoads;
    LoadPattern *loadPatternPtr;
    Domain *theDomain = theAnalysisModel->getDomainPtr();
    LoadPatternIter &thePatterns = theDomain->getLoadPatterns();
    while((loadPatternPtr = thePatterns()) != 0) {
	const Vector &randomLoads = loadPatternPtr->getExternalForceSensitivity(gradNumber);
	sizeRandomLoads = randomLoads.Size();
	if (sizeRandomLoads == 1) {
	    // No random loads in this load pattern
	}
	else {
	    // Random loads: add contributions to the 'B' vector
	    numRandomLoads = (int)(sizeRandomLoads/2);
	    for (i=0; i<numRandomLoads*2; i=i+2) {
		nodeNumber = (int)randomLoads(i);
		dofNumber = (int)randomLoads(i+1);
		aNode = theDomain->getNode(nodeNumber);
		aDofGroup = aNode->getDOF_GroupPtr();
		const ID &anID = aDofGroup->getID();
		relevantID = anID(dofNumber-1);
		oneDimID(0) = relevantID;
		theSOE->addB(oneDimVectorWithOne, oneDimID);
	    }
	}
    }

    // reset sensitivity flag
    sensitivityFlag = 0;

    return 0;
}

int
LoadControl::saveSensitivity(const Vector &v, int gradNum, int numGrads)
{
    // get model
    AnalysisModel* theAnalysisModel = this->getAnalysisModel();
    
    DOF_GrpIter &theDOFGrps = theAnalysisModel->getDOFs();
    DOF_Group 	*dofPtr;
    
    while ( (dofPtr = theDOFGrps() ) != 0)  {
	//dofPtr->saveSensitivity(vNewPtr,0,0,gradNum,numGrads);
	dofPtr->saveDispSensitivity(v,gradNum,numGrads);
    }
    
    return 0;
}

int 
LoadControl::commitSensitivity(int gradNum, int numGrads)
{
    // get model
    AnalysisModel* theAnalysisModel = this->getAnalysisModel();
    
    // Loop through the FE_Elements and set unconditional sensitivities
    FE_Element *elePtr;
    FE_EleIter &theEles = theAnalysisModel->getFEs();    
    while((elePtr = theEles()) != 0) {
	elePtr->commitSensitivity(gradNum, numGrads);
    }
    
    return 0;
}
