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
#include <elementAPI.h>
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
//#include <CTestNormUnbalance.h>
//#include <CTestEnergyIncr.h>
#include <NewtonRaphson.h>
#include <NewtonLineSearch.h>
#include <RegulaFalsiLineSearch.h>
#include <LoadControl.h>
//#include <DisplacementControl.h>

//#include <TransformationConstraintHandler.h>
#include <PenaltyConstraintHandler.h>
//#include <LagrangeConstraintHandler.h>

#include <PlainNumberer.h>
//#include <RCM.h>

//#include <BandSPDLinSOE.h>
//#include <BandSPDLinLapackSolver.h>
#include <BandGenLinSOE.h>
#include <BandGenLinLapackSolver.h>
//#include <ProfileSPDLinSOE.h>
//#include <ProfileSPDLinDirectSolver.h>
//#include <SymSparseLinSOE.h>
//#include <SymSparseLinSolver.h>
//#include <UmfpackGenLinSOE.h>
//#include <UmfpackGenLinSolver.h>
#include <StaticAnalysis.h>

//#include <DataOutputFileHandler.h>
//#include <NodeRecorder.h>
//#include <ElementRecorder.h>


void* OPF_ECSimDomain()
{
    // pointer to experimental control that will be returned
    ExperimentalControl* theControl = 0;
    
    if (OPS_GetNumRemainingInputArgs() < 5) {
        opserr << "WARNING invalid number of arguments\n";
        opserr << "Want: expControl SimDomain tag -trialCP cpTags -outCP cpTags "
            << "<-ctrlFilters (5 filterTag)> <-daqFilters (5 filterTag)>\n";
        return 0;
    }
    
    // control tag
    int tag;
    int numdata = 1;
    if (OPS_GetIntInput(&numdata, &tag) != 0) {
        opserr << "WARNING invalid expControl SimDomain tag\n";
        return 0;
    }
    
    // trialCPs
    const char* type = OPS_GetString();
    if (strcmp(type, "-trialCP") != 0) {
        opserr << "WARNING expecting -trialCP cpTags\n";
        opserr << "expControl SimDomain " << tag << endln;
        return 0;
    }
    ID cpTags(32);
    int numTrialCPs = 0;
    while (OPS_GetNumRemainingInputArgs() > 0) {
        int cpTag;
        numdata = 1;
        int numArgs = OPS_GetNumRemainingInputArgs();
        if (OPS_GetIntInput(&numdata, &cpTag) < 0) {
            if (numArgs > OPS_GetNumRemainingInputArgs()) {
                // move current arg back by one
                OPS_ResetCurrentInputArg(-1);
            }
            break;
        }
        cpTags(numTrialCPs++) = cpTag;
    }
    if (numTrialCPs == 0) {
        opserr << "WARNING no trialCPTags specified\n";
        opserr << "expControl SimDomain " << tag << endln;
        return 0;
    }
    cpTags.resize(numTrialCPs);
    // create the array to hold the trial control points
    ExperimentalCP** trialCPs = new ExperimentalCP * [numTrialCPs];
    if (trialCPs == 0) {
        opserr << "WARNING out of memory\n";
        opserr << "expControl SimDomain " << tag << endln;
        return 0;
    }
    // populate array with trial control points
    for (int i = 0; i < numTrialCPs; i++) {
        trialCPs[i] = 0;
        trialCPs[i] = OPF_getExperimentalCP(cpTags(i));
        if (trialCPs[i] == 0) {
            opserr << "WARNING experimental control point not found\n";
            opserr << "expControlPoint " << cpTags(i) << endln;
            opserr << "expControl SimDomain " << tag << endln;
            return 0;
        }
    }
    
    // outCPs
    type = OPS_GetString();
    if (strcmp(type, "-outCP") != 0) {
        opserr << "WARNING expecting -outCP cpTags\n";
        opserr << "expControl SimDomain " << tag << endln;
        return 0;
    }
    cpTags.resize(32); cpTags.Zero();
    int numOutCPs = 0;
    while (OPS_GetNumRemainingInputArgs() > 0) {
        int cpTag;
        numdata = 1;
        int numArgs = OPS_GetNumRemainingInputArgs();
        if (OPS_GetIntInput(&numdata, &cpTag) < 0) {
            if (numArgs > OPS_GetNumRemainingInputArgs()) {
                // move current arg back by one
                OPS_ResetCurrentInputArg(-1);
            }
            break;
        }
        cpTags(numOutCPs++) = cpTag;
    }
    if (numOutCPs == 0) {
        opserr << "WARNING no outCPTags specified\n";
        opserr << "expControl SimDomain " << tag << endln;
        return 0;
    }
    cpTags.resize(numOutCPs);
    // create the array to hold the output control points
    ExperimentalCP** outCPs = new ExperimentalCP * [numOutCPs];
    if (outCPs == 0) {
        opserr << "WARNING out of memory\n";
        opserr << "expControl SimDomain " << tag << endln;
        return 0;
    }
    // populate array with output control points
    for (int i = 0; i < numOutCPs; i++) {
        outCPs[i] = 0;
        outCPs[i] = OPF_getExperimentalCP(cpTags(i));
        if (outCPs[i] == 0) {
            opserr << "WARNING experimental control point not found\n";
            opserr << "expControlPoint " << cpTags(i) << endln;
            opserr << "expControl SimDomain " << tag << endln;
            return 0;
        }
    }
    
    // get the Domain
    Domain *theDomain = OPS_GetDomain();
    
    // parsing was successful, allocate the control
    theControl = new ECSimDomain(tag, numTrialCPs, trialCPs,
        numOutCPs, outCPs, theDomain);
    if (theControl == 0) {
        opserr << "WARNING could not create experimental control of type SimDomain\n";
        return 0;
    }
    
    // cleanup dynamic memory
    //if (trialCPs != 0)
    //    delete[] trialCPs;
    //if (outCPs != 0)
    //    delete[] outCPs;
    
    return theControl;
}


