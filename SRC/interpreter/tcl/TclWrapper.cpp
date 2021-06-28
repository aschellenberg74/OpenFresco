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
// Description: A tcl wrapper for OpenFresco commands


#include "TclWrapper.h"
#include "OpenFrescoCommands.h"
#include <FrescoGlobals.h>

static TclWrapper* wrapper = 0;


TclWrapper::TclWrapper()
    :currentArgv(0), currentArg(0), numberArgs(0)
{
    wrapper = this;
}


TclWrapper::~TclWrapper()
{
    // does nothing
}


void TclWrapper::resetCommandLine(int nArgs, int cArg, TCL_Char** argv)
{
    numberArgs = nArgs;
    currentArg = cArg;
    currentArgv = argv;
}


void TclWrapper::resetCommandLine(int cArg)
{
    if (cArg < 0) {
        currentArg += cArg;
        if (currentArg < 0) currentArg = 0;
    }
    else {
        currentArg = cArg;
    }
}


void TclWrapper::addCommand(Tcl_Interp* interp, const char* name, Tcl_CmdProc* proc)
{
    Tcl_CreateCommand(interp, name, proc,NULL,NULL);
}


void TclWrapper::setOutputs(Tcl_Interp* interp, int* data, int numArgs)
{
    char buffer[40];
    for (int i = 0; i < numArgs; i++) {
        sprintf(buffer, "%d ", data[i]);
        Tcl_AppendResult(interp, buffer, NULL);
    }
}


void TclWrapper::setOutputs(Tcl_Interp* interp, double* data, int numArgs)
{
    char buffer[40];
    for (int i = 0; i < numArgs; i++) {
        sprintf(buffer, "%35.20f ", data[i]);
        Tcl_AppendResult(interp, buffer, NULL);
    }
}


void TclWrapper::setOutputs(Tcl_Interp* interp, const char* str)
{
    Tcl_SetResult(interp, (char*)str, TCL_VOLATILE);
}


////////////////////////////////////////////////////
/////// Tcl wrapper functions for OpenFresco ///////
////////////////////////////////////////////////////
static int Tcl_opf_expControlPoint(ClientData clientData, Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    wrapper->resetCommandLine(argc, 1, argv);
    
    if (OPF_ExperimentalCP() < 0) return TCL_ERROR;
    
    return TCL_OK;
}

static int Tcl_opf_expSignalFilter(ClientData clientData, Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    wrapper->resetCommandLine(argc, 1, argv);
    
    if (OPF_ExperimentalSignalFilter() < 0) return TCL_ERROR;
    
    return TCL_OK;
}

static int Tcl_opf_expControl(ClientData clientData, Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    wrapper->resetCommandLine(argc, 1, argv);
    
    if (OPF_ExperimentalControl() < 0) return TCL_ERROR;
    
    return TCL_OK;
}

static int Tcl_opf_expSetup(ClientData clientData, Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    wrapper->resetCommandLine(argc, 1, argv);
    
    if (OPF_ExperimentalSetup() < 0) return TCL_ERROR;
    
    return TCL_OK;
}

static int Tcl_opf_expSite(ClientData clientData, Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    wrapper->resetCommandLine(argc, 1, argv);
    
    if (OPF_ExperimentalSite() < 0) return TCL_ERROR;
    
    return TCL_OK;
}

static int Tcl_opf_expTangentStiff(ClientData clientData, Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    wrapper->resetCommandLine(argc, 1, argv);
    
    if (OPF_ExperimentalTangentStiff() < 0) return TCL_ERROR;
    
    return TCL_OK;
}

static int Tcl_opf_expElement(ClientData clientData, Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    wrapper->resetCommandLine(argc, 1, argv);
    
    if (OPF_ExperimentalElement() < 0) return TCL_ERROR;
    
    return TCL_OK;
}

static int Tcl_opf_expRecorder(ClientData clientData, Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    wrapper->resetCommandLine(argc, 1, argv);
    
    if (OPF_ExperimentalRecorder() < 0) return TCL_ERROR;
    
    return TCL_OK;
}

static int Tcl_opf_recordExp(ClientData clientData, Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    wrapper->resetCommandLine(argc, 1, argv);
    
    if (OPF_recordExp() < 0) return TCL_ERROR;
    
    return TCL_OK;
}

static int Tcl_opf_startLabServer(ClientData clientData, Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    wrapper->resetCommandLine(argc, 1, argv);
    
    if (OPF_startLabServer() < 0) return TCL_ERROR;
    
    return TCL_OK;
}

