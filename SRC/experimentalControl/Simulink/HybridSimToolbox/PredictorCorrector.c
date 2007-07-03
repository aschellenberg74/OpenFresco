// File: PredictorCorrector.c
// 
// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 11/04
// Revision: A
//
// Description: This file contains the implementation of the PredictorCorrector

#include "PredictorCorrector.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

//=============================================================================
int initData(int n, double dtcon)
{
    nAct = n;        // initialize number of actuators
    dtCon = dtcon;   // initialize controller time step
    xi = 0;

    dsp1  = calloc(nAct, sizeof(double));
    dsp2  = calloc(nAct, sizeof(double));
    dsp3  = calloc(nAct, sizeof(double));
    dsp4  = calloc(nAct, sizeof(double));
    dsp5  = calloc(nAct, sizeof(double));
    dsp6  = calloc(nAct, sizeof(double));
    dspXi = calloc(nAct, sizeof(double));

    if (!dsp1 || !dsp2 || !dsp3 || !dsp4  || !dsp5 || !dsp6 || !dspXi)  {
        printf("PredictorCorrector::initData - failed to create dsp vectors\n");
        return -1;
    }	

    vel1 = calloc(nAct, sizeof(double));
    vel2 = calloc(nAct, sizeof(double));

    if (!vel1 || !vel2)  {
        printf("PredictorCorrector::initData - failed to create vel vectors\n");
        return -1;
    }	

    acc1 = calloc(nAct, sizeof(double));

    if (!acc1)  {
        printf("PredictorCorrector::initData - failed to create accData vectors\n");
        return -1;
    }

    zeroData();

    return 0;
}

//=============================================================================
int zeroData()
{
    xi = 0;

    for (i=0; i<nAct; i++)  {
        dsp1[i] = 0;  dsp2[i] = 0;  dsp3[i] = 0;
        dsp4[i] = 0;  dsp5[i] = 0;  dsp6[i] = 0; dspXi[i] = 0;
        vel1[i] = 0;  vel2[i] = 0;
        acc1[i] = 0;
    }

    return 0;
}

//=============================================================================
int zeroDsp(double *dsp)
{
    for (i=0; i<nAct; i++)  {
        dsp[i] = 0;
    }

    return 0;
}

//=============================================================================
int setCurDsp(double *dsp, double x)
{
    xi = x;

    for (i=0; i<nAct; i++)  {
        dspXi[i] = dsp[i];
    }

    return 0;
}

//=============================================================================
int setNewDsp(double *dsp)
{
    for (i=0; i<nAct; i++)  {
        // update displacements
        dsp6[i] = dsp5[i];
        dsp5[i] = dsp4[i];
        dsp4[i] = dsp3[i];
        dsp3[i] = dsp2[i];
        dsp2[i] = dsp1[i];
        dsp1[i] = dsp[i];

        // update velocities O(h^2)
        //vel1[i] = 1.0/2.0*(3.0*dsp1[i] - 4.0*dsp2[i] + dsp3[i]);
        //vel2[i] = 1.0/2.0*(dsp1[i] - dsp3[i]);

        // update velocities O(h^3)
        //vel1[i] = 1.0/6.0*(11.0*dsp1[i] - 18.0*dsp2[i] 
        //    + 9.0*dsp3[i] - 2.0*dsp4[i]);
        //vel2[i] = 1.0/6.0*(2.0*dsp1[i] + 3.0*dsp2[i]
        //    - 6.0*dsp3[i] + dsp4[i]);

        // update velocities O(h^4)
        vel1[i] = 1.0/12.0*(25.0*dsp1[i] - 48.0*dsp2[i]
            + 36.0*dsp3[i] - 16.0*dsp4[i] + 3.0*dsp5[i]);
        vel2[i] = 1.0/12.0*(3.0*dsp1[i] + 10.0*dsp2[i]
            - 18.0*dsp3[i] + 6.0*dsp4[i] - dsp5[i]);

        // update acceleration O(h)
        //acc1[i] = dsp1[i] - 2.0*dsp2[i] + dsp3[i];

        // update acceleration O(h^2)
        //acc1[i] = 2.0*dsp1[i] - 5.0*dsp2[i] + 4.0*dsp3[i] - dsp4[i];

        // update acceleration O(h^3)
        //acc1[i] = 1.0/12.0*(35.0*dsp1[i] - 104.0*dsp2[i]
        //    + 114.0*dsp3[i] - 56.0*dsp4[i] + 11.0*dsp5[i]);

        // update acceleration O(h^4)
        acc1[i] = 1.0/12.0*(45.0*dsp1[i] - 154.0*dsp2[i]
            + 214.0*dsp3[i] - 156.0*dsp4[i] + 61.0*dsp5[i]
            - 10.0*dsp6[i]);
    }

    return 0;
}

