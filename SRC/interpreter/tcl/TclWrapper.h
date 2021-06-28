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
// Description: A tcl wrapper for OpenFresco commands

#ifndef TclWrapper_h
#define TclWrapper_h

#include <tcl.h>
#include <OPS_Globals.h>

class TclWrapper
{
public:
    
    TclWrapper();
    ~TclWrapper();
    
    // reset command line
    void resetCommandLine(int nArgs, int cArg, TCL_Char** argv);
    void resetCommandLine(int cArg);
    
    // wrapper commands
    void addOpenFrescoCommands(Tcl_Interp* interp);
    void addCommand(Tcl_Interp* interp, const char* name, Tcl_CmdProc* proc);
    
    // get command line arguments
    TCL_Char** getCurrentArgv() { return currentArgv; }
    int getCurrentArg() const { return currentArg; }
    int getNumberArgs() const { return numberArgs; }
    void incrCurrentArg() { currentArg++; }
    
    // set outputs
    void setOutputs(Tcl_Interp* interp, int* data, int numArgs);
    void setOutputs(Tcl_Interp* interp, double* data, int numArgs);
    void setOutputs(Tcl_Interp* interp, const char* str);
    
private:
    
    // command line arguments
    TCL_Char** currentArgv;
    int currentArg;
    int numberArgs;
    
};

#endif
