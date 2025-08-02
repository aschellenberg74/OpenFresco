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
// Description: this is the main for the PythonInterpreter

#define PY_SSIZE_T_CLEAN
#include <FrescoGlobals.h>
#include <Python.h>
#include <iostream>
#include "PythonInterpreter.h"

PyMODINIT_FUNC
PyInit_openfresco(void);

int main(int argc, char *argv[]) {
    // find python libraries
    wchar_t *program = Py_DecodeLocale(argv[0], NULL);
    if (program == NULL) {
        fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
        exit(1);
    }
    Py_SetProgramName(program);
    
    // print OpenFresco information
    fprintf(stderr, "\n\n");
    fprintf(stderr, "\t OpenFresco -- Open Framework for Experimental Setup and Control\n");
    fprintf(stderr, "\t               Version %s %s              \n\n", OPF_VERSION, WIN_ARCH);
    fprintf(stderr, "\t Copyright (c) 2006 The Regents of the University of California \n");
    fprintf(stderr, "\t                       All Rights Reserved                      \n\n\n");
    
    // convert argv
    wchar_t **wargv = new wchar_t *[argc];
    for (int i = 0; i < argc; ++i) {
        wargv[i] = Py_DecodeLocale(argv[i], NULL);
    }
    
    // import opensees module
    PyImport_AppendInittab("openfresco", &PyInit_openfresco);
    
    // call main python function
    int ret = Py_Main(argc, wargv);
    
    // free memories
    PyMem_RawFree(program);
    for (int i = 0; i < argc; ++i) {
        PyMem_RawFree(wargv[i]);
    }
    delete[] wargv;
    return ret;
}
