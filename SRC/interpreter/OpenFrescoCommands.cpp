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
// Description: all OpenFresco APIs are defined or declared here

#include "OpenFrescoCommands.h"
#include <OPS_Globals.h>
#include <elementAPI.h>
#include <Domain.h>
#include <UniaxialMaterial.h>
#include <NDMaterial.h>
#include <SectionForceDeformation.h>
#include <FrictionModel.h>
#include <CrdTransf.h>
#include <Element.h>
//#include <FileDatastore.h>
#include <FrescoGlobals.h>
#include <ExperimentalSite.h>


// active object
static OpenFrescoCommands* cmds = 0;


OpenFrescoCommands::OpenFrescoCommands(DL_Interpreter* interp)
    :interpreter(interp), theDomain(0), ndf(0), ndm(0),
    theTimer(), theSimulationInfo()
{
    cmds = this;

    theDomain = new Domain;
}


OpenFrescoCommands::~OpenFrescoCommands()
{
    if (theDomain != 0) delete theDomain;
    //if (theDatabase != 0) delete theDatabase;
    cmds = 0;
}


DL_Interpreter* OpenFrescoCommands::getInterpreter()
{
    return interpreter;
}


Domain* OpenFrescoCommands::getDomain()
{
    return theDomain;
}


void OpenFrescoCommands::wipeExp()
{
    OPF_clearExperimentalCPs();
    OPF_clearExperimentalSignalFilters();
    OPF_clearExperimentalControls();
    OPF_clearExperimentalSetups();
    OPF_clearExperimentalSites();
    OPF_clearExperimentalTangentStiffs();

    OPS_clearAllUniaxialMaterial();
    OPS_clearAllNDMaterial();
    OPS_clearAllSectionForceDeformation();
    OPS_clearAllFrictionModel();

    //if (theDatabase != 0) {
    //    delete theDatabase;
    //    theDatabase = 0;
    //}

    if (theDomain != 0)
        theDomain->clearAll();

    ops_Dt = 0.0;
}


/*void OpenFrescoCommands::setFileDatabase(const char* filename)
{
    if (theDatabase != 0) delete theDatabase;
    theDatabase = new FileDatastore(filename, *theDomain, theBroker);
    if (theDatabase == 0) {
        opserr << "WARNING ran out of memory - database File " << filename << endln;
    }
}*/


///////////////////////////
//// OpenSees APIs  ///////
///////////////////////////
int OPS_GetNumRemainingInputArgs()
{
    if (cmds == 0) return 0;
    DL_Interpreter* interp = cmds->getInterpreter();
    return interp->getNumRemainingInputArgs();
}


int OPS_GetIntInput(int* numData, int* data)
{
    if (cmds == 0) return 0;
    DL_Interpreter* interp = cmds->getInterpreter();
    if (numData == 0 || data == 0) return -1;
    return interp->getInt(data, *numData);
}


int OPS_SetIntOutput(int* numData, int* data, bool scalar)
{
    if (cmds == 0) return 0;
    DL_Interpreter* interp = cmds->getInterpreter();
    return interp->setInt(data, *numData, scalar);
}


int OPS_GetDoubleInput(int* numData, double* data)
{
    if (cmds == 0) return 0;
    DL_Interpreter* interp = cmds->getInterpreter();
    if (numData == 0 || data == 0) return -1;
    return interp->getDouble(data, *numData);
}


int OPS_SetDoubleOutput(int* numData, double* data, bool scalar)
{
    if (cmds == 0) return 0;
    DL_Interpreter* interp = cmds->getInterpreter();
    return interp->setDouble(data, *numData, scalar);
}


const char* OPS_GetString()
{
    const char* res = 0;
    if (cmds == 0) return res;
    DL_Interpreter* interp = cmds->getInterpreter();
    return interp->getString();
}


int OPS_SetString(const char* str)
{
    if (cmds == 0) return 0;
    DL_Interpreter* interp = cmds->getInterpreter();
    return interp->setString(str);
}


Domain* OPS_GetDomain()
{
    if (cmds == 0) return 0;
    return cmds->getDomain();
}


int OPS_GetNDF()
{
    if (cmds == 0) return 0;
    return cmds->getNDF();
}


int OPS_GetNDM()
{
    if (cmds == 0) return 0;
    return cmds->getNDM();
}


int OPS_Error(char* errorMessage, int length)
{
    opserr << errorMessage;
    opserr << endln;
    return 0;
}


