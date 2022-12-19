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
// Created: 09/06
// Revision: A
//
// Description: Defines the entry point for the DLL application and
// a function that can be called to set the global pointer variables in 
// the dll to be the same as those in the existing process address space.

#include <tcl.h>
#include <StandardStream.h>
#include <conio.h>
#include <Domain.h>
#include <AnalysisModel.h>
#include <StaticAnalysis.h>
#include <DirectIntegrationAnalysis.h>
#include <VariableTimeStepDirectIntegrationAnalysis.h>
#include <elementAPI.h>
#include <FrescoGlobals.h>
#include <ExperimentalSite.h>

#ifdef _WIN32
#define DllExport _declspec(dllexport)
#else
#define DllExport
extern Domain *ops_TheActiveDomain;
double ops_Dt = 0.0;
StandardStream sserr;
OPS_Stream *opserrPtr = &sserr;
#endif

// main OpenFresco objects commands 
extern int OPF_ExperimentalCP();
extern int OPF_ExperimentalSignalFilter();
extern int OPF_ExperimentalControl();
extern int OPF_ExperimentalSetup();
extern int OPF_ExperimentalSite();
extern int OPF_ExperimentalTangentStiff();
extern int OPF_ExperimentalElement();
extern int OPF_ExperimentalRecorder();
extern int OPF_recordExp();

// server OpenFresco commands
extern int OPF_startLabServer();
extern int OPF_setupLabServer();
extern int OPF_stepLabServer();
extern int OPF_stopLabServer();
extern int OPF_startSimAppSiteServer();
extern int OPF_startSimAppElemServer();

// clear OpenFresco object commands
extern void OPF_clearExperimentalCPs();
extern void OPF_clearExperimentalSignalFilters();
extern void OPF_clearExperimentalControls();
extern void OPF_clearExperimentalSetups();
extern void OPF_clearExperimentalSites();
extern void OPF_clearExperimentalTangentStiffs();

// remove OpenFresco object commands
extern bool OPF_removeExperimentalCP(int tag);
extern bool OPF_removeExperimentalSignalFilter(int tag);
extern bool OPF_removeExperimentalControl(int tag);
extern bool OPF_removeExperimentalSetup(int tag);
extern bool OPF_removeExperimentalSite(int tag);
extern bool OPF_removeExperimentalTangentStiff(int tag);

Tcl_Interp* theInterp = 0;
Domain* theDomain = 0;
StaticAnalysis** theStaticAnalysis = 0;
DirectIntegrationAnalysis** theTransientAnalysis = 0;
VariableTimeStepDirectIntegrationAnalysis** theVariableTimeStepTransientAnalysis = 0;

extern "C" int OPS_ResetInputNoBuilder(ClientData clientData, Tcl_Interp * interp, int cArg, int mArg, TCL_Char * *argv, Domain * domain);


// experimental control point commands
int openFresco_addExperimentalCP(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    // reset the input args
    OPS_ResetInputNoBuilder(clientData, interp, 1, argc, argv, theDomain);
    
    return OPF_ExperimentalCP();
}


// experimental signal filter commands
int openFresco_addExperimentalSignalFilter(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    // reset the input args
    OPS_ResetInputNoBuilder(clientData, interp, 1, argc, argv, theDomain);
    
    return OPF_ExperimentalSignalFilter();
}


// experimental control commands
int openFresco_addExperimentalControl(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    // reset the input args
    OPS_ResetInputNoBuilder(clientData, interp, 1, argc, argv, theDomain);
    
    return OPF_ExperimentalControl();
}


// experimental setup commands
int openFresco_addExperimentalSetup(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    // reset the input args
    OPS_ResetInputNoBuilder(clientData, interp, 1, argc, argv, theDomain);
    
    return OPF_ExperimentalSetup();
}


// experimental site commands
int openFresco_addExperimentalSite(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    // reset the input args
    OPS_ResetInputNoBuilder(clientData, interp, 1, argc, argv, theDomain);
    
    return OPF_ExperimentalSite();
}


