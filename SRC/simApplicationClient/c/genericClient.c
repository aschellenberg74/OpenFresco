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
// Created: 11/06
// Revision: A
//
// Description: This file contains the class definition for genericClient.
// genericClient is a generic element defined by any number of nodes and 
// the degrees of freedom at those nodes. The element communicates with 
// an OpenFresco element through a udp connection.

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

int socketID;
int dataSize = 256;

void udp_setupconnectionclient(unsigned int *port, const char inetAddr[], int *lengthInet, int *socketID);
void udp_senddata(const int *socketID, int *dataTypeSize, char data[], int *lenData, int *ierr);
void udp_recvdata(const int *socketID, int *dataTypeSize, char data[], int *lenData, int *ierr);
void udp_closeconnection(int *socketID, int *ierr);

int genericClient(double *d,
                  double *ul,
                  double *uldot,
                  double *uldotdot,
                  double *xl,
                  int    *ix,
                  double *tl,
                  double *s,
                  double *r,
                  int    ndf,
                  int    ndm,
                  int    nst,
                  int    isw)
{
    // local variables
    static int i, j;
    unsigned int port;
    
    static int    iData[11];
    static double *sData;
    static double *rData;
    
    int ierr, nleft, dataTypeSize;
    char *gMsg;
    
    // ==============================================================
    // output element type 
    if (isw == 0) {
        
        fprintf(stdout, "genericClient numNodes: %f port: %f\n", d[0], d[1]);
    }
    // ==============================================================
    // check for valid input args
    else if (isw == 1) {
        
        int sizeMachineInet;
        
        if (ndf < 1 || ndf > 6)
            return -1;
        if (ndm < 1 || ndm > 3)
            return -1;
        if (nst != d[0]*ndf)
            return -1;
        
        // allocate memory for the send and receive vectors
        dataSize = (1+3*nst>dataSize) ? 1+3*nst : dataSize;
        dataSize = (nst*nst>dataSize) ? nst*nst : dataSize;
        sData = calloc(dataSize, sizeof(double));
        rData = calloc(dataSize, sizeof(double));
        
        // setup the connection
        port = (int)d[1];
        sizeMachineInet = 9+1;
        udp_setupconnectionclient(&port, "127.0.0.1", &sizeMachineInet, &socketID);
        if (socketID < 0)
            return -1;
        
        // set the data size for the experimental element
        // sizeCtrl
        iData[0] = nst;  // disp
        iData[1] = nst;  // vel
        iData[2] = nst;  // accel
        iData[3] = 0;    // force
        iData[4] = 0;    // time
        // sizeDaq
        iData[5] = 0;    // disp
        iData[6] = 0;    // vel
        iData[7] = 0;    // accel
        iData[8] = nst;  // force
        iData[9] = 0;    // time
        // dataSize
        iData[10] = dataSize;
        
        gMsg = (char *)iData;
        dataTypeSize = sizeof(int);
        nleft = 11;
        udp_senddata(&socketID, &dataTypeSize, gMsg, &nleft, &ierr);
    }
    // ==============================================================
    // check element for errors
    else if (isw == 2) {
    
        return 0;    
    }
    // ==============================================================
    // compute element residual and tangent matrix
    else if (isw == 3 || isw == 4 || isw == 6) {
        
        // send trial response to experimental site
        sData[0] = 3;
        for (i=0; i<nst; i++) {
            sData[1+i] = ul[i];
            sData[1+nst+i] = uldot[i];
            sData[1+2*nst+i] = uldotdot[i];
        }
        
        gMsg = (char *)sData;
        dataTypeSize = sizeof(double);
        nleft = dataSize;
        udp_senddata(&socketID, &dataTypeSize, gMsg, &nleft, &ierr);
        
        // add stiffness portion to matrix
        if (isw == 3) {
            sData[0] = 13;
            
            gMsg = (char *)sData;
            dataTypeSize = sizeof(double);
            nleft = dataSize;
            udp_senddata(&socketID, &dataTypeSize, gMsg, &nleft, &ierr);
            
            gMsg = (char *)rData;
            nleft = dataSize;
            udp_recvdata(&socketID, &dataTypeSize, gMsg, &nleft, &ierr);
            
            for (i=0; i<nst*nst; i++) {
                s[i] = rData[i];
            }
        }
        
        // get measured resisting forces
        sData[0] = 10;
        
        gMsg = (char *)sData;
        nleft = dataSize;
        udp_senddata(&socketID, &dataTypeSize, gMsg, &nleft, &ierr);
        
        gMsg = (char *)rData;
        nleft = dataSize;
        udp_recvdata(&socketID, &dataTypeSize, gMsg, &nleft, &ierr);
        
        for (i=0; i<nst; i++) {
            r[i] = rData[i];
        }
        
        if (isw == 6) {
            for (i=0; i<nst; i++) {
                fprintf(stdout, "%f ",r[i]);
            }
            fprintf(stdout, "\n");
        }
    }
    // ==============================================================
    // compute element mass matrix
    else if (isw == 5) {
        
        sData[0] = 15;
        
        gMsg = (char *)sData;
        dataTypeSize = sizeof(double);
        nleft = dataSize;
        udp_senddata(&socketID, &dataTypeSize, gMsg, &nleft, &ierr);
        
        gMsg = (char *)rData;
        nleft = dataSize;
        udp_recvdata(&socketID, &dataTypeSize, gMsg, &nleft, &ierr);
        
        for (i=0; i<nst*nst; i++) {
            s[i] = rData[i];
        }
    }
    // ==============================================================
    // output surface loading
    else if (isw == 7) {
        
        // not implemented yet
    }
    // ==============================================================
    // compute stress projections at nodes
    else if (isw == 8) {
        
        // not implemented yet
    }
    // ==============================================================
    // disconnect from experimental site
    else if (isw == 10) {
        
        sData[0] = 99;
        
        gMsg = (char *)sData;
        dataTypeSize = sizeof(double);
        nleft = dataSize;
        udp_senddata(&socketID, &dataTypeSize, gMsg, &nleft, &ierr);
        
        udp_closeconnection(&socketID, &ierr);
        
        // clean up allocated memory
        free(sData);
        free(rData);
    }
    // ==============================================================
    
    return 0;
}


