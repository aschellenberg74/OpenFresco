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

// $Revision$
// $Date$
// $URL$

#ifndef ECSimDomain_h
#define ECSimDomain_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 07/07
// Revision: A
//
// Description: This file contains the class definition for ECSimDomain.
// ECSimDomain is a controller class for simulating the behavior of a
// specimen using the OpenSees domain.

#include "ECSimulation.h"

class ExperimentalCP;

class Domain;
class AnalysisModel;
class ConvergenceTest;
class EquiSolnAlgo;
class LineSearch;
class StaticIntegrator;
class ConstraintHandler;
class DOF_Numberer;
class LinearSOE;
class StaticAnalysis;
class TimeSeries;
class LoadPattern;
class SP_Constraint;
class Node;

class ECSimDomain : public ECSimulation
{
public:
    // constructors
    ECSimDomain(int tag, int nTrialCPs, ExperimentalCP **trialCPs,
        int nOutCPs, ExperimentalCP **outCPs, Domain *theDomain);
    ECSimDomain(const ECSimDomain& ec);
    
    // destructor
    virtual ~ECSimDomain();
    
    // method to get class type
    const char *getClassType() const {return "ECSimDomain";};
    
    // public methods to set and to get response
    virtual int setup();
    virtual int setSize(ID sizeT, ID sizeO);
    
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
    
    virtual ExperimentalControl *getCopy();
    
    // public methods for experimental control recorder
    virtual Response *setResponse(const char **argv, int argc,
        OPS_Stream &output);
    virtual int getResponse(int responseID, Information &info);
    
    // public methods for output
    void Print(OPS_Stream &s, int flag = 0);    
    
protected:
    // protected methods to set and to get response
    virtual int control();
    virtual int acquire();
    
private:
    int numTrialCPs;            // number of trial control points
    ExperimentalCP **trialCPs;  // trial control points
    int numOutCPs;              // number of output control points
    ExperimentalCP **outCPs;    // output control points
    
    Domain            *theDomain;
    AnalysisModel     *theModel;
    ConvergenceTest   *theTest;
    LineSearch        *theLineSearch;
    EquiSolnAlgo      *theAlgorithm;
    StaticIntegrator  *theIntegrator;
    ConstraintHandler *theHandler;
    DOF_Numberer      *theNumberer;
    LinearSOE         *theSOE;
    StaticAnalysis    *theAnalysis;
    
    TimeSeries    *theSeries;
    LoadPattern   *thePattern;
    SP_Constraint **theSPs;
    Node          **theNodes;
    
    int numSPs;     // total number of SP constraints
    
    double *ctrlDisp, *ctrlVel, *ctrlAccel, *ctrlForce;
    double *daqDisp, *daqVel, *daqAccel, *daqForce;
};

#endif
