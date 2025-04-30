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
extern int OPF_setSizeExperimentalSite();
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

// global variables
Tcl_Interp* theInterp = 0;
Domain* theDomain = 0;
StaticAnalysis** theStaticAnalysis = 0;
DirectIntegrationAnalysis** theTransientAnalysis = 0;
VariableTimeStepDirectIntegrationAnalysis** theVariableTimeStepTransientAnalysis = 0;


int OPS_SetIntOutput(int* numData, int* data, bool scalar)
{
    int numArgs = *numData;
    char buffer[40];
    
    for (int i = 0; i < numArgs; i++) {
        sprintf(buffer, "%d ", data[i]);
        Tcl_AppendResult(theInterp, buffer, NULL);
    }
    
    return 0;
}


int OPS_SetDoubleOutput(int* numData, double* data, bool scalar)
{
    int numArgs = *numData;
    char buffer[40];

    for (int i = 0; i < numArgs; i++) {
        sprintf(buffer, "%35.20f ", data[i]);
        Tcl_AppendResult(theInterp, buffer, NULL);
    }

    return 0;
}


int OPS_SetString(const char* str)
{
    Tcl_SetResult(theInterp, (char*)str, TCL_VOLATILE);
    
    return 0;
}


int OPF_removeObject()
{
    // make sure there is a minimum number of arguments
    if (OPS_GetNumRemainingInputArgs() < 2) {
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


int OPF_version()
{
    if (OPS_SetString(OPF_VERSION) < 0) {
        opserr << "WARNING failed to set version string\n";
        return -1;
    }
    
    return 0;
}


// experimental control point commands
int Tcl_opf_expControlPoint(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    // reset the input args
    OPS_ResetCommandLine(argc, 1, argv);    
    
    if (OPF_ExperimentalCP() < 0) return TCL_ERROR;
    
    return TCL_OK;
}


// experimental signal filter commands
int Tcl_opf_expSignalFilter(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    // reset the input args
    OPS_ResetCommandLine(argc, 1, argv);
    
    if (OPF_ExperimentalSignalFilter() < 0) return TCL_ERROR;
    
    return TCL_OK;
}


// experimental control commands
int Tcl_opf_expControl(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    // reset the input args
    OPS_ResetCommandLine(argc, 1, argv);
    
    if (OPF_ExperimentalControl() < 0) return TCL_ERROR;
    
    return TCL_OK;
}


// experimental setup commands
int Tcl_opf_expSetup(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    // reset the input args
    OPS_ResetCommandLine(argc, 1, argv);
    
    if (OPF_ExperimentalSetup() < 0) return TCL_ERROR;
    
    return TCL_OK;
}


// experimental site commands
int Tcl_opf_expSite(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    // reset the input args
    OPS_ResetCommandLine(argc, 1, argv);
    
    if (OPF_ExperimentalSite() < 0) return TCL_ERROR;
    
    return TCL_OK;
}
int Tcl_opf_setSizeExpSite(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    // reset the input args
    OPS_ResetCommandLine(argc, 1, argv);
    
    if (OPF_setSizeExperimentalSite() < 0) return TCL_ERROR;
    
    return TCL_OK;
}


// experimental tangent stiffness commands
int Tcl_opf_expTangentStiff(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    // reset the input args
    OPS_ResetCommandLine(argc, 1, argv);
    
    if (OPF_ExperimentalTangentStiff() < 0) return TCL_ERROR;
    
    return TCL_OK;
}


// experimental element commands
int Tcl_opf_expElement(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    // reset the input args
    OPS_ResetCommandLine(argc, 1, argv);
    
    if (OPF_ExperimentalElement() < 0) return TCL_ERROR;
    
    return TCL_OK;
}


// experimental recorder commands
int Tcl_opf_expRecorder(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    // reset the input args
    OPS_ResetCommandLine(argc, 1, argv);
    
    if (OPF_ExperimentalRecorder() < 0) return TCL_ERROR;
    
    return TCL_OK;
}
int Tcl_opf_recordExp(ClientData clientData,
    Tcl_Interp* interp, int argc, TCL_Char** argv)
{
    if (OPF_recordExp() < 0) return TCL_ERROR;
    
    return TCL_OK;
}


// start laboratory server command
int Tcl_opf_startLabServer(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    // reset the input args
    OPS_ResetCommandLine(argc, 1, argv);
    
    if (OPF_startLabServer() < 0) return TCL_ERROR;
    
    return TCL_OK;
}


// setup laboratory server command
int Tcl_opf_setupLabServer(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    // reset the input args
    OPS_ResetCommandLine(argc, 1, argv);
    
    if (OPF_setupLabServer() < 0) return TCL_ERROR;
    
    return TCL_OK;
}


// step laboratory server command
int Tcl_opf_stepLabServer(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    // reset the input args
    OPS_ResetCommandLine(argc, 1, argv);
    
    if (OPF_stepLabServer() < 0) return TCL_ERROR;
    
    return TCL_OK;
}


// stop laboratory server command
int Tcl_opf_stopLabServer(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    // reset the input args
    OPS_ResetCommandLine(argc, 1, argv);
    
    if (OPF_stopLabServer() < 0) return TCL_ERROR;
    
    return TCL_OK;
}


// start simulation application site server command
int Tcl_opf_startSimAppSiteServer(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    // reset the input args
    OPS_ResetCommandLine(argc, 1, argv);
    
    if (OPF_startSimAppSiteServer() < 0) return TCL_ERROR;
    
    return TCL_OK;
}


// start simulation application element server command
int Tcl_opf_startSimAppElemServer(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    // reset the input args
    OPS_ResetCommandLine(argc, 1, argv);
    
    if (OPF_startSimAppElemServer() < 0) return TCL_ERROR;
    
    return TCL_OK;
}


// opensees command to analyze a model interactively
int Tcl_ops_analyzeModelInteractive(ClientData clientData,
    Tcl_Interp* interp, int argc, TCL_Char** argv)
{
    // reset the input args
    OPS_ResetCommandLine(argc, 1, argv);
    
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
            return TCL_ERROR;
        }
        
        if (OPS_GetIntInput(&numdata, &numSteps) < 0) {
            opserr << "WARNING invalid analyzeInteractive numSteps\n";
            return TCL_ERROR;
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
            return TCL_ERROR;
        }
        
        double dT;
        if (OPS_GetIntInput(&numdata, &numSteps) < 0) {
            opserr << "WARNING invalid analyzeInteractive numSteps\n";
            return TCL_ERROR;
        }
        if (OPS_GetDoubleInput(&numdata, &dT) < 0) {
            opserr << "WARNING invalid analyzeInteractive dt\n";
            return TCL_ERROR;
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
            return TCL_ERROR;
        }
        else {
            int Jd;
            double dtMin, dtMax;
            if (OPS_GetDoubleInput(&numdata, &dtMin) < 0) {
                opserr << "WARNING invalid analyzeInteractive dtMin\n";
                return TCL_ERROR;
            }
            if (OPS_GetDoubleInput(&numdata, &dtMax) < 0) {
                opserr << "WARNING invalid analyzeInteractive dtMax\n";
                return TCL_ERROR;
            }
            if (OPS_GetIntInput(&numdata, &Jd) < 0) {
                opserr << "WARNING invalid analyzeInteractive Jd\n";
                return TCL_ERROR;
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
                return TCL_ERROR;
            }
        }
    }
    else {
        opserr << "WARNING No Analysis type has been specified\n";
        return TCL_ERROR;
    }
    
    if (result < 0) {
        opserr << "OpenSees > analyzeInteractive failed, returned: " << result << " error flag\n";
    }
    
    if (OPS_SetIntOutput(&numdata, &result, true) < 0) {
        opserr << "WARNING failed to set output\n";
        return TCL_ERROR;
    }
    
    return TCL_OK;
}