static int Tcl_opf_setupLabServer(ClientData clientData, Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    wrapper->resetCommandLine(argc, 1, argv);
    
    if (OPF_setupLabServer() < 0) return TCL_ERROR;
    
    return TCL_OK;
}

static int Tcl_opf_stepLabServer(ClientData clientData, Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    wrapper->resetCommandLine(argc, 1, argv);
    
    if (OPF_stepLabServer() < 0) return TCL_ERROR;
    
    return TCL_OK;
}

static int Tcl_opf_stopLabServer(ClientData clientData, Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    wrapper->resetCommandLine(argc, 1, argv);
    
    if (OPF_stopLabServer() < 0) return TCL_ERROR;
    
    return TCL_OK;
}

static int Tcl_opf_startSimAppSiteServer(ClientData clientData, Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    wrapper->resetCommandLine(argc, 1, argv);
    
    if (OPF_startSimAppSiteServer() < 0) return TCL_ERROR;
    
    return TCL_OK;
}

static int Tcl_opf_startSimAppElemServer(ClientData clientData, Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    wrapper->resetCommandLine(argc, 1, argv);
    
    if (OPF_startSimAppElemServer() < 0) return TCL_ERROR;
    
    return TCL_OK;
}

static int Tcl_opf_wipeExp(ClientData clientData, Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    wrapper->resetCommandLine(argc, 1, argv);
    
    if (OPF_wipeExp() < 0) return TCL_ERROR;
    
    return TCL_OK;
}

static int Tcl_opf_removeExp(ClientData clientData, Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    wrapper->resetCommandLine(argc, 1, argv);
    
    if (OPF_removeObject() < 0) return TCL_ERROR;
    
    return TCL_OK;
}

static int Tcl_opf_version(ClientData clientData, Tcl_Interp* interp, int argc, TCL_Char** argv)
{
    wrapper->resetCommandLine(argc, 1, argv);
    
    if (OPF_version() < 0) return TCL_ERROR;
    
    return TCL_OK;
}


//////////////////////////////////////////////////
/////// Tcl wrapper functions for OpenSees ///////
//////////////////////////////////////////////////
static int Tcl_ops_model(ClientData clientData, Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    wrapper->resetCommandLine(argc, 1, argv);
    
    if (OPS_model() < 0) return TCL_ERROR;
    
    return TCL_OK;
}

static int Tcl_ops_logFile(ClientData clientData, Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    wrapper->resetCommandLine(argc, 1, argv);
    
    if (OPS_logFile() < 0) return TCL_ERROR;
    
    return TCL_OK;
}

static int Tcl_ops_metaData(ClientData clientData, Tcl_Interp* interp, int argc, TCL_Char** argv)
{
    wrapper->resetCommandLine(argc, 1, argv);
    
    if (OPS_metaData() < 0) return TCL_ERROR;
    
    return TCL_OK;
}

static int Tcl_ops_defaultUnits(ClientData clientData, Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    wrapper->resetCommandLine(argc, 1, argv);
    
    if (OPS_defaultUnits() < 0) return TCL_ERROR;
    
    return TCL_OK;
}

static int Tcl_ops_startTimer(ClientData clientData, Tcl_Interp* interp, int argc, TCL_Char** argv)
{
    wrapper->resetCommandLine(argc, 1, argv);
    
    if (OPS_startTimer() < 0) return TCL_ERROR;
    
    return TCL_OK;
}

static int Tcl_ops_stopTimer(ClientData clientData, Tcl_Interp* interp, int argc, TCL_Char** argv)
{
    wrapper->resetCommandLine(argc, 1, argv);
    
    if (OPS_stopTimer() < 0) return TCL_ERROR;
    
    return TCL_OK;
}

static int Tcl_ops_node(ClientData clientData, Tcl_Interp* interp, int argc, TCL_Char** argv)
{
    wrapper->resetCommandLine(argc, 1, argv);
    
    if (OPS_Node() < 0) return TCL_ERROR;
    
    return TCL_OK;
}

static int Tcl_ops_fix(ClientData clientData, Tcl_Interp* interp, int argc, TCL_Char** argv)
{
    wrapper->resetCommandLine(argc, 1, argv);
    
    if (OPS_HomogeneousBC() < 0) return TCL_ERROR;
    
    return TCL_OK;
}

static int Tcl_ops_fixX(ClientData clientData, Tcl_Interp* interp, int argc, TCL_Char** argv)
{
    wrapper->resetCommandLine(argc, 1, argv);
    
    if (OPS_HomogeneousBC_X() < 0) return TCL_ERROR;
    
    return TCL_OK;
}

