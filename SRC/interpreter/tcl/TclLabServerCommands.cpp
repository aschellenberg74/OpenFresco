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
// Created: 04/11
// Revision: A
//
// Description: This file contains the functions invoked when the user
// invokes the different LabServer commands in the interpreter. 

#include <tcl.h>

#include <ExperimentalSite.h>
#include <ActorExpSite.h>


int TclStartLabServer(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{ 
    if (argc != 2)  {
        opserr << "WARNING insufficient arguments\n"
            << "Want: startLabServer siteTag\n";
        return TCL_ERROR;
    }
    
    int siteTag;
    
    if (Tcl_GetInt(interp, argv[1], &siteTag) != TCL_OK)  {
        opserr << "WARNING invalid startLabServer siteTag\n";
        return TCL_ERROR;
    }
    ActorExpSite *theExperimentalSite =
        dynamic_cast <ActorExpSite*> (OPF_GetExperimentalSite(siteTag));
    if (theExperimentalSite != 0)  {
        // start server process
        opserr << "\nActorExpSite " << siteTag
            << " now running..." << endln;
        theExperimentalSite->run();
    } else  {
        opserr << "WARNING actor experimental site not found\n";
        opserr << "unable to start expSite: " << siteTag << endln;
        return TCL_ERROR;
    }
    //OPF_RemoveExperimentalSite(siteTag);
    //theExperimentalSite = 0;
    //delete theExperimentalSite;
    
    return TCL_OK;
}


int TclSetupLabServer(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{ 
    if (argc != 2)  {
        opserr << "WARNING insufficient arguments\n"
            << "Want: setupLabServer siteTag\n";
        return TCL_ERROR;
    }
    
    int siteTag;
    
    if (Tcl_GetInt(interp, argv[1], &siteTag) != TCL_OK)  {
        opserr << "WARNING invalid setupLabServer siteTag\n";
        return TCL_ERROR;
    }
    ActorExpSite *theExperimentalSite =
        dynamic_cast <ActorExpSite*> (OPF_GetExperimentalSite(siteTag));
    if (theExperimentalSite != 0)  {
        // start server process and run till setup is done
        opserr << "\nActorExpSite " << siteTag
            << " now being setup..." << endln;
        theExperimentalSite->runTill(OF_RemoteTest_setup);
    } else  {
        opserr << "WARNING actor experimental site not found\n";
        opserr << "unable to setup expSite: " << siteTag << endln;
        return TCL_ERROR;
    }
    
    return TCL_OK;
}


int TclStepLabServer(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{ 
    if (argc != 3)  {
        opserr << "WARNING insufficient arguments\n"
            << "Want: stepLabServer siteTag numSteps\n";
        return TCL_ERROR;
    }
    
    int siteTag, numSteps;
    int step = 1;
    
    if (Tcl_GetInt(interp, argv[1], &siteTag) != TCL_OK)  {
        opserr << "WARNING invalid stepLabServer siteTag\n";
        return TCL_ERROR;
    }
    if (Tcl_GetInt(interp, argv[2], &numSteps) != TCL_OK)  {
        opserr << "WARNING invalid stepLabServer numSteps\n";
        return TCL_ERROR;
    }
    ActorExpSite *theExperimentalSite =
        dynamic_cast <ActorExpSite*> (OPF_GetExperimentalSite(siteTag));
    if (theExperimentalSite != 0)  {
        // start server process and run for numSteps
        opserr << "\nActorExpSite " << siteTag
            << " now running for " << numSteps << " steps..." << endln;
        while (step <= numSteps)  {
            theExperimentalSite->runTill(OF_RemoteTest_commitState);
            step++;
        }
    } else  {
        opserr << "WARNING actor experimental site not found\n";
        opserr << "unable to step expSite: " << siteTag << endln;
        return TCL_ERROR;
    }
    
    return TCL_OK;
}


int TclStopLabServer(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{ 
    if (argc != 2)  {
        opserr << "WARNING insufficient arguments\n"
            << "Want: stopLabServer siteTag\n";
        return TCL_ERROR;
    }
    
    int siteTag;
    
    if (Tcl_GetInt(interp, argv[1], &siteTag) != TCL_OK)  {
        opserr << "WARNING invalid stopLabServer siteTag\n";
        return TCL_ERROR;
    }
    ActorExpSite *theExperimentalSite =
        dynamic_cast <ActorExpSite*> (OPF_GetExperimentalSite(siteTag));
    if (theExperimentalSite != 0)  {
        // stop server process by destructing ExpSite object
        opserr << "\nActorExpSite " << siteTag
            << " now terminated." << endln;
        delete theExperimentalSite;
    } else  {
        opserr << "WARNING actor experimental site not found\n";
        opserr << "unable to stop expSite: " << siteTag << endln;
        return TCL_ERROR;
    }
    
    return TCL_OK;
}
