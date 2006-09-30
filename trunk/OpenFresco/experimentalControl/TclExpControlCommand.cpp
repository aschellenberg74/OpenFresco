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
// Description: This file contains the function invoked when the user
// invokes the expControl command in the interpreter. 

#include <TclModelBuilder.h>
#include <ArrayOfTaggedObjects.h>

#include <ECxPCtarget.h>
#include <ECdSpace.h>
#include <ECSimUniaxialMaterials.h>

#include <Vector.h>
#include <string.h>

static ArrayOfTaggedObjects *theExperimentalControls(0);


int addExperimentalControl(ExperimentalControl &theTestController)
{
    bool result = theExperimentalControls->addComponent(&theTestController);
    if (result == true)
        return 0;
    else {
        opserr << "TclModelBuilder::addExperimentalControl() - "
            << "failed to add experimental controller: " << theTestController;
        return -1;
    }
}


extern ExperimentalControl *getExperimentalControl(int tag)
{
    TaggedObject *mc = theExperimentalControls->getComponentPtr(tag);
    if (mc == 0) 
        return 0;

    // otherweise we do a cast and return
    ExperimentalControl *result = (ExperimentalControl *)mc;
    return result;
}


static void printCommand(int argc, TCL_Char **argv)
{
    opserr << "Input command: ";
    for (int i=0; i<argc; i++)
        opserr << argv[i] << " ";
    opserr << endln;
} 


