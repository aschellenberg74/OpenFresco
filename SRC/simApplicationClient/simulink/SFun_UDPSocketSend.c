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
// Created: 01/23
// Revision: A
//
// Description: This file contains the Simulink s-function for 
// the sending end of the udp_socket.c.

#define S_FUNCTION_NAME SFun_UDPSocketSend
#define S_FUNCTION_LEVEL 2

#include "simstruc.h"
#include <math.h>
#include <string.h>

// functions defined in udp_socket.c
void udp_setupconnectionserver(unsigned int *port, int *socketID);
void udp_setupconnectionclient(unsigned int *port, const char machineInetAddr[], int *lengthInet, int *socketID);
void udp_closeconnection(int *socketID, int *ierr);
void udp_senddata(const int *socketID, int *dataTypeSize, char data[], int *lenData, int *ierr);

// generic client parameters
#define ipAddr(S)   ssGetSFcnParam(S,0)    // ip address of server
#define ipPort(S)   ssGetSFcnParam(S,1)    // ip port of server

#define NPARAMS 2

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
    if (!mxIsChar(ipAddr(S)))  {
        ssSetErrorStatus(S,"ipAddr must be a string");
        return;
    }
    if (!mxIsDouble(ipPort(S)) || mxGetPr(ipPort(S))[0] <= 0)  {
        ssSetErrorStatus(S,"ipPort must be a positive nonzero value");
        return;
    }
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

    ssSetNumContStates(S, 0);
    ssSetNumDiscStates(S, 0);

    if (!ssSetNumInputPorts(S, 1)) return;
    ssSetInputPortDataType(S, 0, DYNAMICALLY_TYPED);
    ssSetInputPortWidth(S, 0, DYNAMICALLY_SIZED);
    ssSetInputPortDirectFeedThrough(S, 0, 1);

    if (!ssSetNumOutputPorts(S, 0)) return;

    ssSetNumSampleTimes(S, 1);
    ssSetNumDWork(S, 1);
    ssSetNumRWork(S, 0);
    ssSetNumIWork(S, 0);
    ssSetNumPWork(S, 0);
    ssSetNumModes(S, 0);
    ssSetNumNonsampledZCs(S, 0);

    // allocate memory for socketID
    ssSetDWorkWidth(S, 0, 1);
    ssSetDWorkDataType(S, 0, SS_INT32);

    // take care when specifying exception free code - see sfuntmpl_doc.c
    ssSetOptions(S, SS_OPTION_EXCEPTION_FREE_CODE);
}


// ============================================================================
// Function: mdlInitializeSampleTimes

static void mdlInitializeSampleTimes(SimStruct *S)
{
    ssSetSampleTime(S, 0, INHERITED_SAMPLE_TIME);
    ssSetOffsetTime(S, 0, 0.0);
}


#define MDL_START
// ============================================================================
// Function: mdlStart
//    This function is called once at start of model execution. If you
//    have states that should be initialized once, this is the place
//    to do it.

static void mdlStart(SimStruct *S)
{
    static char errMsg[80];
    char_T *ipAddr;
    uint_T ipPort;
    int_T nleft, sizeAddr;

    // get and initialize parameters
    int_T *socketID = (int_T*)ssGetDWork(S,0);
    nleft = ssGetInputPortWidth(S,0);
    socketID[0] = -1;

    // setup the connection
    if (mxIsEmpty(ipAddr(S)))  {
        ipPort = (int_T)mxGetScalar(ipPort(S));
        udp_setupconnectionserver(&ipPort, socketID);
        if (socketID[0] < 0)  {
            sprintf(errMsg, "Failed to setup connection with client: %d", socketID[0]);
            ssSetErrorStatus(S, errMsg);
            return;
        }
    }
    else  {
        ipAddr = mxArrayToString(ipAddr(S));
        sizeAddr = (int_T)mxGetN(ipAddr(S)) + 1;
        ipPort = (int_T)mxGetScalar(ipPort(S));
        udp_setupconnectionclient(&ipPort, ipAddr, &sizeAddr, socketID);
        mxFree(ipAddr);
        if (socketID[0] < 0)  {
            sprintf(errMsg, "Failed to setup connection with server: %d", socketID[0]);
            ssSetErrorStatus(S, errMsg);
            return;
        }
    }
}

  
// ============================================================================
// Function: mdlOutputs
//    Calculate outputs

static void mdlOutputs(SimStruct *S, int_T tid)
{
    InputPtrsType dataPtrs = ssGetInputPortSignalPtrs(S,0);

    int_T ierr, nleft, dataTypeSize;
    char_T *gMsg;

    // get parameters
    int_T *socketID = (int_T*)ssGetDWork(S,0);
    nleft = ssGetInputPortWidth(S,0);

    UNUSED_ARG(tid);    // not used in single tasking mode

    // switch according to data type
    if (ssGetInputPortDataType(S,0) == SS_DOUBLE)  {
        real64_T *data = (real64_T *)(*dataPtrs);
        gMsg = (char_T *)data;
        dataTypeSize = sizeof(real64_T);
    }
    else if (ssGetInputPortDataType(S,0) == SS_SINGLE)  {
        real32_T *data = (real32_T *)(*dataPtrs);
        gMsg = (char_T *)data;
        dataTypeSize = sizeof(real32_T);
    }
    else if (ssGetInputPortDataType(S,0) == SS_INT32)  {
        int32_T *data = (int32_T *)(*dataPtrs);
        gMsg = (char_T *)data;
        dataTypeSize = sizeof(int32_T);
    }
    else if (ssGetInputPortDataType(S,0) == SS_INT16)  {
        int16_T *data = (int16_T *)(*dataPtrs);
        gMsg = (char_T *)data;
        dataTypeSize = sizeof(int16_T);
    }
    else if (ssGetInputPortDataType(S,0) == SS_INT8)  {
        int8_T *data = (int8_T *)(*dataPtrs);
        gMsg = (char_T *)data;
        dataTypeSize = sizeof(int8_T);
    }
    else  {
        ssSetErrorStatus(S,"Data type is not supported");
        return;
    }

    // send the data
    udp_senddata(socketID, &dataTypeSize, gMsg, &nleft, &ierr);
}


// ============================================================================
// Function: mdlTerminate

static void mdlTerminate(SimStruct *S)
{
    int_T ierr;

    // get socketID
    int_T *socketID = (int_T*)ssGetDWork(S,0);

    udp_closeconnection(socketID, &ierr);
}

#ifdef  MATLAB_MEX_FILE    // Is this file being compiled as a MEX-file?
#include "simulink.c"      // MEX-file interface mechanism
#else
#include "cg_sfun.h"       // Code generation registration function
#endif
