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
//
// Description: This file contains the class definition for TclInterpreter
// TclInterpreter implements a DL_Interpreter for the Tcl language

#ifndef TclInterpreter_h
#define TclInterpreter_h

#include <tcl.h>
#include "DL_Interpreter.h"
#include "TclWrapper.h"
#include "OpenFrescoCommands.h"

class TclInterpreter : public DL_Interpreter
{
public:
    TclInterpreter(int argc, char** argv);
    virtual ~TclInterpreter();
    
    // method to run once the interpreter is set up
    virtual int run();
    
    // methods to add & remove additional commands
    virtual int addCommand(const char*, Command&);
    virtual int removeCommand(const char*);
    
    // methods for commands to parse the command line
    virtual int getNumRemainingInputArgs(void);
    virtual int getInt(int*, int numArgs);
    virtual int getDouble(double*, int numArgs);
    virtual int getDoubleList(int* size, Vector* data);
    virtual const char* getString();
    virtual const char* getStringFromAll(char* buffer, int len);
    //virtual int getStringCopy(char** stringPtr);
    virtual int evalDoubleStringExpression(const char* theExpression, double& current_val);
    virtual void resetInput(int cArg);
    
    // methods for interpreter to output results
    virtual int setInt(int*, int numArgs, bool scalar);
    virtual int setInt(std::vector<std::vector<int>>& data);
    virtual int setInt(std::map<const char*, int>& data);
    virtual int setInt(std::map<const char*, std::vector<int>>& data);
    virtual int setDouble(double*, int numArgs, bool scalar);
    virtual int setDouble(std::vector<std::vector<double>>& data);
    virtual int setDouble(std::map<const char*, double>& data);
    virtual int setDouble(std::map<const char*, std::vector<double>>& data);
    virtual int setString(const char*);
    virtual int setString(std::vector<const char*>& data);
    virtual int setString(std::vector<std::vector<const char*>>& data);
    virtual int setString(std::map<const char*, const char*>& data);
    virtual int setString(std::map<const char*, std::vector<const char*>>& data);
    
private:
    Tcl_Obj* resultPtr;
    Tcl_Obj* commandPtr;
    char buffer[1000], * args;
    int code, gotPartial, tty, length;
    int exitCode;
    Tcl_Channel inChannel, outChannel, errChannel;
    Tcl_DString argString;
    Tcl_Interp* interp;
    
    TclWrapper wrapper;
    OpenFrescoCommands cmds;
};

#endif
