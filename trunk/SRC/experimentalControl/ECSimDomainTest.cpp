/* ****************************************************************** **
**    OpenFRESCO - Open Framework                                     **
**                 for Experimental Setup and Control                 **
**                                                                    **
**                                                                    **
** Copyright (c) 2006, Yoshikazu Takahashi, Kyoto University          **
** All rights reserved.                                               **
**                                                                    **
** Licensed under the modified BSD License (the "License");           **
** you may not use this file except in compliance with the License.   **
** You may obtain a copy of the License in main directory.            **
** Unless required by applicable law or agreed to in writing,         **
** software distributed under the License is distributed on an        **
** "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,       **
** either express or implied. See the License for the specific        **
** language governing permissions and limitations under the License.  **
**                                                                    **
** Developed by:                                                      **
**   Yoshikazu Takahashi (yos@catfish.dpri.kyoto-u.ac.jp)             **
**   Andreas Schellenberg (andreas.schellenberg@gmx.net)              **
**   Gregory L. Fenves (fenves@berkeley.edu)                          **
**                                                                    **
** ****************************************************************** */

// $Revision$
// $Date$
// $URL: $

// Written: Yoshi (yos@catfish.dpri.kyoto-u.ac.jp)
// Created: 09/06
// Revision: A
//
// Description: This file contains the implementation of 
// ECSimDomainTest.

#include "ECSimDomainTest.h"
//#include <ExperimentalCP.h>
//#include <ExpCPIter.h>

// standard C++ includes
#include <stdlib.h>
#include <iostream>
#include <fstream>

#include <OPS_Globals.h>
#include <StandardStream.h>

#include <ArrayOfTaggedObjects.h>

// includes for the domain classes
#include <Domain.h>
#include <Node.h>
#include <SP_Constraint.h>
#include <ElasticMaterial.h>
#include <Steel01.h>
#include <ElasticBeam2d.h>
#include <LinearCrdTransf2d.h>
#include <LoadPattern.h>
#include <LinearSeries.h>
#include <NodalLoad.h>

// includes for the analysis classes
#include <StaticAnalysis.h>
#include <StaticIntegrator.h>
#include <LoadControl.h>
#include <DisplacementControl.h>
#include <AnalysisModel.h>
#include <Linear.h>
#include <NewtonRaphson.h>
#include <ConvergenceTest.h>
#include <CTestEnergyIncr.h>
#include <CTestNormDispIncr.h>
#include <PlainHandler.h>
#include <PenaltyConstraintHandler.h>
#include <RCM.h>
#include <PlainNumberer.h>
#include <BandSPDLinSOE.h>
#include <BandSPDLinLapackSolver.h>
#include <UmfpackGenLinSOE.h>
#include <UmfpackGenLinSolver.h>

#include <DataOutputFileHandler.h>
#include <NodeRecorder.h>
#include <ElementRecorder.h>


ECSimDomainTest::ECSimDomainTest(int tag)
    : ECSimulation(tag),
    theDomain(0), theModel(0), theConv(0), theSolnAlgo(0), theIntegrator(0),
    theHandler(0), theRCM(0), theNumberer(0), theSolver(0), theSOE(0), 
    theAnalysis(0), thePattern(0), theSeries(0), theSPp(0),
    cDispV(0), cVelV(0), 
    dDispV(0), dVelV(0), dForceV(0),
    cDisp(0.0), dl(1.e-7), n_SPp(0), tagCtrl(0)
{
    theDomain = new Domain();
    
    int ndf = 3;
    int i;
    int n_node = 3;
    Node **theNode = new Node* [n_node];
    theNode[0] = new Node(1, ndf, 0.0, 0.0);
    theNode[1] = new Node(2, ndf, 0.0, 4.572);
    theNode[2] = new Node(3, ndf, 0.0, 6.553);
    for(i=0; i<n_node; i++) {
        theDomain->addNode(theNode[i]);
    }
    // Fixed base at Node 1 (single point constraints)
    int n_SP = 3;
    SP_Constraint **theSP = new SP_Constraint* [3];
    theSP[0] = new SP_Constraint(1, 1, 0, 0.0);
    theSP[1] = new SP_Constraint(2, 1, 1, 0.0);
    theSP[2] = new SP_Constraint(3, 1, 2, 0.0);
    for(i=0; i<n_SP; i++) {
        theDomain->addSP_Constraint(theSP[i]);
    }
    
    // Define element
    Vector dummy(2);
    LinearCrdTransf2d *theTransf = new LinearCrdTransf2d(1);
    
    int n_ele = 2;
    ElasticBeam2d **ele = new ElasticBeam2d* [n_ele];
    double Es = 199.9e9; // (N/m2) 
    double Ac = 0.022774148; // (m2) for column
    double Ic = 0.00057439936; // (m4) for column
    ele[0] = new ElasticBeam2d(1, Ac, Es, Ic, 1, 2, *theTransf);
    ele[1] = new ElasticBeam2d(2, Ac, Es, 10000.*Ic, 2, 3, *theTransf);
    for(i=0; i<n_ele; i++) {
        theDomain->addElement(ele[i]);
    }
    
    // initialize
    theDomain->initialize();
    
    n_SPp = 2;
    tagCtrl = 2;
    
    //  opserr << "Domain : " << *theDomain;
}