// experimental tangent stiffness commands
int openFresco_addExperimentalTangentStiff(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    // reset the input args
    OPS_ResetInputNoBuilder(clientData, interp, 1, argc, argv, theDomain);
    
    return OPF_ExperimentalTangentStiff();
}


// experimental element commands
int openFresco_addExperimentalElement(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    // reset the input args
    OPS_ResetInputNoBuilder(clientData, interp, 1, argc, argv, theDomain);
    
    return OPF_ExperimentalElement();
}


// experimental recorder commands
int openFresco_addExperimentalRecorder(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    // reset the input args
    OPS_ResetInputNoBuilder(clientData, interp, 1, argc, argv, theDomain);
    
    return OPF_ExperimentalRecorder();
}
int openFresco_record(ClientData clientData,
    Tcl_Interp* interp, int argc, TCL_Char** argv)
{
    return OPF_recordExp();
}


// start laboratory server command
int openFresco_startLabServer(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    // reset the input args
    OPS_ResetInputNoBuilder(clientData, interp, 1, argc, argv, theDomain);
    
    return OPF_startLabServer();
}


// setup laboratory server command
int openFresco_setupLabServer(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    // reset the input args
    OPS_ResetInputNoBuilder(clientData, interp, 1, argc, argv, theDomain);
    
    return OPF_setupLabServer();
}


// step laboratory server command
int openFresco_stepLabServer(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    // reset the input args
    OPS_ResetInputNoBuilder(clientData, interp, 1, argc, argv, theDomain);
    
    return OPF_stepLabServer();
}


// stop laboratory server command
int openFresco_stopLabServer(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    // reset the input args
    OPS_ResetInputNoBuilder(clientData, interp, 1, argc, argv, theDomain);
    
    return OPF_stopLabServer();
}


// start simulation application site server command
int openFresco_startSimAppSiteServer(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    // reset the input args
    OPS_ResetInputNoBuilder(clientData, interp, 1, argc, argv, theDomain);
    
    return OPF_startSimAppSiteServer();
}


// start simulation application element server command
int openFresco_startSimAppElemServer(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    // reset the input args
    OPS_ResetInputNoBuilder(clientData, interp, 1, argc, argv, theDomain);
    
    return OPF_startSimAppElemServer();
}


