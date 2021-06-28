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

// Written: Minjie
//
// Description: A python wrapper for OpenFresco commands


#include "PythonWrapper.h"
#include "OpenFrescoCommands.h"
#include <FrescoGlobals.h>

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
	wrapper->resetCommandLine((int)PyTuple_Size(args), 1, args);
	
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
	wrapper->resetCommandLine((int)PyTuple_Size(args), 1, args);
	
	if (OPF_version() < 0) {
		opserr << (void*)0;
		return NULL;
	}
	
	return wrapper->getResults();
}


/////////////////////////////////////////////////
////////////// Add Python commands //////////////
/////////////////////////////////////////////////
void
PythonWrapper::addOpenFrescoCommands()
{
	addCommand("expControlPoint", &Py_opf_expControlPoint);
	addCommand("expSignalFilter", &Py_opf_expSignalFilter);
	addCommand("expControl", &Py_opf_expControl);
	addCommand("expSetup", &Py_opf_expSetup);
	addCommand("expSite", &Py_opf_expSite);
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
	
	PyMethodDef method = { NULL,NULL,0,NULL };
	methodsOpenFresco.push_back(method);
}
