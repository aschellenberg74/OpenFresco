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
**   Andreas Schellenberg (andreas.schellenberg@gmail.com)            **
**   Yoshikazu Takahashi (yos@catfish.dpri.kyoto-u.ac.jp)             **
**   Gregory L. Fenves (fenves@berkeley.edu)                          **
**   Stephen A. Mahin (mahin@berkeley.edu)                            **
**                                                                    **
** ****************************************************************** */

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 03/25
// Revision: A
//
// Description: This file contains the class definition for CosineBell_Ramp.
// It generates a ramp function that starts at startTime and ramps up from
// 0 to 1 over rampTime. It implements a cosine bell or linear ramp.

#define S_FUNCTION_NAME SFun_CosineBell_Ramp
#define S_FUNCTION_LEVEL 2

#include "simstruc.h"
#include <math.h>
#include <string.h>

// OPFConnect parameters
#define startTime(S)      ssGetSFcnParam(S,0)    // time at start of ramp
#define rampTime(S)       ssGetSFcnParam(S,1)    // duration of ramp
#define initOut(S)        ssGetSFcnParam(S,2)    // initial signal output
#define rampType(S)       ssGetSFcnParam(S,3)    // type of ramp

#define NPARAMS 4

// ====================
//  S-function methods 
// ====================

#define MDL_CHECK_PARAMETERS
#if defined(MDL_CHECK_PARAMETERS) && defined(MATLAB_MEX_FILE)
// ============================================================================
// Function: mdlCheckParameters
//    Validate the parameters to verify they are okay

static void mdlCheckParameters(SimStruct *S)
{
    /*if (!mxIsDouble(startTime(S)) || mxGetPr(startTime(S))[0] < 0) {
        ssSetErrorStatus(S,"startTime must be >= 0");
        return;
    }
    if (!mxIsDouble(rampTime(S)) || mxGetPr(rampTime(S))[0] < 0) {
        ssSetErrorStatus(S, "rampTime must be >= 0");
        return;
    }*/
}
#endif // MDL_CHECK_PARAMETERS


// ============================================================================
// Function: mdlInitializeSizes
//    The sizes information is used by Simulink to determine the S-function
//    block's characteristics (number of inputs, outputs, states, etc.).

static void mdlInitializeSizes(SimStruct *S)
{
    ssSetNumSFcnParams(S, NPARAMS);    // Number of expected parameters
#if defined(MATLAB_MEX_FILE)
    if (ssGetNumSFcnParams(S) == ssGetSFcnParamsCount(S))  {
        mdlCheckParameters(S);
        if (ssGetErrorStatus(S) != NULL)  {
            return;
        }
    } else {
        return;      // Parameter mismatch will be reported by Simulink
    }
#endif // MATLAB_MEX_FILE
        
    if (!ssSetNumInputPorts(S, 0)) return;
    
    if (!ssSetNumOutputPorts(S, 1)) return;
    ssSetOutputPortWidth(S, 0, 1);  // ramp signal
    
    ssSetNumSampleTimes(S, 1);

    /* specify the sim state compliance to be same as a built-in block */
    ssSetOperatingPointCompliance(S, USE_DEFAULT_OPERATING_POINT);
    
    /* Set this S-function as runtime thread-safe for multicore execution */
    ssSetRuntimeThreadSafetyCompliance(S, RUNTIME_THREAD_SAFETY_COMPLIANCE_TRUE);
    
    // take care when specifying exception free code - see sfuntmpl_doc.c
    //ssSetOptions(S, SS_OPTION_EXCEPTION_FREE_CODE);
    ssSetOptions(S,
        SS_OPTION_WORKS_WITH_CODE_REUSE |
        SS_OPTION_EXCEPTION_FREE_CODE |
        SS_OPTION_USE_TLC_WITH_ACCELERATOR);
}


// ============================================================================
// Function: mdlInitializeSampleTimes

static void mdlInitializeSampleTimes(SimStruct *S)
{
    ssSetSampleTime(S, 0, INHERITED_SAMPLE_TIME);
    ssSetOffsetTime(S, 0, 0.0);
    ssSetModelReferenceSampleTimeDefaultInheritance(S);
}


// ============================================================================
// Function: mdlOutputs
//    Calculate outputs

static void mdlOutputs(SimStruct *S, int_T tid)
{
    // get pointer to output signal
    real_T *ramp = ssGetOutputPortRealSignal(S,0);
    
    // get constants
    real_T pi = acos(-1.0);
    
    // get parameters
    real_T startTime = (real_T)mxGetScalar(startTime(S));
    real_T rampTime = (real_T)mxGetScalar(rampTime(S));
    real_T initOut = (real_T)mxGetScalar(initOut(S));
    int_T rampType = (real_T)mxGetScalar(rampType(S));
    
    UNUSED_ARG(tid);    // not used in single tasking mode
    
    // get current time
    real_T time = (real_T)ssGetT(S);
    
    // set output signal values
    if (time >= startTime+rampTime) {
        ramp[0] = 1.0;
    }
    else if (time >= startTime) {
        if (rampType == 1)
            ramp[0] = (1 + cos(pi * ((time - startTime) / rampTime + 1.0))) / 2.0;
        else if (rampType == 2)
            ramp[0] = (time - startTime) / rampTime;
        else
            ramp[0] = (1 + cos(pi * ((time - startTime) / rampTime + 1.0))) / 2.0;
    }
    else {
        ramp[0] = 0.0;
    }
}


// ============================================================================
// Function: mdlTerminate

static void mdlTerminate(SimStruct *S)
{
}

#ifdef  MATLAB_MEX_FILE    // Is this file being compiled as a MEX-file?
#include "simulink.c"      // MEX-file interface mechanism
#else
#include "cg_sfun.h"       // Code generation registration function
#endif
