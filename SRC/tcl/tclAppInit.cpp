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

extern void OPS_clearAllUniaxialMaterial();
extern void OPS_clearAllNDMaterial();
extern void OPS_clearAllSectionForceDeformation();

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
extern int TclRemoveExpCP(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv);
extern void OPF_ClearExperimentalCPs();

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
extern void OPF_ClearExperimentalSignalFilters();

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
extern void OPF_ClearExperimentalControls();

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
extern void OPF_ClearExperimentalSetups();

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
extern void OPF_ClearExperimentalSites();

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
extern void OPF_ClearExperimentalTangentStiffs();

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

/* OpenSees recorder commands
extern int TclAddRecorder(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, Domain &theDomain);

int addRecorder(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    return TclAddRecorder(clientData, interp, argc, argv, *theDomain);
}*/


extern int OPS_ResetInput(ClientData clientData, Tcl_Interp *interp, int cArg,
    int mArg, TCL_Char **argv, Domain *theDomain, TclModelBuilder *theBuilder);


const char *getInterpPWD(Tcl_Interp *interp)
{
    static char *pwd = 0;
    
    if (pwd != 0)
        delete[] pwd;
    
    Tcl_DString buf;
    const char *objPWD = Tcl_GetCwd(interp, &buf);
    
    pwd = new char[strlen(objPWD) + 1];
    strcpy(pwd, objPWD);
    
    Tcl_DStringFree(&buf);
    
    return pwd;
}


int getLibraryFunction(const char *libName, const char *funcName,
    void **libHandle, void **funcHandle)
{
    opserr << "getLibraryFunction() called - FATAL!\n";
    
    return -1;
}


// wipe entire model command
int openFresco_wipeModel(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    OPF_ClearExperimentalCPs();
    OPF_ClearExperimentalSignalFilters();
    OPF_ClearExperimentalControls();
    OPF_ClearExperimentalSetups();
    OPF_ClearExperimentalSites();
    OPF_ClearExperimentalTangentStiffs();
    
    OPS_clearAllUniaxialMaterial();
    OPS_clearAllNDMaterial();
    OPS_clearAllSectionForceDeformation();
    
    if (theDatabase != 0)  {
        delete theDatabase;
        theDatabase = 0;
    }
    
    if (theDomain != 0)
        theDomain->clearAll();
    
    ops_Dt = 0.0;
    
    return TCL_OK;
}