int TclExpControlCommand(ClientData clientData, Tcl_Interp *interp, int argc,
    TCL_Char **argv, TclModelBuilder *theTclBuilder, Domain *theDomain)
{
    if (theExperimentalControls == 0)
        theExperimentalControls = new ArrayOfTaggedObjects(32);

    // make sure there is a minimum number of arguments
    if (argc < 3)  {
		opserr << "WARNING insufficient number of experimental control arguments\n";
		opserr << "Want: expControl type tag <specific experimental control args>\n";
		return TCL_ERROR;
    }
    	
    // ----------------------------------------------------------------------------	
    if (strcmp(argv[1],"xPCtarget") == 0)  {
		if (argc != 8)  {
			opserr << "WARNING invalid number of arguments\n";
			printCommand(argc,argv);
			opserr << "Want: expControl xPCtarget tag type ipAddr ipPort appName appPath\n";
			return TCL_ERROR;
		}    
		
		int tag, type;
		char *ipAddr, *ipPort, *appName, *appPath;
        ExperimentalControl *theControl = 0;
		
		if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK)  {
			opserr << "WARNING invalid xPCtarget tag\n";
			return TCL_ERROR;		
		}
		if (Tcl_GetInt(interp, argv[3], &type) != TCL_OK)  {
			opserr << "WARNING invalid type\n";
			opserr << "xPCtarget controller: " << tag << endln;
			return TCL_ERROR;	
		}
		ipAddr = (char *)malloc((strlen(argv[4]) + 1)*sizeof(char));
		strcpy(ipAddr,argv[4]);
		ipPort = (char *)malloc((strlen(argv[5]) + 1)*sizeof(char));		
		strcpy(ipPort,argv[5]);
		appName = (char *)malloc((strlen(argv[6]) + 1)*sizeof(char));
		strcpy(appName,argv[6]);
		appPath = (char *)malloc((strlen(argv[7]) + 1)*sizeof(char));
		strcpy(appPath,argv[7]);
		
		// parsing was successful, allocate the controller
		theControl = new ECxPCtarget(tag, type, ipAddr, ipPort, appName, appPath);

        if (theControl == 0)  {
            opserr << "WARNING could not create experimental controller " << argv[1] << endln;
            return TCL_ERROR;
        }
        
        // now add the controller to the modelBuilder
        if (addExperimentalControl(*theControl) < 0)  {
            opserr << "WARNING could not add experimental controller to the domain\n";
            opserr << *theControl << endln;
            delete theControl; // invoke the destructor, otherwise mem leak
            return TCL_ERROR;
        }
    }
	
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"dSpace") == 0)  {
		if (argc != 5)  {
			opserr << "WARNING invalid number of arguments\n";
			printCommand(argc,argv);
			opserr << "Want: expControl dSpace tag type boardName\n";
			return TCL_ERROR;
		}    
		
		int tag, type;
		char *boardName;
        ExperimentalControl *theControl = 0;
		
		if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK)  {
			opserr << "WARNING invalid dSpace tag\n";
			return TCL_ERROR;		
		}
		if (Tcl_GetInt(interp, argv[3], &type) != TCL_OK)  {
			opserr << "WARNING invalid type\n";
			opserr << "dSpace controller: " << tag << endln;
			return TCL_ERROR;	
		}
		boardName = (char *)malloc((strlen(argv[4]) + 1)*sizeof(char));
		strcpy(boardName,argv[4]);
		
		// parsing was successful, allocate the controller
		theControl = new ECdSpace(tag, type, boardName);

        if (theControl == 0)  {
            opserr << "WARNING could not create experimental controller " << argv[1] << endln;
            return TCL_ERROR;
        }
        
        // now add the controller to the modelBuilder
        if (addExperimentalControl(*theControl) < 0)  {
            opserr << "WARNING could not add experimental controller to the domain\n";
            opserr << *theControl << endln;
            delete theControl; // invoke the destructor, otherwise mem leak
            return TCL_ERROR;
        }
    }

    // ----------------------------------------------------------------------------	
	else if (strcmp(argv[1],"SimUniaxialMaterials") == 0)  {
		if (argc < 4)  {
			opserr << "WARNING invalid number of arguments\n";
			printCommand(argc,argv);
			opserr << "Want: expControl SimUniaxialMaterials tag matTags\n";
			return TCL_ERROR;
		}    
		
		int tag, numMats, argi;
        ECSimUniaxialMaterials *theControl = 0;
		
		if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK)  {
			opserr << "WARNING invalid SimUniaxialMaterials tag\n";
			return TCL_ERROR;		
		}

		// parsing was successful, allocate the controller
		theControl = new ECSimUniaxialMaterials(tag);	

		// now read the number of materials
		numMats = 0;
		argi = 3; 
		while (argi < argc)  {
			numMats++;
			argi++;
		}
		// add the materials to the controller
		argi = 3; 
		for (int i=0; i<numMats; i++)  {
			int matTag;	
			// read the material tag
			if (Tcl_GetInt(interp, argv[argi], &matTag) != TCL_OK)  {
				opserr << "WARNING invalid matTag\n";
				opserr << "SimUniaxialMaterials controller: " << tag << endln;
				return TCL_ERROR;
			} else  {
				// get a pointer to the material from the modelbuilder	    
				argi++;
				UniaxialMaterial *theMat = theTclBuilder->getUniaxialMaterial(matTag);
				if (theMat == 0)  {
					opserr << "WARNING no material " << matTag << " exists\n";
					opserr << "SimUniaxialMaterials controller: " << tag << endln;
					return TCL_ERROR;		
				} else  {
					// add the material
                    theControl->addDummySpecimen(theMat);
				}
			}
		}

        if (theControl == 0)  {
            opserr << "WARNING could not create experimental controller " << argv[1] << endln;
            return TCL_ERROR;
        }
        
        // now add the controller to the modelBuilder
        if (addExperimentalControl(*theControl) < 0)  {
            opserr << "WARNING could not add experimental controller to the domain\n";
            opserr << *theControl << endln;
            delete theControl; // invoke the destructor, otherwise mem leak
            return TCL_ERROR;
        }
    }
	
    // ----------------------------------------------------------------------------	
    else  {
        // experimental control type not recognized
        opserr << "WARNING unknown experimental control type: "
            <<  argv[1] << ": check the manual\n";
        return TCL_ERROR;
    }

	return TCL_OK;
}
