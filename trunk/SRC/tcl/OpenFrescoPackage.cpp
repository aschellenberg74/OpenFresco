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
extern int clearExperimentalCPs(Tcl_Interp *interp);

int openFresco_addExperimentalCP(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{ 
    return TclExpCPCommand(clientData, interp, argc, argv, theDomain);
}

// experimental signal filter commands
extern int TclExpSignalFilterCommand(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, Domain *theDomain);
extern int clearExperimentalSignalFilters(Tcl_Interp *interp);

int openFresco_addExperimentalSignalFilter(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{ 
    return TclExpSignalFilterCommand(clientData, interp, argc, argv, theDomain);
}

// experimental control commands
extern int TclExpControlCommand(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, Domain *theDomain);
extern int clearExperimentalControls(Tcl_Interp *interp);

int openFresco_addExperimentalControl(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{ 
    return TclExpControlCommand(clientData, interp, argc, argv, theDomain);
}

// experimental setup commands
extern int TclExpSetupCommand(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, Domain *theDomain);
extern int clearExperimentalSetups(Tcl_Interp *interp);

int openFresco_addExperimentalSetup(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{ 
    return TclExpSetupCommand(clientData, interp, argc, argv, theDomain);
}

// experimental site commands
extern int TclExpSiteCommand(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, Domain *theDomain);
extern int clearExperimentalSites(Tcl_Interp *interp);

int openFresco_addExperimentalSite(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{ 
    return TclExpSiteCommand(clientData, interp, argc, argv, theDomain);
}

// experimental tangent stiffness commands
extern int TclExpTangentStiffCommand(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, Domain *theDomain);
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
    /*if (theDatabase != 0)  {
        delete theDatabase;
        theDatabase = 0;
    }*/
    
    if (theDomain != 0)
        theDomain->clearAll();
    
    clearExperimentalCPs(interp);
    clearExperimentalSignalFilters(interp);
    clearExperimentalControls(interp);
    clearExperimentalSetups(interp);
    clearExperimentalSites(interp);
    clearExperimentalTangentStiffs(interp);

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
    
    if (Tcl_InitStubs(interp, TCL_VERSION, 1) == NULL) {
        return TCL_ERROR;
    }
    
    // add the package to list of available packages
    code = Tcl_PkgProvide(interp, "OpenFresco", OPF_VERSION);
    if (code != TCL_OK) {
        return code;
    }
    
    // beginning of OpenFresco additions
    fprintf(stderr,"\n\n");
    fprintf(stderr,"\t OpenFresco -- Open Framework for Experimental Setup and Control\n");
    fprintf(stderr,"\t                 Version %s %s                  \n\n",OPF_VERSION,WIN_ARCH);
    fprintf(stderr,"\t Copyright (c) 2006 The Regents of the University of California \n");
    fprintf(stderr,"\t                       All Rights Reserved                      \n\n\n");    
    
    Tcl_CreateCommand(interp, "expControlPoint", openFresco_addExperimentalCP,
        (ClientData)NULL, NULL);
    
    Tcl_CreateCommand(interp, "expSignalFilter", openFresco_addExperimentalSignalFilter,
        (ClientData)NULL, NULL);
    
    Tcl_CreateCommand(interp, "expControl", openFresco_addExperimentalControl,
        (ClientData)NULL, NULL);
    
    Tcl_CreateCommand(interp, "expSetup", openFresco_addExperimentalSetup,
        (ClientData)NULL, NULL);
    
    Tcl_CreateCommand(interp, "expSite", openFresco_addExperimentalSite,
        (ClientData)NULL, NULL);
    
    Tcl_CreateCommand(interp, "expTangentStiff", openFresco_addExperimentalTangentStiff,
        (ClientData)NULL, NULL);
    
    Tcl_CreateCommand(interp, "expElement", openFresco_addExperimentalElement,
        (ClientData)NULL, NULL);
    
    Tcl_CreateCommand(interp, "expRecorder", openFresco_addExperimentalRecorder,
        (ClientData)NULL, NULL);
    
    Tcl_CreateCommand(interp, "startLabServer", openFresco_startLabServer,
        (ClientData)NULL, NULL);
    
    Tcl_CreateCommand(interp, "setupLabServer", openFresco_setupLabServer,
        (ClientData)NULL, NULL);
    
    Tcl_CreateCommand(interp, "stepLabServer", openFresco_stepLabServer,
        (ClientData)NULL, NULL);
    
    Tcl_CreateCommand(interp, "stopLabServer", openFresco_stopLabServer,
        (ClientData)NULL, NULL);
    
    Tcl_CreateCommand(interp, "startSimAppSiteServer", openFresco_startSimAppSiteServer,
        (ClientData)NULL, NULL);
    
    Tcl_CreateCommand(interp, "startSimAppElemServer", openFresco_startSimAppElemServer,
        (ClientData)NULL, NULL);
    
    Tcl_CreateCommand(interp, "wipeExp", openFresco_wipeModel,
        (ClientData)NULL, NULL); 
    
    return TCL_OK;
}
