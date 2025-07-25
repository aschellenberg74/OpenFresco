/*
 * tclMain.c --
 *
 *	Main program for Tcl shells and other Tcl-based applications.
 *
 * Copyright (c) 1988-1994 The Regents of the University of California.
 * Copyright (c) 1994-1997 Sun Microsystems, Inc.
 *
 * See Tcl/TK License Terms for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: tclMain.cpp,v 1.50 2010-04-23 23:01:14 fmk Exp $

Tcl/Tk License Terms
This software is copyrighted by the Regents of the University of
California, Sun Microsystems, Inc., Scriptics Corporation, ActiveState
Corporation and other parties.  The following terms apply to all files
associated with the software unless explicitly disclaimed in
individual files.

The authors hereby grant permission to use, copy, modify, distribute,
and license this software and its documentation for any purpose, provided
that existing copyright notices are retained in all copies and that this
notice is included verbatim in any distributions. No written agreement,
license, or royalty fee is required for any of the authorized uses.
Modifications to this software may be copyrighted by their authors
and need not follow the licensing terms described here, provided that
the new terms are clearly indicated on the first page of each file where
they apply.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY
DERIVATIVES THEREOF, EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE
IS PROVIDED ON AN "AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE
NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
MODIFICATIONS.

GOVERNMENT USE: If you are acquiring this software on behalf of the
U.S. government, the Government shall have only "Restricted Rights"
in the software and related documentation as defined in the Federal
Acquisition Regulations (FARs) in Clause 52.227.19 (c) (2).  If you
are acquiring the software on behalf of the Department of Defense, the
software shall be classified as "Commercial Computer Software" and the
Government shall have only "Restricted Rights" as defined in Clause
252.227-7013 (c) (1) of DFARs.  Notwithstanding the foregoing, the
authors grant the U.S. Government and others acting in its behalf
permission to use and distribute the software in accordance with the
terms specified in this license.

 */

 /*                            MODIFIED FOR                            */

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

 // Modified: Frank McKenna & Andreas Schellenberg
 // Created: 10/06
 // Revision: A

#include "TclInterpreter.h"
#include <string.h>
#include <StandardStream.h>
#include <FrescoGlobals.h>

StandardStream sserr;
OPS_Stream* opserrPtr = &sserr;

#ifndef _WIN32
#include <unistd.h>
#endif

extern "C" {
#include <tcl.h>
#include <tclDecls.h>
#ifdef _TCL85
#define TclFormatInt(buf, n)   sprintf((buf),"%ld", (long)(n))
#else
    EXTERN int  TclFormatInt _ANSI_ARGS_((char* buffer, long n));
#endif
    EXTERN int  TclObjCommandComplete _ANSI_ARGS_((Tcl_Obj* cmdPtr));
}

// extern int addOpenFrescoCommands(Tcl_Interp *interp);
int Tcl_AppInit _ANSI_ARGS_((Tcl_Interp* interp));

# undef TCL_STORAGE_CLASS
# define TCL_STORAGE_CLASS DLLEXPORT


/*
*----------------------------------------------------------------------
*
* The following code ensures that tclLink.c is linked whenever
* Tcl is linked.  Without this code there's no reference to the
* code in that file from anywhere in Tcl, so it may not be
* linked into the application.
*
*----------------------------------------------------------------------
*/
#ifdef _TCL85
int (*tclDummyLinkVarPtr)(Tcl_Interp* interp, const char* a,
    char* b, int c) = Tcl_LinkVar;
#elif _TCL84
int (*tclDummyLinkVarPtr)(Tcl_Interp* interp, const char* a,
    char* b, int c) = Tcl_LinkVar;
#else
int (*tclDummyLinkVarPtr)(Tcl_Interp* interp, char* a,
    char* b, int c) = Tcl_LinkVar;
#endif


/*
*----------------------------------------------------------------------
*
* Declarations for various library procedures and variables (don't want
* to include tclPort.h here, because people might copy this file out of
* the Tcl source directory to make their own modified versions).
* Note:  "exit" should really be declared here, but there's no way to
* declare it without causing conflicts with other definitions elsewher
* on some systems, so it's better just to leave it out.
*
*----------------------------------------------------------------------
*/
#ifdef _WIN32
extern "C" int	isatty _ANSI_ARGS_((int fd));
//extern "C" char* strcpy _ANSI_ARGS_((char* dst, CONST char* src)) throw();
#endif
static char* tclStartupScriptFileName = NULL;


