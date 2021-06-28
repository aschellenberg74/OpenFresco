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

Tcl_Interp *theInterp = 0;
Domain *theDomain = 0;
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
    
    Tcl_CreateCommand(interp, "wipeExp", openFresco_wipeExp,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "removeExp", openFresco_removeObject,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "packageVersion", openFresco_version,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    return TCL_OK;
}
