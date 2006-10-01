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
// $Source$

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 09/06
// Revision: A
//
// Description: Defines the entry point for the DLL application and
// a function that can be called to set the global pointer variables in 
// the dll to be the same as those in the existing process address space.

#include <windows.h>
#include <tcl.h>
#include <OPS_Stream.h>
#include <Domain.h>
#include <TclModelBuilder.h>
#include <ExperimentalSite.h>
#include <ActorExpSite.h>

#define DllExport _declspec(dllexport)

Domain *theDomain;
TclModelBuilder *theTclBuilder;

extern ExperimentalSite *getExperimentalSite(int tag);

// experimental control commands
extern int TclExpControlCommand(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, TclModelBuilder *theTclBuilder,
    Domain *theDomain);

int openFresco_addExperimentalControl(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{ 
    return TclExpControlCommand(clientData, interp, argc, argv,
        theTclBuilder, theDomain);
}

// experimental setup commands
extern int TclExpSetupCommand(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, TclModelBuilder *theTclBuilder,
    Domain *theDomain);

int openFresco_addExperimentalSetup(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{ 
    return TclExpSetupCommand(clientData, interp, argc, argv,
        theTclBuilder, theDomain);
}

// experimental site commands
extern int TclExpSiteCommand(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, TclModelBuilder *theTclBuilder,
    Domain *theDomain);

int openFresco_addExperimentalSite(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{ 
    return TclExpSiteCommand(clientData, interp, argc, argv,
        theTclBuilder, theDomain);
}

// experimental element commands
extern int TclExpElementCommand(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, TclModelBuilder *theTclBuilder,
    Domain *theDomain);

int openFresco_addExperimentalElement(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{ 
    return TclExpElementCommand(clientData, interp, argc, argv,
        theTclBuilder, theDomain);
}

// start server command
int openFresco_startServer(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{ 
    if (argc != 2)  {
        opserr << "WARNING insufficient arguments\n"
            << "Want: startOpenFrescoServer siteTag\n";
        return TCL_ERROR;
    }
    int siteTag;
    if (Tcl_GetInt(interp, argv[1], &siteTag) != TCL_OK)  {
        opserr << "WARNING invalid server siteTag\n";
        return TCL_ERROR;
    }
    ActorExpSite *theSite =
        dynamic_cast <ActorExpSite*> (getExperimentalSite(siteTag));
    if (theSite != 0)  {
        // start server process
        opserr << "\nActorExpSite " << siteTag
            << " now running..." << endln;
        theSite->run();

    } else  {
        opserr << "WARNING actor experimental site not found\n";
        opserr << "unable to start expSite: " << siteTag << endln;
        return TCL_ERROR;
    }

    return TCL_OK;
}


// This is a package initialization procedure, which is called
// by Tcl when this package is to be added to an interpreter.
extern "C" DllExport int
OpenFresco(ClientData clientData, Tcl_Interp *interp, int argc,
           TCL_Char **argv, Domain *theD, TclModelBuilder *theB)
{
    int code;

    theDomain = theD;
    theTclBuilder = theB;

    if (Tcl_InitStubs(interp, TCL_VERSION, 1) == NULL) {
        return TCL_ERROR;
    }

    // add the package to list of available packages
    code = Tcl_PkgProvide(interp, "OpenFresco", "1.0");
    if (code != TCL_OK) {
        return code;
    }

    Tcl_CreateCommand(interp, "expControl", openFresco_addExperimentalControl,
        (ClientData)NULL, NULL);

    Tcl_CreateCommand(interp, "expSetup", openFresco_addExperimentalSetup,
        (ClientData)NULL, NULL);

    Tcl_CreateCommand(interp, "expSite", openFresco_addExperimentalSite,
        (ClientData)NULL, NULL);

    Tcl_CreateCommand(interp, "expElement", openFresco_addExperimentalElement,
        (ClientData)NULL, NULL);

    Tcl_CreateCommand(interp, "startOpenFrescoServer", openFresco_startServer,
        (ClientData)NULL, NULL);

    return TCL_OK;
}


#include <SimulationInformation.h>
SimulationInformation simulationInfo;

BOOL APIENTRY DllMain( HANDLE hModule, 
                      DWORD  ul_reason_for_call, 
                      LPVOID lpReserved
                      )
{
    return TRUE;
}

int  __cdecl
OpenSeesExit(ClientData clientData, Tcl_Interp *interp,
    int argc, const char **argv)
{
    /*
    if (simulationInfoOutputFilename != 0) {
    simulationInfo.end();
    FileStream simulationInfoOutputFile;
    simulationInfoOutputFile.setFile(simulationInfoOutputFilename);
    simulationInfoOutputFile.open();
    simulationInfoOutputFile << simulationInfo;
    simulationInfoOutputFile.close();
    }
    */
    Tcl_Exit(0);
    return 0;
}


extern "C" DllExport void
setGlobalPointers(OPS_Stream *theErrorStreamPtr)
{
    opserrPtr = theErrorStreamPtr;
}