/*
 *----------------------------------------------------------------------
 *
 * TclSetStartupScriptFileName --
 *
 *	Primes the startup script file name, used to override the
 *      command line processing.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	This procedure initializes the file name of the Tcl script to
 *      run at startup.
 *
 *----------------------------------------------------------------------
 */
void TclSetStartupScriptFileName(char* fileName)
{
    tclStartupScriptFileName = new char[strlen(fileName) + 1];
    strcpy(tclStartupScriptFileName, fileName);
}


/*
 *----------------------------------------------------------------------
 *
 * TclGetStartupScriptFileName --
 *
 *	Gets the startup script file name, used to override the
 *      command line processing.
 *
 * Results:
 *	The startup script file name, NULL if none has been set.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */
char* TclGetStartupScriptFileName()
{
    return tclStartupScriptFileName;
}


/*
 *----------------------------------------------------------------------
 *
 * Tcl_AppInit --
 *
 *	This procedure performs application-specific initialization.
 *	Most applications, especially those that incorporate additional
 *	packages, will have their own version of this procedure.
 *
 * Results:
 *	Returns a standard Tcl completion code, and leaves an error
 *	message in the interp's result if an error occurs.
 *
 * Side effects:
 *	Depends on the startup script.
 *
 *----------------------------------------------------------------------
 */
int Tcl_AppInit(Tcl_Interp* interp)
{
    if (Tcl_Init(interp) == TCL_ERROR) {
        return TCL_ERROR;
    }
    
    /*
    * Call the init procedures for included packages.  Each call should
    * look like this:
    *
    * if (Mod_Init(interp) == TCL_ERROR) {
    *     return TCL_ERROR;
    * }
    *
    * where "Mod" is the name of the module.
    */
    
    /*
    * Call Tcl_CreateCommand for application-specific commands, if
    * they weren't already created by the init procedures called above.
    */
    //if (OpenFrescoAppInit(interp) < 0)
    //    return TCL_ERROR;
    
    /*
    * Specify a user-specific startup file to invoke if the application
    * is run interactively.  Typically the startup file is "~/.apprc"
    * where "app" is the name of the application.  If this line is deleted
    * then no user-specific startup file will be run under any conditions.
    */
    Tcl_SetVar(interp, "tcl_rcFileName", "~/.tclshrc", TCL_GLOBAL_ONLY);
    
    return TCL_OK;
}