int OPS_ResetCurrentInputArg(int cArg)
{
    if (cArg == 0) {
        opserr << "WARNING can't reset to argv[0]\n";
        return -1;
    }
    if (cmds == 0) return 0;
    DL_Interpreter* interp = cmds->getInterpreter();
    interp->resetInput(cArg);
    return 0;
}


int OPS_model()
{
    // num args
    if (OPS_GetNumRemainingInputArgs() < 3) {
        opserr << "WARNING insufficient args: model -ndm ndm <-ndf ndf>\n";
        return -1;
    }

    // model type
    const char* modeltype = OPS_GetString();
    if (strcmp(modeltype, "basic") != 0 && strcmp(modeltype, "Basic") != 0 &&
        strcmp(modeltype, "basicBuilder") != 0 && strcmp(modeltype, "BasicBuilder") != 0) {
        opserr << "WARNING only basic builder is available at this time\n";
        return -1;
    }

    // ndm
    const char* ndmopt = OPS_GetString();
    if (strcmp(ndmopt, "-ndm") != 0 && strcmp(ndmopt, "-NDM") != 0) {
        opserr << "WARNING frist option must be -ndm\n";
        return -1;
    }
    int numdata = 1;
    int ndm = 0;
    if (OPS_GetIntInput(&numdata, &ndm) < 0) {
        opserr << "WARNING failed to read ndm\n";
        return -1;
    }
    if (ndm != 1 && ndm != 2 && ndm != 3) {
        opserr << "ERROR ndm msut be 1, 2 or 3\n";
        return -1;
    }

    // ndf
    int ndf = 0;
    if (OPS_GetNumRemainingInputArgs() > 1) {
        const char* ndfopt = OPS_GetString();
        if (strcmp(ndfopt, "-ndf") != 0 && strcmp(ndfopt, "-NDF") != 0) {
            opserr << "WARNING second option must be -ndf\n";
            return -1;
        }
        if (OPS_GetIntInput(&numdata, &ndf) < 0) {
            opserr << "WARNING failed to read ndf\n";
            return -1;
        }
    }

    // set ndf
    if (ndf <= 0) {
        if (ndm == 1)
            ndf = 1;
        else if (ndm == 2)
            ndf = 3;
        else if (ndm == 3)
            ndf = 6;
    }

    // set ndm and ndf
    if (cmds != 0) {
        cmds->setNDF(ndf);
        cmds->setNDM(ndm);
    }

    return 0;
}


int OPS_logFile()
{
    if (OPS_GetNumRemainingInputArgs() < 1) {
        opserr << "WARNING logFile fileName? - no filename supplied\n";
        return -1;
    }
    openMode mode = OVERWRITE;
    bool echo = true;

    const char* filename = OPS_GetString();
    if (strcmp(filename, "Invalid String Input!") == 0) {
        opserr << "WARNING: invalid string input\n";
        return -1;
    }

    while (OPS_GetNumRemainingInputArgs() > 0) {
        const char* opt = OPS_GetString();
        if (strcmp(opt, "-append") == 0) {
            mode = APPEND;
        }
        else if (strcmp(opt, "-noEcho") == 0) {
            echo = false;
        }
    }

    if (opserr.setFile(filename, mode, echo) < 0) {
        opserr << "WARNING logFile " << filename << " failed to set the file\n";
        return -1;
    }

    return 0;
}


