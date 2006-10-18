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
// Description: This file contains the function invoked when the user
// invokes the expElement command in the interpreter. 

#include <TclModelBuilder.h>
#include <ArrayOfTaggedObjects.h>

#include <string.h>

static ArrayOfTaggedObjects *theExperimentalElements(0);
extern void printCommand(int argc, TCL_Char **argv);


// THE PROTOTYPES OF THE FUNCTIONS INVOKED BY THE INTERPRETER
extern int addEETruss(ClientData clientData, Tcl_Interp *interp,  int argc, 
    TCL_Char **argv, TclModelBuilder*, Domain*, int argStart); 

extern int addEEBeamColumn(ClientData clientData, Tcl_Interp *interp,  int argc, 
    TCL_Char **argv, TclModelBuilder*, Domain*, int argStart); 

extern int addEEZeroLength(ClientData clientData, Tcl_Interp *interp,  int argc, 
    TCL_Char **argv, TclModelBuilder*, Domain*, int argStart); 

extern int addEEGeneric(ClientData clientData, Tcl_Interp *interp,  int argc, 
    TCL_Char **argv, TclModelBuilder*, Domain*, int argStart); 

extern int addEEChevronBrace(ClientData clientData, Tcl_Interp *interp,  int argc, 
    TCL_Char **argv, TclModelBuilder*, Domain*, int argStart); 


int TclExpElementCommand(ClientData clientData, Tcl_Interp *interp, int argc,
    TCL_Char **argv, TclModelBuilder *theTclBuilder, Domain *theTclDomain)
{
    if (theExperimentalElements == 0)
        theExperimentalElements = new ArrayOfTaggedObjects(32);

    // ensure the destructor has not been called
    if (theTclBuilder == 0) {
        opserr << "WARNING builder has been destroyed\n";    
        return TCL_ERROR;
    }

    // check that there is at least two arguments
    if (argc < 2)  {
        opserr << "WARNING need to specify an element type\n";
        opserr << "Want: expElement eleType <specific element args> .. see manual for valid eleTypes & arguments\n";
        return TCL_ERROR;
    }

    // ----------------------------------------------------------------------------	
    if (strcmp(argv[1],"truss") == 0)  {
        int eleArgStart = 1;
        int result = addEETruss(clientData, interp, argc, argv,
            theTclBuilder, theTclDomain, eleArgStart);
        return result;
    }
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"beamColumn") == 0) {
        int eleArgStart = 1;
        int result = addEEBeamColumn(clientData, interp, argc, argv,
            theTclBuilder, theTclDomain, eleArgStart);
        return result;
    }
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"zeroLength") == 0) {
        int eleArgStart = 1;
        int result = addEEZeroLength(clientData, interp, argc, argv,
            theTclBuilder, theTclDomain, eleArgStart);
        return result;
    }
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"generic") == 0) {
        int eleArgStart = 1;
        int result = addEEGeneric(clientData, interp, argc, argv,
            theTclBuilder, theTclDomain, eleArgStart);
        return result;
    }
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"chevronBrace") == 0) {
        int eleArgStart = 1;
        int result = addEEChevronBrace(clientData, interp, argc, argv,
            theTclBuilder, theTclDomain, eleArgStart);
        return result;
    }
    // ----------------------------------------------------------------------------	
    else {
        // element type not recognized
        opserr << "WARNING unknown element type: expElement "
            <<  argv[1] << ": check the manual\n";
        return TCL_ERROR;
    }    
}
