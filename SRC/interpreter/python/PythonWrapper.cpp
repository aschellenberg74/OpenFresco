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
//
// Description: A python wrapper for OpenFresco commands


#include "PythonWrapper.h"
#include "OpenFrescoCommands.h"
#include <FrescoGlobals.h>

extern PyObject* getPyModule();
static PythonWrapper* wrapper = 0;


PythonWrapper::PythonWrapper()
    :currentArgv(0), currentArg(0), numberArgs(0),
    methodsOpenFresco(), openfresco_docstring(""), currentResult(0)
{
    wrapper = this;
}


PythonWrapper::~PythonWrapper()
{
    wrapper = 0;
}


void PythonWrapper::resetCommandLine(int nArgs, int cArg, PyObject* argv)
{
    numberArgs = nArgs;
    currentArg = cArg - 1;
    if (currentArg < 0) currentArg = 0;
    currentArgv = argv;
}


void PythonWrapper::resetCommandLine(int cArg)
{
    if (cArg < 0) {
        currentArg += cArg;
    }
    else {
        currentArg = cArg - 1;
    }
    if (currentArg < 0) currentArg = 0;
}


void PythonWrapper::addCommand(const char* name, PyCFunction proc)
{
    PyMethodDef method = { name,proc,METH_VARARGS,openfresco_docstring };
    methodsOpenFresco.push_back(method);
}


PyMethodDef* PythonWrapper::getMethods()
{
    if (methodsOpenFresco.empty()) {
        return 0;
    }
    
    return &methodsOpenFresco[0];
}


void PythonWrapper::setOutputs(int* data, int numArgs, bool scalar)
{
    if (numArgs < 0) numArgs = 0;
    
    if (scalar) {
        if (numArgs > 0) {
            currentResult = Py_BuildValue("i", data[0]);
        }
    }
    else {
        currentResult = PyList_New(numArgs);
        for (int i = 0; i < numArgs; i++) {
            PyList_SET_ITEM(currentResult, i, Py_BuildValue("i", data[i]));
        }
    }
}


void PythonWrapper::setOutputs(double* data, int numArgs, bool scalar)
{
    if (numArgs < 0) numArgs = 0;
    
    if (scalar) {
        if (numArgs > 0) {
            currentResult = Py_BuildValue("d", data[0]);
        }
    }
    else {
        currentResult = PyList_New(numArgs);
        for (int i = 0; i < numArgs; i++) {
            PyList_SET_ITEM(currentResult, i, Py_BuildValue("d", data[i]));
        }
    }
}


void PythonWrapper::setOutputs(const char* str)
{
    currentResult = Py_BuildValue("s", str);
}


void PythonWrapper::setOutputs(std::vector<std::vector<int>>& data)
{
    PyObject* item = PyList_New((Py_ssize_t)data.size());
    for (std::size_t i = 0; i < data.size(); ++i) {
        setOutputs(&data[i][0], (int)data[i].size(), false);
        PyList_SET_ITEM(item, i, currentResult);
    }
    currentResult = item;
}


void PythonWrapper::setOutputs(std::map<const char*, int>& data)
{
    PyObject* dict = PyDict_New();
    for (auto& item : data) {
        auto* val = Py_BuildValue("i", item.second);
        PyDict_SetItemString(dict, item.first, val);
        Py_DECREF(val);
    }
    currentResult = dict;
}


void PythonWrapper::setOutputs(std::map<const char*, std::vector<int>>& data)
{
    PyObject* dict = PyDict_New();
    for (auto& item : data) {
        setOutputs(&(item.second[0]), (int)item.second.size(), false);
        PyDict_SetItemString(dict, item.first, currentResult);
        Py_DECREF(currentResult);
    }
    currentResult = dict;
}


void PythonWrapper::setOutputs(std::vector<std::vector<double>>& data)
{
    PyObject* item = PyList_New((Py_ssize_t)data.size());
    for (std::size_t i = 0; i < data.size(); ++i) {
        setOutputs(&data[i][0], (int)data[i].size(), false);
        PyList_SET_ITEM(item, i, currentResult);
    }
    currentResult = item;
}


void PythonWrapper::setOutputs(std::map<const char*, double>& data)
{
    PyObject* dict = PyDict_New();
    for (auto& item : data) {
        auto* val = Py_BuildValue("d", item.second);
        PyDict_SetItemString(dict, item.first, val);
        Py_DECREF(val);
    }
    currentResult = dict;
}