int OPS_metaData()
{
    if (cmds == 0) return 0;
    if (OPS_GetNumRemainingInputArgs() < 1) {
        opserr << "WARNING missing args \n";
        return -1;
    }

    SimulationInformation* simulationInfo = cmds->getSimulationInformation();
    if (simulationInfo == 0) return -1;

    while (OPS_GetNumRemainingInputArgs() > 0) {
        const char* flag = OPS_GetString();

        if ((strcmp(flag, "-title") == 0) || (strcmp(flag, "-Title") == 0)
            || (strcmp(flag, "-TITLE") == 0)) {
            if (OPS_GetNumRemainingInputArgs() > 0) {
                simulationInfo->setTitle(OPS_GetString());
            }
        }
        else if ((strcmp(flag, "-contact") == 0) || (strcmp(flag, "-Contact") == 0)
            || (strcmp(flag, "-CONTACT") == 0)) {
            if (OPS_GetNumRemainingInputArgs() > 0) {
                simulationInfo->setContact(OPS_GetString());
            }
        }
        else if ((strcmp(flag, "-description") == 0) || (strcmp(flag, "-Description") == 0)
            || (strcmp(flag, "-DESCRIPTION") == 0)) {
            if (OPS_GetNumRemainingInputArgs() > 0) {
                simulationInfo->setDescription(OPS_GetString());
            }
        }
        else if ((strcmp(flag, "-modelType") == 0) || (strcmp(flag, "-ModelType") == 0)
            || (strcmp(flag, "-MODELTYPE") == 0)) {
            if (OPS_GetNumRemainingInputArgs() > 0) {
                simulationInfo->addModelType(OPS_GetString());
            }
        }
        else if ((strcmp(flag, "-analysisType") == 0) || (strcmp(flag, "-AnalysisType") == 0)
            || (strcmp(flag, "-ANALYSISTYPE") == 0)) {
            if (OPS_GetNumRemainingInputArgs() > 0) {
                simulationInfo->addAnalysisType(OPS_GetString());
            }
        }
        else if ((strcmp(flag, "-elementType") == 0) || (strcmp(flag, "-ElementType") == 0)
            || (strcmp(flag, "-ELEMENTTYPE") == 0)) {
            if (OPS_GetNumRemainingInputArgs() > 0) {
                simulationInfo->addElementType(OPS_GetString());
            }
        }
        else if ((strcmp(flag, "-materialType") == 0) || (strcmp(flag, "-MaterialType") == 0)
            || (strcmp(flag, "-MATERIALTYPE") == 0)) {
            if (OPS_GetNumRemainingInputArgs() > 0) {
                simulationInfo->addMaterialType(OPS_GetString());
            }
        }
        else if ((strcmp(flag, "-loadingType") == 0) || (strcmp(flag, "-LoadingType") == 0)
            || (strcmp(flag, "-LOADINGTYPE") == 0)) {
            if (OPS_GetNumRemainingInputArgs() > 0) {
                simulationInfo->addLoadingType(OPS_GetString());
            }
        }
        else {
            opserr << "WARNING unknown arg type: " << flag << endln;
            return -1;
        }
    }
    return 0;
}


