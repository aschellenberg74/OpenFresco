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
// Description: This file contains the function invoked when the user
// invokes the expSite command in the interpreter. 

#include <string.h>
#include <tcl.h>
#include <ArrayOfTaggedObjects.h>
#include <Vector.h>
#include <TCP_Socket.h>
#include <TCP_SocketSSL.h>
#include <UDP_Socket.h>
#include <elementAPI.h>

#include <ExperimentalSite.h>

extern void* OPF_LocalExpSite();
extern void* OPF_ShadowExpSite();
extern void* OPF_ActorExpSite();


static void printCommand(int argc, TCL_Char **argv)
{
    opserr << "Input command: ";
    for (int i=0; i<argc; i++)
        opserr << argv[i] << " ";
    opserr << endln;
}


int TclExpSiteCommand(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, Domain *theDomain)
{
    // make sure there is a minimum number of arguments
    if (argc < 3)  {
        opserr << "WARNING insufficient number of experimental site arguments\n";
        opserr << "Want: expSite type tag <specific experimental site args>\n";
        return TCL_ERROR;
    }
    
    // reset the input args
    OPS_ResetInputNoBuilder(clientData, interp, 2, argc, argv, theDomain);
    
    // pointer to site that will be added
    ExperimentalSite* theSite = 0;
    
    // ----------------------------------------------------------------------------	
    if (strcmp(argv[1],"LocalSite") == 0)  {
        void* theES = OPF_LocalExpSite();
        if (theES != 0)
            theSite = (ExperimentalSite*)theES;
        else
            return TCL_ERROR;
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"ShadowSite") == 0 || strcmp(argv[1],"RemoteSite") == 0)  {
        void* theES = OPF_ShadowExpSite();
        if (theES != 0)
            theSite = (ExperimentalSite*)theES;
        else
            return TCL_ERROR;
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"ActorSite") == 0)  {
        void* theES = OPF_ActorExpSite();
        if (theES != 0)
            theSite = (ExperimentalSite*)theES;
        else
            return TCL_ERROR;
    }
    
    // ----------------------------------------------------------------------------	
    else  {
        // experimental site type not recognized
        opserr << "WARNING unknown experimental site type: "
            <<  argv[1] << ": check the manual\n";
        return TCL_ERROR;
    }
    
    // now add the site to the modelBuilder
    if (OPF_AddExperimentalSite(theSite) == false) {
        delete theSite; // invoke the destructor, otherwise mem leak
        return TCL_ERROR;
    }
    
    return TCL_OK;
}


int TclRemoveExpSite(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv)
{
    if (strcmp(argv[1], "site") == 0)  {
        if (argc != 3)  {
            opserr << "WARNING invalid number of arguments\n";
            printCommand(argc, argv);
            opserr << "Want: removeExp site tag\n";
            return TCL_ERROR;
        }
        int tag;
        if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK)  {
            opserr << "WARNING invalid removeExp site tag\n";
            return TCL_ERROR;
        }
        if (OPF_RemoveExperimentalSite(tag) == false) {
            opserr << "WARNING could not remove expSite with tag " << tag << endln;
            return TCL_ERROR;
        }
        //ExperimentalSite *theSite = removeExperimentalSite(tag);
        //if (theSite != 0)  {
        //    delete theSite;
        //    theSite = 0;
        //} else  {
        //    opserr << "WARNING could not remove expSite with tag " << argv[2] << endln;
        //    return TCL_ERROR;
        //}
    }
    else if (strcmp(argv[1], "sites") == 0)  {
        OPF_ClearExperimentalSites();
    }
    
    return TCL_OK;
}
