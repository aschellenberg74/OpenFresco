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

#ifndef DL_Interpreter_h
#define DL_Interpreter_h

class Command;

class DL_Interpreter
{
public:
	DL_Interpreter();
	virtual ~DL_Interpreter();
	
	// method to run once the interpreter is set up
	virtual int run() = 0;
	
	// methods to add & remove additional commands
	virtual int addCommand(const char*, Command&);
	virtual int removeCommand(const char*);
	
	// methods for commands to parse the command line
	virtual int getNumRemainingInputArgs(void);
	virtual int getInt(int*, int numArgs);
	virtual int getDouble(double*, int numArgs);
	virtual const char* getString();
	virtual int getStringCopy(char** stringPtr);
	virtual void resetInput(int cArg);
	
	// methods for interpreters to output results
	virtual int setInt(int*, int numArgs, bool scalar);
	virtual int setDouble(double*, int numArgs, bool scalar);
	virtual int setString(const char*);
		
	// methods to run a command in the interpreter
	virtual int runCommand(const char*);
	
private:

};

#endif
