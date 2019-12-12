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
// Created: 02/09
// Revision: A
//
// Description: This file contains the class definition for genericClient.
// genericClient is a generic element defined by any number of nodes and 
// the degrees of freedom at those nodes. The element communicates with 
// an OpenFresco element trough a tcp/ip connection.

#define S_FUNCTION_NAME SFun_GenericClient
#define S_FUNCTION_LEVEL 2

#include "simstruc.h"
#include <math.h>
#include <string.h>

// functions defined in socket.c
void tcp_setupconnectionclient(unsigned int *port, const char machineInetAddr[], int *lengthInet, int *socketID);
void tcp_closeconnection(int *socketID, int *ierr);
void tcp_senddata(const int *socketID, int *dataTypeSize, char data[], int *lenData, int *ierr);
void tcp_recvdata(const int *socketID, int *dataTypeSize, char data[], int *lenData, int *ierr);

// generic client parameters
#define ipAddr(S)   ssGetSFcnParam(S,0)    // ip address of server
#define ipPort(S)   ssGetSFcnParam(S,1)    // ip port of server
#define dataSize(S) ssGetSFcnParam(S,2)    // send/receive vector data size

#define NPARAMS 3

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
    if (!mxIsDouble(dataSize(S)) || mxGetPr(dataSize(S))[0] <= 0)  {
        ssSetErrorStatus(S,"dataSize must be a positive nonzero value");
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
    ssSetInputPortWidth(S, 0, DYNAMICALLY_SIZED);
    ssSetInputPortDirectFeedThrough(S, 0, 1);
    
    if (!ssSetNumOutputPorts(S, 2)) return;
    ssSetOutputPortWidth(S, 0, DYNAMICALLY_SIZED);
    ssSetOutputPortWidth(S, 1, DYNAMICALLY_SIZED);
    
    ssSetNumSampleTimes(S, 1);
    ssSetNumDWork(S, 5);
    ssSetNumRWork(S, 0);
    ssSetNumIWork(S, 0);
    ssSetNumPWork(S, 0);
    ssSetNumModes(S, 0);
    ssSetNumNonsampledZCs(S, 0);
    
    // allocate memory for socketID
    ssSetDWorkWidth(S, 0, 1);
    ssSetDWorkDataType(S, 0, SS_INT32);
    
    // allocate memory for dataSize
    ssSetDWorkWidth(S, 1, 1);
    ssSetDWorkDataType(S, 1, SS_INT32);
    
    // allocate memory for the send and receive vectors
    ssSetDWorkWidth(S, 2, 256);
    ssSetDWorkDataType(S, 2, SS_DOUBLE);
    ssSetDWorkWidth(S, 3, 256);
    ssSetDWorkDataType(S, 3, SS_DOUBLE);
    
    // allocate memory for time
    ssSetDWorkWidth(S, 4, 1);
    ssSetDWorkDataType(S, 4, SS_DOUBLE);
    
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
    int_T sizeAddr, iData[11];
    
    int_T i, ierr, nleft, dataTypeSize;
    char_T *gMsg;
    
    // get parameters
    int_T sizeTrialDisp = ssGetInputPortWidth(S,0);
    int_T sizeDaqDisp   = ssGetOutputPortWidth(S,0);
    int_T sizeDaqForce  = ssGetOutputPortWidth(S,1);
    
    // get work vectors
    int_T *socketID = (int_T*)ssGetDWork(S,0);
    int_T *dataSize = (int_T*)ssGetDWork(S,1);
    real_T *sData   = (real_T*)ssGetDWork(S,2);
    real_T *rData   = (real_T*)ssGetDWork(S,3);
    
    // initialize socketID and dataSize
    socketID[0] = -1;
    dataSize[0] = (int_T)mxGetScalar(dataSize(S));
    dataSize[0] = (1+3*sizeTrialDisp>dataSize[0]) ? 1+3*sizeTrialDisp : dataSize[0];
    dataSize[0] = (sizeDaqDisp+sizeDaqForce>dataSize[0]) ? sizeDaqDisp+sizeDaqForce : dataSize[0];
    
    // resize and initialize send and receive vectors
    ssSetDWorkWidth(S, 2, dataSize[0]);
    ssSetDWorkWidth(S, 3, dataSize[0]);
    for (i=0; i<dataSize[0]; i++)  {
        sData[i] = 0.0;
        rData[i] = 0.0;
    }
    
    // setup the connection
    ipAddr = mxArrayToString(ipAddr(S));
    sizeAddr = (int_T)mxGetN(ipAddr(S)) + 1;
    ipPort = (int_T)mxGetScalar(ipPort(S));
    tcp_setupconnectionclient(&ipPort, ipAddr, &sizeAddr, socketID);
    mxFree(ipAddr);
    if (socketID[0] < 0)  {
        ssSetErrorStatus(S,"Failed to setup connection with server");
        return;
    }
    
    // set the data size for the server
    // sizeCtrl
    iData[0] = sizeTrialDisp;   // disp
    iData[1] = sizeTrialDisp;   // vel
    iData[2] = sizeTrialDisp;   // accel
    iData[3] = 0;               // force
    iData[4] = 0;               // time
    // sizeDaq
    iData[5] = sizeDaqDisp;     // disp
    iData[6] = 0;               // vel
    iData[7] = 0;               // accel
    iData[8] = sizeDaqForce;    // force
    iData[9] = 0;               // time
    // dataSize
    iData[10] = dataSize[0];
    
    // send the data sizes
    gMsg = (char_T *)iData;
    dataTypeSize = sizeof(int_T);
    nleft = 11;
    tcp_senddata(socketID, &dataTypeSize, gMsg, &nleft, &ierr);
}

  
// ============================================================================
// Function: mdlOutputs
//    Calculate outputs

