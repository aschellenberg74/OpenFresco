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
// Created: 24/06
// Revision: A
//
// Description: Defines the entry point for the DLL application and
// a function that can be called to set the global pointer variables in 
// the dll to be the same as those in the existing process address space.

#ifdef _WIN32
#include <corecrt.h>
#endif

#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif

#include <StandardStream.h>
#include <conio.h>
#include <Domain.h>
#include <AnalysisModel.h>
#include <StaticAnalysis.h>
#include <DirectIntegrationAnalysis.h>
#include <VariableTimeStepDirectIntegrationAnalysis.h>
#include <elementAPI.h>
#include <FrescoGlobals.h>
#include <ExperimentalSite.h>

#ifdef _WIN32
#define DllExport _declspec(dllexport)
#else
#define DllExport
extern Domain* ops_TheActiveDomain;
double ops_Dt = 0.0;
StandardStream sserr;
OPS_Stream* opserrPtr = &sserr;
#endif

// main OpenFresco objects commands 
extern int OPF_ExperimentalCP();
extern int OPF_ExperimentalSignalFilter();
extern int OPF_ExperimentalControl();
extern int OPF_ExperimentalSetup();
extern int OPF_ExperimentalSite();
extern int OPF_ExperimentalTangentStiff();
extern int OPF_ExperimentalElement();
extern int OPF_ExperimentalRecorder();
extern int OPF_setSizeExperimentalSite();
extern int OPF_recordExp();

// server OpenFresco commands
extern int OPF_startLabServer();
extern int OPF_setupLabServer();
extern int OPF_stepLabServer();
extern int OPF_stopLabServer();
extern int OPF_startSimAppSiteServer();
extern int OPF_startSimAppElemServer();

// clear OpenFresco object commands
extern void OPF_clearExperimentalCPs();
extern void OPF_clearExperimentalSignalFilters();
extern void OPF_clearExperimentalControls();
extern void OPF_clearExperimentalSetups();
extern void OPF_clearExperimentalSites();
extern void OPF_clearExperimentalTangentStiffs();

// remove OpenFresco object commands
extern bool OPF_removeExperimentalCP(int tag);
extern bool OPF_removeExperimentalSignalFilter(int tag);
extern bool OPF_removeExperimentalControl(int tag);
extern bool OPF_removeExperimentalSetup(int tag);
extern bool OPF_removeExperimentalSite(int tag);
extern bool OPF_removeExperimentalTangentStiff(int tag);

// global variables
std::vector<PyMethodDef> methodsOpenFresco;
const char* openfresco_docstring = "";
PyObject* currentResult;
//PyObject* theModule = 0;
Domain* theDomain = 0;
StaticAnalysis** theStaticAnalysis = 0;
DirectIntegrationAnalysis** theTransientAnalysis = 0;
VariableTimeStepDirectIntegrationAnalysis** theVariableTimeStepTransientAnalysis = 0;


void setOutputs(int* data, int numArgs, bool scalar)
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


void setOutputs(double* data, int numArgs, bool scalar)
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


void setOutputs(const char* str)
{
    currentResult = Py_BuildValue("s", str);
}


PyObject* getResults()
{
    PyObject* result = currentResult;
    currentResult = 0;
    
    if (result == 0) {
        Py_INCREF(Py_None);
        result = Py_None;
    }
    
    return result;
}


int OPS_SetIntOutput(int* numData, int* data, bool scalar)
{
    int numArgs = *numData;
    setOutputs(data, numArgs, scalar);
    
    return 0;
}


int OPS_SetDoubleOutput(int* numData, double* data, bool scalar)
{
    int numArgs = *numData;
    setOutputs(data, numArgs, scalar);
    
    return 0;
}