ECSimDomainTest::ECSimDomainTest(const ECSimDomainTest& ec)
    : ECSimulation(ec)
{
    // temporarily does nothing
}


ECSimDomainTest::~ECSimDomainTest()
{
    if(theDomain != 0) {
        theDomain->clearAll();
        delete theDomain;
    }
}


int ECSimDomainTest::setSize(ID sizeT, ID sizeO)
{
    // check sizeTrial and sizeOut
    // for ECSimDomainTest object
    
    // ECSimDomainTest object only use 
    // disp, vel for trial
    // disp, vel, force for output
    // check these are available in sizeT/sizeO.
    
    if(sizeT[OF_Resp_Disp] != 2 ||
        sizeT[OF_Resp_Vel] != 2 ||
        sizeO[OF_Resp_Disp] != 2 ||
        sizeO[OF_Resp_Vel] != 2 ||
        sizeO[OF_Resp_Force] != 2) {
        opserr << "ECSimDomainTestTest::setSize - wrong sizeTrial/Out\n"; 
        opserr << "see User Manual.\n";
        exit(1);
    }
    
    *sizeCtrl = sizeT;
    *sizeDaq = sizeO;
    
    return OF_ReturnType_completed;
}


int ECSimDomainTest::setup()
{
    // setup for ctrl/daq vectors of ECSimUniaxialmaterials
    cDispV = new Vector((*sizeCtrl)[0]);
    cVelV = new Vector((*sizeCtrl)[1]); 
    
    dDispV = new Vector((*sizeDaq)[0]);
    dVelV = new Vector((*sizeDaq)[1]);
    dForceV = new Vector((*sizeDaq)[3]);
    
    cDispV->Zero();
    cVelV->Zero();
    
    // define load pattern
    thePattern = new LoadPattern(1);
    theSeries = new LinearSeries(1.0);
    thePattern->setTimeSeries(theSeries);
    theDomain->addLoadPattern(thePattern);
    
    double targetDisp;
    double targetDisp2;
    // ---------------------------
    targetDisp = (*cDispV)[0];
    targetDisp2 = (*cDispV)[1];
    
    theSPp = new SP_Constraint* [n_SPp];
    theSPp[0] = new SP_Constraint(1, tagCtrl, 0, targetDisp);
    theSPp[1] = new SP_Constraint(2, 3, 0, targetDisp2);
    for(int i=0; i<n_SPp; i++) {
        theDomain->addSP_Constraint(theSPp[i], 1);
    }
    theIntegrator = new DisplacementControl(tagCtrl, 0, dl, 
        theDomain,
        1, dl, dl);
    theModel = new AnalysisModel();
    theConv = new CTestNormDispIncr(1.0e-12, 10, 0);
    theSolnAlgo = new NewtonRaphson(*theConv);
    theHandler = new PenaltyConstraintHandler(1.0e12, 1.0e12);
    theRCM = new RCM();
    theNumberer = new DOF_Numberer(*theRCM);
    theSolver = new BandSPDLinLapackSolver();
    theSOE = new BandSPDLinSOE(*theSolver);
    theAnalysis = new StaticAnalysis(*theDomain,
        *theHandler,
        *theNumberer,
        *theModel,
        *theSolnAlgo,
        *theSOE,
        *theIntegrator);
    
    // initialize
    //  theAnalysis->initialize();
    
    theAnalysis->analyze(0);
    
    cDisp = targetDisp;
    
    //  opserr << "\nele[0]: " << theDomain->getElement(1)->getResistingForce();
    //  opserr << "ele[1]: " << theDomain->getElement(2)->getResistingForce();
    
    theAnalysis->clearAll();
    delete theAnalysis;
    
    theDomain->removeLoadPattern(1);
    delete thePattern;
    
    return OF_ReturnType_completed;
}


