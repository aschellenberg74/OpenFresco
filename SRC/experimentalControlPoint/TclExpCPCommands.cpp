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
// Created: 02/07
// Revision: A
//
// Description: This file contains the function invoked when the user
// invokes the expControlPoint command in the interpreter. 

#include <string.h>
#include <tcl.h>
#include <ArrayOfTaggedObjects.h>
#include <Domain.h>
#include <Node.h>
#include <elementAPI.h>

#include <ExperimentalCP.h>


static void printCommand(int argc, TCL_Char **argv)
{
    opserr << "Input command: ";
    for (int i=0; i<argc; i++)
        opserr << argv[i] << " ";
    opserr << endln;
}


int TclExpCPCommand(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, Domain *theDomain)
{
    // reset the input args
    OPS_ResetInputNoBuilder(clientData, interp, 1, argc, argv, theDomain);
    
    // pointer to control that will be added
    ExperimentalCP *theControlPoint = 0;
    
    void* theCP = OPF_ExperimentalCP();
    if (theCP != 0)
        theControlPoint = (ExperimentalCP*)theCP;
    else
        return TCL_ERROR;
    
    // now add the control point to the modelBuilder
    if (OPF_AddExperimentalCP(theControlPoint) == false)  {
        delete theControlPoint; // invoke the destructor, otherwise mem leak
        return TCL_ERROR;
    }
    
    return TCL_OK;
}


int TclRemoveExpCP(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv)
{
    if (strcmp(argv[1], "controlPoint") == 0)  {
        if (argc != 3)  {
            opserr << "WARNING invalid number of arguments\n";
            printCommand(argc, argv);
            opserr << "Want: removeExp controlPoint tag\n";
            return TCL_ERROR;
        }
        int tag;
        if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK)  {
            opserr << "WARNING invalid removeExp controlPoint tag\n";
            return TCL_ERROR;
        }
        if (OPF_RemoveExperimentalCP(tag) == false) {
            opserr << "WARNING could not remove expControlPoint with tag " << tag << endln;
            return TCL_ERROR;
        }
    }
    else if (strcmp(argv[1], "controlPoints") == 0)  {
        OPF_ClearExperimentalCPs();
    }
    
    return TCL_OK;
}
