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
// invokes the expSetup command in the interpreter. 

#include <TclModelBuilder.h>
#include <ArrayOfTaggedObjects.h>

#include <ESNoTransformation.h>
#include <ESOneActuator.h>
#include <ESTwoActuators2d.h>
#include <ESThreeActuators2d.h>
#include <ESThreeActuatorsJntOff2d.h>
#include <ESInvertedVBrace2d.h>
#include <ESInvertedVBraceJntOff2d.h>
#include <ESAggregator.h>
#include <ESFourActuators3d.h>

#include <ID.h>
#include <Vector.h>
#include <string.h>

extern ExperimentalControl *getExperimentalControl(int tag);
static ArrayOfTaggedObjects *theExperimentalSetups(0);


int addExperimentalSetup(ExperimentalSetup &theSetup)
{
    bool result = theExperimentalSetups->addComponent(&theSetup);
    if (result == true)
        return 0;
    else {
        opserr << "addExperimentalSetup() - "
            << "failed to add experimental setup: " << theSetup;
        return -1;
    }
}


extern ExperimentalSetup *getExperimentalSetup(int tag)
{
    if (theExperimentalSetups == 0) {
        opserr << "getExperimentalSetup() - "
            << "failed to get experimental setup: " << tag << endln
            << "no experimental setup objects have been defined\n";
        return 0;
    }

    TaggedObject *mc = theExperimentalSetups->getComponentPtr(tag);
    if (mc == 0) 
        return 0;

    // otherwise we do a cast and return
    ExperimentalSetup *result = (ExperimentalSetup *)mc;
    return result;
}


static void printCommand(int argc, TCL_Char **argv)
{
    opserr << "Input command: ";
    for (int i=0; i<argc; i++)
        opserr << argv[i] << " ";
    opserr << endln;
} 


