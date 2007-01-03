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

// $Revision: $
// $Date: $
// $URL: $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 10/06
// Revision: A
//
// Description: This file contains the Matlab gateway function for 
// the c tcp/ip socket.

#include "mex.h"
#include <string.h>


// functions defined in socket.c
void senddata(const int *socketID, int *dataTypeSize, char data[], int *lenData, int *ierr);
void recvdata(const int *socketID, int *dataTypeSize, char data[], int *lenData, int *ierr);
void establishconnection(unsigned int *port, const char machineInetAddr[], int *lengthInet, int *socketID);
void closeconnection(int *socketID, int *ierr);
void getsocketid(unsigned int *port, const char machineInetAddr[], int *lengthInet, int *socketID);


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
        mexErrMsgTxt("TCPSocket::mexFunction - WARNING insufficient input arguments \nWant: varargout = TCPSocket(action,varargin);");
    if (nlhs>1)
        mexErrMsgTxt("TCPSocket::mexFunction - WARNING to many output arguments \nWant: varargout = TCPSocket(action,varargin);");

    // first input must be a row vector string 
    if (mxIsChar(prhs[0]) != 1)
        mexErrMsgTxt("TCPSocket::mexFunction - WARNING first input parameter, action, must be a string");
    if (mxGetM(prhs[0]) != 1)
        mexErrMsgTxt("TCPSocket::mexFunction - WARNING first input parameter, action, must be a row vector");

    // get the action
    action = mxArrayToString(prhs[0]);

    // switch according to requested action
    if (strcmp(action,"sendData") == 0) {

        socketID = (int)mxGetScalar(prhs[1]);
        nleft = (int)mxGetScalar(prhs[3]);
        
        // check if data has correct size
        if (nleft != (int)(mxGetM(prhs[2])*mxGetN(prhs[2])))
            mexErrMsgTxt("TCPSocket::mexFunction - WARNING size of data does not agree with specified dataSize");
        
        if (mxIsInt32(prhs[2]) == 1) {
            int *data;
            dataTypeSize = sizeof(int);
            data = (int *)mxGetPr(prhs[2]);
            gMsg = (char *)data;
        }
        else if (mxIsDouble(prhs[2]) == 1) {
            double *data;
            dataTypeSize = sizeof(double);
            data = (double *)mxGetPr(prhs[2]);
            gMsg = (char *)data;
        }
        else if (mxIsChar(prhs[2]) == 1) {
            char *data;
            dataTypeSize = sizeof(char);
            data = mxArrayToString(prhs[2]);
            gMsg = (char *)data;
        }
        
        senddata(&socketID, &dataTypeSize, gMsg, &nleft, &ierr);

        plhs[0] = mxCreateDoubleScalar(ierr);
    }
    else if (strcmp(action,"recvData") == 0) {

        int dataSize[2];
        char *dataType;

        socketID = (int)mxGetScalar(prhs[1]);
        nleft = (int)mxGetScalar(prhs[2]);
        dataSize[0] = 1;
        dataSize[1] = nleft;

        // set the output pointer to the output matrix
        if (nrhs < 4) {
            double *data;
            dataTypeSize = sizeof(double);
            plhs[0] = mxCreateNumericArray(2, dataSize, mxDOUBLE_CLASS, mxREAL);
            data = (double *)mxGetPr(plhs[0]);
            gMsg = (char *)data;
        }
        else {
            dataType = mxArrayToString(prhs[3]);
            if (strcmp(dataType,"int") == 0) {
                int *data;
                dataTypeSize = sizeof(int);
                plhs[0] = mxCreateNumericArray(2, dataSize, mxINT32_CLASS, mxREAL);
                data = (int *)mxGetPr(plhs[0]);
                gMsg = (char *)data;
            }
            else if (strcmp(dataType,"double") == 0) {
                double *data;
                dataTypeSize = sizeof(double);
                plhs[0] = mxCreateNumericArray(2, dataSize, mxDOUBLE_CLASS, mxREAL);
                data = (double *)mxGetPr(plhs[0]);
                gMsg = (char *)data;
            }
            else if (strcmp(dataType,"char") == 0) {
                char *data;
                dataTypeSize = sizeof(char);
                plhs[0] = mxCreateNumericArray(2, dataSize, mxCHAR_CLASS, mxREAL);
                data = mxArrayToString(plhs[0]);
                gMsg = (char *)data;
            }
        }
        
        recvdata(&socketID, &dataTypeSize, gMsg, &nleft, &ierr);
    }
    else if (strcmp(action,"openConnection") == 0) {
        
        int sizeAddr;

        ipPort = (int)mxGetScalar(prhs[1]);
        if (nrhs<3) {
            ipAddr = "127.0.0.1";
            sizeAddr = 10;
        }
        else {
            ipAddr = mxArrayToString(prhs[2]);
            sizeAddr = (int)mxGetN(prhs[2]) + 1;
        }
        
        establishconnection(&ipPort, ipAddr, &sizeAddr, &socketID);

        plhs[0] = mxCreateDoubleScalar(socketID);

        if (nrhs>=3)
            mxFree(ipAddr);
    }
    else if (strcmp(action,"closeConnection") == 0) {

        socketID = (int)mxGetScalar(prhs[1]);

        closeconnection(&socketID, &ierr);

        plhs[0] = mxCreateDoubleScalar(ierr);
    }
    else if (strcmp(action,"getSocketID") == 0) {
        
        int sizeAddr;

        ipPort = (int)mxGetScalar(prhs[1]);
        if (nrhs<3) {
            ipAddr = "127.0.0.1";
            sizeAddr = 10;
        }
        else {
            ipAddr = mxArrayToString(prhs[2]);
            sizeAddr = (int)mxGetN(prhs[2]) + 1;
        }

        getsocketid(&ipPort, ipAddr, &sizeAddr, &socketID);

        plhs[0] = mxCreateDoubleScalar(socketID);

        if (nrhs>=3)
            mxFree(ipAddr);
    }
    else {
        mexErrMsgTxt("TCPSocket::mexFunction - WARNING invalid action received");        
    }
}
