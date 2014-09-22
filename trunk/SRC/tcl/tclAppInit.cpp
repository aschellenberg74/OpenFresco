/* 
* tclAppInit.cpp --
*
*	Provides a default version of the main program and Tcl_AppInit
*	procedure for Tcl applications (without Tk).
*
* Copyright (c) 1994-1997 Sun Microsystems, Inc.
* Copyright (c) 1998-1999 by Scriptics Corporation.
*
* See the file "license.terms" for information on usage and
* redistribution of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

/*                            MODIFIED FOR                            */

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

// Modified: Frank McKenna & Andreas Schellenberg
// Created: 10/06
// Revision: A

extern "C" {
#include <tcl.h>
//#include <tk.h>
}

/*
* The following variable is a special hack that is needed in order for
* Sun shared libraries to be used for Tcl.
*/

#ifdef _UNIX
//extern "C" int matherr();
//int *tclDummyMathPtr = (int *) matherr;
#endif /* UNIX */

#ifdef TCL_TEST
extern "C" {
#include "tclInt.h"
}

extern int		Procbodytest_Init _ANSI_ARGS_((Tcl_Interp *interp));
extern int		Procbodytest_SafeInit _ANSI_ARGS_((Tcl_Interp *interp));
extern int		TclObjTest_Init _ANSI_ARGS_((Tcl_Interp *interp));
extern int		Tcltest_Init _ANSI_ARGS_((Tcl_Interp *interp));
#ifdef TCL_THREADS
extern int		TclThread_Init _ANSI_ARGS_((Tcl_Interp *interp));
#endif
#endif /* TCL_TEST */

#ifdef TCL_XT_TEST
extern void		XtToolkitInitialize _ANSI_ARGS_((void));
extern int		Tclxttest_Init _ANSI_ARGS_((Tcl_Interp *interp));
#endif /* TCL_XT_TEST */

#include <Domain.h>
#include <TclModelBuilder.h>
#include <StandardStream.h>
#include <FE_Datastore.h>
#include <SimulationInformation.h>
#include <Node.h>
#include <Matrix.h>
#include <CrdTransf.h>
#include <TCP_Socket.h>
#include <TCP_SocketSSL.h>
#include <UDP_Socket.h>

#include <ExperimentalElement.h>
#include <ExperimentalSite.h>
#include <ActorExpSite.h>

Domain *theDomain = 0;
TclModelBuilder *theModelBuilder = 0;
FE_Datastore *theDatabase = 0;
SimulationInformation simulationInfo;
SimulationInformation *theSimulationInfo = 0;
StandardStream sserr;
OPS_Stream *opserrPtr = &sserr;
//double ops_Dt = 0.0;

#ifndef _WIN32
extern Domain *ops_TheActiveDomain;
#endif

/*
*----------------------------------------------------------------------
*
* main --
*
*	This is the main program for the application.
*
* Results:
*	None: Tcl_Main never returns here, so this procedure never
*	returns either.
*
* Side effects:
*	Whatever the application does.
*
*----------------------------------------------------------------------
*/
extern void tclMain(int argc, char **argv, Tcl_AppInitProc *appInitProc);

int main(int argc, char **argv)
{
    /*
    * The following #if block allows you to change the AppInit
    * function by using a #define of TCL_LOCAL_APPINIT instead
    * of rewriting this entire file.  The #if checks for that
    * #define and uses Tcl_AppInit if it doesn't exist.
    */
    
#ifndef TCL_LOCAL_APPINIT
#define TCL_LOCAL_APPINIT Tcl_AppInit    
#endif
    
    /* fmk - comment out the following block to get to compile 
    extern "C" int TCL_LOCAL_APPINIT _ANSI_ARGS_((Tcl_Interp *interp));
    fmk - end commented block */

    /*
    * The following #if block allows you to change how Tcl finds the startup
    * script, prime the library or encoding paths, fiddle with the argv,
    * etc., without needing to rewrite Tcl_Main()
    */
    
#ifdef TCL_LOCAL_MAIN_HOOK
    extern int TCL_LOCAL_MAIN_HOOK _ANSI_ARGS_((int *argc, char ***argv));
#endif
    
#ifdef TCL_XT_TEST
    XtToolkitInitialize();
#endif
    
#ifdef TCL_LOCAL_MAIN_HOOK
    TCL_LOCAL_MAIN_HOOK(&argc, &argv);
#endif
    
    tclMain(argc, argv, TCL_LOCAL_APPINIT);
    
    return 0;   /* Needed only to prevent compiler warning. */
}


