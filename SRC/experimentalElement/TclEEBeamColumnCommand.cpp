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
// for the EEBeamColumn element.

#include <stdlib.h>
#include <string.h>
#include <tcl.h>
#include <Domain.h>
#include <elementAPI.h>

#include <EEBeamColumn2d.h>
#include <EEBeamColumn3d.h>

extern ExperimentalSite *getExperimentalSite(int tag);


static void printCommand(int argc, TCL_Char **argv)
{
    opserr << "Input command: ";
    for (int i=0; i<argc; i++)
        opserr << argv[i] << " ";
    opserr << endln;
} 


int addEEBeamColumn(ClientData clientData, Tcl_Interp *interp, int argc, 
    TCL_Char **argv, Domain *theTclDomain, int eleArgStart)
{
	ExperimentalElement *theExpElement = 0;
	int ndm = OPS_GetNDM();
	int ndf = OPS_GetNDF();
	int tag;
	
	if (ndm == 2)  {
		// check plane frame problem has 3 dof per node
		if (ndf != 3)  {
			opserr << "WARNING invalid ndf: " << ndf;
			opserr << ", for plane problem need 3 - expElement beamColumn \n";    
			return TCL_ERROR;
		} 
		
		// check the number of arguments
		if ((argc-eleArgStart) < 17)  {
			opserr << "WARNING insufficient arguments\n";
			printCommand(argc, argv);
			opserr << "Want: expElement beamColumn eleTag iNode jNode transTag -site siteTag -initStif Kij <-iMod> <-rho rho>\n";
			opserr << "  or: expElement beamColumn eleTag iNode jNode transTag -server ipPort <ipAddr> <-ssl> <-dataSize size> -initStif Kij <-iMod> <-rho rho>\n";
			return TCL_ERROR;
		}
		
		// get the id and end nodes
		int iNode, jNode, transId, siteTag, ipPort, i, j, k;
        CrdTransf *theTrans = 0;
        ExperimentalSite *theSite = 0;
        char *ipAddr = 0;
        int ssl = 0;
        int dataSize = OF_Network_dataSize;
        bool iMod = false;
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
		if (Tcl_GetInt(interp, argv[4+eleArgStart], &transId) != TCL_OK)  {
			opserr << "WARNING invalid transTag\n";
			opserr << "expElement beamColumn element: " << tag << endln;
			return TCL_ERROR;
		}
		theTrans = OPS_GetCrdTransfPtr(transId);
		if (theTrans == 0)  {
			opserr << "WARNING transformation object not found\n";
	        opserr << "expElement beamColumn element: " << tag << endln;
			return TCL_ERROR;
		}
        if (strcmp(argv[5+eleArgStart], "-site") == 0)  {
		    if (Tcl_GetInt(interp, argv[6+eleArgStart], &siteTag) != TCL_OK)  {
			    opserr << "WARNING invalid siteTag\n";
			    opserr << "expElement beamColumn element: " << tag << endln;
			    return TCL_ERROR;
		    }
		    theSite = getExperimentalSite(siteTag);
		    if (theSite == 0)  {
			    opserr << "WARNING experimental site not found\n";
			    opserr << "expSite: " << siteTag << endln;
			    opserr << "expElement beamColumn element: " << tag << endln;
			    return TCL_ERROR;
		    }
        }
        else if (strcmp(argv[5+eleArgStart], "-server") == 0)  {
            if (Tcl_GetInt(interp, argv[6+eleArgStart], &ipPort) != TCL_OK)  {
		        opserr << "WARNING invalid ipPort\n";
		        opserr << "expElement beamColumn element: " << tag << endln;
		        return TCL_ERROR;
	        }
            if (strcmp(argv[7+eleArgStart], "-initStif") != 0  &&
                strcmp(argv[7+eleArgStart], "-ssl") != 0  &&
                strcmp(argv[7+eleArgStart], "-dataSize") != 0)  {
                ipAddr = new char [strlen(argv[7+eleArgStart])+1];
                strcpy(ipAddr,argv[7+eleArgStart]);
            }
            else  {
                ipAddr = new char [9+1];
                strcpy(ipAddr,"127.0.0.1");
            }
            for (i = 7+eleArgStart; i < argc; i++)  {
                if (strcmp(argv[i], "-ssl") == 0)
                    ssl = 1;
                else if (strcmp(argv[i], "-dataSize") == 0)  {
                    if (Tcl_GetInt(interp, argv[i+1], &dataSize) != TCL_OK)  {
		                opserr << "WARNING invalid dataSize\n";
		                opserr << "expElement beamColumn element: " << tag << endln;
		                return TCL_ERROR;
	                }
                }
            }
        }
        else  {
            opserr << "WARNING expecting -site or -server string but got ";
            opserr << argv[5+eleArgStart] << endln;
		    opserr << "expElement beamColumn element: " << tag << endln;
            return TCL_ERROR;
        }
		for (i = 7+eleArgStart; i < argc; i++)  {
			if (strcmp(argv[i], "-iMod") == 0)  {
                iMod = true;
			}
		}
		for (i = 7+eleArgStart; i < argc; i++)  {
			if (i+1 < argc && strcmp(argv[i], "-rho") == 0)  {
				if (Tcl_GetDouble(interp, argv[i+1], &rho) != TCL_OK)  {
					opserr << "WARNING invalid rho\n";
					opserr << "expElement beamColumn element: " << tag << endln;
					return TCL_ERROR;
				}
			}
		}
        
		// now create the EEBeamColumn
        if (theSite != 0)
		    theExpElement = new EEBeamColumn2d(tag, iNode, jNode, *theTrans, theSite, iMod, rho);
        else
		    theExpElement = new EEBeamColumn2d(tag, iNode, jNode, *theTrans, ipPort, ipAddr, ssl, dataSize, iMod, rho);
		
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
        int setInitStif = -1;
		for (i = 7+eleArgStart; i < argc; i++)  {
			if (strcmp(argv[i], "-initStif") == 0)  {
				if (argc-1 < i+9)  {
					opserr << "WARNING incorrect number of inital stiffness terms\n";
					opserr << "expElement beamColumn element: " << tag << endln;
					return TCL_ERROR;      
				}
				Matrix theInitStif(3,3);
				double stif;
				for (j=0; j<3; j++)  {
					for (k=0; k<3; k++)  {
						if (Tcl_GetDouble(interp, argv[i+1 + 3*j+k], &stif) != TCL_OK)  {
							opserr << "WARNING invalid initial stiffness term\n";
							opserr << "expElement beamColumn element: " << tag << endln;
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
            opserr << "expElement beamColumn element: " << tag << endln;
            return TCL_ERROR;
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
        if ((argc-eleArgStart) < 44)  {
			opserr << "WARNING insufficient arguments\n";
			printCommand(argc, argv);
			opserr << "Want: expElement beamColumn eleTag iNode jNode transTag -site siteTag -initStif Kij <-iMod> <-rho rho>\n";
			opserr << "  or: expElement beamColumn eleTag iNode jNode transTag -server ipPort <ipAddr> <-ssl> <-dataSize size> -initStif Kij <-iMod> <-rho rho>\n";
			return TCL_ERROR;
		}
		
		// get the id and end nodes
		int iNode, jNode, transId, siteTag, ipPort, i, j, k;
        CrdTransf *theTrans = 0;
        ExperimentalSite *theSite = 0;
        char *ipAddr = 0;
        int ssl = 0;
        int dataSize = OF_Network_dataSize;
        bool iMod = false;
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
		if (Tcl_GetInt(interp, argv[4+eleArgStart], &transId) != TCL_OK)  {
			opserr << "WARNING invalid transTag\n";
			opserr << "expElement beamColumn element: " << tag << endln;
			return TCL_ERROR;
		}
		theTrans = OPS_GetCrdTransfPtr(transId);
		if (theTrans == 0)  {
			opserr << "WARNING transformation object not found\n";
	        opserr << "expElement beamColumn element: " << tag << endln;
			return TCL_ERROR;
		}
        if (strcmp(argv[5+eleArgStart], "-site") == 0)  {
		    if (Tcl_GetInt(interp, argv[6+eleArgStart], &siteTag) != TCL_OK)  {
			    opserr << "WARNING invalid siteTag\n";
			    opserr << "expElement beamColumn element: " << tag << endln;
			    return TCL_ERROR;
		    }
		    theSite = getExperimentalSite(siteTag);
		    if (theSite == 0)  {
			    opserr << "WARNING experimental site not found\n";
			    opserr << "expSite: " << siteTag << endln;
			    opserr << "expElement beamColumn element: " << tag << endln;
			    return TCL_ERROR;
		    }
        }
        else if (strcmp(argv[5+eleArgStart], "-server") == 0)  {
            if (Tcl_GetInt(interp, argv[6+eleArgStart], &ipPort) != TCL_OK)  {
		        opserr << "WARNING invalid ipPort\n";
		        opserr << "expElement beamColumn element: " << tag << endln;
		        return TCL_ERROR;
	        }
            if (strcmp(argv[7+eleArgStart], "-initStif") != 0 &&
                strcmp(argv[7+eleArgStart], "-ssl") != 0 &&
                strcmp(argv[7+eleArgStart], "-dataSize") != 0)  {
                ipAddr = new char [strlen(argv[7+eleArgStart])+1];
                strcpy(ipAddr,argv[7+eleArgStart]);
            }
            else  {
                ipAddr = new char [9+1];
                strcpy(ipAddr,"127.0.0.1");
            }
            for (i = 7+eleArgStart; i < argc; i++)  {
                if (strcmp(argv[i], "-ssl") == 0)
                    ssl = 1;
                else if (strcmp(argv[i], "-dataSize") == 0)  {
                    if (Tcl_GetInt(interp, argv[i+1], &dataSize) != TCL_OK)  {
		                opserr << "WARNING invalid dataSize\n";
		                opserr << "expElement beamColumn element: " << tag << endln;
		                return TCL_ERROR;
	                }
                }
            }
        }
        else  {
            opserr << "WARNING expecting -site or -server string but got ";
            opserr << argv[5+eleArgStart] << endln;
		    opserr << "expElement beamColumn element: " << tag << endln;
            return TCL_ERROR;
        }
        for (i = 7+eleArgStart; i < argc; i++)  {
			if (strcmp(argv[i], "-iMod") == 0)  {
                iMod = true;
			}
		}
		for (i = 7+eleArgStart; i < argc; i++)  {
			if (i+1 < argc && strcmp(argv[i], "-rho") == 0)  {
				if (Tcl_GetDouble(interp, argv[i+1], &rho) != TCL_OK)  {
					opserr << "WARNING invalid rho\n";
					opserr << "expElement beamColumn element: " << tag << endln;
					return TCL_ERROR;
				}
			}
		}

		// now create the EEBeamColumn
        if (theSite != 0)
		    theExpElement = new EEBeamColumn3d(tag, iNode, jNode, *theTrans, theSite, iMod, rho);
        else
		    theExpElement = new EEBeamColumn3d(tag, iNode, jNode, *theTrans, ipPort, ipAddr, ssl, dataSize, iMod, rho);
		
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
        int setInitStif = -1;
		for (i = 7+eleArgStart; i < argc; i++)  {
			if (strcmp(argv[i], "-initStif") == 0)  {
				if (argc-1 < i+36)  {
					opserr << "WARNING incorrect number of inital stiffness terms\n";
					opserr << "expElement beamColumn element: " << tag << endln;
					return TCL_ERROR;      
				}
				Matrix theInitStif(6,6);
				double stif;
				for (j=0; j<6; j++)  {
					for (k=0; k<6; k++)  {
						if (Tcl_GetDouble(interp, argv[i+1 + 6*j+k], &stif) != TCL_OK)  {
							opserr << "WARNING invalid initial stiffness term\n";
							opserr << "expElement beamColumn element: " << tag << endln;
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
            opserr << "expElement beamColumn element: " << tag << endln;
            return TCL_ERROR;
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