int openFresco_removeComp(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    // make sure there is a minimum number of arguments
    if (argc < 2)  {
        opserr << "WARNING insufficient number of remove component arguments\n";
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
    
    else if (strcmp(argv[1], "recorder") == 0 || strcmp(argv[1], "recorders") == 0)  {
        return TclRemoveExpRecorder(clientData, interp, argc, argv);
    }
    
    return TCL_OK;
}


int openFresco_record(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    return TclExpRecord(clientData, interp, argc, argv);
}


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


int openFresco_logFile(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    if (argc < 2)  {
        opserr << "WARNING logFile fileName - no filename supplied\n";
        return TCL_ERROR;
    }
    openMode mode = OVERWRITE;
    bool echo = true;
    
    int cArg = 2;
    while (cArg < argc)  {
        if (strcmp(argv[cArg], "-append") == 0)
            mode = APPEND;
        if (strcmp(argv[cArg], "-noEcho") == 0)
            echo = false;
        cArg++;
    }
    
    if (opserr.setFile(argv[1], mode, echo) < 0)
        opserr << "WARNING logFile " << argv[1] << " failed to set the file\n";
    
    const char *pwd = getInterpPWD(interp);
    simulationInfo.addOutputFile(argv[1], pwd);
    
    return TCL_OK;
}


int openFresco_defaultUnits(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    if (argc < 9)  {
        opserr << "WARNING missing a unit type\n";
        opserr << "Want: defaultUnits -force type -length type -time type -temperature type\n";
        return TCL_ERROR;
    }
    
    const char *force = 0;
    const char *length = 0;
    const char *time = 0;
    const char *temperature = 0;
    
    int count = 1;
    while (count < 9)  {
        if ((strcmp(argv[count], "-force") == 0) || (strcmp(argv[count], "-Force") == 0)
            || (strcmp(argv[count], "-FORCE") == 0))  {
            force = argv[count + 1];
        }
        else if ((strcmp(argv[count], "-length") == 0) || (strcmp(argv[count], "-Length") == 0)
            || (strcmp(argv[count], "-LENGTH") == 0))  {
            length = argv[count + 1];
        }
        else if ((strcmp(argv[count], "-time") == 0) || (strcmp(argv[count], "-Time") == 0)
            || (strcmp(argv[count], "-TIME") == 0))  {
            time = argv[count + 1];
        }
        else if ((strcmp(argv[count], "-temperature") == 0) || (strcmp(argv[count], "-Temperature") == 0)
            || (strcmp(argv[count], "-TEMPERATURE") == 0) || (strcmp(argv[count], "-temp") == 0)
            || (strcmp(argv[count], "-Temp") == 0) || (strcmp(argv[count], "-TEMP") == 0))  {
            temperature = argv[count + 1];
        }
        else  {
            opserr << "WARNING unrecognized unit: " << argv[count] << endln;
            opserr << "Want: defaultUnits -force type -length type -time type -temperature type\n";
            return TCL_ERROR;
        }
        count += 2;
    }
    
    if (length == 0 || force == 0 || time == 0 || temperature == 0)  {
        opserr << "WARNING missing a unit type\n";
        opserr << "Want: defaultUnits -force type -length type -time type -temperature type\n";
        return TCL_ERROR;
    }
    
    double lb, kip, n, kn, mn, kgf, tonf;
    double in, ft, mm, cm, m;
    double sec, msec;
    double F, C;
    
    if ((strcmp(force, "lb") == 0) || (strcmp(force, "lbs") == 0))  {
        lb = 1.0;
    }
    else if ((strcmp(force, "kip") == 0) || (strcmp(force, "kips") == 0))  {
        lb = 0.001;
    }
    else if ((strcmp(force, "N") == 0))  {
        lb = 4.4482216152605;
    }
    else if ((strcmp(force, "kN") == 0) || (strcmp(force, "KN") == 0) || (strcmp(force, "kn") == 0))  {
        lb = 0.0044482216152605;
    }
    else if ((strcmp(force, "mN") == 0) || (strcmp(force, "MN") == 0) || (strcmp(force, "mn") == 0))  {
        lb = 0.0000044482216152605;
    }
    else if ((strcmp(force, "kgf") == 0))  {
        lb = 9.80665*4.4482216152605;
    }
    else if ((strcmp(force, "tonf") == 0))  {
        lb = 9.80665 / 1000.0*4.4482216152605;
    }
    else  {
        lb = 1.0;
        opserr << "WARNING unknown force type\n";
        opserr << "Valid options: lb, kip, N, kN, MN, kgf, tonf\n";
        return TCL_ERROR;
    }
    
    if ((strcmp(length, "in") == 0) || (strcmp(length, "inch") == 0))  {
        in = 1.0;
    }
    else if ((strcmp(length, "ft") == 0) || (strcmp(length, "feet") == 0))  {
        in = 1.0 / 12.0;
    }
    else if ((strcmp(length, "mm") == 0))  {
        in = 25.4;
    }
    else if ((strcmp(length, "cm") == 0))  {
        in = 2.54;
    }
    else if ((strcmp(length, "m") == 0))  {
        in = 0.0254;
    }
    else  {
        in = 1.0;
        opserr << "WARNING unknown length type\n";
        opserr << "Valid options: in, ft, mm, cm, m\n";
        return TCL_ERROR;
    }
    
    if ((strcmp(time, "sec") == 0) || (strcmp(time, "Sec") == 0))  {
        sec = 1.0;
    }
    else if ((strcmp(time, "msec") == 0) || (strcmp(time, "mSec") == 0))  {
        sec = 1000.0;
    }
    else  {
        sec = 1.0;
        opserr << "WARNING unknown time type\n";
        opserr << "Valid options: sec, msec\n";
        return TCL_ERROR;
    }
    
    if ((strcmp(temperature, "F") == 0) || (strcmp(temperature, "degF") == 0))  {
        F = 1.0;
    }
    else if ((strcmp(temperature, "C") == 0) || (strcmp(temperature, "degC") == 0))  {
        F = 9.0 / 5.0 + 32.0;
    }
    else  {
        F = 1.0;
        opserr << "WARNING unknown temperature type\n";
        opserr << "Valid options: F, C\n";
        return TCL_ERROR;
    }
    
    kip = lb / 0.001;
    n = lb / 4.4482216152605;
    kn = lb / 0.0044482216152605;
    mn = lb / 0.0000044482216152605;
    kgf = lb / (9.80665*4.4482216152605);
    tonf = lb / (9.80665 / 1000.0*4.4482216152605);
    
    ft = in * 12.0;
    mm = in / 25.44;
    cm = in / 2.54;
    m = in / 0.0254;
    
    msec = sec * 0.001;
    
    C = (F - 32.0)*5.0 / 9.0;
    
    char buffer[50];
    
    sprintf(buffer, "set lb %.18e", lb);   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set lbf %.18e", lb);   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set kip %.18e", kip);   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set N %.18e", n);   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set kN %.18e", kn);   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set Newton %.18e", n);   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set kNewton %.18e", kn);   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set MN %.18e", mn);   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set kgf %.18e", kgf);   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set tonf %.18e", tonf);   Tcl_Eval(interp, buffer);
    
    sprintf(buffer, "set in %.18e", in);   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set inch %.18e", in);   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set ft %.18e", ft);   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set mm %.18e", mm);   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set cm %.18e", cm);   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set m  %.18e", m);   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set meter  %.18e", m);   Tcl_Eval(interp, buffer);
    
    sprintf(buffer, "set sec %.18e", sec);   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set msec %.18e", msec);   Tcl_Eval(interp, buffer);
    
    sprintf(buffer, "set F %.18e", F);   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set degF %.18e", F);   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set C %.18e", C);   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set degC %.18e", C);   Tcl_Eval(interp, buffer);
    
    double g = 32.174049*ft / (sec*sec);
    sprintf(buffer, "set g %.18e", g);   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set kg %.18e", n*sec*sec / m);   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set Mg %.18e", 1e3*n*sec*sec / m);   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set slug %.18e", lb*sec*sec / ft);   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set Pa %.18e", n / (m*m));   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set kPa %.18e", 1e3*n / (m*m));   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set MPa %.18e", 1e6*n / (m*m));   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set psi %.18e", lb / (in*in));   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set ksi %.18e", kip / (in*in));   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set psf %.18e", lb / (ft*ft));   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set ksf %.18e", kip / (ft*ft));   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set pcf %.18e", lb / (ft*ft*ft));   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set in2 %.18e", in*in);   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set ft2 %.18e", ft*ft);   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set mm2 %.18e", mm*mm);   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set cm2 %.18e", cm*cm);   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set m2 %.18e", m*m);   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set in4 %.18e", in*in*in*in);   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set ft4 %.18e", ft*ft*ft*ft);   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set mm4 %.18e", mm*mm*mm*mm);   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set cm4 %.18e", cm*cm*cm*cm);   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set m4 %.18e", m*m*m*m);   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set pi %.18e", 2.0*asin(1.0));   Tcl_Eval(interp, buffer);
    sprintf(buffer, "set PI %.18e", 2.0*asin(1.0));   Tcl_Eval(interp, buffer);
    
    int res = simulationInfo.setForceUnit(force);
    res += simulationInfo.setLengthUnit(length);
    res += simulationInfo.setTimeUnit(time);
    res += simulationInfo.setTemperatureUnit(temperature);
    
    return TCL_OK;
}


