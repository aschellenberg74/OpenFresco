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
// Description: This file contains the class definition for Python Interpreter
// PythonInterpreter implements a DL_Interpreter for the python language

#include "PythonInterpreter.h"
#include "PythonStream.h"
#include <FrescoGlobals.h>

static PythonStream sserr;
OPS_Stream* opserrPtr = &sserr;
static PythonInterpreter* module = 0;


PythonInterpreter::PythonInterpreter()
	: wrapper(), cmds(this)
{
	// does nothing
}


PythonInterpreter::~PythonInterpreter()
{
	// does nothing
}


int PythonInterpreter::run()
{
	return 0;
}


int PythonInterpreter::addCommand(const char*, Command&)
{
	return -1;
}


int PythonInterpreter::removeCommand(const char*)
{
	return -1;
}


int PythonInterpreter::getNumRemainingInputArgs(void)
{
	return wrapper.getNumberArgs() - wrapper.getCurrentArg();
}


int PythonInterpreter::getInt(int* data, int numArgs)
{
	if ((wrapper.getNumberArgs() - wrapper.getCurrentArg()) < numArgs) {
		return -1;
	}
	
	for (int i = 0; i < numArgs; i++) {
		PyObject* o = PyTuple_GetItem(wrapper.getCurrentArgv(), wrapper.getCurrentArg());
		//wrapper.incrCurrentArg();
		if (PyLong_Check(o) || PyFloat_Check(o) || PyBool_Check(o)) {
			PyErr_Clear();
			data[i] = PyLong_AsLong(o);
			if (PyErr_Occurred()) {
				return -1;
			}
			wrapper.incrCurrentArg();
		}
		else {
			return -1;
		}
	}
	
	return 0;
}


int PythonInterpreter::getDouble(double* data, int numArgs)
{
	if ((wrapper.getNumberArgs() - wrapper.getCurrentArg()) < numArgs) {
		return -1;
	}
	
	for (int i = 0; i < numArgs; i++) {
		PyObject* o = PyTuple_GetItem(wrapper.getCurrentArgv(), wrapper.getCurrentArg());
		//wrapper.incrCurrentArg();
		if (PyLong_Check(o) || PyFloat_Check(o) || PyBool_Check(o)) {
			PyErr_Clear();
			data[i] = PyFloat_AsDouble(o);
			if (PyErr_Occurred()) {
				return -1;
			}
			wrapper.incrCurrentArg();
		}
		else {
			return -1;
		}
	}
	
	return 0;
}


const char* PythonInterpreter::getString()
{
	if (wrapper.getCurrentArg() >= wrapper.getNumberArgs()) {
		return 0;
	}
		
	PyObject* o = PyTuple_GetItem(wrapper.getCurrentArgv(), wrapper.getCurrentArg());
	wrapper.incrCurrentArg();
#if PY_MAJOR_VERSION >= 3
	if (!PyUnicode_Check(o)) {
		return 0;
	}
		
	PyObject* space = PyUnicode_FromString(" ");
	PyObject* empty = PyUnicode_FromString("");
	PyObject* newo = PyUnicode_Replace(o, space, empty, -1);
	const char* res = PyUnicode_AsUTF8(newo);
		
	Py_DECREF(newo);
	Py_DECREF(space);
	Py_DECREF(empty);
		
	return res;
#else
	if (!PyString_Check(o)) {
		return 0;
	}
		
	return PyString_AS_STRING(o);
#endif
}


