// File: PredictorCorrector.c
// 
// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 11/04
// Revision: A
//
// Description: This file contains the implementation of the Predictor-Corrector

#include "PredictorCorrector.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

// global variables
int i, nDOF;
double dtCon, dtSim, dtInt, xi;

double *dsp1, *dsp2, *dsp3, *dsp4, *dsp5, *dsp6, *dspXi;
double *vel1, *vel2;
double *acc1;

//=============================================================================
int initData(int ndof, double dtcon, double dtsim, double dtint)
{
    nDOF = ndof;     // initialize number of DOF
    dtCon = dtcon;   // initialize controller time step size
    dtSim = dtsim;   // initialize simulation time step size
    dtInt = dtint;   // initialize integration time step size
    xi = 0;
    
    dsp1  = calloc(nDOF, sizeof(double));
    dsp2  = calloc(nDOF, sizeof(double));
    dsp3  = calloc(nDOF, sizeof(double));
    dsp4  = calloc(nDOF, sizeof(double));
    dsp5  = calloc(nDOF, sizeof(double));
    dsp6  = calloc(nDOF, sizeof(double));
    dspXi = calloc(nDOF, sizeof(double));
    
    if (!dsp1 || !dsp2 || !dsp3 || !dsp4  || !dsp5 || !dsp6 || !dspXi)  {
        printf("PredictorCorrector::initData - failed to create dsp vectors\n");
        return -1;
    }
    
    vel1 = calloc(nDOF, sizeof(double));
    vel2 = calloc(nDOF, sizeof(double));
    
    if (!vel1 || !vel2)  {
        printf("PredictorCorrector::initData - failed to create vel vectors\n");
        return -1;
    }
    
    acc1 = calloc(nDOF, sizeof(double));
    
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
    
    for (i=0; i<nDOF; i++)  {
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
    for (i=0; i<nDOF; i++)  {
        dsp[i] = 0;
    }
    
    return 0;
}

//=============================================================================
int setCurDsp(double *dsp, double x)
{
    xi = x;
    
    for (i=0; i<nDOF; i++)  {
        dspXi[i] = dsp[i];
    }
    
    return 0;
}

//=============================================================================
int setNewDsp(double *dsp)
{
    for (i=0; i<nDOF; i++)  {
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
int setNewVel(double *vel)
{
    for (i=0; i<nDOF; i++)  {
        vel2[i] = vel1[i];
        vel1[i] = dtInt*vel[i];
    }
    
    return 0;
}

//=============================================================================
int setNewAcc(double *acc)
{
    for (i=0; i<nDOF; i++)  {
        acc1[i] = dtInt*dtInt*acc[i];
    }
    
    return 0;
}

//=============================================================================
double getNumSubSteps(double vDOF, int dofID, double vDOFmax, int Nmin)
{
    double Nmaster, Nslave;
    
    dofID--;  // change to zero indexing
    Nmaster = ceil(fabs(dsp1[dofID] - dsp2[dofID])/(vDOF*dtCon));
    
    // none of the slaved actuators should exceed vDOFmax
    for (i=0; i<nDOF; i++)  {
        Nslave  = ceil(fabs(dsp1[i] - dsp2[i])/(vDOFmax*dtCon));
        Nmaster = (Nmaster>Nslave) ? Nmaster : Nslave;
    }
    
    Nmaster = (Nmaster>Nmin) ? Nmaster : Nmin;
    Nmaster = (Nmaster>10) ? Nmaster : 10;      // need at least 10 substeps
    
    return Nmaster;
}

//=============================================================================
int predictP0(double *dsp, double x)
{
    for (i=0; i<nDOF; i++)  {
        dsp[i] = dsp1[i];
    }
    
    return 0;
}

//=============================================================================
int predictP1(double *dsp, double x)
{
    for (i=0; i<nDOF; i++)  {
        dsp[i] = dsp1[i]*(1.0+x)
            - dsp2[i]*(x);
    }
    
    return 0;
}

//=============================================================================
int correctP1(double *dsp, double x)
{
    for (i=0; i<nDOF; i++)  {
        dsp[i] = dsp1[i]*(x)
            + dsp2[i]*(1.0-x);
    }
    
    return 0;
}

//=============================================================================
int predictP2(double *dsp, double x)
{
    for (i=0; i<nDOF; i++)  {
        dsp[i] = dsp1[i]*(1.0+x)*(2.0+x)/(2.0)
            - dsp2[i]*(x)*(2.0+x)
            + dsp3[i]*(x)*(1.0+x)/(2.0);
    }
    
    return 0;
}

//=============================================================================
int correctP2(double *dsp, double x)
{
    for (i=0; i<nDOF; i++)  {
        dsp[i] = dsp1[i]*(x)*(1.0+x)/(2.0)
            + dsp2[i]*(1.0-x)*(1.0+x)
            - dsp3[i]*(1.0-x)*(x)/(2.0);
    }
    
    return 0;
}

//=============================================================================
int predictP3(double *dsp, double x)
{
    for (i=0; i<nDOF; i++)  {
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
    for (i=0; i<nDOF; i++)  {
        dsp[i] = dsp1[i]*(x)*(1.0+x)*(2.0+x)/(6.0)
            + dsp2[i]*(1.0-x)*(1.0+x)*(2.0+x)/(2.0)
            - dsp3[i]*(1.0-x)*(x)*(2.0+x)/(2.0)
            + dsp4[i]*(1.0-x)*(x)*(1.0+x)/(6.0);
    }
    
    return 0;
}

//=============================================================================
int predictD0(double *dsp, double x)
{
    for (i=0; i<nDOF; i++)  {
        dsp[i] = dsp1[i];
    }
    
    return 0;
}

//=============================================================================
int predictD1(double *dsp, double x)
{
    for (i=0; i<nDOF; i++)  {
        dsp[i] = dspXi[i]*(1.0+x)/(xi)
            - dsp2[i]*(1.0+x-xi)/(xi);
    }
    
    return 0;
}

//=============================================================================
int predictV1(double *vel, double x)
{
    for (i=0; i<nDOF; i++)  {
        vel[i] = (dspXi[i] - dsp2[i])/(xi);
        vel[i] /= dtSim;
    }
    
    return 0;
}

//=============================================================================
int predictA1(double *acc, double x)
{
    for (i=0; i<nDOF; i++)  {
        acc[i] = 0.0;
    }
    
    return 0;
}

//=============================================================================
int correctD1(double *dsp, double x)
{
    for (i=0; i<nDOF; i++)  {
        dsp[i] = dsp1[i]*(x-xi)/(1.0-xi)
            + dspXi[i]*(1.0-x)/(1.0-xi);
    }
    
    return 0;
}

//=============================================================================
int correctV1(double *vel, double x)
{
    for (i=0; i<nDOF; i++)  {
        vel[i] = (dsp1[i] - dspXi[i])/(1.0-xi);
        vel[i] /= dtSim;
    }
    
    return 0;
}

//=============================================================================
int correctA1(double *acc, double x)
{
    for (i=0; i<nDOF; i++)  {
        acc[i] = 0.0;
    }
    
    return 0;
}

//=============================================================================
int predictD2(double *dsp, double x)
{
    for (i=0; i<nDOF; i++)  {
        dsp[i] = dspXi[i]*(1.0+x)*(2.0+x)/(xi)/(1.0+xi)
            - dsp2[i]*(1.0+x-xi)*(2.0+x)/(xi)
            + dsp3[i]*(1.0+x-xi)*(1.0+x)/(1.0+xi);
    }
    
    return 0;
}

//=============================================================================
int predictV2(double *vel, double x)
{
    for (i=0; i<nDOF; i++)  {
        vel[i] = dspXi[i]*(3.0+2.0*x)/(xi)/(1.0+xi)
            - dsp2[i]*(3.0+2.0*x-xi)/(xi)
            + dsp3[i]*(2.0+2.0*x-xi)/(1.0+xi);
        vel[i] /= dtSim;
    }
    
    return 0;
}

//=============================================================================
int predictA2(double *acc, double x)
{
    for (i=0; i<nDOF; i++)  {
        acc[i] = dspXi[i]*(2.0)/(xi)/(1.0+xi)
            - dsp2[i]*(2.0)/(xi)
            + dsp3[i]*(2.0)/(1.0+xi);
        acc[i] /= (dtSim*dtSim);
    }
    
    return 0;
}

//=============================================================================
int correctD2(double *dsp, double x)
{
    for (i=0; i<nDOF; i++)  {
        dsp[i] = dsp1[i]*(x-xi)*(1.0+x)/(2.0)/(1.0-xi)
            + dspXi[i]*(1.0-x)*(1.0+x)/(1.0-xi)/(1.0+xi)
            - dsp3[i]*(1.0-x)*(x-xi)/(2.0)/(1.0+xi);
    }
    
    return 0;
}

//=============================================================================
int correctV2(double *vel, double x)
{
    for (i=0; i<nDOF; i++)  {
        vel[i] = dsp1[i]*(1.0+2.0*x-xi)/(2.0)/(1.0-xi)
            - dspXi[i]*(2.0*x)/(1.0-xi)/(1.0+xi)
            - dsp3[i]*(1.0-2.0*x+xi)/(2.0)/(1.0+xi);
        vel[i] /= dtSim;
    }
    
    return 0;
}

//=============================================================================
int correctA2(double *acc, double x)
{
    for (i=0; i<nDOF; i++)  {
        acc[i] = dsp1[i]/(1.0-xi)
            - dspXi[i]*(2.0)/(1.0-xi)/(1.0+xi)
            + dsp3[i]/(1.0+xi);
        acc[i] /= (dtSim*dtSim);
    }
    
    return 0;
}

//=============================================================================
int predictD3(double *dsp, double x)
{
    for (i=0; i<nDOF; i++)  {
        dsp[i] = dspXi[i]*(1.0+x)*(2.0+x)*(3.0+x)/(xi)/(1.0+xi)/(2.0+xi)
            - dsp2[i]*(1.0+x-xi)*(2.0+x)*(3.0+x)/(2.0*xi)
            + dsp3[i]*(1.0+x-xi)*(1.0+x)*(3.0+x)/(1.0+xi)
            - dsp4[i]*(1.0+x-xi)*(1.0+x)*(2.0+x)/(2.0)/(2.0+xi);
    }
    
    return 0;
}

//=============================================================================
int predictV3(double *vel, double x)
{
    for (i=0; i<nDOF; i++)  {
        vel[i] = dspXi[i]*(11.0+12.0*x+3.0*x*x)/(xi)/(1.0+xi)/(2.0+xi)
            - dsp2[i]*(11.0+12.0*x+3.0*x*x-5.0*xi-2.0*x*xi)/(2.0*xi)
            + dsp3[i]*(7.0+10.0*x+3.0*x*x-4.0*xi-2.0*x*xi)/(1.0+xi)
            - dsp4[i]*(5.0+8.0*x+3.0*x*x-3.0*xi-2.0*x*xi)/(2.0)/(2.0+xi);
        vel[i] /= dtSim;
    }
    
    return 0;
}

//=============================================================================
int predictA3(double *acc, double x)
{
    for (i=0; i<nDOF; i++)  {
        acc[i] = dspXi[i]*(6.0)*(2.0+x)/(xi)/(1.0+xi)/(2.0+xi)
            - dsp2[i]*(6.0+3.0*x-xi)/(xi)
            + dsp3[i]*(2.0)*(5.0+3.0*x-xi)/(1.0+xi)
            - dsp4[i]*(4.0+3.0*x-xi)/(2.0+xi);
        acc[i] /= (dtSim*dtSim);
    }
    
    return 0;
}

//=============================================================================
int correctD3(double *dsp, double x)
{
    for (i=0; i<nDOF; i++)  {
        dsp[i] = dsp1[i]*(x-xi)*(1.0+x)*(2.0+x)/(6.0)/(1-xi)
            + dspXi[i]*(1.0-x)*(1.0+x)*(2.0+x)/(1.0-xi)/(1.0+xi)/(2.0+xi)
            - dsp3[i]*(1.0-x)*(x-xi)*(2.0+x)/(2.0)/(1.0+xi)
            + dsp4[i]*(1.0-x)*(x-xi)*(1.0+x)/(3.0)/(2.0+xi);
    }
    
    return 0;
}

//=============================================================================
int correctV3(double *vel, double x)
{
    for (i=0; i<nDOF; i++)  {
        vel[i] = dsp1[i]*(2.0+6.0*x+3.0*x*x-3.0*xi-2.0*x*xi)/(6.0)/(1-xi)
            + dspXi[i]*(1.0-4.0*x-3.0*x*x)/(1.0-xi)/(1.0+xi)/(2.0+xi)
            - dsp3[i]*(2.0-2.0*x-3.0*x*x+xi+2.0*x*xi)/(2.0)/(1.0+xi)
            + dsp4[i]*(1.0-3.0*x*x+2.0*x*xi)/(3.0)/(2.0+xi);
        vel[i] /= dtSim;
    }
    
    return 0;
}


//=============================================================================
int correctA3(double *acc, double x)
{
    for (i=0; i<nDOF; i++)  {
        acc[i] = dsp1[i]*(3.0+3.0*x-xi)/(3.0)/(1-xi)
            - dspXi[i]*(2.0)*(2.0+3.0*x)/(1.0-xi)/(1.0+xi)/(2.0+xi)
            + dsp3[i]*(1.0+3.0*x-xi)/(1.0+xi)
            - dsp4[i]*(2.0)*(3.0*x-xi)/(3.0)/(2.0+xi);
        acc[i] /= (dtSim*dtSim);
    }
    
    return 0;
}

//=============================================================================
int predictDV(double *dsp, double x)
{
    for (i=0; i<nDOF; i++)  {
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
    for (i=0; i<nDOF; i++)  {
        dsp[i] = dsp1[i]*(x-xi)*(2.0-x-xi)/(1.0-xi)/(1.0-xi)
            - vel1[i]*(1.0-x)*(x-xi)/(1.0-xi)
            + dspXi[i]*(1.0-x)*(1.0-x)/(1.0-xi)/(1.0-xi);
    }
    
    return 0;
}

//=============================================================================
int predictDVA(double *dsp, double x)
{
    for (i=0; i<nDOF; i++)  {
        dsp[i] = dsp1[i]
            + vel1[i]*(x)
            + acc1[i]*(x)*(x)/(2.0);
    }
    
    return 0;
}

//=============================================================================
int correctDVA(double *dsp, double x)
{
    for (i=0; i<nDOF; i++)  {
        dsp[i] = dsp1[i]*(x-xi)*(3.0-3.0*x+x*x-3.0*xi+x*xi+xi*xi)/(1.0-xi)/(1.0-xi)/(1.0-xi)
            - vel1[i]*(1.0-x)*(x-xi)*(2.0-x-xi)/(1.0-xi)/(1.0-xi)
            + acc1[i]*(1.0-x)*(1.0-x)*(x-xi)/(2.0)/(1.0-xi)
            + dspXi[i]*(1.0-x)*(1.0-x)*(1.0-x)/(1.0-xi)/(1.0-xi)/(1.0-xi);
    }
    
    return 0;
}

//=============================================================================