int openFresco_metaData(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    if (argc < 2)
        return -1;
    
    int count = 1;
    while (count < argc)  {
        if ((strcmp(argv[count], "-title") == 0) || (strcmp(argv[count], "-Title") == 0)
            || (strcmp(argv[count], "-TITLE") == 0))  {
            if (count + 1 < argc)  {
                simulationInfo.setTitle(argv[count + 1]);
                count += 2;
            }
        }
        else if ((strcmp(argv[count], "-contact") == 0) || (strcmp(argv[count], "-Contact") == 0)
            || (strcmp(argv[count], "-CONTACT") == 0))  {
            if (count + 1 < argc)  {
                simulationInfo.setContact(argv[count + 1]);
                count += 2;
            }
        }
        else if ((strcmp(argv[count], "-description") == 0) || (strcmp(argv[count], "-Description") == 0)
            || (strcmp(argv[count], "-DESCRIPTION") == 0))  {
            if (count + 1 < argc)  {
                simulationInfo.setDescription(argv[count + 1]);
                count += 2;
            }
        }
        else if ((strcmp(argv[count], "-modelType") == 0) || (strcmp(argv[count], "-ModelType") == 0)
            || (strcmp(argv[count], "-MODELTYPE") == 0))  {
            if (count + 1 < argc)  {
                simulationInfo.addModelType(argv[count + 1]);
                count += 2;
            }
        }
        else if ((strcmp(argv[count], "-analysisType") == 0) || (strcmp(argv[count], "-AnalysisType") == 0)
            || (strcmp(argv[count], "-ANALYSISTYPE") == 0))  {
            if (count + 1 < argc)  {
                simulationInfo.addAnalysisType(argv[count + 1]);
                count += 2;
            }
        }
        else if ((strcmp(argv[count], "-elementType") == 0) || (strcmp(argv[count], "-ElementType") == 0)
            || (strcmp(argv[count], "-ELEMENTTYPE") == 0))  {
            if (count + 1 < argc)  {
                simulationInfo.addElementType(argv[count + 1]);
                count += 2;
            }
        }
        else if ((strcmp(argv[count], "-materialType") == 0) || (strcmp(argv[count], "-MaterialType") == 0)
            || (strcmp(argv[count], "-MATERIALTYPE") == 0))  {
            if (count + 1 < argc)  {
                simulationInfo.addMaterialType(argv[count + 1]);
                count += 2;
            }
        }
        else if ((strcmp(argv[count], "-loadingType") == 0) || (strcmp(argv[count], "-LoadingType") == 0)
            || (strcmp(argv[count], "-LOADINGTYPE") == 0))  {
            if (count + 1 < argc)  {
                simulationInfo.addLoadingType(argv[count + 1]);
                count += 2;
            }
        }
        else  {
            opserr << "WARNING unknown arg type: " << argv[count] << endln;
            count++;
        }
    }
    
    return TCL_OK;
}


int openFresco_version(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    char buffer[20];
    
    sprintf(buffer, "%s", OPF_VERSION);
    Tcl_SetResult(interp, buffer, TCL_VOLATILE);
    
    return TCL_OK;
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
    
    Tcl_CreateCommand(interp, "model", specifyModelBuilder,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "rayleigh", rayleighDamping,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "setElementRayleighDampingFactors", setElementRayleighDampingFactors,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "logFile", openFresco_logFile,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "defaultUnits", openFresco_defaultUnits,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "metaData", openFresco_metaData,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    Tcl_CreateCommand(interp, "version", openFresco_version,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
    
    /* OpenSees commands
    Tcl_CreateCommand(interp, "recorder", addRecorder,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);*/
    
    /*
    * Specify a user-specific startup file to invoke if the application
    * is run interactively.  Typically the startup file is "~/.apprc"
    * where "app" is the name of the application.  If this line is deleted
    * then no user-specific startup file will be run under any conditions.
    */
    Tcl_SetVar(interp, "tcl_rcFileName", "~/.tclshrc", TCL_GLOBAL_ONLY);
    return TCL_OK;
}
