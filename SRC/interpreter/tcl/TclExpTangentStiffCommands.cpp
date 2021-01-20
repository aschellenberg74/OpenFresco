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

// Written: Hong Kim (hongkim@berkeley.edu)
// Created: 05/10
// Revision: A
//
// Description: This file contains the function invoked when the user
// invokes the expTangentStiff command in the interpreter. 

#include <string.h>
#include <tcl.h>
#include <ArrayOfTaggedObjects.h>
#include <elementAPI.h>

#include <ExperimentalTangentStiff.h>

extern void* OPF_ETBroyden();
extern void* OPF_ETBfgs();
extern void* OPF_ETTranspose();


static void printCommand(int argc, TCL_Char **argv)
{
    opserr << "Input command: ";
    for (int i=0; i<argc; i++)
        opserr << argv[i] << " ";
    opserr << endln;
}


int TclExpTangentStiffCommand(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, Domain *theDomain)
{
    // make sure there is a minimum number of arguments
    if (argc < 3)  {
        opserr << "WARNING insufficient number of experimental tangent stiff arguments\n";
        opserr << "Want: expTangentStiff type tag <specific experimental tangent stiff args>\n";
        return TCL_ERROR;
    }
    
    // reset the input args
    OPS_ResetInputNoBuilder(clientData, interp, 2, argc, argv, theDomain);
    
    // pointer to tangent stiff that will be added
    ExperimentalTangentStiff *theTangentStiff = 0;
    
    // ----------------------------------------------------------------------------	
    if (strcmp(argv[1],"Broyden") == 0)  {
        void* theET = OPF_ETBroyden();
        if (theET != 0)
            theTangentStiff = (ExperimentalTangentStiff*)theET;
        else
            return TCL_ERROR;
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"BFGS") == 0)  {
        void* theET = OPF_ETBfgs();
        if (theET != 0)
            theTangentStiff = (ExperimentalTangentStiff*)theET;
        else
            return TCL_ERROR;
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"Transpose") == 0)  {
        void* theET = OPF_ETTranspose();
        if (theET != 0)
            theTangentStiff = (ExperimentalTangentStiff*)theET;
        else
            return TCL_ERROR;
    }
    
    // ----------------------------------------------------------------------------	
    else  {
        // experimental tangent stiff type not recognized
        opserr << "WARNING unknown experimental tangent stiff type: "
            <<  argv[1] << ": check the manual\n";
        return TCL_ERROR;
    }
    
    if (theTangentStiff == 0)  {
        opserr << "WARNING could not create experimental tangent stiff " << argv[1] << endln;
        return TCL_ERROR;
    }
    
    // now add the tangent stiff to the modelBuilder
    if (OPF_AddExperimentalTangentStiff(theTangentStiff) == false)  {
        delete theTangentStiff; // invoke the destructor, otherwise mem leak
        return TCL_ERROR;
    }
    
    return TCL_OK;
}


int TclRemoveExpTangentStiff(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv)
{
    if (strcmp(argv[1], "tangentStiff") == 0)  {
        if (argc != 3)  {
            opserr << "WARNING invalid number of arguments\n";
            printCommand(argc, argv);
            opserr << "Want: removeExp tangentStiff tag\n";
            return TCL_ERROR;
        }
        int tag;
        if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK)  {
            opserr << "WARNING invalid removeExp tangentStiff tag\n";
            return TCL_ERROR;
        }
        if (OPF_RemoveExperimentalTangentStiff(tag) == false) {
            opserr << "WARNING could not remove expTangentStiff with tag " << tag << endln;
            return TCL_ERROR;
        }
        //ExperimentalTangentStiff *theTangentStiff = removeExperimentalTangentStiff(tag);
        //if (theTangentStiff != 0)  {
        //    delete theTangentStiff;
        //    theTangentStiff = 0;
        //} else  {
        //    opserr << "WARNING could not remove expTangentStiff with tag " << argv[2] << endln;
        //    return TCL_ERROR;
        //}
    }
    else if (strcmp(argv[1], "tangentStiffs") == 0)  {
        OPF_ClearExperimentalTangentStiffs();
    }
    
    return TCL_OK;
}
