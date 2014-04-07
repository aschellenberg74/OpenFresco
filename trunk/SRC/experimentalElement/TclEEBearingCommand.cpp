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
**   Andreas Schellenberg (andreas.schellenberg@gmail.com)            **
**   Yoshikazu Takahashi (yos@catfish.dpri.kyoto-u.ac.jp)             **
**   Gregory L. Fenves (fenves@berkeley.edu)                          **
**   Stephen A. Mahin (mahin@berkeley.edu)                            **
**                                                                    **
** ****************************************************************** */

// $Revision$
// $Date$
// $URL$

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 01/14
// Revision: A
//
// Description: This file contains the function to parse the TCL input
// for the EEBearing element.

#include <stdlib.h>
#include <string.h>
#include <tcl.h>
#include <Domain.h>
#include <elementAPI.h>

#include <EEBearing2d.h>
#include <EEBearing3d.h>

extern ExperimentalSite *getExperimentalSite(int tag);


static void printCommand(int argc, TCL_Char **argv)
{
    opserr << "Input command: ";
    for (int i=0; i<argc; i++)
        opserr << argv[i] << " ";
    opserr << endln;
} 


int addEEBearing(ClientData clientData, Tcl_Interp *interp, int argc, 
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
			opserr << ", for plane problem need 3 - expElement bearing \n";    
			return TCL_ERROR;
		} 
		
		// check the number of arguments
		if ((argc-eleArgStart) < 13)  {
			opserr << "WARNING insufficient arguments\n";
			printCommand(argc, argv);
			opserr << "Want: expElement bearing eleTag iNode jNode pFrcCtrl -P matTag -Mz matTag -site siteTag -initStif Kij <-orient x1 x2 x3 y1 y2 y3> <-pDelta Mratios> <-shearDist sDratio> <-iMod> <-doRayleigh> <-mass m>\n";
			opserr << "  or: expElement bearing eleTag iNode jNode pFrcCtrl -P matTag -Mz matTag -server ipPort <ipAddr> <-ssl> <-udp> <-dataSize size> -initStif Kij <-orient x1 x2 x3 y1 y2 y3> <-pDelta Mratios> <-shearDist sDratio> <-iMod> <-doRayleigh> <-mass m>\n";
			return TCL_ERROR;
		}
		
		// get the id and end nodes
		int iNode, jNode, pFrcCtrl, matTag, siteTag, ipPort, i, j, k;
        UniaxialMaterial *theMaterials[2];
        ExperimentalSite *theSite = 0;
        char *ipAddr = 0;
        int ssl = 0, udp = 0;
        int dataSize = OF_Network_dataSize;
        Vector Mratio(0);
        double shearDistI = 0.5;
        bool iMod = false;
        int doRayleigh = 0;
        double mass = 0.0;
		
		if (Tcl_GetInt(interp, argv[1+eleArgStart], &tag) != TCL_OK)  {
			opserr << "WARNING invalid expElement bearing eleTag\n";
			return TCL_ERROR;
		}
		if (Tcl_GetInt(interp, argv[2+eleArgStart], &iNode) != TCL_OK)  {
			opserr << "WARNING invalid iNode\n";
			opserr << "expElement bearing element: " << tag << endln;
			return TCL_ERROR;
		}
		if (Tcl_GetInt(interp, argv[3+eleArgStart], &jNode) != TCL_OK)  {
			opserr << "WARNING invalid jNode\n";
			opserr << "expElement bearing element: " << tag << endln;
			return TCL_ERROR;
		}
		if (Tcl_GetInt(interp, argv[4+eleArgStart], &pFrcCtrl) != TCL_OK)  {
			opserr << "WARNING invalid pFrcCtrl\n";
			opserr << "expElement bearing element: " << tag << endln;
			return TCL_ERROR;
		}
        if (strcmp(argv[5+eleArgStart], "-P") == 0)  {
		    if (Tcl_GetInt(interp, argv[6+eleArgStart], &matTag) != TCL_OK)  {
			    opserr << "WARNING invalid -P matTag\n";
			    opserr << "expElement bearing element: " << tag << endln;
			    return TCL_ERROR;
		    }
            theMaterials[0] = OPS_GetUniaxialMaterial(matTag);
            if (theMaterials[0] == 0)  {
                opserr << "WARNING material model not found\n";
                opserr << "uniaxialMaterial: " << matTag << endln;
                opserr << "expElement bearing element: " << tag << endln;
                return TCL_ERROR;
            }
        }
        if (strcmp(argv[7+eleArgStart], "-Mz") == 0)  {
		    if (Tcl_GetInt(interp, argv[8+eleArgStart], &matTag) != TCL_OK)  {
			    opserr << "WARNING invalid -Mz matTag\n";
			    opserr << "expElement bearing element: " << tag << endln;
			    return TCL_ERROR;
		    }
            theMaterials[1] = OPS_GetUniaxialMaterial(matTag);
            if (theMaterials[1] == 0)  {
                opserr << "WARNING material model not found\n";
                opserr << "uniaxialMaterial: " << matTag << endln;
                opserr << "expElement bearing element: " << tag << endln;
                return TCL_ERROR;
            }
        }
        if (strcmp(argv[9+eleArgStart], "-site") == 0)  {
		    if (Tcl_GetInt(interp, argv[10+eleArgStart], &siteTag) != TCL_OK)  {
			    opserr << "WARNING invalid siteTag\n";
			    opserr << "expElement bearing element: " << tag << endln;
			    return TCL_ERROR;
		    }
		    theSite = getExperimentalSite(siteTag);
		    if (theSite == 0)  {
			    opserr << "WARNING experimental site not found\n";
			    opserr << "expSite: " << siteTag << endln;
			    opserr << "expElement bearing element: " << tag << endln;
			    return TCL_ERROR;
		    }
        }
        else if (strcmp(argv[9+eleArgStart], "-server") == 0)  {
            if (Tcl_GetInt(interp, argv[10+eleArgStart], &ipPort) != TCL_OK)  {
		        opserr << "WARNING invalid ipPort\n";
		        opserr << "expElement bearing element: " << tag << endln;
		        return TCL_ERROR;
	        }
            if (strcmp(argv[11+eleArgStart], "-initStif") != 0  &&
                strcmp(argv[11+eleArgStart], "-ssl") != 0  &&
                strcmp(argv[11+eleArgStart], "-udp") != 0  &&
                strcmp(argv[11+eleArgStart], "-dataSize") != 0)  {
                ipAddr = new char [strlen(argv[11+eleArgStart])+1];
                strcpy(ipAddr,argv[11+eleArgStart]);
            }
            else  {
                ipAddr = new char [9+1];
                strcpy(ipAddr,"127.0.0.1");
            }
            for (i = 11+eleArgStart; i < argc; i++)  {
                if (strcmp(argv[i], "-ssl") == 0)
                    ssl = 1;
                else if (strcmp(argv[i], "-udp") == 0)
                    udp = 1;
                else if (strcmp(argv[i], "-dataSize") == 0)  {
                    if (Tcl_GetInt(interp, argv[i+1], &dataSize) != TCL_OK)  {
		                opserr << "WARNING invalid dataSize\n";
		                opserr << "expElement bearing element: " << tag << endln;
		                return TCL_ERROR;
	                }
                }
            }
        }
        else  {
            opserr << "WARNING expecting -site or -server string but got ";
            opserr << argv[9+eleArgStart] << endln;
		    opserr << "expElement bearing element: " << tag << endln;
            return TCL_ERROR;
        }
        
        // check for optional arguments
        Vector x(0), y(0);
        for (i = 11+eleArgStart; i < argc; i++)  {
            if (strcmp(argv[i],"-orient") == 0)  {
                j = i+1;
                int numOrient = 0;
                while (j < argc &&
                    strcmp(argv[j],"-pDelta") != 0 &&
                    strcmp(argv[j],"-shearDist") != 0 &&
                    strcmp(argv[j],"-iMod") != 0 &&
                    strcmp(argv[j],"-doRayleigh") != 0 &&
                    strcmp(argv[j],"-mass") != 0)  {
                        numOrient++;
                        j++;
                }
                if (numOrient == 6)  {
                    x.resize(3);
                    y.resize(3);
                    double value;
                    // read the x values
                    for (j=0; j<3; j++)  {
                        if (Tcl_GetDouble(interp, argv[i+1+j], &value) != TCL_OK)  {
                            opserr << "WARNING invalid -orient value\n";
                            opserr << "expElement bearing element: " << tag << endln;
                            return TCL_ERROR;
                        }
                        x(j) = value;
                    }
                    // read the y values
                    for (j=0; j<3; j++)  {
                        if (Tcl_GetDouble(interp, argv[i+4+j], &value) != TCL_OK)  {
                            opserr << "WARNING invalid -orient value\n";
                            opserr << "expElement bearing element: " << tag << endln;
                            return TCL_ERROR;
                        }
                        y(j) = value;
                    }
                }
                else  {
                    opserr << "WARNING insufficient arguments after -orient flag\n";
                    opserr << "expElement bearing element: " << tag << endln;
                    return TCL_ERROR;
                }
            }
        }
        for (i = 11+eleArgStart; i < argc; i++)  {
            if (i+1 < argc && strcmp(argv[i], "-pDelta") == 0)  {
                double Mr;
                Mratio.resize(2);
                Mratio.Zero();
                for (j=0; j<2; j++)  {
                    if (Tcl_GetDouble(interp, argv[i+1+j], &Mr) != TCL_OK)  {
                        opserr << "WARNING invalid -pDelta value\n";
                        opserr << "expElement bearing element: " << tag << endln;
                        return TCL_ERROR;
                    }
                    Mratio(j) = Mr;
                }
            }
        }
        for (i = 11+eleArgStart; i < argc; i++)  {
            if (i+1 < argc && strcmp(argv[i], "-shearDist") == 0)  {
                if (Tcl_GetDouble(interp, argv[i+1], &shearDistI) != TCL_OK)  {
                    opserr << "WARNING invalid -shearDist value\n";
                    opserr << "expElement bearing element: " << tag << endln;
                    return TCL_ERROR;
                }
            }
        }
		for (i = 11+eleArgStart; i < argc; i++)  {
			if (strcmp(argv[i], "-iMod") == 0)  {
                iMod = true;
			}
		}
		for (i = 11+eleArgStart; i < argc; i++)  {
			if (strcmp(argv[i], "-doRayleigh") == 0)  {
                doRayleigh = 1;
            } else if (strcmp(argv[i], "-noRayleigh") == 0)  {
                doRayleigh = 0;
            }
        }
        for (i = 11+eleArgStart; i < argc; i++)  {
            if (i+1 < argc && strcmp(argv[i], "-mass") == 0)  {
                if (Tcl_GetDouble(interp, argv[i+1], &mass) != TCL_OK)  {
                    opserr << "WARNING invalid -mass value\n";
                    opserr << "expElement bearing element: " << tag << endln;
                    return TCL_ERROR;
                }
            }
        }
        
		// now create the EEBearing
        if (theSite != 0)  {
		    theExpElement = new EEBearing2d(tag, iNode, jNode, pFrcCtrl, theMaterials,
                theSite, y, x, Mratio, shearDistI, iMod, doRayleigh, mass);
        } else  {
		    theExpElement = new EEBearing2d(tag, iNode, jNode, pFrcCtrl, theMaterials,
                ipPort, ipAddr, ssl, udp, dataSize, y, x, Mratio, shearDistI,
                iMod, doRayleigh, mass);
        }
		
		if (theExpElement == 0) {
			opserr << "WARNING ran out of memory creating element\n";
			opserr << "expElement bearing element: " << tag << endln;
			return TCL_ERROR;
		}
        
		// then add the EEBearing to the domain
		if (theTclDomain->addElement(theExpElement) == false)  {
			opserr << "WARNING could not add element to domain\n";
			opserr << "expElement bearing element: " << tag << endln;
			delete theExpElement; // clean up the memory to avoid leaks
			return TCL_ERROR;
		}
		
		// finally check for initial stiffness terms
        int setInitStif = -1;
		for (i = 11+eleArgStart; i < argc; i++)  {
			if (strcmp(argv[i], "-initStif") == 0)  {
				if (argc-1 < i+1)  {
					opserr << "WARNING incorrect number of inital stiffness terms\n";
					opserr << "expElement bearing element: " << tag << endln;
					return TCL_ERROR;      
				}
				Matrix theInitStif(1,1);
				double stif;
				for (j=0; j<1; j++)  {
					for (k=0; k<1; k++)  {
						if (Tcl_GetDouble(interp, argv[i+1 + 1*j+k], &stif) != TCL_OK)  {
							opserr << "WARNING invalid initial stiffness term\n";
							opserr << "expElement bearing element: " << tag << endln;
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
            opserr << "expElement bearing element: " << tag << endln;
            return TCL_ERROR;
        }
	}
	
	else if (ndm == 3)  {
		// check space frame problem has 6 dof per node
		if (ndf != 6)  {
			opserr << "WARNING invalid ndf: " << ndf;
			opserr << ", for space problem need 6 - expElement bearing \n";    
			return TCL_ERROR;
		}

		// check the number of arguments
		if ((argc-eleArgStart) < 20)  {
			opserr << "WARNING insufficient arguments\n";
			printCommand(argc, argv);
			opserr << "Want: expElement bearing eleTag iNode jNode pFrcCtrl -P matTag -T matTag -My matTag -Mz matTag -site siteTag -initStif Kij <-orient <x1 x2 x3> y1 y2 y3> <-pDelta Mratios> <-shearDist sDratio> <-iMod> <-doRayleigh> <-mass m>\n";
			opserr << "  or: expElement bearing eleTag iNode jNode pFrcCtrl -P matTag -T matTag -My matTag -Mz matTag -server ipPort <ipAddr> <-ssl> <-udp> <-dataSize size> -initStif Kij <-orient <x1 x2 x3> y1 y2 y3> <-pDelta Mratios> <-shearDist sDratio> <-iMod> <-doRayleigh> <-mass m>\n";
			return TCL_ERROR;
		}
		
		// get the id and end nodes
		int iNode, jNode, pFrcCtrl, matTag, siteTag, ipPort, i, j, k;
        UniaxialMaterial *theMaterials[4];
        ExperimentalSite *theSite = 0;
        char *ipAddr = 0;
        int ssl = 0, udp = 0;
        int dataSize = OF_Network_dataSize;
        Vector Mratio(0);
        double shearDistI = 0.5;
        bool iMod = false;
        int doRayleigh = 0;
        double mass = 0.0;
		
		if (Tcl_GetInt(interp, argv[1+eleArgStart], &tag) != TCL_OK)  {
			opserr << "WARNING invalid expElement bearing eleTag\n";
			return TCL_ERROR;
		}
		if (Tcl_GetInt(interp, argv[2+eleArgStart], &iNode) != TCL_OK)  {
			opserr << "WARNING invalid iNode\n";
			opserr << "expElement bearing element: " << tag << endln;
			return TCL_ERROR;
		}
		if (Tcl_GetInt(interp, argv[3+eleArgStart], &jNode) != TCL_OK)  {
			opserr << "WARNING invalid jNode\n";
			opserr << "expElement bearing element: " << tag << endln;
			return TCL_ERROR;
		}
		if (Tcl_GetInt(interp, argv[4+eleArgStart], &pFrcCtrl) != TCL_OK)  {
			opserr << "WARNING invalid pFrcCtrl\n";
			opserr << "expElement bearing element: " << tag << endln;
			return TCL_ERROR;
		}
        if (strcmp(argv[5+eleArgStart], "-P") == 0)  {
		    if (Tcl_GetInt(interp, argv[6+eleArgStart], &matTag) != TCL_OK)  {
			    opserr << "WARNING invalid -P matTag\n";
			    opserr << "expElement bearing element: " << tag << endln;
			    return TCL_ERROR;
		    }
            theMaterials[0] = OPS_GetUniaxialMaterial(matTag);
            if (theMaterials[0] == 0)  {
                opserr << "WARNING material model not found\n";
                opserr << "uniaxialMaterial: " << matTag << endln;
                opserr << "expElement bearing element: " << tag << endln;
                return TCL_ERROR;
            }
        }
        if (strcmp(argv[7+eleArgStart], "-T") == 0)  {
		    if (Tcl_GetInt(interp, argv[8+eleArgStart], &matTag) != TCL_OK)  {
			    opserr << "WARNING invalid -T matTag\n";
			    opserr << "expElement bearing element: " << tag << endln;
			    return TCL_ERROR;
		    }
            theMaterials[1] = OPS_GetUniaxialMaterial(matTag);
            if (theMaterials[1] == 0)  {
                opserr << "WARNING material model not found\n";
                opserr << "uniaxialMaterial: " << matTag << endln;
                opserr << "expElement bearing element: " << tag << endln;
                return TCL_ERROR;
            }
        }
        if (strcmp(argv[9+eleArgStart], "-My") == 0)  {
		    if (Tcl_GetInt(interp, argv[10+eleArgStart], &matTag) != TCL_OK)  {
			    opserr << "WARNING invalid -My matTag\n";
			    opserr << "expElement bearing element: " << tag << endln;
			    return TCL_ERROR;
		    }
            theMaterials[2] = OPS_GetUniaxialMaterial(matTag);
            if (theMaterials[2] == 0)  {
                opserr << "WARNING material model not found\n";
                opserr << "uniaxialMaterial: " << matTag << endln;
                opserr << "expElement bearing element: " << tag << endln;
                return TCL_ERROR;
            }
        }
        if (strcmp(argv[11+eleArgStart], "-Mz") == 0)  {
		    if (Tcl_GetInt(interp, argv[12+eleArgStart], &matTag) != TCL_OK)  {
			    opserr << "WARNING invalid -Mz matTag\n";
			    opserr << "expElement bearing element: " << tag << endln;
			    return TCL_ERROR;
		    }
            theMaterials[3] = OPS_GetUniaxialMaterial(matTag);
            if (theMaterials[3] == 0)  {
                opserr << "WARNING material model not found\n";
                opserr << "uniaxialMaterial: " << matTag << endln;
                opserr << "expElement bearing element: " << tag << endln;
                return TCL_ERROR;
            }
        }
        if (strcmp(argv[13+eleArgStart], "-site") == 0)  {
		    if (Tcl_GetInt(interp, argv[14+eleArgStart], &siteTag) != TCL_OK)  {
			    opserr << "WARNING invalid siteTag\n";
			    opserr << "expElement bearing element: " << tag << endln;
			    return TCL_ERROR;
		    }
		    theSite = getExperimentalSite(siteTag);
		    if (theSite == 0)  {
			    opserr << "WARNING experimental site not found\n";
			    opserr << "expSite: " << siteTag << endln;
			    opserr << "expElement bearing element: " << tag << endln;
			    return TCL_ERROR;
		    }
        }
        else if (strcmp(argv[13+eleArgStart], "-server") == 0)  {
            if (Tcl_GetInt(interp, argv[14+eleArgStart], &ipPort) != TCL_OK)  {
		        opserr << "WARNING invalid ipPort\n";
		        opserr << "expElement bearing element: " << tag << endln;
		        return TCL_ERROR;
	        }
            if (strcmp(argv[15+eleArgStart], "-initStif") != 0  &&
                strcmp(argv[15+eleArgStart], "-ssl") != 0  &&
                strcmp(argv[15+eleArgStart], "-udp") != 0  &&
                strcmp(argv[15+eleArgStart], "-dataSize") != 0)  {
                ipAddr = new char [strlen(argv[15+eleArgStart])+1];
                strcpy(ipAddr,argv[15+eleArgStart]);
            }
            else  {
                ipAddr = new char [9+1];
                strcpy(ipAddr,"127.0.0.1");
            }
            for (i = 15+eleArgStart; i < argc; i++)  {
                if (strcmp(argv[i], "-ssl") == 0)
                    ssl = 1;
                else if (strcmp(argv[i], "-udp") == 0)
                    udp = 1;
                else if (strcmp(argv[i], "-dataSize") == 0)  {
                    if (Tcl_GetInt(interp, argv[i+1], &dataSize) != TCL_OK)  {
		                opserr << "WARNING invalid dataSize\n";
		                opserr << "expElement bearing element: " << tag << endln;
		                return TCL_ERROR;
	                }
                }
            }
        }
        else  {
            opserr << "WARNING expecting -site or -server string but got ";
            opserr << argv[13+eleArgStart] << endln;
		    opserr << "expElement bearing element: " << tag << endln;
            return TCL_ERROR;
        }
        
        // check for optional arguments
        Vector x(0);
        Vector y(3); y(0) = 0.0; y(1) = 1.0; y(2) = 0.0;
        for (i = 15+eleArgStart; i < argc; i++)  {
            if (strcmp(argv[i],"-orient") == 0)  {
                j = i+1;
                int numOrient = 0;
                while (j < argc &&
                    strcmp(argv[j],"-pDelta") != 0 &&
                    strcmp(argv[j],"-shearDist") != 0 &&
                    strcmp(argv[j],"-iMod") != 0 &&
                    strcmp(argv[j],"-doRayleigh") != 0 &&
                    strcmp(argv[j],"-mass") != 0)  {
                        numOrient++;
                        j++;
                }
                if (numOrient == 3)  {
                    double value;
                    // read the y values
                    for (j=0; j<3; j++)  {
                        if (Tcl_GetDouble(interp, argv[i+1+j], &value) != TCL_OK)  {
                            opserr << "WARNING invalid -orient value\n";
                            opserr << "expElement bearing element: " << tag << endln;
                            return TCL_ERROR;
                        }
                        y(j) = value;
                    }
                }
                else if (numOrient == 6)  {
                    x.resize(3);
                    double value;
                    // read the x values
                    for (j=0; j<3; j++)  {
                        if (Tcl_GetDouble(interp, argv[i+1+j], &value) != TCL_OK)  {
                            opserr << "WARNING invalid -orient value\n";
                            opserr << "expElement bearing element: " << tag << endln;
                            return TCL_ERROR;
                        }
                        x(j) = value;
                    }
                    // read the y values
                    for (j=0; j<3; j++)  {
                        if (Tcl_GetDouble(interp, argv[i+4+j], &value) != TCL_OK)  {
                            opserr << "WARNING invalid -orient value\n";
                            opserr << "expElement bearing element: " << tag << endln;
                            return TCL_ERROR;
                        }
                        y(j) = value;
                    }
                }
                else  {
                    opserr << "WARNING insufficient arguments after -orient flag\n";
                    opserr << "expElement bearing element: " << tag << endln;
                    return TCL_ERROR;
                }
            }
        }
        for (i = 15+eleArgStart; i < argc; i++)  {
            if (i+1 < argc && strcmp(argv[i], "-pDelta") == 0)  {
                double Mr;
                Mratio.resize(4);
                Mratio.Zero();
                for (j=0; j<4; j++)  {
                    if (Tcl_GetDouble(interp, argv[i+1+j], &Mr) != TCL_OK)  {
                        opserr << "WARNING invalid -pDelta value\n";
                        opserr << "expElement bearing element: " << tag << endln;
                        return TCL_ERROR;
                    }
                    Mratio(j) = Mr;
                }
            }
        }
        for (i = 15+eleArgStart; i < argc; i++)  {
            if (i+1 < argc && strcmp(argv[i], "-shearDist") == 0)  {
                if (Tcl_GetDouble(interp, argv[i+1], &shearDistI) != TCL_OK)  {
                    opserr << "WARNING invalid -shearDist value\n";
                    opserr << "expElement bearing element: " << tag << endln;
                    return TCL_ERROR;
                }
            }
        }
		for (i = 15+eleArgStart; i < argc; i++)  {
			if (strcmp(argv[i], "-iMod") == 0)  {
                iMod = true;
			}
		}
		for (i = 15+eleArgStart; i < argc; i++)  {
			if (strcmp(argv[i], "-doRayleigh") == 0)  {
                doRayleigh = 1;
            } else if (strcmp(argv[i], "-noRayleigh") == 0)  {
                doRayleigh = 0;
            }
        }
        for (i = 15+eleArgStart; i < argc; i++)  {
            if (i+1 < argc && strcmp(argv[i], "-mass") == 0)  {
                if (Tcl_GetDouble(interp, argv[i+1], &mass) != TCL_OK)  {
                    opserr << "WARNING invalid -mass value\n";
                    opserr << "expElement bearing element: " << tag << endln;
                    return TCL_ERROR;
                }
            }
        }
        
		// now create the EEBearing
        if (theSite != 0)  {
		    theExpElement = new EEBearing3d(tag, iNode, jNode, pFrcCtrl, theMaterials,
                theSite, y, x, Mratio, shearDistI, iMod, doRayleigh, mass);
        } else  {
		    theExpElement = new EEBearing3d(tag, iNode, jNode, pFrcCtrl, theMaterials,
                ipPort, ipAddr, ssl, udp, dataSize, y, x, Mratio, shearDistI,
                iMod, doRayleigh, mass);
        }
		
		if (theExpElement == 0) {
			opserr << "WARNING ran out of memory creating element\n";
			opserr << "expElement bearing element: " << tag << endln;
			return TCL_ERROR;
		}
        
		// then add the EEBearing to the domain
		if (theTclDomain->addElement(theExpElement) == false)  {
			opserr << "WARNING could not add element to domain\n";
			opserr << "expElement bearing element: " << tag << endln;
			delete theExpElement; // clean up the memory to avoid leaks
			return TCL_ERROR;
		}
		
		// finally check for initial stiffness terms
        int setInitStif = -1;
		for (i = 15+eleArgStart; i < argc; i++)  {
			if (strcmp(argv[i], "-initStif") == 0)  {
				if (argc-1 < i+4)  {
					opserr << "WARNING incorrect number of inital stiffness terms\n";
					opserr << "expElement bearing element: " << tag << endln;
					return TCL_ERROR;      
				}
				Matrix theInitStif(2,2);
				double stif;
				for (j=0; j<2; j++)  {
					for (k=0; k<2; k++)  {
						if (Tcl_GetDouble(interp, argv[i+1 + 2*j+k], &stif) != TCL_OK)  {
							opserr << "WARNING invalid initial stiffness term\n";
							opserr << "expElement bearing element: " << tag << endln;
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
            opserr << "expElement bearing element: " << tag << endln;
            return TCL_ERROR;
        }
	}
	
	else  {
		opserr << "WARNING expElement bearing command only works when ndm is 2 or 3, ndm: ";
		opserr << ndm << endln;
		return TCL_ERROR;
	}
		
	// if get here we have sucessfully created the EEBearing and added it to the domain
	return TCL_OK;
}