static int Tcl_ops_fixY(ClientData clientData, Tcl_Interp* interp, int argc, TCL_Char** argv)
{
    wrapper->resetCommandLine(argc, 1, argv);
    
    if (OPS_HomogeneousBC_Y() < 0) return TCL_ERROR;
    
    return TCL_OK;
}

static int Tcl_ops_fixZ(ClientData clientData, Tcl_Interp* interp, int argc, TCL_Char** argv)
{
    wrapper->resetCommandLine(argc, 1, argv);
    
    if (OPS_HomogeneousBC_Z() < 0) return TCL_ERROR;
    
    return TCL_OK;
}

static int Tcl_ops_UniaxialMaterial(ClientData clientData, Tcl_Interp* interp, int argc, TCL_Char** argv)
{
    wrapper->resetCommandLine(argc, 1, argv);
    
    if (OPS_UniaxialMaterial() < 0) return TCL_ERROR;
    
    return TCL_OK;
}

static int Tcl_ops_geomTransf(ClientData clientData, Tcl_Interp* interp, int argc, TCL_Char** argv)
{
    wrapper->resetCommandLine(argc, 1, argv);
    
    if (OPS_CrdTransf() < 0) return TCL_ERROR;
    
    return TCL_OK;
}

static int Tcl_ops_rayleigh(ClientData clientData, Tcl_Interp* interp, int argc, TCL_Char** argv)
{
    wrapper->resetCommandLine(argc, 1, argv);
    
    if (OPS_rayleighDamping() < 0) return TCL_ERROR;
    
    return TCL_OK;
}

static int Tcl_ops_setElementRayleighDampingFactors(ClientData clientData, Tcl_Interp* interp, int argc, TCL_Char** argv)
{
    wrapper->resetCommandLine(argc, 1, argv);
    
    if (OPS_setElementRayleighDampingFactors() < 0) return TCL_ERROR;
    
    return TCL_OK;
}


////////////////////////////////
/////// Add Tcl commands ///////
////////////////////////////////
void TclWrapper::addOpenFrescoCommands(Tcl_Interp* interp)
{
    // OpenFresco commands
    addCommand(interp, "expControlPoint", &Tcl_opf_expControlPoint);
    addCommand(interp, "expSignalFilter", &Tcl_opf_expSignalFilter);
    addCommand(interp, "expControl", &Tcl_opf_expControl);
    addCommand(interp, "expSetup", &Tcl_opf_expSetup);
    addCommand(interp, "expSite", &Tcl_opf_expSite);
    addCommand(interp, "expTangentStiff", &Tcl_opf_expTangentStiff);
    addCommand(interp, "expElement", &Tcl_opf_expElement);
    addCommand(interp, "expRecorder", &Tcl_opf_expRecorder);
    addCommand(interp, "recordExp", &Tcl_opf_recordExp);
    addCommand(interp, "startLabServer", &Tcl_opf_startLabServer);
    addCommand(interp, "setupLabServer", &Tcl_opf_setupLabServer);
    addCommand(interp, "stepLabServer", &Tcl_opf_stepLabServer);
    addCommand(interp, "stopLabServer", &Tcl_opf_stopLabServer);
    addCommand(interp, "startSimAppSiteServer", &Tcl_opf_startSimAppSiteServer);
    addCommand(interp, "startSimAppElemServer", &Tcl_opf_startSimAppElemServer);
    addCommand(interp, "wipeExp", &Tcl_opf_wipeExp);
    addCommand(interp, "removeExp", &Tcl_opf_removeExp);
    addCommand(interp, "version", &Tcl_opf_version);

    // OpenSees commands
    addCommand(interp, "model", &Tcl_ops_model);
    addCommand(interp, "logFile", &Tcl_ops_logFile);
    addCommand(interp, "metaData", &Tcl_ops_metaData);
    addCommand(interp, "defaultUnits", &Tcl_ops_defaultUnits);
    addCommand(interp, "start", &Tcl_ops_startTimer);
    addCommand(interp, "stop", &Tcl_ops_stopTimer);
    addCommand(interp, "node", &Tcl_ops_node);
    addCommand(interp, "fix", &Tcl_ops_fix);
    addCommand(interp, "fixX", &Tcl_ops_fixX);
    addCommand(interp, "fixY", &Tcl_ops_fixY);
    addCommand(interp, "fixZ", &Tcl_ops_fixZ);
    addCommand(interp, "uniaxialMaterial", &Tcl_ops_UniaxialMaterial);
    addCommand(interp, "geomTransf", &Tcl_ops_geomTransf);
    addCommand(interp, "rayleigh", &Tcl_ops_rayleigh);
    addCommand(interp, "setElementRayleighDampingFactors", &Tcl_ops_setElementRayleighDampingFactors);
}