int main(int argc, char **argv) {
    
    int ndm = 2;
    int ndf = 2;
    int nst = 4;
    
    int i,j;
    double d[2];
    double disp[4];
    double vel[4];
    double accel[4];
    double s[16];  // 4x4 stiffness matrix
    double r[4];
    
    int  nodeNum[2];
    double coord[4];
    double  temp[2];
    
    d[0] = 2;
    d[1] = 8090;
    nodeNum[0] = 3; nodeNum[1] = 4;
    coord[0] = 168.0; coord[1] = 0.0; coord[2] = 72.0; coord[3] = 96.0;
    temp[0] = 0.0; temp[1] = 0.0;
    
    // setup
    genericClient(d,disp,vel,accel,coord,nodeNum,temp,s,r,ndf,ndm,nst,1);
    
    // disp, stiff, force
    fprintf(stderr,"\nDISP:\n");
    
    disp[0] = 0.0; disp[1] = 0.0; disp[2] = 1.06017; disp[3] = -0.355778;
    for (i=0; i<nst; i++) {
        vel[i] = 0.0;
        accel[i] = 0.0;
        for (j=0; j<nst; j++)
            s[i*nst+j] = 0.0;
        r[i] = 0.0;
        fprintf(stderr,"%f %f %f\n",disp[i],vel[i],accel[i]);
    }
    fprintf(stderr,"\n");
    
    genericClient(d,disp,vel,accel,coord,nodeNum,temp,s,r,ndf,ndm,nst,3);
    for (i=0; i<nst; i++) {
        fprintf(stderr,"%f %f %f\n",disp[i],vel[i],accel[i]);
    }
    fprintf(stderr,"\nFORCES:\n");
    for (j=0; j<nst; j++)
        fprintf(stderr,"%f ",r[j]);
    fprintf(stderr,"\n");
    
    // shutdown
    genericClient(d,disp,vel,accel,coord,nodeNum,temp,s,r,ndf,ndm,nst,10);
    
    return 0;
}
