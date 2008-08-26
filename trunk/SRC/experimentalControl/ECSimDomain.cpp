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
// Description: This file contains the implementation of ECSimDomain.

#include "ECSimDomain.h"
#include <ExperimentalCP.h>

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <OPS_Globals.h>
#include <StandardStream.h>
#include <ArrayOfTaggedObjects.h>

#include <Domain.h>
#include <Node.h>
#include <LinearSeries.h>
#include <LoadPattern.h>
#include <SP_Constraint.h>
#include <ExpControlSP.h>
#include <NodalLoad.h>

#include <AnalysisModel.h>
//#include <ConvergenceTest.h>
#include <CTestNormDispIncr.h>
#include <CTestNormUnbalance.h>
#include <CTestEnergyIncr.h>
#include <NewtonRaphson.h>
//#include <StaticIntegrator.h>
#include <LoadControl.h>
#include <DisplacementControl.h>

#include <TransformationConstraintHandler.h>
#include <PenaltyConstraintHandler.h>
#include <LagrangeConstraintHandler.h>

#include <PlainNumberer.h>
#include <RCM.h>

#include <BandSPDLinSOE.h>
#include <BandSPDLinLapackSolver.h>
#include <BandGenLinSOE.h>
#include <BandGenLinLapackSolver.h>
#include <ProfileSPDLinSOE.h>
#include <ProfileSPDLinDirectSolver.h>
#include <SymSparseLinSOE.h>
#include <SymSparseLinSolver.h>
#include <UmfpackGenLinSOE.h>
#include <UmfpackGenLinSolver.h>
#include <StaticAnalysis.h>

#include <DataOutputFileHandler.h>
#include <NodeRecorder.h>
#include <ElementRecorder.h>


ECSimDomain::ECSimDomain(int tag,
    int nTrialCPs, ExperimentalCP **trialcps,
    int nOutCPs, ExperimentalCP **outcps, Domain *thedomain)
    : ECSimulation(tag), numTrialCPs(nTrialCPs), numOutCPs(nOutCPs),
    theDomain(thedomain), theModel(0), theTest(0), theAlgorithm(0),
    theIntegrator(0), theHandler(0), theNumberer(0), theSOE(0),
    theAnalysis(0), theSeries(0), thePattern(0), theSP(0), numSPs(0),
    targDisp(0), targVel(0), targAccel(0), targForce(0),
    measDisp(0), measVel(0), measAccel(0), measForce(0)
{    
    if (trialcps == 0 || outcps == 0)  {
      opserr << "ECSimDomain::ECSimDomain() - "
          << "null trialCPs or outCPs array passed.\n";
      exit(OF_ReturnType_failed);
    }
    trialCPs = trialcps;
    outCPs = outcps;

    // initialize the domain
    theDomain->initialize();
}


ECSimDomain::ECSimDomain(const ECSimDomain& ec)
    : ECSimulation(ec)
{
    // temporarily does nothing
}


ECSimDomain::~ECSimDomain()
{
    // delete memory of target vectors
    if (targDisp != 0)
        delete targDisp;
    if (targVel != 0)
        delete targVel;
    if (targAccel != 0)
        delete targAccel;
    if (targForce != 0)
        delete targForce;
    
    // delete memory of measured vectors
    if (measDisp != 0)
        delete measDisp;
    if (measVel != 0)
        delete measVel;
    if (measAccel != 0)
        delete measAccel;
    if (measForce != 0)
        delete measForce;

    // cleanup the domain
    if (theDomain != 0)  {
        theDomain->clearAll();
        delete theDomain;
    }
}


