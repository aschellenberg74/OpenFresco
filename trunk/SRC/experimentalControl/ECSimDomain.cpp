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
#include <CTestNormDispIncr.h>
#include <CTestNormUnbalance.h>
#include <CTestEnergyIncr.h>
#include <NewtonRaphson.h>
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
    theDomain(0), theModel(0), theTest(0), theAlgorithm(0), theIntegrator(0),
    theHandler(0), theNumberer(0), theSOE(0), theAnalysis(0),
    theSeries(0), thePattern(0), theSPs(0), theNodes(0), numSPs(0),
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
    theDomain = thedomain;
    if (theDomain->initialize() != 0)  {
        opserr << "ECSimDomain::ECSimDomain() - "
            << "failed to initialize the domain.\n";
        exit(OF_ReturnType_failed);
    }

    opserr << "***********************************\n";
    opserr << "* The Domain has been initialized *\n";
    opserr << "***********************************\n";
    opserr << endln;
}


ECSimDomain::ECSimDomain(const ECSimDomain& ec)
    : ECSimulation(ec), trialCPs(0), outCPs(0),
    theDomain(0), theModel(0), theTest(0), theAlgorithm(0), theIntegrator(0),
    theHandler(0), theNumberer(0), theSOE(0), theAnalysis(0),
    theSeries(0), thePattern(0), theSPs(0), theNodes(0), numSPs(0),
    targDisp(0), targVel(0), targAccel(0), targForce(0),
    measDisp(0), measVel(0), measAccel(0), measForce(0)
{
    numTrialCPs = ec.numTrialCPs;
    trialCPs = ec.trialCPs;
    numOutCPs = ec.numOutCPs;
    outCPs = ec.outCPs;
    
    // initialize the domain
    theDomain = ec.theDomain;
    if (theDomain->initialize() != 0)  {
        opserr << "ECSimDomain::ECSimDomain() - "
            << "failed to initialize the domain.\n";
        exit(OF_ReturnType_failed);
    }
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
    
    // cleanup the analysis
    if (theAnalysis != 0)  {
        theAnalysis->clearAll();
        delete theAnalysis;
    }

    // delete memory of SP constraints and nodes
    if (theSPs != 0)
        delete [] theSPs;
    if (theNodes != 0)
        delete [] theNodes;
    
    // cleanup the domain
    if (theDomain != 0)
        delete theDomain;
    
    // delete memory of control points
    if (trialCPs != 0)
        delete [] trialCPs;
    if (outCPs != 0)
        delete [] outCPs;
    
    opserr << endln;
    opserr << "**********************************\n";
    opserr << "* The Domain has been cleaned up *\n";
    opserr << "**********************************\n";
    opserr << endln;
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
    
    // print experimental control information
    this->Print(opserr);
    
    // define load pattern
    theSeries  = new LinearSeries(1.0);
    thePattern = new LoadPattern(1);
    thePattern->setTimeSeries(theSeries);
    theDomain->addLoadPattern(thePattern);
    
    // find total number of required SPs
    for (int i=0; i<numTrialCPs; i++)
        numSPs += trialCPs[i]->getNumUniqueDir();
    
    // create array of single point constraints
    theSPs = new SP_Constraint* [numSPs];
    for (int i=0; i<numSPs; i++)
        theSPs[i] = 0;
    
    // loop through all the trial control points
    int iSP = 0;
    for (int i=0; i<numTrialCPs; i++)  {
        // get trial control point parameters
        int nodeTag = trialCPs[i]->getNodeTag();
        int numDir = trialCPs[i]->getNumUniqueDir();
        ID dir = trialCPs[i]->getUniqueDir();
        
        // loop through all the directions
        for (int j=0; j<numDir; j++)  {
            if ((*sizeCtrl)(OF_Resp_Disp) != 0 &&
                (*sizeCtrl)(OF_Resp_Vel) != 0 &&
                (*sizeCtrl)(OF_Resp_Accel) != 0)
                theSPs[iSP] = new ExpControlSP(iSP+1, nodeTag, dir(j), &targDisp[iSP], 1.0, &targVel[iSP], 1.0, &targAccel[iSP], 1.0);
            else if ((*sizeCtrl)(OF_Resp_Disp) != 0 &&
                (*sizeCtrl)(OF_Resp_Vel) != 0)
                theSPs[iSP] = new ExpControlSP(iSP+1, nodeTag, dir(j), &targDisp[iSP], 1.0, &targVel[iSP], 1.0);
            else if ((*sizeCtrl)(OF_Resp_Disp) != 0)
                theSPs[iSP] = new ExpControlSP(iSP+1, nodeTag, dir(j), &targDisp[iSP], 1.0);
            
            // add the SP constraints to the load pattern
            theDomain->addSP_Constraint(theSPs[iSP], 1);
            iSP++;
        }
    }
    
    // create array of output nodes to be used in acquire method
    theNodes = new Node* [numOutCPs];
    for (int i=0; i<numOutCPs; i++)  {
        int nodeTag = outCPs[i]->getNodeTag();
        theNodes[i] = theDomain->getNode(nodeTag);
    }
    
    theModel      = new AnalysisModel();
    theTest       = new CTestNormDispIncr(1.0E-8, 100, 0);
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
    
    opserr << "*****************\n";
    opserr << "* Running...... *\n";
    opserr << "*****************\n";
    opserr << endln;
    
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


Response* ECSimDomain::setResponse(const char **argv, int argc,
    OPS_Stream &output)
{
    int i;
    char outputData[15];
    Response *theResponse = 0;
    
    output.tag("ExpControlOutput");
    output.attr("ctrlType",this->getClassType());
    output.attr("ctrlTag",this->getTag());
        
    // target displacements
    if (strcmp(argv[0],"targDisp") == 0 ||
        strcmp(argv[0],"targetDisp") == 0 ||
        strcmp(argv[0],"targetDisplacement") == 0 ||
        strcmp(argv[0],"targetDisplacements") == 0)
    {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Disp); i++)  {
            sprintf(outputData,"targDisp%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 1,
            Vector((*sizeCtrl)(OF_Resp_Disp)));
    }
    
    // target velocities
    if (strcmp(argv[0],"targVel") == 0 ||
        strcmp(argv[0],"targetVel") == 0 ||
        strcmp(argv[0],"targetVelocity") == 0 ||
        strcmp(argv[0],"targetVelocities") == 0)
    {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Vel); i++)  {
            sprintf(outputData,"targVel%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 2,
            Vector((*sizeCtrl)(OF_Resp_Vel)));
    }
    
    // target accelerations
    if (strcmp(argv[0],"targAccel") == 0 ||
        strcmp(argv[0],"targetAccel") == 0 ||
        strcmp(argv[0],"targetAcceleration") == 0 ||
        strcmp(argv[0],"targetAccelerations") == 0)
    {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Accel); i++)  {
            sprintf(outputData,"targAccel%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 3,
            Vector((*sizeCtrl)(OF_Resp_Accel)));
    }
    
    // target forces
    if (strcmp(argv[0],"targForce") == 0 ||
        strcmp(argv[0],"targetForce") == 0 ||
        strcmp(argv[0],"targetForces") == 0)
    {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Force); i++)  {
            sprintf(outputData,"targForce%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 4,
            Vector((*sizeCtrl)(OF_Resp_Force)));
    }
    
    // measured displacements
    if (strcmp(argv[0],"measDisp") == 0 ||
        strcmp(argv[0],"measuredDisp") == 0 ||
        strcmp(argv[0],"measuredDisplacement") == 0 ||
        strcmp(argv[0],"measuredDisplacements") == 0)
    {
        for (i=0; i<(*sizeDaq)(OF_Resp_Disp); i++)  {
            sprintf(outputData,"measDisp%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 5,
            Vector((*sizeDaq)(OF_Resp_Disp)));
    }
    
    // measured velocities
    if (strcmp(argv[0],"measVel") == 0 ||
        strcmp(argv[0],"measuredVel") == 0 ||
        strcmp(argv[0],"measuredVelocity") == 0 ||
        strcmp(argv[0],"measuredVelocities") == 0)
    {
        for (i=0; i<(*sizeDaq)(OF_Resp_Vel); i++)  {
            sprintf(outputData,"measVel%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 6,
            Vector((*sizeDaq)(OF_Resp_Vel)));
    }
    
    // measured accelerations
    if (strcmp(argv[0],"measAccel") == 0 ||
        strcmp(argv[0],"measuredAccel") == 0 ||
        strcmp(argv[0],"measuredAcceleration") == 0 ||
        strcmp(argv[0],"measuredAccelerations") == 0)
    {
        for (i=0; i<(*sizeDaq)(OF_Resp_Accel); i++)  {
            sprintf(outputData,"measAccel%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 7,
            Vector((*sizeDaq)(OF_Resp_Accel)));
    }
    
    // measured forces
    if (strcmp(argv[0],"measForce") == 0 ||
        strcmp(argv[0],"measuredForce") == 0 ||
        strcmp(argv[0],"measuredForces") == 0)
    {
        for (i=0; i<(*sizeDaq)(OF_Resp_Force); i++)  {
            sprintf(outputData,"measForce%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 8,
            Vector((*sizeDaq)(OF_Resp_Force)));
    }
    
    output.endTag();
    
    return theResponse;
}


int ECSimDomain::getResponse(int responseID, Information &info)
{
    Vector resp(0);
    
    switch (responseID)  {
    case 1:  // target displacements
        resp.setData(targDisp,(*sizeCtrl)(OF_Resp_Disp));
        return info.setVector(resp);
        
    case 2:  // target velocities
        resp.setData(targVel,(*sizeCtrl)(OF_Resp_Vel));
        return info.setVector(resp);
        
    case 3:  // target accelerations
        resp.setData(targAccel,(*sizeCtrl)(OF_Resp_Accel));
        return info.setVector(resp);
        
    case 4:  // target forces
        resp.setData(targForce,(*sizeCtrl)(OF_Resp_Force));
        return info.setVector(resp);
        
    case 5:  // measured displacements
        resp.setData(measDisp,(*sizeDaq)(OF_Resp_Disp));
        return info.setVector(resp);
        
    case 6:  // measured velocities
        resp.setData(measVel,(*sizeDaq)(OF_Resp_Vel));
        return info.setVector(resp);
        
    case 7:  // measured accelerations
        resp.setData(measAccel,(*sizeDaq)(OF_Resp_Accel));
        return info.setVector(resp);
        
    case 8:  // measured forces
        resp.setData(measForce,(*sizeDaq)(OF_Resp_Force));
        return info.setVector(resp);
        
    default:
        return -1;
    }
}


void ECSimDomain::Print(OPS_Stream &s, int flag)
{
    s << "****************************************************************\n";
    s << "* ExperimentalControl: " << this->getTag() << endln; 
    s << "*   type: ECSimDomain\n";
    s << "*   trialCPs:";
    for (int i=0; i<numTrialCPs; i++)
        s << " " << trialCPs[i]->getTag();
    s << endln;
    s << "*   outCPs:";
    for (int i=0; i<numOutCPs; i++)
        s << " " << outCPs[i]->getTag();
    s << "\n*   ctrlFilters:";
    for (int i=0; i<OF_Resp_All; i++)  {
        if (theCtrlFilters[i] != 0)
            s << " " << theCtrlFilters[i]->getTag();
        else
            s << " 0";
    }
    s << "\n*   daqFilters:";
    for (int i=0; i<OF_Resp_All; i++)  {
        if (theCtrlFilters[i] != 0)
            s << " " << theCtrlFilters[i]->getTag();
        else
            s << " 0";
    }
    s << endln;
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
        int numDir = outCPs[i]->getNumUniqueDir();
        ID dir = outCPs[i]->getUniqueDir();

        // loop through all the directions
        for (int j=0; j<numDir; j++)  {
            if ((*sizeDaq)(OF_Resp_Disp) != 0)  {
                const Vector &d = theNodes[i]->getTrialDisp();
                measDisp[iSP] = d(dir(j));
            }
            if ((*sizeDaq)(OF_Resp_Vel) != 0)  {
                const Vector &v = theNodes[i]->getTrialVel();
                measVel[iSP] = v(dir(j));
            }
            if ((*sizeDaq)(OF_Resp_Accel) != 0)  {
                const Vector &a = theNodes[i]->getTrialAccel();
                measAccel[iSP] = a(dir(j));
            }
            if ((*sizeDaq)(OF_Resp_Force) != 0)  {
                const Vector &f = theNodes[i]->getReaction();
                measForce[iSP] = f(dir(j));
            }
            iSP++;
        }
    }

    return OF_ReturnType_completed;
}
