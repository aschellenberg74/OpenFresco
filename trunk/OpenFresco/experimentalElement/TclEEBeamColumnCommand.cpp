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
// for the EEBeamColumn element.

#include <TclModelBuilder.h>

#include <stdlib.h>
#include <string.h>
#include <Domain.h>

#include <EEBeamColumn2d.h>
#include <EEBeamColumn3d.h>
#include <CrdTransf2d.h>
#include <CrdTransf3d.h>

extern void printCommand(int argc, TCL_Char **argv);
extern ExperimentalSite *getExperimentalSite(int tag);


int addEEBeamColumn(ClientData clientData, Tcl_Interp *interp, int argc, 
    TCL_Char **argv, TclModelBuilder *theTclBuilder, Domain *theTclDomain,
    int eleArgStart)
{
	// ensure the destructor has not been called
	if (theTclBuilder == 0)  {
		opserr << "WARNING builder has been destroyed - expElement beamColumn\n";    
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
			opserr << ", for plane problem need 3 - expElement beamColumn \n";    
			return TCL_ERROR;
		} 
		
		// check the number of arguments
		if ((argc-eleArgStart) < 6)  {
			opserr << "WARNING insufficient arguments\n";
			printCommand(argc, argv);
			opserr << "Want: expElement beamColumn eleTag iNode jNode siteTag transTag -initStif Kij <-iMod> <-isCopy> <-rho rho>\n";
			return TCL_ERROR;
		}    
		
		// get the id and end nodes
		int iNode, jNode, transId, siteTag;
        bool iMod = false;
        bool isCopy = false;
		double rho = 0.0;
		
		if (Tcl_GetInt(interp, argv[1+eleArgStart], &tag) != TCL_OK)  {
			opserr << "WARNING invalid expElement beamColumn eleTag\n";
			return TCL_ERROR;
		}
		if (Tcl_GetInt(interp, argv[2+eleArgStart], &iNode) != TCL_OK)  {
			opserr << "WARNING invalid iNode\n";
			opserr << "expElement beamColumn element: " << tag << endln;
			return TCL_ERROR;
		}
		if (Tcl_GetInt(interp, argv[3+eleArgStart], &jNode) != TCL_OK)  {
			opserr << "WARNING invalid jNode\n";
			opserr << "expElement beamColumn element: " << tag << endln;
			return TCL_ERROR;
		}
		if (Tcl_GetInt(interp, argv[4+eleArgStart], &siteTag) != TCL_OK)  {
			opserr << "WARNING invalid siteTag\n";
			opserr << "expElement beamColumn element: " << tag << endln;
			return TCL_ERROR;
		}
		ExperimentalSite *theSite = getExperimentalSite(siteTag);
		if (theSite == 0)  {
			opserr << "WARNING experimental site not found\n";
			opserr << "expSite: " << siteTag << endln;
			opserr << "expElement beamColumn element: " << tag << endln;
			return TCL_ERROR;
		}
		if (Tcl_GetInt(interp, argv[5+eleArgStart], &transId) != TCL_OK)  {
			opserr << "WARNING invalid transTag\n";
			opserr << "expElement beamColumn element: " << tag << endln;
			return TCL_ERROR;
		}
		CrdTransf2d *theTrans = theTclBuilder->getCrdTransf2d(transId);
		if (theTrans == 0)  {
			opserr << "WARNING transformation object not found\n";
	        opserr << "expElement beamColumn element: " << tag << endln;
			return TCL_ERROR;
		}
		int i;
		for (i = 6+eleArgStart; i < argc; i++)  {
			if (strcmp(argv[i], "-iMod") == 0)  {
                iMod = true;
			}
		}
		for (i = 6+eleArgStart; i < argc; i++)  {
			if (strcmp(argv[i], "-isCopy") == 0)  {
                isCopy = true;
			}
		}
		for (i = 6+eleArgStart; i < argc; i++)  {
			if (i+1 < argc && strcmp(argv[i], "-rho") == 0)  {
				if (Tcl_GetDouble(interp, argv[i+1], &rho) != TCL_OK)  {
					opserr << "WARNING invalid rho\n";
					opserr << "expElement beamColumn element: " << tag << endln;
					return TCL_ERROR;
				}
			}
		}

		// now create the EEBeamColumn
		theExpElement = new EEBeamColumn2d(tag, iNode, jNode, theSite, *theTrans, iMod, isCopy, rho);
		
		if (theExpElement == 0) {
			opserr << "WARNING ran out of memory creating element\n";
			opserr << "expElement beamColumn element: " << tag << endln;
			return TCL_ERROR;
		}

		// then add the EEBeamColumn to the domain
		if (theTclDomain->addElement(theExpElement) == false)  {
			opserr << "WARNING could not add element to domain\n";
			opserr << "expElement beamColumn element: " << tag << endln;
			delete theExpElement; // clean up the memory to avoid leaks
			return TCL_ERROR;
		}
		
		// finally check for initial stiffness terms
		for (i = 6+eleArgStart; i < argc; i++)  {
			if (i+1 < argc && strcmp(argv[i], "-initStif") == 0)  {
				if (argc < i+9)  {
					opserr << "WARNING incorrect number of inital stiffness terms\n";
					opserr << "expElement beamColumn element: " << tag << endln;
					return TCL_ERROR;      
				}
				Matrix theInitStif(3,3);
				double stif;
				for (int j=0; j<3; j++)  {
					for (int k=0; k<3; k++)  {
						if (Tcl_GetDouble(interp, argv[i+1+3*j+k], &stif) != TCL_OK)  {
							opserr << "WARNING invalid initial stiffness term\n";
							opserr << "expElement beamColumn element: " << tag << endln;
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
		// check space frame problem has 6 dof per node
		if (ndf != 6)  {
			opserr << "WARNING invalid ndf: " << ndf;
			opserr << ", for space problem need 6 - expElement beamColumn \n";    
			return TCL_ERROR;
		} 
		
		// check the number of arguments
		if ((argc-eleArgStart) < 6)  {
			opserr << "WARNING insufficient arguments\n";
			printCommand(argc, argv);
			opserr << "Want: expElement beamColumn eleTag iNode jNode siteTag transTag -initStif Kij <-rho rho>\n";
			return TCL_ERROR;
		}    
		
		// get the id and end nodes
		int iNode, jNode, transId, siteTag;
        bool iMod = false;
        bool isCopy = false;
		double rho = 0.0;
		
		if (Tcl_GetInt(interp, argv[1+eleArgStart], &tag) != TCL_OK)  {
			opserr << "WARNING invalid expElement beamColumn eleTag\n";
			return TCL_ERROR;
		}
		if (Tcl_GetInt(interp, argv[2+eleArgStart], &iNode) != TCL_OK)  {
			opserr << "WARNING invalid iNode\n";
			opserr << "expElement beamColumn element: " << tag << endln;
			return TCL_ERROR;
		}
		if (Tcl_GetInt(interp, argv[3+eleArgStart], &jNode) != TCL_OK)  {
			opserr << "WARNING invalid jNode\n";
			opserr << "expElement beamColumn element: " << tag << endln;
			return TCL_ERROR;
		}
		if (Tcl_GetInt(interp, argv[4+eleArgStart], &siteTag) != TCL_OK)  {
			opserr << "WARNING invalid siteTag\n";
			opserr << "expElement beamColumn element: " << tag << endln;
			return TCL_ERROR;
		}
		ExperimentalSite *theSite = getExperimentalSite(siteTag);
		if (theSite == 0)  {
			opserr << "WARNING experimental site not found\n";
			opserr << "expSite: " << siteTag << endln;
			opserr << "expElement beamColumn element: " << tag << endln;
			return TCL_ERROR;
		}
		if (Tcl_GetInt(interp, argv[5+eleArgStart], &transId) != TCL_OK)  {
			opserr << "WARNING invalid transTag\n";
			opserr << "expElement beamColumn element: " << tag << endln;
			return TCL_ERROR;
		}
		CrdTransf3d *theTrans = theTclBuilder->getCrdTransf3d(transId);
		if (theTrans == 0)  {
			opserr << "WARNING transformation object not found\n";
	        opserr << "expElement beamColumn element: " << tag << endln;
			return TCL_ERROR;
		}
		int i;
		for (i = 6+eleArgStart; i < argc; i++)  {
			if (strcmp(argv[i], "-iMod") == 0)  {
                iMod = true;
			}
		}
		for (i = 6+eleArgStart; i < argc; i++)  {
			if (strcmp(argv[i], "-isCopy") == 0)  {
                isCopy = true;
			}
		}
		for (i = 6+eleArgStart; i < argc; i++)  {
			if (i+1 < argc && strcmp(argv[i], "-rho") == 0)  {
				if (Tcl_GetDouble(interp, argv[i+1], &rho) != TCL_OK)  {
					opserr << "WARNING invalid rho\n";
					opserr << "expElement beamColumn element: " << tag << endln;
					return TCL_ERROR;
				}
			}
		}

		// now create the EEBeamColumn
		theExpElement = new EEBeamColumn3d(tag, iNode, jNode, theSite, *theTrans, iMod, isCopy, rho);
		
		if (theExpElement == 0) {
			opserr << "WARNING ran out of memory creating element\n";
			opserr << "expElement beamColumn element:" << tag << endln;
			return TCL_ERROR;
		}

		// then add the EEBeamColumn to the domain
		if (theTclDomain->addElement(theExpElement) == false)  {
			opserr << "WARNING could not add element to domain\n";
			opserr << "expElement beamColumn element: " << tag << endln;
			delete theExpElement; // clean up the memory to avoid leaks
			return TCL_ERROR;
		}
		
		// finally check for initial stiffness terms
		for (i = 6+eleArgStart; i < argc; i++)  {
			if (i+1 < argc && strcmp(argv[i], "-initStif") == 0)  {
				if (argc < i+36)  {
					opserr << "WARNING incorrect number of inital stiffness terms\n";
					opserr << "expElement beamColumn element: " << tag << endln;
					return TCL_ERROR;      
				}
				Matrix theInitStif(6,6);
				double stif;
				for (int j=0; j<6; j++)  {
					for (int k=0; k<6; k++)  {
						if (Tcl_GetDouble(interp, argv[i+1+6*j+k], &stif) != TCL_OK)  {
							opserr << "WARNING invalid initial stiffness term\n";
							opserr << "expElement beamColumn element: " << tag << endln;
							return TCL_ERROR;
						}
						theInitStif(j,k) = stif;
					}
				}
				theExpElement->setInitialStiff(theInitStif);
			}
		}
	}
	
	else  {
		opserr << "WARNING expElement beamColumn command only works when ndm is 2 or 3, ndm: ";
		opserr << ndm << endln;
		return TCL_ERROR;
	}
		
	// if get here we have sucessfully created the EEBeamColumn and added it to the domain
	return TCL_OK;
}
