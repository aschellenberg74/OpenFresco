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

// Written: Frank McKenna & Andreas Schellenberg
// Created: 01/21
// Revision: A
//
// Description: This file contains the class definition for TclInterpreter
// TclInterpreter implements a DL_Interpreter for the Tcl language

#ifndef TclInterpreter_h
#define TclInterpreter_h

#include <tcl.h>
//#include <OPS_Globals.h>
#include "DL_Interpreter.h"
#include "TclWrapper.h"
#include "OpenFrescoCommands.h"

class TclInterpreter: public DL_Interpreter
{
  public:
    TclInterpreter(int argc, char **argv);
    virtual ~TclInterpreter();

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
    virtual int getStringCopy(char **stringPtr);
    virtual void resetInput(int cArg);

    // methods for interpreters to output results
    virtual int setInt(int *, int numArgs, bool scalar);
    virtual int setDouble(double *, int numArgs, bool scalar);
    virtual int setString(const char*);
    
  private:
    Tcl_Obj *resultPtr;
    Tcl_Obj *commandPtr;
    char buffer[1000], *args;
    int code, gotPartial, tty, length;
    int exitCode;
    Tcl_Channel inChannel, outChannel, errChannel;
    Tcl_DString argString;
    Tcl_Interp *interp;

    TclWrapper wrapper;
    OpenFrescoCommands cmds;
};

#endif
