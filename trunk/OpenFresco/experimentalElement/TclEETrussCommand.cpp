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
// Description: This file contains the function to parse the TCL input
// for the EETruss element.

#include <TclModelBuilder.h>

#include <stdlib.h>
#include <string.h>
#include <Domain.h>

#include <EETruss.h>

extern void printCommand(int argc, TCL_Char **argv);
extern ExperimentalSite *getExperimentalSite(int tag);


int addEETruss(ClientData clientData, Tcl_Interp *interp,  int argc, 
    TCL_Char **argv, TclModelBuilder *theTclBuilder, Domain *theTclDomain,
    int eleArgStart)
{
	// ensure the destructor has not been called
	if (theTclBuilder == 0)  {
		opserr << "WARNING builder has been destroyed - expElement truss\n";    
		return TCL_ERROR;
	}
	
	ExperimentalElement *theExpElement = 0;
	int ndm = theTclBuilder->getNDM();

	// check the number of arguments is correct
	if ((argc-eleArgStart) < 6)  {
		opserr << "WARNING insufficient arguments\n";
		printCommand(argc, argv);
		opserr << "Want: expElement truss eleTag iNode jNode siteTag -initStif Kij <-iMod> <-isCopy> <-rho rho>\n";
		return TCL_ERROR;
	}    
	
	// get the id and end nodes 
	int tag, iNode, jNode, siteTag;
    bool iMod = false;
    bool isCopy = false;
	double rho = 0.0;

	if (Tcl_GetInt(interp, argv[1+eleArgStart], &tag) != TCL_OK)  {
		opserr << "WARNING invalid expElement truss eleTag\n";
		return TCL_ERROR;
	}
	if (Tcl_GetInt(interp, argv[2+eleArgStart], &iNode) != TCL_OK)  {
		opserr << "WARNING invalid iNode\n";
		opserr << "expElement truss element: " << tag << endln;
		return TCL_ERROR;
	}
	if (Tcl_GetInt(interp, argv[3+eleArgStart], &jNode) != TCL_OK)  {
		opserr << "WARNING invalid jNode\n";
		opserr << "expElement truss element: " << tag << endln;
		return TCL_ERROR;
	}
	if (Tcl_GetInt(interp, argv[4+eleArgStart], &siteTag) != TCL_OK)  {
		opserr << "WARNING invalid siteTag\n";
		opserr << "expElement truss element: " << tag << endln;
		return TCL_ERROR;
	}
	ExperimentalSite *theSite = getExperimentalSite(siteTag);
	if (theSite == 0)  {
		opserr << "WARNING experimental site not found\n";
		opserr << "expSite: " << siteTag << endln;
		opserr << "expElement truss element: " << tag << endln;
		return TCL_ERROR;
	}
    for (int i = 5+eleArgStart; i < argc; i++)  {
        if (strcmp(argv[i], "-iMod") == 0)  {
            iMod = true;
        }
    }
    for (i = 5+eleArgStart; i < argc; i++)  {
        if (strcmp(argv[i], "-isCopy") == 0)  {
            isCopy = true;
        }
    }
	for (i = 5+eleArgStart; i < argc; i++)  {
		if (i+1 < argc && strcmp(argv[i], "-rho") == 0)  {
			if (Tcl_GetDouble(interp, argv[i+1], &rho) != TCL_OK)  {
				opserr << "WARNING invalid rho\n";
				opserr << "expElement truss element: " << tag << endln;
				return TCL_ERROR;
			}
		}
	}

	// now create the EETruss
	theExpElement = new EETruss(tag, ndm, iNode, jNode, theSite, iMod, isCopy, rho);		
	
	if (theExpElement == 0)  {
		opserr << "WARNING ran out of memory creating element\n";
		opserr << "expElement truss element: " << tag << endln;
		return TCL_ERROR;
	}
	
	// then add the EETruss to the domain
	if (theTclDomain->addElement(theExpElement) == false)  {
		opserr << "WARNING could not add element to the domain\n";
		opserr << "expElement truss element: " << tag << endln;
		delete theExpElement;
		return TCL_ERROR;
	}

	// finally check for initial stiffness terms
	for (i = 5+eleArgStart; i < argc; i++)  {
		if (i+1 < argc && strcmp(argv[i], "-initStif") == 0)  {
			if (argc < i+1)  {
				opserr << "WARNING incorrect number of inital stiffness terms\n";
				opserr << "expElement truss element: " << tag << endln;
				return TCL_ERROR;      
			}
			Matrix theInitStif(1,1);
			double stif;
			for (int j=0; j<1; j++)  {
				for (int k=0; k<1; k++)  {
					if (Tcl_GetDouble(interp, argv[i+1+1*j+k], &stif) != TCL_OK)  {
						opserr << "WARNING invalid initial stiffness term\n";
						opserr << "expElement truss element: " << tag << endln;
						return TCL_ERROR;
					}
					theInitStif(j,k) = stif;
				}
			}
			theExpElement->setInitialStiff(theInitStif);
		}
	}

	// if get here we have sucessfully created the EETruss and added it to the domain
	return TCL_OK;
}