int ECSimDomainTest::setTrialResponse(const Vector* disp,
    const Vector* vel,
    const Vector* accel,
    const Vector* force,
    const Vector* time)
{
    *cDispV = *disp;
    *cVelV = *vel;
    
    this->control();
    
    return OF_ReturnType_completed;
}


int ECSimDomainTest::getDaqResponse(Vector* disp,
    Vector* vel,
    Vector* accel,
    Vector* force,
    Vector* time)
{
    this->acquire();
    
    *disp = *dDispV;
    *vel = *dVelV;
    *force = *dForceV;
    
    return OF_ReturnType_completed;
}


int ECSimDomainTest::commitState()
{
    ExperimentalControl::commitState();
    
    return OF_ReturnType_completed;
}


ExperimentalControl* ECSimDomainTest::getCopy()
{
    return new ECSimDomainTest(*this);
}


void ECSimDomainTest::Print(OPS_Stream &s, int flag)
{
    s << "ExperimentalControl: " << this->getTag(); 
    s << " type: ECSimDomainTest\n";
    s << "\tDomain: " << *theDomain << endln;
}


int ECSimDomainTest::control()
{
    double targetDisp = (*cDispV)[0];
    double targetDisp2 = (*cDispV)[1];
    double deltaDisp = targetDisp-cDisp;
    //  opserr << "targetDisp = " << (*cDispV)[0];
    dl = deltaDisp/2.0;
    int numSteps = abs((int)(deltaDisp/dl));
    //  if(deltaDisp*dl < 0.0) dl *= -1.;
    //  opserr << "deltaDisp = " << deltaDisp << ", dl = " << dl << endln;
    
    thePattern = new LoadPattern(1);
    theSeries = new LinearSeries(1.0);
    thePattern->setTimeSeries(theSeries);
    theDomain->addLoadPattern(thePattern);
    
    theSPp[0] = new SP_Constraint(1, tagCtrl, 0, targetDisp);
    theSPp[1] = new SP_Constraint(2, 3, 0, targetDisp2);
    for(int i=0; i<n_SPp; i++) {
        theDomain->addSP_Constraint(theSPp[i], 1);
    }
    
    theIntegrator = new DisplacementControl(tagCtrl, 0, dl, 
					   theDomain,
                       1, dl, dl);
    theModel = new AnalysisModel();
    theConv = new CTestNormDispIncr(1.0e-12, 10, 0);
    theSolnAlgo = new NewtonRaphson(*theConv);
    theHandler = new PenaltyConstraintHandler(1.0e12, 1.0e12);
    theRCM = new RCM();
    theNumberer = new DOF_Numberer(*theRCM);
    theSolver = new BandSPDLinLapackSolver();
    theSOE = new BandSPDLinSOE(*theSolver);
    theAnalysis = new StaticAnalysis(*theDomain,
        *theHandler,
        *theNumberer,
        *theModel,
        *theSolnAlgo,
        *theSOE,
        *theIntegrator);
    
    theAnalysis->analyze(numSteps);
    
    cDisp = targetDisp;
    
    //  opserr << "ele[0]: " << theDomain->getElement(1)->getResistingForce();
    //  opserr << "ele[1]: " << theDomain->getElement(2)->getResistingForce();
    
    theAnalysis->clearAll();
    delete theAnalysis;
    
    theDomain->removeLoadPattern(1);
    delete thePattern;
    
    return OF_ReturnType_completed;
}


int ECSimDomainTest::acquire()
{
    double f1, f2;
    //  opserr << *(theDomain->getElement(1));
    const Vector &F1 = theDomain->getElement(1)->getResistingForce();
    f1 = F1(3);
    //  opserr << "F1: " << F1;
    //  opserr << *(theDomain->getElement(2));
    const Vector &F2 = theDomain->getElement(2)->getResistingForce();
    f1 += F2(0);
    f2 = F2(3);
    //  opserr << "F2: " << F2;
    
    const Vector &D1 = theDomain->getNode(2)->getTrialDisp();
    const Vector &D2 = theDomain->getNode(3)->getTrialDisp();
    
    //  opserr << "D1: " << D1;
    //  opserr << "D2: " << D2;
    //  opserr << "D1(0) = " << D1(0) << ", F1(3)+F2(0) = " << f1 << endln;
    //  opserr << "D2(0) = " << D2(0) << ", F2(3) = " << f2 << endln;
    
    (*dDispV)[0] = D1(0);
    (*dForceV)[0] = f1;
    (*dDispV)[1] = D2(0);
    (*dForceV)[1] = f2;
    
    return OF_ReturnType_completed;
}