int OPS_defaultUnits()
{
    if (OPS_GetNumRemainingInputArgs() < 8) {
        opserr << "WARNING missing a unit type\n";
        opserr << "Want: defaultUnits -force type -length type -time type -temperature type\n";
        return -1;
    }

    const char* force = 0;
    const char* length = 0;
    const char* time = 0;
    const char* temperature = 0;

    while (OPS_GetNumRemainingInputArgs() > 0) {
        const char* unitType = OPS_GetString();

        if ((strcmp(unitType, "-force") == 0) || (strcmp(unitType, "-Force") == 0)
            || (strcmp(unitType, "-FORCE") == 0)) {
            force = OPS_GetString();
        }
        else if ((strcmp(unitType, "-length") == 0) || (strcmp(unitType, "-Length") == 0)
            || (strcmp(unitType, "-LENGTH") == 0)) {
            length = OPS_GetString();
        }
        else if ((strcmp(unitType, "-time") == 0) || (strcmp(unitType, "-Time") == 0)
            || (strcmp(unitType, "-TIME") == 0)) {
            time = OPS_GetString();
        }
        else if ((strcmp(unitType, "-temperature") == 0) || (strcmp(unitType, "-Temperature") == 0)
            || (strcmp(unitType, "-TEMPERATURE") == 0) || (strcmp(unitType, "-temp") == 0)
            || (strcmp(unitType, "-Temp") == 0) || (strcmp(unitType, "-TEMP") == 0)) {
            temperature = OPS_GetString();
        }
        else {
            opserr << "WARNING defaultUnits - unrecognized unit: " << unitType << " want: defaultUnits -Force type? -Length type? -Time type? -Temperature type?\n";
            return -1;
        }
    }

    if (length == 0 || force == 0 || time == 0 || temperature == 0) {
        opserr << "defaultUnits - missing a unit type want: defaultUnits -Force type? -Length type? -Time type? -Temperature type?\n";
        return -1;
    }

    double lb, kip, n, kn, mn, kgf, tonf;
    double in, ft, mm, cm, m;
    double sec, msec;
    double F, C;

    if ((strcmp(force, "lb") == 0) || (strcmp(force, "lbs") == 0)) {
        lb = 1.0;
    }
    else if ((strcmp(force, "kip") == 0) || (strcmp(force, "kips") == 0)) {
        lb = 0.001;
    }
    else if ((strcmp(force, "N") == 0)) {
        lb = 4.4482216152605;
    }
    else if ((strcmp(force, "kN") == 0) || (strcmp(force, "KN") == 0) || (strcmp(force, "kn") == 0)) {
        lb = 0.0044482216152605;
    }
    else if ((strcmp(force, "mN") == 0) || (strcmp(force, "MN") == 0) || (strcmp(force, "mn") == 0)) {
        lb = 0.0000044482216152605;
    }
    else if ((strcmp(force, "kgf") == 0)) {
        lb = 9.80665 * 4.4482216152605;
    }
    else if ((strcmp(force, "tonf") == 0)) {
        lb = 9.80665 / 1000.0 * 4.4482216152605;
    }
    else {
        lb = 1.0;
        opserr << "defaultUnits - unknown force type, valid options: lb, kip, N, kN, MN, kgf, tonf\n";
        return -1;
    }

    if ((strcmp(length, "in") == 0) || (strcmp(length, "inch") == 0)) {
        in = 1.0;
    }
    else if ((strcmp(length, "ft") == 0) || (strcmp(length, "feet") == 0)) {
        in = 1.0 / 12.0;
    }
    else if ((strcmp(length, "mm") == 0)) {
        in = 25.4;
    }
    else if ((strcmp(length, "cm") == 0)) {
        in = 2.54;
    }
    else if ((strcmp(length, "m") == 0)) {
        in = 0.0254;
    }
    else {
        in = 1.0;
        opserr << "defaultUnits - unknown length type, valid options: in, ft, mm, cm, m\n";
        return -1;
    }

    if ((strcmp(time, "sec") == 0) || (strcmp(time, "Sec") == 0)) {
        sec = 1.0;
    }
    else if ((strcmp(time, "msec") == 0) || (strcmp(time, "mSec") == 0)) {
        sec = 1000.0;
    }
    else {
        sec = 1.0;
        opserr << "defaultUnits - unknown time type, valid options: sec, msec\n";
        return -1;
    }

    if ((strcmp(temperature, "F") == 0) || (strcmp(temperature, "degF") == 0)) {
        F = 1.0;
    }
    else if ((strcmp(temperature, "C") == 0) || (strcmp(temperature, "degC") == 0)) {
        F = 9.0 / 5.0 + 32.0;
    }
    else {
        F = 1.0;
        opserr << "defaultUnits - unknown temperature type, valid options: F, C\n";
        return -1;
    }

    kip = lb / 0.001;
    n = lb / 4.4482216152605;
    kn = lb / 0.0044482216152605;
    mn = lb / 0.0000044482216152605;
    kgf = lb / (9.80665 * 4.4482216152605);
    tonf = lb / (9.80665 / 1000.0 * 4.4482216152605);

    ft = in * 12.0;
    mm = in / 25.44;
    cm = in / 2.54;
    m = in / 0.0254;

    msec = sec * 0.001;

    C = (F - 32.0) * 5.0 / 9.0;

    char string[50];
    DL_Interpreter* theInter = cmds->getInterpreter();
    if (theInter == 0) return -1;

    sprintf(string, "lb = %.18e", lb);   theInter->runCommand(string);
    sprintf(string, "lbf = %.18e", lb);   theInter->runCommand(string);
    sprintf(string, "kip = %.18e", kip);   theInter->runCommand(string);
    sprintf(string, "N = %.18e", n);   theInter->runCommand(string);
    sprintf(string, "kN = %.18e", kn);   theInter->runCommand(string);
    sprintf(string, "Newton = %.18e", n);   theInter->runCommand(string);
    sprintf(string, "kNewton = %.18e", kn);   theInter->runCommand(string);
    sprintf(string, "MN = %.18e", mn);   theInter->runCommand(string);
    sprintf(string, "kgf = %.18e", kgf);   theInter->runCommand(string);
    sprintf(string, "tonf = %.18e", tonf);   theInter->runCommand(string);

    //sprintf(string, "in = %.18e", in);   theInter->runCommand(string);  // "in" is a keyword in Python
    sprintf(string, "inch = %.18e", in);   theInter->runCommand(string);
    sprintf(string, "ft = %.18e", ft);   theInter->runCommand(string);
    sprintf(string, "mm = %.18e", mm);   theInter->runCommand(string);
    sprintf(string, "cm = %.18e", cm);   theInter->runCommand(string);
    sprintf(string, "m = %.18e", m);   theInter->runCommand(string);
    sprintf(string, "meter = %.18e", m);   theInter->runCommand(string);

    sprintf(string, "sec = %.18e", sec);   theInter->runCommand(string);
    sprintf(string, "msec = %.18e", msec);   theInter->runCommand(string);

    sprintf(string, "F = %.18e", F);   theInter->runCommand(string);
    sprintf(string, "degF = %.18e", F);   theInter->runCommand(string);
    sprintf(string, "C = %.18e", C);   theInter->runCommand(string);
    sprintf(string, "degC = %.18e", C);   theInter->runCommand(string);

    double g = 32.174049 * ft / (sec * sec);
    sprintf(string, "g = %.18e", g);   theInter->runCommand(string);
    sprintf(string, "kg = %.18e", n * sec * sec / m);   theInter->runCommand(string);
    sprintf(string, "Mg = %.18e", 1e3 * n * sec * sec / m);   theInter->runCommand(string);
    sprintf(string, "slug = %.18e", lb * sec * sec / ft);   theInter->runCommand(string);
    sprintf(string, "Pa = %.18e", n / (m * m));   theInter->runCommand(string);
    sprintf(string, "kPa = %.18e", 1e3 * n / (m * m));   theInter->runCommand(string);
    sprintf(string, "MPa = %.18e", 1e6 * n / (m * m));   theInter->runCommand(string);
    sprintf(string, "psi = %.18e", lb / (in * in));   theInter->runCommand(string);
    sprintf(string, "ksi = %.18e", kip / (in * in));   theInter->runCommand(string);
    sprintf(string, "psf = %.18e", lb / (ft * ft));   theInter->runCommand(string);
    sprintf(string, "ksf = %.18e", kip / (ft * ft));   theInter->runCommand(string);
    sprintf(string, "pcf = %.18e", lb / (ft * ft * ft));   theInter->runCommand(string);
    sprintf(string, "in2 = %.18e", in * in);   theInter->runCommand(string);
    sprintf(string, "ft2 = %.18e", ft * ft);   theInter->runCommand(string);
    sprintf(string, "mm2 = %.18e", mm * mm);   theInter->runCommand(string);
    sprintf(string, "cm2 = %.18e", cm * cm);   theInter->runCommand(string);
    sprintf(string, "m2 = %.18e", m * m);   theInter->runCommand(string);
    sprintf(string, "in4 = %.18e", in * in * in * in);   theInter->runCommand(string);
    sprintf(string, "ft4 = %.18e", ft * ft * ft * ft);   theInter->runCommand(string);
    sprintf(string, "mm4 = %.18e", mm * mm * mm * mm);   theInter->runCommand(string);
    sprintf(string, "cm4 = %.18e", cm * cm * cm * cm);   theInter->runCommand(string);
    sprintf(string, "m4 = %.18e", m * m * m * m);   theInter->runCommand(string);
    sprintf(string, "pi = %.18e", 2.0 * asin(1.0));   theInter->runCommand(string);
    sprintf(string, "PI = %.18e", 2.0 * asin(1.0));   theInter->runCommand(string);

    SimulationInformation* simulationInfo = cmds->getSimulationInformation();
    if (simulationInfo == 0) return -1;

    simulationInfo->setForceUnit(force);
    simulationInfo->setLengthUnit(length);
    simulationInfo->setTimeUnit(time);
    simulationInfo->setTemperatureUnit(temperature);

    return 0;
}