int ECSimDomain::setup()
{
    if (targDisp != 0)
        delete targDisp;
    if (targVel != 0)
        delete targVel;
    if (targAccel != 0)
        delete targAccel;
    if (targForce != 0)
        delete targForce;

    if ((*sizeCtrl)(OF_Resp_Disp) != 0)  {
        targDisp = new double [(*sizeCtrl)(OF_Resp_Disp)];
        for (int i=0; i<(*sizeCtrl)(OF_Resp_Disp); i++)
            targDisp[i] = 0.0;
    }
    if ((*sizeCtrl)(OF_Resp_Vel) != 0)  {
        targVel = new double [(*sizeCtrl)(OF_Resp_Vel)];
        for (int i=0; i<(*sizeCtrl)(OF_Resp_Vel); i++)
            targVel[i] = 0.0;
    }
    if ((*sizeCtrl)(OF_Resp_Accel) != 0)  {
        targAccel = new double [(*sizeCtrl)(OF_Resp_Accel)];
        for (int i=0; i<(*sizeCtrl)(OF_Resp_Accel); i++)
            targAccel[i] = 0.0;
    }
    if ((*sizeCtrl)(OF_Resp_Force) != 0)  {
        targForce = new double [(*sizeCtrl)(OF_Resp_Force)];
        for (int i=0; i<(*sizeCtrl)(OF_Resp_Force); i++)
            targForce[i] = 0.0;
    }

    if (measDisp != 0)
        delete measDisp;
    if (measVel != 0)
        delete measVel;
    if (measAccel != 0)
        delete measAccel;
    if (measForce != 0)
        delete measForce;

    if ((*sizeDaq)(OF_Resp_Disp) != 0)  {
        measDisp = new double [(*sizeDaq)(OF_Resp_Disp)];
        for (int i=0; i<(*sizeDaq)(OF_Resp_Disp); i++)
            measDisp[i] = 0.0;
    }
    if ((*sizeDaq)(OF_Resp_Vel) != 0)  {
        measVel = new double [(*sizeDaq)(OF_Resp_Vel)];
        for (int i=0; i<(*sizeDaq)(OF_Resp_Vel); i++)
            measVel[i] = 0.0;
    }
    if ((*sizeDaq)(OF_Resp_Accel) != 0)  {
        measAccel = new double [(*sizeDaq)(OF_Resp_Accel)];
        for (int i=0; i<(*sizeDaq)(OF_Resp_Accel); i++)
            measAccel[i] = 0.0;
    }
    if ((*sizeDaq)(OF_Resp_Force) != 0)  {
        measForce = new double [(*sizeDaq)(OF_Resp_Force)];
        for (int i=0; i<(*sizeDaq)(OF_Resp_Force); i++)
            measForce[i] = 0.0;
    }

    // define load pattern
    theSeries  = new LinearSeries(1.0);
    thePattern = new LoadPattern(1);
    thePattern->setTimeSeries(theSeries);
    theDomain->addLoadPattern(thePattern);

    // find total number of existing SPs
    int numESPs = theDomain->getNumSPs();

    // find total number of new SPs
    for (int i=0; i<numTrialCPs; i++)
        numSPs += trialCPs[i]->getNumUniqueDir();
    theSP = new SP_Constraint* [numSPs];

    // loop through all the trial control points
    int iSP = 0;
    for (int i=0; i<numTrialCPs; i++)  {
        // get trial control point parameters
        int nodeTag = trialCPs[i]->getNodeTag();
        int numDir = trialCPs[i]->getNumUniqueDir();
        ID dir = trialCPs[i]->getUniqueDir();
		Vector fact = trialCPs[i]->getFactor();

        // loop through all the directions
        for (int j=0; j<numDir; j++)  {
            if ((*sizeCtrl)(OF_Resp_Vel) != 0 && (*sizeCtrl)(OF_Resp_Accel) != 0)
                theSP[iSP] = new ExpControlSP(numESPs+1+iSP, nodeTag, dir(j), &targDisp[iSP], 1.0, &targVel[iSP], 1.0, &targAccel[iSP], 1.0);
            else if ((*sizeCtrl)(OF_Resp_Accel) != 0)
                theSP[iSP] = new ExpControlSP(numESPs+1+iSP, nodeTag, dir(j), &targDisp[iSP], 1.0, &targVel[iSP], 1.0);
            else
                theSP[iSP] = new ExpControlSP(numESPs+1+iSP, nodeTag, dir(j), &targDisp[iSP], fact(j));
            
            // add the SP constraints to the load pattern
            theDomain->addSP_Constraint(theSP[iSP], 1);
            iSP++;
        }
    }

    theModel      = new AnalysisModel();
    theTest       = new CTestNormDispIncr(1.0E-12, 100, 0);
    theAlgorithm  = new NewtonRaphson(*theTest);
    theIntegrator = new LoadControl(1.0, 1, 1.0, 1.0);
    //theHandler    = new TransformationConstraintHandler();
    theHandler    = new PenaltyConstraintHandler(1.0E12, 1.0E12);
    theNumberer   = new PlainNumberer();
    //RCM *theRCM = new RCM(false);
    //theNumberer   = new DOF_Numberer(*theRCM);
    //BandGenLinSolver *theSolver = new BandGenLinLapackSolver();
    //theSOE        = new BandGenLinSOE(*theSolver);
    ProfileSPDLinSolver *theSolver = new ProfileSPDLinDirectSolver();
    theSOE        = new ProfileSPDLinSOE(*theSolver);

    theAnalysis = new StaticAnalysis(*theDomain, *theHandler, *theNumberer,
                                     *theModel, *theAlgorithm, *theSOE,
                                     *theIntegrator, theTest);

    // initialize and analyze one step
    theAnalysis->initialize();
    theAnalysis->analyze(1);

    return OF_ReturnType_completed;
}