ECSimDomain::ECSimDomain(int tag,
    int nTrialCPs, ExperimentalCP **trialcps,
    int nOutCPs, ExperimentalCP **outcps, Domain *thedomain)
    : ECSimulation(tag), numTrialCPs(nTrialCPs), numOutCPs(nOutCPs),
    theDomain(0), theModel(0), theTest(0), theLineSearch(0), theAlgorithm(0),
    theIntegrator(0), theHandler(0), theNumberer(0), theSOE(0), theAnalysis(0),
    theSeries(0), thePattern(0), theSPs(0), theNodes(0), numSPs(0),
    ctrlDisp(0), ctrlVel(0), ctrlAccel(0), ctrlForce(0),
    daqDisp(0), daqVel(0), daqAccel(0), daqForce(0)
{
    // get trial and output control points
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
    theDomain(0), theModel(0), theTest(0), theLineSearch(0), theAlgorithm(0),
    theIntegrator(0), theHandler(0), theNumberer(0), theSOE(0), theAnalysis(0),
    theSeries(0), thePattern(0), theSPs(0), theNodes(0), numSPs(0),
    ctrlDisp(0), ctrlVel(0), ctrlAccel(0), ctrlForce(0),
    daqDisp(0), daqVel(0), daqAccel(0), daqForce(0)
{
    numTrialCPs = ec.numTrialCPs;
    trialCPs    = ec.trialCPs;
    numOutCPs   = ec.numOutCPs;
    outCPs      = ec.outCPs;
    
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
    // delete memory of ctrl vectors
    if (ctrlDisp != 0)
        delete ctrlDisp;
    if (ctrlVel != 0)
        delete ctrlVel;
    if (ctrlAccel != 0)
        delete ctrlAccel;
    if (ctrlForce != 0)
        delete ctrlForce;
    
    // delete memory of daq vectors
    if (daqDisp != 0)
        delete daqDisp;
    if (daqVel != 0)
        delete daqVel;
    if (daqAccel != 0)
        delete daqAccel;
    if (daqForce != 0)
        delete daqForce;
    
    // cleanup the analysis
    if (theAnalysis != 0)  {
        theAnalysis->clearAll();
        delete theAnalysis;
        theAnalysis = 0;
    }
    
    // cleanup the domain
    if (theDomain != 0)
        theDomain->clearAll();
    
    // delete memory of SP constraints and nodes
    if (theSPs != 0)
        delete [] theSPs;
    if (theNodes != 0)
        delete [] theNodes;
    
    // control points are not copies, so do not clean them up here
    //int i;
    //if (trialCPs != 0)  {
    //    for (i=0; i<numTrialCPs; i++)  {
    //        if (trialCPs[i] != 0)
    //            delete trialCPs[i];
    //    }
    //    delete [] trialCPs;
    //}
    //if (outCPs != 0)  {
    //    for (i=0; i<numOutCPs; i++)  {
    //        if (outCPs[i] != 0)
    //            delete outCPs[i];
    //    }
    //    delete [] outCPs;
    //}
    
    opserr << endln;
    opserr << "**********************************\n";
    opserr << "* The Domain has been cleaned up *\n";
    opserr << "**********************************\n";
    opserr << endln;
}


int ECSimDomain::setup()
{
    if (ctrlDisp != 0)
        delete ctrlDisp;
    if (ctrlVel != 0)
        delete ctrlVel;
    if (ctrlAccel != 0)
        delete ctrlAccel;
    if (ctrlForce != 0)
        delete ctrlForce;
    
    if ((*sizeCtrl)(OF_Resp_Disp) != 0)  {
        ctrlDisp = new double [(*sizeCtrl)(OF_Resp_Disp)];
        for (int i=0; i<(*sizeCtrl)(OF_Resp_Disp); i++)
            ctrlDisp[i] = 0.0;
    }
    if ((*sizeCtrl)(OF_Resp_Vel) != 0)  {
        ctrlVel = new double [(*sizeCtrl)(OF_Resp_Vel)];
        for (int i=0; i<(*sizeCtrl)(OF_Resp_Vel); i++)
            ctrlVel[i] = 0.0;
    }
    if ((*sizeCtrl)(OF_Resp_Accel) != 0)  {
        ctrlAccel = new double [(*sizeCtrl)(OF_Resp_Accel)];
        for (int i=0; i<(*sizeCtrl)(OF_Resp_Accel); i++)
            ctrlAccel[i] = 0.0;
    }
    if ((*sizeCtrl)(OF_Resp_Force) != 0)  {
        ctrlForce = new double [(*sizeCtrl)(OF_Resp_Force)];
        for (int i=0; i<(*sizeCtrl)(OF_Resp_Force); i++)
            ctrlForce[i] = 0.0;
    }
    
    if (daqDisp != 0)
        delete daqDisp;
    if (daqVel != 0)
        delete daqVel;
    if (daqAccel != 0)
        delete daqAccel;
    if (daqForce != 0)
        delete daqForce;
    
    if ((*sizeDaq)(OF_Resp_Disp) != 0)  {
        daqDisp = new double [(*sizeDaq)(OF_Resp_Disp)];
        for (int i=0; i<(*sizeDaq)(OF_Resp_Disp); i++)
            daqDisp[i] = 0.0;
    }
    if ((*sizeDaq)(OF_Resp_Vel) != 0)  {
        daqVel = new double [(*sizeDaq)(OF_Resp_Vel)];
        for (int i=0; i<(*sizeDaq)(OF_Resp_Vel); i++)
            daqVel[i] = 0.0;
    }
    if ((*sizeDaq)(OF_Resp_Accel) != 0)  {
        daqAccel = new double [(*sizeDaq)(OF_Resp_Accel)];
        for (int i=0; i<(*sizeDaq)(OF_Resp_Accel); i++)
            daqAccel[i] = 0.0;
    }
    if ((*sizeDaq)(OF_Resp_Force) != 0)  {
        daqForce = new double [(*sizeDaq)(OF_Resp_Force)];
        for (int i=0; i<(*sizeDaq)(OF_Resp_Force); i++)
            daqForce[i] = 0.0;
    }
    
    // print experimental control information
    this->Print(opserr);
    
    // define load pattern
    theSeries  = new LinearSeries(1,1.0);
    thePattern = new LoadPattern(1,1.0);
    thePattern->setTimeSeries(theSeries);
    theDomain->addLoadPattern(thePattern);
    
    // find total number of required SPs
    for (int i=0; i<numTrialCPs; i++)
        numSPs += trialCPs[i]->getNumDOF();
    
    // create array of single point constraints
    theSPs = new SP_Constraint* [numSPs];
    for (int i=0; i<numSPs; i++)
        theSPs[i] = 0;
    
    // loop through all the trial control points
    int iSP = 0;
    for (int i=0; i<numTrialCPs; i++)  {
        // get trial control point parameters
        int nodeTag = trialCPs[i]->getNodeTag();
        int numDir = trialCPs[i]->getNumDOF();
        ID dir = trialCPs[i]->getUniqueDOF();
        
        // loop through all the directions
        for (int j=0; j<numDir; j++)  {
            if ((*sizeCtrl)(OF_Resp_Disp) != 0 &&
                (*sizeCtrl)(OF_Resp_Vel) != 0 &&
                (*sizeCtrl)(OF_Resp_Accel) != 0)
                theSPs[iSP] = new ExpControlSP(nodeTag, dir(j), &ctrlDisp[iSP], 1.0, &ctrlVel[iSP], 1.0, &ctrlAccel[iSP], 1.0);
            else if ((*sizeCtrl)(OF_Resp_Disp) != 0 &&
                (*sizeCtrl)(OF_Resp_Vel) != 0)
                theSPs[iSP] = new ExpControlSP(nodeTag, dir(j), &ctrlDisp[iSP], 1.0, &ctrlVel[iSP], 1.0);
            else if ((*sizeCtrl)(OF_Resp_Disp) != 0)
                theSPs[iSP] = new ExpControlSP(nodeTag, dir(j), &ctrlDisp[iSP], 1.0);
            
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
    
    theModel = new AnalysisModel();
    
    theTest = new CTestNormDispIncr(1.0E-8, 25, 0);
    //theTest = new CTestNormUnbalance(1.0E-8, 25, 0);
    //theTest = new CTestEnergyIncr(1.0E-8, 25, 0);
    
    theAlgorithm = new NewtonRaphson(*theTest);
    //theLineSearch = new RegulaFalsiLineSearch(0.8, 10, 0.1, 10.0, 1);
    //theAlgorithm = new NewtonLineSearch(*theTest, theLineSearch);
    
    theIntegrator = new LoadControl(1.0, 1, 1.0, 1.0);
    
    //theHandler = new TransformationConstraintHandler();
    theHandler = new PenaltyConstraintHandler(1.0E12, 1.0E12);
    //theHandler = new LagrangeConstraintHandler(1.0, 1.0);
    
    theNumberer = new PlainNumberer();
    
    BandGenLinSolver *theSolver = new BandGenLinLapackSolver();
    theSOE = new BandGenLinSOE(*theSolver);
    //ProfileSPDLinSolver *theSolver = new ProfileSPDLinDirectSolver();
    //theSOE = new ProfileSPDLinSOE(*theSolver);
    
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
        sizeTDisp  += (trialCPs[i]->getSizeRspType())(OF_Resp_Disp);
        sizeTForce += (trialCPs[i]->getSizeRspType())(OF_Resp_Force);
    }
    for (int i=0; i<numOutCPs; i++)  {
        sizeODisp  += (outCPs[i]->getSizeRspType())(OF_Resp_Disp);
        sizeOForce += (outCPs[i]->getSizeRspType())(OF_Resp_Force);
    }
    if ((sizeTDisp != 0 && sizeTDisp != sizeT(OF_Resp_Disp)) ||
        (sizeTForce != 0 && sizeTForce != sizeT(OF_Resp_Force)) ||
        (sizeODisp != 0 && sizeODisp != sizeO(OF_Resp_Disp)) ||
        (sizeOForce != 0 && sizeOForce != sizeO(OF_Resp_Force)))  {
        opserr << "ECSimDomain::setSize() - wrong sizeTrial/Out\n"; 
        opserr << "see User Manual.\n";
        exit(OF_ReturnType_failed);
    }
    
    (*sizeCtrl) = sizeT;
    (*sizeDaq)  = sizeO;
    
    return OF_ReturnType_completed;
}


int ECSimDomain::setTrialResponse(
    const Vector* disp,
    const Vector* vel,
    const Vector* accel,
    const Vector* force,
    const Vector* time)
{
    int i, rValue = 0;
    if (disp != 0)  {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Disp); i++)  {
            ctrlDisp[i] = (*disp)(i);
            if (theCtrlFilters[OF_Resp_Disp] != 0)
                ctrlDisp[i] = theCtrlFilters[OF_Resp_Disp]->filtering(ctrlDisp[i]);
        }
    }
    if (vel != 0)  {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Vel); i++)  {
            ctrlVel[i] = (*vel)(i);
            if (theCtrlFilters[OF_Resp_Vel] != 0)
                ctrlVel[i] = theCtrlFilters[OF_Resp_Vel]->filtering(ctrlVel[i]);
        }
    }
    if (accel != 0)  {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Accel); i++)  {
            ctrlAccel[i] = (*accel)(i);
            if (theCtrlFilters[OF_Resp_Accel] != 0)
                ctrlAccel[i] = theCtrlFilters[OF_Resp_Accel]->filtering(ctrlAccel[i]);
        }
    }
    if (force != 0)  {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Force); i++)  {
            ctrlForce[i] = (*force)(i);
            if (theCtrlFilters[OF_Resp_Force] != 0)
                ctrlForce[i] = theCtrlFilters[OF_Resp_Force]->filtering(ctrlForce[i]);
        }
    }
    
    rValue = this->control();
    
    return rValue;
}


