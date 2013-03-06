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
// Description: This file contains the function invoked when the user
// invokes the expControl command in the interpreter. 

#include <string.h>
#include <tcl.h>
#include <ArrayOfTaggedObjects.h>
#include <Vector.h>
#include <elementAPI.h>

#include <ECSimUniaxialMaterials.h>
//#include <ECSimUniaxialMaterialsForce.h>
#include <ECSimDomain.h>
#include <ECSimFEAdapter.h>
#include <ECSimSimulink.h>
#ifdef _WIN32
#include <ECxPCtarget.h>
//#include <ECxPCtargetForce.h>
#include <ECdSpace.h>
#include <ECMtsCsi.h>
#include <ECLabVIEW.h>
//#include <ECNIEseries.h>
#include <ECSCRAMNet.h>
#include <ECGenericTCP.h>
#endif

extern ExperimentalCP *getExperimentalCP(int tag);
extern ExperimentalSignalFilter *getExperimentalSignalFilter(int tag);
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


extern int clearExperimentalControls(Tcl_Interp *interp)
{
    if (theExperimentalControls != 0) {
        theExperimentalControls->clearAll(false);
    }
    
    return 0;
}


static void printCommand(int argc, TCL_Char **argv)
{
    opserr << "Input command: ";
    for (int i=0; i<argc; i++)
        opserr << argv[i] << " ";
    opserr << endln;
} 