static void mdlOutputs(SimStruct *S, int_T tid)
{
    InputRealPtrsType trialResp = ssGetInputPortRealSignalPtrs(S,0);
    real_T *daqDisp  = ssGetOutputPortRealSignal(S,0);
    real_T *daqForce = ssGetOutputPortRealSignal(S,1);
    
    int_T i, ierr, nleft, dataTypeSize;
    char_T *gMsg;
    
    // get parameters
    int_T sizeTrialDisp = ssGetInputPortWidth(S,0);
    int_T sizeDaqDisp   = ssGetOutputPortWidth(S,0);
    int_T sizeDaqForce  = ssGetOutputPortWidth(S,1);
    
    // get work vectors
    int_T *socketID = (int_T*)ssGetDWork(S,0);
    int_T *dataSize = (int_T*)ssGetDWork(S,1);
    real_T *sData   = (real_T*)ssGetDWork(S,2);
    real_T *rData   = (real_T*)ssGetDWork(S,3);
    time_T *time    = (time_T*)ssGetDWork(S,4); 
    
    UNUSED_ARG(tid);    // not used in single tasking mode
    
    if (time[0] < ssGetT(S))  {
        // commit state
        sData[0] = 5;
        gMsg = (char_T *)sData;
        dataTypeSize = sizeof(real_T);
        nleft = dataSize[0];
        tcp_senddata(socketID, &dataTypeSize, gMsg, &nleft, &ierr);
        // save current time
        time[0] = ssGetT(S);
    }
    
    // send trial response
    sData[0] = 3;
    for (i=0; i<sizeTrialDisp; i++)  {
        sData[1+i] = (*trialResp[i]);
        sData[1+sizeTrialDisp+i] = 0.0;
        sData[1+2*sizeTrialDisp+i] = 0.0;
    }
    gMsg = (char_T *)sData;
    dataTypeSize = sizeof(real_T);
    nleft = dataSize[0];
    tcp_senddata(socketID, &dataTypeSize, gMsg, &nleft, &ierr);
    
    // get measured response
    sData[0] = 6;
    gMsg = (char_T *)sData;
    nleft = dataSize[0];
    tcp_senddata(socketID, &dataTypeSize, gMsg, &nleft, &ierr);
    
    gMsg = (char_T *)rData;
    nleft = dataSize[0];
    tcp_recvdata(socketID, &dataTypeSize, gMsg, &nleft, &ierr);
    for (i=0; i<sizeDaqDisp; i++) {
        daqDisp[i] = rData[i];
    }
    for (i=0; i<sizeDaqForce; i++) {
        daqForce[i] = rData[sizeDaqDisp+i];
    }
}


// ============================================================================
// Function: mdlTerminate

static void mdlTerminate(SimStruct *S)
{
    int_T ierr, nleft, dataTypeSize;
    char_T *gMsg;
    
    // get work vectors
    int_T *socketID = (int_T*)ssGetDWork(S,0);
    int_T *dataSize = (int_T*)ssGetDWork(S,1);
    real_T *sData   = (real_T*)ssGetDWork(S,2);
    real_T *rData   = (real_T*)ssGetDWork(S,3);
    
    // shutdown server
    sData[0] = 99;
    
    gMsg = (char_T *)sData;
    dataTypeSize = sizeof(real_T);
    nleft = dataSize[0];
    tcp_senddata(socketID, &dataTypeSize, gMsg, &nleft, &ierr);
    
    tcp_closeconnection(socketID, &ierr);
}

#ifdef  MATLAB_MEX_FILE    // Is this file being compiled as a MEX-file?
#include "simulink.c"      // MEX-file interface mechanism
#else
#include "cg_sfun.h"       // Code generation registration function
#endif