int TclExpSetupCommand(ClientData clientData, Tcl_Interp *interp, int argc,
    TCL_Char **argv, Domain *theDomain, TclModelBuilder *theTclBuilder)
{
    if (theExperimentalSetups == 0)
        theExperimentalSetups = new ArrayOfTaggedObjects(32);

    // make sure there is a minimum number of arguments
    if (argc < 3)  {
        opserr << "WARNING insufficient number of experimental setup arguments\n";
        opserr << "Want: expSetup type tag <specific experimental setup args>\n";
        return TCL_ERROR;
    }
    
    // pointer to an experimental setup that will be added
    ExperimentalSetup *theSetup = 0;
    ExperimentalControl *theControl = 0;

    int ndm = theTclBuilder->getNDM();
    int tag, argi;
    
    // ----------------------------------------------------------------------------	
    if (strcmp(argv[1],"NoTransformation") == 0)  {
        if (argc < 5)  {
            opserr << "WARNING invalid number of arguments\n";
            printCommand(argc,argv);
            opserr << "Want: expSetup NoTransformation tag <-control ctrlTag> -dir dirs "
                << "<-ctrlDispFact f> <-ctrlVelFact f> <-ctrlAccelFact f> "
                << "<-ctrlForceFact f> <-ctrlTimeFact f> "
                << "<-daqDispFact f> <-daqVelFact f> <-daqAccelFact f> "
                << "<-daqForceFact f> <-daqTimeFact f>\n";
            return TCL_ERROR;
        }    
        
        int ctrlTag, numDirs = 0, dirID, i;
        
        argi = 2;
        if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
            opserr << "WARNING invalid expSetup NoTransformation tag\n";
            return TCL_ERROR;		
        }
        argi++;
        if (strcmp(argv[argi],"-control") == 0)  {
            argi++;
            if (Tcl_GetInt(interp, argv[argi], &ctrlTag) != TCL_OK)  {
                opserr << "WARNING invalid ctrlTag\n";
                opserr << "expSetup NoTransformation " << tag << endln;
                return TCL_ERROR;	
            }
            theControl = getExperimentalControl(ctrlTag);
            if (theControl == 0)  {
                opserr << "WARNING experimental control not found\n";
                opserr << "expControl: " << ctrlTag << endln;
                opserr << "expSetup NoTransformation " << tag << endln;
                return TCL_ERROR;
            }
            argi++;
        }
        // now read the direction IDs
        if (strcmp(argv[argi],"-dir") != 0)  {
            opserr << "WARNING expecting -dir dirs\n";
            opserr << "expSetup NoTransformation " << tag << endln;
            return TCL_ERROR;	
        }
        argi++;
        i = argi;
        while ((i < argc) && (strcmp(argv[i],"-ctrlDispFact") != 0)
            && (strcmp(argv[i],"-ctrlVelFact") != 0)
            && (strcmp(argv[i],"-ctrlAccelFact") != 0)
            && (strcmp(argv[i],"-ctrlForceFact") != 0)
            && (strcmp(argv[i],"-ctrlTimeFact") != 0)
            && (strcmp(argv[i],"-daqDispFact") != 0)
            && (strcmp(argv[i],"-daqVelFact") != 0)
            && (strcmp(argv[i],"-daqAccelFact") != 0)
            && (strcmp(argv[i],"-daqForceFact") != 0)
            && (strcmp(argv[i],"-daqTimeFact") != 0))  {
            numDirs++;
            i++;
        }
        if (numDirs == 0)  {
		    opserr << "WARNING no dirs specified\n";
		    opserr << "expSetup NoTransformation " << tag << endln;
		    return TCL_ERROR;
	    }
        // create an ID array to hold the direction IDs
        ID theDirIDs(numDirs);
        // read the dir identifiers
        for (i=0; i<numDirs; i++)  {
            if (Tcl_GetInt(interp, argv[argi], &dirID) != TCL_OK)  {
                opserr << "WARNING invalid direction ID\n";
                opserr << "expSetup NoTransformation " << tag << endln;
                return TCL_ERROR;	
            }
            theDirIDs[i] = dirID - 1;
            argi++;
        }
        
        // parsing was successful, allocate the setup
        theSetup = new ESNoTransformation(tag, theDirIDs, theControl);
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"OneActuator") == 0)  {
        if (argc < 4)  {
            opserr << "WARNING invalid number of arguments\n";
            printCommand(argc,argv);
            opserr << "Want: expSetup OneActuator tag <-control ctrlTag> dir "
                << "<-ctrlDispFact f> <-ctrlVelFact f> <-ctrlAccelFact f> "
                << "<-ctrlForceFact f> <-ctrlTimeFact f> "
                << "<-daqDispFact f> <-daqVelFact f> <-daqAccelFact f> "
                << "<-daqForceFact f> <-daqTimeFact f>\n";
            return TCL_ERROR;
        }    
        
        int ctrlTag, dir;
        
        argi = 2;
        if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
            opserr << "WARNING invalid expSetup OneActuator tag\n";
            return TCL_ERROR;		
        }
        argi++;
        if (strcmp(argv[argi],"-control") == 0)  {
            argi++;
            if (Tcl_GetInt(interp, argv[argi], &ctrlTag) != TCL_OK)  {
                opserr << "WARNING invalid ctrlTag\n";
                opserr << "expSetup OneActuator " << tag << endln;
                return TCL_ERROR;	
            }
            theControl = getExperimentalControl(ctrlTag);
            if (theControl == 0)  {
                opserr << "WARNING experimental control not found\n";
                opserr << "expControl: " << ctrlTag << endln;
                opserr << "expSetup OneActuator " << tag << endln;
                return TCL_ERROR;
            }
            argi++;
        }
        if (Tcl_GetInt(interp, argv[argi], &dir) != TCL_OK)  {
            opserr << "WARNING invalid dir\n";
            opserr << "expSetup OneActuator " << tag << endln;
            return TCL_ERROR;	
        }
        
        // parsing was successful, allocate the setup
        theSetup = new ESOneActuator(tag, dir-1, theControl);
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"TwoActuators") == 0)  {        
        if (ndm == 2 || ndm == 3)  {
            if (argc < 6)  {
                opserr << "WARNING invalid number of arguments\n";
                printCommand(argc,argv);
                opserr << "Want: expSetup TwoActuators tag <-control ctrlTag> La1 La2 L "
                    << "<-nlGeom> <-posAct pos> <-phiLocX phi> "
                    << "<-ctrlDispFact f> <-ctrlVelFact f> <-ctrlAccelFact f> "
                    << "<-ctrlForceFact f> <-ctrlTimeFact f> "
                    << "<-daqDispFact f> <-daqVelFact f> <-daqAccelFact f> "
                    << "<-daqForceFact f> <-daqTimeFact f>\n";
                return TCL_ERROR;
            }    
            
            int ctrlTag, i;
            double La1, La2, L;
            int nlGeom = 0;
            char posAct[6] = {'l','e','f','t','\0'};
            double phiLocX = 0.0;
            
            argi = 2;
            if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
                opserr << "WARNING invalid expSetup TwoActuators tag\n";
                return TCL_ERROR;		
            }
            argi++;
            if (strcmp(argv[argi],"-control") == 0)  {
                argi++;
                if (Tcl_GetInt(interp, argv[argi], &ctrlTag) != TCL_OK)  {
                    opserr << "WARNING invalid ctrlTag\n";
                    opserr << "expSetup TwoActuators " << tag << endln;
                    return TCL_ERROR;	
                }
                theControl = getExperimentalControl(ctrlTag);
                if (theControl == 0)  {
                    opserr << "WARNING experimental control not found\n";
                    opserr << "expControl: " << ctrlTag << endln;
                    opserr << "expSetup TwoActuators " << tag << endln;
                    return TCL_ERROR;
                }
                argi++;
            }
            if (Tcl_GetDouble(interp, argv[argi], &La1) != TCL_OK)  {
                opserr << "WARNING invalid La1\n";
                opserr << "expSetup TwoActuators " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &La2) != TCL_OK)  {
                opserr << "WARNING invalid La2\n";
                opserr << "expSetup TwoActuators " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L) != TCL_OK)  {
                opserr << "WARNING invalid L\n";
                opserr << "expSetup TwoActuators " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
		    for (i = argi; i < argc; i++)  {
			    if (strcmp(argv[i], "-nlGeom") == 0)  {
                    nlGeom = 1;
			    }
		    }
		    for (i = argi; i < argc; i++)  {
			    if (strcmp(argv[i], "-posAct") == 0)  {
                    if (strcmp(argv[i+1], "left") == 0 || strcmp(argv[i+1], "l") == 0)
                        strcpy(posAct, "left");
                    else if (strcmp(argv[i+1], "right") == 0 || strcmp(argv[i+1], "r") == 0)
                        strcpy(posAct, "right");
			    }
		    }
            for (i = argi; i < argc; i++)  {
                if (strcmp(argv[i], "-phiLocX") == 0)  {
                    if (Tcl_GetDouble(interp, argv[i+1], &phiLocX) != TCL_OK)  {
                        opserr << "WARNING invalid phiLocX\n";
                        opserr << "expSetup TwoActuators " << tag << endln;
                        return TCL_ERROR;	
                    }                
                }
            }

            // parsing was successful, allocate the setup
            theSetup = new ESTwoActuators2d(tag, La1, La2, L, theControl, nlGeom, posAct, phiLocX);
        }
        
        else if (ndm == 3)  {
            // not implemented yet
            opserr << "WARNING expSetup TwoActuators command not implemented yet for ndm = 3\n";
            return TCL_ERROR;
        }
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"ThreeActuators") == 0)  {
        if (ndm == 2 || ndm == 3)  {
            if (argc < 8)  {
                opserr << "WARNING invalid number of arguments\n";
                printCommand(argc,argv);
                opserr << "Want: expSetup ThreeActuators tag <-control ctrlTag> La1 La2 La3 L1 L2 "
                    << "<-nlGeom> <-posAct1 pos> <-phiLocX phi> "
                    << "<-ctrlDispFact f> <-ctrlVelFact f> <-ctrlAccelFact f> "
                    << "<-ctrlForceFact f> <-ctrlTimeFact f> "
                    << "<-daqDispFact f> <-daqVelFact f> <-daqAccelFact f> "
                    << "<-daqForceFact f> <-daqTimeFact f>\n";
                return TCL_ERROR;
            }    
            
            int ctrlTag, i;
            double La1, La2, La3, L1, L2;
            int nlGeom = 0;
            char posAct0[6] = {'l','e','f','t','\0'};
            double phiLocX = 0.0;
            
            argi = 2;
            if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
                opserr << "WARNING invalid expSetup ThreeActuators tag\n";
                return TCL_ERROR;		
            }
            argi++;
            if (strcmp(argv[argi],"-control") == 0)  {
                argi++;
                if (Tcl_GetInt(interp, argv[argi], &ctrlTag) != TCL_OK)  {
                    opserr << "WARNING invalid ctrlTag\n";
                    opserr << "expSetup ThreeActuators " << tag << endln;
                    return TCL_ERROR;	
                }
                theControl = getExperimentalControl(ctrlTag);
                if (theControl == 0)  {
                    opserr << "WARNING experimental control not found\n";
                    opserr << "expControl: " << ctrlTag << endln;
                    opserr << "expSetup ThreeActuators " << tag << endln;
                    return TCL_ERROR;
                }
                argi++;
            }
            if (Tcl_GetDouble(interp, argv[argi], &La1) != TCL_OK)  {
                opserr << "WARNING invalid La1\n";
                opserr << "expSetup ThreeActuators " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &La2) != TCL_OK)  {
                opserr << "WARNING invalid La2\n";
                opserr << "expSetup ThreeActuators " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &La3) != TCL_OK)  {
                opserr << "WARNING invalid La3\n";
                opserr << "expSetup ThreeActuators " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L1) != TCL_OK)  {
                opserr << "WARNING invalid L1\n";
                opserr << "expSetup ThreeActuators " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L2) != TCL_OK)  {
                opserr << "WARNING invalid L2\n";
                opserr << "expSetup ThreeActuators " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
		    for (i = argi; i < argc; i++)  {
			    if (strcmp(argv[i], "-nlGeom") == 0)  {
                    nlGeom = 1;
			    }
		    }
		    for (i = argi; i < argc; i++)  {
			    if (strcmp(argv[i], "-posAct1") == 0)  {
                    if (strcmp(argv[i+1], "left") == 0 || strcmp(argv[i+1], "l") == 0)
                        strcpy(posAct0, "left");
                    else if (strcmp(argv[i+1], "right") == 0 || strcmp(argv[i+1], "r") == 0)
                        strcpy(posAct0, "right");
			    }
		    }
            for (i = argi; i < argc; i++)  {
                if (strcmp(argv[i], "-phiLocX") == 0)  {
                    if (Tcl_GetDouble(interp, argv[i+1], &phiLocX) != TCL_OK)  {
                        opserr << "WARNING invalid phiLocX\n";
                        opserr << "expSetup ThreeActuators " << tag << endln;
                        return TCL_ERROR;	
                    }                
                }
            }

            // parsing was successful, allocate the setup
            theSetup = new ESThreeActuators2d(tag, La1, La2, La3, L1, L2, theControl, nlGeom, posAct0, phiLocX);
        }
        
        else if (ndm == 3)  {
            // not implemented yet
            opserr << "WARNING expSetup ThreeActuators command not implemented yet for ndm = 3\n";
            return TCL_ERROR;
        }
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"ThreeActuatorsJntOff") == 0)  {
        if (ndm == 2 || ndm == 3)  {
            if (argc < 12)  {
                opserr << "WARNING invalid number of arguments\n";
                printCommand(argc,argv);
                opserr << "Want: expSetup ThreeActuatorsJntOff tag <-control ctrlTag> La1 La2 La3 L1 L2 L3 L4 L5 L6 "
                    << "<-nlGeom> <-posAct1 pos> <-phiLocX phi> "
                    << "<-ctrlDispFact f> <-ctrlVelFact f> <-ctrlAccelFact f> "
                    << "<-ctrlForceFact f> <-ctrlTimeFact f> "
                    << "<-daqDispFact f> <-daqVelFact f> <-daqAccelFact f> "
                    << "<-daqForceFact f> <-daqTimeFact f>\n";
                return TCL_ERROR;
            }    
            
            int ctrlTag, i;
            double La1, La2, La3, L1, L2, L3, L4, L5, L6;
            int nlGeom = 0;
            char posAct0[6] = {'l','e','f','t','\0'};
            double phiLocX = 0.0;
            
            argi = 2;
            if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
                opserr << "WARNING invalid expSetup ThreeActuatorsJntOff tag\n";
                return TCL_ERROR;		
            }
            argi++;
            if (strcmp(argv[argi],"-control") == 0)  {
                argi++;
                if (Tcl_GetInt(interp, argv[argi], &ctrlTag) != TCL_OK)  {
                    opserr << "WARNING invalid ctrlTag\n";
                    opserr << "expSetup ThreeActuatorsJntOff " << tag << endln;
                    return TCL_ERROR;	
                }
                theControl = getExperimentalControl(ctrlTag);
                if (theControl == 0)  {
                    opserr << "WARNING experimental control not found\n";
                    opserr << "expControl: " << ctrlTag << endln;
                    opserr << "expSetup ThreeActuatorsJntOff " << tag << endln;
                    return TCL_ERROR;
                }
                argi++;
            }
            if (Tcl_GetDouble(interp, argv[argi], &La1) != TCL_OK)  {
                opserr << "WARNING invalid La1\n";
                opserr << "expSetup ThreeActuatorsJntOff " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &La2) != TCL_OK)  {
                opserr << "WARNING invalid La2\n";
                opserr << "expSetup ThreeActuatorsJntOff " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &La3) != TCL_OK)  {
                opserr << "WARNING invalid La3\n";
                opserr << "expSetup ThreeActuatorsJntOff " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L1) != TCL_OK)  {
                opserr << "WARNING invalid L1\n";
                opserr << "expSetup ThreeActuatorsJntOff " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L2) != TCL_OK)  {
                opserr << "WARNING invalid L2\n";
                opserr << "expSetup ThreeActuatorsJntOff " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L3) != TCL_OK)  {
                opserr << "WARNING invalid L3\n";
                opserr << "expSetup ThreeActuatorsJntOff " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L4) != TCL_OK)  {
                opserr << "WARNING invalid L4\n";
                opserr << "expSetup ThreeActuatorsJntOff " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L5) != TCL_OK)  {
                opserr << "WARNING invalid L5\n";
                opserr << "expSetup ThreeActuatorsJntOff " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L6) != TCL_OK)  {
                opserr << "WARNING invalid L6\n";
                opserr << "expSetup ThreeActuatorsJntOff " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
		    for (i = argi; i < argc; i++)  {
			    if (strcmp(argv[i], "-nlGeom") == 0)  {
                    nlGeom = 1;
			    }
		    }
		    for (i = argi; i < argc; i++)  {
			    if (strcmp(argv[i], "-posAct1") == 0)  {
                    if (strcmp(argv[i+1], "left") == 0 || strcmp(argv[i+1], "l") == 0)
                        strcpy(posAct0, "left");
                    else if (strcmp(argv[i+1], "right") == 0 || strcmp(argv[i+1], "r") == 0)
                        strcpy(posAct0, "right");
			    }
		    }
            for (i = argi; i < argc; i++)  {
                if (strcmp(argv[i], "-phiLocX") == 0)  {
                    if (Tcl_GetDouble(interp, argv[i+1], &phiLocX) != TCL_OK)  {
                        opserr << "WARNING invalid phiLocX\n";
                        opserr << "expSetup ThreeActuatorsJntOff " << tag << endln;
                        return TCL_ERROR;	
                    }                
                }
            }
            
            // parsing was successful, allocate the setup
            theSetup = new ESThreeActuatorsJntOff2d(tag, La1, La2, La3,
                L1, L2, L3, L4, L5, L6, theControl, nlGeom, posAct0, phiLocX);
        }
        
        else if (ndm == 3)  {
            // not implemented yet
            opserr << "WARNING expSetup ThreeActuatorsJntOff command not implemented yet for ndm = 3\n";
            return TCL_ERROR;
        }
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"InvertedVBrace") == 0)  {
        if (ndm == 2 || ndm == 3)  {
            if (argc < 8)  {
                opserr << "WARNING invalid number of arguments\n";
                printCommand(argc,argv);
                opserr << "Want: expSetup InvertedVBrace tag <-control ctrlTag> La1 La2 La3 L1 L2 "
                    << "<-nlGeom> <-posAct1 pos> <-phiLocX phi> "
                    << "<-ctrlDispFact f> <-ctrlVelFact f> <-ctrlAccelFact f> "
                    << "<-ctrlForceFact f> <-ctrlTimeFact f> "
                    << "<-daqDispFact f> <-daqVelFact f> <-daqAccelFact f> "
                    << "<-daqForceFact f> <-daqTimeFact f>\n";
                return TCL_ERROR;
            }    
            
            int ctrlTag, i;
            double La1, La2, La3, L1, L2;
            int nlGeom = 0;
            char posAct0[6] = {'l','e','f','t','\0'};
            double phiLocX = 0.0;
            
            argi = 2;
            if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
                opserr << "WARNING invalid expSetup InvertedVBrace tag\n";
                return TCL_ERROR;		
            }
            argi++;
            if (strcmp(argv[argi],"-control") == 0)  {
                argi++;
                if (Tcl_GetInt(interp, argv[argi], &ctrlTag) != TCL_OK)  {
                    opserr << "WARNING invalid ctrlTag\n";
                    opserr << "expSetup InvertedVBrace " << tag << endln;
                    return TCL_ERROR;	
                }
                theControl = getExperimentalControl(ctrlTag);
                if (theControl == 0)  {
                    opserr << "WARNING experimental control not found\n";
                    opserr << "expControl: " << ctrlTag << endln;
                    opserr << "expSetup InvertedVBrace " << tag << endln;
                    return TCL_ERROR;
                }
                argi++;
            }
            if (Tcl_GetDouble(interp, argv[argi], &La1) != TCL_OK)  {
                opserr << "WARNING invalid La1\n";
                opserr << "expSetup InvertedVBrace " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &La2) != TCL_OK)  {
                opserr << "WARNING invalid La2\n";
                opserr << "expSetup InvertedVBrace " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &La3) != TCL_OK)  {
                opserr << "WARNING invalid La3\n";
                opserr << "expSetup InvertedVBrace " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L1) != TCL_OK)  {
                opserr << "WARNING invalid L1\n";
                opserr << "expSetup InvertedVBrace " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L2) != TCL_OK)  {
                opserr << "WARNING invalid L2\n";
                opserr << "expSetup InvertedVBrace " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
		    for (i = argi; i < argc; i++)  {
			    if (strcmp(argv[i], "-nlGeom") == 0)  {
                    nlGeom = 1;
			    }
		    }
		    for (i = argi; i < argc; i++)  {
			    if (strcmp(argv[i], "-posAct1") == 0)  {
                    if (strcmp(argv[i+1], "left") == 0 || strcmp(argv[i+1], "l") == 0)
                        strcpy(posAct0, "left");
                    else if (strcmp(argv[i+1], "right") == 0 || strcmp(argv[i+1], "r") == 0)
                        strcpy(posAct0, "right");
			    }
		    }
            for (i = argi; i < argc; i++)  {
                if (strcmp(argv[i], "-phiLocX") == 0)  {
                    if (Tcl_GetDouble(interp, argv[i+1], &phiLocX) != TCL_OK)  {
                        opserr << "WARNING invalid phiLocX\n";
                        opserr << "expSetup InvertedVBrace " << tag << endln;
                        return TCL_ERROR;	
                    }                
                }
            }
            
            // parsing was successful, allocate the setup
            theSetup = new ESInvertedVBrace2d(tag, La1, La2, La3, L1, L2, theControl, nlGeom, posAct0, phiLocX);
        }
        
        else if (ndm == 3)  {
            // not implemented yet
            opserr << "WARNING expSetup InvertedVBrace command not implemented yet for ndm = 3\n";
            return TCL_ERROR;
        }
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"InvertedVBraceJntOff") == 0)  {
        if (ndm == 2 || ndm == 3)  {
            if (argc < 12)  {
                opserr << "WARNING invalid number of arguments\n";
                printCommand(argc,argv);
                opserr << "Want: expSetup InvertedVBraceJntOff tag <-control ctrlTag> La1 La2 La3 L1 L2 L3 L4 L5 L6 "
                    << "<-nlGeom> <-posAct1 pos> <-phiLocX phi> "
                    << "<-ctrlDispFact f> <-ctrlVelFact f> <-ctrlAccelFact f> "
                    << "<-ctrlForceFact f> <-ctrlTimeFact f> "
                    << "<-daqDispFact f> <-daqVelFact f> <-daqAccelFact f> "
                    << "<-daqForceFact f> <-daqTimeFact f>\n";
                return TCL_ERROR;
            }    
            
            int ctrlTag, i;
            double La1, La2, La3, L1, L2, L3, L4, L5, L6;
            int nlGeom = 0;
            char posAct0[6] = {'l','e','f','t','\0'};
            double phiLocX = 0.0;
            
            argi = 2;
            if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
                opserr << "WARNING invalid expSetup InvertedVBraceJntOff tag\n";
                return TCL_ERROR;		
            }
            argi++;
            if (strcmp(argv[argi],"-control") == 0)  {
                argi++;
                if (Tcl_GetInt(interp, argv[argi], &ctrlTag) != TCL_OK)  {
                    opserr << "WARNING invalid ctrlTag\n";
                    opserr << "expSetup InvertedVBraceJntOff " << tag << endln;
                    return TCL_ERROR;	
                }
                theControl = getExperimentalControl(ctrlTag);
                if (theControl == 0)  {
                    opserr << "WARNING experimental control not found\n";
                    opserr << "expControl: " << ctrlTag << endln;
                    opserr << "expSetup InvertedVBraceJntOff " << tag << endln;
                    return TCL_ERROR;
                }
                argi++;
            }
            if (Tcl_GetDouble(interp, argv[argi], &La1) != TCL_OK)  {
                opserr << "WARNING invalid La1\n";
                opserr << "expSetup InvertedVBraceJntOff " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &La2) != TCL_OK)  {
                opserr << "WARNING invalid La2\n";
                opserr << "expSetup InvertedVBraceJntOff " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &La3) != TCL_OK)  {
                opserr << "WARNING invalid La3\n";
                opserr << "expSetup InvertedVBraceJntOff " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L1) != TCL_OK)  {
                opserr << "WARNING invalid L1\n";
                opserr << "expSetup InvertedVBraceJntOff " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L2) != TCL_OK)  {
                opserr << "WARNING invalid L2\n";
                opserr << "expSetup InvertedVBraceJntOff " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L3) != TCL_OK)  {
                opserr << "WARNING invalid L3\n";
                opserr << "expSetup InvertedVBraceJntOff " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L4) != TCL_OK)  {
                opserr << "WARNING invalid L4\n";
                opserr << "expSetup InvertedVBraceJntOff " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L5) != TCL_OK)  {
                opserr << "WARNING invalid L5\n";
                opserr << "expSetup InvertedVBraceJntOff " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L6) != TCL_OK)  {
                opserr << "WARNING invalid L6\n";
                opserr << "expSetup InvertedVBraceJntOff " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
		    for (i = argi; i < argc; i++)  {
			    if (strcmp(argv[i], "-nlGeom") == 0)  {
                    nlGeom = 1;
			    }
		    }
		    for (i = argi; i < argc; i++)  {
			    if (strcmp(argv[i], "-posAct1") == 0)  {
                    if (strcmp(argv[i+1], "left") == 0 || strcmp(argv[i+1], "l") == 0)
                        strcpy(posAct0, "left");
                    else if (strcmp(argv[i+1], "right") == 0 || strcmp(argv[i+1], "r") == 0)
                        strcpy(posAct0, "right");
			    }
		    }
            for (i = argi; i < argc; i++)  {
                if (strcmp(argv[i], "-phiLocX") == 0)  {
                    if (Tcl_GetDouble(interp, argv[i+1], &phiLocX) != TCL_OK)  {
                        opserr << "WARNING invalid phiLocX\n";
                        opserr << "expSetup InvertedVBraceJntOff " << tag << endln;
                        return TCL_ERROR;	
                    }                
                }
            }
            
            // parsing was successful, allocate the setup
            theSetup = new ESInvertedVBraceJntOff2d(tag, La1, La2, La3,
                L1, L2, L3, L4, L5, L6, theControl, nlGeom, posAct0, phiLocX);
        }
        
        else if (ndm == 3)  {
            // not implemented yet
            opserr << "WARNING expSetup InvertedVBraceJntOff command not implemented yet for ndm = 3\n";
            return TCL_ERROR;
        }
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"Aggregator") == 0)  {
        if (argc < 5)  {
            opserr << "WARNING invalid number of arguments\n";
            printCommand(argc,argv);
            opserr << "Want: expSetup Aggregator tag <-control ctrlTag> -setup setupTagi -sizeTrialOut si -sizeTrialOut sj ... "
                << "<-ctrlDispFact f> <-ctrlVelFact f> <-ctrlAccelFact f> "
                << "<-ctrlForceFact f> <-ctrlTimeFact f> "
                << "<-daqDispFact f> <-daqVelFact f> <-daqAccelFact f> "
                << "<-daqForceFact f> <-daqTimeFact f>\n";
            return TCL_ERROR;
        }    
        
        int ctrlTag, numSetups = 0, setupTag, size, i, j;
        
        argi = 2;
        if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
            opserr << "WARNING invalid expSetup Aggregator tag\n";
            return TCL_ERROR;		
        }
        argi++;
        if (strcmp(argv[argi],"-control") == 0)  {
            argi++;
            if (Tcl_GetInt(interp, argv[argi], &ctrlTag) != TCL_OK)  {
                opserr << "WARNING invalid ctrlTag\n";
                opserr << "expSetup Aggregator " << tag << endln;
                return TCL_ERROR;	
            }
            theControl = getExperimentalControl(ctrlTag);
            if (theControl == 0)  {
                opserr << "WARNING experimental control not found\n";
                opserr << "expControl: " << ctrlTag << endln;
                opserr << "expSetup Aggregator " << tag << endln;
                return TCL_ERROR;
            }
            argi++;
        }
        // read the number of setups
        if (strcmp(argv[argi],"-setup") != 0)  {
            opserr << "WARNING expecting -setup flag\n";
            opserr << "expSetup Aggregator " << tag << endln;
            return TCL_ERROR;	
        }
        argi++;
        while (strcmp(argv[argi+numSetups],"-sizeTrialOut") != 0 && 
            argi+numSetups < argc)  {
            numSetups++;
        }
        if (numSetups == 0)  {
		    opserr << "WARNING no setups specified\n";
		    opserr << "expSetup Aggregator " << tag << endln;
		    return TCL_ERROR;
	    }
        // create the arrays to hold the setups and sizes
        ExperimentalSetup **expSetups = new ExperimentalSetup* [numSetups];
        ID *sizeTrialAll = new ID [numSetups];
        ID *sizeOutAll = new ID [numSetups];
        if (expSetups == 0 || sizeTrialAll ==0 || sizeOutAll == 0)  {
		    opserr << "WARNING out of memory\n";
		    opserr << "expSetup Aggregator " << tag << endln;
		    return TCL_ERROR;
	    }
        // read the setup tags
        for (i=0; i<numSetups; i++)  {
            if (Tcl_GetInt(interp, argv[argi], &setupTag) != TCL_OK)  {
                opserr << "WARNING invalid setupTag\n";
                opserr << "expSetup Aggregator " << tag << endln;
                return TCL_ERROR;	
            }
            expSetups[i] = getExperimentalSetup(setupTag);
            if (expSetups[i] == 0)  {
                opserr << "WARNING experimental setup not found\n";
                opserr << "expSetup " << setupTag << endln;
                opserr << "expSetup Aggregator " << tag << endln;
                return TCL_ERROR;
            }
            argi++;
        }
        for (j=0; j<numSetups; j++)  {
            // read the trial and out sizes for each setup j
            if (strcmp(argv[argi], "-sizeTrialOut") != 0)  {
                opserr << "WARNING expect -sizeTrialOut flag\n";
                opserr << "expSetup Aggregator " << tag << endln;
                return TCL_ERROR;
            }
            argi++;
            // fill in the dofs ID array
            ID sizeT(5);
            ID sizeO(5);
            for (i=0; i<5; i++)  {
                if (Tcl_GetInt(interp, argv[argi], &size) != TCL_OK)  {
                    opserr << "WARNING invalid trial size\n";
                    opserr << "expSetup Aggregator " << tag << endln;
                    return TCL_ERROR;
                }
                sizeT(i) = size;
                argi++; 
            }
            for (i=0; i<5; i++)  {
                if (Tcl_GetInt(interp, argv[argi], &size) != TCL_OK)  {
                    opserr << "WARNING invalid out size\n";
                    opserr << "expSetup Aggregator " << tag << endln;
                    return TCL_ERROR;
                }
                sizeO(i) = size;
                argi++; 
            }
            sizeTrialAll[j] = sizeT;
            sizeOutAll[j] = sizeO;
        }
        
        // parsing was successful, allocate the setup
        theSetup = new ESAggregator(tag, numSetups, expSetups, sizeTrialAll, sizeOutAll, theControl);
    }
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"FourActuators") == 0)  {
        if (ndm == 3)  {
            if (argc < 18)  {
                opserr << "WARNING invalid number of arguments\n";
                printCommand(argc,argv);
                opserr << "Want: expSetup FourActuators tag <-control ctrlTag> L1 L2 L3 L4 a1 a2 a3 a4 h h1 h2 arlN arlS LrodN LrodS "
                    << " <-nlGeom> <-phiLocX phi> "
                    << "<-ctrlDispFact f> <-ctrlVelFact f> <-ctrlAccelFact f> "
                    << "<-ctrlForceFact f> <-ctrlTimeFact f> "
                    << "<-daqDispFact f> <-daqVelFact f> <-daqAccelFact f> "
                    << "<-daqForceFact f> <-daqTimeFact f>\n";
                return TCL_ERROR;
            }    
            
            int ctrlTag, i;
            double L1, L2, L3, L4, a1, a2, a3, a4, h, h1, h2, arlN, arlS, LrodN, LrodS;
            int nlGeom = 0;
            double phiLocX = 45.0;
            
            argi = 2;
            if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
                opserr << "WARNING invalid expSetup FourActuators tag\n";
                return TCL_ERROR;		
            }
            argi++;
            if (strcmp(argv[argi],"-control") == 0)  {
                argi++;
                if (Tcl_GetInt(interp, argv[argi], &ctrlTag) != TCL_OK)  {
                    opserr << "WARNING invalid ctrlTag\n";
                    opserr << "expSetup FourActuators " << tag << endln;
                    return TCL_ERROR;	
                }
                theControl = getExperimentalControl(ctrlTag);
                if (theControl == 0)  {
                    opserr << "WARNING experimental control not found\n";
                    opserr << "expControl: " << ctrlTag << endln;
                    opserr << "expSetup FourActuators " << tag << endln;
                    return TCL_ERROR;
                }
                argi++;
            }
            if (Tcl_GetDouble(interp, argv[argi], &L1) != TCL_OK)  {
                opserr << "WARNING invalid L1\n";
                opserr << "expSetup FourActuators " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L2) != TCL_OK)  {
                opserr << "WARNING invalid L2\n";
                opserr << "expSetup FourActuators " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L3) != TCL_OK)  {
                opserr << "WARNING invalid L3\n";
                opserr << "expSetup FourActuators " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L4) != TCL_OK)  {
                opserr << "WARNING invalid L4\n";
                opserr << "expSetup FourActuators " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &a1) != TCL_OK)  {
                opserr << "WARNING invalid a1\n";
                opserr << "expSetup FourActuators " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &a2) != TCL_OK)  {
                opserr << "WARNING invalid a2\n";
                opserr << "expSetup FourActuators " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &a3) != TCL_OK)  {
                opserr << "WARNING invalid a3\n";
                opserr << "expSetup FourActuators " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &a4) != TCL_OK)  {
                opserr << "WARNING invalid a4\n";
                opserr << "expSetup FourActuators " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &h) != TCL_OK)  {
                opserr << "WARNING invalid h\n";
                opserr << "expSetup FourActuators " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &h1) != TCL_OK)  {
                opserr << "WARNING invalid h1\n";
                opserr << "expSetup FourActuators " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &h2) != TCL_OK)  {
                opserr << "WARNING invalid h2\n";
                opserr << "expSetup FourActuators " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &arlN) != TCL_OK)  {
                opserr << "WARNING invalid arlN\n";
                opserr << "expSetup FourActuators " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &arlS) != TCL_OK)  {
                opserr << "WARNING invalid arlS\n";
                opserr << "expSetup FourActuators " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &LrodN) != TCL_OK)  {
                opserr << "WARNING invalid LrodN\n";
                opserr << "expSetup FourActuators " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &LrodS) != TCL_OK)  {
                opserr << "WARNING invalid LrodS\n";
                opserr << "expSetup FourActuators " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
			for (i = argi; i < argc; i++)  {
			    if (strcmp(argv[i], "-nlGeom") == 0)  {
                    nlGeom = 1;
			    }
		    }
            for (i = argi; i < argc; i++)  {
                if (strcmp(argv[i], "-phiLocX") == 0)  {
                    if (Tcl_GetDouble(interp, argv[i+1], &phiLocX) != TCL_OK)  {
                        opserr << "WARNING invalid phiLocX\n";
                        opserr << "expSetup FourActuators " << tag << endln;
                        return TCL_ERROR;	
                    }                
                }
            }
			
            // parsing was successful, allocate the setup
            theSetup = new ESFourActuators3d(tag, L1, L2, L3, L4, a1, a2, a3, a4, h, h1, h2, arlN, arlS, LrodN, LrodS, theControl, nlGeom, phiLocX);
        }
        
        else {
            // it has to be in 3D
            opserr << "WARNING expSetup FourActuators command works only in 3D domain\n";
            return TCL_ERROR;
        }
    }
     
    // ----------------------------------------------------------------------------	
    else  {
        // experimental setup type not recognized
        opserr << "WARNING unknown experimental setup type: "
            <<  argv[1] << ": check the manual\n";
        return TCL_ERROR;
    }

    if (theSetup == 0)  {
        opserr << "WARNING could not create experimental setup " << argv[1] << endln;
        return TCL_ERROR;
    }
    
    // finally check for factors
    int i, size;
    size = theSetup->getCtrlSize(OF_Resp_Disp);
    for (i = argi; i < argc; i++)  {
        if (i+1 < argc && strcmp(argv[i], "-ctrlDispFact") == 0)  {
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid ctrlDispFact\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setCtrlDispFactor(factor);
        }
    }
    size = theSetup->getCtrlSize(OF_Resp_Vel);
    for (i = argi; i < argc; i++)  {
        if (i+1 < argc && strcmp(argv[i], "-ctrlVelFact") == 0)  {
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid ctrlVelFact\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setCtrlVelFactor(factor);
        }
    }
    size = theSetup->getCtrlSize(OF_Resp_Accel);
    for (i = argi; i < argc; i++)  {
        if (i+1 < argc && strcmp(argv[i], "-ctrlAccelFact") == 0)  {
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid ctrlAccelFact\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setCtrlAccelFactor(factor);
        }
    }
    size = theSetup->getCtrlSize(OF_Resp_Force);
    for (i = argi; i < argc; i++)  {
        if (i+1 < argc && strcmp(argv[i], "-ctrlForceFact") == 0)  {
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid ctrlForceFact\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setCtrlForceFactor(factor);
        }
    }
    size = theSetup->getCtrlSize(OF_Resp_Time);
    for (i = argi; i < argc; i++)  {
        if (i+1 < argc && strcmp(argv[i], "-ctrlTimeFact") == 0)  {
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid ctrlTimeFact\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setCtrlTimeFactor(factor);
        }
    }
    size = theSetup->getDaqSize(OF_Resp_Disp);
    for (i = argi; i < argc; i++)  {
        if (i+1 < argc && strcmp(argv[i], "-daqDispFact") == 0)  {
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid daqDispFact\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setDaqDispFactor(factor);
        }
    }
    size = theSetup->getDaqSize(OF_Resp_Vel);
    for (i = argi; i < argc; i++)  {
        if (i+1 < argc && strcmp(argv[i], "-daqVelFact") == 0)  {
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid daqVelFact\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setDaqVelFactor(factor);
        }
    }
    size = theSetup->getDaqSize(OF_Resp_Accel);
    for (i = argi; i < argc; i++)  {
        if (i+1 < argc && strcmp(argv[i], "-daqAccelFact") == 0)  {
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid daqAccelFact\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setDaqAccelFactor(factor);
        }
    }
    size = theSetup->getDaqSize(OF_Resp_Force);
    for (i = argi; i < argc; i++)  {
        if (i+1 < argc && strcmp(argv[i], "-daqForceFact") == 0)  {
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid daqForceFact\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setDaqForceFactor(factor);
        }
    }
    size = theSetup->getDaqSize(OF_Resp_Time);
    for (i = argi; i < argc; i++)  {
        if (i+1 < argc && strcmp(argv[i], "-daqTimeFact") == 0)  {
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid daqTimeFact\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setDaqTimeFactor(factor);
        }
    }
    
    // now add the setup to the modelBuilder
    if (addExperimentalSetup(*theSetup) < 0)  {
        delete theSetup; // invoke the destructor, otherwise mem leak
        return TCL_ERROR;
    }
    
    return TCL_OK;
}