int OPS_startTimer()
{
    if (cmds == 0) return 0;
    Timer* timer = cmds->getTimer();
    if (timer == 0) return -1;
    timer->start();
    return 0;
}


int OPS_stopTimer()
{
    if (cmds == 0) return 0;
    Timer* theTimer = cmds->getTimer();
    if (theTimer == 0) return -1;
    theTimer->pause();
    opserr << *theTimer;
    return 0;
}


int OPS_rayleighDamping()
{
    if (OPS_GetNumRemainingInputArgs() < 4) {
        opserr << "WARNING rayleigh alphaM? betaK? betaK0? betaKc? - not enough arguments to command\n";
        return -1;
    }

    double data[4];
    int numdata = 4;
    if (OPS_GetDoubleInput(&numdata, data) < 0) {
        opserr << "WARNING rayleigh alphaM? betaK? betaK0? betaKc? - could not read ? \n";
        return -1;
    }

    Domain* theDomain = OPS_GetDomain();
    if (theDomain == 0) return -1;
    theDomain->setRayleighDampingFactors(data[0], data[1], data[2], data[3]);

    return 0;
}


int OPS_setElementRayleighDampingFactors()
{
    if (OPS_GetNumRemainingInputArgs() < 5) {
        opserr << "WARNING setElementRayleighDampingFactors eleTag? alphaM? betaK? betaK0? betaKc? - not enough arguments to command\n";
        return -1;
    }
    int eleTag;
    int numdata = 1;
    if (OPS_GetIntInput(&numdata, &eleTag) < 0) {
        opserr << "WARNING rayleigh alphaM? betaK? betaK0? betaKc? - could not read eleTag? \n";
        return -1;
    }

    numdata = 4;
    double data[4];
    if (OPS_GetDoubleInput(&numdata, data) < 0) {
        opserr << "WARNING rayleigh alphaM? betaK? betaK0? betaKc? - could not read double inputs? \n";
        return -1;
    }

    double alphaM = data[0];
    double betaK  = data[1];
    double betaK0 = data[2];
    double betaKc = data[3];

    Domain* theDomain = OPS_GetDomain();
    if (theDomain == 0) return -1;

    Element* theEle = theDomain->getElement(eleTag);
    theEle->setRayleighDampingFactors(alphaM, betaK, betaK0, betaKc);

    return 0;
}