int ECSimDomain::setSize(ID sizeT, ID sizeO)
{
    // check sizeTrial and sizeOut
    // for ECSimDomain object

    // ECSimDomain objects can use 
    // disp, vel, accel and force for trial and
    // disp, vel, accel and force for output
    // only check if disp and force are available in sizeT/sizeO.
    int sizeTDisp = 0, sizeTForce = 0;
    int sizeODisp = 0, sizeOForce = 0;
    for (int i=0; i<numTrialCPs; i++)  {
        sizeTDisp  += (trialCPs[i]->getSizeRespType())(OF_Resp_Disp);
        sizeTForce += (trialCPs[i]->getSizeRespType())(OF_Resp_Force);
    }
    for (int i=0; i<numOutCPs; i++)  {
        sizeODisp  += (outCPs[i]->getSizeRespType())(OF_Resp_Disp);
        sizeOForce += (outCPs[i]->getSizeRespType())(OF_Resp_Force);
    }
    if ((sizeTDisp != 0 && sizeTDisp != sizeT(OF_Resp_Disp)) ||
        (sizeTForce != 0 && sizeTForce != sizeT(OF_Resp_Force)) ||
        (sizeODisp != 0 && sizeODisp != sizeO(OF_Resp_Disp)) ||
        (sizeOForce != 0 && sizeOForce != sizeO(OF_Resp_Force)))  {
        opserr << "ECSimDomain::setSize() - wrong sizeTrial/Out\n"; 
        opserr << "see User Manual.\n";
        exit(OF_ReturnType_failed);
    }

    *sizeCtrl = sizeT;
    *sizeDaq  = sizeO;

    return OF_ReturnType_completed;
}


int ECSimDomain::setTrialResponse(const Vector* disp,
    const Vector* vel,
    const Vector* accel,
    const Vector* force,
    const Vector* time)
{
    int i, rValue = 0;
    if (disp != 0)  {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Disp); i++)  {
            targDisp[i] = (*disp)(i);
            if (theCtrlFilters[OF_Resp_Disp] != 0)
                targDisp[i] = theCtrlFilters[OF_Resp_Disp]->filtering(targDisp[i]);
        }
    }
    if (vel != 0)  {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Vel); i++)  {
            targVel[i] = (*vel)(i);
            if (theCtrlFilters[OF_Resp_Vel] != 0)
                targVel[i] = theCtrlFilters[OF_Resp_Vel]->filtering(targVel[i]);
        }
    }
    if (accel != 0)  {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Accel); i++)  {
            targAccel[i] = (*accel)(i);
            if (theCtrlFilters[OF_Resp_Accel] != 0)
                targAccel[i] = theCtrlFilters[OF_Resp_Accel]->filtering(targAccel[i]);
        }
    }
    if (force != 0)  {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Force); i++)  {
            targForce[i] = (*force)(i);
            if (theCtrlFilters[OF_Resp_Force] != 0)
                targForce[i] = theCtrlFilters[OF_Resp_Force]->filtering(targForce[i]);
        }
    }

    rValue = this->control();

    return rValue;
}


