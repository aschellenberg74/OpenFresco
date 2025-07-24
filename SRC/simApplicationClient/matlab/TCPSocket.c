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
// Created: 10/06
// Revision: A
//
// Description: This file contains the Matlab gateway function for 
// the tcp_socket.c.

#include "mex.h"
#include <string.h>

// functions defined in tcp_socket.c
void tcp_setupconnectionserver(unsigned int *port, int *socketID);
void tcp_setupconnectionclient(unsigned int *port, const char machineInetAddr[],
    int *lengthInet, int *socketID);
void tcp_closeconnection(int *socketID, int *ierr);
void tcp_senddata(const int *socketID, int *dataTypeSize, char data[],
    int *lenData, int *ierr);
void tcp_sendnbdata(const int* socketID, int* dataTypeSize, char data[],
    int* lenData, int* ierr);
void tcp_recvdata(const int *socketID, int *dataTypeSize, char data[],
    int *lenData, int *ierr);
void tcp_recvnbdata(const int* socketID, int* dataTypeSize, char data[],
    int* lenData, int* ierr);
void tcp_getsocketid(unsigned int *port, const char machineInetAddr[],
    int *lengthInet, int *socketID);

// the gateway function
void mexFunction(int nlhs, mxArray *plhs[],
                 int nrhs, const mxArray *prhs[])
{
    char *action, *ipAddr;
    unsigned int ipPort;
    int socketID;
    
    int ierr, nleft, dataTypeSize;
    char *gMsg;
    
    // check for proper number of arguments
    if (nrhs<2)
        mexErrMsgIdAndTxt("OpenFresco:TCPSocket:invalidNumInputs",
                "Insufficient input arguments \nWant: varargout = TCPSocket(action,varargin);\n");
    if (nlhs>1)
        mexErrMsgIdAndTxt("OpenFresco:TCPSocket:invalidNumOutputs",
                "To many output arguments \nWant: varargout = TCPSocket(action,varargin);\n");
    
    // first input must be a row vector string 
    if (mxIsChar(prhs[0]) != 1)
        mexErrMsgIdAndTxt("OpenFresco:TCPSocket:inputNotString",
                "First input parameter, action, must be a string.\n");
    if (mxGetM(prhs[0]) != 1)
        mexErrMsgIdAndTxt("OpenFresco:TCPSocket:inputNotRowVector",
                "First input parameter, action, must be a row vector.\n");
    
    // get the action
    action = mxArrayToString(prhs[0]);
    
    // switch according to requested action
    if (strcmp(action,"sendData") == 0)  {
        
        socketID = (int)mxGetScalar(prhs[1]);
        nleft = (int)mxGetScalar(prhs[3]);
        
        // check if data has correct size
        if (nleft != (int)(mxGetM(prhs[2])*mxGetN(prhs[2])))
            mexErrMsgIdAndTxt("OpenFresco:TCPSocket:invalidDataSize",
                    "Size of data does not agree with specified dataSize.\n");
        
        if (mxIsDouble(prhs[2]) == 1)  {
            double *data = (double *)mxGetPr(prhs[2]);
            gMsg = (char *)data;
            dataTypeSize = sizeof(double);
        }
        else if (mxIsInt32(prhs[2]) == 1)  {
            int *data = (int *)mxGetPr(prhs[2]);
            gMsg = (char *)data;
            dataTypeSize = sizeof(int);
        }
        else if (mxIsChar(prhs[2]) == 1)  {
            char *data = mxArrayToString(prhs[2]);
            gMsg = (char *)data;
            dataTypeSize = sizeof(char);
        }
        else  {
            mexErrMsgIdAndTxt("OpenFresco:TCPSocket:invalidDataType",
                    "Data type is not supported.\n");
        }
        
        tcp_senddata(&socketID, &dataTypeSize, gMsg, &nleft, &ierr);
        
        plhs[0] = mxCreateDoubleScalar(ierr);
    }
    else if (strcmp(action, "sendNBData") == 0) {
        
        socketID = (int)mxGetScalar(prhs[1]);
        nleft = (int)mxGetScalar(prhs[3]);
        
        // check if data has correct size
        if (nleft != (int)(mxGetM(prhs[2]) * mxGetN(prhs[2])))
            mexErrMsgIdAndTxt("OpenFresco:TCPSocket:invalidDataSize",
                "Size of data does not agree with specified dataSize.\n");
        
        if (mxIsDouble(prhs[2]) == 1) {
            double* data = (double*)mxGetPr(prhs[2]);
            gMsg = (char*)data;
            dataTypeSize = sizeof(double);
        }
        else if (mxIsInt32(prhs[2]) == 1) {
            int* data = (int*)mxGetPr(prhs[2]);
            gMsg = (char*)data;
            dataTypeSize = sizeof(int);
        }
        else if (mxIsChar(prhs[2]) == 1) {
            char* data = mxArrayToString(prhs[2]);
            gMsg = (char*)data;
            dataTypeSize = sizeof(char);
        }
        else {
            mexErrMsgIdAndTxt("OpenFresco:TCPSocket:invalidDataType",
                "Data type is not supported.\n");
        }
        
        tcp_sendnbdata(&socketID, &dataTypeSize, gMsg, &nleft, &ierr);
        
        plhs[0] = mxCreateDoubleScalar(ierr);
    }
    else if (strcmp(action,"recvData") == 0)  {
        
        mwSize ndim;
        const mwSize *dataSize[2];
        char *dataType;
        
        socketID = (int)mxGetScalar(prhs[1]);
        nleft = (int)mxGetScalar(prhs[2]);
        ndim = 2;
        dataSize[0] = 1;
        dataSize[1] = nleft;
        
        // set the output pointer to the output matrix
        if (nrhs < 4)  {
            double *data;
            plhs[0] = mxCreateNumericArray(ndim, dataSize, mxDOUBLE_CLASS, mxREAL);
            data = (double *)mxGetPr(plhs[0]);
            gMsg = (char *)data;
            dataTypeSize = sizeof(double);
        }
        else  {
            dataType = mxArrayToString(prhs[3]);
            if (strcmp(dataType,"double") == 0)  {
                double *data;
                plhs[0] = mxCreateNumericArray(ndim, dataSize, mxDOUBLE_CLASS, mxREAL);
                data = (double *)mxGetPr(plhs[0]);
                gMsg = (char *)data;
                dataTypeSize = sizeof(double);
            }
            else if (strcmp(dataType,"int") == 0)  {
                int *data;
                plhs[0] = mxCreateNumericArray(ndim, dataSize, mxINT32_CLASS, mxREAL);
                data = (int *)mxGetPr(plhs[0]);
                gMsg = (char *)data;
                dataTypeSize = sizeof(int);
            }
            else if (strcmp(dataType,"char") == 0)  {
                char *data;
                plhs[0] = mxCreateNumericArray(ndim, dataSize, mxCHAR_CLASS, mxREAL);
                data = mxArrayToString(plhs[0]);
                gMsg = (char *)data;
                dataTypeSize = sizeof(char);
            }
            else  {
                mexErrMsgIdAndTxt("OpenFresco:TCPSocket:invalidDataType",
                        "Data type is not supported.\n");
            }
        }
        
        tcp_recvdata(&socketID, &dataTypeSize, gMsg, &nleft, &ierr);
    }
    else if (strcmp(action, "recvNBData") == 0) {
        
        mwSize ndim;
        const mwSize* dataSize[2];
        char* dataType;
        
        socketID = (int)mxGetScalar(prhs[1]);
        nleft = (int)mxGetScalar(prhs[2]);
        ndim = 2;
        dataSize[0] = 1;
        dataSize[1] = nleft;
        
        // set the output pointer to the output matrix
        if (nrhs < 4) {
            double* data;
            plhs[0] = mxCreateNumericArray(ndim, dataSize, mxDOUBLE_CLASS, mxREAL);
            data = (double*)mxGetPr(plhs[0]);
            gMsg = (char*)data;
            dataTypeSize = sizeof(double);
        }
        else {
            dataType = mxArrayToString(prhs[3]);
            if (strcmp(dataType, "double") == 0) {
                double* data;
                plhs[0] = mxCreateNumericArray(ndim, dataSize, mxDOUBLE_CLASS, mxREAL);
                data = (double*)mxGetPr(plhs[0]);
                gMsg = (char*)data;
                dataTypeSize = sizeof(double);
            }
            else if (strcmp(dataType, "int") == 0) {
                int* data;
                plhs[0] = mxCreateNumericArray(ndim, dataSize, mxINT32_CLASS, mxREAL);
                data = (int*)mxGetPr(plhs[0]);
                gMsg = (char*)data;
                dataTypeSize = sizeof(int);
            }
            else if (strcmp(dataType, "char") == 0) {
                char* data;
                plhs[0] = mxCreateNumericArray(ndim, dataSize, mxCHAR_CLASS, mxREAL);
                data = mxArrayToString(plhs[0]);
                gMsg = (char*)data;
                dataTypeSize = sizeof(char);
            }
            else {
                mexErrMsgIdAndTxt("OpenFresco:TCPSocket:invalidDataType",
                    "Data type is not supported.\n");
            }
        }
        
        tcp_recvnbdata(&socketID, &dataTypeSize, gMsg, &nleft, &ierr);
    }
    else if (strcmp(action,"openConnection") == 0)  {
        
        int sizeAddr;
        
        if (nrhs==2)  {
            ipPort = (int)mxGetScalar(prhs[1]);
            tcp_setupconnectionserver(&ipPort, &socketID);
        }
        else if (nrhs==3)  {
            ipAddr = mxArrayToString(prhs[1]);
            sizeAddr = (int)mxGetN(prhs[1]) + 1;
            ipPort = (int)mxGetScalar(prhs[2]);
            tcp_setupconnectionclient(&ipPort, ipAddr, &sizeAddr, &socketID);
            mxFree(ipAddr);
        }
        else  {
            mexErrMsgIdAndTxt("OpenFresco:TCPSocket:invalidNumInputs",
                    "Wrong number of input arguments received.\n");
        }
        
        plhs[0] = mxCreateDoubleScalar(socketID);
    }
    else if (strcmp(action,"closeConnection") == 0)  {
        
        socketID = (int)mxGetScalar(prhs[1]);
        
        tcp_closeconnection(&socketID, &ierr);
        
        plhs[0] = mxCreateDoubleScalar(ierr);
    }
    else if (strcmp(action,"getSocketID") == 0)  {
        
        int sizeAddr;
        
        ipPort = (int)mxGetScalar(prhs[1]);
        ipAddr = mxArrayToString(prhs[2]);
        sizeAddr = (int)mxGetN(prhs[2]) + 1;
        
        tcp_getsocketid(&ipPort, ipAddr, &sizeAddr, &socketID);
        
        plhs[0] = mxCreateDoubleScalar(socketID);
        
        mxFree(ipAddr);
    }
    else  {
        mexErrMsgIdAndTxt("OpenFresco:TCPSocket:invalidAction",
                "Invalid action received.\n");
    }
}