TclInterpreter::TclInterpreter(int argc, char** argv)
    : wrapper(), resultPtr(0), commandPtr(0),
    code(0), gotPartial(0), length(0), exitCode(0),
    inChannel(0), outChannel(0), errChannel(0),
    cmds(this)
{
    fprintf(stderr, "\n\n");
    fprintf(stderr, "\t OpenFresco -- Open Framework for Experimental Setup and Control\n");
    fprintf(stderr, "\t               Version %s %s              \n\n", OPF_VERSION, WIN_ARCH);
    fprintf(stderr, "\t Copyright (c) 2006 The Regents of the University of California \n");
    fprintf(stderr, "\t                       All Rights Reserved                      \n\n\n");
    
    Tcl_FindExecutable(argv[0]);
    interp = Tcl_CreateInterp();
    
#ifdef TCL_MEM_DEBUG
    Tcl_InitMemory(interp);
#endif
    
    // Make command-line arguments available in the Tcl variables "argc"
    // and "argv".  If the first argument doesn't start with a "-" then
    // strip it off and use it as the name of a script file to process.
    if (tclStartupScriptFileName == NULL) {
        if ((argc > 1) && (argv[1][0] != '-')) {
            tclStartupScriptFileName = argv[1];
            argc--;
            argv++;
        }
    }
    
    args = Tcl_Merge(argc - 1, argv + 1);
    Tcl_ExternalToUtfDString(NULL, args, -1, &argString);
    Tcl_SetVar(interp, "argv", Tcl_DStringValue(&argString), TCL_GLOBAL_ONLY);
    Tcl_DStringFree(&argString);
    ckfree(args);
    
    if (tclStartupScriptFileName == NULL) {
        Tcl_ExternalToUtfDString(NULL, argv[0], -1, &argString);
    }
    else {
        tclStartupScriptFileName = Tcl_ExternalToUtfDString(NULL,
            tclStartupScriptFileName, -1, &argString);
    }
    
    TclFormatInt(buffer, argc - 1);
    Tcl_SetVar(interp, "argc", buffer, TCL_GLOBAL_ONLY);
    Tcl_SetVar(interp, "argv0", Tcl_DStringValue(&argString), TCL_GLOBAL_ONLY);
    
    // Set the "tcl_interactive" variable.
    tty = isatty(0);
    char one[2] = "1";
    char zero[2] = "0";
    
    Tcl_SetVar(interp, "tcl_interactive",
        ((tclStartupScriptFileName == NULL) && tty) ? one : zero,
        TCL_GLOBAL_ONLY);
    
    // Invoke application-specific initialization.
#ifndef TCL_LOCAL_APPINIT
#define TCL_LOCAL_APPINIT Tcl_AppInit    
#endif
    
    if ((*Tcl_AppInit)(interp) != TCL_OK) {
        errChannel = Tcl_GetStdChannel(TCL_STDERR);
        if (errChannel) {
            Tcl_WriteChars(errChannel,
                "application-specific initialization failed: ", -1);
            Tcl_WriteObj(errChannel, Tcl_GetObjResult(interp));
            Tcl_WriteChars(errChannel, "\n", 1);
        }
    }
    
    wrapper.addOpenFrescoCommands(interp);
}


TclInterpreter::~TclInterpreter()
{
    if (commandPtr != NULL) {
        Tcl_DecrRefCount(commandPtr);
    }
    
    // Rather than calling exit, invoke the "exit" command so that
    // users can replace "exit" with some other command to do additional
    // cleanup on exit.  The Tcl_Eval call should never return.
    Tcl_Eval(interp, buffer);
    Tcl_Eval(interp, "quit");
}


