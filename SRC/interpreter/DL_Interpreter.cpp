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

// Description: This file contains the class definition for DL_Interpreter
// DL_Interpreter is the abstract base class for dynamic language interpreters
// concrete examples being TclInterpreter, PythonInterpreter, MatlabInterpreter,...
//
// written: fmk

#include "DL_Interpreter.h"

DL_Interpreter  *ops_TheActiveInterpreter = 0;


DL_Interpreter::DL_Interpreter()
{
    // does nothing
}


DL_Interpreter::~DL_Interpreter()
{
    // does nothing
}


int DL_Interpreter::addCommand(const char *, Command &)
{
    return -1;
}


int DL_Interpreter::removeCommand(const char *)
{
    return -1;
}


int DL_Interpreter::getNumRemainingInputArgs(void)
{
    return -1;
}


int DL_Interpreter::getInt(int *, int numArgs)
{
    return -1;
}


int DL_Interpreter::getDouble(double *, int numArgs)
{
    return -1;
}


const char* DL_Interpreter::getString()
{
    return 0;
}


int DL_Interpreter::getStringCopy(char **stringPtr)
{
    return -1;
}


void DL_Interpreter::resetInput(int cArg)
{
    // does nothing
}


int DL_Interpreter::setInt(int *, int numArgs, bool scalar)
{
    return -1;
}


int DL_Interpreter::setDouble(double *, int numArgs, bool scalar)
{
    return -1;
}


int DL_Interpreter::setString(const char*)
{
    return -1;
}


int DL_Interpreter::runCommand(const char*)
{
    return -1;
}