const char* PythonInterpreter::getStringFromAll(char* buffer, int len)
{
	if (wrapper.getCurrentArg() >= wrapper.getNumberArgs()) {
		return 0;
	}

	PyObject* o = PyTuple_GetItem(wrapper.getCurrentArgv(), wrapper.getCurrentArg());
	wrapper.incrCurrentArg();

	// check if int
	if (PyLong_Check(o) || PyBool_Check(o)) {
		PyErr_Clear();
		int data = PyLong_AsLong(o);
		if (PyErr_Occurred()) {
			return 0;
		}
		snprintf(buffer, len, "%d", data);
		return buffer;
	}
	// check if double
	else if (PyFloat_Check(o)) {
		PyErr_Clear();
		double data = PyFloat_AsDouble(o);
		if (PyErr_Occurred()) {
			return 0;
		}
		snprintf(buffer, len, "%.20f", data);
		return buffer;
	}

#if PY_MAJOR_VERSION >= 3
	if (!PyUnicode_Check(o)) {
		return 0;
	}

	PyObject* space = PyUnicode_FromString(" ");
	PyObject* empty = PyUnicode_FromString("");
	PyObject* newo = PyUnicode_Replace(o, space, empty, -1);
	const char* res = PyUnicode_AsUTF8(newo);

	Py_DECREF(newo);
	Py_DECREF(space);
	Py_DECREF(empty);

	int lenres = (int)strlen(res) + 1;
	if (lenres > len) {
		lenres = len;
	}
	
	strncpy(buffer, res, lenres);
	
	return buffer;
#else
	if (!PyString_Check(o)) {
		return 0;
	}
	
	return PyString_AS_STRING(o);
#endif
}


int PythonInterpreter::getStringCopy(char** stringPtr)
{
	return -1;
}


void PythonInterpreter::resetInput(int cArg)
{
	wrapper.resetCommandLine(cArg);
}


int PythonInterpreter::setInt(int* data, int numArgs, bool scalar)
{
	wrapper.setOutputs(data, numArgs, scalar);
	
	return 0;
}


int PythonInterpreter::setDouble(double* data, int numArgs, bool scalar)
{
	wrapper.setOutputs(data, numArgs, scalar);
	
	return 0;
}


int PythonInterpreter::setString(const char* str)
{
	wrapper.setOutputs(str);
	
	return 0;
}


int PythonInterpreter::runCommand(const char* cmd)
{
	return PyRun_SimpleString(cmd);
}


PyMethodDef* getmethodsFunc()
{
	module = new PythonInterpreter;
	PythonWrapper* wrapper = module->getWrapper();
	wrapper->addOpenFrescoCommands();
		
	return wrapper->getMethods();
}


void cleanupFunc()
{
	module->getCmds().wipeExp();
	if (module != 0) {
		delete module;
	}
}


struct module_state
{
	PyObject* error;
};


#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct module_state*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct module_state _state;
#endif

#if PY_MAJOR_VERSION >= 3

static int openfresco_traverse(PyObject* m, visitproc visit, void* arg)
{
	Py_VISIT(GETSTATE(m)->error);
	
	return 0;
}


static int openfresco_clear(PyObject* m)
{
	Py_CLEAR(GETSTATE(m)->error);
	
	return 0;
}


static struct PyModuleDef moduledef = {
		PyModuleDef_HEAD_INIT,
		"openfresco",
		NULL,
		sizeof(struct module_state),
		getmethodsFunc(),
		NULL,
		openfresco_traverse,
		openfresco_clear,
		NULL
};

#define INITERROR return NULL

PyMODINIT_FUNC
PyInit_openfresco(void)

#else
#define INITERROR return

//void
PyMODINIT_FUNC
initopenfresco(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
	PyObject* pymodule = PyModule_Create(&moduledef);
#else
	PyObject* pymodule = Py_InitModule("openfresco", getmethodsFunc());
#endif
	
	if (pymodule == NULL)
		INITERROR;
	struct module_state* st = GETSTATE(pymodule);
	
	st->error = PyErr_NewExceptionWithDoc("openfresco.OpenFrescoError", "Internal OpenFresco errors.", NULL, NULL);
	if (st->error == NULL) {
		Py_DECREF(pymodule);
		INITERROR;
	}
	Py_INCREF(st->error);
	PyModule_AddObject(pymodule, "OpenFrescoError", st->error);
	
	// char version[10];
	// const char *py_version = ".6";
	// for (int i = 0; i < 5; ++i) {
	//     version[i] = OPF_VERSION[i];
	// }
	// for (int i = 0; i < 3; ++i) {
	//     version[5 + i] = py_version[i];
	// }
	// PyModule_AddStringConstant(pymodule, "__version__", version);
	
	sserr.setError(st->error);
	
	Py_AtExit(cleanupFunc);
	
#if PY_MAJOR_VERSION >= 3
	return pymodule;
#endif
}
