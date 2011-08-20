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
// Created: 10/06
// Revision: A
//
// Description: This file contains the function to parse the TCL input
// for the EEGeneric element.

#include <stdlib.h>
#include <string.h>
#include <tcl.h>
#include <Domain.h>

#include <EEGeneric.h>

extern ExperimentalSite *getExperimentalSite(int tag);


static void printCommand(int argc, TCL_Char **argv)
{
    opserr << "Input command: ";
    for (int i=0; i<argc; i++)
        opserr << argv[i] << " ";
    opserr << endln;
} 


int addEEGeneric(ClientData clientData, Tcl_Interp *interp,  int argc, 
    TCL_Char **argv, Domain *theTclDomain, int eleArgStart)
{
	ExperimentalElement *theExpElement = 0;
    
	// check the number of arguments is correct
	if ((argc-eleArgStart) < 10)  {
		opserr << "WARNING insufficient arguments\n";
		printCommand(argc, argv);
		opserr << "Want: expElement generic eleTag -node Ndi -dof dofNdi -dof dofNdj ... -site siteTag -initStif Kij <-iMod> <-mass Mij>\n";
		opserr << "  or: expElement generic eleTag -node Ndi -dof dofNdi -dof dofNdj ... -server ipPort <ipAddr> <-ssl> <-dataSize size> -initStif Kij <-iMod> <-mass Mij>\n";
		return TCL_ERROR;
	}    
	
	// get the id and end nodes     
    int tag, siteTag, node, dof, ipPort, argi, i, j, k;
    int numNodes = 0, numDOFj = 0, numDOF = 0;
    ExperimentalSite *theSite = 0;
    char *ipAddr = 0;
    int ssl = 0, udp = 0;
    int dataSize = OF_Network_dataSize;
    bool iMod = false;
    Matrix *mass = 0;

	if (Tcl_GetInt(interp, argv[1+eleArgStart], &tag) != TCL_OK)  {
		opserr << "WARNING invalid expElement generic eleTag\n";
		return TCL_ERROR;
	}
    // read the number of nodes
    if (strcmp(argv[2+eleArgStart], "-node") != 0)  {
		opserr << "WARNING expecting -node flag\n";
		opserr << "expElement generic element: " << tag << endln;
		return TCL_ERROR;
	}
    argi = 3+eleArgStart;
    i = argi;
    while (i < argc && strcmp(argv[i], "-dof") != 0)  {
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
		    opserr << "WARNING expect -dof flag\n";
		    opserr << "expElement generic element: " << tag << endln;
		    return TCL_ERROR;
	    }
        argi++;
        i = argi;
        while (i < argc &&
            strcmp(argv[i], "-dof") != 0 && 
            strcmp(argv[i], "-site") != 0 && 
            strcmp(argv[i], "-server") != 0)  {
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
        dofs[j] = dofsj;
    }
    if (strcmp(argv[argi], "-site") == 0)  {
        argi++;
	    if (Tcl_GetInt(interp, argv[argi], &siteTag) != TCL_OK)  {
		    opserr << "WARNING invalid siteTag\n";
		    opserr << "expElement generic element: " << tag << endln;
		    return TCL_ERROR;
	    }
        argi++;
	    theSite = getExperimentalSite(siteTag);
	    if (theSite == 0)  {
		    opserr << "WARNING experimental site not found\n";
		    opserr << "expSite: " << siteTag << endln;
		    opserr << "expElement generic element: " << tag << endln;
		    return TCL_ERROR;
	    }
    }
    else if (strcmp(argv[argi], "-server") == 0)  {
        argi++;
        if (Tcl_GetInt(interp, argv[argi], &ipPort) != TCL_OK)  {
	        opserr << "WARNING invalid ipPort\n";
	        opserr << "expElement generic element: " << tag << endln;
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
		            opserr << "expElement generic element: " << tag << endln;
		            return TCL_ERROR;
	            }
            }
        }
    }
    else  {
        opserr << "WARNING expecting -site or -server string but got ";
        opserr << argv[argi] << endln;
	    opserr << "expElement generic element: " << tag << endln;
        return TCL_ERROR;
    }
    for (i=argi; i<argc; i++)  {
        if (strcmp(argv[i], "-iMod") == 0)  {
            iMod = true;
        }
    }
	for (i=argi; i<argc; i++)  {
		if (strcmp(argv[i], "-mass") == 0)  {
			if (argc-1 < i+numDOF*numDOF)  {
				opserr << "WARNING incorrect number of mass terms\n";
				opserr << "expElement generic element: " << tag << endln;
				return TCL_ERROR;      
			}
			mass = new Matrix(numDOF,numDOF);
			double m;
			for (j=0; j<numDOF; j++)  {
				for (k=0; k<numDOF; k++)  {
					if (Tcl_GetDouble(interp, argv[i+1 + numDOF*j+k], &m) != TCL_OK)  {
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
    if (theSite != 0) {
        if (mass == 0) {
	        theExpElement = new EEGeneric(tag, nodes, dofs, theSite, iMod);
        }
        else {
            theExpElement = new EEGeneric(tag, nodes, dofs, theSite, iMod, mass);
        }
    }
    else {
        if (mass == 0) {
	        theExpElement = new EEGeneric(tag, nodes, dofs, ipPort, ipAddr, ssl, udp, dataSize, iMod);
        }
        else {
            theExpElement = new EEGeneric(tag, nodes, dofs, ipPort, ipAddr, ssl, udp, dataSize, iMod, mass);
        }
    }
	
    // cleanup dynamic memory
    if (dofs != 0)
        delete [] dofs;
    
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
    int setInitStif = -1;
	for (i=argi; i<argc; i++)  {
		if (strcmp(argv[i], "-initStif") == 0)  {
			if (argc-1 < i+numDOF*numDOF)  {
				opserr << "WARNING incorrect number of inital stiffness terms\n";
				opserr << "expElement generic element: " << tag << endln;
				return TCL_ERROR;      
			}
			Matrix theInitStif(numDOF,numDOF);
			double stif;
			for (j=0; j<numDOF; j++)  {
				for (k=0; k<numDOF; k++)  {
					if (Tcl_GetDouble(interp, argv[i+1 + numDOF*j+k], &stif) != TCL_OK)  {
						opserr << "WARNING invalid initial stiffness term\n";
						opserr << "expElement generic element: " << tag << endln;
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
        opserr << "expElement generic element: " << tag << endln;
        return TCL_ERROR;
    }
    
	// if get here we have sucessfully created the EEGeneric and added it to the domain
	return TCL_OK;
}
