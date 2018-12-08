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
#include <Domain.h>
#include <StandardStream.h>
#include <FrescoGlobals.h>

#ifdef _WIN32
#define DllExport _declspec(dllexport)
#else
#define DllExport
extern Domain *ops_TheActiveDomain;
double ops_Dt = 0.0;
StandardStream sserr;
OPS_Stream *opserrPtr = &sserr;
#endif

Domain *theDomain = 0;

// experimental control point commands
extern int TclExpCPCommand(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, Domain *theDomain);
extern int TclRemoveExpCP(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv);
extern int clearExperimentalCPs(Tcl_Interp *interp);

int openFresco_addExperimentalCP(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{ 
    return TclExpCPCommand(clientData, interp, argc, argv, theDomain);
}

// experimental signal filter commands
extern int TclExpSignalFilterCommand(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, Domain *theDomain);
extern int TclRemoveExpSignalFilter(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv);
extern int clearExperimentalSignalFilters(Tcl_Interp *interp);

int openFresco_addExperimentalSignalFilter(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{ 
    return TclExpSignalFilterCommand(clientData, interp, argc, argv, theDomain);
}

// experimental control commands
extern int TclExpControlCommand(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, Domain *theDomain);
extern int TclRemoveExpControl(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv);
extern int clearExperimentalControls(Tcl_Interp *interp);

int openFresco_addExperimentalControl(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{ 
    return TclExpControlCommand(clientData, interp, argc, argv, theDomain);
}

// experimental setup commands
extern int TclExpSetupCommand(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, Domain *theDomain);
extern int TclRemoveExpSetup(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv);
extern int clearExperimentalSetups(Tcl_Interp *interp);

int openFresco_addExperimentalSetup(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{ 
    return TclExpSetupCommand(clientData, interp, argc, argv, theDomain);
}

// experimental site commands
extern int TclExpSiteCommand(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, Domain *theDomain);
extern int TclRemoveExpSite(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv);
extern int clearExperimentalSites(Tcl_Interp *interp);

int openFresco_addExperimentalSite(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{ 
    return TclExpSiteCommand(clientData, interp, argc, argv, theDomain);
}

// experimental tangent stiffness commands
extern int TclExpTangentStiffCommand(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, Domain *theDomain);
extern int TclRemoveExpTangentStiff(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv);
extern int clearExperimentalTangentStiffs(Tcl_Interp *interp);

int openFresco_addExperimentalTangentStiff(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{ 
    return TclExpTangentStiffCommand(clientData, interp, argc, argv, theDomain);
}

// experimental element commands
extern int TclExpElementCommand(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, Domain *theDomain);

int openFresco_addExperimentalElement(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    return TclExpElementCommand(clientData, interp, argc, argv, theDomain);
}

// experimental recorder commands
extern int TclAddExpRecorder(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, Domain *theDomain);
extern int TclRemoveExpRecorder(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv);
extern int TclExpRecord(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv);

int openFresco_addExperimentalRecorder(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    return TclAddExpRecorder(clientData, interp, argc, argv, theDomain);
}

// start laboratory server command
extern int TclStartLabServer(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv);

int openFresco_startLabServer(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    return TclStartLabServer(clientData, interp, argc, argv);
}

// setup laboratory server command
extern int TclSetupLabServer(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv);

int openFresco_setupLabServer(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    return TclSetupLabServer(clientData, interp, argc, argv);
}

// step laboratory server command
extern int TclStepLabServer(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv);

int openFresco_stepLabServer(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    return TclStepLabServer(clientData, interp, argc, argv);
}

// stop laboratory server command
extern int TclStopLabServer(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv);

int openFresco_stopLabServer(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    return TclStopLabServer(clientData, interp, argc, argv);
}

// start simulation application site server command
extern int TclStartSimAppSiteServer(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv);

int openFresco_startSimAppSiteServer(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    return TclStartSimAppSiteServer(clientData, interp, argc, argv);
}

// start simulation application element server command
extern int TclStartSimAppElemServer(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, Domain *theDomain);

int openFresco_startSimAppElemServer(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    return TclStartSimAppElemServer(clientData, interp, argc, argv, theDomain);
}


// wipe entire model command
int openFresco_wipeModel(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    // OpenSees will clean up theDomain when wipe is called
    //if (theDomain != 0)
    //    theDomain->clearAll();
    
    clearExperimentalCPs(interp);
    clearExperimentalSignalFilters(interp);
    clearExperimentalControls(interp);
    clearExperimentalSetups(interp);
    clearExperimentalSites(interp);
    clearExperimentalTangentStiffs(interp);
    
    return TCL_OK;
}


int openFresco_removeComp(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    // make sure there is a minimum number of arguments
    if (argc < 2)  {
        opserr << "WARNING insufficient number of removeExp component arguments\n";
        opserr << "Want: removeExp type <specific args>\n";
        return TCL_ERROR;
    }
    
    if (strcmp(argv[1], "controlPoint") == 0 || strcmp(argv[1], "controlPoints") == 0)  {
        return TclRemoveExpCP(clientData, interp, argc, argv);
    }
    
    else if (strcmp(argv[1], "signalFilter") == 0 || strcmp(argv[1], "signalFilters") == 0)  {
        return TclRemoveExpSignalFilter(clientData, interp, argc, argv);
    }
    
    else if (strcmp(argv[1], "control") == 0 || strcmp(argv[1], "controls") == 0)  {
        return TclRemoveExpControl(clientData, interp, argc, argv);
    }
    
    else if (strcmp(argv[1], "setup") == 0 || strcmp(argv[1], "setups") == 0)  {
        return TclRemoveExpSetup(clientData, interp, argc, argv);
    }
    
    else if (strcmp(argv[1], "site") == 0 || strcmp(argv[1], "sites") == 0)  {
        return TclRemoveExpSite(clientData, interp, argc, argv);
    }
    
    else if (strcmp(argv[1], "tangentStiff") == 0 || strcmp(argv[1], "tangentStiffs") == 0)  {
        return TclRemoveExpTangentStiff(clientData, interp, argc, argv);
    }
    
    else if (strcmp(argv[1],"recorder") == 0 || strcmp(argv[1],"recorders") == 0)  {
        return TclRemoveExpRecorder(clientData, interp, argc, argv);
    }
    
    else {
        // experimental control type not recognized
        opserr << "WARNING unknown removeExp type: "
            << argv[1] << ": check the manual\n";
        return TCL_ERROR;
    }
    
    return TCL_OK;
}


int openFresco_record(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    return TclExpRecord(clientData, interp, argc, argv);
}


int openFresco_version(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    char buffer[20];
    
    sprintf(buffer, "%s", OPF_VERSION);
    Tcl_SetResult(interp, buffer, TCL_VOLATILE);
    
    return TCL_OK;
}


// This is a package initialization procedure, which is called
// by Tcl when this package is to be added to an interpreter.
extern "C" DllExport int
OpenFresco(ClientData clientData, Tcl_Interp *interp, int argc,
           TCL_Char **argv, Domain *thedomain)
{
    int code;
    
    theDomain = thedomain;
    
    if (Tcl_InitStubs(interp, TCL_VERSION, 1) == NULL)  {
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
    
    Tcl_CreateCommand(interp, "wipeExp", openFresco_wipeModel,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "removeExp", openFresco_removeComp,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "recordExp", openFresco_record,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "packageVersion", openFresco_version,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    return TCL_OK;
}