int ECSimDomain::getDaqResponse(Vector* disp,
    Vector* vel,
    Vector* accel,
    Vector* force,
    Vector* time)
{
    this->acquire();

    int i;
    if (disp != 0)  {
        for (i=0; i<(*sizeDaq)(OF_Resp_Disp); i++)  {
            if (theDaqFilters[OF_Resp_Disp] != 0)
                measDisp[i] = theDaqFilters[OF_Resp_Disp]->filtering(measDisp[i]);
            (*disp)(i) = measDisp[i];
        }
    }
    if (vel != 0)  {
        for (i=0; i<(*sizeDaq)(OF_Resp_Vel); i++)  {
            if (theDaqFilters[OF_Resp_Vel] != 0)
                measVel[i] = theDaqFilters[OF_Resp_Vel]->filtering(measVel[i]);
            (*vel)(i) = measVel[i];
        }
    }
    if (accel != 0)  {
        for (i=0; i<(*sizeDaq)(OF_Resp_Accel); i++)  {
            if (theDaqFilters[OF_Resp_Accel] != 0)
                measAccel[i] = theDaqFilters[OF_Resp_Accel]->filtering(measAccel[i]);
            (*accel)(i) = measAccel[i];
        }
    }
    if (force != 0)  {
        for (i=0; i<(*sizeDaq)(OF_Resp_Force); i++)  {
            if (theDaqFilters[OF_Resp_Force] != 0)
                measForce[i] = theDaqFilters[OF_Resp_Force]->filtering(measForce[i]);
            (*force)(i) = measForce[i];
        }
    }

    return OF_ReturnType_completed;
}


int ECSimDomain::commitState()
{
    return OF_ReturnType_completed;
}


ExperimentalControl* ECSimDomain::getCopy()
{
    return new ECSimDomain(*this);
}


void ECSimDomain::Print(OPS_Stream &s, int flag)
{
    s << "****************************************************************\n";
    s << "* ExperimentalControl: " << this->getTag() << endln; 
    s << "* type: ECSimDomain\n";
    s << "*   trialCPs:";
    for (int i=0; i<numTrialCPs; i++)
        s << " " << trialCPs[i]->getTag();
    s << endln;
    s << "*   outCPs:";
    for (int i=0; i<numOutCPs; i++)
        s << " " << outCPs[i]->getTag();
    s << endln;
    s << "*   Domain: " << *theDomain << endln;
    if (theCtrlFilters != 0)
        s << "*   ctrlFilter: " << *theCtrlFilters << endln;
    if (theDaqFilters != 0)
        s << "*   daqFilter: " << *theDaqFilters << endln;
    s << "****************************************************************\n";
    s << endln;
}


int ECSimDomain::control()
{
    theAnalysis->analyze(1);

    return OF_ReturnType_completed;
}


int ECSimDomain::acquire()
{
    // get nodal reactions if forces need to be acquired
    if ((*sizeDaq)(OF_Resp_Force) != 0)
        theDomain->calculateNodalReactions(true);

    // loop through all the output control points
    int iSP = 0;
    for (int i=0; i<numOutCPs; i++)  {
        // get output control point parameters
        int nodeTag = outCPs[i]->getNodeTag();
        int numDir = outCPs[i]->getNumUniqueDir();
        ID dir = outCPs[i]->getUniqueDir();
		Vector fact = outCPs[i]->getFactor();

        // loop through all the directions
        for (int j=0; j<numDir; j++)  {
            if ((*sizeDaq)(OF_Resp_Disp) != 0)  {
                const Vector &d = theDomain->getNode(nodeTag)->getTrialDisp();
                measDisp[iSP] = fact(j)*d(dir(j));
            }
            if ((*sizeDaq)(OF_Resp_Vel) != 0)  {
                const Vector &v = theDomain->getNode(nodeTag)->getTrialVel();
                measVel[iSP] = fact(j)*v(dir(j));
            }
            if ((*sizeDaq)(OF_Resp_Accel) != 0)  {
                const Vector &a = theDomain->getNode(nodeTag)->getTrialAccel();
                measAccel[iSP] = fact(j)*a(dir(j));
            }
            if ((*sizeDaq)(OF_Resp_Force) != 0)  {
                const Vector &f = theDomain->getNode(nodeTag)->getReaction();
                measForce[iSP] = fact(j)*f(dir(j));
            }
            iSP++;
        }
    }

    return OF_ReturnType_completed;
}