int OPS_SetString(const char* str)
{
    setOutputs(str);
    
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


int OPF_version()
{
    if (OPS_SetString(OPF_VERSION) < 0) {
        opserr << "WARNING failed to set version string\n";
        return -1;
    }
    
    return 0;
}


// method to add Python commands
void Py_opf_addCommand(const char* name, PyCFunction proc)
{
    PyMethodDef method = { name,proc,METH_VARARGS,openfresco_docstring };
    methodsOpenFresco.push_back(method);
    /*
    // get the module name and dictionary
    PyObject* moduleName = PyUnicode_FromString(PyModule_GetName(theModule));
    PyObject* moduleDict = PyModule_GetDict(theModule);
    
    // set up the method definition structure
    PyMethodDef method = { name,proc,METH_VARARGS,openfresco_docstring };
    
    // create a callable Python object from the method definition
    PyObject* obj = PyCFunction_NewEx(&method, (PyObject*)NULL, moduleName);
    if (obj == NULL) {
        opserr << "WARNING failed to create Python object for method: " << name << endln;
        return;
    }
    
    // add the method to the module dictionary
    if (PyDict_SetItemString(moduleDict, name, obj) < 0) {
        opserr << "WARNING failed to add method: " << name << " to module dictionary\n";
        Py_DECREF(obj);
        return;
    }
    */
}


// experimental control point commands
static PyObject* Py_opf_expControlPoint(PyObject* self, PyObject* args)
{
    // reset the input args
    OPS_ResetCommandLine((int)PyTuple_Size(args), 1, (const char**)args);
    
    if (OPF_ExperimentalCP() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return getResults();
}


// experimental signal filter commands
static PyObject* Py_opf_expSignalFilter(PyObject* self, PyObject* args)
{
    // reset the input args
    OPS_ResetCommandLine((int)PyTuple_Size(args), 1, (const char**)args);
    
    if (OPF_ExperimentalSignalFilter() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return getResults();
}


// experimental control commands
static PyObject* Py_opf_expControl(PyObject* self, PyObject* args)
{
    // reset the input args
    OPS_ResetCommandLine((int)PyTuple_Size(args), 1, (const char**)args);
    
    if (OPF_ExperimentalControl() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return getResults();
}


// experimental setup commands
static PyObject* Py_opf_expSetup(PyObject* self, PyObject* args)
{
    // reset the input args
    OPS_ResetCommandLine((int)PyTuple_Size(args), 1, (const char**)args);
    
    if (OPF_ExperimentalSetup() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return getResults();
}


// experimental site commands
static PyObject* Py_opf_expSite(PyObject* self, PyObject* args)
{
    // reset the input args
    OPS_ResetCommandLine((int)PyTuple_Size(args), 1, (const char**)args);
    
    if (OPF_ExperimentalSite() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return getResults();
}
static PyObject* Py_opf_setSizeExpSite(PyObject* self, PyObject* args)
{
    // reset the input args
    OPS_ResetCommandLine((int)PyTuple_Size(args), 1, (const char**)args);
    
    if (OPF_setSizeExperimentalSite() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return getResults();
}


// experimental tangent stiffness commands
static PyObject* Py_opf_expTangentStiff(PyObject* self, PyObject* args)
{
    // reset the input args
    OPS_ResetCommandLine((int)PyTuple_Size(args), 1, (const char**)args);
    
    if (OPF_ExperimentalTangentStiff() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return getResults();
}


// experimental element commands
static PyObject* Py_opf_expElement(PyObject* self, PyObject* args)
{
    // reset the input args
    OPS_ResetCommandLine((int)PyTuple_Size(args), 1, (const char**)args);
    
    if (OPF_ExperimentalElement() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return getResults();
}


// experimental recorder commands
static PyObject* Py_opf_expRecorder(PyObject* self, PyObject* args)
{
    // reset the input args
    OPS_ResetCommandLine((int)PyTuple_Size(args), 1, (const char**)args);
    
    if (OPF_ExperimentalRecorder() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return getResults();
}
static PyObject* Py_opf_recordExp(PyObject* self, PyObject* args)
{
    if (OPF_recordExp() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return getResults();
}


// start laboratory server command
static PyObject* Py_opf_startLabServer(PyObject* self, PyObject* args)
{
    // reset the input args
    OPS_ResetCommandLine((int)PyTuple_Size(args), 1, (const char**)args);
    
    if (OPF_startLabServer() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return getResults();
}


// setup laboratory server command
static PyObject* Py_opf_setupLabServer(PyObject* self, PyObject* args)
{
    // reset the input args
    OPS_ResetCommandLine((int)PyTuple_Size(args), 1, (const char**)args);
    
    if (OPF_setupLabServer() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return getResults();
}


// step laboratory server command
static PyObject* Py_opf_stepLabServer(PyObject* self, PyObject* args)
{
    // reset the input args
    OPS_ResetCommandLine((int)PyTuple_Size(args), 1, (const char**)args);
    
    if (OPF_stepLabServer() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return getResults();
}


// stop laboratory server command
static PyObject* Py_opf_stopLabServer(PyObject* self, PyObject* args)
{
    // reset the input args
    OPS_ResetCommandLine((int)PyTuple_Size(args), 1, (const char**)args);
    
    if (OPF_stopLabServer() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return getResults();
}


// start simulation application site server command
static PyObject* Py_opf_startSimAppSiteServer(PyObject* self, PyObject* args)
{
    // reset the input args
    OPS_ResetCommandLine((int)PyTuple_Size(args), 1, (const char**)args);
    
    if (OPF_startSimAppSiteServer() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return getResults();
}


// start simulation application element server command
static PyObject* Py_opf_startSimAppElemServer(PyObject* self, PyObject* args)
{
    // reset the input args
    OPS_ResetCommandLine((int)PyTuple_Size(args), 1, (const char**)args);
    
    if (OPF_startSimAppElemServer() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return getResults();
}


// opensees command to analyze a model interactively
static PyObject* Py_ops_analyzeModelInteractive(PyObject* self, PyObject* args)
{
    // reset the input args
    OPS_ResetCommandLine((int)PyTuple_Size(args), 1, (const char**)args);
    
    int result = 0;
    int numSteps;
    bool exitYet = false;
    bool showMsg = true;
    int action = 'r';
    int step = 1;
    int numdata = 1;
    
    theStaticAnalysis = OPS_GetStaticAnalysis();
    theTransientAnalysis = OPS_GetTransientAnalysis();
    theVariableTimeStepTransientAnalysis = OPS_GetVariableTimeStepTransientAnalysis();
    
    if (*theStaticAnalysis != 0) {
        if (OPS_GetNumRemainingInputArgs() < 1) {
            opserr << "WARNING insufficient arguments\n"
                << "Want: analyzeInteractive numSteps ...\n";
            return NULL;
        }
        
        if (OPS_GetIntInput(&numdata, &numSteps) < 0) {
            opserr << "WARNING invalid analyzeInteractive numSteps\n";
            return NULL;
        }
        
        // interactively run the analysis (i.e. the test) for numSteps
        opserr << "\nStaticAnalysis - now interactively running for "
            << numSteps << " steps..." << endln;
        opserr << "Press 'p' to pause, 'r' to resume, 's' to step, or 'e' to end the test." << endln;
        while (step <= numSteps && exitYet == false) {
            // check if we got a keyboard input
            if (_kbhit())
                action = _getch();
            
            switch (action) {
            case 'r':
                // run or resume test
                if (!showMsg) {
                    opserr << "\nStaticAnalysis - now resuming the test..." << endln;
                    showMsg = true;
                }
                result = (*theStaticAnalysis)->analyze(1);
                step++;
                break;
            case 'p':
                // pause test
                if (showMsg) {
                    opserr << "\nStaticAnalysis - at step "
                        << step << " - now pausing the test..." << endln;
                    showMsg = false;
                }
                break;
            case 's':
            case '0':
                // 10^0 times step test
                if (!showMsg) {
                    opserr << "\nStaticAnalysis - at step "
                        << step << " - now stepping the test..." << endln;
                    showMsg = false;
                }
                result = (*theStaticAnalysis)->analyze(1);
                action = 'p';
                step++;
                break;
            case '1':
                // 10^1 times step test
                if (!showMsg) {
                    opserr << "\nStaticAnalysis - at step "
                        << step << " - now 10x stepping the test..." << endln;
                    showMsg = false;
                }
                result = (*theStaticAnalysis)->analyze(10);
                action = 'p';
                step += 10;
                break;
            case '2':
                // 10^2 times step test
                if (!showMsg) {
                    opserr << "\nStaticAnalysis - at step "
                        << step << " - now 100x stepping the test..." << endln;
                    showMsg = false;
                }
                result = (*theStaticAnalysis)->analyze(100);
                action = 'p';
                step += 100;
                break;
            case '3':
                // 10^3 times step test
                if (!showMsg) {
                    opserr << "\nStaticAnalysis - at step "
                        << step << " - now 1000x stepping the test..." << endln;
                    showMsg = false;
                }
                result = (*theStaticAnalysis)->analyze(1000);
                action = 'p';
                step += 1000;
                break;
            case '4':
                // 10^4 times step test
                if (!showMsg) {
                    opserr << "\nStaticAnalysis - at step "
                        << step << " - now 10000x stepping the test..." << endln;
                    showMsg = false;
                }
                result = (*theStaticAnalysis)->analyze(10000);
                action = 'p';
                step += 10000;
                break;
            case 'e':
                // end test
                opserr << "\nStaticAnalysis - at step "
                    << step << " - now shut down." << endln;
                (*theStaticAnalysis)->clearAll();
                exitYet = true;
                break;
            }
        }
    }
    else if (*theTransientAnalysis != 0) {
        if (OPS_GetNumRemainingInputArgs() < 2) {
            opserr << "WARNING insufficient arguments\n"
                << "Want: analyzeInteractive numSteps dt\n";
            return NULL;
        }
        
        double dT;
        if (OPS_GetIntInput(&numdata, &numSteps) < 0) {
            opserr << "WARNING invalid analyzeInteractive numSteps\n";
            return NULL;
        }
        if (OPS_GetDoubleInput(&numdata, &dT) < 0) {
            opserr << "WARNING invalid analyzeInteractive dt\n";
            return NULL;
        }
        
        // set global timestep variable
        ops_Dt = dT;
        
        if (OPS_GetNumRemainingInputArgs() == 0) {
            
            // interactively run the analysis (i.e. the test) for numSteps
            opserr << "\nTransientAnalysis - now interactively running for "
                << numSteps << " steps..." << endln;
            opserr << "Press 'p' to pause, 'r' to resume, 's' to step, or 'e' to end the test." << endln;
            while (step <= numSteps && exitYet == false) {
                // check if we got a keyboard input
                if (_kbhit())
                    action = _getch();

                switch (action) {
                case 'r':
                    // run or resume test
                    if (!showMsg) {
                        opserr << "\nTransientAnalysis - now resuming the test..." << endln;
                        showMsg = true;
                    }
                    result = (*theTransientAnalysis)->analyze(1, dT);
                    step++;
                    break;
                case 'p':
                    // pause test
                    if (showMsg) {
                        opserr << "\nTransientAnalysis - at step "
                            << step << " - now pausing the test..." << endln;
                        showMsg = false;
                    }
                    break;
                case 's':
                case '0':
                    // 10^0 times step test
                    if (!showMsg) {
                        opserr << "\nTransientAnalysis - at step "
                            << step << " - now stepping the test..." << endln;
                        showMsg = false;
                    }
                    result = (*theTransientAnalysis)->analyze(1, dT);
                    action = 'p';
                    step++;
                    break;
                case '1':
                    // 10^1 times step test
                    if (!showMsg) {
                        opserr << "\nTransientAnalysis - at step "
                            << step << " - now 10x stepping the test..." << endln;
                        showMsg = false;
                    }
                    result = (*theTransientAnalysis)->analyze(10, dT);
                    action = 'p';
                    step += 10;
                    break;
                case '2':
                    // 10^2 times step test
                    if (!showMsg) {
                        opserr << "\nTransientAnalysis - at step "
                            << step << " - now 100x stepping the test..." << endln;
                        showMsg = false;
                    }
                    result = (*theTransientAnalysis)->analyze(100, dT);
                    action = 'p';
                    step += 100;
                    break;
                case '3':
                    // 10^3 times step test
                    if (!showMsg) {
                        opserr << "\nTransientAnalysis - at step "
                            << step << " - now 1000x stepping the test..." << endln;
                        showMsg = false;
                    }
                    result = (*theTransientAnalysis)->analyze(1000, dT);
                    action = 'p';
                    step += 1000;
                    break;
                case '4':
                    // 10^4 times step test
                    if (!showMsg) {
                        opserr << "\nTransientAnalysis - at step "
                            << step << " - now 10000x stepping the test..." << endln;
                        showMsg = false;
                    }
                    result = (*theTransientAnalysis)->analyze(10000, dT);
                    action = 'p';
                    step += 10000;
                    break;
                case 'e':
                    // end test
                    opserr << "\nTransientAnalysis - at step "
                        << step << " - now shut down." << endln;
                    (*theTransientAnalysis)->clearAll();
                    exitYet = true;
                    break;
                }
            }
        }
        else if (OPS_GetNumRemainingInputArgs() < 3) {
            opserr << "WARNING insufficient arguments for variable transient\n"
                << "Want: analyzeInteractive numSteps dt dtMin dtMax Jd\n";
            return NULL;
        }
        else {
            int Jd;
            double dtMin, dtMax;
            if (OPS_GetDoubleInput(&numdata, &dtMin) < 0) {
                opserr << "WARNING invalid analyzeInteractive dtMin\n";
                return NULL;
            }
            if (OPS_GetDoubleInput(&numdata, &dtMax) < 0) {
                opserr << "WARNING invalid analyzeInteractive dtMax\n";
                return NULL;
            }
            if (OPS_GetIntInput(&numdata, &Jd) < 0) {
                opserr << "WARNING invalid analyzeInteractive Jd\n";
                return NULL;
            }
            
            if (*theVariableTimeStepTransientAnalysis != 0) {
                
                // interactively run the analysis (i.e. the test) for numSteps
                opserr << "\nVariableTimeStepTransientAnalysis - now interactively running for "
                    << numSteps << " steps..." << endln;
                opserr << "Press 'p' to pause, 'r' to resume, 's' to step, or 'e' to end the test." << endln;
                while (step <= numSteps && exitYet == false) {
                    // check if we got a keyboard input
                    if (_kbhit())
                        action = _getch();
                    
                    switch (action) {
                    case 'r':
                        // run or resume test
                        if (!showMsg) {
                            opserr << "\nVariableTimeStepTransientAnalysis - now resuming the test..." << endln;
                            showMsg = true;
                        }
                        result = (*theVariableTimeStepTransientAnalysis)->analyze(1, dT, dtMin, dtMax, Jd);
                        step++;
                        break;
                    case 'p':
                        // pause test
                        if (showMsg) {
                            opserr << "\nVariableTimeStepTransientAnalysis - at step "
                                << step << " - now pausing the test..." << endln;
                            showMsg = false;
                        }
                        break;
                    case 's':
                    case '0':
                        // 10^0 times step test
                        if (!showMsg) {
                            opserr << "\nVariableTimeStepTransientAnalysis - at step "
                                << step << " - now stepping the test..." << endln;
                            showMsg = false;
                        }
                        result = (*theVariableTimeStepTransientAnalysis)->analyze(1, dT, dtMin, dtMax, Jd);
                        action = 'p';
                        step++;
                        break;
                    case '1':
                        // 10^1 times step test
                        if (!showMsg) {
                            opserr << "\nVariableTimeStepTransientAnalysis - at step "
                                << step << " - now 10x stepping the test..." << endln;
                            showMsg = false;
                        }
                        result = (*theVariableTimeStepTransientAnalysis)->analyze(10, dT, dtMin, dtMax, Jd);
                        action = 'p';
                        step += 10;
                        break;
                    case '2':
                        // 10^2 times step test
                        if (!showMsg) {
                            opserr << "\nVariableTimeStepTransientAnalysis - at step "
                                << step << " - now 100x stepping the test..." << endln;
                            showMsg = false;
                        }
                        result = (*theVariableTimeStepTransientAnalysis)->analyze(100, dT, dtMin, dtMax, Jd);
                        action = 'p';
                        step += 100;
                        break;
                    case '3':
                        // 10^3 times step test
                        if (!showMsg) {
                            opserr << "\nVariableTimeStepTransientAnalysis - at step "
                                << step << " - now 1000x stepping the test..." << endln;
                            showMsg = false;
                        }
                        result = (*theVariableTimeStepTransientAnalysis)->analyze(1000, dT, dtMin, dtMax, Jd);
                        action = 'p';
                        step += 1000;
                        break;
                    case '4':
                        // 10^4 times step test
                        if (!showMsg) {
                            opserr << "\nVariableTimeStepTransientAnalysis - at step "
                                << step << " - now 10000x stepping the test..." << endln;
                            showMsg = false;
                        }
                        result = (*theVariableTimeStepTransientAnalysis)->analyze(10000, dT, dtMin, dtMax, Jd);
                        action = 'p';
                        step += 10000;
                        break;
                    case 'e':
                        // end test
                        opserr << "\nVariableTimeStepTransientAnalysis - at step "
                            << step << " - now shut down." << endln;
                        (*theVariableTimeStepTransientAnalysis)->clearAll();
                        exitYet = true;
                        break;
                    }
                }
            }
            else {
                opserr << "WARNING analyzeInteractive - no variable time step transient analysis object constructed\n";
                return NULL;
            }
        }
    }
    else {
        opserr << "WARNING No Analysis type has been specified\n";
        return NULL;
    }
    
    if (result < 0) {
        opserr << "OpenSees > analyzeInteractive failed, returned: " << result << " error flag\n";
    }
    
    if (OPS_SetIntOutput(&numdata, &result, true) < 0) {
        opserr << "WARNING failed to set output\n";
        return NULL;
    }
    
    return getResults();
}


// wipe entire experiment
static PyObject* Py_opf_wipeExp(PyObject* self, PyObject* args)
{
    OPF_clearExperimentalCPs();
    OPF_clearExperimentalSignalFilters();
    OPF_clearExperimentalControls();
    OPF_clearExperimentalSetups();
    OPF_clearExperimentalSites();
    OPF_clearExperimentalTangentStiffs();
    
    // OpenSees will clean up theDomain when wipe is called
    //if (theDomain != 0)
    //    theDomain->clearAll();
    
    return getResults();
}


static PyObject* Py_opf_removeExp(PyObject* self, PyObject* args)
{
    // reset the input args
    OPS_ResetCommandLine((int)PyTuple_Size(args), 1, (const char**)args);
    
    if (OPF_removeObject() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return getResults();
}


static PyObject* Py_opf_version(PyObject* self, PyObject* args)
{
    if (OPF_version() < 0) {
        opserr << (void*)0;
        return NULL;
    }
    
    return getResults();
}


PyMethodDef* getmethodsFunc()
{
    if (methodsOpenFresco.empty()) {
        return 0;
    }
    
    return &methodsOpenFresco[0];
}


struct module_state
{
    PyObject* error;
};
#define GETSTATE(m) ((struct module_state*)PyModule_GetState(m))


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


// This is a package initialization procedure, which is called
// by Python when this package is to be added to an interpreter.
extern "C" DllExport PyObject*
OpenFrescoPy(PyObject* self, PyObject* args, Domain* domain)
{
    //theModule = module;
    theDomain = domain;
    
    // alternative way to get the module by its name
    //PyObject* moduleName = PyUnicode_FromString("opensees");
    //PyObject* theModuleAlt = PyImport_GetModule(moduleName);
    
    // beginning of OpenFresco additions
    Py_opf_addCommand("expControlPoint", &Py_opf_expControlPoint);
    Py_opf_addCommand("expSignalFilter", &Py_opf_expSignalFilter);
    Py_opf_addCommand("expControl", &Py_opf_expControl);
    Py_opf_addCommand("expSetup", &Py_opf_expSetup);
    Py_opf_addCommand("expSite", &Py_opf_expSite);
    Py_opf_addCommand("expTangentStiff", &Py_opf_expTangentStiff);
    Py_opf_addCommand("expElement", &Py_opf_expElement);
    Py_opf_addCommand("expRecorder", &Py_opf_expRecorder);
    Py_opf_addCommand("recordExp", &Py_opf_recordExp);
    Py_opf_addCommand("startLabServer", &Py_opf_startLabServer);
    Py_opf_addCommand("setupLabServer", &Py_opf_setupLabServer);
    Py_opf_addCommand("stepLabServer", &Py_opf_stepLabServer);
    Py_opf_addCommand("stopLabServer", &Py_opf_stopLabServer);
    Py_opf_addCommand("startSimAppSiteServer", &Py_opf_startSimAppSiteServer);
    Py_opf_addCommand("startSimAppElemServer", &Py_opf_startSimAppElemServer);
    Py_opf_addCommand("analyzeInteractive", &Py_ops_analyzeModelInteractive);
    Py_opf_addCommand("wipeExp", &Py_opf_wipeExp);
    Py_opf_addCommand("removeExp", &Py_opf_removeExp);
    Py_opf_addCommand("packageVersion", &Py_opf_version);
    
    PyMethodDef method = { NULL,NULL,0,NULL };
    methodsOpenFresco.push_back(method);
    
    // update the module definition
    moduledef.m_methods = getmethodsFunc();
    
    // create the Python module
    PyObject* theModule = 0;
    theModule = PyModule_Create(&moduledef);
    
    return theModule;
}
