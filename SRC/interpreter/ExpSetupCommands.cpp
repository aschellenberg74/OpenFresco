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
// Created: 01/21
// Revision: A
//
// Description: This file contains the function invoked when the user
// invokes the expControl command in the interpreter. 

#include <ExperimentalSetup.h>
#include <elementAPI.h>
#include <map>

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


namespace {
    struct char_cmp {
        bool operator () (const char *a, const char *b) const
        {
            return strcmp(a, b) < 0;
        }
    };
    
    typedef std::map<const char *, void *(*)(void), char_cmp> OPF_ParsingFunctionMap;
    
    static OPF_ParsingFunctionMap expSetupMap;
    
    static int setUpExpSetup(void)
    {
        expSetupMap.insert(std::make_pair("NoTransformation", &OPF_ESNoTransformation));
        expSetupMap.insert(std::make_pair("OneActuator", &OPF_ESOneActuator));
        expSetupMap.insert(std::make_pair("TwoActuators2d", &OPF_ESTwoActuators2d));
        expSetupMap.insert(std::make_pair("ThreeActuators2d", &OPF_ESThreeActuators2d));
        expSetupMap.insert(std::make_pair("ThreeActuators", &OPF_ESThreeActuators));
        expSetupMap.insert(std::make_pair("ThreeActuatorsJntOff2d", &OPF_ESThreeActuatorsJntOff2d));
        expSetupMap.insert(std::make_pair("ThreeActuatorsJntOff", &OPF_ESThreeActuatorsJntOff));
        expSetupMap.insert(std::make_pair("FourActuators3d", &OPF_ESFourActuators3d));
        expSetupMap.insert(std::make_pair("InvertedVBrace2d", &OPF_ESInvertedVBrace2d));
        expSetupMap.insert(std::make_pair("InvertedVBraceJntOff2d", &OPF_ESInvertedVBraceJntOff2d));
        expSetupMap.insert(std::make_pair("Aggregator", &OPF_ESAggregator));
        
        return 0;
    }
}


