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
// Description: This file contains the class definition for Python Interpreter
// PythonInterpreter implements a DL_Interpreter for the python language

#include "PythonInterpreter.h"
#include "PythonStream.h"
#include <FrescoGlobals.h>

static PythonStream sserr;
OPS_Stream* opserrPtr = &sserr;
static PythonInterpreter* interp = 0;


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


int PythonInterpreter::getDoubleList(int* size, Vector* data)
{
	if (wrapper.getCurrentArg() >= wrapper.getNumberArgs()) {
		return -1;
	}
	
	PyObject* o = PyTuple_GetItem(wrapper.getCurrentArgv(), wrapper.getCurrentArg());
	wrapper.incrCurrentArg();
	
	if (PyList_Check(o)) {
		*size = (int)PyList_Size(o);
		data->resize(*size);
		for (int i = 0; i < *size; i++) {
			PyErr_Clear();
			PyObject* item = PyList_GetItem(o, i);
			if (!(PyLong_Check(item) || PyFloat_Check(item) || PyBool_Check(item))) {
				opserr << "PythonInterpreter::getDoubleList error: item " << i << " in list is not a float (or int or bool)\n";
				return -1;
			}
			(*data)(i) = PyFloat_AsDouble(item);
			if (PyErr_Occurred()) {
				return -1;
			}
		}
	}
	else if (PyTuple_Check(o)) {
		*size = (int)PyTuple_Size(o);
		data->resize(*size);
		for (int i = 0; i < *size; i++) {
			PyErr_Clear();
			PyObject* item = PyTuple_GetItem(o, i);
			if (!(PyLong_Check(item) || PyFloat_Check(item) || PyBool_Check(item))) {
				opserr << "PythonInterpreter::getDoubleList error: item " << i << " in tuple is not a float (or int or bool)\n";
				return -1;
			}
			(*data)(i) = PyFloat_AsDouble(item);
			if (PyErr_Occurred()) {
				return -1;
			}
		}
	}
	else {
		opserr << "PythonInterpreter::getDoubleList error: input is neither a list nor a tuple\n";
		return -1;
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


int PythonInterpreter::evalDoubleStringExpression(const char* theExpression, double& current_val)
{
	if (theExpression == 0) {
		opserr << "PythonInterpreter::evalDoubleStringExpression error: Expression not set\n";
		return -1;
	}
	
	// run the string and get results
	PyObject* py_main = PyImport_AddModule("__main__");
	if (py_main == NULL) {
		opserr << "PythonInterpreter::evalDoubleStringExpression error: cannot add module  __main__\n";
		return -1;
	}
	PyObject* py_dict = PyModule_GetDict(py_main);
	if (py_main == NULL) {
		opserr << "PythonInterpreter::evalDoubleStringExpression error: cannot get dict of module __main__\n";
		return -1;
	}
	PyObject* PyRes = PyRun_String(theExpression, Py_eval_input, py_dict, py_dict);
	
	if (PyRes == NULL) {
		opserr << "PythonInterpreter::evalDoubleStringExpression error: failed to evaluate expression\n";
		return -1;
	}
	
	// get results
	if (!(PyLong_Check(PyRes) || PyFloat_Check(PyRes) || PyBool_Check(PyRes))) {
		opserr << "PythonInterpreter::evalDoubleStringExpression error: the expression must return a float (or int or bool)\n";
		return -1;
	}
	current_val = PyFloat_AsDouble(PyRes);
	
	// done
	return 0;
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


int PythonInterpreter::setInt(std::vector<std::vector<int>>& data)
{
	wrapper.setOutputs(data);
	
	return 0;
}


int PythonInterpreter::setInt(std::map<const char*, int>& data)
{
	wrapper.setOutputs(data);
		
	return 0;
}


int PythonInterpreter::setInt(std::map<const char*, std::vector<int>>& data)
{
	wrapper.setOutputs(data);
		
	return 0;
}


int PythonInterpreter::setDouble(double* data, int numArgs, bool scalar)
{
	wrapper.setOutputs(data, numArgs, scalar);
	
	return 0;
}


int PythonInterpreter::setDouble(std::vector<std::vector<double>>& data)
{
	wrapper.setOutputs(data);
	
	return 0;
}


int PythonInterpreter::setDouble(std::map<const char*, double>& data)
{
	wrapper.setOutputs(data);
		
	return 0;
}


int PythonInterpreter::setDouble(std::map<const char*, std::vector<double>>& data)
{
	wrapper.setOutputs(data);
		
	return 0;
}


int PythonInterpreter::setString(const char* str)
{
	wrapper.setOutputs(str);
	
	return 0;
}


int PythonInterpreter::setString(std::vector<const char*>& data)
{
	wrapper.setOutputs(data);
	
	return 0;
}


int PythonInterpreter::setString(std::vector<std::vector<const char*>>& data)
{
	wrapper.setOutputs(data);
		
	return 0;
}


int PythonInterpreter::setString(std::map<const char*, const char*>& data)
{
	wrapper.setOutputs(data);
		
	return 0;
}


int PythonInterpreter::setString(std::map<const char*, std::vector<const char*>>& data)
{
	wrapper.setOutputs(data);
		
	return 0;
}


int PythonInterpreter::runCommand(const char* cmd)
{
	return PyRun_SimpleString(cmd);
}


PyMethodDef* getmethodsFunc()
{
	interp = new PythonInterpreter;
	PythonWrapper* wrapper = interp->getWrapper();
	wrapper->addOpenFrescoCommands();
		
	return wrapper->getMethods();
}


void cleanupFunc()
{
	interp->getCmds().wipeExp();
	if (interp != 0) {
		delete interp;
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
		"OpenFrescoPy",
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
PyInit_OpenFrescoPy(void)

#else
#define INITERROR return

//void
PyMODINIT_FUNC
initopenfrescopy(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
	PyObject* pymodule = PyModule_Create(&moduledef);
#else
	PyObject* pymodule = Py_InitModule("OpenFrescoPy", getmethodsFunc());
#endif
	
	if (pymodule == NULL)
		INITERROR;
	struct module_state* st = GETSTATE(pymodule);
	
	st->error = PyErr_NewExceptionWithDoc("OpenFrescoPy.OpenFrescoError", "Internal OpenFresco errors.", NULL, NULL);
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
