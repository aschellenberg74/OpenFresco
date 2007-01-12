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
// for the EEChevronBrace element.

#include <TclModelBuilder.h>

#include <stdlib.h>
#include <string.h>
#include <Domain.h>

#include <EEChevronBrace2d.h>

extern void printCommand(int argc, TCL_Char **argv);
extern ExperimentalSite *getExperimentalSite(int tag);


int addEEChevronBrace(ClientData clientData, Tcl_Interp *interp, int argc, 
    TCL_Char **argv, Domain *theTclDomain, TclModelBuilder *theTclBuilder,
    int eleArgStart)
{
	// ensure the destructor has not been called
	if (theTclBuilder == 0) {
		opserr << "WARNING builder has been destroyed - expElement chevronBrace\n";    
		return TCL_ERROR;
	}
	
	ExperimentalElement *theExpElement = 0;
	int ndm = theTclBuilder->getNDM();
	int ndf = theTclBuilder->getNDF();
	int tag;
	
	if (ndm == 2)  {
		// check plane frame problem has 3 dof per node
		if (ndf != 3)  {
			opserr << "WARNING invalid ndf: " << ndf;
			opserr << ", for plane problem need 3 - expElement chevronBrace \n";    
			return TCL_ERROR;
		} 
		
		// check the number of arguments
		if ((argc-eleArgStart) < 17)  {
			opserr << "WARNING insufficient arguments\n";
			printCommand(argc, argv);
			opserr << "Want: expElement chevronBrace eleTag iNode jNode kNode -site siteTag -initStif Kij <-iMod> <-nlGeom> <-rho1 rho1> <-rho2 rho2>\n";
			opserr << "  or: expElement chevronBrace eleTag iNode jNode kNode -server ipPort <ipAddr> <-dataSize size> -initStif Kij <-iMod> <-nlGeom> <-rho1 rho1> <-rho2 rho2>\n";
			return TCL_ERROR;
		}    
		
		// get the id and end nodes
		int iNode, jNode, kNode, siteTag, ipPort, i, j, k;
        ExperimentalSite *theSite = 0;
        char *ipAddr = 0;
        int dataSize = OF_Network_dataSize;
        bool iMod = false;
        bool nlGeom = false;
		double rho1 = 0.0;
		double rho2 = 0.0;
		
		if (Tcl_GetInt(interp, argv[1+eleArgStart], &tag) != TCL_OK)  {
			opserr << "WARNING invalid expElement chevronBrace eleTag\n";
			return TCL_ERROR;
		}
		if (Tcl_GetInt(interp, argv[2+eleArgStart], &iNode) != TCL_OK)  {
			opserr << "WARNING invalid iNode\n";
			opserr << "expElement chevronBrace element: " << tag << endln;
			return TCL_ERROR;
		}
		if (Tcl_GetInt(interp, argv[3+eleArgStart], &jNode) != TCL_OK)  {
			opserr << "WARNING invalid jNode\n";
			opserr << "expElement chevronBrace element: " << tag << endln;
			return TCL_ERROR;
		}
		if (Tcl_GetInt(interp, argv[4+eleArgStart], &kNode) != TCL_OK)  {
			opserr << "WARNING invalid kNode\n";
			opserr << "expElement chevronBrace element: " << tag << endln;
			return TCL_ERROR;
		}		
        if (strcmp(argv[5+eleArgStart], "-site") == 0)  {
		    if (Tcl_GetInt(interp, argv[6+eleArgStart], &siteTag) != TCL_OK)  {
			    opserr << "WARNING invalid siteTag\n";
			    opserr << "expElement chevronBrace element: " << tag << endln;
			    return TCL_ERROR;
		    }
		    theSite = getExperimentalSite(siteTag);
		    if (theSite == 0)  {
			    opserr << "WARNING experimental site not found\n";
			    opserr << "expSite: " << siteTag << endln;
			    opserr << "expElement chevronBrace element: " << tag << endln;
			    return TCL_ERROR;
		    }
        }
        else if (strcmp(argv[5+eleArgStart], "-server") == 0)  {
            if (Tcl_GetInt(interp, argv[6+eleArgStart], &ipPort) != TCL_OK)  {
		        opserr << "WARNING invalid ipPort\n";
		        opserr << "expElement chevronBrace element: " << tag << endln;
		        return TCL_ERROR;
	        }
            if (strcmp(argv[7+eleArgStart], "-initStif") != 0 &&
                strcmp(argv[7+eleArgStart], "-dataSize") != 0)  {
                ipAddr = (char *)malloc((strlen(argv[7+eleArgStart]) + 1)*sizeof(char));
                strcpy(ipAddr,argv[7+eleArgStart]);
            }
            else  {
                ipAddr = (char *)malloc((9 + 1)*sizeof(char));
                strcpy(ipAddr,"127.0.0.1");
            }
            for (i = 7+eleArgStart; i < argc; i++)  {
                if (strcmp(argv[i], "-dataSize") == 0)  {
                    if (Tcl_GetInt(interp, argv[i+1], &dataSize) != TCL_OK)  {
		                opserr << "WARNING invalid dataSize\n";
		                opserr << "expElement chevronBrace element: " << tag << endln;
		                return TCL_ERROR;
	                }
                }
            }
        }
        else  {
            opserr << "WARNING expecting -site or -server string but got ";
            opserr << argv[5+eleArgStart] << endln;
		    opserr << "expElement chevronBrace element: " << tag << endln;
            return TCL_ERROR;
        }
        for (i = 7+eleArgStart; i < argc; i++)  {
			if (strcmp(argv[i], "-iMod") == 0)  {
                iMod = true;
			}
		}
		for (i = 7+eleArgStart; i < argc; i++)  {
			if (strcmp(argv[i], "-nlGeom") == 0)  {
                nlGeom = true;
			}
		}
		for (i = 7+eleArgStart; i < argc; i++)  {
			if (i+1 < argc && strcmp(argv[i], "-rho1") == 0)  {
				if (Tcl_GetDouble(interp, argv[i+1], &rho1) != TCL_OK)  {
					opserr << "WARNING invalid rho1\n";
					opserr << "expElement chevronBrace element: " << tag << endln;
					return TCL_ERROR;
				}
			}
			if (i+1 < argc && strcmp(argv[i], "-rho2") == 0)  {
				if (Tcl_GetDouble(interp, argv[i+1], &rho2) != TCL_OK)  {
					opserr << "WARNING invalid rho2\n";
					opserr << "expElement chevronBrace element: " << tag << endln;
					return TCL_ERROR;
				}
			}
		}

		// now create the EEChevronBrace and add it to the Domain
        if (theSite != 0)
		    theExpElement = new EEChevronBrace2d(tag, iNode, jNode, kNode, theSite, iMod, nlGeom, rho1, rho2);
        else
		    theExpElement = new EEChevronBrace2d(tag, iNode, jNode, kNode, ipPort, ipAddr, dataSize, iMod, nlGeom, rho1, rho2);
		
		if (theExpElement == 0) {
			opserr << "WARNING ran out of memory creating element\n";
			opserr << "expElement chevronBrace element:" << tag << endln;
			return TCL_ERROR;
		}

		// then add the EEChevronBrace to the domain
		if (theTclDomain->addElement(theExpElement) == false)  {
			opserr << "WARNING could not add element to domain\n";
			opserr << "expElement chevronBrace element: " << tag << endln;
			delete theExpElement; // clean up the memory to avoid leaks
			return TCL_ERROR;
		}

		// finally check for initial stiffness terms
		for (i = 7+eleArgStart; i < argc; i++)  {
			if (strcmp(argv[i], "-initStif") == 0)  {
				if (argc-1 < i+9)  {
					opserr << "WARNING incorrect number of inital stiffness terms\n";
					opserr << "expElement chevronBrace element: " << tag << endln;
					return TCL_ERROR;      
				}
				Matrix theInitStif(3,3);
				double stif;
				for (j=0; j<3; j++)  {
					for (k=0; k<3; k++)  {
						if (Tcl_GetDouble(interp, argv[i+1 + 3*j+k], &stif) != TCL_OK)  {
							opserr << "WARNING invalid initial stiffness term\n";
							opserr << "expElement chevronBrace element: " << tag << endln;
							return TCL_ERROR;
						}
						theInitStif(j,k) = stif;
					}
				}
				theExpElement->setInitialStiff(theInitStif);
			}
		}
	}
	
	else if (ndm == 3)  {
		// not implemented yet
		opserr << "WARNING expElement chevronBrace command not implemented yet for ndm = 3\n";
		return TCL_ERROR;
	}
	
	else  {
		opserr << "WARNING expElement chevronBrace command only works when ndm is 2 or 3, ndm: ";
		opserr << ndm << endln;
		return TCL_ERROR;
	}
	
	// if get here we have sucessfully created the EEChevronBrace and added it to the domain
	return TCL_OK;
}