extern ExperimentalSite *getExperimentalSite(int tag);

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

/* OpenSees recorder commands
extern int TclAddRecorder(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, Domain &theDomain);

int addRecorder(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    return TclAddRecorder(clientData, interp, argc, argv, *theDomain);
}*/


// wipe entire model command
int openFresco_wipeModel(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    if (theDatabase != 0)  {
        delete theDatabase;
        theDatabase = 0;
    }
    
    if (theDomain != 0)
        theDomain->clearAll();
    
    clearExperimentalCPs(interp);
    clearExperimentalSignalFilters(interp);
    clearExperimentalControls(interp);
    clearExperimentalSetups(interp);
    clearExperimentalSites(interp);
    
    return TCL_OK;  
}


extern int OPS_ResetInput(ClientData clientData, Tcl_Interp *interp, int cArg,
    int mArg, TCL_Char **argv, Domain *theDomain, TclModelBuilder *theBuilder);

// model builder command
int specifyModelBuilder(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv)
{
    // make sure at least one other argument contains model builder type
    if (argc < 2)  {
        opserr << "WARNING need to specify a model type, valid types:\n";
        opserr << "\tBasicBuilder\n";
        return TCL_ERROR;
    }    
    
    // invoke the destructor on the old builder
    if (theModelBuilder != 0)  {
        delete theModelBuilder;
        theModelBuilder = 0;
    }
    
    // check argv[1] for type of ModelBuilder and create the object 
    if (strcmp(argv[1],"basic") == 0 ||
        strcmp(argv[1],"basicBuilder") == 0 ||
        strcmp(argv[1],"BasicBuilder") == 0)  {
        int ndm = 0;
        int ndf = 0;
        
        if (argc < 4)  {
            opserr << "WARNING incorrect number of command arguments\n";
            opserr << "model modelBuilderType -ndm ndm? <-ndf ndf?>\n";
            return TCL_ERROR;
        }
        
        int argPos = 2;
        while (argPos < argc)  {
            if (strcmp(argv[argPos],"-ndm") == 0 ||
                strcmp(argv[argPos],"-NDM") == 0)  {	
                argPos++;
                if (argPos < argc)  {
                    if (Tcl_GetInt(interp, argv[argPos], &ndm) != TCL_OK)  {
                        opserr << "WARNING error reading ndm: " << argv[argPos];
                        opserr << "\nmodel modelBuilderType -ndm ndm? <-ndf ndf?>\n";
                        return TCL_ERROR;
                    }	  
                    argPos++;
                }
            }
            else if (strcmp(argv[argPos],"-ndf") == 0 ||
                strcmp(argv[argPos],"-NDF") == 0)  {	
                argPos++;
                if (argPos < argc)  {
                    if (Tcl_GetInt(interp, argv[argPos], &ndf) != TCL_OK)  {
                        opserr << "WARNING error reading ndf: " << argv[argPos];
                        opserr << "\nmodel modelBuilderType -ndm ndm? <-ndf ndf?>\n";
                        return TCL_ERROR;
                    }	  
                    argPos++;
                }
            }
            else
                argPos++;
        }
        
        // check that ndm was specified
        if (ndm == 0)  {
            opserr << "WARNING need to specify ndm\n";
            opserr << "model modelBuilderType -ndm ndm? <-ndf ndf?>\n";
            return TCL_ERROR;
        }
        
        // check for ndf, if not assume one
        if (ndf == 0)  {
            if (ndm == 1) 
                ndf = 1;
            else if (ndm == 2)
                ndf = 3;
            else if (ndm == 3)
                ndf = 6;
            else {
                opserr << "WARNING specified ndm, " << ndm << ", will not work\n";
                opserr << "with any elements in BasicBuilder\n";
                return TCL_ERROR;
            }
        }
        
        // create the model builder
        theModelBuilder = new TclModelBuilder(*theDomain, interp, ndm, ndf);
        if (theModelBuilder == 0)  {
            opserr << "WARNING ran out of memory in creating BasicBuilder model\n";
            return TCL_ERROR;
        }

        // set pointers
        OPS_ResetInput(clientData, interp, 0, argc, argv, theDomain, theModelBuilder);
    }
    else {
        opserr << "WARNING model builder type " << argv[1]
            << " not supported\n";
        return TCL_ERROR;
    }
    
    return TCL_OK;
}