int ECSimDomain::getDaqResponse(
    Vector* disp,
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
                daqDisp[i] = theDaqFilters[OF_Resp_Disp]->filtering(daqDisp[i]);
            (*disp)(i) = daqDisp[i];
        }
    }
    if (vel != 0)  {
        for (i=0; i<(*sizeDaq)(OF_Resp_Vel); i++)  {
            if (theDaqFilters[OF_Resp_Vel] != 0)
                daqVel[i] = theDaqFilters[OF_Resp_Vel]->filtering(daqVel[i]);
            (*vel)(i) = daqVel[i];
        }
    }
    if (accel != 0)  {
        for (i=0; i<(*sizeDaq)(OF_Resp_Accel); i++)  {
            if (theDaqFilters[OF_Resp_Accel] != 0)
                daqAccel[i] = theDaqFilters[OF_Resp_Accel]->filtering(daqAccel[i]);
            (*accel)(i) = daqAccel[i];
        }
    }
    if (force != 0)  {
        for (i=0; i<(*sizeDaq)(OF_Resp_Force); i++)  {
            if (theDaqFilters[OF_Resp_Force] != 0)
                daqForce[i] = theDaqFilters[OF_Resp_Force]->filtering(daqForce[i]);
            (*force)(i) = daqForce[i];
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
    
    // ctrl displacements
    if (ctrlDisp != 0 && (
        strcmp(argv[0], "ctrlSig") == 0 ||
        strcmp(argv[0],"ctrlDisp") == 0 ||
        strcmp(argv[0],"ctrlDisplacement") == 0 ||
        strcmp(argv[0],"ctrlDisplacements") == 0))
    {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Disp); i++)  {
            sprintf(outputData,"ctrlDisp%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 1,
            Vector((*sizeCtrl)(OF_Resp_Disp)));
    }
    
    // ctrl velocities
    else if (ctrlVel != 0 && (
        strcmp(argv[0],"ctrlVel") == 0 ||
        strcmp(argv[0],"ctrlVelocity") == 0 ||
        strcmp(argv[0],"ctrlVelocities") == 0))
    {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Vel); i++)  {
            sprintf(outputData,"ctrlVel%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 2,
            Vector((*sizeCtrl)(OF_Resp_Vel)));
    }
    
    // ctrl accelerations
    else if (ctrlAccel != 0 && (
        strcmp(argv[0],"ctrlAccel") == 0 ||
        strcmp(argv[0],"ctrlAcceleration") == 0 ||
        strcmp(argv[0],"ctrlAccelerations") == 0))
    {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Accel); i++)  {
            sprintf(outputData,"ctrlAccel%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 3,
            Vector((*sizeCtrl)(OF_Resp_Accel)));
    }
    
    // ctrl forces
    else if (ctrlForce != 0 && (
        strcmp(argv[0],"ctrlForce") == 0 ||
        strcmp(argv[0],"ctrlForces") == 0))
    {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Force); i++)  {
            sprintf(outputData,"ctrlForce%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 4,
            Vector((*sizeCtrl)(OF_Resp_Force)));
    }
    
    // daq displacements
    else if (daqDisp != 0 && (
        strcmp(argv[0],"daqDisp") == 0 ||
        strcmp(argv[0],"daqDisplacement") == 0 ||
        strcmp(argv[0],"daqDisplacements") == 0))
    {
        for (i=0; i<(*sizeDaq)(OF_Resp_Disp); i++)  {
            sprintf(outputData,"daqDisp%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 5,
            Vector((*sizeDaq)(OF_Resp_Disp)));
    }
    
    // daq velocities
    else if (daqVel != 0 && (
        strcmp(argv[0],"daqVel") == 0 ||
        strcmp(argv[0],"daqVelocity") == 0 ||
        strcmp(argv[0],"daqVelocities") == 0))
    {
        for (i=0; i<(*sizeDaq)(OF_Resp_Vel); i++)  {
            sprintf(outputData,"daqVel%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 6,
            Vector((*sizeDaq)(OF_Resp_Vel)));
    }
    
    // daq accelerations
    else if (daqAccel != 0 && (
        strcmp(argv[0],"daqAccel") == 0 ||
        strcmp(argv[0],"daqAcceleration") == 0 ||
        strcmp(argv[0],"daqAccelerations") == 0))
    {
        for (i=0; i<(*sizeDaq)(OF_Resp_Accel); i++)  {
            sprintf(outputData,"daqAccel%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 7,
            Vector((*sizeDaq)(OF_Resp_Accel)));
    }
    
    // daq forces
    else if (daqForce != 0 && (
        strcmp(argv[0], "daqSig") == 0 ||
        strcmp(argv[0],"daqForce") == 0 ||
        strcmp(argv[0],"daqForces") == 0))
    {
        for (i=0; i<(*sizeDaq)(OF_Resp_Force); i++)  {
            sprintf(outputData,"daqForce%d",i+1);
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
    case 1:  // ctrl signals, ctrl displacements
        resp.setData(ctrlDisp,(*sizeCtrl)(OF_Resp_Disp));
        return info.setVector(resp);
        
    case 2:  // ctrl velocities
        resp.setData(ctrlVel,(*sizeCtrl)(OF_Resp_Vel));
        return info.setVector(resp);
        
    case 3:  // ctrl accelerations
        resp.setData(ctrlAccel,(*sizeCtrl)(OF_Resp_Accel));
        return info.setVector(resp);
        
    case 4:  // ctrl forces
        resp.setData(ctrlForce,(*sizeCtrl)(OF_Resp_Force));
        return info.setVector(resp);
        
    case 5:  // daq displacements
        resp.setData(daqDisp,(*sizeDaq)(OF_Resp_Disp));
        return info.setVector(resp);
        
    case 6:  // daq velocities
        resp.setData(daqVel,(*sizeDaq)(OF_Resp_Vel));
        return info.setVector(resp);
        
    case 7:  // daq accelerations
        resp.setData(daqAccel,(*sizeDaq)(OF_Resp_Accel));
        return info.setVector(resp);
        
    case 8:  // daq signals, daq forces
        resp.setData(daqForce,(*sizeDaq)(OF_Resp_Force));
        return info.setVector(resp);
        
    default:
        return -1;
    }
}


void ECSimDomain::Print(OPS_Stream &s, int flag)
{
    s << "****************************************************************\n";
    s << "* ExperimentalControl: " << this->getTag() << endln; 
    s << "*   type: ECSimDomain";
    s << "\n*   trialCPs:";
    for (int i=0; i<numTrialCPs; i++)
        s << " " << trialCPs[i]->getTag();
    s << "\n*   outCPs:";
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
        if (theDaqFilters[i] != 0)
            s << " " << theDaqFilters[i]->getTag();
        else
            s << " 0";
    }
    s << "\n****************************************************************\n\n";
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
        int numDir = outCPs[i]->getNumDOF();
        ID dir = outCPs[i]->getUniqueDOF();
        
        // loop through all the directions
        for (int j=0; j<numDir; j++)  {
            if ((*sizeDaq)(OF_Resp_Disp) != 0)  {
                const Vector &d = theNodes[i]->getTrialDisp();
                daqDisp[iSP] = d(dir(j));
            }
            if ((*sizeDaq)(OF_Resp_Vel) != 0)  {
                const Vector &v = theNodes[i]->getTrialVel();
                daqVel[iSP] = v(dir(j));
            }
            if ((*sizeDaq)(OF_Resp_Accel) != 0)  {
                const Vector &a = theNodes[i]->getTrialAccel();
                daqAccel[iSP] = a(dir(j));
            }
            if ((*sizeDaq)(OF_Resp_Force) != 0)  {
                const Vector &f = theNodes[i]->getReaction();
                daqForce[iSP] = f(dir(j));
            }
            iSP++;
        }
    }
    
    return OF_ReturnType_completed;
}