int TclInterpreter::run()
{
    // If a script file was specified then just source that file and quit.
    if (tclStartupScriptFileName != NULL) {
        
        code = Tcl_EvalFile(interp, tclStartupScriptFileName);
        
        if (code != TCL_OK) {
            errChannel = Tcl_GetStdChannel(TCL_STDERR);
            if (errChannel) {
                // The following statement guarantees that the errorInfo
                // variable is set properly.
                Tcl_AddErrorInfo(interp, "");
                Tcl_WriteObj(errChannel, Tcl_GetVar2Ex(interp, "errorInfo",
                    NULL, TCL_GLOBAL_ONLY));
                Tcl_WriteChars(errChannel, "\n", 1);
            }
            exitCode = 1;
        }
        return 0;
    }
    else {
        // const char *pwd = getInterpPWD(interp);
        //simulationInfo.start();
        //simulationInfo.addInputFile(tclStartupScriptFileName, pwd);
        
        // We're running interactively.  Source a user-specific startup
        // file if the application specified one and if the file exists.
        Tcl_DStringFree(&argString);
        Tcl_SourceRCFile(interp);
        
        // Process commands from stdin until there's an end-of-file.  Note
        // that we need to fetch the standard channels again after every
        // eval, since they may have been changed.
        
        //if (simulationInfoOutputFilename != 0) {
        //simulationInfo.start();
        //}
        commandPtr = Tcl_NewObj();
        Tcl_IncrRefCount(commandPtr);
        
        inChannel = Tcl_GetStdChannel(TCL_STDIN);
        outChannel = Tcl_GetStdChannel(TCL_STDOUT);
        gotPartial = 0;
        while (1) {
            if (tty) {
                Tcl_Obj* promptCmdPtr;
                
                char one[12] = "tcl_prompt1";
                char two[12] = "tcl_prompt2";
                promptCmdPtr = Tcl_GetVar2Ex(interp,
                    (gotPartial ? one : two),
                    NULL, TCL_GLOBAL_ONLY);
                if (promptCmdPtr == NULL) {
                defaultPrompt:
                    if (!gotPartial && outChannel) {
                        Tcl_WriteChars(outChannel, "OpenFresco > ", 13);
                    }
                }
                else {
                    code = Tcl_EvalObjEx(interp, promptCmdPtr, 0);
                    
                    inChannel = Tcl_GetStdChannel(TCL_STDIN);
                    outChannel = Tcl_GetStdChannel(TCL_STDOUT);
                    errChannel = Tcl_GetStdChannel(TCL_STDERR);
                    if (code != TCL_OK) {
                        if (errChannel) {
                            Tcl_WriteObj(errChannel, Tcl_GetObjResult(interp));
                            Tcl_WriteChars(errChannel, "\n", 1);
                        }
                        Tcl_AddErrorInfo(interp,
                            "\n    (script that generates prompt)");
                        goto defaultPrompt;
                    }
                }
                if (outChannel) {
                    Tcl_Flush(outChannel);
                }
            }
            if (!inChannel) {
                return 0;
            }
            length = Tcl_GetsObj(inChannel, commandPtr);
            if (length < 0) {
                return 0;
            }
            if ((length == 0) && Tcl_Eof(inChannel) && (!gotPartial)) {
                return 0;
            }
            
            // Add the newline removed by Tcl_GetsObj back to the string.
            Tcl_AppendToObj(commandPtr, "\n", 1);
            if (!TclObjCommandComplete(commandPtr)) {
                gotPartial = 1;
                continue;
            }
            
            gotPartial = 0;
            code = Tcl_RecordAndEvalObj(interp, commandPtr, 0);
            inChannel = Tcl_GetStdChannel(TCL_STDIN);
            outChannel = Tcl_GetStdChannel(TCL_STDOUT);
            errChannel = Tcl_GetStdChannel(TCL_STDERR);
            Tcl_DecrRefCount(commandPtr);
            commandPtr = Tcl_NewObj();
            Tcl_IncrRefCount(commandPtr);
            if (code != TCL_OK) {
                if (errChannel) {
                    Tcl_WriteObj(errChannel, Tcl_GetObjResult(interp));
                    Tcl_WriteChars(errChannel, "\n", 1);
                }
            }
            else if (tty) {
                resultPtr = Tcl_GetObjResult(interp);
                Tcl_GetStringFromObj(resultPtr, &length);
                if ((length > 0) && outChannel) {
                    Tcl_WriteObj(outChannel, resultPtr);
                    Tcl_WriteChars(outChannel, "\n", 1);
                }
            }
#ifdef TCL_MEM_DEBUG
            if (tclMemDumpFileName != NULL) {
                Tcl_DecrRefCount(commandPtr);
                Tcl_DeleteInterp(interp);
                Tcl_Exit(0);
            }
#endif
        }
    }
    
    return 0;
}


int TclInterpreter::addCommand(const char*, Command&)
{
    return -1;
}


int TclInterpreter::removeCommand(const char*)
{
    return -1;
}


int TclInterpreter::getNumRemainingInputArgs(void)
{
    return wrapper.getNumberArgs() - wrapper.getCurrentArg();
}


int TclInterpreter::getInt(int* data, int numArgs)
{
    if ((wrapper.getNumberArgs() - wrapper.getCurrentArg()) < numArgs) {
        return -1;
    }
    
    for (int i = 0; i < numArgs; i++) {
        if (Tcl_GetInt(interp, wrapper.getCurrentArgv()[wrapper.getCurrentArg()], &data[i]) != TCL_OK) {
            return -1;
        }
        else {
            wrapper.incrCurrentArg();
        }
    }
    
    return 0;
}


int TclInterpreter::getDouble(double* data, int numArgs)
{
    if ((wrapper.getNumberArgs() - wrapper.getCurrentArg()) < numArgs) {
        return -1;
    }
    
    for (int i = 0; i < numArgs; i++) {
        if (Tcl_GetDouble(interp, wrapper.getCurrentArgv()[wrapper.getCurrentArg()], &data[i]) != TCL_OK) {
            return -1;
        }
        else {
            wrapper.incrCurrentArg();
        }
    }
    
    return 0;
}