int OPF_ExperimentalSetup()
{
    static bool initDone = false;
    if (initDone == false) {
        setUpExpSetup();
        initDone = true;
    }
    
    // make sure there is a minimum number of arguments
    if (OPS_GetNumRemainingInputArgs() < 2) {
        opserr << "WARNING insufficient number of experimental setup arguments\n";
        opserr << "Want: expSetup type tag <specific experimental setup args>\n";
        return -1;
    }
    
    const char* type = OPS_GetString();
    
    OPF_ParsingFunctionMap::const_iterator iter = expSetupMap.find(type);
    if (iter == expSetupMap.end()) {
        opserr << "WARNING unknown experimental setup type: "
            << type << ": check the manual\n";
        return -1;
    }
    
    ExperimentalSetup* theSetup = (ExperimentalSetup*)(*iter->second)();
    if (theSetup == 0) {
        opserr << "WARNING could not create experimental setup " << type << endln;
        return -1;
    }
    
    // finally check for the factors
    double* f = 0;
    while (OPS_GetNumRemainingInputArgs() > 0) {
        type = OPS_GetString();
        if (strcmp(type, "-trialDspFact") == 0 ||
            strcmp(type, "-trialDispFact") == 0 ||
            strcmp(type, "-trialDisplacementFact") == 0) {
            int size = theSetup->getTrialSize(OF_Resp_Disp);
            if (size <= 0) {
                opserr << "WARNING trialDispSize <= 0\n";
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return -1;
            }
            if (f != 0) delete[] f;
            f = new double[size];
            if (OPS_GetDoubleInput(&size, f) < 0) {
				opserr << "WARNING invalid trialDispFact: want ";
                opserr << size << " factors\n";
				opserr << "expSetup: " << theSetup->getTag() << endln;
				return -1;
			}
            Vector factor(f, size);
            theSetup->setTrialDispFactor(factor);
        }
        else if (strcmp(type, "-trialVelFact") == 0 ||
            strcmp(type, "-trialVelocityFact") == 0) {
            int size = theSetup->getTrialSize(OF_Resp_Vel);
            if (size <= 0) {
                opserr << "WARNING trialVelSize <= 0\n";
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return -1;
            }
            if (f != 0) delete[] f;
            f = new double[size];
            if (OPS_GetDoubleInput(&size, f) < 0) {
                opserr << "WARNING invalid trialVelFact: want ";
                opserr << size << " factors\n";
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return -1;
            }
            Vector factor(f, size);
            theSetup->setTrialVelFactor(factor);
        }
        else if (strcmp(type, "-trialAccFact") == 0 ||
            strcmp(type, "-trialAccelFact") == 0 ||
            strcmp(type, "-trialAccelerationFact") == 0) {
            int size = theSetup->getTrialSize(OF_Resp_Accel);
            if (size <= 0) {
                opserr << "WARNING trialAccelSize <= 0\n";
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return -1;
            }
            if (f != 0) delete[] f;
            f = new double[size];
            if (OPS_GetDoubleInput(&size, f) < 0) {
                opserr << "WARNING invalid trialAccelFact: want ";
                opserr << size << " factors\n";
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return -1;
            }
            Vector factor(f, size);
            theSetup->setTrialAccelFactor(factor);
        }
        else if (strcmp(type, "-trialFrcFact") == 0 ||
            strcmp(type, "-trialForcFact") == 0 ||
            strcmp(type, "-trialForceFact") == 0) {
            int size = theSetup->getTrialSize(OF_Resp_Force);
            if (size <= 0) {
                opserr << "WARNING trialForceSize <= 0\n";
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return -1;
            }
            if (f != 0) delete[] f;
            f = new double[size];
            if (OPS_GetDoubleInput(&size, f) < 0) {
                opserr << "WARNING invalid trialForceFact: want ";
                opserr << size << " factors\n";
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return -1;
            }
            Vector factor(f, size);
            theSetup->setTrialForceFactor(factor);
        }
        else if (strcmp(type, "-trialTimeFact") == 0) {
            int size = theSetup->getTrialSize(OF_Resp_Time);
            if (size <= 0) {
                opserr << "WARNING trialTimeSize <= 0\n";
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return -1;
            }
            if (f != 0) delete[] f;
            f = new double[size];
            if (OPS_GetDoubleInput(&size, f) < 0) {
                opserr << "WARNING invalid trialTimeFact: want ";
                opserr << size << " factors\n";
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return -1;
            }
            Vector factor(f, size);
            theSetup->setTrialTimeFactor(factor);
        }
        if (strcmp(type, "-outDspFact") == 0 ||
            strcmp(type, "-outDispFact") == 0 ||
            strcmp(type, "-outDisplacementFact") == 0) {
            int size = theSetup->getOutSize(OF_Resp_Disp);
            if (size <= 0) {
                opserr << "WARNING outDispSize <= 0\n";
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return -1;
            }
            if (f != 0) delete[] f;
            f = new double[size];
            if (OPS_GetDoubleInput(&size, f) < 0) {
                opserr << "WARNING invalid outDispFact: want ";
                opserr << size << " factors\n";
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return -1;
            }
            Vector factor(f, size);
            theSetup->setOutDispFactor(factor);
        }
        else if (strcmp(type, "-outVelFact") == 0 ||
            strcmp(type, "-outVelocityFact") == 0) {
            int size = theSetup->getOutSize(OF_Resp_Vel);
            if (size <= 0) {
                opserr << "WARNING outVelSize <= 0\n";
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return -1;
            }
            if (f != 0) delete[] f;
            f = new double[size];
            if (OPS_GetDoubleInput(&size, f) < 0) {
                opserr << "WARNING invalid outVelFact: want ";
                opserr << size << " factors\n";
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return -1;
            }
            Vector factor(f, size);
            theSetup->setOutVelFactor(factor);
        }
        else if (strcmp(type, "-outAccFact") == 0 ||
            strcmp(type, "-outAccelFact") == 0 ||
            strcmp(type, "-outAccelerationFact") == 0) {
            int size = theSetup->getOutSize(OF_Resp_Accel);
            if (size <= 0) {
                opserr << "WARNING outAccelSize <= 0\n";
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return -1;
            }
            if (f != 0) delete[] f;
            f = new double[size];
            if (OPS_GetDoubleInput(&size, f) < 0) {
                opserr << "WARNING invalid outAccelFact: want ";
                opserr << size << " factors\n";
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return -1;
            }
            Vector factor(f, size);
            theSetup->setOutAccelFactor(factor);
        }
        else if (strcmp(type, "-outFrcFact") == 0 ||
            strcmp(type, "-outForcFact") == 0 ||
            strcmp(type, "-outForceFact") == 0) {
            int size = theSetup->getOutSize(OF_Resp_Force);
            if (size <= 0) {
                opserr << "WARNING outForceSize <= 0\n";
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return -1;
            }
            if (f != 0) delete[] f;
            f = new double[size];
            if (OPS_GetDoubleInput(&size, f) < 0) {
                opserr << "WARNING invalid outForceFact: want ";
                opserr << size << " factors\n";
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return -1;
            }
            Vector factor(f, size);
            theSetup->setOutForceFactor(factor);
        }
        else if (strcmp(type, "-outTimeFact") == 0) {
            int size = theSetup->getOutSize(OF_Resp_Time);
            if (size <= 0) {
                opserr << "WARNING outTimeSize <= 0\n";
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return -1;
            }
            if (f != 0) delete[] f;
            f = new double[size];
            if (OPS_GetDoubleInput(&size, f) < 0) {
                opserr << "WARNING invalid outTimeFact: want ";
                opserr << size << " factors\n";
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return -1;
            }
            Vector factor(f, size);
            theSetup->setOutTimeFactor(factor);
        }
        if (strcmp(type, "-ctrlDspFact") == 0 ||
            strcmp(type, "-ctrlDispFact") == 0 ||
            strcmp(type, "-ctrlDisplacementFact") == 0) {
            int size = theSetup->getCtrlSize(OF_Resp_Disp);
            if (size <= 0) {
                opserr << "WARNING ctrlDispSize <= 0\n";
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return -1;
            }
            if (f != 0) delete[] f;
            f = new double[size];
            if (OPS_GetDoubleInput(&size, f) < 0) {
                opserr << "WARNING invalid ctrlDispFact: want ";
                opserr << size << " factors\n";
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return -1;
            }
            Vector factor(f, size);
            theSetup->setCtrlDispFactor(factor);
        }
        else if (strcmp(type, "-ctrlVelFact") == 0 ||
            strcmp(type, "-ctrlVelocityFact") == 0) {
            int size = theSetup->getCtrlSize(OF_Resp_Vel);
            if (size <= 0) {
                opserr << "WARNING ctrlVelSize <= 0\n";
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return -1;
            }
            if (f != 0) delete[] f;
            f = new double[size];
            if (OPS_GetDoubleInput(&size, f) < 0) {
                opserr << "WARNING invalid ctrlVelFact: want ";
                opserr << size << " factors\n";
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return -1;
            }
            Vector factor(f, size);
            theSetup->setCtrlVelFactor(factor);
        }
        else if (strcmp(type, "-ctrlAccFact") == 0 ||
            strcmp(type, "-ctrlAccelFact") == 0 ||
            strcmp(type, "-ctrlAccelerationFact") == 0) {
            int size = theSetup->getCtrlSize(OF_Resp_Accel);
            if (size <= 0) {
                opserr << "WARNING ctrlAccelSize <= 0\n";
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return -1;
            }
            if (f != 0) delete[] f;
            f = new double[size];
            if (OPS_GetDoubleInput(&size, f) < 0) {
                opserr << "WARNING invalid ctrlAccelFact: want ";
                opserr << size << " factors\n";
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return -1;
            }
            Vector factor(f, size);
            theSetup->setCtrlAccelFactor(factor);
        }
        else if (strcmp(type, "-ctrlFrcFact") == 0 ||
            strcmp(type, "-ctrlForcFact") == 0 ||
            strcmp(type, "-ctrlForceFact") == 0) {
            int size = theSetup->getCtrlSize(OF_Resp_Force);
            if (size <= 0) {
                opserr << "WARNING ctrlForceSize <= 0\n";
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return -1;
            }
            if (f != 0) delete[] f;
            f = new double[size];
            if (OPS_GetDoubleInput(&size, f) < 0) {
                opserr << "WARNING invalid ctrlForceFact: want ";
                opserr << size << " factors\n";
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return -1;
            }
            Vector factor(f, size);
            theSetup->setCtrlForceFactor(factor);
        }
        else if (strcmp(type, "-ctrlTimeFact") == 0) {
            int size = theSetup->getCtrlSize(OF_Resp_Time);
            if (size <= 0) {
                opserr << "WARNING ctrlTimeSize <= 0\n";
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return -1;
            }
            if (f != 0) delete[] f;
            f = new double[size];
            if (OPS_GetDoubleInput(&size, f) < 0) {
                opserr << "WARNING invalid ctrlTimeFact: want ";
                opserr << size << " factors\n";
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return -1;
            }
            Vector factor(f, size);
            theSetup->setCtrlTimeFactor(factor);
        }
        if (strcmp(type, "-daqDspFact") == 0 ||
            strcmp(type, "-daqDispFact") == 0 ||
            strcmp(type, "-daqDisplacementFact") == 0) {
            int size = theSetup->getDaqSize(OF_Resp_Disp);
            if (size <= 0) {
                opserr << "WARNING daqDispSize <= 0\n";
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return -1;
            }
            if (f != 0) delete[] f;
            f = new double[size];
            if (OPS_GetDoubleInput(&size, f) < 0) {
                opserr << "WARNING invalid daqDispFact: want ";
                opserr << size << " factors\n";
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return -1;
            }
            Vector factor(f, size);
            theSetup->setDaqDispFactor(factor);
        }
        else if (strcmp(type, "-daqVelFact") == 0 ||
            strcmp(type, "-daqVelocityFact") == 0) {
            int size = theSetup->getDaqSize(OF_Resp_Vel);
            if (size <= 0) {
                opserr << "WARNING daqVelSize <= 0\n";
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return -1;
            }
            if (f != 0) delete[] f;
            f = new double[size];
            if (OPS_GetDoubleInput(&size, f) < 0) {
                opserr << "WARNING invalid daqVelFact: want ";
                opserr << size << " factors\n";
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return -1;
            }
            Vector factor(f, size);
            theSetup->setDaqVelFactor(factor);
        }
        else if (strcmp(type, "-daqAccFact") == 0 ||
            strcmp(type, "-daqAccelFact") == 0 ||
            strcmp(type, "-daqAccelerationFact") == 0) {
            int size = theSetup->getDaqSize(OF_Resp_Accel);
            if (size <= 0) {
                opserr << "WARNING daqAccelSize <= 0\n";
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return -1;
            }
            if (f != 0) delete[] f;
            f = new double[size];
            if (OPS_GetDoubleInput(&size, f) < 0) {
                opserr << "WARNING invalid daqAccelFact: want ";
                opserr << size << " factors\n";
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return -1;
            }
            Vector factor(f, size);
            theSetup->setDaqAccelFactor(factor);
        }
        else if (strcmp(type, "-daqFrcFact") == 0 ||
            strcmp(type, "-daqForcFact") == 0 ||
            strcmp(type, "-daqForceFact") == 0) {
            int size = theSetup->getDaqSize(OF_Resp_Force);
            if (size <= 0) {
                opserr << "WARNING daqForceSize <= 0\n";
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return -1;
            }
            if (f != 0) delete[] f;
            f = new double[size];
            if (OPS_GetDoubleInput(&size, f) < 0) {
                opserr << "WARNING invalid daqForceFact: want ";
                opserr << size << " factors\n";
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return -1;
            }
            Vector factor(f, size);
            theSetup->setDaqForceFactor(factor);
        }
        else if (strcmp(type, "-daqTimeFact") == 0) {
            int size = theSetup->getDaqSize(OF_Resp_Time);
            if (size <= 0) {
                opserr << "WARNING daqTimeSize <= 0\n";
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return -1;
            }
            if (f != 0) delete[] f;
            f = new double[size];
            if (OPS_GetDoubleInput(&size, f) < 0) {
                opserr << "WARNING invalid daqTimeFact: want ";
                opserr << size << " factors\n";
                opserr << "expSetup: " << theSetup->getTag() << endln;
                return -1;
            }
            Vector factor(f, size);
            theSetup->setDaqTimeFactor(factor);
        }
    }
    // delete dynamic memory
    if (f != 0) delete[] f;
    
    // now add the setup to the modelBuilder
    if (OPF_addExperimentalSetup(theSetup) == false) {
        opserr << "WARNING could not add experimental setup.\n";
        delete theSetup;
        return -1;
    }
    
    return 0;
}