void PythonWrapper::setOutputs(std::map<const char*, std::vector<double>>& data)
{
    PyObject* dict = PyDict_New();
    for (auto& item : data) {
        setOutputs(&(item.second[0]), (int)item.second.size(), false);
        PyDict_SetItemString(dict, item.first, currentResult);
        Py_DECREF(currentResult);
    }
    currentResult = dict;
}


void PythonWrapper::setOutputs(std::vector<const char*>& data)
{
    PyObject* item = PyList_New((Py_ssize_t)data.size());
    for (std::size_t i = 0; i < data.size(); ++i) {
        setOutputs(data[i]);
        PyList_SET_ITEM(item, i, currentResult);
    }
    currentResult = item;
}


void PythonWrapper::setOutputs(std::vector<std::vector<const char*>>& data)
{
    PyObject* item = PyList_New((Py_ssize_t)data.size());
    for (std::size_t i = 0; i < data.size(); ++i) {
        setOutputs(data[i]);
        PyList_SET_ITEM(item, i, currentResult);
    }
    currentResult = item;
}


void PythonWrapper::setOutputs(std::map<const char*, const char*>& data)
{
    PyObject* dict = PyDict_New();
    for (auto& item : data) {
        auto* val = Py_BuildValue("s", item.second);
        PyDict_SetItemString(dict, item.first, val);
        Py_DECREF(val);
    }
    currentResult = dict;
}


void PythonWrapper::setOutputs(std::map<const char*, std::vector<const char*>>& data)
{
    PyObject* dict = PyDict_New();
    for (auto& item : data) {
        setOutputs(item.second);
        PyDict_SetItemString(dict, item.first, currentResult);
        Py_DECREF(currentResult);
    }
    currentResult = dict;
}


PyObject* PythonWrapper::getResults()
{
    PyObject* result = currentResult;
    currentResult = 0;
    
    if (result == 0) {
        Py_INCREF(Py_None);
        result = Py_None;
    }
    
    return result;
}


