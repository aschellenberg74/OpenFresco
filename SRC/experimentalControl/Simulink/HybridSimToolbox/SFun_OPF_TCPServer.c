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
// Description: This file contains the class definition for OPF_TCPServer.
// OPF_TCPServer communicates with a SimSimulink experimental control in
// OpenFresco through a nonblocking tcp connection.

#define S_FUNCTION_NAME SFun_OPF_TCPServer
#define S_FUNCTION_LEVEL 2

#include "simstruc.h"
#include <math.h>
#include <string.h>

// functions defined in socket.c
void tcp_setupconnectionserver(unsigned int *port, int *socketID);
void tcp_closeconnection(int *socketID, int *ierr);
void tcp_senddata(const int *socketID, int *dataTypeSize, char data[], int *lenData, int *ierr);
void tcp_sendnbdata(const int *socketID, int *dataTypeSize, char data[], int *lenData, int *ierr);
void tcp_recvdata(const int *socketID, int *dataTypeSize, char data[], int *lenData, int *ierr);
void tcp_recvnbdata(const int *socketID, int *dataTypeSize, char data[], int *lenData, int *ierr);

// OPFConnect parameters
#define ipPort(S)         ssGetSFcnParam(S,0)    // ip port of server
#define numCtrlSignals(S) ssGetSFcnParam(S,1)    // number of control signals
#define numDaqSignals(S)  ssGetSFcnParam(S,2)    // number of daq signals
#define nbMode(S)         ssGetSFcnParam(S,3)    // nonblocking mode flag

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
    if (!mxIsDouble(ipPort(S)) || mxGetPr(ipPort(S))[0] <= 0)  {
        ssSetErrorStatus(S,"ipPort must be a positive nonzero value");
        return;
    }
    if (!mxIsDouble(numCtrlSignals(S)) || mxGetPr(numCtrlSignals(S))[0] <= 0) {
        ssSetErrorStatus(S, "numCtrlSignals must be a positive nonzero value");
        return;
    }
    if (!mxIsDouble(numDaqSignals(S)) || mxGetPr(numDaqSignals(S))[0] <= 0) {
        ssSetErrorStatus(S, "numDaqSignals must be a positive nonzero value");
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

    // get input and output port widths from parameters
    int_T sizeDaqSig = (int_T)mxGetScalar(numDaqSignals(S));
    int_T sizeCtrlSig = (int_T)mxGetScalar(numCtrlSignals(S));
    
    ssSetNumContStates(S, 0);
    ssSetNumDiscStates(S, 0);
    
    if (!ssSetNumInputPorts(S, 3)) return;
    ssSetInputPortWidth(S, 0, 1);           // switchPC
    ssSetInputPortWidth(S, 1, 1);           // atTarget
    ssSetInputPortWidth(S, 2, sizeDaqSig);  // daqSig
    ssSetInputPortDirectFeedThrough(S, 0, 1);
    ssSetInputPortDirectFeedThrough(S, 1, 1);
    ssSetInputPortDirectFeedThrough(S, 2, 1);
    
    if (!ssSetNumOutputPorts(S, 2)) return;
    ssSetOutputPortWidth(S, 0, 1);           // newTarget
    ssSetOutputPortWidth(S, 1, sizeCtrlSig); // ctrlSig
    
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
    uint_T ipPort;
    int_T iData[11];
    
    int_T i, ierr, nleft, dataTypeSize;
    char_T *gMsg;
    
    // get parameters
    int_T sizeDaqSig = ssGetInputPortWidth(S,2);
    int_T sizeCtrlSig = ssGetOutputPortWidth(S,1);
    
    // get work vectors
    int_T *socketID = (int_T*)ssGetDWork(S,0);
    int_T *dataSize = (int_T*)ssGetDWork(S,1);
    real_T *sData   = (real_T*)ssGetDWork(S,2);
    real_T *rData   = (real_T*)ssGetDWork(S,3);
    
    // initialize socketID and dataSize
    socketID[0] = -1;
    dataSize[0] = 256;
    
    // setup the connection with the ECSimSimulink client
    ipPort = (uint_T)mxGetScalar(ipPort(S));
    tcp_setupconnectionserver(&ipPort, socketID);
    if (socketID[0] < 0)  {
        ssSetErrorStatus(S,"Failed to setup connection with ECSimSimulink");
        return;
    }
    
    // receive the data sizes
    gMsg = (char_T *)iData;
    dataTypeSize = sizeof(int_T);
    nleft = 11;
    tcp_recvdata(socketID, &dataTypeSize, gMsg, &nleft, &ierr);
    dataSize[0] = iData[10];
    
    // check for correct data sizes
    // iData = {ctrlDisp, ctrlVel, ctrlAccel, ctrlForce, ctrlTime,
    //          daqDisp,  daqVel,  daqAccel,  daqForce,  daqTime,  dataSize}
    if ((iData[0] != sizeCtrlSig && iData[3] != sizeCtrlSig) || 
        (iData[5] + iData[6] + iData[7] + iData[8] != sizeDaqSig))  {
        ssSetErrorStatus(S,"Wrong data sizes from ECSimSimulink received");
        return;
    }
    
    // resize and initialize send and receive vectors
    ssSetDWorkWidth(S, 2, dataSize[0]);
    ssSetDWorkWidth(S, 3, dataSize[0]);
    for (i=0; i<dataSize[0]; i++)  {
        sData[i] = 0.0;
        rData[i] = 0.0;
    }
}

  
// ============================================================================
// Function: mdlOutputs
//    Calculate outputs