int TclExpControlCommand(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, Domain *theDomain)
{
    if (theExperimentalControls == 0)
        theExperimentalControls = new ArrayOfTaggedObjects(32);
    
    // make sure there is a minimum number of arguments
    if (argc < 3)  {
		opserr << "WARNING insufficient number of experimental control arguments\n";
		opserr << "Want: expControl type tag <specific experimental control args>\n";
		return TCL_ERROR;
    }
    
    // pointer to control that will be added
    ExperimentalControl *theControl = 0;
    
    int tag, argi;
    
	// ----------------------------------------------------------------------------	
	if (strcmp(argv[1],"SimUniaxialMaterials") == 0)  {
		if (argc < 4)  {
			opserr << "WARNING invalid number of arguments\n";
			printCommand(argc,argv);
			opserr << "Want: expControl SimUniaxialMaterials tag matTags "
	               << "<-ctrlFilters (5 filterTag)> <-daqFilters (5 filterTag)>\n";
			return TCL_ERROR;
		}    
        
		int matTag, numMats = 0;
        
        argi = 2;
		if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
			opserr << "WARNING invalid expControl SimUniaxialMaterials tag\n";
			return TCL_ERROR;		
		}
		argi++;
		// now read the number of materials
		while (argi+numMats < argc &&
			strcmp(argv[argi+numMats],"-ctrlFilters") != 0 &&
			strcmp(argv[argi+numMats],"-daqFilters") != 0)  {
				numMats++;
		}
		if (numMats == 0)  {
		    opserr << "WARNING no uniaxial materials specified\n";
		    opserr << "expControl SimUniaxialMaterials " << tag << endln;
		    return TCL_ERROR;
	    }
		// create the array to hold the uniaxial materials
	    UniaxialMaterial **theSpecimen = new UniaxialMaterial* [numMats];
	    if (theSpecimen == 0)  {
		    opserr << "WARNING out of memory\n";
		    opserr << "expControl SimUniaxialMaterials " << tag << endln;
		    return TCL_ERROR;
	    }
        for (int i=0; i<numMats; i++)  {
            theSpecimen[i] = 0;
            if (Tcl_GetInt(interp, argv[argi], &matTag) != TCL_OK)  {
                opserr << "WARNING invalid matTag\n";
                opserr << "expControl SimUniaxialMaterials " << tag << endln;
                return TCL_ERROR;	
            }
            theSpecimen[i] = OPS_GetUniaxialMaterial(matTag);
            if (theSpecimen[i] == 0)  {
				opserr << "WARNING uniaxial material not found\n";
	            opserr << "uniaxialMaterial " << matTag << endln;
	            opserr << "expControl SimUniaxialMaterials " << tag << endln;
	            return TCL_ERROR;
	        }
	        argi++;
		}
        
		// parsing was successful, allocate the control
		theControl = new ECSimUniaxialMaterials(tag, numMats, theSpecimen);
        
        // cleanup dynamic memory
        if (theSpecimen != 0)
		    delete [] theSpecimen;
    }
	
	// ----------------------------------------------------------------------------	
	/*else if (strcmp(argv[1],"SimUniaxialMaterialsForce") == 0)  {
		if (argc < 4)  {
			opserr << "WARNING invalid number of arguments\n";
			printCommand(argc,argv);
			opserr << "Want: expControl SimUniaxialMaterialsForce tag matTags "
	               << "<-ctrlFilters (5 filterTag)> <-daqFilters (5 filterTag)>\n";
			return TCL_ERROR;
		}    
        
		int matTag, numMats = 0;
        
        argi = 2;
		if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
			opserr << "WARNING invalid expControl SimUniaxialMaterialsForce tag\n";
			return TCL_ERROR;		
		}
		argi++;
		// now read the number of materials
		while (argi+numMats < argc &&
			strcmp(argv[argi+numMats],"-ctrlFilters") != 0 &&
			strcmp(argv[argi+numMats],"-daqFilters") != 0)  {
				numMats++;
		}
		if (numMats == 0)  {
		    opserr << "WARNING no uniaxial materials specified\n";
		    opserr << "expControl SimUniaxialMaterialsForce " << tag << endln;
		    return TCL_ERROR;
	    }
		// create the array to hold the uniaxial materials
	    UniaxialMaterial **theSpecimen = new UniaxialMaterial* [numMats];
	    if (theSpecimen == 0)  {
		    opserr << "WARNING out of memory\n";
		    opserr << "expControl SimUniaxialMaterialsForce " << tag << endln;
		    return TCL_ERROR;
	    }
        for (int i=0; i<numMats; i++)  {
            theSpecimen[i] = 0;
            if (Tcl_GetInt(interp, argv[argi], &matTag) != TCL_OK)  {
                opserr << "WARNING invalid matTag\n";
                opserr << "expControl SimUniaxialMaterialsForce " << tag << endln;
                return TCL_ERROR;	
            }
            theSpecimen[i] = OPS_GetUniaxialMaterial(matTag);
            if (theSpecimen[i] == 0)  {
				opserr << "WARNING uniaxial material not found\n";
	            opserr << "uniaxialMaterial " << matTag << endln;
	            opserr << "expControl SimUniaxialMaterialsForce " << tag << endln;
	            return TCL_ERROR;
	        }
	        argi++;
		}
        
		// parsing was successful, allocate the control
		theControl = new ECSimUniaxialMaterialsForce(tag, numMats, theSpecimen);
        
        // cleanup dynamic memory
        if (theSpecimen != 0)
		    delete [] theSpecimen;
    }*/
	
    // ----------------------------------------------------------------------------	
	else if (strcmp(argv[1],"SimDomain") == 0)  {
		if (argc < 7)  {
			opserr << "WARNING invalid number of arguments\n";
			printCommand(argc,argv);
			opserr << "Want: expControl SimDomain tag -trialCP cpTags -outCP cpTags "
                   << "<-ctrlFilters (5 filterTag)> <-daqFilters (5 filterTag)>\n";
			return TCL_ERROR;
		}    
        
		int cpTag, i;
		int numTrialCPs = 0, numOutCPs = 0;
        
	    argi = 2;
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
		while (argi+numTrialCPs < argc &&
			strcmp(argv[argi+numTrialCPs],"-outCP") != 0)  {
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
		for (i=0; i<numTrialCPs; i++)  {
			trialCPs[i] = 0;
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
		while (argi+numOutCPs < argc &&
			strcmp(argv[argi+numOutCPs],"-ctrlFilters") != 0 &&
			strcmp(argv[argi+numOutCPs],"-daqFilters") != 0)  {
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
		for (i=0; i<numOutCPs; i++)  {
			outCPs[i] = 0;
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
		theControl = new ECSimDomain(tag, numTrialCPs, trialCPs,
            numOutCPs, outCPs, theDomain);
    }
	// ----------------------------------------------------------------------------	
	else if (strcmp(argv[1],"SimFEAdapter") == 0)  {
		if (argc < 5)  {
			opserr << "WARNING invalid number of arguments\n";
			printCommand(argc,argv);
			opserr << "Want: expControl SimFEAdapter tag ipAddr ipPort "
                << "<-ctrlFilters (5 filterTag)> <-daqFilters (5 filterTag)>\n";
			return TCL_ERROR;
		}    
        
		int ipPort = 44000;
		char *ipAddr;
        
        argi = 2;
		if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
			opserr << "WARNING invalid expControl SimFEAdapter tag\n";
			return TCL_ERROR;
		}
        argi++;
        ipAddr = new char [strlen(argv[argi])+1];
		strcpy(ipAddr,argv[argi]);
        argi++;
	    if (Tcl_GetInt(interp, argv[argi], &ipPort) != TCL_OK)  {
		    opserr << "WARNING invalid ipPort\n";
		    opserr << "expControl SimFEAdapter " << tag << endln;
		    return TCL_ERROR;
	    }
        argi++;
        
		// parsing was successful, allocate the control
		theControl = new ECSimFEAdapter(tag, ipAddr, ipPort);	
    }
	// ----------------------------------------------------------------------------	
	else if (strcmp(argv[1],"SimSimulink") == 0)  {
		if (argc < 5)  {
			opserr << "WARNING invalid number of arguments\n";
			printCommand(argc,argv);
			opserr << "Want: expControl SimSimulink tag ipAddr ipPort "
                << "<-ctrlFilters (5 filterTag)> <-daqFilters (5 filterTag)>\n";
			return TCL_ERROR;
		}    
        
		int ipPort = 44000;
		char *ipAddr;
        
        argi = 2;
		if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
			opserr << "WARNING invalid expControl SimSimulink tag\n";
			return TCL_ERROR;
		}
        argi++;
        ipAddr = new char [strlen(argv[argi])+1];
		strcpy(ipAddr,argv[argi]);
        argi++;
	    if (Tcl_GetInt(interp, argv[argi], &ipPort) != TCL_OK)  {
		    opserr << "WARNING invalid ipPort\n";
		    opserr << "expControl SimSimulink " << tag << endln;
		    return TCL_ERROR;
	    }
        argi++;
        
		// parsing was successful, allocate the control
		theControl = new ECSimSimulink(tag, ipAddr, ipPort);	
    }
	
#ifdef _WIN32
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"xPCtarget") == 0)  {
		if (argc < 7)  {
			opserr << "WARNING invalid number of arguments\n";
			printCommand(argc,argv);
			opserr << "Want: expControl xPCtarget tag type ipAddr ipPort appName <appPath> "
                << "<-ctrlFilters (5 filterTag)> <-daqFilters (5 filterTag)>\n";
			return TCL_ERROR;
		}    
		
		int type, i, timeOut = 10;
		char *ipAddr, *ipPort, *appName, *appPath = 0;
		
        argi = 2;
		if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
			opserr << "WARNING invalid expControl xPCtarget tag\n";
			return TCL_ERROR;		
		}
        argi++;
		if (Tcl_GetInt(interp, argv[argi], &type) != TCL_OK)  {
			opserr << "WARNING invalid type\n";
			opserr << "expControl xPCtarget " << tag << endln;
			return TCL_ERROR;	
		}
        argi++;
        ipAddr = new char [strlen(argv[argi])+1];
		strcpy(ipAddr,argv[argi]);
        argi++;
        ipPort = new char [strlen(argv[argi])+1];
		strcpy(ipPort,argv[argi]);
        argi++;
        appName = new char [strlen(argv[argi])+1];
		strcpy(appName,argv[argi]);
        argi++;
        if (argc > 7 &&
            strcmp(argv[argi],"-ctrlFilters") != 0 &&
            strcmp(argv[argi],"-daqFilters") != 0 &&
            strcmp(argv[argi],"-timeOut") != 0)  {
            appPath = new char [strlen(argv[argi])+1];
            strcpy(appPath,argv[argi]);
            argi++;
        }
        for (i = argi; i < argc; i++)  {
            if (i+1 < argc && strcmp(argv[i], "-timeOut") == 0)  {
                if (Tcl_GetInt(interp, argv[i+1], &timeOut) != TCL_OK)  {
                    opserr << "WARNING invalid timeOut value\n";
                    opserr << "expControl xPCtarget " << tag << endln;
                    return TCL_ERROR;	
                }
            }
        }
		
		// parsing was successful, allocate the control
		theControl = new ECxPCtarget(tag, type, ipAddr, ipPort,
            appName, appPath, timeOut);
    }
	
    // ----------------------------------------------------------------------------	
    /*else if (strcmp(argv[1],"xPCtargetForce") == 0)  {
		if (argc < 7)  {
			opserr << "WARNING invalid number of arguments\n";
			printCommand(argc,argv);
			opserr << "Want: expControl xPCtargetForce tag ipAddr ipPort appName <appPath> "
                << "<-ctrlFilters (5 filterTag)> <-daqFilters (5 filterTag)>\n";
			return TCL_ERROR;
		}    
		
		char *ipAddr, *ipPort, *appName, *appPath = 0;
		
        argi = 2;
		if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
			opserr << "WARNING invalid expControl xPCtargetForce tag\n";
			return TCL_ERROR;		
		}
        argi++;
        ipAddr = new char [strlen(argv[argi])+1];
		strcpy(ipAddr,argv[argi]);
        argi++;
        ipPort = new char [strlen(argv[argi])+1];
		strcpy(ipPort,argv[argi]);
        argi++;
        appName = new char [strlen(argv[argi])+1];
		strcpy(appName,argv[argi]);
        argi++;
        if (argc > 7 &&
            strcmp(argv[argi],"-ctrlFilters") != 0 &&
            strcmp(argv[argi],"-daqFilters") != 0)  {
            appPath = new char [strlen(argv[argi])+1];
            strcpy(appPath,argv[argi]);
            argi++;
        }
		
		// parsing was successful, allocate the control
		theControl = new ECxPCtargetForce(tag, ipAddr, ipPort, appName, appPath);
    }*/
	
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"dSpace") == 0)  {
		if (argc < 5)  {
			opserr << "WARNING invalid number of arguments\n";
			printCommand(argc,argv);
			opserr << "Want: expControl dSpace tag type boardName "
                << "<-ctrlFilters (5 filterTag)> <-daqFilters (5 filterTag)>\n";
			return TCL_ERROR;
		}    
		
		int type;
		char *boardName;
		
        argi = 2;
		if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
			opserr << "WARNING invalid expControl dSpace tag\n";
			return TCL_ERROR;
		}
        argi++;
		if (Tcl_GetInt(interp, argv[argi], &type) != TCL_OK)  {
			opserr << "WARNING invalid type\n";
			opserr << "expControl dSpace " << tag << endln;
			return TCL_ERROR;
		}
        argi++;
        boardName = new char [strlen(argv[argi])+1];
		strcpy(boardName,argv[argi]);
        argi++;
		
		// parsing was successful, allocate the control
		theControl = new ECdSpace(tag, type, boardName);
    }

    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"MTSCsi") == 0)  {
		if (argc < 4)  {
			opserr << "WARNING invalid number of arguments\n";
			printCommand(argc,argv);
			opserr << "Want: expControl MTSCsi tag configFileName <rampTime> "
                << "<-ctrlFilters (5 filterTag)> <-daqFilters (5 filterTag)>\n";
			return TCL_ERROR;
		}    
		
        char *cfgFile;
        double rampTime = 0.02;
        
        argi = 2;
		if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
			opserr << "WARNING invalid expControl MTSCsi tag\n";
			return TCL_ERROR;
		}
        argi++;
        cfgFile = new char [strlen(argv[argi])+1];
		strcpy(cfgFile,argv[argi]);
        argi++;
        if (argc > 4 &&
            strcmp(argv[argi],"-ctrlFilters") != 0 &&
            strcmp(argv[argi],"-daqFilters") != 0)  {
            if (Tcl_GetDouble(interp, argv[argi], &rampTime) != TCL_OK)  {
                opserr << "WARNING invalid rampTime\n";
                opserr << "expControl MTSCsi " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
        }
		// parsing was successful, allocate the control
		theControl = new ECMtsCsi(tag, cfgFile, rampTime);
    }

    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"LabVIEW") == 0)  {
		if (argc < 8)  {
			opserr << "WARNING invalid number of arguments\n";
			printCommand(argc,argv);
			opserr << "Want: expControl LabVIEW tag ipAddr <ipPort> -trialCP cpTags -outCP cpTags "
                << "<-ctrlFilters (5 filterTag)> <-daqFilters (5 filterTag)>\n";
			return TCL_ERROR;
		}
        
		int cpTag, ipPort = 44000;
        int numTrialCPs = 0, numOutCPs = 0;
		char *ipAddr;
		
        argi = 2;
		if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
			opserr << "WARNING invalid expControl LabVIEW tag\n";
			return TCL_ERROR;
		}
        argi++;
        ipAddr = new char [strlen(argv[argi])+1];
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
        while (argi+numTrialCPs < argc &&
            strcmp(argv[argi+numTrialCPs],"-outCP") != 0 &&
            strcmp(argv[argi+numTrialCPs],"-ctrlFilters") != 0 &&
            strcmp(argv[argi+numTrialCPs],"-daqFilters") != 0)  {
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
            trialCPs[i] = 0;
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
        while (argi+numOutCPs < argc &&
            strcmp(argv[argi+numOutCPs],"-ctrlFilters") != 0 &&
            strcmp(argv[argi+numOutCPs],"-daqFilters") != 0)  {
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
            outCPs[i] = 0;
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
		theControl = new ECLabVIEW(tag, numTrialCPs, trialCPs,
            numOutCPs, outCPs, ipAddr, ipPort);
    }
	
    // ----------------------------------------------------------------------------	
/*    else if (strcmp(argv[1],"NIEseries") == 0)  {
		if (argc < 4)  {
			opserr << "WARNING invalid number of arguments\n";
			printCommand(argc,argv);
			opserr << "Want: expControl NIEseries tag device ";
                << "<-ctrlFilters (5 filterTag)> <-daqFilters (5 filterTag)>\n";
			return TCL_ERROR;
		}    
		
		int device;
		
        argi = 2;
		if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
			opserr << "WARNING invalid expControl NIEseries tag\n";
			return TCL_ERROR;		
		}
        argi++;
		if (Tcl_GetInt(interp, argv[argi], &device) != TCL_OK)  {
			opserr << "WARNING invalid device\n";
			opserr << "expControl NIEseries " << tag << endln;
			return TCL_ERROR;	
		}
        argi++;
		
		// parsing was successful, allocate the control
		theControl = new ECNIEseries(tag, device);
    }*/

    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"SCRAMNet") == 0)  {
		if (argc < 5)  {
			opserr << "WARNING invalid number of arguments\n";
			printCommand(argc,argv);
			opserr << "Want: expControl SCRAMNet tag memOffset numActCh "
                << "<-ctrlFilters (5 filterTag)> <-daqFilters (5 filterTag)>\n";
			return TCL_ERROR;
		}    
		
		int memOffset, numActCh;
		
        argi = 2;
		if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
			opserr << "WARNING invalid expControl SCRAMNet tag\n";
			return TCL_ERROR;		
		}
        argi++;
		if (Tcl_GetInt(interp, argv[argi], &memOffset) != TCL_OK)  {
			opserr << "WARNING invalid memOffset\n";
			opserr << "expControl SCRAMNet " << tag << endln;
			return TCL_ERROR;
		}
        argi++;
		if (Tcl_GetInt(interp, argv[argi], &numActCh) != TCL_OK)  {
			opserr << "WARNING invalid numActCh\n";
			opserr << "expControl SCRAMNet " << tag << endln;
			return TCL_ERROR;
		}
        argi++;
        
		// parsing was successful, allocate the control
		theControl = new ECSCRAMNet(tag, memOffset, numActCh);
    }