// Rayleigh damping command
int rayleighDamping(ClientData clientData, Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    if (argc < 5)  {
        opserr << "WARNING rayleigh alphaM betaKt betaK0 betaKc - not enough arguments to command\n";
        return TCL_ERROR;
    }
    
    double alphaM, betaKt, betaK0, betaKc;
    
    if (Tcl_GetDouble(interp, argv[1], &alphaM) != TCL_OK)  {
        opserr << "WARNING rayleigh alphaM betaKt betaK0 betaKc - could not read alphaM \n";
        return TCL_ERROR;
    }
    if (Tcl_GetDouble(interp, argv[2], &betaKt) != TCL_OK)  {
        opserr << "WARNING rayleigh alphaM betaKt betaK0 betaKc - could not read betaKt \n";
        return TCL_ERROR;
    }
    if (Tcl_GetDouble(interp, argv[3], &betaK0) != TCL_OK)  {
        opserr << "WARNING rayleigh alphaM betaKt betaK0 betaKc - could not read betaK0 \n";
        return TCL_ERROR;
    }
    if (Tcl_GetDouble(interp, argv[4], &betaKc) != TCL_OK)  {
        opserr << "WARNING rayleigh alphaM betaKt betaK0 betaKc - could not read betaKc \n";
        return TCL_ERROR;
    }
    
    theDomain->setRayleighDampingFactors(alphaM, betaKt, betaK0, betaKc);
    
    return TCL_OK;
}


// element Rayleigh damping command
int setElementRayleighDampingFactors(ClientData clientData, Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    if (argc < 6)  {
        opserr << "WARNING setElementRayleighDampingFactors eleTag alphaM betaKt betaK0 betaKc - not enough arguments to command\n";
        return TCL_ERROR;
    }
    
    int eleTag;
    double alphaM, betaKt, betaK0, betaKc;
    
    if (Tcl_GetInt(interp, argv[1], &eleTag) != TCL_OK)  {
        opserr << "WARNING setElementRayleighDampingFactors eleTag alphaM betaKt betaK0 betaKc - could not read eleTag \n";
        return TCL_ERROR;
    }
    if (Tcl_GetDouble(interp, argv[2], &alphaM) != TCL_OK)  {
        opserr << "WARNING setElementRayleighDampingFactors eleTag alphaM betaKt betaK0 betaKc - could not read alphaM \n";
        return TCL_ERROR;
    }
    if (Tcl_GetDouble(interp, argv[3], &betaKt) != TCL_OK)  {
        opserr << "WARNING setElementRayleighDampingFactors eleTag alphaM betaKt betaK0 betaKc - could not read betaKt \n";
        return TCL_ERROR;
    }
    if (Tcl_GetDouble(interp, argv[4], &betaK0) != TCL_OK)  {
        opserr << "WARNING setElementRayleighDampingFactors eleTag alphaM betaKt betaK0 betaKc - could not read betaK0 \n";
        return TCL_ERROR;
    }
    if (Tcl_GetDouble(interp, argv[5], &betaKc) != TCL_OK)  {
        opserr << "WARNING setElementRayleighDampingFactors eleTag alphaM betaKt betaK0 betaKc - could not read betaKc \n";
        return TCL_ERROR;
    }
    
    Element *theEle = theDomain->getElement(eleTag);
    theEle->setRayleighDampingFactors(alphaM, betaKt, betaK0, betaKc);
    
    return TCL_OK;
}