// wipe entire experiment
int Tcl_opf_wipeExp(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    OPF_clearExperimentalCPs();
    OPF_clearExperimentalSignalFilters();
    OPF_clearExperimentalControls();
    OPF_clearExperimentalSetups();
    OPF_clearExperimentalSites();
    OPF_clearExperimentalTangentStiffs();
    
    // OpenSees will clean up theDomain when wipe is called
    //if (theDomain != 0)
    //    theDomain->clearAll();
    
    return TCL_OK;
}


int Tcl_opf_removeExp(ClientData clientData,
    Tcl_Interp* interp, int argc, TCL_Char** argv)
{
    // reset the input args
    OPS_ResetCommandLine(argc, 1, argv);
    
    if (OPF_removeObject() < 0) return TCL_ERROR;
    
    return TCL_OK;
}


int Tcl_opf_version(ClientData clientData,
    Tcl_Interp* interp, int argc, TCL_Char** argv)
{
    if (OPF_version() < 0) return TCL_ERROR;
    
    return TCL_OK;
}


// This is a package initialization procedure, which is called
// by Tcl when this package is to be added to an interpreter.
extern "C" DllExport int
OpenFrescoTcl(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, Domain *thedomain)
{
    int code;
    
    theInterp = interp;
    theDomain = thedomain;
    
    if (Tcl_InitStubs(interp, TCL_VERSION, 0) == NULL)  {
        return TCL_ERROR;
    }
    
    // add the package to list of available packages
    code = Tcl_PkgProvide(interp, "OpenFrescoTcl", OPF_VERSION);
    if (code != TCL_OK)  {
        return code;
    }
    
    // beginning of OpenFresco additions
    fprintf(stderr,"\n\n");
    fprintf(stderr,"\t OpenFresco -- Open Framework for Experimental Setup and Control\n");
    fprintf(stderr,"\t               Version %s %s              \n\n",OPF_VERSION,WIN_ARCH);
    fprintf(stderr,"\t Copyright (c) 2006 The Regents of the University of California \n");
    fprintf(stderr,"\t                       All Rights Reserved                      \n\n\n");
    
    Tcl_CreateCommand(interp, "expControlPoint", Tcl_opf_expControlPoint,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "expSignalFilter", Tcl_opf_expSignalFilter,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "expControl", Tcl_opf_expControl,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "expSetup", Tcl_opf_expSetup,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "expSite", Tcl_opf_expSite,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "expTangentStiff", Tcl_opf_expTangentStiff,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "expElement", Tcl_opf_expElement,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "expRecorder", Tcl_opf_expRecorder,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "recordExp", Tcl_opf_recordExp,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "startLabServer", Tcl_opf_startLabServer,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "setupLabServer", Tcl_opf_setupLabServer,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "stepLabServer", Tcl_opf_stepLabServer,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "stopLabServer", Tcl_opf_stopLabServer,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "startSimAppSiteServer", Tcl_opf_startSimAppSiteServer,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "startSimAppElemServer", Tcl_opf_startSimAppElemServer,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "analyzeInteractive", Tcl_ops_analyzeModelInteractive,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "wipeExp", Tcl_opf_wipeExp,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "removeExp", Tcl_opf_removeExp,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "packageVersion", Tcl_opf_version,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    return TCL_OK;
}
