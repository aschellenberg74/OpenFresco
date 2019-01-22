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

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 09/06
// Revision: A
//
// Description: This file contains the function invoked when the user
// invokes the expSetup command in the interpreter. 

#include <string.h>
#include <tcl.h>
#include <ArrayOfTaggedObjects.h>
#include <ID.h>
#include <Vector.h>
#include <elementAPI.h>

#include <ESNoTransformation.h>
#include <ESOneActuator.h>
#include <ESTwoActuators2d.h>
#include <ESThreeActuators.h>
#include <ESThreeActuators2d.h>
#include <ESThreeActuatorsJntOff.h>
#include <ESThreeActuatorsJntOff2d.h>
#include <ESInvertedVBrace2d.h>
#include <ESInvertedVBraceJntOff2d.h>
#include <ESAggregator.h>
#include <ESFourActuators3d.h>

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
    if (theExperimentalSetups == 0)  {
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


extern ExperimentalSetup *removeExperimentalSetup(int tag)
{
    if (theExperimentalSetups == 0)  {
        opserr << "removeExperimentalSetup() - "
            << "failed to remove experimental setup: " << tag << endln
            << "no experimental setup objects have been defined\n";
        return 0;
    }
    
    TaggedObject *mc = theExperimentalSetups->removeComponent(tag);
    if (mc == 0)
        return 0;
    
    // otherwise we do a cast and return
    ExperimentalSetup *result = (ExperimentalSetup *)mc;
    return result;
}


extern int clearExperimentalSetups(Tcl_Interp *interp)
{
    if (theExperimentalSetups != 0)  {
        theExperimentalSetups->clearAll();
        delete theExperimentalSetups;
        theExperimentalSetups = 0;
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


int TclExpSetupCommand(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, Domain *theDomain)
{
    if (theExperimentalSetups == 0)
        theExperimentalSetups = new ArrayOfTaggedObjects(32);
    
    // make sure there is a minimum number of arguments
    if (argc < 3)  {
        opserr << "WARNING insufficient number of experimental setup arguments\n";
        opserr << "Want: expSetup type tag <specific experimental setup args>\n";
        return TCL_ERROR;
    }
    
    // pointers to setup and control that will be added
    ExperimentalSetup *theSetup = 0;
    ExperimentalControl *theControl = 0;
    
    int ndm = OPS_GetNDM();
    int tag, argi;
    
    // ----------------------------------------------------------------------------	
    if (strcmp(argv[1],"NoTransformation") == 0)  {
        if (argc < 8)  {
            opserr << "WARNING invalid number of arguments\n";
            printCommand(argc, argv);
            opserr << "Want: expSetup NoTransformation tag <-control ctrlTag> "
                << "-dof DOFs -sizeTrialOut t o\n";
            return TCL_ERROR;
        }
        
        int ctrlTag, numDOF = 0, dof, sizeTrial, sizeOut, i;
        
        argi = 2;
        if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
            opserr << "WARNING invalid expSetup NoTransformation tag\n";
            return TCL_ERROR;
        }
        argi++;
        if (strcmp(argv[argi], "-control") == 0)  {
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
        // now read the DOF IDs
        if (strcmp(argv[argi],"-dof") != 0 && strcmp(argv[argi],"-dir") != 0)  {
            opserr << "WARNING expecting -dof DOFs\n";
            opserr << "expSetup NoTransformation " << tag << endln;
            return TCL_ERROR;
        }
        argi++;
        while (argi+numDOF < argc &&
            strcmp(argv[argi+numDOF],"-sizeTrialOut") != 0)  {
            numDOF++;
        }
        if (numDOF == 0)  {
            opserr << "WARNING no DOFs specified\n";
            opserr << "expSetup NoTransformation " << tag << endln;
            return TCL_ERROR;
        }
        // create an ID array to hold the DOFs
        ID theDOF(numDOF);
        // read the DOF identifiers
        for (i=0; i<numDOF; i++)  {
            if (Tcl_GetInt(interp, argv[argi], &dof) != TCL_OK)  {
                opserr << "WARNING invalid DOF\n";
                opserr << "expSetup NoTransformation " << tag << endln;
                return TCL_ERROR;
            }
            theDOF[i] = dof - 1;
            argi++;
        }
        // now read the trial and out sizes
        if (argi >= argc || strcmp(argv[argi],"-sizeTrialOut") != 0)  {
            opserr << "WARNING expecting -sizeTrialOut t o\n";
            opserr << "expSetup NoTransformation " << tag << endln;
            return TCL_ERROR;
        }
        argi++;
        if (Tcl_GetInt(interp, argv[argi], &sizeTrial) != TCL_OK)  {
            opserr << "WARNING invalid sizeTrial\n";
            opserr << "expSetup NoTransformation " << tag << endln;
            return TCL_ERROR;
        }
        argi++;
        if (Tcl_GetInt(interp, argv[argi], &sizeOut) != TCL_OK)  {
            opserr << "WARNING invalid sizeOut\n";
            opserr << "expSetup NoTransformation " << tag << endln;
            return TCL_ERROR;
        }
        argi++;
        
        // parsing was successful, allocate the setup
        theSetup = new ESNoTransformation(tag, theDOF, sizeTrial, sizeOut, theControl);
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"OneActuator") == 0)  {
        if (argc < 7)  {
            opserr << "WARNING invalid number of arguments\n";
            printCommand(argc, argv);
            opserr << "Want: expSetup OneActuator tag <-control ctrlTag> "
                << "DOF -sizeTrialOut t o\n";
            return TCL_ERROR;
        }    
        
        int ctrlTag, dof, sizeTrial, sizeOut;
        
        argi = 2;
        if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
            opserr << "WARNING invalid expSetup OneActuator tag\n";
            return TCL_ERROR;
        }
        argi++;
        if (strcmp(argv[argi], "-control") == 0)  {
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
        if (Tcl_GetInt(interp, argv[argi], &dof) != TCL_OK)  {
            opserr << "WARNING invalid DOF\n";
            opserr << "expSetup OneActuator " << tag << endln;
            return TCL_ERROR;
        }
        dof--;
        argi++;
        // now read the trial and out sizes
        if (argi >= argc || strcmp(argv[argi],"-sizeTrialOut") != 0)  {
            opserr << "WARNING expecting -sizeTrialOut t o\n";
            opserr << "expSetup OneActuator " << tag << endln;
            return TCL_ERROR;
        }
        argi++;
        if (Tcl_GetInt(interp, argv[argi], &sizeTrial) != TCL_OK)  {
            opserr << "WARNING invalid sizeTrial\n";
            opserr << "expSetup OneActuator " << tag << endln;
            return TCL_ERROR;
        }
        argi++;
        if (Tcl_GetInt(interp, argv[argi], &sizeOut) != TCL_OK)  {
            opserr << "WARNING invalid sizeOut\n";
            opserr << "expSetup OneActuator " << tag << endln;
            return TCL_ERROR;
        }
        argi++;
        
        // parsing was successful, allocate the setup
        theSetup = new ESOneActuator(tag, dof, sizeTrial, sizeOut, theControl);
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"TwoActuators2d") == 0)  {
        if (ndm == 2)  {
            if (argc < 6)  {
                opserr << "WARNING invalid number of arguments\n";
                printCommand(argc, argv);
                opserr << "Want: expSetup TwoActuators2d tag <-control ctrlTag> "
                    << "La1 La2 L "
                    << "<-nlGeom> <-posAct pos> <-phiLocX phi>\n";
                return TCL_ERROR;
            }
            
            int ctrlTag, i;
            double La1, La2, L;
            int nlGeom = 0;
            char posAct[6] = {'l','e','f','t','\0'};
            double phiLocX = 0.0;
            
            argi = 2;
            if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
                opserr << "WARNING invalid expSetup TwoActuators2d tag\n";
                return TCL_ERROR;
            }
            argi++;
            if (strcmp(argv[argi], "-control") == 0)  {
                argi++;
                if (Tcl_GetInt(interp, argv[argi], &ctrlTag) != TCL_OK)  {
                    opserr << "WARNING invalid ctrlTag\n";
                    opserr << "expSetup TwoActuators2d " << tag << endln;
                    return TCL_ERROR;
                }
                theControl = getExperimentalControl(ctrlTag);
                if (theControl == 0)  {
                    opserr << "WARNING experimental control not found\n";
                    opserr << "expControl: " << ctrlTag << endln;
                    opserr << "expSetup TwoActuators2d " << tag << endln;
                    return TCL_ERROR;
                }
                argi++;
            }
            if (Tcl_GetDouble(interp, argv[argi], &La1) != TCL_OK)  {
                opserr << "WARNING invalid La1\n";
                opserr << "expSetup TwoActuators2d " << tag << endln;
                return TCL_ERROR;
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &La2) != TCL_OK)  {
                opserr << "WARNING invalid La2\n";
                opserr << "expSetup TwoActuators2d " << tag << endln;
                return TCL_ERROR;
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L) != TCL_OK)  {
                opserr << "WARNING invalid L\n";
                opserr << "expSetup TwoActuators2d " << tag << endln;
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
                        opserr << "expSetup TwoActuators2d " << tag << endln;
                        return TCL_ERROR;
                    }
                }
            }
            
            // parsing was successful, allocate the setup
            theSetup = new ESTwoActuators2d(tag, La1, La2, L,
                theControl, nlGeom, posAct, phiLocX);
        }
        
        else if (ndm == 3)  {
            // not implemented yet
            opserr << "WARNING expSetup TwoActuators2d command not implemented yet for ndm = 3\n";
            return TCL_ERROR;
        }
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1], "ThreeActuators2d") == 0)  {
        if (ndm == 2)  {
            if (argc < 8)  {
                opserr << "WARNING invalid number of arguments\n";
                printCommand(argc, argv);
                opserr << "Want: expSetup ThreeActuators2d tag <-control ctrlTag> "
                    << "La1 La2 La3 L1 L2 "
                    << "<-nlGeom> <-posAct1 pos> <-phiLocX phi>\n";
                return TCL_ERROR;
            }
            
            int ctrlTag, i;
            double La1, La2, La3, L1, L2;
            int nlGeom = 0;
            char posAct0[6] = {'l','e','f','t','\0'};
            double phiLocX = 0.0;
            
            argi = 2;
            if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
                opserr << "WARNING invalid expSetup ThreeActuators2d tag\n";
                return TCL_ERROR;
            }
            argi++;
            if (strcmp(argv[argi], "-control") == 0)  {
                argi++;
                if (Tcl_GetInt(interp, argv[argi], &ctrlTag) != TCL_OK)  {
                    opserr << "WARNING invalid ctrlTag\n";
                    opserr << "expSetup ThreeActuators2d " << tag << endln;
                    return TCL_ERROR;
                }
                theControl = getExperimentalControl(ctrlTag);
                if (theControl == 0)  {
                    opserr << "WARNING experimental control not found\n";
                    opserr << "expControl: " << ctrlTag << endln;
                    opserr << "expSetup ThreeActuators2d " << tag << endln;
                    return TCL_ERROR;
                }
                argi++;
            }
            if (Tcl_GetDouble(interp, argv[argi], &La1) != TCL_OK)  {
                opserr << "WARNING invalid La1\n";
                opserr << "expSetup ThreeActuators2d " << tag << endln;
                return TCL_ERROR;
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &La2) != TCL_OK)  {
                opserr << "WARNING invalid La2\n";
                opserr << "expSetup ThreeActuators2d " << tag << endln;
                return TCL_ERROR;
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &La3) != TCL_OK)  {
                opserr << "WARNING invalid La3\n";
                opserr << "expSetup ThreeActuators2d " << tag << endln;
                return TCL_ERROR;
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L1) != TCL_OK)  {
                opserr << "WARNING invalid L1\n";
                opserr << "expSetup ThreeActuators2d " << tag << endln;
                return TCL_ERROR;
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L2) != TCL_OK)  {
                opserr << "WARNING invalid L2\n";
                opserr << "expSetup ThreeActuators2d " << tag << endln;
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
                        opserr << "expSetup ThreeActuators2d " << tag << endln;
                        return TCL_ERROR;
                    }
                }
            }
            
            // parsing was successful, allocate the setup
            theSetup = new ESThreeActuators2d(tag, La1, La2, La3, L1, L2,
                theControl, nlGeom, posAct0, phiLocX);
        }
        
        else if (ndm == 3)  {
            // not implemented yet
            opserr << "WARNING expSetup ThreeActuators2d command not implemented yet for ndm = 3\n";
            return TCL_ERROR;
        }
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1], "ThreeActuators") == 0) {
        if (argc < 11)  {
            opserr << "WARNING invalid number of arguments\n";
            printCommand(argc, argv);
            opserr << "Want: expSetup ThreeActuators tag <-control ctrlTag> "
                << "dofH dofV dofR sizeTrial sizeOut La1 La2 La3 L1 L2 "
                << "<-nlGeom> <-posAct1 pos>\n";
            return TCL_ERROR;
        }
        
        int ctrlTag, dof, sizeTrial, sizeOut, i;
        ID theDOF(3);
        double La1, La2, La3, L1, L2;
        int nlGeom = 0;
        char posAct0[6] = {'l','e','f','t','\0'};
        
        argi = 2;
        if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
            opserr << "WARNING invalid expSetup ThreeActuators tag\n";
            return TCL_ERROR;
        }
        argi++;
        if (strcmp(argv[argi], "-control") == 0)  {
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
        if (Tcl_GetInt(interp, argv[argi], &dof) != TCL_OK)  {
            opserr << "WARNING invalid dofH\n";
            opserr << "expSetup ThreeActuators " << tag << endln;
            return TCL_ERROR;
        }
        theDOF[0] = dof - 1;
        argi++;
        if (Tcl_GetInt(interp, argv[argi], &dof) != TCL_OK)  {
            opserr << "WARNING invalid dofV\n";
            opserr << "expSetup ThreeActuators " << tag << endln;
            return TCL_ERROR;
        }
        theDOF[1] = dof - 1;
        argi++;
        if (Tcl_GetInt(interp, argv[argi], &dof) != TCL_OK)  {
            opserr << "WARNING invalid dofR\n";
            opserr << "expSetup ThreeActuators " << tag << endln;
            return TCL_ERROR;
        }
        theDOF[2] = dof - 1;
        argi++;
        if (Tcl_GetInt(interp, argv[argi], &sizeTrial) != TCL_OK) {
            opserr << "WARNING invalid sizeTrial\n";
            opserr << "expSetup ThreeActuators " << tag << endln;
            return TCL_ERROR;
        }
        argi++;
        if (Tcl_GetInt(interp, argv[argi], &sizeOut) != TCL_OK) {
            opserr << "WARNING invalid sizeOut\n";
            opserr << "expSetup ThreeActuators " << tag << endln;
            return TCL_ERROR;
        }
        argi++;
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
                if (strcmp(argv[i + 1], "left") == 0 || strcmp(argv[i + 1], "l") == 0)
                    strcpy(posAct0, "left");
                else if (strcmp(argv[i + 1], "right") == 0 || strcmp(argv[i + 1], "r") == 0)
                    strcpy(posAct0, "right");
            }
        }
        
        // parsing was successful, allocate the setup
        theSetup = new ESThreeActuators(tag, theDOF, sizeTrial, sizeOut,
            La1, La2, La3, L1, L2, theControl, nlGeom, posAct0);
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"ThreeActuatorsJntOff2d") == 0)  {
        if (ndm == 2)  {
            if (argc < 12)  {
                opserr << "WARNING invalid number of arguments\n";
                printCommand(argc,argv);
                opserr << "Want: expSetup ThreeActuatorsJntOff2d tag <-control ctrlTag> "
                    << "La1 La2 La3 L1 L2 L3 L4 L5 L6 "
                    << "<-nlGeom> <-posAct1 pos> <-phiLocX phi>\n";
                return TCL_ERROR;
            }    
            
            int ctrlTag, i;
            double La1, La2, La3, L1, L2, L3, L4, L5, L6;
            int nlGeom = 0;
            char posAct0[6] = {'l','e','f','t','\0'};
            double phiLocX = 0.0;
            
            argi = 2;
            if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
                opserr << "WARNING invalid expSetup ThreeActuatorsJntOff2d tag\n";
                return TCL_ERROR;
            }
            argi++;
            if (strcmp(argv[argi], "-control") == 0)  {
                argi++;
                if (Tcl_GetInt(interp, argv[argi], &ctrlTag) != TCL_OK)  {
                    opserr << "WARNING invalid ctrlTag\n";
                    opserr << "expSetup ThreeActuatorsJntOff2d " << tag << endln;
                    return TCL_ERROR;
                }
                theControl = getExperimentalControl(ctrlTag);
                if (theControl == 0)  {
                    opserr << "WARNING experimental control not found\n";
                    opserr << "expControl: " << ctrlTag << endln;
                    opserr << "expSetup ThreeActuatorsJntOff2d " << tag << endln;
                    return TCL_ERROR;
                }
                argi++;
            }
            if (Tcl_GetDouble(interp, argv[argi], &La1) != TCL_OK)  {
                opserr << "WARNING invalid La1\n";
                opserr << "expSetup ThreeActuatorsJntOff2d " << tag << endln;
                return TCL_ERROR;
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &La2) != TCL_OK)  {
                opserr << "WARNING invalid La2\n";
                opserr << "expSetup ThreeActuatorsJntOff2d " << tag << endln;
                return TCL_ERROR;
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &La3) != TCL_OK)  {
                opserr << "WARNING invalid La3\n";
                opserr << "expSetup ThreeActuatorsJntOff2d " << tag << endln;
                return TCL_ERROR;
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L1) != TCL_OK)  {
                opserr << "WARNING invalid L1\n";
                opserr << "expSetup ThreeActuatorsJntOff2d " << tag << endln;
                return TCL_ERROR;
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L2) != TCL_OK)  {
                opserr << "WARNING invalid L2\n";
                opserr << "expSetup ThreeActuatorsJntOff2d " << tag << endln;
                return TCL_ERROR;
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L3) != TCL_OK)  {
                opserr << "WARNING invalid L3\n";
                opserr << "expSetup ThreeActuatorsJntOff2d " << tag << endln;
                return TCL_ERROR;
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L4) != TCL_OK)  {
                opserr << "WARNING invalid L4\n";
                opserr << "expSetup ThreeActuatorsJntOff2d " << tag << endln;
                return TCL_ERROR;
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L5) != TCL_OK)  {
                opserr << "WARNING invalid L5\n";
                opserr << "expSetup ThreeActuatorsJntOff2d " << tag << endln;
                return TCL_ERROR;
            }
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &L6) != TCL_OK)  {
                opserr << "WARNING invalid L6\n";
                opserr << "expSetup ThreeActuatorsJntOff2d " << tag << endln;
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
                        opserr << "expSetup ThreeActuatorsJntOff2d " << tag << endln;
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
            opserr << "WARNING expSetup ThreeActuatorsJntOff2d command not implemented yet for ndm = 3\n";
            return TCL_ERROR;
        }
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1], "ThreeActuatorsJntOff") == 0) {
    if (argc < 15) {
        opserr << "WARNING invalid number of arguments\n";
        printCommand(argc, argv);
        opserr << "Want: expSetup ThreeActuatorsJntOff tag <-control ctrlTag> "
            << "dofH dofV dofR sizeTrial sizeOut La1 La2 La3 L1 L2 L3 L4 L5 L6"
            << "<-nlGeom> <-posAct1 pos>\n";
        return TCL_ERROR;
    }
    
    int ctrlTag, dof, sizeTrial, sizeOut, i;
    ID theDOF(3);
    double La1, La2, La3, L1, L2, L3, L4, L5, L6;
    int nlGeom = 0;
    char posAct0[6] = { 'l','e','f','t','\0' };
    
    argi = 2;
    if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK) {
        opserr << "WARNING invalid expSetup ThreeActuatorsJntOff tag\n";
        return TCL_ERROR;
    }
    argi++;
    if (strcmp(argv[argi], "-control") == 0) {
        argi++;
        if (Tcl_GetInt(interp, argv[argi], &ctrlTag) != TCL_OK) {
            opserr << "WARNING invalid ctrlTag\n";
            opserr << "expSetup ThreeActuatorsJntOff " << tag << endln;
            return TCL_ERROR;
        }
        theControl = getExperimentalControl(ctrlTag);
        if (theControl == 0) {
            opserr << "WARNING experimental control not found\n";
            opserr << "expControl: " << ctrlTag << endln;
            opserr << "expSetup ThreeActuatorsJntOff " << tag << endln;
            return TCL_ERROR;
        }
        argi++;
    }
    if (Tcl_GetInt(interp, argv[argi], &dof) != TCL_OK) {
        opserr << "WARNING invalid dofH\n";
        opserr << "expSetup ThreeActuatorsJntOff " << tag << endln;
        return TCL_ERROR;
    }
    theDOF[0] = dof - 1;
    argi++;
    if (Tcl_GetInt(interp, argv[argi], &dof) != TCL_OK) {
        opserr << "WARNING invalid dofV\n";
        opserr << "expSetup ThreeActuatorsJntOff " << tag << endln;
        return TCL_ERROR;
    }
    theDOF[1] = dof - 1;
    argi++;
    if (Tcl_GetInt(interp, argv[argi], &dof) != TCL_OK) {
        opserr << "WARNING invalid dofR\n";
        opserr << "expSetup ThreeActuatorsJntOff " << tag << endln;
        return TCL_ERROR;
    }
    theDOF[2] = dof - 1;
    argi++;
    if (Tcl_GetInt(interp, argv[argi], &sizeTrial) != TCL_OK) {
        opserr << "WARNING invalid sizeTrial\n";
        opserr << "expSetup ThreeActuatorsJntOff " << tag << endln;
        return TCL_ERROR;
    }
    argi++;
    if (Tcl_GetInt(interp, argv[argi], &sizeOut) != TCL_OK) {
        opserr << "WARNING invalid sizeOut\n";
        opserr << "expSetup ThreeActuatorsJntOff " << tag << endln;
        return TCL_ERROR;
    }
    argi++;
    if (Tcl_GetDouble(interp, argv[argi], &La1) != TCL_OK) {
        opserr << "WARNING invalid La1\n";
        opserr << "expSetup ThreeActuatorsJntOff " << tag << endln;
        return TCL_ERROR;
    }
    argi++;
    if (Tcl_GetDouble(interp, argv[argi], &La2) != TCL_OK) {
        opserr << "WARNING invalid La2\n";
        opserr << "expSetup ThreeActuatorsJntOff " << tag << endln;
        return TCL_ERROR;
    }
    argi++;
    if (Tcl_GetDouble(interp, argv[argi], &La3) != TCL_OK) {
        opserr << "WARNING invalid La3\n";
        opserr << "expSetup ThreeActuatorsJntOff " << tag << endln;
        return TCL_ERROR;
    }
    argi++;
    if (Tcl_GetDouble(interp, argv[argi], &L1) != TCL_OK) {
        opserr << "WARNING invalid L1\n";
        opserr << "expSetup ThreeActuatorsJntOff " << tag << endln;
        return TCL_ERROR;
    }
    argi++;
    if (Tcl_GetDouble(interp, argv[argi], &L2) != TCL_OK) {
        opserr << "WARNING invalid L2\n";
        opserr << "expSetup ThreeActuatorsJntOff " << tag << endln;
        return TCL_ERROR;
    }
    argi++;
    if (Tcl_GetDouble(interp, argv[argi], &L3) != TCL_OK) {
        opserr << "WARNING invalid L3\n";
        opserr << "expSetup ThreeActuatorsJntOff " << tag << endln;
        return TCL_ERROR;
    }
    argi++;
    if (Tcl_GetDouble(interp, argv[argi], &L4) != TCL_OK) {
        opserr << "WARNING invalid L4\n";
        opserr << "expSetup ThreeActuatorsJntOff " << tag << endln;
        return TCL_ERROR;
    }
    argi++;
    if (Tcl_GetDouble(interp, argv[argi], &L5) != TCL_OK) {
        opserr << "WARNING invalid L5\n";
        opserr << "expSetup ThreeActuatorsJntOff " << tag << endln;
        return TCL_ERROR;
    }
    argi++;
    if (Tcl_GetDouble(interp, argv[argi], &L6) != TCL_OK) {
        opserr << "WARNING invalid L6\n";
        opserr << "expSetup ThreeActuatorsJntOff " << tag << endln;
        return TCL_ERROR;
    }
    argi++;
    for (i = argi; i < argc; i++) {
        if (strcmp(argv[i], "-nlGeom") == 0) {
            nlGeom = 1;
        }
    }
    for (i = argi; i < argc; i++) {
        if (strcmp(argv[i], "-posAct1") == 0) {
            if (strcmp(argv[i + 1], "left") == 0 || strcmp(argv[i + 1], "l") == 0)
                strcpy(posAct0, "left");
            else if (strcmp(argv[i + 1], "right") == 0 || strcmp(argv[i + 1], "r") == 0)
                strcpy(posAct0, "right");
        }
    }
    
    // parsing was successful, allocate the setup
    theSetup = new ESThreeActuatorsJntOff(tag, theDOF, sizeTrial, sizeOut,
        La1, La2, La3, L1, L2, L3, L4, L5, L6, theControl, nlGeom, posAct0);
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"InvertedVBrace") == 0)  {
        if (ndm == 2)  {
            if (argc < 8)  {
                opserr << "WARNING invalid number of arguments\n";
                printCommand(argc, argv);
                opserr << "Want: expSetup InvertedVBrace tag <-control ctrlTag> "
                    << "La1 La2 La3 L1 L2 "
                    << "<-nlGeom> <-posAct1 pos> <-phiLocX phi>\n";
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
            if (strcmp(argv[argi], "-control") == 0)  {
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
            theSetup = new ESInvertedVBrace2d(tag, La1, La2, La3, L1, L2,
                theControl, nlGeom, posAct0, phiLocX);
        }
        
        else if (ndm == 3)  {
            // not implemented yet
            opserr << "WARNING expSetup InvertedVBrace command not implemented yet for ndm = 3\n";
            return TCL_ERROR;
        }
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"InvertedVBraceJntOff") == 0)  {
        if (ndm == 2)  {
            if (argc < 12)  {
                opserr << "WARNING invalid number of arguments\n";
                printCommand(argc, argv);
                opserr << "Want: expSetup InvertedVBraceJntOff tag <-control ctrlTag> "
                    << "La1 La2 La3 L1 L2 L3 L4 L5 L6 "
                    << "<-nlGeom> <-posAct1 pos> <-phiLocX phi>\n";
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
            if (strcmp(argv[argi], "-control") == 0)  {
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
            printCommand(argc, argv);
            opserr << "Want: expSetup Aggregator tag <-control ctrlTag> "
                << "-setup setupTagi ...\n";
            return TCL_ERROR;
        }
        
        int ctrlTag, numSetups = 0, setupTag, i;
        
        argi = 2;
        if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
            opserr << "WARNING invalid expSetup Aggregator tag\n";
            return TCL_ERROR;
        }
        argi++;
        if (strcmp(argv[argi], "-control") == 0)  {
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
        while (argi+numSetups < argc &&
            strcmp(argv[argi+numSetups],"-sizeTrialOut") != 0)  {
            numSetups++;
        }
        if (numSetups == 0)  {
            opserr << "WARNING no setups specified\n";
            opserr << "expSetup Aggregator " << tag << endln;
            return TCL_ERROR;
        }
        // create the arrays to hold the setups and sizes
        ExperimentalSetup **expSetups = new ExperimentalSetup* [numSetups];
        if (expSetups == 0)  {
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
        
        // parsing was successful, allocate the setup
        theSetup = new ESAggregator(tag, numSetups, expSetups, theControl);
        
        // cleanup dynamic memory
        if (expSetups != 0)  {
            for (i=0; i<numSetups; i++)
                if (expSetups[i] != 0)
                    delete expSetups[i];
            delete [] expSetups;
        }
    }
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"FourActuators") == 0)  {
        if (ndm == 3)  {
            if (argc < 19)  {
                opserr << "WARNING invalid number of arguments\n";
                printCommand(argc, argv);
                opserr << "Want: expSetup FourActuators tag <-control ctrlTag> "
                    << "L1 L2 L3 L4 a1 a2 a3 a4 h h1 h2 arlN arlS LrodN LrodS Hbeam "
                    << " <-nlGeom> <-phiLocX phi>\n";
                return TCL_ERROR;
            }
            
            int ctrlTag, i;
            double L1, L2, L3, L4, a1, a2, a3, a4, h, h1, h2, arlN, arlS, LrodN, LrodS, Hbeam;
            int nlGeom = 0;
            double phiLocX = 45.0;
            
            argi = 2;
            if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
                opserr << "WARNING invalid expSetup FourActuators tag\n";
                return TCL_ERROR;
            }
            argi++;
            if (strcmp(argv[argi], "-control") == 0)  {
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
            if (Tcl_GetDouble(interp, argv[argi], &Hbeam) != TCL_OK)  {
                opserr << "WARNING invalid Hbeam\n";
                opserr << "expSetup FourActuators " << tag << endln;
                return TCL_ERROR;
            }
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
            theSetup = new ESFourActuators3d(tag, L1, L2, L3, L4, a1, a2, a3, a4,
                h, h1, h2, arlN, arlS, LrodN, LrodS, Hbeam, theControl, nlGeom, phiLocX);
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
    
    // finally check for the factors
    int i, size;
    size = theSetup->getTrialSize(OF_Resp_Disp);
    for (i = argi; i < argc; i++)  {
        if (i+1 < argc && (strcmp(argv[i], "-trialDspFact") == 0 ||
            strcmp(argv[i], "-trialDispFact") == 0 ||
            strcmp(argv[i], "-trialDisplacementFact") == 0
            ))  {
            if (size <= 0)  {
                opserr << "WARNING trialDispSize <= 0\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
            }
            int numFact = 0;
            while (i+1+numFact < argc &&
                strncmp(argv[i+1+numFact],"-trial",6) != 0 &&
                strncmp(argv[i+1+numFact],"-out",4) != 0 &&
                strncmp(argv[i+1+numFact],"-ctrl",5) != 0 &&
                strncmp(argv[i+1+numFact],"-daq",4) != 0)  {
                numFact++;
            }
            if (numFact != size)  {
                opserr << "WARNING wrong number of trialDispFact: want ";
                opserr << size << " but got " << numFact << endln;
                opserr << "expSetup: " << tag << endln;
                return TCL_ERROR;
            }
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid trialDispFact\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setTrialDispFactor(factor);
        }
    }
    size = theSetup->getTrialSize(OF_Resp_Vel);
    for (i = argi; i < argc; i++)  {
        if (i+1 < argc && (strcmp(argv[i], "-trialVelFact") == 0 ||
            strcmp(argv[i], "-trialVelocityFact") == 0))  {
            if (size <= 0)  {
                opserr << "WARNING trialVelSize <= 0\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
            }
            int numFact = 0;
            while (i+1+numFact < argc &&
                strncmp(argv[i+1+numFact],"-trial",6) != 0 &&
                strncmp(argv[i+1+numFact],"-out",4) != 0 &&
                strncmp(argv[i+1+numFact],"-ctrl",5) != 0 &&
                strncmp(argv[i+1+numFact],"-daq",4) != 0)  {
                numFact++;
            }
            if (numFact != size)  {
                opserr << "WARNING wrong number of trialVelFact: want ";
                opserr << size << " but got " << numFact << endln;
                opserr << "expSetup: " << tag << endln;
                return TCL_ERROR;
            }
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid trialVelFact\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setTrialVelFactor(factor);
        }
    }
    size = theSetup->getTrialSize(OF_Resp_Accel);
    for (i = argi; i < argc; i++)  {
        if (i+1 < argc && (strcmp(argv[i], "-trialAccFact") == 0 ||
            strcmp(argv[i], "-trialAccelFact") == 0 ||
            strcmp(argv[i], "-trialAccelerationFact") == 0))  {
            if (size <= 0)  {
                opserr << "WARNING trialAccelSize <= 0\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
            }
            int numFact = 0;
            while (i+1+numFact < argc &&
                strncmp(argv[i+1+numFact],"-trial",6) != 0 &&
                strncmp(argv[i+1+numFact],"-out",4) != 0 &&
                strncmp(argv[i+1+numFact],"-ctrl",5) != 0 &&
                strncmp(argv[i+1+numFact],"-daq",4) != 0)  {
                numFact++;
            }
            if (numFact != size)  {
                opserr << "WARNING wrong number of trialAccelFact: want ";
                opserr << size << " but got " << numFact << endln;
                opserr << "expSetup: " << tag << endln;
                return TCL_ERROR;
            }
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid trialAccelFact\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setTrialAccelFactor(factor);
        }
    }
    size = theSetup->getTrialSize(OF_Resp_Force);
    for (i = argi; i < argc; i++)  {
        if (i+1 < argc && (strcmp(argv[i], "-trialFrcFact") == 0 ||
            strcmp(argv[i], "-trialForcFact") == 0 ||
            strcmp(argv[i], "-trialForceFact") == 0))  {
            if (size <= 0)  {
                opserr << "WARNING trialForceSize <= 0\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
            }
            int numFact = 0;
            while (i+1+numFact < argc &&
                strncmp(argv[i+1+numFact],"-trial",6) != 0 &&
                strncmp(argv[i+1+numFact],"-out",4) != 0 &&
                strncmp(argv[i+1+numFact],"-ctrl",5) != 0 &&
                strncmp(argv[i+1+numFact],"-daq",4) != 0)  {
                numFact++;
            }
            if (numFact != size)  {
                opserr << "WARNING wrong number of trialForceFact: want ";
                opserr << size << " but got " << numFact << endln;
                opserr << "expSetup: " << tag << endln;
                return TCL_ERROR;
            }
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid trialForceFact\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setTrialForceFactor(factor);
        }
    }
    size = theSetup->getTrialSize(OF_Resp_Time);
    for (i = argi; i < argc; i++)  {
        if (i+1 < argc && strcmp(argv[i], "-trialTimeFact") == 0)  {
            if (size <= 0)  {
                opserr << "WARNING trialTimeSize <= 0\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
            }
            int numFact = 0;
            while (i+1+numFact < argc &&
                strncmp(argv[i+1+numFact],"-trial",6) != 0 &&
                strncmp(argv[i+1+numFact],"-out",4) != 0 &&
                strncmp(argv[i+1+numFact],"-ctrl",5) != 0 &&
                strncmp(argv[i+1+numFact],"-daq",4) != 0)  {
                numFact++;
            }
            if (numFact != size)  {
                opserr << "WARNING wrong number of trialTimeFact: want ";
                opserr << size << " but got " << numFact << endln;
                opserr << "expSetup: " << tag << endln;
                return TCL_ERROR;
            }
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid trialTimeFact\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setTrialTimeFactor(factor);
        }
    }
    size = theSetup->getOutSize(OF_Resp_Disp);
    for (i = argi; i < argc; i++)  {
        if (i+1 < argc && (strcmp(argv[i], "-outDspFact") == 0 ||
            strcmp(argv[i], "-outDispFact") == 0 ||
            strcmp(argv[i], "-outDisplacementFact") == 0))  {
            if (size <= 0)  {
                opserr << "WARNING outDispSize <= 0\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
            }
            int numFact = 0;
            while (i+1+numFact < argc &&
                strncmp(argv[i+1+numFact],"-trial",6) != 0 &&
                strncmp(argv[i+1+numFact],"-out",4) != 0 &&
                strncmp(argv[i+1+numFact],"-ctrl",5) != 0 &&
                strncmp(argv[i+1+numFact],"-daq",4) != 0)  {
                numFact++;
            }
            if (numFact != size)  {
                opserr << "WARNING wrong number of outDispFact: want ";
                opserr << size << " but got " << numFact << endln;
                opserr << "expSetup: " << tag << endln;
                return TCL_ERROR;
            }
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid outDispFact\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setOutDispFactor(factor);
        }
    }
    size = theSetup->getOutSize(OF_Resp_Vel);
    for (i = argi; i < argc; i++)  {
        if (i+1 < argc && (strcmp(argv[i], "-outVelFact") == 0 ||
            strcmp(argv[i], "-outVelocityFact") == 0))  {
            if (size <= 0)  {
                opserr << "WARNING outVelSize <= 0\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
            }
            int numFact = 0;
            while (i+1+numFact < argc &&
                strncmp(argv[i+1+numFact],"-trial",6) != 0 &&
                strncmp(argv[i+1+numFact],"-out",4) != 0 &&
                strncmp(argv[i+1+numFact],"-ctrl",5) != 0 &&
                strncmp(argv[i+1+numFact],"-daq",4) != 0)  {
                numFact++;
            }
            if (numFact != size)  {
                opserr << "WARNING wrong number of outVelFact: want ";
                opserr << size << " but got " << numFact << endln;
                opserr << "expSetup: " << tag << endln;
                return TCL_ERROR;
            }
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid outVelFact\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setOutVelFactor(factor);
        }
    }
    size = theSetup->getOutSize(OF_Resp_Accel);
    for (i = argi; i < argc; i++)  {
        if (i+1 < argc && (strcmp(argv[i], "-outAccFact") == 0 ||
            strcmp(argv[i], "-outAccelFact") == 0 ||
            strcmp(argv[i], "-outAccelerationFact") == 0))  {
            if (size <= 0)  {
                opserr << "WARNING outAccelSize <= 0\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
            }
            int numFact = 0;
            while (i+1+numFact < argc &&
                strncmp(argv[i+1+numFact],"-trial",6) != 0 &&
                strncmp(argv[i+1+numFact],"-out",4) != 0 &&
                strncmp(argv[i+1+numFact],"-ctrl",5) != 0 &&
                strncmp(argv[i+1+numFact],"-daq",4) != 0)  {
                numFact++;
            }
            if (numFact != size)  {
                opserr << "WARNING wrong number of outAccelFact: want ";
                opserr << size << " but got " << numFact << endln;
                opserr << "expSetup: " << tag << endln;
                return TCL_ERROR;
            }
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid outAccelFact\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setOutAccelFactor(factor);
        }
    }
    size = theSetup->getOutSize(OF_Resp_Force);
    for (i = argi; i < argc; i++)  {
        if (i+1 < argc && (strcmp(argv[i], "-outFrcFact") == 0 ||
            strcmp(argv[i], "-outForcFact") == 0 ||
            strcmp(argv[i], "-outForceFact") == 0))  {
            if (size <= 0)  {
                opserr << "WARNING outForceSize <= 0\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
            }
            int numFact = 0;
            while (i+1+numFact < argc &&
                strncmp(argv[i+1+numFact],"-trial",6) != 0 &&
                strncmp(argv[i+1+numFact],"-out",4) != 0 &&
                strncmp(argv[i+1+numFact],"-ctrl",5) != 0 &&
                strncmp(argv[i+1+numFact],"-daq",4) != 0)  {
                numFact++;
            }
            if (numFact != size)  {
                opserr << "WARNING wrong number of outForceFact: want ";
                opserr << size << " but got " << numFact << endln;
                opserr << "expSetup: " << tag << endln;
                return TCL_ERROR;
            }
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid outForceFact\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setOutForceFactor(factor);
        }
    }
    size = theSetup->getOutSize(OF_Resp_Time);
    for (i = argi; i < argc; i++)  {
        if (i+1 < argc && strcmp(argv[i], "-outTimeFact") == 0)  {
            if (size <= 0)  {
                opserr << "WARNING outTimeSize <= 0\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
            }
            int numFact = 0;
            while (i+1+numFact < argc &&
                strncmp(argv[i+1+numFact],"-trial",6) != 0 &&
                strncmp(argv[i+1+numFact],"-out",4) != 0 &&
                strncmp(argv[i+1+numFact],"-ctrl",5) != 0 &&
                strncmp(argv[i+1+numFact],"-daq",4) != 0)  {
                numFact++;
            }
            if (numFact != size)  {
                opserr << "WARNING wrong number of outTimeFact: want ";
                opserr << size << " but got " << numFact << endln;
                opserr << "expSetup: " << tag << endln;
                return TCL_ERROR;
            }
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid outTimeFact\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setOutTimeFactor(factor);
        }
    }
    size = theSetup->getCtrlSize(OF_Resp_Disp);
    for (i = argi; i < argc; i++)  {
        if (i+1 < argc && (strcmp(argv[i], "-ctrlDspFact") == 0 ||
            strcmp(argv[i], "-ctrlDispFact") == 0 ||
            strcmp(argv[i], "-ctrlDisplacementFact") == 0))  {
            if (size <= 0)  {
                opserr << "WARNING ctrlDispSize <= 0\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
            }
            int numFact = 0;
            while (i+1+numFact < argc &&
                strncmp(argv[i+1+numFact],"-trial",6) != 0 &&
                strncmp(argv[i+1+numFact],"-out",4) != 0 &&
                strncmp(argv[i+1+numFact],"-ctrl",5) != 0 &&
                strncmp(argv[i+1+numFact],"-daq",4) != 0)  {
                numFact++;
            }
            if (numFact != size)  {
                opserr << "WARNING wrong number of ctrlDispFact: want ";
                opserr << size << " but got " << numFact << endln;
                opserr << "expSetup: " << tag << endln;
                return TCL_ERROR;
            }
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
        if (i+1 < argc && (strcmp(argv[i], "-ctrlVelFact") == 0 ||
            strcmp(argv[i], "-ctrlVelocityFact") == 0))  {
            if (size <= 0)  {
                opserr << "WARNING ctrlVelSize <= 0\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
            }
            int numFact = 0;
            while (i+1+numFact < argc &&
                strncmp(argv[i+1+numFact],"-trial",6) != 0 &&
                strncmp(argv[i+1+numFact],"-out",4) != 0 &&
                strncmp(argv[i+1+numFact],"-ctrl",5) != 0 &&
                strncmp(argv[i+1+numFact],"-daq",4) != 0)  {
                numFact++;
            }
            if (numFact != size)  {
                opserr << "WARNING wrong number of ctrlVelFact: want ";
                opserr << size << " but got " << numFact << endln;
                opserr << "expSetup: " << tag << endln;
                return TCL_ERROR;
            }
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
        if (i+1 < argc && (strcmp(argv[i], "-ctrlAccFact") == 0 ||
            strcmp(argv[i], "-ctrlAccelFact") == 0 ||
            strcmp(argv[i], "-ctrlAccelerationFact") == 0))  {
            if (size <= 0)  {
                opserr << "WARNING ctrlAccelSize <= 0\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
            }
            int numFact = 0;
            while (i+1+numFact < argc &&
                strncmp(argv[i+1+numFact],"-trial",6) != 0 &&
                strncmp(argv[i+1+numFact],"-out",4) != 0 &&
                strncmp(argv[i+1+numFact],"-ctrl",5) != 0 &&
                strncmp(argv[i+1+numFact],"-daq",4) != 0)  {
                numFact++;
            }
            if (numFact != size)  {
                opserr << "WARNING wrong number of ctrlAccelFact: want ";
                opserr << size << " but got " << numFact << endln;
                opserr << "expSetup: " << tag << endln;
                return TCL_ERROR;
            }
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
        if (i+1 < argc && (strcmp(argv[i], "-ctrlFrcFact") == 0 ||
            strcmp(argv[i], "-ctrlForcFact") == 0 ||
            strcmp(argv[i], "-ctrlForceFact") == 0))  {
            if (size <= 0)  {
                opserr << "WARNING ctrlForceSize <= 0\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
            }
            int numFact = 0;
            while (i+1+numFact < argc &&
                strncmp(argv[i+1+numFact],"-trial",6) != 0 &&
                strncmp(argv[i+1+numFact],"-out",4) != 0 &&
                strncmp(argv[i+1+numFact],"-ctrl",5) != 0 &&
                strncmp(argv[i+1+numFact],"-daq",4) != 0)  {
                numFact++;
            }
            if (numFact != size)  {
                opserr << "WARNING wrong number of ctrlForceFact: want ";
                opserr << size << " but got " << numFact << endln;
                opserr << "expSetup: " << tag << endln;
                return TCL_ERROR;
            }
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
            if (size <= 0)  {
                opserr << "WARNING ctrlTimeSize <= 0\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
            }
            int numFact = 0;
            while (i+1+numFact < argc &&
                strncmp(argv[i+1+numFact],"-trial",6) != 0 &&
                strncmp(argv[i+1+numFact],"-out",4) != 0 &&
                strncmp(argv[i+1+numFact],"-ctrl",5) != 0 &&
                strncmp(argv[i+1+numFact],"-daq",4) != 0)  {
                numFact++;
            }
            if (numFact != size)  {
                opserr << "WARNING wrong number of ctrlTimeFact: want ";
                opserr << size << " but got " << numFact << endln;
                opserr << "expSetup: " << tag << endln;
                return TCL_ERROR;
            }
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
        if (i+1 < argc && (strcmp(argv[i], "-daqDspFact") == 0 ||
            strcmp(argv[i], "-daqDispFact") == 0 ||
            strcmp(argv[i], "-daqDisplacementFact") == 0))  {
            if (size <= 0)  {
                opserr << "WARNING daqDispSize <= 0\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
            }
            int numFact = 0;
            while (i+1+numFact < argc &&
                strncmp(argv[i+1+numFact],"-trial",6) != 0 &&
                strncmp(argv[i+1+numFact],"-out",4) != 0 &&
                strncmp(argv[i+1+numFact],"-ctrl",5) != 0 &&
                strncmp(argv[i+1+numFact],"-daq",4) != 0)  {
                numFact++;
            }
            if (numFact != size)  {
                opserr << "WARNING wrong number of daqDispFact: want ";
                opserr << size << " but got " << numFact << endln;
                opserr << "expSetup: " << tag << endln;
                return TCL_ERROR;
            }
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
        if (i+1 < argc && (strcmp(argv[i], "-daqVelFact") == 0 ||
            strcmp(argv[i], "-daqVelocityFact") == 0))  {
            if (size <= 0)  {
                opserr << "WARNING daqVelSize <= 0\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
            }
            int numFact = 0;
            while (i+1+numFact < argc &&
                strncmp(argv[i+1+numFact],"-trial",6) != 0 &&
                strncmp(argv[i+1+numFact],"-out",4) != 0 &&
                strncmp(argv[i+1+numFact],"-ctrl",5) != 0 &&
                strncmp(argv[i+1+numFact],"-daq",4) != 0)  {
                numFact++;
            }
            if (numFact != size)  {
                opserr << "WARNING wrong number of daqVelFact: want ";
                opserr << size << " but got " << numFact << endln;
                opserr << "expSetup: " << tag << endln;
                return TCL_ERROR;
            }
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
        if (i+1 < argc && (strcmp(argv[i], "-daqAccFact") == 0 ||
            strcmp(argv[i], "-daqAccelFact") == 0 ||
            strcmp(argv[i], "-daqAccelerationFact") == 0))  {
            if (size <= 0)  {
                opserr << "WARNING daqAccelSize <= 0\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
            }
            int numFact = 0;
            while (i+1+numFact < argc &&
                strncmp(argv[i+1+numFact],"-trial",6) != 0 &&
                strncmp(argv[i+1+numFact],"-out",4) != 0 &&
                strncmp(argv[i+1+numFact],"-ctrl",5) != 0 &&
                strncmp(argv[i+1+numFact],"-daq",4) != 0)  {
                numFact++;
            }
            if (numFact != size)  {
                opserr << "WARNING wrong number of daqAccelFact: want ";
                opserr << size << " but got " << numFact << endln;
                opserr << "expSetup: " << tag << endln;
                return TCL_ERROR;
            }
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
        if (i+1 < argc && (strcmp(argv[i], "-daqFrcFact") == 0 ||
            strcmp(argv[i], "-daqForcFact") == 0 ||
            strcmp(argv[i], "-daqForceFact") == 0))  {
            if (size <= 0)  {
                opserr << "WARNING daqForceSize <= 0\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
            }
            int numFact = 0;
            while (i+1+numFact < argc &&
                strncmp(argv[i+1+numFact],"-trial",6) != 0 &&
                strncmp(argv[i+1+numFact],"-out",4) != 0 &&
                strncmp(argv[i+1+numFact],"-ctrl",5) != 0 &&
                strncmp(argv[i+1+numFact],"-daq",4) != 0)  {
                numFact++;
            }
            if (numFact != size)  {
                opserr << "WARNING wrong number of daqForceFact: want ";
                opserr << size << " but got " << numFact << endln;
                opserr << "expSetup: " << tag << endln;
                return TCL_ERROR;
            }
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
            if (size <= 0)  {
                opserr << "WARNING daqTimeSize <= 0\n";
                    opserr << "expSetup: " << tag << endln;
                    return TCL_ERROR;
            }
            int numFact = 0;
            while (i+1+numFact < argc &&
                strncmp(argv[i+1+numFact],"-trial",6) != 0 &&
                strncmp(argv[i+1+numFact],"-out",4) != 0 &&
                strncmp(argv[i+1+numFact],"-ctrl",5) != 0 &&
                strncmp(argv[i+1+numFact],"-daq",4) != 0)  {
                numFact++;
            }
            if (numFact != size)  {
                opserr << "WARNING wrong number of daqTimeFact: want ";
                opserr << size << " but got " << numFact << endln;
                opserr << "expSetup: " << tag << endln;
                return TCL_ERROR;
            }
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


int TclRemoveExpSetup(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv)
{
    if (strcmp(argv[1], "setup") == 0)  {
        if (argc != 3)  {
            opserr << "WARNING invalid number of arguments\n";
            printCommand(argc, argv);
            opserr << "Want: removeExp setup tag\n";
            return TCL_ERROR;
        }
        int tag;
        if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK)  {
            opserr << "WARNING invalid removeExp setup tag\n";
            return TCL_ERROR;
        }
        ExperimentalSetup *theSetup = removeExperimentalSetup(tag);
        if (theSetup != 0)  {
            delete theSetup;
            theSetup = 0;
        } else  {
            opserr << "WARNING could not remove expSetup with tag " << argv[2] << endln;
            return TCL_ERROR;
        }
    }
    else if (strcmp(argv[1], "setups") == 0)  {
        if (clearExperimentalSetups(interp) < 0)  {
            opserr << "WARNING could not remove expSetups\n";
            return TCL_ERROR;
        }
    }
    
    return TCL_OK;
}