static void mdlOutputs(SimStruct *S, int_T tid)
{
    InputRealPtrsType switchPC = ssGetInputPortRealSignalPtrs(S,0);
    InputRealPtrsType atTarget = ssGetInputPortRealSignalPtrs(S,1);
    InputRealPtrsType daqSig   = ssGetInputPortRealSignalPtrs(S,2);
    
    real_T *newTarget = ssGetOutputPortRealSignal(S,0);
    real_T *ctrlSig   = ssGetOutputPortRealSignal(S,1);
    
    int_T i, id, ierr, nleft, dataTypeSize;
    char_T *gMsg;
    
    // get parameters
    int_T sizeDaqSig  = ssGetInputPortWidth(S,2);
    int_T sizeCtrlSig = ssGetOutputPortWidth(S,1);
    
    // get work vectors
    int_T *socketID = (int_T*)ssGetDWork(S,0);
    int_T *dataSize = (int_T*)ssGetDWork(S,1);
    real_T *sData   = (real_T*)ssGetDWork(S,2);
    real_T *rData   = (real_T*)ssGetDWork(S,3);
    time_T *time    = (time_T*)ssGetDWork(S,4); 
    
    UNUSED_ARG(tid);    // not used in single tasking mode
    
    // receive the data from ECSimSimulink control in specified mode
    int nbMode = (int)mxGetScalar(nbMode(S));
    gMsg = (char_T *)rData;
    dataTypeSize = sizeof(real_T);
    nleft = dataSize[0];
    if (nbMode == 1)
        tcp_recvnbdata(socketID, &dataTypeSize, gMsg, &nleft, &ierr);
    else
        tcp_recvdata(socketID, &dataTypeSize, gMsg, &nleft, &ierr);
    
    if (ierr == 0)  {
        // process new ctrlSig
        if (rData[0] == 3)  {
            for (i=0; i<sizeCtrlSig; i++)
                ctrlSig[i] = rData[1+i];
            time[0] = rData[sizeCtrlSig+1];
            // also set newTarget flag
            newTarget[0] = 1;
        }
        // process newTarget flag
        else if (rData[0] == 4)  {
            newTarget[0] = 1;
        }
        // send switchPC flag back
        else if (rData[0] == 4.1)  {
            sData[0] = (*switchPC)[0];
            
            gMsg = (char_T *)sData;
            tcp_senddata(socketID, &dataTypeSize, gMsg, &nleft, &ierr);
        }
        // process newTarget flag
        else if (rData[0] == 4.2)  {
            newTarget[0] = 0;
        }
        // send switchPC flag back
        else if (rData[0] == 4.3)  {
            sData[0] = (*switchPC)[0];
            
            gMsg = (char_T *)sData;
            tcp_senddata(socketID, &dataTypeSize, gMsg, &nleft, &ierr);
        }
        // send daq response back
        else if (rData[0] == 6)  {
            id = 1;
            sData[0] = (*atTarget)[0];
            // loop through daq signals
            for (i=0; i<sizeDaqSig; i++)
                sData[id+i] = (*daqSig)[i];
            id += sizeDaqSig;
            // add current daq time
            sData[id] = ssGetT(S);
            
            gMsg = (char_T *)sData;
            tcp_senddata(socketID, &dataTypeSize, gMsg, &nleft, &ierr);
        }
        // stop the simulation
        else if (rData[0] == 99)  {
            ssSetStopRequested(S, 1);
        }
    }
    // do not use this in case we are receiving data in nonblocking mode
    //else {
    //    // stop the simulation
    //    ssSetStopRequested(S, 1);
    //    ssSetErrorStatus(S, "Failed to receive data from ECSimSimulink while executing tcp_recvnbdata()");
    //}
}


// ============================================================================
// Function: mdlTerminate

static void mdlTerminate(SimStruct *S)
{
    int_T ierr;
    
    // get work vector
    int_T *socketID = (int_T*)ssGetDWork(S,0);
    
    tcp_closeconnection(socketID, &ierr);
}

#ifdef  MATLAB_MEX_FILE    // Is this file being compiled as a MEX-file?
#include "simulink.c"      // MEX-file interface mechanism
#else
#include "cg_sfun.h"       // Code generation registration function
#endif
