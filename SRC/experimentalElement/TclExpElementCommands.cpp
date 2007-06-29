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
// $URL: $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 09/06
// Revision: A
//
// Description: This file contains the function invoked when the user
// invokes the expElement command in the interpreter. 

#include <TclModelBuilder.h>
#include <ArrayOfTaggedObjects.h>

#include <string.h>


extern int addEETruss(ClientData clientData, Tcl_Interp *interp,  int argc, 
    TCL_Char **argv, Domain*, TclModelBuilder*, int argStart); 

extern int addEEBeamColumn(ClientData clientData, Tcl_Interp *interp,  int argc, 
    TCL_Char **argv, Domain*, TclModelBuilder*, int argStart); 

extern int addEETwoNodeLink(ClientData clientData, Tcl_Interp *interp,  int argc, 
    TCL_Char **argv, Domain*, TclModelBuilder*, int argStart); 

extern int addEEGeneric(ClientData clientData, Tcl_Interp *interp,  int argc, 
    TCL_Char **argv, Domain*, TclModelBuilder*, int argStart); 

extern int addEEInvertedVBrace(ClientData clientData, Tcl_Interp *interp,  int argc, 
    TCL_Char **argv, Domain*, TclModelBuilder*, int argStart); 


static void printCommand(int argc, TCL_Char **argv)
{
    opserr << "Input command: ";
    for (int i=0; i<argc; i++)
		opserr << argv[i] << " ";
    opserr << endln;
} 


int TclExpElementCommand(ClientData clientData, Tcl_Interp *interp, int argc,
    TCL_Char **argv, Domain *theTclDomain, TclModelBuilder *theTclBuilder)
{
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
            theTclDomain, theTclBuilder, eleArgStart);
        return result;
    }
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"beamColumn") == 0) {
        int eleArgStart = 1;
        int result = addEEBeamColumn(clientData, interp, argc, argv,
            theTclDomain, theTclBuilder, eleArgStart);
        return result;
    }
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"twoNodeLink") == 0) {
        int eleArgStart = 1;
        int result = addEETwoNodeLink(clientData, interp, argc, argv,
            theTclDomain, theTclBuilder, eleArgStart);
        return result;
    }
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"generic") == 0) {
        int eleArgStart = 1;
        int result = addEEGeneric(clientData, interp, argc, argv,
            theTclDomain, theTclBuilder, eleArgStart);
        return result;
    }
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"invertedVBrace") == 0) {
        int eleArgStart = 1;
        int result = addEEInvertedVBrace(clientData, interp, argc, argv,
            theTclDomain, theTclBuilder, eleArgStart);
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