UniaxialMaterial* OPS_GetUniaxialMaterial(int matTag)
{
    return OPS_getUniaxialMaterial(matTag);
}


NDMaterial* OPS_GetNDMaterial(int matTag)
{
    return OPS_getNDMaterial(matTag);
}


SectionForceDeformation* OPS_GetSectionForceDeformation(int secTag)
{
    return OPS_getSectionForceDeformation(secTag);
}


FrictionModel* OPS_GetFrictionModel(int frnTag)
{
    return OPS_getFrictionModel(frnTag);
}


CrdTransf* OPS_GetCrdTransf(int crdTag)
{
    return OPS_getCrdTransf(crdTag);
}


/////////////////////////////
//// OpenFresco APIs  ///////
/////////////////////////////
int OPF_version()
{
    if (OPS_SetString(OPF_VERSION) < 0) {
        opserr << "WARNING failed to set version string\n";
        return -1;
    }

    return 0;
}


int OPF_wipeExp()
{
    // wipe
    if (cmds != 0) {
        cmds->wipeExp();
    }

    return 0;
}


int OPF_removeObject()
{
    // make sure there is a minimum number of arguments
    if (OPS_GetNumRemainingInputArgs() < 2) {
        opserr << "WARNING insufficient number of removeExp component arguments\n";
        opserr << "Want: removeExp type <specific args>\n";
        return -1;
    }

    const char* type = OPS_GetString();
    if (strcmp(type, "controlPoint") == 0) {
        if (OPS_GetNumRemainingInputArgs() < 1) {
            opserr << "WARNING invalid number of arguments\n";
            opserr << "Want: removeExp controlPoint tag\n";
            return -1;
        }
        int tag;
        int numdata = 1;
        if (OPS_GetIntInput(&numdata, &tag) < 0) {
            opserr << "WARNING invalid removeExp controlPoint tag\n";
            return -1;
        }
        if (OPF_removeExperimentalCP(tag) == false) {
            opserr << "WARNING could not remove expControlPoint with tag " << tag << endln;
            return -1;
        }
    }
    else if (strcmp(type, "controlPoints") == 0) {
        OPF_clearExperimentalCPs();
    }
    else if (strcmp(type, "signalFilter") == 0) {
        if (OPS_GetNumRemainingInputArgs() < 1) {
            opserr << "WARNING invalid number of arguments\n";
            opserr << "Want: removeExp signalFilter tag\n";
            return -1;
        }
        int tag;
        int numdata = 1;
        if (OPS_GetIntInput(&numdata, &tag) < 0) {
            opserr << "WARNING invalid removeExp signalFilter tag\n";
            return -1;
        }
        if (OPF_removeExperimentalSignalFilter(tag) == false) {
            opserr << "WARNING could not remove expSignalFilter with tag " << tag << endln;
            return -1;
        }
    }
    else if (strcmp(type, "signalFilters") == 0) {
        OPF_clearExperimentalSignalFilters();
    }
    else if (strcmp(type, "control") == 0) {
        if (OPS_GetNumRemainingInputArgs() < 1) {
            opserr << "WARNING invalid number of arguments\n";
            opserr << "Want: removeExp control tag\n";
            return -1;
        }
        int tag;
        int numdata = 1;
        if (OPS_GetIntInput(&numdata, &tag) < 0) {
            opserr << "WARNING invalid removeExp control tag\n";
            return -1;
        }
        if (OPF_removeExperimentalControl(tag) == false) {
            opserr << "WARNING could not remove expControl with tag " << tag << endln;
            return -1;
        }
    }
    else if (strcmp(type, "controls") == 0) {
        OPF_clearExperimentalControls();
    }
    else if (strcmp(type, "setup") == 0) {
        if (OPS_GetNumRemainingInputArgs() < 1) {
            opserr << "WARNING invalid number of arguments\n";
            opserr << "Want: removeExp setup tag\n";
            return -1;
        }
        int tag;
        int numdata = 1;
        if (OPS_GetIntInput(&numdata, &tag) < 0) {
            opserr << "WARNING invalid removeExp setup tag\n";
            return -1;
        }
        if (OPF_removeExperimentalSetup(tag) == false) {
            opserr << "WARNING could not remove expSetup with tag " << tag << endln;
            return -1;
        }
    }
    else if (strcmp(type, "setups") == 0) {
        OPF_clearExperimentalSetups();
    }
    else if (strcmp(type, "site") == 0) {
        if (OPS_GetNumRemainingInputArgs() < 1) {
            opserr << "WARNING invalid number of arguments\n";
            opserr << "Want: removeExp site tag\n";
            return -1;
        }
        int tag;
        int numdata = 1;
        if (OPS_GetIntInput(&numdata, &tag) < 0) {
            opserr << "WARNING invalid removeExp site tag\n";
            return -1;
        }
        if (OPF_removeExperimentalSite(tag) == false) {
            opserr << "WARNING could not remove expSite with tag " << tag << endln;
            return -1;
        }
    }
    else if (strcmp(type, "sites") == 0) {
        OPF_clearExperimentalSites();
    }
    else if (strcmp(type, "tangentStiff") == 0) {
        if (OPS_GetNumRemainingInputArgs() < 1) {
            opserr << "WARNING invalid number of arguments\n";
            opserr << "Want: removeExp tangentStiff tag\n";
            return -1;
        }
        int tag;
        int numdata = 1;
        if (OPS_GetIntInput(&numdata, &tag) < 0) {
            opserr << "WARNING invalid removeExp tangentStiff tag\n";
            return -1;
        }
        if (OPF_removeExperimentalTangentStiff(tag) == false) {
            opserr << "WARNING could not remove expTangentStiff with tag " << tag << endln;
            return -1;
        }
    }
    else if (strcmp(type, "tangentStiffs") == 0) {
        OPF_clearExperimentalTangentStiffs();
    }
    else if (strcmp(type, "recorder") == 0) {
        if (OPS_GetNumRemainingInputArgs() < 1) {
            opserr << "WARNING invalid number of arguments\n";
            opserr << "Want: removeExp recorder tag\n";
            return -1;
        }
        int tag;
        int numdata = 1;
        if (OPS_GetIntInput(&numdata, &tag) < 0) {
            opserr << "WARNING invalid removeExp recorder tag\n";
            return -1;
        }
        ExperimentalSite* theSite = OPF_getExperimentalSiteFirst();
        if (theSite == 0) {
            opserr << "WARNING failed to get first experimental site\n";
            return -1;
        }
        if ((theSite->removeRecorder(tag)) < 0) {
            opserr << "WARNING could not remove expRecorder with tag " << tag << endln;
            return -1;
        }
    }
    else if (strcmp(type, "recorders") == 0) {
        ExperimentalSite* theSite = OPF_getExperimentalSiteFirst();
        if (theSite == 0) {
            opserr << "WARNING failed to get first experimental site\n";
            return -1;
        }
        if ((theSite->removeRecorders()) < 0) {
            opserr << "WARNING could not remove expRecorders\n";
            return -1;
        }
    }
    else {
        // experimental object type not recognized
        opserr << "WARNING unknown removeExp type: "
            << type << ": check the manual\n";
        return -1;
    }

    return 0;
}
