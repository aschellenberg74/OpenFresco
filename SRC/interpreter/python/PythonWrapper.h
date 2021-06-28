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

#ifndef PythonWrapper_h
#define PythonWrapper_h

#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif

#include <vector>

class PythonWrapper
{
public:
	PythonWrapper();
	~PythonWrapper();

	// reset command line
	void resetCommandLine(int nArgs, int cArg, PyObject* argv);
	void resetCommandLine(int cArg);

	// wrapper commands
	void addOpenFrescoCommands();
	void addCommand(const char* name, PyCFunction proc);
	PyMethodDef* getMethods();

	// get command line arguments
	PyObject* getCurrentArgv() { return currentArgv; }
	int getCurrentArg() const { return currentArg; }
	int getNumberArgs() const { return numberArgs; }
	void incrCurrentArg() { currentArg++; }

	// set outputs
	void setOutputs(int* data, int numArgs, bool scalar);
	void setOutputs(double* data, int numArgs, bool scalar);
	void setOutputs(const char* str);
	PyObject* getResults();

private:
	// command line arguments
	PyObject* currentArgv;
	int currentArg;
	int numberArgs;

	// methods table
	std::vector<PyMethodDef> methodsOpenFresco;
	const char* openfresco_docstring;
	PyObject* currentResult;
};

#endif
