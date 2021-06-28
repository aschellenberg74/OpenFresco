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
// Description: This file contains the class definition for PythonInterpreter
// PythonInterpreter implements a DL_Interpreter for the Python language

#ifndef PythonInterpreter_h
#define PythonInterpreter_h

#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif

#include "DL_Interpreter.h"
#include "PythonWrapper.h"
#include "OpenFrescoCommands.h"

class PythonInterpreter: public DL_Interpreter
{
  public:
    PythonInterpreter();
    virtual ~PythonInterpreter();

    // method to run once the interpreter is set up
    virtual int run();

    // methods to add & remove additional commands
    virtual int addCommand(const char *, Command &);
    virtual int removeCommand(const char *);

    // methods for commands to parse the command line
    virtual int getNumRemainingInputArgs(void);
    virtual int getInt(int *, int numArgs);
    virtual int getDouble(double *, int numArgs);
    virtual const char* getString();
    virtual const char* getStringFromAll(char* buffer, int len);
    virtual int getStringCopy(char **stringPtr);
    virtual void resetInput(int cArg);

    // methods for interpreters to output results
    virtual int setInt(int *, int numArgs, bool scalar);
    virtual int setDouble(double *, int numArgs, bool scalar);
    virtual int setString(const char*);

    // methods to run a command in the interpreter
    virtual int runCommand(const char*);

    // getwrapper
    PythonWrapper* getWrapper() {return &wrapper;}
    OpenFrescoCommands& getCmds() {return cmds;}
    
  private:
    PythonWrapper wrapper;
    OpenFrescoCommands cmds;
};

#endif
