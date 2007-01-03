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
    TCL_Char **argv, Domain *theTclDomain, TclModelBuilder *theTclBuilder,
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
    if ((argc-eleArgStart) < 10)  {
        opserr << "WARNING insufficient arguments\n";
        printCommand(argc, argv);
        opserr << "Want: expElement zeroLength eleTag iNode jNode -dir dirs -site siteTag -initStif Kij <-orient x1 x2 x3 y1 y2 y3> <-iMod> <-mass m>\n";
        opserr << "  or: expElement zeroLength eleTag iNode jNode -dir dirs -server ipPort <ipAddr> <-dataSize size> -initStif Kij <-orient x1 x2 x3 y1 y2 y3> <-iMod> <-mass m>\n";
        return TCL_ERROR;
    }    
    
    // get the id and end nodes 
    int tag, iNode, jNode, siteTag, numDir, dirID, ipPort, argi, i, j, k;
    ExperimentalSite *theSite = 0;
    char *ipAddr = 0;
    int dataSize = OF_Network_dataSize;
    bool iMod = false;
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
    // read the number of directions
    numDir = 0;
    if (strcmp(argv[4+eleArgStart],"-dir") != 0)  {
        opserr << "WARNING expecting -dir flag\n";
        opserr << "expElement zeroLength element: " << tag << endln;
        return TCL_ERROR;	
    }
    argi = 5+eleArgStart;
    i = argi;
    while (strcmp(argv[i],"-site") != 0 && 
        strcmp(argv[i],"-server") != 0 && i < argc)  {
        numDir++;
        i++;
    }
    if (numDir == 0)  {
		opserr << "WARNING no directions specified\n";
		opserr << "expElement zeroLength element: " << tag << endln;
		return TCL_ERROR;
	}
    // create the ID array to hold the direction IDs
    ID theDirIDs(numDir);
    // fill in the directions
    for (i=0; i<numDir; i++)  {
        if (Tcl_GetInt(interp, argv[argi], &dirID) != TCL_OK)  {
            opserr << "WARNING invalid direction ID\n";
            opserr << "expElement zeroLength element: " << tag << endln;	    
            return TCL_ERROR;
        }
        theDirIDs(i) = dirID-1;
        argi++;
    }
    if (strcmp(argv[argi], "-site") == 0)  {
        argi++;
	    if (Tcl_GetInt(interp, argv[argi], &siteTag) != TCL_OK)  {
		    opserr << "WARNING invalid siteTag\n";
		    opserr << "expElement zeroLength element: " << tag << endln;
		    return TCL_ERROR;
	    }
        argi++;
	    theSite = getExperimentalSite(siteTag);
	    if (theSite == 0)  {
		    opserr << "WARNING experimental site not found\n";
		    opserr << "expSite: " << siteTag << endln;
		    opserr << "expElement zeroLength element: " << tag << endln;
		    return TCL_ERROR;
	    }
    }
    else if (strcmp(argv[argi], "-server") == 0)  {
        argi++;
        if (Tcl_GetInt(interp, argv[argi], &ipPort) != TCL_OK)  {
	        opserr << "WARNING invalid ipPort\n";
	        opserr << "expElement zeroLength element: " << tag << endln;
	        return TCL_ERROR;
        }
        argi++;
        if (strcmp(argv[argi], "-initStif") != 0 &&
            strcmp(argv[argi], "-dataSize") != 0)  {
            ipAddr = (char *)malloc((strlen(argv[argi]) + 1)*sizeof(char));
            strcpy(ipAddr,argv[argi]);
            argi++;
        }
        else  {
            ipAddr = (char *)malloc((9 + 1)*sizeof(char));
            strcpy(ipAddr,"127.0.0.1");
        }
        for (i = argi; i < argc; i++)  {
            if (strcmp(argv[i], "-dataSize") == 0)  {
                if (Tcl_GetInt(interp, argv[i+1], &dataSize) != TCL_OK)  {
		            opserr << "WARNING invalid dataSize\n";
		            opserr << "expElement zeroLength element: " << tag << endln;
		            return TCL_ERROR;
	            }
            }
        }
    }
    else  {
        opserr << "WARNING expecting -site or -server string but got ";
        opserr << argv[argi] << endln;
	    opserr << "expElement zeroLength element: " << tag << endln;
        return TCL_ERROR;
    }
    // check for optional arguments
    Vector x(3); x(0) = 1.0; x(1) = 0.0; x(2) = 0.0;
    Vector y(3); y(0) = 0.0; y(1) = 1.0; y(2) = 0.0;
    for (i = argi; i < argc; i++)  {
        if (strcmp(argv[i],"-orient") == 0)  {
            if (argc-1 < i+6)  {
                opserr << "WARNING not enough paramaters after -orient flag\n";
                opserr << "expElement zeroLength element: " << tag << endln;
                return TCL_ERROR;		
            }
            double value;
            // read the x values
            for (j=0; j<3; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &value) != TCL_OK)  {
                    opserr << "WARNING invalid -orient value\n";
                    opserr << "expElement zeroLength element: " << tag << endln;
                    return TCL_ERROR;
                }
                x(j) = value;
            }
            // read the y values
            for (j=0; j<3; j++)  {
                if (Tcl_GetDouble(interp, argv[i+4+j], &value) != TCL_OK)  {
                    opserr << "WARNING invalid -orient value\n";
                    opserr << "expElement zeroLength element: " << tag << endln;
                    return TCL_ERROR;
                }
                y(j) = value;		
            }
        }
    }
    for (i = argi; i < argc; i++)  {
        if (strcmp(argv[i], "-iMod") == 0)  {
            iMod = true;
        }
    }
	for (i = argi; i < argc; i++)  {
		if (i+1 < argc && strcmp(argv[i], "-mass") == 0)  {
			if (Tcl_GetDouble(interp, argv[i+1], &mass) != TCL_OK)  {
				opserr << "WARNING invalid mass\n";
				opserr << "expElement zeroLength element: " << tag << endln;
				return TCL_ERROR;
			}
		}
	}
    // now create the EEZeroLength
    if (theSite != 0)
        theExpElement = new EEZeroLength(tag, ndm, iNode, jNode, theDirIDs, x, y, theSite, iMod, mass);
    else
        theExpElement = new EEZeroLength(tag, ndm, iNode, jNode, theDirIDs, x, y, ipPort, ipAddr, dataSize, iMod, mass);
    
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
	for (i = argi; i < argc; i++)  {
		if (strcmp(argv[i], "-initStif") == 0)  {
			if (argc-1 < i+numDir*numDir)  {
				opserr << "WARNING incorrect number of inital stiffness terms\n";
				opserr << "expElement zeroLength element: " << tag << endln;
				return TCL_ERROR;      
			}
			Matrix theInitStif(numDir,numDir);
			double stif;
			for (j=0; j<numDir; j++)  {
				for (k=0; k<numDir; k++)  {
					if (Tcl_GetDouble(interp, argv[i+1 + numDir*j+k], &stif) != TCL_OK)  {
						opserr << "WARNING invalid initial stiffness term\n";
						opserr << "expElement zeroLength element: " << tag << endln;
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