///////////////////////////////////////////////////////
/////// Python wrapper functions for OpenFresco ///////
///////////////////////////////////////////////////////
static PyObject* Py_opf_expControlPoint(PyObject* self, PyObject* args)
{
    wrapper->resetCommandLine((int)PyTuple_Size(args), 1, args);
    
    if (OPF_ExperimentalCP() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return wrapper->getResults();
}


static PyObject* Py_opf_expSignalFilter(PyObject* self, PyObject* args)
{
    wrapper->resetCommandLine((int)PyTuple_Size(args), 1, args);
    
    if (OPF_ExperimentalSignalFilter() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return wrapper->getResults();
}


static PyObject* Py_opf_expControl(PyObject* self, PyObject* args)
{
    wrapper->resetCommandLine((int)PyTuple_Size(args), 1, args);
    
    if (OPF_ExperimentalControl() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return wrapper->getResults();
}


static PyObject* Py_opf_expSetup(PyObject* self, PyObject* args)
{
    wrapper->resetCommandLine((int)PyTuple_Size(args), 1, args);
    
    if (OPF_ExperimentalSetup() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return wrapper->getResults();
}


static PyObject* Py_opf_expSite(PyObject* self, PyObject* args)
{
    wrapper->resetCommandLine((int)PyTuple_Size(args), 1, args);
    
    if (OPF_ExperimentalSite() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return wrapper->getResults();
}


static PyObject* Py_opf_setSizeExpSite(PyObject* self, PyObject* args)
{
    wrapper->resetCommandLine((int)PyTuple_Size(args), 1, args);
    
    if (OPF_setSizeExperimentalSite() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return wrapper->getResults();
}


static PyObject* Py_opf_expTangentStiff(PyObject* self, PyObject* args)
{
    wrapper->resetCommandLine((int)PyTuple_Size(args), 1, args);
    
    if (OPF_ExperimentalTangentStiff() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return wrapper->getResults();
}


static PyObject* Py_opf_expElement(PyObject* self, PyObject* args)
{
    wrapper->resetCommandLine((int)PyTuple_Size(args), 1, args);
    
    if (OPF_ExperimentalElement() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return wrapper->getResults();
}


static PyObject* Py_opf_expRecorder(PyObject* self, PyObject* args)
{
    wrapper->resetCommandLine((int)PyTuple_Size(args), 1, args);
    
    if (OPF_ExperimentalRecorder() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return wrapper->getResults();
}


static PyObject* Py_opf_recordExp(PyObject* self, PyObject* args)
{
    if (OPF_recordExp() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return wrapper->getResults();
}


static PyObject* Py_opf_startLabServer(PyObject* self, PyObject* args)
{
    wrapper->resetCommandLine((int)PyTuple_Size(args), 1, args);
    
    if (OPF_startLabServer() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return wrapper->getResults();
}


static PyObject* Py_opf_setupLabServer(PyObject* self, PyObject* args)
{
    wrapper->resetCommandLine((int)PyTuple_Size(args), 1, args);
    
    if (OPF_setupLabServer() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return wrapper->getResults();
}


static PyObject* Py_opf_stepLabServer(PyObject* self, PyObject* args)
{
    wrapper->resetCommandLine((int)PyTuple_Size(args), 1, args);
    
    if (OPF_stepLabServer() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return wrapper->getResults();
}


static PyObject* Py_opf_stopLabServer(PyObject* self, PyObject* args)
{
    wrapper->resetCommandLine((int)PyTuple_Size(args), 1, args);
    
    if (OPF_stopLabServer() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return wrapper->getResults();
}


static PyObject* Py_opf_startSimAppSiteServer(PyObject* self, PyObject* args)
{
    wrapper->resetCommandLine((int)PyTuple_Size(args), 1, args);
    
    if (OPF_startSimAppSiteServer() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return wrapper->getResults();
}


static PyObject* Py_opf_startSimAppElemServer(PyObject* self, PyObject* args)
{
    wrapper->resetCommandLine((int)PyTuple_Size(args), 1, args);
    
    if (OPF_startSimAppElemServer() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return wrapper->getResults();
}


static PyObject* Py_opf_wipeExp(PyObject* self, PyObject* args)
{
    wrapper->resetCommandLine((int)PyTuple_Size(args), 1, args);
    
    if (OPF_wipeExp() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return wrapper->getResults();
}


static PyObject* Py_opf_removeExp(PyObject* self, PyObject* args)
{
    wrapper->resetCommandLine((int)PyTuple_Size(args), 1, args);
    
    if (OPF_removeObject() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return wrapper->getResults();
}


static PyObject* Py_opf_version(PyObject* self, PyObject* args)
{
    if (OPF_version() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return wrapper->getResults();
}


/////////////////////////////////////////////////////
/////// Python wrapper functions for OpenSees ///////
/////////////////////////////////////////////////////
static PyObject* Py_ops_model(PyObject* self, PyObject* args)
{
    wrapper->resetCommandLine((int)PyTuple_Size(args), 1, args);
    
    if (OPS_model() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return wrapper->getResults();
}


static PyObject* Py_ops_logFile(PyObject* self, PyObject* args)
{
    wrapper->resetCommandLine((int)PyTuple_Size(args), 1, args);
    
    if (OPS_logFile() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return wrapper->getResults();
}


static PyObject* Py_ops_metaData(PyObject* self, PyObject* args)
{
    wrapper->resetCommandLine((int)PyTuple_Size(args), 1, args);
    
    if (OPS_metaData() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return wrapper->getResults();
}


static PyObject* Py_ops_defaultUnits(PyObject* self, PyObject* args)
{
    wrapper->resetCommandLine((int)PyTuple_Size(args), 1, args);
    
    if (OPS_defaultUnits() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return wrapper->getResults();
}


static PyObject* Py_ops_startTimer(PyObject* self, PyObject* args)
{
    wrapper->resetCommandLine((int)PyTuple_Size(args), 1, args);
    
    if (OPS_startTimer() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return wrapper->getResults();
}


static PyObject* Py_ops_stopTimer(PyObject* self, PyObject* args)
{
    wrapper->resetCommandLine((int)PyTuple_Size(args), 1, args);
    
    if (OPS_stopTimer() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return wrapper->getResults();
}


static PyObject* Py_ops_node(PyObject* self, PyObject* args)
{
    wrapper->resetCommandLine((int)PyTuple_Size(args), 1, args);
    
    if (OPS_Node() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return wrapper->getResults();
}


static PyObject* Py_ops_fix(PyObject* self, PyObject* args)
{
    wrapper->resetCommandLine((int)PyTuple_Size(args), 1, args);
    
    if (OPS_HomogeneousBC() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return wrapper->getResults();
}


static PyObject* Py_ops_fixX(PyObject* self, PyObject* args)
{
    wrapper->resetCommandLine((int)PyTuple_Size(args), 1, args);
    
    if (OPS_HomogeneousBC_X() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return wrapper->getResults();
}


static PyObject* Py_ops_fixY(PyObject* self, PyObject* args)
{
    wrapper->resetCommandLine((int)PyTuple_Size(args), 1, args);
    
    if (OPS_HomogeneousBC_Y() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return wrapper->getResults();
}


static PyObject* Py_ops_fixZ(PyObject* self, PyObject* args)
{
    wrapper->resetCommandLine((int)PyTuple_Size(args), 1, args);
    
    if (OPS_HomogeneousBC_Z() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return wrapper->getResults();
}


static PyObject* Py_ops_UniaxialMaterial(PyObject* self, PyObject* args)
{
    wrapper->resetCommandLine((int)PyTuple_Size(args), 1, args);
    
    if (OPS_UniaxialMaterial() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return wrapper->getResults();
}


static PyObject* Py_ops_geomTransf(PyObject* self, PyObject* args)
{
    wrapper->resetCommandLine((int)PyTuple_Size(args), 1, args);
    
    if (OPS_CrdTransf() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return wrapper->getResults();
}


static PyObject* Py_ops_rayleigh(PyObject* self, PyObject* args)
{
    wrapper->resetCommandLine((int)PyTuple_Size(args), 1, args);
    
    if (OPS_rayleighDamping() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return wrapper->getResults();
}


static PyObject* Py_ops_setElementRayleighDampingFactors(PyObject* self, PyObject* args)
{
    wrapper->resetCommandLine((int)PyTuple_Size(args), 1, args);
    
    if (OPS_setElementRayleighDampingFactors() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return wrapper->getResults();
}


/////////////////////////////////////////////////
////////////// Add Python commands //////////////
/////////////////////////////////////////////////
void PythonWrapper::addOpenFrescoCommands()
{
    // OpenFresco commands
    addCommand("expControlPoint", &Py_opf_expControlPoint);
    addCommand("expSignalFilter", &Py_opf_expSignalFilter);
    addCommand("expControl", &Py_opf_expControl);
    addCommand("expSetup", &Py_opf_expSetup);
    addCommand("expSite", &Py_opf_expSite);
    addCommand("setSizeExpSite", &Py_opf_setSizeExpSite);
    addCommand("expTangentStiff", &Py_opf_expTangentStiff);
    addCommand("expElement", &Py_opf_expElement);
    addCommand("expRecorder", &Py_opf_expRecorder);
    addCommand("recordExp", &Py_opf_recordExp);
    addCommand("startLabServer", &Py_opf_startLabServer);
    addCommand("setupLabServer", &Py_opf_setupLabServer);
    addCommand("stepLabServer", &Py_opf_stepLabServer);
    addCommand("stopLabServer", &Py_opf_stopLabServer);
    addCommand("startSimAppSiteServer", &Py_opf_startSimAppSiteServer);
    addCommand("startSimAppElemServer", &Py_opf_startSimAppElemServer);
    addCommand("wipeExp", &Py_opf_wipeExp);
    addCommand("removeExp", &Py_opf_removeExp);
    addCommand("version", &Py_opf_version);
    
    // OpenSees commands
    addCommand("model", &Py_ops_model);
    addCommand("logFile", &Py_ops_logFile);
    addCommand("metaData", &Py_ops_metaData);
    addCommand("defaultUnits", &Py_ops_defaultUnits);
    addCommand("start", &Py_ops_startTimer);
    addCommand("stop", &Py_ops_stopTimer);
    addCommand("node", &Py_ops_node);
    addCommand("fix", &Py_ops_fix);
    addCommand("fixX", &Py_ops_fixX);
    addCommand("fixY", &Py_ops_fixY);
    addCommand("fixZ", &Py_ops_fixZ);
    addCommand("uniaxialMaterial", &Py_ops_UniaxialMaterial);
    addCommand("geomTransf", &Py_ops_geomTransf);
    addCommand("rayleigh", &Py_ops_rayleigh);
    addCommand("setElementRayleighDampingFactors", &Py_ops_setElementRayleighDampingFactors);
    
    PyMethodDef method = { NULL,NULL,0,NULL };
    methodsOpenFresco.push_back(method);
}