//=============================================================================
int setNewVel(double *vel, double dtInt)
{
    for (i=0; i<nAct; i++)  {
        vel2[i] = vel1[i];
        vel1[i] = dtInt*vel[i];
    }

    return 0;
}

//=============================================================================
int setNewAcc(double *acc, double dtInt)
{
    for (i=0; i<nAct; i++)  {
        acc1[i] = dtInt*dtInt*acc[i];
    }

    return 0;
}

//=============================================================================
double getNumSubSteps(double vAct, int actID, double vActMax, int Nmin)
{
    double Nmaster, Nslave;

    actID--;  // change to zero indexing
    Nmaster = ceil(fabs(dsp1[actID] - dsp2[actID])/(vAct*dtCon));

    // none of the slaved actuators should exceed vActMax
    for (i=0; i<nAct; i++)  {
        Nslave  = ceil(fabs(dsp1[i] - dsp2[i])/(vActMax*dtCon));
        Nmaster = (Nmaster>Nslave) ? Nmaster : Nslave;
    }

    Nmaster = (Nmaster>Nmin) ? Nmaster : Nmin;
    Nmaster = (Nmaster>10) ? Nmaster : 10;      // need at least 10 substeps

    return Nmaster;
}

//=============================================================================
int predictP1(double *dsp, double x)
{
    for (i=0; i<nAct; i++)  {
        dsp[i] = dsp1[i]*(1.0+x)
            - dsp2[i]*(x);
    }

    return 0;
}

//=============================================================================
int correctP1(double *dsp, double x)
{
    for (i=0; i<nAct; i++)  {
        dsp[i] = dsp1[i]*(x)
            - dsp2[i]*(x-1.0);
    }

    return 0;
}

//=============================================================================
int predictP2(double *dsp, double x)
{
    for (i=0; i<nAct; i++)  {
        dsp[i] = dsp1[i]*(1.0+x)*(2.0+x)/(2.0)
            - dsp2[i]*(x)*(2.0+x)
            + dsp3[i]*(x)*(1.0+x)/(2.0);
    }

    return 0;
}

//=============================================================================
int correctP2(double *dsp, double x)
{
    for (i=0; i<nAct; i++)  {
        dsp[i] = dsp1[i]*(x)*(1.0+x)/(2.0)
            - dsp2[i]*(x-1.0)*(x+1.0)
            + dsp3[i]*(x)*(x-1.0)/(2.0);
    }

    return 0;
}

//=============================================================================
int predictP3(double *dsp, double x)
{
    for (i=0; i<nAct; i++)  {
        dsp[i] = dsp1[i]*(1.0+x)*(2.0+x)*(3.0+x)/(6.0)
            - dsp2[i]*(x)*(2.0+x)*(3.0+x)/(2.0)
            + dsp3[i]*(x)*(1.0+x)*(3.0+x)/(2.0)
            - dsp4[i]*(x)*(1.0+x)*(2.0+x)/(6.0);
    }

    return 0;
}

//=============================================================================
int correctP3(double *dsp, double x)
{
    for (i=0; i<nAct; i++)  {    
        dsp[i] = dsp1[i]*(x)*(1.0+x)*(2.0+x)/(6.0)
            - dsp2[i]*(x-1.0)*(x+1.0)*(x+2.0)/(2.0)
            + dsp3[i]*(x)*(x-1.0)*(x+2.0)/(2.0)
            - dsp4[i]*(x)*(x-1.0)*(x+1.0)/(6.0);
    }

    return 0;
}

//=============================================================================
int predictD0(double *dsp, double x)
{
    for (i=0; i<nAct; i++)  {
        dsp[i] = dsp1[i];
    }

    return 0;
}

//=============================================================================
int predictD1(double *dsp, double x)
{
    for (i=0; i<nAct; i++)  {
        dsp[i] = dspXi[i]*(1.0+x)/(xi)
            - dsp2[i]*(1.0+x-xi)/(xi);
    }

    return 0;
}

//=============================================================================
int correctD1(double *dsp, double x)
{
    for (i=0; i<nAct; i++)  {
        dsp[i] = dsp1[i]*(x-xi)/(1.0-xi)
            + dspXi[i]*(x-1.0)/(xi-1.0);
    }

    return 0;
}