#endif

    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"GenericTCP") == 0)  {
		if (argc < 8)  {
			opserr << "WARNING invalid number of arguments\n";
			printCommand(argc,argv);
			opserr << "Want: expControl GenericTCP tag ipAddr ipPort -ctrlModes (5 mode) -daqModes (5 mode) "
                << "<-initFile fileName> <-ssl> <-udp> <-ctrlFilters (5 filterTag)> <-daqFilters (5 filterTag)>\n";
			return TCL_ERROR;
		}
        
		char *ipAddr;
		int ipPort, mode, i;
        ID ctrlModes(5), daqModes(5);
        char *initFileName = 0;
        int ssl = 0, udp = 0;
		
        argi = 2;
		if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
			opserr << "WARNING invalid expControl GenericTCP tag\n";
			return TCL_ERROR;
		}
        argi++;
        ipAddr = new char [strlen(argv[argi])+1];
		strcpy(ipAddr,argv[argi]);
        argi++;
        if (Tcl_GetInt(interp, argv[argi], &ipPort) != TCL_OK)  {
            opserr << "WARNING invalid ipPort\n";
            opserr << "expControl GenericTCP " << tag << endln;
            return TCL_ERROR;
        }
        argi++;
        if (strcmp(argv[argi],"-ctrlModes") != 0)  {
		    opserr << "WARNING expecting -ctrlModes (5 mode)\n";
		    opserr << "expControl GenericTCP " << tag << endln;
		    return TCL_ERROR;
        }
        argi++;
        for (i=0; i<5; i++)  {
            if (Tcl_GetInt(interp, argv[argi], &mode) != TCL_OK)  {
                opserr << "WARNING invalid mode\n";
                opserr << "expControl GenericTCP " << tag << endln;
                return TCL_ERROR;	
            }
            ctrlModes(i) = mode;
            argi++;
        }
        if (strcmp(argv[argi],"-daqModes") != 0)  {
		    opserr << "WARNING expecting -daqModes (5 mode)\n";
		    opserr << "expControl GenericTCP " << tag << endln;
		    return TCL_ERROR;
        }
        argi++;
        for (i=0; i<5; i++)  {
            if (Tcl_GetInt(interp, argv[argi], &mode) != TCL_OK)  {
                opserr << "WARNING invalid mode\n";
                opserr << "expControl GenericTCP " << tag << endln;
                return TCL_ERROR;	
            }
            daqModes(i) = mode;
            argi++;
        }
        for (i = argi; i < argc; i++)  {
            if (strcmp(argv[i], "-initFile") == 0)  {
                initFileName = new char [strlen(argv[i+1])+1];
                strcpy(initFileName,argv[i+1]);
            }
        }
        for (i = argi; i < argc; i++)  {
            if (strcmp(argv[i], "-ssl") == 0)
                ssl = 1;
            else if (strcmp(argv[i], "-udp") == 0)
                udp = 1;
        }
        
		// parsing was successful, allocate the control
		theControl = new ECGenericTCP(tag, ipAddr, ipPort,
            ctrlModes, daqModes, initFileName, ssl, udp);
    }

    // ----------------------------------------------------------------------------	
    else  {
        // experimental control type not recognized
        opserr << "WARNING unknown experimental control type: "
            <<  argv[1] << ": check the manual\n";
        return TCL_ERROR;
    }
    
    if (theControl == 0)  {
        opserr << "WARNING could not create experimental control " << argv[1] << endln;
        return TCL_ERROR;
    }
    
    // finally check for filters
    int i;
    for (i = argi; i < argc; i++)  {
        if (i+5 < argc && strcmp(argv[i], "-ctrlFilters") == 0)  {
            int filterTag;
            ExperimentalSignalFilter *theFilter = 0;
            for (int j=0; j<5; j++)  {
                if (Tcl_GetInt(interp, argv[i+1+j], &filterTag) != TCL_OK)  {
                    opserr << "WARNING invalid ctrlFilter tag\n";
                    opserr << "expControl: " << tag << endln;
                    return TCL_ERROR;
                }
                if (filterTag > 0)  {
                    theFilter = getExperimentalSignalFilter(filterTag);
                    if (theFilter == 0)  {
                        opserr << "WARNING experimental signal filter not found\n";
                        opserr << "expSignalFilter: " << filterTag << endln;
                        opserr << "expControl: " << tag << endln;
                        return TCL_ERROR;
                    }
                    theControl->setCtrlFilter(theFilter,j);
                }
            }
        }
    }
    for (i = argi; i < argc; i++)  {
        if (i+5 < argc && strcmp(argv[i], "-daqFilters") == 0)  {
            int filterTag;
            ExperimentalSignalFilter *theFilter = 0;
            for (int j=0; j<5; j++)  {
                if (Tcl_GetInt(interp, argv[i+1+j], &filterTag) != TCL_OK)  {
                    opserr << "WARNING invalid daqFilter tag\n";
                    opserr << "expControl: " << tag << endln;
                    return TCL_ERROR;
                }
                if (filterTag > 0)  {
                    theFilter = getExperimentalSignalFilter(filterTag);
                    if (theFilter == 0)  {
                        opserr << "WARNING experimental signal filter not found\n";
                        opserr << "expSignalFilter: " << filterTag << endln;
                        opserr << "expControl: " << tag << endln;
                        return TCL_ERROR;
                    }
                    theControl->setDaqFilter(theFilter,j);
                }
            }
        }
    }
    
    // now add the control to the modelBuilder
    if (addExperimentalControl(*theControl) < 0)  {
        delete theControl; // invoke the destructor, otherwise mem leak
        return TCL_ERROR;
    }
    
	return TCL_OK;
}
