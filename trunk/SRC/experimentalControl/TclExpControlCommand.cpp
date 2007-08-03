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
// Description: This file contains the function invoked when the user
// invokes the expControl command in the interpreter. 

#include <TclModelBuilder.h>
#include <ArrayOfTaggedObjects.h>

#ifdef _WIN32
#include <ECxPCtarget.h>
#include <ECdSpace.h>
#include <ECMtsCsi.h>
#include <ECLabVIEW.h>
//#include <ECNIEseries.h>
#endif

#include <ECSCRAMNet.h>
#include <ECSimUniaxialMaterials.h>
#include <ECSimDomain.h>

#include <Vector.h>
#include <string.h>

extern ExperimentalCP *getExperimentalCP(int tag);
static ArrayOfTaggedObjects *theExperimentalControls(0);


int addExperimentalControl(ExperimentalControl &theControl)
{
    bool result = theExperimentalControls->addComponent(&theControl);
    if (result == true)
        return 0;
    else {
        opserr << "addExperimentalControl() - "
            << "failed to add experimental control: " << theControl;
        return -1;
    }
}


extern ExperimentalControl *getExperimentalControl(int tag)
{
    if (theExperimentalControls == 0) {
        opserr << "getExperimentalControl() - "
            << "failed to get experimental control: " << tag << endln
            << "no experimental control objects have been defined\n";
        return 0;
    }

    TaggedObject *mc = theExperimentalControls->getComponentPtr(tag);
    if (mc == 0) 
        return 0;

    // otherwise we do a cast and return
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
    TCL_Char **argv, Domain *theDomain, TclModelBuilder *theTclBuilder)
{
    if (theExperimentalControls == 0)
        theExperimentalControls = new ArrayOfTaggedObjects(32);

    // make sure there is a minimum number of arguments
    if (argc < 3)  {
		opserr << "WARNING insufficient number of experimental control arguments\n";
		opserr << "Want: expControl type tag <specific experimental control args>\n";
		return TCL_ERROR;
    }
    	
#ifdef _WIN32
    // ----------------------------------------------------------------------------	
    if (strcmp(argv[1],"xPCtarget") == 0)  {
		if (argc < 7)  {
			opserr << "WARNING invalid number of arguments\n";
			printCommand(argc,argv);
			opserr << "Want: expControl xPCtarget tag type ipAddr ipPort appName <appPath>\n";
			return TCL_ERROR;
		}    
		
		int tag, type;
		char *ipAddr, *ipPort, *appName, *appPath = 0;
        ExperimentalControl *theControl = 0;
		
		if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK)  {
			opserr << "WARNING invalid expControl xPCtarget tag\n";
			return TCL_ERROR;		
		}
		if (Tcl_GetInt(interp, argv[3], &type) != TCL_OK)  {
			opserr << "WARNING invalid type\n";
			opserr << "expControl xPCtarget " << tag << endln;
			return TCL_ERROR;	
		}
		ipAddr = (char*) calloc(strlen(argv[4])+1, sizeof(char));
		strcpy(ipAddr,argv[4]);
		ipPort = (char*) calloc(strlen(argv[5])+1, sizeof(char));
		strcpy(ipPort,argv[5]);
		appName = (char*) calloc(strlen(argv[6])+1, sizeof(char));
		strcpy(appName,argv[6]);
        if (argc == 8)  {
		    appPath = (char*) calloc(strlen(argv[7])+1, sizeof(char));
		    strcpy(appPath,argv[7]);
        }
		
		// parsing was successful, allocate the control
		theControl = new ECxPCtarget(tag, type, ipAddr, ipPort, appName, appPath);

        if (theControl == 0)  {
            opserr << "WARNING could not create experimental control " << argv[1] << endln;
            return TCL_ERROR;
        }
        
        // now add the control to the modelBuilder
        if (addExperimentalControl(*theControl) < 0)  {
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
			opserr << "WARNING invalid expControl dSpace tag\n";
			return TCL_ERROR;
		}
		if (Tcl_GetInt(interp, argv[3], &type) != TCL_OK)  {
			opserr << "WARNING invalid type\n";
			opserr << "expControl dSpace " << tag << endln;
			return TCL_ERROR;
		}
		boardName = (char*) calloc(strlen(argv[4])+1, sizeof(char));
		strcpy(boardName,argv[4]);
		
		// parsing was successful, allocate the control
		theControl = new ECdSpace(tag, type, boardName);

        if (theControl == 0)  {
            opserr << "WARNING could not create experimental control " << argv[1] << endln;
            return TCL_ERROR;
        }
        
        // now add the control to the modelBuilder
        if (addExperimentalControl(*theControl) < 0)  {
            delete theControl; // invoke the destructor, otherwise mem leak
            return TCL_ERROR;
        }
    }

    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"MTSCsi") == 0)  {
		if (argc < 4)  {
			opserr << "WARNING invalid number of arguments\n";
			printCommand(argc,argv);
			opserr << "Want: expControl MTSCsi tag configFileName <rampTime>\n";
			return TCL_ERROR;
		}    
		
		int tag;
        char *cfgFile;
        double rampTime = 0.02;

		if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK)  {
			opserr << "WARNING invalid expControl MTSCsi tag\n";
			return TCL_ERROR;
		}
		cfgFile = (char*) calloc(strlen(argv[3])+1, sizeof(char));
		strcpy(cfgFile,argv[3]);
        if (argc == 5) {
		    if (Tcl_GetDouble(interp, argv[4], &rampTime) != TCL_OK)  {
			    opserr << "WARNING invalid rampTime\n";
			    opserr << "expControl MTSCsi " << tag << endln;
			    return TCL_ERROR;	
            }
        }
		// parsing was successful, allocate the control
		ExperimentalControl *theControl = new ECMtsCsi(tag, cfgFile, rampTime);

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
    else if (strcmp(argv[1],"LabVIEW") == 0)  {
		if (argc < 8)  {
			opserr << "WARNING invalid number of arguments\n";
			printCommand(argc,argv);
			opserr << "Want: expControl LabVIEW tag ipAddr <ipPort> -trialCP cpTags -outCP cpTags\n";
			return TCL_ERROR;
		}

		int tag, cpTag, ipPort = 44000, argi = 2;
        int numTrialCPs = 0, numOutCPs = 0;
		char *ipAddr;
        ExperimentalControl *theControl = 0;
		
		if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
			opserr << "WARNING invalid expControl LabVIEW tag\n";
			return TCL_ERROR;
		}
        argi++;
		ipAddr = (char*) calloc(strlen(argv[argi])+1, sizeof(char));
		strcpy(ipAddr,argv[argi]);
        argi++;
        if (strcmp(argv[argi],"-trialCP") != 0)  {
		    if (Tcl_GetInt(interp, argv[argi], &ipPort) != TCL_OK)  {
			    opserr << "WARNING invalid ipPort\n";
			    opserr << "expControl LabVIEW " << tag << endln;
			    return TCL_ERROR;
		    }
            argi++;
        }
        if (strcmp(argv[argi],"-trialCP") != 0)  {
		    opserr << "WARNING expecting -trialCP cpTags\n";
		    opserr << "expControl LabVIEW " << tag << endln;
		    return TCL_ERROR;
        }
        argi++;
        while (strcmp(argv[argi+numTrialCPs],"-outCP") != 0 &&
            argi+numTrialCPs < argc)  {
            numTrialCPs++;
        }
        if (numTrialCPs == 0)  {
		    opserr << "WARNING no trialCPTags specified\n";
		    opserr << "expControl LabVIEW " << tag << endln;
		    return TCL_ERROR;
	    }
        // create the array to hold the trial control points
        ExperimentalCP **trialCPs = new ExperimentalCP* [numTrialCPs];
        if (trialCPs == 0)  {
		    opserr << "WARNING out of memory\n";
		    opserr << "expControl LabVIEW " << tag << endln;
		    return TCL_ERROR;
	    }
        for (int i=0; i<numTrialCPs; i++)  {
            if (Tcl_GetInt(interp, argv[argi], &cpTag) != TCL_OK)  {
                opserr << "WARNING invalid cpTag\n";
                opserr << "expControl LabVIEW " << tag << endln;
                return TCL_ERROR;	
            }
            trialCPs[i] = getExperimentalCP(cpTag);
            if (trialCPs[i] == 0)  {
                opserr << "WARNING experimental control point not found\n";
                opserr << "expControlPoint " << cpTag << endln;
                opserr << "expControl LabVIEW " << tag << endln;
                return TCL_ERROR;
            }
            argi++;
        }
        if (strcmp(argv[argi],"-outCP") != 0)  {
		    opserr << "WARNING expecting -outCP cpTags\n";
		    opserr << "expControl LabVIEW " << tag << endln;
		    return TCL_ERROR;		
        }
        argi++;
        while (argi+numOutCPs < argc)  {
            numOutCPs++;
        }
        if (numOutCPs == 0)  {
		    opserr << "WARNING no outCPTags specified\n";
		    opserr << "expControl LabVIEW " << tag << endln;
		    return TCL_ERROR;
	    }
        // create the array to hold the output control points
        ExperimentalCP **outCPs = new ExperimentalCP* [numOutCPs];
        if (outCPs == 0)  {
		    opserr << "WARNING out of memory\n";
		    opserr << "expControl LabVIEW " << tag << endln;
		    return TCL_ERROR;
	    }
        for (int i=0; i<numOutCPs; i++)  {
            if (Tcl_GetInt(interp, argv[argi], &cpTag) != TCL_OK)  {
                opserr << "WARNING invalid cpTag\n";
                opserr << "expControl LabVIEW " << tag << endln;
                return TCL_ERROR;
            }
            outCPs[i] = getExperimentalCP(cpTag);
            if (outCPs[i] == 0)  {
                opserr << "WARNING experimental control point not found\n";
                opserr << "expControlPoint " << cpTag << endln;
                opserr << "expControl LabVIEW " << tag << endln;
                return TCL_ERROR;
            }
            argi++;
        }
		
		// parsing was successful, allocate the control
		theControl = new ECLabVIEW(tag, numTrialCPs, trialCPs, numOutCPs, outCPs, ipAddr, ipPort);

        if (theControl == 0)  {
            opserr << "WARNING could not create experimental control " << argv[1] << endln;
            return TCL_ERROR;
        }
        
        // now add the control to the modelBuilder
        if (addExperimentalControl(*theControl) < 0)  {
            delete theControl; // invoke the destructor, otherwise mem leak

            return TCL_ERROR;
        }
    }
	
    // ----------------------------------------------------------------------------	
