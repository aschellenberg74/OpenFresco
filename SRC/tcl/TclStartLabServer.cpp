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

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 04/11
// Revision: A
//
// Description: This file contains the function invoked when the user
// invokes the startLabServer command in the interpreter. 

#include <tcl.h>

#include <ExperimentalSite.h>
#include <ActorExpSite.h>

extern ExperimentalSite *getExperimentalSite(int tag);


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
        dynamic_cast <ActorExpSite*> (getExperimentalSite(siteTag));
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
    delete theExperimentalSite;
    
    return TCL_OK;
}
