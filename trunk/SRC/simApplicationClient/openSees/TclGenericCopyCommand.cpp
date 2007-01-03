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

// $Revision: $
// $Date: $
// $URL: $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 11/06
// Revision: A
//
// Description: This file contains the function to parse the TCL input
// for the GenericCopy element.

#include <TclModelBuilder.h>

#include <stdlib.h>
#include <string.h>
#include <Domain.h>

#include <GenericCopy.h>


extern void printCommand(int argc, TCL_Char **argv);


int TclGenericCopyCommand(ClientData clientData, Tcl_Interp *interp,  int argc, 
    TCL_Char **argv, Domain *theTclDomain, TclModelBuilder *theTclBuilder)
{
	// ensure the destructor has not been called
	if (theTclBuilder == 0)  {
		opserr << "WARNING builder has been destroyed - expElement genericCopy\n";    
		return TCL_ERROR;
	}
	
	Element *theElement = 0;
	int ndm = theTclBuilder->getNDM();
    int eleArgStart = 1;

	// check the number of arguments is correct
	if ((argc-eleArgStart) < 6)  {
		opserr << "WARNING insufficient arguments\n";
		printCommand(argc, argv);
		opserr << "Want: expElement genericCopy eleTag -node Ndi ... -src srcTag\n";
		return TCL_ERROR;
	}    
	
	// get the id and end nodes     
    int tag, node, srcTag, argi, i;
    int numNodes = 0;

	if (Tcl_GetInt(interp, argv[1+eleArgStart], &tag) != TCL_OK)  {
		opserr << "WARNING invalid expElement genericCopy eleTag\n";
		return TCL_ERROR;
	}
    // read the number of nodes
    if (strcmp(argv[2+eleArgStart], "-node") != 0)  {
		opserr << "WARNING expecting -node flag\n";
		opserr << "expElement genericCopy element: " << tag << endln;
		return TCL_ERROR;
	}
    argi = 3+eleArgStart;
    i = argi;
    while (strcmp(argv[i], "-src") != 0  && i < argc)  {
        numNodes++;
        i++;
    }
    if (numNodes == 0)  {
		opserr << "WARNING no nodes specified\n";
		opserr << "expElement genericCopy element: " << tag << endln;
		return TCL_ERROR;
	}
    // create and fill in the ID array to hold the nodes
    ID nodes(numNodes);
    for (i=0; i<numNodes; i++)  {
        if (Tcl_GetInt(interp, argv[argi], &node) != TCL_OK)  {
		    opserr << "WARNING invalid node\n";
		    opserr << "expElement genericCopy element: " << tag << endln;
		    return TCL_ERROR;
	    }
        nodes(i) = node;
        argi++; 
    }
    if (strcmp(argv[argi], "-src") != 0)  {
	    opserr << "WARNING expect -src\n";
	    opserr << "expElement genericCopy element: " << tag << endln;
	    return TCL_ERROR;
    }
    argi++;
    if (Tcl_GetInt(interp, argv[argi], &srcTag) != TCL_OK)  {
        opserr << "WARNING invalid srcTag\n";
        opserr << "expElement genericCopy element: " << tag << endln;
        return TCL_ERROR;
    }
 
	// now create the GenericCopy
    theElement = new GenericCopy(tag, nodes, srcTag);
	
	if (theElement == 0)  {
		opserr << "WARNING ran out of memory creating element\n";
		opserr << "expElement genericCopy element: " << tag << endln;
		return TCL_ERROR;
	}
	
	// then add the GenericCopy to the domain
	if (theTclDomain->addElement(theElement) == false)  {
		opserr << "WARNING could not add element to the domain\n";
		opserr << "expElement genericCopy element: " << tag << endln;
		delete theElement;
		return TCL_ERROR;
	}

	// if get here we have sucessfully created the GenericCopy and added it to the domain
	return TCL_OK;
}
