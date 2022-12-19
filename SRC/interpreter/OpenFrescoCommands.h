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
// Created: 01/21
// Revision: A
//
// Description: all OpenFresco APIs are defined or declared here

#ifndef OpenFrescoCommands_h
#define OpenFrescoCommands_h

#include "DL_Interpreter.h"
#include <Domain.h>
//#include <FE_Datastore.h>
//#include <FEM_ObjectBrokerAllClasses.h>
#include <Timer.h>
#include <SimulationInformation.h>
#include <elementAPI.h>

class OpenFrescoCommands
{
public:
    explicit OpenFrescoCommands(DL_Interpreter* interpreter);
    ~OpenFrescoCommands();

    DL_Interpreter* getInterpreter();
    Domain* getDomain();

    int getNDF() const {return ndf;}
    void setNDF(int n) {ndf = n;}

    int getNDM() const {return ndm;}
    void setNDM(int n) {ndm = n;}
    
    //void setFileDatabase(const char* filename);
    //FE_Datastore* getDatabase() { return theDatabase; }
    
    Timer* getTimer() { return &theTimer; }
    SimulationInformation* getSimulationInformation() { return &theSimulationInfo; }

    void wipeExp();
    
private:
    DL_Interpreter* interpreter;
    Domain* theDomain;
    int ndf, ndm;
    
    //FE_Datastore* theDatabase;
    //FEM_ObjectBrokerAllClasses theBroker;
    Timer theTimer;
    SimulationInformation theSimulationInfo;
};


/////////////////////////////////////////////////////////////////////////////
// Declaration of all OpenFresco APIs except those declared in elementAPI.h//
/////////////////////////////////////////////////////////////////////////////

// required OpenSees objects commands
int OPS_model();
int OPS_logFile();
int OPS_metaData();
int OPS_defaultUnits();
int OPS_startTimer();
int OPS_stopTimer();
int OPS_Node();
int OPS_HomogeneousBC();
int OPS_HomogeneousBC_X();
int OPS_HomogeneousBC_Y();
int OPS_HomogeneousBC_Z();
int OPS_UniaxialMaterial();
int OPS_CrdTransf();
int OPS_rayleighDamping();
int OPS_setElementRayleighDampingFactors();

// main OpenFresco objects commands 
int OPF_ExperimentalCP();
int OPF_ExperimentalSignalFilter();
int OPF_ExperimentalControl();
int OPF_ExperimentalSetup();
int OPF_ExperimentalSite();
int OPF_ExperimentalTangentStiff();
int OPF_ExperimentalElement();
int OPF_ExperimentalRecorder();

// server OpenFresco commands
int OPF_startLabServer();
int OPF_startLabServerInteractive();
int OPF_setupLabServer();
int OPF_stepLabServer();
int OPF_stopLabServer();
int OPF_startSimAppSiteServer();
int OPF_startSimAppElemServer();

// clear OpenFresco object commands
void OPF_clearExperimentalCPs();
void OPF_clearExperimentalSignalFilters();
void OPF_clearExperimentalControls();
void OPF_clearExperimentalSetups();
void OPF_clearExperimentalSites();
void OPF_clearExperimentalTangentStiffs();

// remove OpenFresco object commands
bool OPF_removeExperimentalCP(int tag);
bool OPF_removeExperimentalSignalFilter(int tag);
bool OPF_removeExperimentalControl(int tag);
bool OPF_removeExperimentalSetup(int tag);
bool OPF_removeExperimentalSite(int tag);
bool OPF_removeExperimentalTangentStiff(int tag);

// miscellaneous OpenFresco commands
int OPF_version();
int OPF_recordExp();
int OPF_wipeExp();
int OPF_removeObject();

#endif
