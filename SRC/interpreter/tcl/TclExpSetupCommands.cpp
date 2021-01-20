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

#include <ExperimentalSetup.h>

extern void* OPF_ESNoTransformation();
extern void* OPF_ESOneActuator();
extern void* OPF_ESTwoActuators2d();
extern void* OPF_ESThreeActuators2d();
extern void* OPF_ESThreeActuators();
extern void* OPF_ESThreeActuatorsJntOff2d();
extern void* OPF_ESThreeActuatorsJntOff();
extern void* OPF_ESFourActuators3d();
extern void* OPF_ESInvertedVBrace2d();
extern void* OPF_ESInvertedVBraceJntOff2d();
extern void* OPF_ESAggregator();


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
    // make sure there is a minimum number of arguments
    if (argc < 3)  {
        opserr << "WARNING insufficient number of experimental setup arguments\n";
        opserr << "Want: expSetup type tag <specific experimental setup args>\n";
        return TCL_ERROR;
    }
    
    // reset the input args
    OPS_ResetInputNoBuilder(clientData, interp, 2, argc, argv, theDomain);
    
    // pointers to setup and control that will be added
    ExperimentalSetup *theSetup = 0;
    ExperimentalControl *theControl = 0;
    
    int ndm = OPS_GetNDM();
    
    // ----------------------------------------------------------------------------	
    if (strcmp(argv[1],"NoTransformation") == 0)  {
        void* theES = OPF_ESNoTransformation();
        if (theES != 0)
            theSetup = (ExperimentalSetup*)theES;
        else
            return TCL_ERROR;
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"OneActuator") == 0)  {
        void* theES = OPF_ESOneActuator();
        if (theES != 0)
            theSetup = (ExperimentalSetup*)theES;
        else
            return TCL_ERROR;
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"TwoActuators2d") == 0)  {
        if (ndm == 2)  {
            void* theES = OPF_ESTwoActuators2d();
            if (theES != 0)
                theSetup = (ExperimentalSetup*)theES;
            else
                return TCL_ERROR;
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
            void* theES = OPF_ESThreeActuators2d();
            if (theES != 0)
                theSetup = (ExperimentalSetup*)theES;
            else
                return TCL_ERROR;
        }
        else if (ndm == 3)  {
            // not implemented yet
            opserr << "WARNING expSetup ThreeActuators2d command not implemented yet for ndm = 3\n";
            return TCL_ERROR;
        }
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1], "ThreeActuators") == 0) {
        void* theES = OPF_ESThreeActuators();
        if (theES != 0)
            theSetup = (ExperimentalSetup*)theES;
        else
            return TCL_ERROR;
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"ThreeActuatorsJntOff2d") == 0)  {
        if (ndm == 2)  {
            void* theES = OPF_ESThreeActuatorsJntOff2d();
            if (theES != 0)
                theSetup = (ExperimentalSetup*)theES;
            else
                return TCL_ERROR;
        }
        else if (ndm == 3)  {
            // not implemented yet
            opserr << "WARNING expSetup ThreeActuatorsJntOff2d command not implemented yet for ndm = 3\n";
            return TCL_ERROR;
        }
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1], "ThreeActuatorsJntOff") == 0) {
        void* theES = OPF_ESThreeActuatorsJntOff();
        if (theES != 0)
            theSetup = (ExperimentalSetup*)theES;
        else
            return TCL_ERROR;
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"InvertedVBrace") == 0)  {
        if (ndm == 2)  {
            void* theES = OPF_ESInvertedVBrace2d();
            if (theES != 0)
                theSetup = (ExperimentalSetup*)theES;
            else
                return TCL_ERROR;
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
            void* theES = OPF_ESInvertedVBraceJntOff2d();
            if (theES != 0)
                theSetup = (ExperimentalSetup*)theES;
            else
                return TCL_ERROR;
        }
        else if (ndm == 3)  {
            // not implemented yet
            opserr << "WARNING expSetup InvertedVBraceJntOff command not implemented yet for ndm = 3\n";
            return TCL_ERROR;
        }
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"Aggregator") == 0)  {
        void* theES = OPF_ESAggregator();
        if (theES != 0)
            theSetup = (ExperimentalSetup*)theES;
        else
            return TCL_ERROR;
    }
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"FourActuators") == 0)  {
        if (ndm == 3)  {
            void* theES = OPF_ESFourActuators3d();
            if (theES != 0)
                theSetup = (ExperimentalSetup*)theES;
            else
                return TCL_ERROR;
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
    for (i = 3; i < argc; i++)  {
        if (i+1 < argc && (strcmp(argv[i], "-trialDspFact") == 0 ||
            strcmp(argv[i], "-trialDispFact") == 0 ||
            strcmp(argv[i], "-trialDisplacementFact") == 0
            ))  {
            if (size <= 0)  {
                opserr << "WARNING trialDispSize <= 0\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
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
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return TCL_ERROR;
            }
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid trialDispFact\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setTrialDispFactor(factor);
        }
    }
    size = theSetup->getTrialSize(OF_Resp_Vel);
    for (i = 3; i < argc; i++)  {
        if (i+1 < argc && (strcmp(argv[i], "-trialVelFact") == 0 ||
            strcmp(argv[i], "-trialVelocityFact") == 0))  {
            if (size <= 0)  {
                opserr << "WARNING trialVelSize <= 0\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
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
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return TCL_ERROR;
            }
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid trialVelFact\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setTrialVelFactor(factor);
        }
    }
    size = theSetup->getTrialSize(OF_Resp_Accel);
    for (i = 3; i < argc; i++)  {
        if (i+1 < argc && (strcmp(argv[i], "-trialAccFact") == 0 ||
            strcmp(argv[i], "-trialAccelFact") == 0 ||
            strcmp(argv[i], "-trialAccelerationFact") == 0))  {
            if (size <= 0)  {
                opserr << "WARNING trialAccelSize <= 0\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
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
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return TCL_ERROR;
            }
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid trialAccelFact\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setTrialAccelFactor(factor);
        }
    }
    size = theSetup->getTrialSize(OF_Resp_Force);
    for (i = 3; i < argc; i++)  {
        if (i+1 < argc && (strcmp(argv[i], "-trialFrcFact") == 0 ||
            strcmp(argv[i], "-trialForcFact") == 0 ||
            strcmp(argv[i], "-trialForceFact") == 0))  {
            if (size <= 0)  {
                opserr << "WARNING trialForceSize <= 0\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
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
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return TCL_ERROR;
            }
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid trialForceFact\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setTrialForceFactor(factor);
        }
    }
    size = theSetup->getTrialSize(OF_Resp_Time);
    for (i = 3; i < argc; i++)  {
        if (i+1 < argc && strcmp(argv[i], "-trialTimeFact") == 0)  {
            if (size <= 0)  {
                opserr << "WARNING trialTimeSize <= 0\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
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
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return TCL_ERROR;
            }
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid trialTimeFact\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setTrialTimeFactor(factor);
        }
    }
    size = theSetup->getOutSize(OF_Resp_Disp);
    for (i = 3; i < argc; i++)  {
        if (i+1 < argc && (strcmp(argv[i], "-outDspFact") == 0 ||
            strcmp(argv[i], "-outDispFact") == 0 ||
            strcmp(argv[i], "-outDisplacementFact") == 0))  {
            if (size <= 0)  {
                opserr << "WARNING outDispSize <= 0\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
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
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return TCL_ERROR;
            }
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid outDispFact\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setOutDispFactor(factor);
        }
    }
    size = theSetup->getOutSize(OF_Resp_Vel);
    for (i = 3; i < argc; i++)  {
        if (i+1 < argc && (strcmp(argv[i], "-outVelFact") == 0 ||
            strcmp(argv[i], "-outVelocityFact") == 0))  {
            if (size <= 0)  {
                opserr << "WARNING outVelSize <= 0\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
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
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return TCL_ERROR;
            }
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid outVelFact\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setOutVelFactor(factor);
        }
    }
    size = theSetup->getOutSize(OF_Resp_Accel);
    for (i = 3; i < argc; i++)  {
        if (i+1 < argc && (strcmp(argv[i], "-outAccFact") == 0 ||
            strcmp(argv[i], "-outAccelFact") == 0 ||
            strcmp(argv[i], "-outAccelerationFact") == 0))  {
            if (size <= 0)  {
                opserr << "WARNING outAccelSize <= 0\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
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
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return TCL_ERROR;
            }
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid outAccelFact\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setOutAccelFactor(factor);
        }
    }
    size = theSetup->getOutSize(OF_Resp_Force);
    for (i = 3; i < argc; i++)  {
        if (i+1 < argc && (strcmp(argv[i], "-outFrcFact") == 0 ||
            strcmp(argv[i], "-outForcFact") == 0 ||
            strcmp(argv[i], "-outForceFact") == 0))  {
            if (size <= 0)  {
                opserr << "WARNING outForceSize <= 0\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
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
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return TCL_ERROR;
            }
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid outForceFact\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setOutForceFactor(factor);
        }
    }
    size = theSetup->getOutSize(OF_Resp_Time);
    for (i = 3; i < argc; i++)  {
        if (i+1 < argc && strcmp(argv[i], "-outTimeFact") == 0)  {
            if (size <= 0)  {
                opserr << "WARNING outTimeSize <= 0\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
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
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return TCL_ERROR;
            }
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid outTimeFact\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setOutTimeFactor(factor);
        }
    }
    size = theSetup->getCtrlSize(OF_Resp_Disp);
    for (i = 3; i < argc; i++)  {
        if (i+1 < argc && (strcmp(argv[i], "-ctrlDspFact") == 0 ||
            strcmp(argv[i], "-ctrlDispFact") == 0 ||
            strcmp(argv[i], "-ctrlDisplacementFact") == 0))  {
            if (size <= 0)  {
                opserr << "WARNING ctrlDispSize <= 0\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
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
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return TCL_ERROR;
            }
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid ctrlDispFact\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setCtrlDispFactor(factor);
        }
    }
    size = theSetup->getCtrlSize(OF_Resp_Vel);
    for (i = 3; i < argc; i++)  {
        if (i+1 < argc && (strcmp(argv[i], "-ctrlVelFact") == 0 ||
            strcmp(argv[i], "-ctrlVelocityFact") == 0))  {
            if (size <= 0)  {
                opserr << "WARNING ctrlVelSize <= 0\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
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
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return TCL_ERROR;
            }
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid ctrlVelFact\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setCtrlVelFactor(factor);
        }
    }
    size = theSetup->getCtrlSize(OF_Resp_Accel);
    for (i = 3; i < argc; i++)  {
        if (i+1 < argc && (strcmp(argv[i], "-ctrlAccFact") == 0 ||
            strcmp(argv[i], "-ctrlAccelFact") == 0 ||
            strcmp(argv[i], "-ctrlAccelerationFact") == 0))  {
            if (size <= 0)  {
                opserr << "WARNING ctrlAccelSize <= 0\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
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
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return TCL_ERROR;
            }
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid ctrlAccelFact\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setCtrlAccelFactor(factor);
        }
    }
    size = theSetup->getCtrlSize(OF_Resp_Force);
    for (i = 3; i < argc; i++)  {
        if (i+1 < argc && (strcmp(argv[i], "-ctrlFrcFact") == 0 ||
            strcmp(argv[i], "-ctrlForcFact") == 0 ||
            strcmp(argv[i], "-ctrlForceFact") == 0))  {
            if (size <= 0)  {
                opserr << "WARNING ctrlForceSize <= 0\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
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
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return TCL_ERROR;
            }
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid ctrlForceFact\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setCtrlForceFactor(factor);
        }
    }
    size = theSetup->getCtrlSize(OF_Resp_Time);
    for (i = 3; i < argc; i++)  {
        if (i+1 < argc && strcmp(argv[i], "-ctrlTimeFact") == 0)  {
            if (size <= 0)  {
                opserr << "WARNING ctrlTimeSize <= 0\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
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
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return TCL_ERROR;
            }
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid ctrlTimeFact\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setCtrlTimeFactor(factor);
        }
    }
    size = theSetup->getDaqSize(OF_Resp_Disp);
    for (i = 3; i < argc; i++)  {
        if (i+1 < argc && (strcmp(argv[i], "-daqDspFact") == 0 ||
            strcmp(argv[i], "-daqDispFact") == 0 ||
            strcmp(argv[i], "-daqDisplacementFact") == 0))  {
            if (size <= 0)  {
                opserr << "WARNING daqDispSize <= 0\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
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
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return TCL_ERROR;
            }
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid daqDispFact\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setDaqDispFactor(factor);
        }
    }
    size = theSetup->getDaqSize(OF_Resp_Vel);
    for (i = 3; i < argc; i++)  {
        if (i+1 < argc && (strcmp(argv[i], "-daqVelFact") == 0 ||
            strcmp(argv[i], "-daqVelocityFact") == 0))  {
            if (size <= 0)  {
                opserr << "WARNING daqVelSize <= 0\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
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
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return TCL_ERROR;
            }
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid daqVelFact\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setDaqVelFactor(factor);
        }
    }
    size = theSetup->getDaqSize(OF_Resp_Accel);
    for (i = 3; i < argc; i++)  {
        if (i+1 < argc && (strcmp(argv[i], "-daqAccFact") == 0 ||
            strcmp(argv[i], "-daqAccelFact") == 0 ||
            strcmp(argv[i], "-daqAccelerationFact") == 0))  {
            if (size <= 0)  {
                opserr << "WARNING daqAccelSize <= 0\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
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
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return TCL_ERROR;
            }
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid daqAccelFact\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setDaqAccelFactor(factor);
        }
    }
    size = theSetup->getDaqSize(OF_Resp_Force);
    for (i = 3; i < argc; i++)  {
        if (i+1 < argc && (strcmp(argv[i], "-daqFrcFact") == 0 ||
            strcmp(argv[i], "-daqForcFact") == 0 ||
            strcmp(argv[i], "-daqForceFact") == 0))  {
            if (size <= 0)  {
                opserr << "WARNING daqForceSize <= 0\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
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
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return TCL_ERROR;
            }
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid daqForceFact\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setDaqForceFactor(factor);
        }
    }
    size = theSetup->getDaqSize(OF_Resp_Time);
    for (i = 3; i < argc; i++)  {
        if (i+1 < argc && strcmp(argv[i], "-daqTimeFact") == 0)  {
            if (size <= 0)  {
                opserr << "WARNING daqTimeSize <= 0\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
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
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return TCL_ERROR;
            }
            Vector factor(size);
            double f;
            for (int j=0; j<size; j++)  {
                if (Tcl_GetDouble(interp, argv[i+1+j], &f) != TCL_OK)  {
                    opserr << "WARNING invalid daqTimeFact\n";
                    opserr << "expSetup: " << theSetup->getTag() << endln;
                    return TCL_ERROR;
                }
                factor(j) = f;
            }
            theSetup->setDaqTimeFactor(factor);
        }
    }
    
    // now add the setup to the modelBuilder
    if (OPF_AddExperimentalSetup(theSetup) == false)  {
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
        if (OPF_RemoveExperimentalSetup(tag) == false) {
            opserr << "WARNING could not remove expSetup with tag " << tag << endln;
            return TCL_ERROR;
        }
        //ExperimentalSetup *theSetup = removeExperimentalSetup(tag);
        //if (theSetup != 0)  {
        //    delete theSetup;
        //    theSetup = 0;
        //} else  {
        //    opserr << "WARNING could not remove expSetup with tag " << argv[2] << endln;
        //    return TCL_ERROR;
        //}
    }
    else if (strcmp(argv[1], "setups") == 0)  {
        OPF_ClearExperimentalSetups();
    }
    
    return TCL_OK;
}
