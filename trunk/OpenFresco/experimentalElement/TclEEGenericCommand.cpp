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
// Created: 10/06
// Revision: A
//
// Description: This file contains the function to parse the TCL input
// for the EEGeneric element.

#include <TclModelBuilder.h>

#include <stdlib.h>
#include <string.h>
#include <Domain.h>

#include <EEGeneric.h>

extern void printCommand(int argc, TCL_Char **argv);
extern ExperimentalSite *getExperimentalSite(int tag);


int addEEGeneric(ClientData clientData, Tcl_Interp *interp,  int argc, 
    TCL_Char **argv, TclModelBuilder *theTclBuilder, Domain *theTclDomain,
    int eleArgStart)
{
	// ensure the destructor has not been called
	if (theTclBuilder == 0)  {
		opserr << "WARNING builder has been destroyed - expElement generic\n";    
		return TCL_ERROR;
	}
	
	ExperimentalElement *theExpElement = 0;
	int ndm = theTclBuilder->getNDM();

	// check the number of arguments is correct
	if ((argc-eleArgStart) < 6)  {
		opserr << "WARNING insufficient arguments\n";
		printCommand(argc, argv);
		opserr << "Want: expElement generic eleTag siteTag -node Ndi -dof dofNdi -dof dofNdj ... -initStif Kij <-iMod> <-mass Mij>\n";
		return TCL_ERROR;
	}    
	
	// get the id and end nodes     
    int tag, siteTag, node, dof, argi, i, j, k;
    int numNodes = 0, numDOFj = 0, numDOF = 0;
    bool iMod = false;
    Matrix *mass = 0;

	if (Tcl_GetInt(interp, argv[1+eleArgStart], &tag) != TCL_OK)  {
		opserr << "WARNING invalid expElement generic eleTag\n";
		return TCL_ERROR;
	}
	if (Tcl_GetInt(interp, argv[2+eleArgStart], &siteTag) != TCL_OK)  {
		opserr << "WARNING invalid siteTag\n";
		opserr << "expElement generic element: " << tag << endln;
		return TCL_ERROR;
	}
	ExperimentalSite *theSite = getExperimentalSite(siteTag);
	if (theSite == 0)  {
		opserr << "WARNING experimental site not found\n";
		opserr << "expSite: " << siteTag << endln;
		opserr << "expElement generic element: " << tag << endln;
		return TCL_ERROR;
	}
    // read the number of nodes
    if (strcmp(argv[3+eleArgStart], "-node") != 0)  {
		opserr << "WARNING expecting -node flag\n";
		opserr << "expElement generic element: " << tag << endln;
		return TCL_ERROR;
	}
    argi = 4+eleArgStart;
    i = argi;
    while (strcmp(argv[i], "-dof") != 0  && i < argc)  {
        numNodes++;
        i++;
    }
    if (numNodes == 0)  {
		opserr << "WARNING no nodes specified\n";
		opserr << "expElement generic element: " << tag << endln;
		return TCL_ERROR;
	}
    // create the ID arrays to hold the nodes and dofs
    ID nodes(numNodes);
    ID *dofs = new ID [numNodes];
    if (dofs == 0)  {
		opserr << "WARNING out of memory\n";
		opserr << "expElement generic element: " << tag << endln;
		return TCL_ERROR;
	}
    // fill in the nodes ID
    for (i=0; i<numNodes; i++)  {
        if (Tcl_GetInt(interp, argv[argi], &node) != TCL_OK)  {
		    opserr << "WARNING invalid node\n";
		    opserr << "expElement generic element: " << tag << endln;
		    return TCL_ERROR;
	    }
        nodes(i) = node;
        argi++; 
    }
    for (j=0; j<numNodes; j++)  {
        // read the number of dofs per node j
        numDOFj = 0;
        if (strcmp(argv[argi], "-dof") != 0)  {
		    opserr << "WARNING expect -dof\n";
		    opserr << "expElement generic element: " << tag << endln;
		    return TCL_ERROR;
	    }
        argi++;
        i = argi;
        while (strcmp(argv[i], "-dof") != 0 && 
            strcmp(argv[i], "-initStif") != 0 && i < argc)  {
            numDOFj++;
            numDOF++;
            i++;
        }
        // fill in the dofs ID array
        ID dofsj(numDOFj);
        for (i=0; i<numDOFj; i++)  {
            if (Tcl_GetInt(interp, argv[argi], &dof) != TCL_OK)  {
		        opserr << "WARNING invalid dof\n";
		        opserr << "expElement generic element: " << tag << endln;
		        return TCL_ERROR;
	        }
            dofsj(i) = dof-1;
            argi++; 
        }
        dofs[0] = dofsj;
    }
    for (i=argi; i<argc; i++)  {
        if (strcmp(argv[i], "-iMod") == 0)  {
            iMod = true;
        }
    }
	for (i=argi; i<argc; i++)  {
		if (argc && strcmp(argv[i], "-mass") == 0)  {
			if (argc < i+numDOF)  {
				opserr << "WARNING incorrect number of mass terms\n";
				opserr << "expElement generic element: " << tag << endln;
				return TCL_ERROR;      
			}
			mass = new Matrix(numDOF,numDOF);
			double m;
			for (j=0; j<1; j++)  {
				for (k=0; k<1; k++)  {
					if (Tcl_GetDouble(interp, argv[i+1+1*j+k], &m) != TCL_OK)  {
						opserr << "WARNING invalid mass term\n";
						opserr << "expElement generic element: " << tag << endln;
						return TCL_ERROR;
					}
					(*mass)(j,k) = m;
				}
			}
		}
	}
 
	// now create the EEGeneric
    if (mass == 0) {
        opserr << "no mass\n";
	    theExpElement = new EEGeneric(tag, theSite, nodes, dofs, iMod);
    }
    else {
        opserr << "got mass " << *mass << endln;
        theExpElement = new EEGeneric(tag, theSite, nodes, dofs, iMod, mass);
    }
	
	if (theExpElement == 0)  {
		opserr << "WARNING ran out of memory creating element\n";
		opserr << "expElement generic element: " << tag << endln;
		return TCL_ERROR;
	}
	
	// then add the EEGeneric to the domain
	if (theTclDomain->addElement(theExpElement) == false)  {
		opserr << "WARNING could not add element to the domain\n";
		opserr << "expElement generic element: " << tag << endln;
		delete theExpElement;
		return TCL_ERROR;
	}

	// finally check for initial stiffness terms
	for (i=argi; i<argc; i++)  {
		if (strcmp(argv[i], "-initStif") == 0)  {
			if (argc < i+numDOF)  {
				opserr << "WARNING incorrect number of inital stiffness terms\n";
				opserr << "expElement generic element: " << tag << endln;
				return TCL_ERROR;      
			}
			Matrix theInitStif(numDOF,numDOF);
			double stif;
			for (j=0; j<1; j++)  {
				for (k=0; k<1; k++)  {
					if (Tcl_GetDouble(interp, argv[i+1+1*j+k], &stif) != TCL_OK)  {
						opserr << "WARNING invalid initial stiffness term\n";
						opserr << "expElement generic element: " << tag << endln;
						return TCL_ERROR;
					}
					theInitStif(j,k) = stif;
				}
			}
			theExpElement->setInitialStiff(theInitStif);
		}
	}

	// if get here we have sucessfully created the EEGeneric and added it to the domain
	return TCL_OK;
}