// opensees command to analyze a model interactively
int opensees_analyzeModelInteractive(ClientData clientData,
    Tcl_Interp* interp, int argc, TCL_Char** argv)
{
    // reset the input args
    OPS_ResetInputNoBuilder(clientData, interp, 1, argc, argv, theDomain);
    
    int result = 0;
    int numSteps;
    bool exitYet = false;
    bool showMsg = true;
    int action = 'r';
    int step = 1;
    int numdata = 1;
    
    theStaticAnalysis = OPS_GetStaticAnalysis();
    theTransientAnalysis = OPS_GetTransientAnalysis();
    theVariableTimeStepTransientAnalysis = OPS_GetVariableTimeStepTransientAnalysis();
    
    if (*theStaticAnalysis != 0) {
        if (OPS_GetNumRemainingInputArgs() < 1) {
            opserr << "WARNING insufficient arguments\n"
                << "Want: analyzeInteractive numSteps ...\n";
            return -1;
        }
        
        if (OPS_GetIntInput(&numdata, &numSteps) < 0) {
            opserr << "WARNING invalid analyzeInteractive numSteps\n";
            return -1;
        }
        
        // interactively run the analysis (i.e. the test) for numSteps
        opserr << "\nStaticAnalysis - now interactively running for "
            << numSteps << " steps..." << endln;
        opserr << "Press 'p' to pause, 'r' to resume, 's' to step, or 'e' to end the test." << endln;
        while (step <= numSteps && exitYet == false) {
            // check if we got a keyboard input
            if (_kbhit())
                action = _getch();
            
            switch (action) {
            case 'r':
                // run or resume test
                if (!showMsg) {
                    opserr << "\nStaticAnalysis - now resuming the test..." << endln;
                    showMsg = true;
                }
                result = (*theStaticAnalysis)->analyze(1);
                step++;
                break;
            case 'p':
                // pause test
                if (showMsg) {
                    opserr << "\nStaticAnalysis - at step "
                        << step << " - now pausing the test..." << endln;
                    showMsg = false;
                }
                break;
            case 's':
            case '0':
                // 10^0 times step test
                if (!showMsg) {
                    opserr << "\nStaticAnalysis - at step "
                        << step << " - now stepping the test..." << endln;
                    showMsg = false;
                }
                result = (*theStaticAnalysis)->analyze(1);
                action = 'p';
                step++;
                break;
            case '1':
                // 10^1 times step test
                if (!showMsg) {
                    opserr << "\nStaticAnalysis - at step "
                        << step << " - now 10x stepping the test..." << endln;
                    showMsg = false;
                }
                result = (*theStaticAnalysis)->analyze(10);
                action = 'p';
                step += 10;
                break;
            case '2':
                // 10^2 times step test
                if (!showMsg) {
                    opserr << "\nStaticAnalysis - at step "
                        << step << " - now 100x stepping the test..." << endln;
                    showMsg = false;
                }
                result = (*theStaticAnalysis)->analyze(100);
                action = 'p';
                step += 100;
                break;
            case '3':
                // 10^3 times step test
                if (!showMsg) {
                    opserr << "\nStaticAnalysis - at step "
                        << step << " - now 1000x stepping the test..." << endln;
                    showMsg = false;
                }
                result = (*theStaticAnalysis)->analyze(1000);
                action = 'p';
                step += 1000;
                break;
            case '4':
                // 10^4 times step test
                if (!showMsg) {
                    opserr << "\nStaticAnalysis - at step "
                        << step << " - now 10000x stepping the test..." << endln;
                    showMsg = false;
                }
                result = (*theStaticAnalysis)->analyze(10000);
                action = 'p';
                step += 10000;
                break;
            case 'e':
                // end test
                opserr << "\nStaticAnalysis - at step "
                    << step << " - now shut down." << endln;
                (*theStaticAnalysis)->clearAll();
                exitYet = true;
                break;
            }
        }
    }
    else if (*theTransientAnalysis != 0) {
        if (OPS_GetNumRemainingInputArgs() < 2) {
            opserr << "WARNING insufficient arguments\n"
                << "Want: analyzeInteractive numSteps dt\n";
            return -1;
        }
        
        double dT;
        if (OPS_GetIntInput(&numdata, &numSteps) < 0) {
            opserr << "WARNING invalid analyzeInteractive numSteps\n";
            return -1;
        }
        if (OPS_GetDoubleInput(&numdata, &dT) < 0) {
            opserr << "WARNING invalid analyzeInteractive dt\n";
            return -1;
        }
        
        // set global timestep variable
        ops_Dt = dT;
        
        if (OPS_GetNumRemainingInputArgs() == 0) {
            
            // interactively run the analysis (i.e. the test) for numSteps
            opserr << "\nTransientAnalysis - now interactively running for "
                << numSteps << " steps..." << endln;
            opserr << "Press 'p' to pause, 'r' to resume, 's' to step, or 'e' to end the test." << endln;
            while (step <= numSteps && exitYet == false) {
                // check if we got a keyboard input
                if (_kbhit())
                    action = _getch();
                
                switch (action) {
                case 'r':
                    // run or resume test
                    if (!showMsg) {
                        opserr << "\nTransientAnalysis - now resuming the test..." << endln;
                        showMsg = true;
                    }
                    result = (*theTransientAnalysis)->analyze(1,dT);
                    step++;
                    break;
                case 'p':
                    // pause test
                    if (showMsg) {
                        opserr << "\nTransientAnalysis - at step "
                            << step << " - now pausing the test..." << endln;
                        showMsg = false;
                    }
                    break;
                case 's':
                case '0':
                    // 10^0 times step test
                    if (!showMsg) {
                        opserr << "\nTransientAnalysis - at step "
                            << step << " - now stepping the test..." << endln;
                        showMsg = false;
                    }
                    result = (*theTransientAnalysis)->analyze(1,dT);
                    action = 'p';
                    step++;
                    break;
                case '1':
                    // 10^1 times step test
                    if (!showMsg) {
                        opserr << "\nTransientAnalysis - at step "
                            << step << " - now 10x stepping the test..." << endln;
                        showMsg = false;
                    }
                    result = (*theTransientAnalysis)->analyze(10, dT);
                    action = 'p';
                    step+=10;
                    break;
                case '2':
                    // 10^2 times step test
                    if (!showMsg) {
                        opserr << "\nTransientAnalysis - at step "
                            << step << " - now 100x stepping the test..." << endln;
                        showMsg = false;
                    }
                    result = (*theTransientAnalysis)->analyze(100, dT);
                    action = 'p';
                    step+=100;
                    break;
                case '3':
                    // 10^3 times step test
                    if (!showMsg) {
                        opserr << "\nTransientAnalysis - at step "
                            << step << " - now 1000x stepping the test..." << endln;
                        showMsg = false;
                    }
                    result = (*theTransientAnalysis)->analyze(1000, dT);
                    action = 'p';
                    step += 1000;
                    break;
                case '4':
                    // 10^4 times step test
                    if (!showMsg) {
                        opserr << "\nTransientAnalysis - at step "
                            << step << " - now 10000x stepping the test..." << endln;
                        showMsg = false;
                    }
                    result = (*theTransientAnalysis)->analyze(10000, dT);
                    action = 'p';
                    step += 10000;
                    break;
                case 'e':
                    // end test
                    opserr << "\nTransientAnalysis - at step "
                        << step << " - now shut down." << endln;
                    (*theTransientAnalysis)->clearAll();
                    exitYet = true;
                    break;
                }
            }
        }
        else if (OPS_GetNumRemainingInputArgs() < 3) {
            opserr << "WARNING insufficient arguments for variable transient\n"
                << "Want: analyzeInteractive numSteps dt dtMin dtMax Jd\n";
            return -1;
        }
        else {
            int Jd;
            double dtMin, dtMax;
            if (OPS_GetDoubleInput(&numdata, &dtMin) < 0) {
                opserr << "WARNING invalid analyzeInteractive dtMin\n";
                return -1;
            }
            if (OPS_GetDoubleInput(&numdata, &dtMax) < 0) {
                opserr << "WARNING invalid analyzeInteractive dtMax\n";
                return -1;
            }
            if (OPS_GetIntInput(&numdata, &Jd) < 0) {
                opserr << "WARNING invalid analyzeInteractive Jd\n";
                return -1;
            }
            
            if (*theVariableTimeStepTransientAnalysis != 0) {
                
                // interactively run the analysis (i.e. the test) for numSteps
                opserr << "\nVariableTimeStepTransientAnalysis - now interactively running for "
                    << numSteps << " steps..." << endln;
                opserr << "Press 'p' to pause, 'r' to resume, 's' to step, or 'e' to end the test." << endln;
                while (step <= numSteps && exitYet == false) {
                    // check if we got a keyboard input
                    if (_kbhit())
                        action = _getch();
                    
                    switch (action) {
                    case 'r':
                        // run or resume test
                        if (!showMsg) {
                            opserr << "\nVariableTimeStepTransientAnalysis - now resuming the test..." << endln;
                            showMsg = true;
                        }
                        result = (*theVariableTimeStepTransientAnalysis)->analyze(1, dT, dtMin, dtMax, Jd);
                        step++;
                        break;
                    case 'p':
                        // pause test
                        if (showMsg) {
                            opserr << "\nVariableTimeStepTransientAnalysis - at step "
                                << step << " - now pausing the test..." << endln;
                            showMsg = false;
                        }
                        break;
                    case 's':
                    case '0':
                        // 10^0 times step test
                        if (!showMsg) {
                            opserr << "\nVariableTimeStepTransientAnalysis - at step "
                                << step << " - now stepping the test..." << endln;
                            showMsg = false;
                        }
                        result = (*theVariableTimeStepTransientAnalysis)->analyze(1, dT, dtMin, dtMax, Jd);
                        action = 'p';
                        step++;
                        break;
                    case '1':
                        // 10^1 times step test
                        if (!showMsg) {
                            opserr << "\nVariableTimeStepTransientAnalysis - at step "
                                << step << " - now 10x stepping the test..." << endln;
                            showMsg = false;
                        }
                        result = (*theVariableTimeStepTransientAnalysis)->analyze(10, dT, dtMin, dtMax, Jd);
                        action = 'p';
                        step += 10;
                        break;
                    case '2':
                        // 10^2 times step test
                        if (!showMsg) {
                            opserr << "\nVariableTimeStepTransientAnalysis - at step "
                                << step << " - now 100x stepping the test..." << endln;
                            showMsg = false;
                        }
                        result = (*theVariableTimeStepTransientAnalysis)->analyze(100, dT, dtMin, dtMax, Jd);
                        action = 'p';
                        step += 100;
                        break;
                    case '3':
                        // 10^3 times step test
                        if (!showMsg) {
                            opserr << "\nVariableTimeStepTransientAnalysis - at step "
                                << step << " - now 1000x stepping the test..." << endln;
                            showMsg = false;
                        }
                        result = (*theVariableTimeStepTransientAnalysis)->analyze(1000, dT, dtMin, dtMax, Jd);
                        action = 'p';
                        step += 1000;
                        break;
                    case '4':
                        // 10^4 times step test
                        if (!showMsg) {
                            opserr << "\nVariableTimeStepTransientAnalysis - at step "
                                << step << " - now 10000x stepping the test..." << endln;
                            showMsg = false;
                        }
                        result = (*theVariableTimeStepTransientAnalysis)->analyze(10000, dT, dtMin, dtMax, Jd);
                        action = 'p';
                        step += 10000;
                        break;
                    case 'e':
                        // end test
                        opserr << "\nVariableTimeStepTransientAnalysis - at step "
                            << step << " - now shut down." << endln;
                        (*theVariableTimeStepTransientAnalysis)->clearAll();
                        exitYet = true;
                        break;
                    }
                }
            }
            else {
                opserr << "WARNING analyzeInteractive - no variable time step transient analysis object constructed\n";
                return -1;
            }
        }
    }
    else {
        opserr << "WARNING No Analysis type has been specified\n";
        return -1;
    }
    
    if (result < 0) {
        opserr << "OpenSees > analyzeInteractive failed, returned: " << result << " error flag\n";
    }
    
    if (OPS_SetIntOutput(&numdata, &result, true) < 0) {
        opserr << "WARNING failed to set output\n";
        return -1;
    }
    
    return 0;
}