int TclInterpreter::getDoubleList(int* size, Vector* data)
{
    TCL_Char** strings;
    
    if (Tcl_SplitList(interp, wrapper.getCurrentArgv()[wrapper.getCurrentArg()],
        size, &strings) != TCL_OK) {
        opserr << "TclInterpreter::getDoubleList error: problem splitting list "
            << wrapper.getCurrentArgv()[wrapper.getCurrentArg()] << " \n";
        return -1;
    }
    
    data->resize(*size);
    for (int i = 0; i < *size; i++) {
        double value;
        if (Tcl_GetDouble(interp, strings[i], &value) != TCL_OK) {
            opserr << "TclInterpreter::getDoubleList error: problem reading data value "
                << strings[i] << " \n";
            // free up the array of strings .. see tcl man pages as to why
            Tcl_Free((char*)strings);
            return -1;
        }
        (*data)(i) = value;
    }
    // free up the array of strings .. see tcl man pages as to why
    Tcl_Free((char*)strings);
    
    wrapper.incrCurrentArg();
    
    return 0;
}


const char* TclInterpreter::getString()
{
    if (wrapper.getCurrentArg() >= wrapper.getNumberArgs()) {
        return 0;
    }
    
    const char* res = wrapper.getCurrentArgv()[wrapper.getCurrentArg()];
    wrapper.incrCurrentArg();
    
    return res;
}


const char* TclInterpreter::getStringFromAll(char* buffer, int len)
{
    return this->getString(); // everything is a string in Tcl
}


/*int TclInterpreter::getStringCopy(char** stringPtr)
{
    return -1;
}*/


int TclInterpreter::evalDoubleStringExpression(
    const char* theExpression, double& current_val) {
    
    if (Tcl_ExprDouble(interp, theExpression, &current_val) != TCL_OK) {
        opserr << "TclInterpreter::evalDoubleStringExpression -- expression \"" << theExpression;
        opserr << "\" caused error:" << endln << Tcl_GetStringResult(interp) << endln;
        return -1;
    }
    
    return 0;
}


void TclInterpreter::resetInput(int cArg)
{
    wrapper.resetCommandLine(cArg);
}


int TclInterpreter::setInt(int* data, int numArgs, bool scalar)
{
    wrapper.setOutputs(interp, data, numArgs);
    
    return 0;
}


int TclInterpreter::setInt(std::vector<std::vector<int>>& data)
{
    wrapper.setOutputs(interp, data);

    return 0;
}


int TclInterpreter::setInt(std::map<const char*, int>& data)
{
    wrapper.setOutputs(interp, data);

    return 0;
}


int TclInterpreter::setInt(std::map<const char*, std::vector<int>>& data)
{
    wrapper.setOutputs(interp, data);

    return 0;
}


int TclInterpreter::setDouble(double* data, int numArgs, bool scalar)
{
    wrapper.setOutputs(interp, data, numArgs);
    
    return 0;
}


int TclInterpreter::setDouble(std::vector<std::vector<double>>& data)
{
    wrapper.setOutputs(interp, data);

    return 0;
}


int TclInterpreter::setDouble(std::map<const char*, double>& data)
{
    wrapper.setOutputs(interp, data);

    return 0;
}


int TclInterpreter::setDouble(std::map<const char*, std::vector<double>>& data)
{
    wrapper.setOutputs(interp, data);

    return 0;
}


int TclInterpreter::setString(const char* str)
{
    wrapper.setOutputs(interp, str);
    
    return 0;
}


int TclInterpreter::setString(std::vector<const char*>& data)
{
    wrapper.setOutputs(interp, data);

    return 0;
}


int TclInterpreter::setString(std::vector<std::vector<const char*>>& data)
{
    wrapper.setOutputs(interp, data);

    return 0;
}


int TclInterpreter::setString(std::map<const char*, const char*>& data)
{
    wrapper.setOutputs(interp, data);

    return 0;
}


int TclInterpreter::setString(std::map<const char*, std::vector<const char*>>& data)
{
    wrapper.setOutputs(interp, data);

    return 0;
}
