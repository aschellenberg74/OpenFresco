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
// $Source: $

// Written: Frank McKenna & Andreas Schellenberg
// Created: 10/06
// Revision: A
//
// Description: This file contains the implementation of the eeTruss.
// eeTruss is an experimental truss element defined by two nodes.
// The element communicates with an OpenFresco site through a tcp/ip
// connection.

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

int socketID;
int dataSize = 256;

void setupconnectionclient(unsigned int *port, const char inetAddr[], int *lengthInet, int *socketID);
void senddata(const int *socketID, int *dataTypeSize, char data[], int *lenData, int *ierr);
void recvdata(const int *socketID, int *dataTypeSize, char data[], int *lenData, int *ierr);
void closeconnection(int *socketID, int *ierr);

int eeTruss(double *d,
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
    static double L, dx, dy, dz, cosX[3];

    static int    iData[11];
    static double *sData;
    static double *rData;

    int ierr, nleft, dataTypeSize;
    char *gMsg;

    // ==============================================================
    // output element type 
    if (isw == 0) {

        fprintf(stdout, "eeTruss EA/L: %f port: %f\n", d[0], d[1]);
    }
    // ==============================================================
    // check for valid input args
    else if (isw == 1) {

        int sizeMachineInet;

        if (ndf < 1 || ndf > 6)
            return -1;
        if (ndm < 1 || ndm > 3)
            return -1;
        if (nst != 2*ndf)
            return -1;
        if (d[0] == 0.0)
            return -1;

        // allocate memory for the send and receive vectors
        dataSize = (1+3>dataSize) ? 1+3 : dataSize;
        sData = calloc(dataSize, sizeof(double));
        rData = calloc(dataSize, sizeof(double));

        // setup the connection
        port = (int)d[1];
        sizeMachineInet = 9+1;
        setupconnectionclient(&port, "127.0.0.1", &sizeMachineInet, &socketID);
        if (socketID < 0)
            return -1;

        // set the data size for the experimental site
        // sizeCtrl
        iData[0] = 1;  // disp
        iData[1] = 1;  // vel
        iData[2] = 1;  // accel
        iData[3] = 0;  // force
        iData[4] = 0;  // time
        // sizeDaq
        iData[5] = 0;  // disp
        iData[6] = 0;  // vel
        iData[7] = 0;  // accel
        iData[8] = 1;  // force
        iData[9] = 0;  // time
        // dataSize
        iData[10] = dataSize;

        gMsg = (char *)iData;
        dataTypeSize = sizeof(int);
        nleft = 11;
        senddata(&socketID, &dataTypeSize, gMsg, &nleft, &ierr);
    }
    // ==============================================================
    // check element for errors
    else if (isw == 2) {

        return 0;    
    }
    // ==============================================================
    // compute element residual and tangent matrix
    else if (isw == 3 || isw == 4 || isw == 6) {

        double kbInit, temp;
        double db, vb, ab;

        // compute element length and transformation matrix
        if (ndm == 1) {
            dx = xl[1] - xl[0];
            L = sqrt(dx*dx);
            cosX[0] = 1.0;
        }
        else if (ndm == 2) {
            dx = xl[2] - xl[0];
            dy = xl[3] - xl[1];
            L = sqrt(dx*dx + dy*dy);
            cosX[0] = dx/L;
            cosX[1] = dy/L;
        }
        else if (ndm == 3) {
            dx = xl[3] - xl[0];
            dy = xl[4] - xl[1];
            dz = xl[5] - xl[2];
            L = sqrt(dx*dx + dy*dy + dz*dz);
            cosX[0] = dx/L;
            cosX[1] = dy/L;
            cosX[2] = dz/L;
        }

        // compute the trial response in basic system
        db = vb = ab = 0;
        for (i=0; i<ndm; i++) {
            db += cosX[i]*(ul[ndm+i] - ul[i]);
            vb += cosX[i]*(uldot[ndm+i] - uldot[i]);
            ab += cosX[i]*(uldotdot[ndm+i] - uldotdot[i]);
        }

        // send trial response to experimental site
        sData[0] = 3;
        sData[1] = db;
        sData[2] = vb;
        sData[3] = ab;

        gMsg = (char *)sData;
        dataTypeSize = sizeof(double);
        nleft = dataSize;
        senddata(&socketID, &dataTypeSize, gMsg, &nleft, &ierr);

        // add stiffness portion to matrix
        if (isw == 3) {
            kbInit = d[0];
            for (i=0; i<ndm; i++) {
                for (j=0; j<ndm; j++) {
                    temp = cosX[i]*cosX[j]*kbInit;
                    s[i*nst+j] += temp;
                    s[i*nst+ndf+j] -= temp;
                    s[(i+ndm)*nst+j] -= temp;
                    s[(i+ndm)*nst+ndf+j] += temp;
                }
            }
        }

        // get measured resisting forces
        sData[0] = 10;

        gMsg = (char *)sData;
        nleft = dataSize;
        senddata(&socketID, &dataTypeSize, gMsg, &nleft, &ierr);    

        gMsg = (char *)rData;
        nleft = dataSize;
        recvdata(&socketID, &dataTypeSize, gMsg, &nleft, &ierr);    

        for (i=0; i<ndm; i++) {
            r[i] = -cosX[i]*rData[0];
            r[i+ndf] = cosX[i]*rData[0];
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

        double m = 0.5*d[2]*L;

        for (i=0; i<ndm; i++) {
            r[i] = m;
            r[i+ndf] = m;
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
        senddata(&socketID, &dataTypeSize, gMsg, &nleft, &ierr);

        closeconnection(&socketID, &ierr);

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
    double s[4][4];
    double r[4];

    int  nodeNum[2];
    double coord[4];
    double  temp[2];

    d[0] = 3000*5/135.76;
    d[1] = 8090;
    nodeNum[0] = 3; nodeNum[1] = 4;
    coord[0] = 168.0; coord[1] = 0.0; coord[2] = 72.0; coord[3] = 96.0;
    temp[0] = 0.0; temp[1] = 0.0;

    // setup
    eeTruss(d,disp,vel,accel,coord,nodeNum,temp,s,r,ndf,ndm,nst,1);

    // disp, stiff, force
    fprintf(stderr,"\nDISP:\n");

    disp[0] = 0.0; disp[1] = 0.0; disp[2] = 1.06017; disp[3] = -0.355778;
    for (i=0; i<4; i++) {
        vel[i] = 0.0;
        accel[i] = 0.0;
        for (j=0; j<4; j++)
            s[i][j] = 0.0;
        r[i] = 0.0;
        fprintf(stderr,"%f %f %f\n",disp[i],vel[i],accel[i]);
    }
    fprintf(stderr,"\n");

    eeTruss(d,disp,vel,accel,coord,nodeNum,temp,s,r,ndf,ndm,nst,3);
    for (i=0; i<4; i++) {
        fprintf(stderr,"%f %f %f\n",disp[i],vel[i],accel[i]);
    }
    fprintf(stderr,"\nFORCES:\n");
    for (j=0; j<4; j++)
        fprintf(stderr,"%f ",r[j]);
    fprintf(stderr,"\n");

    // shutdown
    eeTruss(d,disp,vel,accel,coord,nodeNum,temp,s,r,ndf,ndm,nst,10);

    return 0;
}