// wipe entire experiment
int openFresco_wipeExp(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    // OpenSees will clean up theDomain when wipe is called
    //if (theDomain != 0)
    //    theDomain->clearAll();
    
    OPF_clearExperimentalCPs();
    OPF_clearExperimentalSignalFilters();
    OPF_clearExperimentalControls();
    OPF_clearExperimentalSetups();
    OPF_clearExperimentalSites();
    OPF_clearExperimentalTangentStiffs();
    
    return TCL_OK;
}


int OPF_removeObject();
int openFresco_removeObject(ClientData clientData,
    Tcl_Interp* interp, int argc, TCL_Char** argv)
{
    // reset the input args
    OPS_ResetInputNoBuilder(clientData, interp, 1, argc, argv, theDomain);
    
    return OPF_removeObject();
}


int OPF_removeObject()
{
    // make sure there is a minimum number of arguments
    if (OPS_GetNumRemainingInputArgs() < 2)  {
        opserr << "WARNING insufficient number of removeExp component arguments\n";
        opserr << "Want: removeExp type <specific args>\n";
        return -1;
    }
    
    const char* type = OPS_GetString();
    if (strcmp(type, "controlPoint") == 0) {
        if (OPS_GetNumRemainingInputArgs() < 1) {
            opserr << "WARNING invalid number of arguments\n";
            opserr << "Want: removeExp controlPoint tag\n";
            return -1;
        }
        int tag;
        int numdata = 1;
        if (OPS_GetIntInput(&numdata, &tag) < 0) {
            opserr << "WARNING invalid removeExp controlPoint tag\n";
            return -1;
        }
        if (OPF_removeExperimentalCP(tag) == false) {
            opserr << "WARNING could not remove expControlPoint with tag " << tag << endln;
            return -1;
        }
    }
    else if (strcmp(type, "controlPoints") == 0) {
        OPF_clearExperimentalCPs();
    }
    else if (strcmp(type, "signalFilter") == 0) {
        if (OPS_GetNumRemainingInputArgs() < 1) {
            opserr << "WARNING invalid number of arguments\n";
            opserr << "Want: removeExp signalFilter tag\n";
            return -1;
        }
        int tag;
        int numdata = 1;
        if (OPS_GetIntInput(&numdata, &tag) < 0) {
            opserr << "WARNING invalid removeExp signalFilter tag\n";
            return -1;
        }
        if (OPF_removeExperimentalSignalFilter(tag) == false) {
            opserr << "WARNING could not remove expSignalFilter with tag " << tag << endln;
            return -1;
        }
    }
    else if (strcmp(type, "signalFilters") == 0) {
        OPF_clearExperimentalSignalFilters();
    }
    else if (strcmp(type, "control") == 0) {
        if (OPS_GetNumRemainingInputArgs() < 1) {
            opserr << "WARNING invalid number of arguments\n";
            opserr << "Want: removeExp control tag\n";
            return -1;
        }
        int tag;
        int numdata = 1;
        if (OPS_GetIntInput(&numdata, &tag) < 0) {
            opserr << "WARNING invalid removeExp control tag\n";
            return -1;
        }
        if (OPF_removeExperimentalControl(tag) == false) {
            opserr << "WARNING could not remove expControl with tag " << tag << endln;
            return -1;
        }
    }
    else if (strcmp(type, "controls") == 0) {
        OPF_clearExperimentalControls();
    }
    else if (strcmp(type, "setup") == 0) {
        if (OPS_GetNumRemainingInputArgs() < 1) {
            opserr << "WARNING invalid number of arguments\n";
            opserr << "Want: removeExp setup tag\n";
            return -1;
        }
        int tag;
        int numdata = 1;
        if (OPS_GetIntInput(&numdata, &tag) < 0) {
            opserr << "WARNING invalid removeExp setup tag\n";
            return -1;
        }
        if (OPF_removeExperimentalSetup(tag) == false) {
            opserr << "WARNING could not remove expSetup with tag " << tag << endln;
            return -1;
        }
    }
    else if (strcmp(type, "setups") == 0) {
        OPF_clearExperimentalSetups();
    }
    else if (strcmp(type, "site") == 0) {
        if (OPS_GetNumRemainingInputArgs() < 1) {
            opserr << "WARNING invalid number of arguments\n";
            opserr << "Want: removeExp site tag\n";
            return -1;
        }
        int tag;
        int numdata = 1;
        if (OPS_GetIntInput(&numdata, &tag) < 0) {
            opserr << "WARNING invalid removeExp site tag\n";
            return -1;
        }
        if (OPF_removeExperimentalSite(tag) == false) {
            opserr << "WARNING could not remove expSite with tag " << tag << endln;
            return -1;
        }
    }
    else if (strcmp(type, "sites") == 0) {
        OPF_clearExperimentalSites();
    }
    else if (strcmp(type, "tangentStiff") == 0) {
        if (OPS_GetNumRemainingInputArgs() < 1) {
            opserr << "WARNING invalid number of arguments\n";
            opserr << "Want: removeExp tangentStiff tag\n";
            return -1;
        }
        int tag;
        int numdata = 1;
        if (OPS_GetIntInput(&numdata, &tag) < 0) {
            opserr << "WARNING invalid removeExp tangentStiff tag\n";
            return -1;
        }
        if (OPF_removeExperimentalTangentStiff(tag) == false) {
            opserr << "WARNING could not remove expTangentStiff with tag " << tag << endln;
            return -1;
        }
    }
    else if (strcmp(type, "tangentStiffs") == 0) {
        OPF_clearExperimentalTangentStiffs();
    }
    else if (strcmp(type, "recorder") == 0) {
        if (OPS_GetNumRemainingInputArgs() < 1) {
            opserr << "WARNING invalid number of arguments\n";
            opserr << "Want: removeExp recorder tag\n";
            return -1;
        }
        int tag;
        int numdata = 1;
        if (OPS_GetIntInput(&numdata, &tag) < 0) {
            opserr << "WARNING invalid removeExp recorder tag\n";
            return -1;
        }
        ExperimentalSite* theSite = OPF_getExperimentalSiteFirst();
        if (theSite == 0) {
            opserr << "WARNING failed to get first experimental site\n";
            return -1;
        }
        if ((theSite->removeRecorder(tag)) < 0) {
            opserr << "WARNING could not remove expRecorder with tag " << tag << endln;
            return -1;
        }
    }
    else if (strcmp(type, "recorders") == 0) {
        ExperimentalSite* theSite = OPF_getExperimentalSiteFirst();
        if (theSite == 0) {
            opserr << "WARNING failed to get first experimental site\n";
            return -1;
        }
        if ((theSite->removeRecorders()) < 0) {
            opserr << "WARNING could not remove expRecorders\n";
            return -1;
        }
    }
    else {
        // experimental object type not recognized
        opserr << "WARNING unknown removeExp type: "
            << type << ": check the manual\n";
        return -1;
    }
    
    return 0;
}


