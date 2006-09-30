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
// invokes the expSetup command in the interpreter. 

#include <TclModelBuilder.h>
#include <ArrayOfTaggedObjects.h>

#include <ESNoTransformation.h>
#include <ESOneActuator.h>
#include <ESTwoActuators2d.h>
#include <ESThreeActuators2d.h>
#include <ESChevronBrace2d.h>
#include <ESChevronBraceJntOff2d.h>

#include <ID.h>
#include <Vector.h>
#include <string.h>

extern ExperimentalControl *getExperimentalControl(int tag);
static ArrayOfTaggedObjects *theExperimentalSetups(0);


int addExperimentalSetup(ExperimentalSetup &theTestSetup)
{
    bool result = theExperimentalSetups->addComponent(&theTestSetup);
    if (result == true)
        return 0;
    else {
        opserr << "TclModelBuilder::addExperimentalSetup() - "
            << "failed to add experimental setup: " << theTestSetup;
        return -1;
    }
}


extern ExperimentalSetup *getExperimentalSetup(int tag)
{
    TaggedObject *mc = theExperimentalSetups->getComponentPtr(tag);
    if (mc == 0) 
        return 0;

    // otherweise we do a cast and return
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
    TCL_Char **argv, TclModelBuilder *theTclBuilder, Domain *theDomain)
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
                << "<-daqDispFact f> <-daqVelFact f> <-ctrlAccelFact f> "
                << "<-daqForceFact f> <-daqTimeFact f>\n";
            return TCL_ERROR;
        }    
        
        int ctrlTag, numDir, dirID, i;
        
        argi = 2;
        if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
            opserr << "WARNING invalid NoTransformation tag\n";
            return TCL_ERROR;		
        }
        argi++;
        if (strcmp(argv[argi],"-control") == 0)  {
            argi++;
            if (Tcl_GetInt(interp, argv[argi], &ctrlTag) != TCL_OK)  {
                opserr << "WARNING invalid ctrlTag\n";
                opserr << "NoTransformation setup: " << tag << endln;
                return TCL_ERROR;	
            }
            theControl = getExperimentalControl(ctrlTag);
            if (theControl == 0)  {
                opserr << "WARNING experimental controller not found\n";
                opserr << "expControl: " << ctrlTag << endln;
                opserr << "NoTransformation setup: " << tag << endln;
                return TCL_ERROR;
            }
            argi++;
        }
        // now read the direction IDs
        if (strcmp(argv[argi],"-dir") != 0)  {
            opserr << "WARNING expecting -dir dirs\n";
            opserr << "NoTransformation setup: " << tag << endln;
            return TCL_ERROR;	
        }
        argi++;
        i = argi;
        numDir = 0;
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
            numDir++;
            i++;
        }
        // create an ID array to hold the direction IDs
        ID theDirIDs(numDir);
        // read the dir identifiers
        for (i=0; i<numDir; i++)  {
            if (Tcl_GetInt(interp, argv[argi], &dirID) != TCL_OK)  {
                opserr << "WARNING invalid direction ID\n";
                opserr << "NoTransformation setup: " << tag << endln;
                return TCL_ERROR;	
            } else  {
                theDirIDs[i] = dirID - 1;
                argi++;
            }
        }
        
        // parsing was successful, allocate the setup
        if (theControl != 0)
            theSetup = new ESNoTransformation(tag, theDirIDs, theControl);
        else
            theSetup = new ESNoTransformation(tag, theDirIDs);
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"OneActuator") == 0)  {
        if (argc < 4)  {
            opserr << "WARNING invalid number of arguments\n";
            printCommand(argc,argv);
            opserr << "Want: expSetup OneActuator tag <-control ctrlTag> dir "
                << "<-ctrlDispFact f> <-ctrlVelFact f> <-ctrlAccelFact f> "
                << "<-ctrlForceFact f> <-ctrlTimeFact f> "
                << "<-daqDispFact f> <-daqVelFact f> <-ctrlAccelFact f> "
                << "<-daqForceFact f> <-daqTimeFact f>\n";
            return TCL_ERROR;
        }    
        
        int ctrlTag, dir;
        
        argi = 2;
        if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
            opserr << "WARNING invalid OneActuator tag\n";
            return TCL_ERROR;		
        }
        argi++;
        if (strcmp(argv[argi],"-control") == 0)  {
            argi++;
            if (Tcl_GetInt(interp, argv[argi], &ctrlTag) != TCL_OK)  {
                opserr << "WARNING invalid ctrlTag\n";
                opserr << "OneActuator setup: " << tag << endln;
                return TCL_ERROR;	
            }
            theControl = getExperimentalControl(ctrlTag);
            if (theControl == 0)  {
                opserr << "WARNING experimental controller not found\n";
                opserr << "expControl: " << ctrlTag << endln;
                opserr << "OneActuator setup: " << tag << endln;
                return TCL_ERROR;
            }
            argi++;
        }
        if (Tcl_GetInt(interp, argv[argi], &dir) != TCL_OK)  {
            opserr << "WARNING invalid dir\n";
            opserr << "OneActuator setup: " << tag << endln;
            return TCL_ERROR;	
        }
        
        // parsing was successful, allocate the setup
        if (theControl != 0)
            theSetup = new ESOneActuator(tag, dir-1, theControl);
        else
            theSetup = new ESOneActuator(tag, dir-1);
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"TwoActuators") == 0)  {        
        if (ndm == 2)  {
            if (argc < 7)  {
                opserr << "WARNING invalid number of arguments\n";
                printCommand(argc,argv);
                opserr << "Want: expSetup TwoActuators tag <-control ctrlTag> nlGeomFlag La1 La2 L "
                    << "<-ctrlDispFact f> <-ctrlVelFact f> <-ctrlAccelFact f> "
                    << "<-ctrlForceFact f> <-ctrlTimeFact f> "
                    << "<-daqDispFact f> <-daqVelFact f> <-ctrlAccelFact f> "
                    << "<-daqForceFact f> <-daqTimeFact f>\n";
                return TCL_ERROR;
            }    
            
            int ctrlTag, nlGeomFlag;
            double La1, La2, L;
            
            argi = 2;
            if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
                opserr << "WARNING invalid TwoActuators tag\n";
                return TCL_ERROR;		
            }
            argi++;
            if (strcmp(argv[argi],"-control") == 0)  {
                argi++;
                if (Tcl_GetInt(interp, argv[argi], &ctrlTag) != TCL_OK)  {
                    opserr << "WARNING invalid ctrlTag\n";
                    opserr << "TwoActuators setup: " << tag << endln;
                    return TCL_ERROR;	
                }
                theControl = getExperimentalControl(ctrlTag);
                if (theControl == 0)  {
                    opserr << "WARNING experimental controller not found\n";
                    opserr << "expControl: " << ctrlTag << endln;
                    opserr << "TwoActuators setup: " << tag << endln;
                    return TCL_ERROR;
                }
                argi++;
            }
            if (Tcl_GetInt(interp, argv[argi], &nlGeomFlag) != TCL_OK)  {
                opserr << "WARNING invalid nlGeom\n";
                opserr << "TwoActuators setup: " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &La1) != TCL_OK)  {
                opserr << "WARNING invalid La1\n";
                opserr << "TwoActuators setup: " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &La2) != TCL_OK)  {
                opserr << "WARNING invalid La2\n";
                opserr << "TwoActuators setup: " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L) != TCL_OK)  {
                opserr << "WARNING invalid L\n";
                opserr << "TwoActuators setup: " << tag << endln;
                return TCL_ERROR;	
            }
            
            // parsing was successful, allocate the setup
            if (theControl != 0)
                theSetup = new ESTwoActuators2d(tag, nlGeomFlag, La1, La2, L, theControl);
            else
                theSetup = new ESTwoActuators2d(tag, nlGeomFlag, La1, La2, L);
        }
        
        else if (ndm == 3)  {
            // not implemented yet
            opserr << "WARNING expSetup TwoActuators command not implemented yet for ndm = 3\n";
            return TCL_ERROR;
        }
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"ThreeActuators") == 0)  {
        if (ndm == 2)  {
            if (argc < 9)  {
                opserr << "WARNING invalid number of arguments\n";
                printCommand(argc,argv);
                opserr << "Want: expSetup ThreeActuators tag <-control ctrlTag> nlGeom La1 La2 La3 L1 L2 "
                    << "<-ctrlDispFact f> <-ctrlVelFact f> <-ctrlAccelFact f> "
                    << "<-ctrlForceFact f> <-ctrlTimeFact f> "
                    << "<-daqDispFact f> <-daqVelFact f> <-ctrlAccelFact f> "
                    << "<-daqForceFact f> <-daqTimeFact f>\n";
                return TCL_ERROR;
            }    
            
            int ctrlTag, nlGeomFlag;
            double La1, La2, La3, L1, L2;
            
            argi = 2;
            if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
                opserr << "WARNING invalid ThreeActuators tag\n";
                return TCL_ERROR;		
            }
            argi++;
            if (strcmp(argv[argi],"-control") == 0)  {
                argi++;
                if (Tcl_GetInt(interp, argv[argi], &ctrlTag) != TCL_OK)  {
                    opserr << "WARNING invalid ctrlTag\n";
                    opserr << "ThreeActuators setup: " << tag << endln;
                    return TCL_ERROR;	
                }
                theControl = getExperimentalControl(ctrlTag);
                if (theControl == 0)  {
                    opserr << "WARNING experimental controller not found\n";
                    opserr << "expControl: " << ctrlTag << endln;
                    opserr << "ThreeActuators setup: " << tag << endln;
                    return TCL_ERROR;
                }
                argi++;
            }
            if (Tcl_GetInt(interp, argv[argi], &nlGeomFlag) != TCL_OK)  {
                opserr << "WARNING invalid nlGeom\n";
                opserr << "ThreeActuators setup: " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &La1) != TCL_OK)  {
                opserr << "WARNING invalid La1\n";
                opserr << "ThreeActuators setup: " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &La2) != TCL_OK)  {
                opserr << "WARNING invalid La2\n";
                opserr << "ThreeActuators setup: " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &La3) != TCL_OK)  {
                opserr << "WARNING invalid La3\n";
                opserr << "ThreeActuators setup: " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L1) != TCL_OK)  {
                opserr << "WARNING invalid L1\n";
                opserr << "ThreeActuators setup: " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L2) != TCL_OK)  {
                opserr << "WARNING invalid L2\n";
                opserr << "ThreeActuators setup: " << tag << endln;
                return TCL_ERROR;	
            }
            
            // parsing was successful, allocate the setup
            if (theControl != 0)
                theSetup = new ESThreeActuators2d(tag, nlGeomFlag, La1, La2, La3, L1, L2, theControl);
            else
                theSetup = new ESThreeActuators2d(tag, nlGeomFlag, La1, La2, La3, L1, L2);
        }
        
        else if (ndm == 3)  {
            // not implemented yet
            opserr << "WARNING expSetup ThreeActuators command not implemented yet for ndm = 3\n";
            return TCL_ERROR;
        }
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"ChevronBrace") == 0)  {
        if (ndm == 2)  {
            if (argc < 9)  {
                opserr << "WARNING invalid number of arguments\n";
                printCommand(argc,argv);
                opserr << "Want: expSetup ChevronBrace tag <-control ctrlTag> nlGeom La1 La2 La3 L1 L2 "
                    << "<-ctrlDispFact f> <-ctrlVelFact f> <-ctrlAccelFact f> "
                    << "<-ctrlForceFact f> <-ctrlTimeFact f> "
                    << "<-daqDispFact f> <-daqVelFact f> <-ctrlAccelFact f> "
                    << "<-daqForceFact f> <-daqTimeFact f>\n";
                return TCL_ERROR;
            }    
            
            int ctrlTag, nlGeomFlag;
            double La1, La2, La3, L1, L2;
            
            argi = 2;
            if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
                opserr << "WARNING invalid ChevronBrace tag\n";
                return TCL_ERROR;		
            }
            argi++;
            if (strcmp(argv[argi],"-control") == 0)  {
                argi++;
                if (Tcl_GetInt(interp, argv[argi], &ctrlTag) != TCL_OK)  {
                    opserr << "WARNING invalid ctrlTag\n";
                    opserr << "ChevronBrace setup: " << tag << endln;
                    return TCL_ERROR;	
                }
                theControl = getExperimentalControl(ctrlTag);
                if (theControl == 0)  {
                    opserr << "WARNING experimental controller not found\n";
                    opserr << "expControl: " << ctrlTag << endln;
                    opserr << "ChevronBrace setup: " << tag << endln;
                    return TCL_ERROR;
                }
                argi++;
            }
            if (Tcl_GetInt(interp, argv[argi], &nlGeomFlag) != TCL_OK)  {
                opserr << "WARNING invalid nlGeom\n";
                opserr << "ChevronBrace setup: " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &La1) != TCL_OK)  {
                opserr << "WARNING invalid La1\n";
                opserr << "ChevronBrace setup: " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &La2) != TCL_OK)  {
                opserr << "WARNING invalid La2\n";
                opserr << "ChevronBrace setup: " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &La3) != TCL_OK)  {
                opserr << "WARNING invalid La3\n";
                opserr << "ChevronBrace setup: " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L1) != TCL_OK)  {
                opserr << "WARNING invalid L1\n";
                opserr << "ChevronBrace setup: " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L2) != TCL_OK)  {
                opserr << "WARNING invalid L2\n";
                opserr << "ChevronBrace setup: " << tag << endln;
                return TCL_ERROR;	
            }
            
            // parsing was successful, allocate the setup
            if (theControl != 0)
                theSetup = new ESChevronBrace2d(tag, nlGeomFlag, La1, La2, La3, L1, L2, theControl);
            else
                theSetup = new ESChevronBrace2d(tag, nlGeomFlag, La1, La2, La3, L1, L2);
        }
        
        else if (ndm == 3)  {
            // not implemented yet
            opserr << "WARNING expSetup ChevronBrace command not implemented yet for ndm = 3\n";
            return TCL_ERROR;
        }
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"ChevronBraceJntOff") == 0)  {
        if (ndm == 2)  {
            if (argc < 13)  {
                opserr << "WARNING invalid number of arguments\n";
                printCommand(argc,argv);
                opserr << "Want: expSetup ChevronBraceJntOff tag <-control ctrlTag> nlGeom La1 La2 La3 L1 L2 L3 L4 L5 L6 "
                    << "<-ctrlDispFact f> <-ctrlVelFact f> <-ctrlAccelFact f> "
                    << "<-ctrlForceFact f> <-ctrlTimeFact f> "
                    << "<-daqDispFact f> <-daqVelFact f> <-ctrlAccelFact f> "
                    << "<-daqForceFact f> <-daqTimeFact f>\n";
                return TCL_ERROR;
            }    
            
            int ctrlTag, nlGeomFlag;
            double La1, La2, La3, L1, L2, L3, L4, L5, L6;
            
            argi = 2;
            if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
                opserr << "WARNING invalid ChevronBraceJntOff tag\n";
                return TCL_ERROR;		
            }
            argi++;
            if (strcmp(argv[argi],"-control") == 0)  {
                argi++;
                if (Tcl_GetInt(interp, argv[argi], &ctrlTag) != TCL_OK)  {
                    opserr << "WARNING invalid ctrlTag\n";
                    opserr << "ChevronBraceJntOff setup: " << tag << endln;
                    return TCL_ERROR;	
                }
                ExperimentalControl *theControl = getExperimentalControl(ctrlTag);
                if (theControl == 0)  {
                    opserr << "WARNING experimental controller not found\n";
                    opserr << "expControl: " << ctrlTag << endln;
                    opserr << "ChevronBraceJntOff setup: " << tag << endln;
                    return TCL_ERROR;
                }
                argi++;
            }
            if (Tcl_GetInt(interp, argv[argi], &nlGeomFlag) != TCL_OK)  {
                opserr << "WARNING invalid nlGeom\n";
                opserr << "ChevronBraceJntOff setup: " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &La1) != TCL_OK)  {
                opserr << "WARNING invalid La1\n";
                opserr << "ChevronBraceJntOff setup: " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &La2) != TCL_OK)  {
                opserr << "WARNING invalid La2\n";
                opserr << "ChevronBraceJntOff setup: " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &La3) != TCL_OK)  {
                opserr << "WARNING invalid La3\n";
                opserr << "ChevronBraceJntOff setup: " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L1) != TCL_OK)  {
                opserr << "WARNING invalid L1\n";
                opserr << "ChevronBraceJntOff setup: " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L2) != TCL_OK)  {
                opserr << "WARNING invalid L2\n";
                opserr << "ChevronBraceJntOff setup: " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L3) != TCL_OK)  {
                opserr << "WARNING invalid L3\n";
                opserr << "ChevronBraceJntOff setup: " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L4) != TCL_OK)  {
                opserr << "WARNING invalid L4\n";
                opserr << "ChevronBraceJntOff setup: " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L5) != TCL_OK)  {
                opserr << "WARNING invalid L5\n";
                opserr << "ChevronBraceJntOff setup: " << tag << endln;
                return TCL_ERROR;	
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L6) != TCL_OK)  {
                opserr << "WARNING invalid L6\n";
                opserr << "ChevronBraceJntOff setup: " << tag << endln;
                return TCL_ERROR;	
            }
            
            // parsing was successful, allocate the setup
            if (theControl != 0)
                theSetup = new ESChevronBraceJntOff2d(tag, nlGeomFlag, La1, La2, La3,
                L1, L2, L3, L4, L5, L6, theControl);
            else
                theSetup = new ESChevronBraceJntOff2d(tag, nlGeomFlag, La1, La2, La3,
                L1, L2, L3, L4, L5, L6);
        }
        
        else if (ndm == 3)  {
            // not implemented yet
            opserr << "WARNING expSetup ChevronBraceJntOff command not implemented yet for ndm = 3\n";
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
                    opserr << "Setup: " << tag << endln;
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
                    opserr << "Setup: " << tag << endln;
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
                    opserr << "Setup: " << tag << endln;
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
                    opserr << "Setup: " << tag << endln;
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
                    opserr << "Setup: " << tag << endln;
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
                    opserr << "Setup: " << tag << endln;
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
                    opserr << "Setup: " << tag << endln;
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
                    opserr << "Setup: " << tag << endln;
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
                    opserr << "Setup: " << tag << endln;
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
                    opserr << "Setup: " << tag << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setDaqTimeFactor(factor);
        }
    }
    
    // now add the setup to the modelBuilder
    if (addExperimentalSetup(*theSetup) < 0)  {
        opserr << "WARNING could not add experimental setup to the domain\n";
        opserr << *theSetup << endln;
        delete theSetup; // invoke the destructor, otherwise mem leak
        return TCL_ERROR;
    }
    
    return TCL_OK;
}