int getLibraryFunction(const char *libName, const char *funcName,
    void **libHandle, void **funcHandle)
{
    opserr << "getLibraryFunction() called - FATAL!\n";
    return -1;
}


const char *getInterpPWD(Tcl_Interp *interp)
{
    static char *pwd = 0;

    if (pwd != 0)
        delete [] pwd;

    Tcl_DString buf;
    const char *objPWD = Tcl_GetCwd(interp, &buf);

    pwd = new char[strlen(objPWD)+1];
    strcpy(pwd, objPWD);

    Tcl_DStringFree(&buf);

    return pwd;
}


/*
*----------------------------------------------------------------------
*
* Tcl_AppInit --
*
*	This procedure performs application-specific initialization.
*	Most applications, especially those that incorporate additional
*	packages, will have their own version of this procedure.
*
* Results:
*	Returns a standard Tcl completion code, and leaves an error
*	message in the interp's result if an error occurs.
*
* Side effects:
*	Depends on the startup script.
*
*----------------------------------------------------------------------
*/
int Tcl_AppInit(Tcl_Interp *interp)
{
    if (Tcl_Init(interp) == TCL_ERROR)  {
        return TCL_ERROR;
    }
    
#ifdef TCL_TEST
#ifdef TCL_XT_TEST
    if (Tclxttest_Init(interp) == TCL_ERROR)  {
        return TCL_ERROR;
    }
#endif
    if (Tcltest_Init(interp) == TCL_ERROR)  {
        return TCL_ERROR;
    }
    Tcl_StaticPackage(interp, "Tcltest", Tcltest_Init,
        (Tcl_PackageInitProc *) NULL);
    if (TclObjTest_Init(interp) == TCL_ERROR)  {
        return TCL_ERROR;
    }
#ifdef TCL_THREADS
    if (TclThread_Init(interp) == TCL_ERROR)  {
        return TCL_ERROR;
    }
#endif
    if (Procbodytest_Init(interp) == TCL_ERROR)  {
        return TCL_ERROR;
    }
    Tcl_StaticPackage(interp, "procbodytest", Procbodytest_Init,
        Procbodytest_SafeInit);
#endif /* TCL_TEST */
    
    /*
    * Call the init procedures for included packages.  Each call should
    * look like this:
    *
    * if (Mod_Init(interp) == TCL_ERROR)  {
    *     return TCL_ERROR;
    * }
    *
    * where "Mod" is the name of the module.
    */

    /*
    * Call Tcl_CreateCommand for application-specific commands, if
    * they weren't already created by the init procedures called above.
    */
    
    theDomain = new Domain();
    theSimulationInfo = &simulationInfo;
    
    // OpenFresco commands
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
    
    Tcl_CreateCommand(interp, "wipe", openFresco_wipeModel,
        (ClientData)NULL, NULL); 
    
    Tcl_CreateCommand(interp, "model", specifyModelBuilder,
        (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
    
    Tcl_CreateCommand(interp, "rayleigh", rayleighDamping,
        (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);       
    
    Tcl_CreateCommand(interp, "setElementRayleighDampingFactors", setElementRayleighDampingFactors,
        (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);       

    /* OpenSees commands
    Tcl_CreateCommand(interp, "recorder", addRecorder,
	    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);*/
    
    /*
    * Specify a user-specific startup file to invoke if the application
    * is run interactively.  Typically the startup file is "~/.apprc"
    * where "app" is the name of the application.  If this line is deleted
    * then no user-specific startup file will be run under any conditions.
    */
    Tcl_SetVar(interp, "tcl_rcFileName", "~/.tclshrc", TCL_GLOBAL_ONLY);
    return TCL_OK;
}