int openFresco_version(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    char buffer[20];
    
    sprintf(buffer, "%s", OPF_VERSION);
    Tcl_SetResult(interp, buffer, TCL_VOLATILE);
    
    return TCL_OK;
}


extern "C" int OPS_SetIntOutput(int* numData, int* data, bool scalar)
{
    int numArgs = *numData;
    char buffer[40];
    
    for (int i = 0; i < numArgs; i++) {
        sprintf(buffer, "%d ", data[i]);
        Tcl_AppendResult(theInterp, buffer, NULL);
    }
    
    return 0;
}


// This is a package initialization procedure, which is called
// by Tcl when this package is to be added to an interpreter.
extern "C" DllExport int
OpenFresco(ClientData clientData, Tcl_Interp *interp, int argc,
           TCL_Char **argv, Domain *thedomain)
{
    int code;
    
    theInterp = interp;
    theDomain = thedomain;
    
    if (Tcl_InitStubs(interp, TCL_VERSION, 0) == NULL)  {
        return TCL_ERROR;
    }
    
    // add the package to list of available packages
    code = Tcl_PkgProvide(interp, "OpenFresco", OPF_VERSION);
    if (code != TCL_OK)  {
        return code;
    }
    
    // beginning of OpenFresco additions
    fprintf(stderr,"\n\n");
    fprintf(stderr,"\t OpenFresco -- Open Framework for Experimental Setup and Control\n");
    fprintf(stderr,"\t               Version %s %s              \n\n",OPF_VERSION,WIN_ARCH);
    fprintf(stderr,"\t Copyright (c) 2006 The Regents of the University of California \n");
    fprintf(stderr,"\t                       All Rights Reserved                      \n\n\n");
    
    Tcl_CreateCommand(interp, "expControlPoint", openFresco_addExperimentalCP,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "expSignalFilter", openFresco_addExperimentalSignalFilter,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "expControl", openFresco_addExperimentalControl,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "expSetup", openFresco_addExperimentalSetup,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "expSite", openFresco_addExperimentalSite,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "expTangentStiff", openFresco_addExperimentalTangentStiff,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "expElement", openFresco_addExperimentalElement,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "expRecorder", openFresco_addExperimentalRecorder,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "recordExp", openFresco_record,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "startLabServer", openFresco_startLabServer,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "setupLabServer", openFresco_setupLabServer,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "stepLabServer", openFresco_stepLabServer,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "stopLabServer", openFresco_stopLabServer,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "startSimAppSiteServer", openFresco_startSimAppSiteServer,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "startSimAppElemServer", openFresco_startSimAppElemServer,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "analyzeInteractive", opensees_analyzeModelInteractive,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "wipeExp", openFresco_wipeExp,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "removeExp", openFresco_removeObject,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "packageVersion", openFresco_version,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    return TCL_OK;
}
