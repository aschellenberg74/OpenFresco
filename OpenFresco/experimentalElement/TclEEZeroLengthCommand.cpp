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
// for the EEZeroLength element.

#include <TclModelBuilder.h>

#include <stdlib.h>
#include <string.h>
#include <Domain.h>
#include <ID.h>
#include <Vector.h>

#include <EEZeroLength.h>

extern void printCommand(int argc, TCL_Char **argv);
extern ExperimentalSite *getExperimentalSite(int tag);


int addEEZeroLength(ClientData clientData, Tcl_Interp *interp, int argc,
    TCL_Char **argv, TclModelBuilder *theTclBuilder, Domain *theTclDomain,
    int eleArgStart)
{
    // ensure the destructor has not been called
    if (theTclBuilder == 0)  {
        opserr << "WARNING builder has been destroyed - expElement zeroLength\n";    
        return TCL_ERROR;
    }
    
    ExperimentalElement *theExpElement = 0;
    int ndm = theTclBuilder->getNDM();
    
    // check the number of arguments is correct
    if ((argc-eleArgStart) < 8)  {
        opserr << "WARNING insufficient arguments\n";
        printCommand(argc, argv);
        opserr << "Want: expElement zeroLength eleTag iNode jNode siteTag -dir dirs -initStif Kij <-orient x1 x2 x3 y1 y2 y3> <-iMod> <-isCopy> <-mass m>\n";
        return TCL_ERROR;
    }    
    
    // get the id and end nodes 
    int tag, iNode, jNode, siteTag, numDir, dirID, argi;
    bool iMod = false;
    bool isCopy = false;
	double mass = 0.0;
    
    if (Tcl_GetInt(interp, argv[1+eleArgStart], &tag) != TCL_OK)  {
        opserr << "WARNING invalid expElement zeroLength eleTag\n";
        return TCL_ERROR;
    }
    if (Tcl_GetInt(interp, argv[2+eleArgStart], &iNode) != TCL_OK)  {
        opserr << "WARNING invalid iNode\n";
        opserr << "expElement zeroLength element: " << tag << endln;
        return TCL_ERROR;
    }
    if (Tcl_GetInt(interp, argv[3+eleArgStart], &jNode) != TCL_OK)  {
        opserr << "WARNING invalid jNode\n";
        opserr << "expElement zeroLength element: " << tag << endln;
        return TCL_ERROR;
    }
    if (Tcl_GetInt(interp, argv[4+eleArgStart], &siteTag) != TCL_OK)  {
        opserr << "WARNING invalid siteTag\n";
        opserr << "expElement zeroLength element: " << tag << endln;
        return TCL_ERROR;
    }
    ExperimentalSite *theSite = getExperimentalSite(siteTag);
    if (theSite == 0)  {
        opserr << "WARNING experimental site not found\n";
        opserr << "expSite: " << siteTag << endln;
        opserr << "expElement zeroLength element: " << tag << endln;
        return TCL_ERROR;
    }
    // read the number of directions
    numDir = 0;
    if (strcmp(argv[5+eleArgStart],"-dir") != 0)  {
        opserr << "WARNING expecting -dir flag\n";
        opserr << "expElement zeroLength element: " << tag << endln;
        return TCL_ERROR;	
    }
    argi = 6 + eleArgStart; 
    while ((argi < argc) && (strcmp(argv[argi],"-initStif") != 0))  {
        numDir++;
        argi++;
    }
    // create an ID array to hold the direction IDs
    ID theDirIDs(numDir);
    argi = 6 + eleArgStart; 	
    // read the act identifiers
	int i;
    for (i=0; i<numDir; i++)  {
        if (Tcl_GetInt(interp, argv[argi], &dirID) != TCL_OK)  {
            opserr << "WARNING invalid direction ID\n";
            opserr << "expElement zeroLength element: " << tag << endln;	    
            return TCL_ERROR;
        } else  {
            theDirIDs[i] = dirID - 1;
            argi++;
        }
    }
    
    // check for optional arguments
    Vector x(3); x(0) = 1.0; x(1) = 0.0; x(2) = 0.0;
    Vector y(3); y(0) = 0.0; y(1) = 1.0; y(2) = 0.0;
    for (i = 7+eleArgStart; i < argc; i++)  {
        if (i+1 < argc && strcmp(argv[i],"-orient") == 0)  {
            if (argc < (i+7))  {
                opserr << "WARNING not enough paramaters after -orient flag\n";
                opserr << "expElement zeroLength element: " << tag << endln;
                return TCL_ERROR;		
            } else  {
                argi = i+1;
                double value;
                // read the x values
                for (int i=0; i<3; i++)  {
                    if (Tcl_GetDouble(interp, argv[argi], &value) != TCL_OK)  {
                        opserr << "WARNING invalid -orient value\n";
                        opserr << "expElement zeroLength element: " << tag << endln;
                        return TCL_ERROR;
                    } else  {
                        argi++;
                        x(i) = value;
                    }
                }
                // read the y values
                for (int j=0; j<3; j++)  {
                    if (Tcl_GetDouble(interp, argv[argi], &value) != TCL_OK)  {
                        opserr << "WARNING invalid -orient value\n";
                        opserr << "expElement zeroLength element: " << tag << endln;
                        return TCL_ERROR;
                    } else  {
                        argi++;
                        y(j) = value;		
                    }
                }
            }
        }
    }
    for (i = 7+eleArgStart; i < argc; i++)  {
        if (strcmp(argv[i], "-iMod") == 0)  {
            iMod = true;
        }
    }
    for (i = 7+eleArgStart; i < argc; i++)  {
        if (strcmp(argv[i], "-isCopy") == 0)  {
            isCopy = true;
        }
    }
	for (i = 7+eleArgStart; i < argc; i++)  {
		if (i+1 < argc && strcmp(argv[i], "-mass") == 0)  {
			if (Tcl_GetDouble(interp, argv[i+1], &mass) != TCL_OK)  {
				opserr << "WARNING invalid mass\n";
				opserr << "expElement zeroLength element: " << tag << endln;
				return TCL_ERROR;
			}
		}
	}

    // now create the EEZeroLength
    theExpElement = new EEZeroLength(tag, ndm, iNode, jNode, theSite, theDirIDs, x, y, iMod, isCopy, mass);
    
	if (theExpElement == 0)  {
		opserr << "WARNING ran out of memory creating element\n";
		opserr << "expElement zeroLength element: " << tag << endln;
		return TCL_ERROR;
	}
    
	// then add the EEZeroLength to the domain
	if (theTclDomain->addElement(theExpElement) == false)  {
		opserr << "WARNING could not add element to the domain\n";
		opserr << "expElement zeroLength element: " << tag << endln;
		delete theExpElement;
		return TCL_ERROR;
	}
    
	// finally check for initial stiffness terms
	for (i = 7+eleArgStart; i < argc; i++)  {
		if (i+1 < argc && strcmp(argv[i], "-initStif") == 0)  {
			if (argc < i+numDir*numDir)  {
				opserr << "WARNING incorrect number of inital stiffness terms\n";
				opserr << "expTruss element: " << tag << endln;
				return TCL_ERROR;      
			}
			Matrix theInitStif(numDir,numDir);
			double stif;
			for (int j=0; j<numDir; j++)  {
				for (int k=0; k<numDir; k++)  {
					if (Tcl_GetDouble(interp, argv[i+1+1*j+k], &stif) != TCL_OK)  {
						opserr << "WARNING invalid initial stiffness term\n";
						opserr << "expTruss element: " << tag << endln;
						return TCL_ERROR;
					}
					theInitStif(j,k) = stif;
				}
			}
			theExpElement->setInitialStiff(theInitStif);
		}
	}

    // if get here we have sucessfully created the EEZeroLength and added it to the domain
	return TCL_OK;
}