/*    else if (strcmp(argv[1],"NIEseries") == 0)  {
		if (argc != 4)  {
			opserr << "WARNING invalid number of arguments\n";
			printCommand(argc,argv);
			opserr << "Want: expControl NIEseries tag device\n";
			return TCL_ERROR;
		}    
		
		int tag, device;
        ExperimentalControl *theControl = 0;
		
		if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK)  {
			opserr << "WARNING invalid expControl NIEseries tag\n";
			return TCL_ERROR;		
		}
		if (Tcl_GetInt(interp, argv[3], &device) != TCL_OK)  {
			opserr << "WARNING invalid device\n";
			opserr << "expControl NIEseries " << tag << endln;
			return TCL_ERROR;	
		}
		
		// parsing was successful, allocate the control
		theControl = new ECNIEseries(tag, device);

        if (theControl == 0)  {
            opserr << "WARNING could not create experimental control " << argv[1] << endln;
            return TCL_ERROR;
        }
        
        // now add the control to the modelBuilder
        if (addExperimentalControl(*theControl) < 0)  {
            delete theControl; // invoke the destructor, otherwise mem leak
            return TCL_ERROR;
        }
    }*/
#endif

    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"SCRAMNet") == 0)  {
		if (argc != 5)  {
			opserr << "WARNING invalid number of arguments\n";
			printCommand(argc,argv);
			opserr << "Want: expControl SCRAMNet tag memOffset numActCh\n";
			return TCL_ERROR;
		}    
		
		int tag, memOffset, numActCh;
        ExperimentalControl *theControl = 0;
		
		if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK)  {
			opserr << "WARNING invalid expControl SCRAMNet tag\n";
			return TCL_ERROR;		
		}
		if (Tcl_GetInt(interp, argv[3], &memOffset) != TCL_OK)  {
			opserr << "WARNING invalid memOffset\n";
			opserr << "expControl SCRAMNet " << tag << endln;
			return TCL_ERROR;
		}
		if (Tcl_GetInt(interp, argv[4], &numActCh) != TCL_OK)  {
			opserr << "WARNING invalid numActCh\n";
			opserr << "expControl SCRAMNet " << tag << endln;
			return TCL_ERROR;
		}
        		
		// parsing was successful, allocate the control
		theControl = new ECSCRAMNet(tag, memOffset, numActCh);

        if (theControl == 0)  {
            opserr << "WARNING could not create experimental control " << argv[1] << endln;
            return TCL_ERROR;
        }
        
        // now add the control to the modelBuilder
        if (addExperimentalControl(*theControl) < 0)  {
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
		
		int tag, matTag, numMats = 0, argi = 2;
        ECSimUniaxialMaterials *theControl = 0;
		
		if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
			opserr << "WARNING invalid expControl SimUniaxialMaterials tag\n";
			return TCL_ERROR;		
		}
        argi++;

		// parsing was successful, allocate the control
		theControl = new ECSimUniaxialMaterials(tag);	

		// now read the number of materials
		while (argi+numMats < argc)  {
			numMats++;
		}
		// add the materials to the control
		for (int i=0; i<numMats; i++)  {
			// read the material tag
			if (Tcl_GetInt(interp, argv[argi], &matTag) != TCL_OK)  {
				opserr << "WARNING invalid matTag\n";
				opserr << "expControl SimUniaxialMaterials " << tag << endln;
				return TCL_ERROR;
			} else  {
				// get a pointer to the material from the modelbuilder	    
				argi++;
				UniaxialMaterial *theMat = theTclBuilder->getUniaxialMaterial(matTag);
				if (theMat == 0)  {
					opserr << "WARNING no material " << matTag << " exists\n";
					opserr << "expControl SimUniaxialMaterials " << tag << endln;
					return TCL_ERROR;		
				} else  {
					// add the material
                    theControl->addDummySpecimen(theMat);
				}
			}
		}

        if (theControl == 0)  {
            opserr << "WARNING could not create experimental control " << argv[1] << endln;
            return TCL_ERROR;
        }
        
        // now add the control to the modelBuilder
        if (addExperimentalControl(*theControl) < 0)  {
            delete theControl; // invoke the destructor, otherwise mem leak
            return TCL_ERROR;
        }
    }
	
    // ----------------------------------------------------------------------------	
	else if (strcmp(argv[1],"SimDomain") == 0)  {
		if (argc < 7)  {
			opserr << "WARNING invalid number of arguments\n";
			printCommand(argc,argv);
			opserr << "Want: expControl SimDomain tag -trialCP cpTags -outCP cpTags\n";
			return TCL_ERROR;
		}    
		
		int tag, cpTag, argi = 2;
        int numTrialCPs = 0, numOutCPs = 0;
        ExperimentalControl *theControl = 0;
		
		if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
			opserr << "WARNING invalid expControl SimDomain tag\n";
			return TCL_ERROR;		
		}
        argi++;
        if (strcmp(argv[argi],"-trialCP") != 0)  {
		    opserr << "WARNING expecting -trialCP cpTags\n";
		    opserr << "expControl SimDomain " << tag << endln;
		    return TCL_ERROR;
        }
        argi++;
        while (strcmp(argv[argi+numTrialCPs],"-outCP") != 0 &&
            argi+numTrialCPs < argc)  {
            numTrialCPs++;
        }
        if (numTrialCPs == 0)  {
		    opserr << "WARNING no trialCPTags specified\n";
		    opserr << "expControl SimDomain " << tag << endln;
		    return TCL_ERROR;
	    }
        // create the array to hold the trial control points
        ExperimentalCP **trialCPs = new ExperimentalCP* [numTrialCPs];
        if (trialCPs == 0)  {
		    opserr << "WARNING out of memory\n";
		    opserr << "expControl SimDomain " << tag << endln;
		    return TCL_ERROR;
	    }
        for (int i=0; i<numTrialCPs; i++)  {
            if (Tcl_GetInt(interp, argv[argi], &cpTag) != TCL_OK)  {
                opserr << "WARNING invalid cpTag\n";
                opserr << "expControl SimDomain " << tag << endln;
                return TCL_ERROR;	
            }
            trialCPs[i] = getExperimentalCP(cpTag);
            if (trialCPs[i] == 0)  {
                opserr << "WARNING experimental control point not found\n";
                opserr << "expControlPoint " << cpTag << endln;
                opserr << "expControl SimDomain " << tag << endln;
                return TCL_ERROR;
            }
            argi++;
        }
        if (strcmp(argv[argi],"-outCP") != 0)  {
		    opserr << "WARNING expecting -outCP cpTags\n";
		    opserr << "expControl SimDomain " << tag << endln;
		    return TCL_ERROR;		
        }
        argi++;
        while (argi+numOutCPs < argc)  {
            numOutCPs++;
        }
        if (numOutCPs == 0)  {
		    opserr << "WARNING no outCPTags specified\n";
		    opserr << "expControl SimDomain " << tag << endln;
		    return TCL_ERROR;
	    }
        // create the array to hold the output control points
        ExperimentalCP **outCPs = new ExperimentalCP* [numOutCPs];
        if (outCPs == 0)  {
		    opserr << "WARNING out of memory\n";
		    opserr << "expControl SimDomain " << tag << endln;
		    return TCL_ERROR;
	    }
        for (int i=0; i<numOutCPs; i++)  {
            if (Tcl_GetInt(interp, argv[argi], &cpTag) != TCL_OK)  {
                opserr << "WARNING invalid cpTag\n";
                opserr << "expControl SimDomain " << tag << endln;
                return TCL_ERROR;
            }
            outCPs[i] = getExperimentalCP(cpTag);
            if (outCPs[i] == 0)  {
                opserr << "WARNING experimental control point not found\n";
                opserr << "expControlPoint " << cpTag << endln;
                opserr << "expControl SimDomain " << tag << endln;
                return TCL_ERROR;
            }
            argi++;
        }
		
		// parsing was successful, allocate the control
		theControl = new ECSimDomain(tag, numTrialCPs, trialCPs, numOutCPs, outCPs, theDomain);	

        if (theControl == 0)  {
            opserr << "WARNING could not create experimental control " << argv[1] << endln;
            return TCL_ERROR;
        }
        
        // now add the control to the modelBuilder
        if (addExperimentalControl(*theControl) < 0)  {
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
