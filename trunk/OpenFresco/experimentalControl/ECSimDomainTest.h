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
// $Source$

#ifndef ECSimDomainTest_h
#define ECSimDomainTest_h

// Written: Yoshi (yos@catfish.dpri.kyoto-u.ac.jp)
// Created: 09/06
// Revision: A
//
// Description: This file contains the class definition for 
// ECSimDomainTest.

#include "ECSimulation.h"

class Domain;
class AnalysisModel;
class ConvergenceTest;
class EquiSolnAlgo;
class StaticIntegrator;
class ConstraintHandler;
class RCM;
class DOF_Numberer;
class BandSPDLinSolver;
class LinearSOE;
class StaticAnalysis;
class LoadPattern;
class TimeSeries;
class SP_Constraint;

class ECSimDomainTest : public ECSimulation
{
public:
    // constructors
    ECSimDomainTest(int tag); 
    ECSimDomainTest(const ECSimDomainTest& ec);

    // destructor
    virtual ~ECSimDomainTest();
    
    // public methods to set and to get response
    virtual int setSize(ID sizeT, ID sizeO);
    virtual int setup();
    
    virtual int setTrialResponse(const Vector* disp, 
        const Vector* vel,
        const Vector* accel,
        const Vector* force,
        const Vector* time);
    virtual int getDaqResponse(Vector* disp,
        Vector* vel,
        Vector* accel,
        Vector* force,
        Vector* time);
    
    virtual int commitState();
    virtual ExperimentalControl *getCopy (void);
    
    // public methods for output
    void Print(OPS_Stream &s, int flag = 0);    
    
protected:
    // protected methods to set and to get response
    virtual int control();
    virtual int acquire();
    
private:
    Domain *theDomain;
    AnalysisModel *theModel;
    ConvergenceTest *theConv;
    EquiSolnAlgo *theSolnAlgo;
    StaticIntegrator *theIntegrator;
    ConstraintHandler *theHandler;
    RCM *theRCM;
    DOF_Numberer *theNumberer;
    BandSPDLinSolver *theSolver;
    LinearSOE *theSOE;
    StaticAnalysis *theAnalysis;
    LoadPattern *thePattern;
    TimeSeries *theSeries;
    SP_Constraint **theSPp;
    
    // minimum variable set for UniaxialMaterial
    Vector *cDispV;
    Vector *cVelV;
    
    Vector *dDispV;
    Vector *dVelV;
    Vector *dForceV;
    
    double cDisp; // comitted control disp
    double dl; // minimum control displacement
    int n_SPp; // number of control SP
    int tagCtrl; // node tag for control
};

#endif
