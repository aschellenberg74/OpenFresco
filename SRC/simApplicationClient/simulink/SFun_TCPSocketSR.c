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

// Written: Chin-Ta (Kiida) Lai (u93132@gmail.com)
// Modified from: SFun_TCPSocketRecv.c and SFun_TCPSocketSend.c
// Created: Mar. 15, 2023
// Revision: A
//
// Description: This file contains the Simulink s-function for 
// sending and then receiving when Simulink side worked as a client
// 
// 1. This function also features with timeout
// 2. When the send/recv works together in a single function/block,
//    only one TCP/IP connection will be generated when initializing.
// 
// This function has been tested and validated with python and OpenSEES tcl
// tcp/ip servers

#define S_FUNCTION_NAME SFun_TCPSocketSR
#define S_FUNCTION_LEVEL 2

#include "simstruc.h"
#include <math.h>
#include <string.h>

// functions defined in tcp_socket.c
void tcp_setupconnectionserver(unsigned int *port, int *socketID);
void tcp_setupconnectionclient(unsigned int *port, const char machineInetAddr[], int *lengthInet, int *socketID);
void tcp_closeconnection(int *socketID, int *ierr);
void tcp_senddata(const int *socketID, int *dataTypeSize, char data[], int *lenData, int *ierr);
void tcp_recvdata_timeout(const int *socketID, int *dataTypeSize, char data[], int *lenData, int *ierr, double *timeout);

// generic client parameters
#define ipAddr(S)    ssGetSFcnParam(S,0)    // ip address of server
#define ipPort(S)    ssGetSFcnParam(S,1)    // ip port of server
#define sendWidth(S) ssGetSFcnParam(S,2)    // number of received data points
#define dataType(S)  ssGetSFcnParam(S,3)    // receive vector data type
#define recvWidth(S) ssGetSFcnParam(S,4)    // number of received data points
#define timeout(S)   ssGetSFcnParam(S,5)    // number of received data points

#define NPARAMS 6

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
    if (!mxIsChar(dataType(S)))  {
        ssSetErrorStatus(S,"dataType must be a string");
        return;
    }
    if (!mxIsDouble(recvWidth(S)) || mxGetPr(recvWidth(S))[0] <= 0)  {
        ssSetErrorStatus(S,"Receive data points must be a positive nonzero value");
        return;
    }
    
    if (!mxIsDouble(timeout(S)))  {
        ssSetErrorStatus(S,"Timout is a number with its unit in second");
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
    char_T *dataType;
    int_T sendWidth, recvWidth;
    
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
    
    sendWidth  = (int_T)mxGetScalar(sendWidth(S));
    
    if (!ssSetNumInputPorts(S, 1)) return;
    ssSetInputPortDataType(S, 0, DYNAMICALLY_TYPED);
    ssSetInputPortWidth(S, 0, sendWidth);
    ssSetInputPortDirectFeedThrough(S, 0, 1);

    if (!ssSetNumOutputPorts(S, 1)) return;
    // switch according to data type
    dataType = mxArrayToString(dataType(S));
    recvWidth  = (int_T)mxGetScalar(recvWidth(S));
    
    if (strcmp(dataType,"double") == 0)  {
        ssSetOutputPortDataType(S, 0, SS_DOUBLE);
    }
    else if (strcmp(dataType,"single") == 0)  {
        ssSetOutputPortDataType(S, 0, SS_SINGLE);
    }
    else if (strcmp(dataType,"int32") == 0)  {
        ssSetOutputPortDataType(S, 0, SS_INT32);
    }
    else if (strcmp(dataType,"int16") == 0)  {
        ssSetOutputPortDataType(S, 0, SS_INT16);
    }
    else if (strcmp(dataType,"int8") == 0)  {
        ssSetOutputPortDataType(S, 0, SS_INT8);
    }
    else  {
        ssSetErrorStatus(S,"Data type is not supported");
        return;
    }
    ssSetOutputPortWidth(S, 0, recvWidth);

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
        tcp_setupconnectionserver(&ipPort, socketID);
        if (socketID[0] < 0)  {
            ssSetErrorStatus(S,"Failed to setup connection with client");
            return;
        }
    }
    else  {
        ipAddr = mxArrayToString(ipAddr(S));
        sizeAddr = (int_T)mxGetN(ipAddr(S)) + 1;
        ipPort = (int_T)mxGetScalar(ipPort(S));
        tcp_setupconnectionclient(&ipPort, ipAddr, &sizeAddr, socketID);
        mxFree(ipAddr);
        if (socketID[0] < 0)  {
            ssSetErrorStatus(S,"Failed to setup connection with server");
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

    int_T ierr, nleft, dataTypeSize, recvWidth;
    char_T *gMsg, *gMsg_recv, *dataType;
    real64_T timeout = (real64_T)mxGetScalar(timeout(S));

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
    tcp_senddata(socketID, &dataTypeSize, gMsg, &nleft, &ierr);
    
    // switch according to data type
    dataType   = mxArrayToString(dataType(S));
    recvWidth  = (int_T)mxGetScalar(recvWidth(S));
    
    if (strcmp(dataType,"double") == 0)  {
        real64_T *data_recv = (real64_T *)ssGetOutputPortRealSignal(S,0);
        gMsg_recv = (char_T *)data_recv;
        dataTypeSize = (int_T)sizeof(real64_T);
    }
    else if (strcmp(dataType,"single") == 0)  {
        real32_T *data_recv = (real32_T *)ssGetOutputPortRealSignal(S,0);
        gMsg_recv = (char_T *)data_recv;
        dataTypeSize = (int_T)sizeof(real32_T);
    }
    else if (strcmp(dataType,"int32") == 0)  {
        int32_T *data_recv = (int32_T *)ssGetOutputPortRealSignal(S,0);
        gMsg_recv = (char_T *)data_recv;
        dataTypeSize = (int_T)sizeof(int32_T);
    }
    else if (strcmp(dataType,"int16") == 0)  {
        int16_T *data_recv = (int16_T *)ssGetOutputPortRealSignal(S,0);
        gMsg_recv = (char_T *)data_recv;
        dataTypeSize = (int_T)sizeof(int16_T);
    }
    else if (strcmp(dataType,"int8") == 0)  {
        int8_T *data_recv = (int8_T *)ssGetOutputPortRealSignal(S,0);
        gMsg_recv = (char_T *)data_recv;
        dataTypeSize = (int_T)sizeof(int8_T);
    }
    else  {
        ssSetErrorStatus(S,"Data type is not supported");
        return;
    }

    // receive the data
    tcp_recvdata_timeout(socketID, &dataTypeSize, gMsg_recv, &recvWidth, &ierr, &timeout);
    if (ierr < 0) {
        ssSetErrorStatus(S,"Data didn't arrive on time...");
    }
}


// ============================================================================
// Function: mdlTerminate

static void mdlTerminate(SimStruct *S)
{
    int_T ierr;

    // get socketID
    int_T *socketID = (int_T*)ssGetDWork(S,0);

    tcp_closeconnection(socketID, &ierr);
}

#ifdef  MATLAB_MEX_FILE    // Is this file being compiled as a MEX-file?
#include "simulink.c"      // MEX-file interface mechanism
#else
#include "cg_sfun.h"       // Code generation registration function
#endif