//=============================================================================
int predictD2(double *dsp, double x)
{
    for (i=0; i<nAct; i++)  {
//        dsp[i] = dsp1[i]*(1.0+x)*(2.0+x)/(2.0)
//            - dsp2[i]*(x)*(2.0+x)
//            + dsp3[i]*(x)*(1.0+x)/(2.0);
        dsp[i] = dspXi[i]*(1.0+x)*(2.0+x)/(xi)/(1.0+xi)
            - dsp2[i]*(2.0+x)*(1.0+x-xi)/(xi)
            + dsp3[i]*(1.0+x)*(1.0+x-xi)/(1.0+xi);
    }

    return 0;
}

//=============================================================================
int correctD2(double *dsp, double x)
{
    for (i=0; i<nAct; i++)  {
        dsp[i] = dsp1[i]*(x)*(x-xi)/(1.0-xi)
            + dspXi[i]*(x-1.0)*(x)/(xi-1.0)/(xi)
            + dsp2[i]*(x-1.0)*(x-xi)/(xi);
    }

    return 0;
}

//=============================================================================
int predictD3(double *dsp, double x)
{
    for (i=0; i<nAct; i++)  {
//        dsp[i] = dsp1[i]*(1.0+x)*(2.0+x)*(3.0+x)/(6.0)
//            - dsp2[i]*(x)*(2.0+x)*(3.0+x)/(2.0)
//            + dsp3[i]*(x)*(1.0+x)*(3.0+x)/(2.0)
//            - dsp4[i]*(x)*(1.0+x)*(2.0+x)/(6.0);
        dsp[i] = dspXi[i]*(1.0+x)*(2.0+x)*(3.0+x)/(xi)/(1.0+xi)/(2.0+xi)
            - dsp2[i]*(x)*(2.0+x)*(3.0+x)*(1.0+x-xi)/(2.0*xi)
            + dsp3[i]*(x)*(1.0+x)*(3.0+x)*(1.0+x-xi)/(1.0+xi)
            - dsp4[i]*(x)*(1.0+x)*(2.0+x)*(1.0+x-xi)/(2.0)/(2.0+xi);
    }

    return 0;
}

//=============================================================================
int correctD3(double *dsp, double x)
{
    for (i=0; i<nAct; i++)  {    
        dsp[i] = dsp1[i]*(x)*(1.0+x)*(x-xi)/(2.0)/(1-xi)
            + dspXi[i]*(x-1.0)*(x)*(1.0+x)/(xi-1.0)/(xi)/(1.0+xi)
            + dsp2[i]*(x-1.0)*(1.0+x)*(x-xi)/(xi)
            - dsp3[i]*(x-1.0)*(x)*(x-xi)/(2.0)/(1.0+xi);
    }

    return 0;
}

//=============================================================================
int predictDV(double *dsp, double x)
{
    for (i=0; i<nAct; i++)  {
        dsp[i] = dsp1[i]*(1.0+x)*(1.0+x)*(1.0-2.0*x)
            + vel1[i]*(x)*(1.0+x)*(1.0+x) 
            + dsp2[i]*(x)*(x)*(3.0+2.0*x)
            + vel2[i]*(x)*(x)*(1.0+x);
    }

    return 0;
}

//=============================================================================
int correctDV(double *dsp, double x)
{
    for (i=0; i<nAct; i++)  {
        dsp[i] = dsp1[i]*(-x+xi)*(-2.0+x+xi)/(-1.0+xi)/(-1.0+xi)
            - vel1[i]*(-1.0+x)*(x-xi)/(-1.0+xi)
            + dspXi[i]*(-1.0+x)*(-1.0+x)/(-1.0+xi)/(-1.0+xi);
    }

    return 0;
}

//=============================================================================
int predictDVA(double *dsp, double x)
{
    for (i=0; i<nAct; i++)  {
        dsp[i] = dsp1[i]
            + vel1[i]*(x)
            + acc1[i]*(x)*(x)/(2.0);
    }

    return 0;
}

//=============================================================================
int correctDVA(double *dsp, double x)
{
    for (i=0; i<nAct; i++)  {
        dsp[i] = dsp1[i]*(-x+xi)*(3.0-3.0*x+x*x-3.0*xi+x*xi+xi*xi)/(-1.0+xi)/(-1.0+xi)/(-1.0+xi)
            - vel1[i]*(-1.0+x)*(x-xi)*(-2.0+x+xi)/(-1.0+xi)/(-1.0+xi)
            - acc1[i]*(-1.0+x)*(-1.0+x)*(x-xi)/(2.0)/(-1.0+xi)
            + dspXi[i]*(-1.0+x)*(-1.0+x)*(-1.0+x)/(-1.0+xi)/(-1.0+xi)/(-1.0+xi);
    }

    return 0;
}

//=============================================================================
