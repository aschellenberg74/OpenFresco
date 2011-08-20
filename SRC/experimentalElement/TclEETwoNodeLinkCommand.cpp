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
// $URL$

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 09/06
// Revision: A
//
// Description: This file contains the function to parse the TCL input
// for the EETwoNodeLink element.

#include <stdlib.h>
#include <string.h>
#include <tcl.h>
#include <Domain.h>
#include <ID.h>
#include <Vector.h>
#include <elementAPI.h>

#include <EETwoNodeLink.h>

extern ExperimentalSite *getExperimentalSite(int tag);


static void printCommand(int argc, TCL_Char **argv)
{
    opserr << "Input command: ";
    for (int i=0; i<argc; i++)
        opserr << argv[i] << " ";
    opserr << endln;
} 


int addEETwoNodeLink(ClientData clientData, Tcl_Interp *interp, int argc,
    TCL_Char **argv, Domain *theTclDomain, int eleArgStart)
{    
    ExperimentalElement *theExpElement = 0;
    int ndm = OPS_GetNDM();
    int ndf = OPS_GetNDF();
    
    // check the number of arguments is correct
    if ((argc-eleArgStart) < 10)  {
        opserr << "WARNING insufficient arguments\n";
        printCommand(argc, argv);
        opserr << "Want: expElement twoNodeLink eleTag iNode jNode -dir dirs -site siteTag -initStif Kij <-orient <x1 x2 x3> y1 y2 y3> <-pDelta Mratios> <-shearDist sDratios> <-iMod> <-mass m>\n";
        opserr << "  or: expElement twoNodeLink eleTag iNode jNode -dir dirs -server ipPort <ipAddr> <-ssl> <-dataSize size> -initStif Kij <-orient <x1 x2 x3> y1 y2 y3> <-pDelta Mratios> <-shearDist sDratios> <-iMod> <-mass m>\n";
        return TCL_ERROR;
    }    
    
    // get the id and end nodes 
    int tag, iNode, jNode, siteTag, numDir, dirID, ipPort, argi, i, j, k;
    ExperimentalSite *theSite = 0;
    char *ipAddr = 0;
    int ssl = 0, udp = 0;
    int dataSize = OF_Network_dataSize;
    Vector Mratio(0), shearDistI(0);
    bool iMod = false;
	double mass = 0.0;
    
    if (Tcl_GetInt(interp, argv[1+eleArgStart], &tag) != TCL_OK)  {
        opserr << "WARNING invalid expElement twoNodeLink eleTag\n";
        return TCL_ERROR;
    }
    if (Tcl_GetInt(interp, argv[2+eleArgStart], &iNode) != TCL_OK)  {
        opserr << "WARNING invalid iNode\n";
        opserr << "expElement twoNodeLink element: " << tag << endln;
        return TCL_ERROR;
    }
    if (Tcl_GetInt(interp, argv[3+eleArgStart], &jNode) != TCL_OK)  {
        opserr << "WARNING invalid jNode\n";
        opserr << "expElement twoNodeLink element: " << tag << endln;
        return TCL_ERROR;
    }
    // read the number of directions
    numDir = 0;
    if (strcmp(argv[4+eleArgStart],"-dir") != 0)  {
        opserr << "WARNING expecting -dir flag\n";
        opserr << "expElement twoNodeLink element: " << tag << endln;
        return TCL_ERROR;	
    }
    argi = 5+eleArgStart;
    i = argi;
    while (i < argc &&
        strcmp(argv[i],"-site") != 0 && 
        strcmp(argv[i],"-server") != 0)  {
        numDir++;
        i++;
    }
    if (numDir == 0)  {
		opserr << "WARNING no directions specified\n";
		opserr << "expElement twoNodeLink element: " << tag << endln;
		return TCL_ERROR;
	}
    // create the ID array to hold the direction IDs
    ID theDirIDs(numDir);
    // fill in the directions
    for (i=0; i<numDir; i++)  {
        if (Tcl_GetInt(interp, argv[argi], &dirID) != TCL_OK)  {
            opserr << "WARNING invalid direction ID\n";
            opserr << "expElement twoNodeLink element: " << tag << endln;	    
            return TCL_ERROR;
        }
        if (dirID < 1 || dirID > ndf)  {
            opserr << "WARNING invalid direction ID: ";
            opserr << "dir = " << dirID << " > ndf = " << ndf;
            opserr << "\nexpElement twoNodeLink element: " << tag << endln;	    
            return TCL_ERROR;
        }
        theDirIDs(i) = dirID-1;
        argi++;
    }
    if (strcmp(argv[argi], "-site") == 0)  {
        argi++;
	    if (Tcl_GetInt(interp, argv[argi], &siteTag) != TCL_OK)  {
		    opserr << "WARNING invalid siteTag\n";
		    opserr << "expElement twoNodeLink element: " << tag << endln;
		    return TCL_ERROR;
	    }
        argi++;
	    theSite = getExperimentalSite(siteTag);
	    if (theSite == 0)  {
		    opserr << "WARNING experimental site not found\n";
		    opserr << "expSite: " << siteTag << endln;
		    opserr << "expElement twoNodeLink element: " << tag << endln;
		    return TCL_ERROR;
	    }
    }
    else if (strcmp(argv[argi], "-server") == 0)  {
        argi++;
        if (Tcl_GetInt(interp, argv[argi], &ipPort) != TCL_OK)  {
	        opserr << "WARNING invalid ipPort\n";
	        opserr << "expElement twoNodeLink element: " << tag << endln;
	        return TCL_ERROR;
        }
        argi++;
        if (strcmp(argv[argi], "-initStif") != 0 &&
            strcmp(argv[argi], "-ssl") != 0 &&
            strcmp(argv[argi], "-udp") != 0 &&
            strcmp(argv[argi], "-dataSize") != 0)  {
            ipAddr = new char [strlen(argv[argi])+1];
            strcpy(ipAddr,argv[argi]);
            argi++;
        }
        else  {
            ipAddr = new char [9+1];
            strcpy(ipAddr,"127.0.0.1");
        }
        for (i = argi; i < argc; i++)  {
            if (strcmp(argv[i], "-ssl") == 0)
                ssl = 1;
            else if (strcmp(argv[i], "-udp") == 0)
                udp = 1;
            else if (strcmp(argv[i], "-dataSize") == 0)  {
                if (Tcl_GetInt(interp, argv[i+1], &dataSize) != TCL_OK)  {
		            opserr << "WARNING invalid dataSize\n";
		            opserr << "expElement twoNodeLink element: " << tag << endln;
		            return TCL_ERROR;
	            }
            }
        }
    }
    else  {
        opserr << "WARNING expecting -site or -server string but got ";
        opserr << argv[argi] << endln;
	    opserr << "expElement twoNodeLink element: " << tag << endln;
        return TCL_ERROR;
    }
    // check for optional arguments
    Vector x(0), y(0);
    for (i = argi; i < argc; i++)  {
        if (strcmp(argv[i],"-orient") == 0)  {
            int j = i+1;
            int numOrient = 0;
            while (j < argc &&
                strcmp(argv[j],"-pDelta") != 0 &&
                strcmp(argv[j],"-shearDist") != 0 &&
                strcmp(argv[j],"-iMod") != 0 &&
                strcmp(argv[j],"-mass") != 0)  {
                numOrient++;
                j++;
            }
            if (numOrient == 3)  {
                y.resize(3);
                double value;
                // read the y values
                for (j=0; j<3; j++)  {
                    if (Tcl_GetDouble(interp, argv[i+1+j], &value) != TCL_OK)  {
                        opserr << "WARNING invalid -orient value\n";
                        opserr << "expElement twoNodeLink element: " << tag << endln;
                        return TCL_ERROR;
                    } else  {
                        y(j) = value;
                    }
                }
            }
            else if (numOrient == 6)  {
                x.resize(3);
                y.resize(3);
                double value;
                // read the x values
                for (j=0; j<3; j++)  {
                    if (Tcl_GetDouble(interp, argv[i+1+j], &value) != TCL_OK)  {
                        opserr << "WARNING invalid -orient value\n";
                        opserr << "expElement twoNodeLink element: " << tag << endln;
                        return TCL_ERROR;
                    } else  {
                        x(j) = value;
                    }
                }
                // read the y values
                for (j=0; j<3; j++)  {
                    if (Tcl_GetDouble(interp, argv[i+4+j], &value) != TCL_OK)  {
                        opserr << "WARNING invalid -orient value\n";
                        opserr << "expElement twoNodeLink element: " << tag << endln;
                        return TCL_ERROR;
                    } else  {
                        y(j) = value;		
                    }
                }
            }
            else  {
                opserr << "WARNING insufficient arguments after -orient flag\n";
                opserr << "expElement twoNodeLink element: " << tag << endln;
                return TCL_ERROR;
            }
        }
    }
	for (i=argi; i<argc; i++)  {
		if (i+1 < argc && strcmp(argv[i], "-pDelta") == 0)  {
            double Mr;
            Mratio.resize(4);
            if (ndm == 2)  {
                Mratio.Zero();
                for (j=0; j<2; j++)  {
			        if (Tcl_GetDouble(interp, argv[i+1+j], &Mr) != TCL_OK)  {
				        opserr << "WARNING invalid -pDelta value\n";
				        opserr << "expElement twoNodeLink element: " << tag << endln;
				        return TCL_ERROR;
                    }
                    Mratio(2+j) = Mr;
                }
            } else if (ndm == 3)  {
                for (j=0; j<4; j++)  {
			        if (Tcl_GetDouble(interp, argv[i+1+j], &Mr) != TCL_OK)  {
				        opserr << "WARNING invalid -pDelta value\n";
				        opserr << "expElement twoNodeLink element: " << tag << endln;
				        return TCL_ERROR;
                    }
                    Mratio(j) = Mr;
                }
            }
		}
	}
	for (i=argi; i<argc; i++)  {
		if (i+1 < argc && strcmp(argv[i], "-shearDist") == 0)  {
            double sDI;
            shearDistI.resize(2);
            if (ndm == 2)  {
                if (Tcl_GetDouble(interp, argv[i+1], &sDI) != TCL_OK)  {
                    opserr << "WARNING invalid -shearDist value\n";
                    opserr << "expElement twoNodeLink element: " << tag << endln;
                    return TCL_ERROR;
                }
                shearDistI(0) = sDI;
                shearDistI(1) = 0.5;
            } else if (ndm == 3)  {
                for (j=0; j<2; j++)  {
			        if (Tcl_GetDouble(interp, argv[i+1+j], &sDI) != TCL_OK)  {
				        opserr << "WARNING invalid -shearDist value\n";
				        opserr << "expElement twoNodeLink element: " << tag << endln;
				        return TCL_ERROR;
                    }
                    shearDistI(j) = sDI;
                }
            }
		}
	}
    for (i=argi; i<argc; i++)  {
        if (strcmp(argv[i], "-iMod") == 0)  {
            iMod = true;
        }
    }
	for (i=argi; i<argc; i++)  {
		if (i+1 < argc && strcmp(argv[i], "-mass") == 0)  {
			if (Tcl_GetDouble(interp, argv[i+1], &mass) != TCL_OK)  {
				opserr << "WARNING invalid -mass value\n";
				opserr << "expElement twoNodeLink element: " << tag << endln;
				return TCL_ERROR;
			}
		}
	}
    // now create the EETwoNodeLink
    if (theSite != 0)
        theExpElement = new EETwoNodeLink(tag, ndm, iNode, jNode, theDirIDs, theSite, y, x, Mratio, shearDistI, iMod, mass);
    else
        theExpElement = new EETwoNodeLink(tag, ndm, iNode, jNode, theDirIDs, ipPort, ipAddr, ssl, udp, dataSize, y, x, Mratio, shearDistI, iMod, mass);
    
	if (theExpElement == 0)  {
		opserr << "WARNING ran out of memory creating element\n";
		opserr << "expElement twoNodeLink element: " << tag << endln;
		return TCL_ERROR;
	}
    
	// then add the EETwoNodeLink to the domain
	if (theTclDomain->addElement(theExpElement) == false)  {
		opserr << "WARNING could not add element to the domain\n";
		opserr << "expElement twoNodeLink element: " << tag << endln;
		delete theExpElement;
		return TCL_ERROR;
	}
    
	// finally check for initial stiffness terms
    int setInitStif = -1;
	for (i = argi; i < argc; i++)  {
		if (strcmp(argv[i], "-initStif") == 0)  {
			if (argc-1 < i+numDir*numDir)  {
				opserr << "WARNING incorrect number of inital stiffness terms\n";
				opserr << "expElement twoNodeLink element: " << tag << endln;
				return TCL_ERROR;      
			}
			Matrix theInitStif(numDir,numDir);
			double stif;
			for (j=0; j<numDir; j++)  {
				for (k=0; k<numDir; k++)  {
					if (Tcl_GetDouble(interp, argv[i+1 + numDir*j+k], &stif) != TCL_OK)  {
						opserr << "WARNING invalid initial stiffness term\n";
						opserr << "expElement twoNodeLink element: " << tag << endln;
						return TCL_ERROR;
					}
					theInitStif(j,k) = stif;
				}
			}
			setInitStif = theExpElement->setInitialStiff(theInitStif);
		}
	}
    if (setInitStif != 0)  {
        opserr << "WARNING initial stiffness not set\n";
        opserr << "expElement twoNodeLink element: " << tag << endln;
        return TCL_ERROR;
    }
    
    // if get here we have sucessfully created the EETwoNodeLink and added it to the domain
	return TCL_OK;
}
