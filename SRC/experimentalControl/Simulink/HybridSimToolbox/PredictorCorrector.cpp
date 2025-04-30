// File: PredictorCorrector.cpp
// 
// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 11/04
// Revision: A
//
// Description: This file contains the implementation of the Predictor-Corrector
//
// Modified: Akiri Seki (sekia@stanford.edu)
// Date: 01/2023
// Description: This file is modified to be in cpp format.


#include "PredictorCorrector.hpp"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>


// constructor
PredictorCorrector::PredictorCorrector()
{
    // does nothing
}


// destructor
PredictorCorrector::~PredictorCorrector()
{
    // does nothing
}


int PredictorCorrector::initData(int ndof, double dtcon, double dtsim, double dtint)
{
    nDOF = ndof;     // initialize number of DOF
    dtCon = dtcon;   // initialize controller time step size
    dtSim = dtsim;   // initialize simulation time step size
    dtInt = dtint;   // initialize integration time step size
    
    sig1  = (double*)calloc(nDOF, sizeof(double));
    sig2  = (double*)calloc(nDOF, sizeof(double));
    sig3  = (double*)calloc(nDOF, sizeof(double));
    sig4  = (double*)calloc(nDOF, sizeof(double));
    sig5  = (double*)calloc(nDOF, sizeof(double));
    sig6  = (double*)calloc(nDOF, sizeof(double));
    sigXi = (double*)calloc(nDOF, sizeof(double));
    
    if (!sig1 || !sig2 || !sig3 || !sig4  || !sig5 || !sig6 || !sigXi)  {
        printf("PredictorCorrector::initData - failed to create sig vectors\n");
        return -1;
    }
    
    sigDot1 = (double*)calloc(nDOF, sizeof(double));
    sigDot2 = (double*)calloc(nDOF, sizeof(double));
    
    if (!sigDot1 || !sigDot2)  {
        printf("PredictorCorrector::initData - failed to create sigDot vectors\n");
        return -1;
    }
    
    sigDotDot1 = (double*)calloc(nDOF, sizeof(double));
    
    if (!sigDotDot1)  {
        printf("PredictorCorrector::initData - failed to create sigDotDot vectors\n");
        return -1;
    }
    
    zeroData();
    
    return 0;
}


int PredictorCorrector::zeroData()
{
    xi = 0.0;
    
    for (i=0; i<nDOF; i++)  {
        sig1[i] = sig2[i] = sig3[i] = 0.0;
        sig4[i] = sig5[i] = sig6[i] = 0.0;
        sigXi[i] = 0.0;
        sigDot1[i] = sigDot2[i] = 0.0;
        sigDotDot1[i] = 0.0;
    }
    
    return 0;
}


int PredictorCorrector::setData(double* sig)
{
    for (i = 0; i < nDOF; i++) {
        sig1[i] = sig2[i] = sig3[i] = sig[i];
        sig4[i] = sig5[i] = sig6[i] = sig[i];
        sigXi[i] = sig[i];
    }
    
    return 0;
}


int PredictorCorrector::setDataDot(double* sigDot)
{
    for (i = 0; i < nDOF; i++) {
        sigDot1[i] = sigDot2[i] = dtInt*sigDot[i];
    }
    
    return 0;
}


int PredictorCorrector::setDataDotDot(double* sigDotDot)
{
    for (i = 0; i < nDOF; i++) {
        sigDotDot1[i] = dtInt*sigDotDot[i];
    }
    
    return 0;
}


int PredictorCorrector::setCurSig(double* sig, double x)
{
    xi = x;
    
    for (i=0; i<nDOF; i++)  {
        sigXi[i] = sig[i];
    }
    
    return 0;
}


int PredictorCorrector::setNewSig(double* sig)
{
    for (i=0; i<nDOF; i++)  {
        // update signals
        sig6[i] = sig5[i];
        sig5[i] = sig4[i];
        sig4[i] = sig3[i];
        sig3[i] = sig2[i];
        sig2[i] = sig1[i];
        sig1[i] = sig[i];
        
        // update signal derivatives O(h^2)
        //sigDot1[i] = 1.0/2.0*(3.0*sig1[i] - 4.0*sig2[i] + sig3[i]);
        //sigDot2[i] = 1.0/2.0*(sig1[i] - sig3[i]);
        
        // update signal derivatives O(h^3)
        //sigDot1[i] = 1.0/6.0*(11.0*sig1[i] - 18.0*sig2[i] 
        //    + 9.0*sig3[i] - 2.0*sig4[i]);
        //sigDot2[i] = 1.0/6.0*(2.0*sig1[i] + 3.0*sig2[i]
        //    - 6.0*sig3[i] + sig4[i]);
        
        // update signal derivatives O(h^4)
        sigDot1[i] = 1.0/12.0*(25.0*sig1[i] - 48.0*sig2[i]
            + 36.0*sig3[i] - 16.0*sig4[i] + 3.0*sig5[i]);
        sigDot2[i] = 1.0/12.0*(3.0*sig1[i] + 10.0*sig2[i]
            - 18.0*sig3[i] + 6.0*sig4[i] - sig5[i]);
        
        // update signal double derivatives O(h)
        //sigDotDot1[i] = sig1[i] - 2.0*sig2[i] + sig3[i];
        
        // update signal double derivatives O(h^2)
        //sigDotDot1[i] = 2.0*sig1[i] - 5.0*sig2[i] + 4.0*sig3[i] - sig4[i];
        
        // update signal double derivatives O(h^3)
        //sigDotDot1[i] = 1.0/12.0*(35.0*sig1[i] - 104.0*sig2[i]
        //    + 114.0*sig3[i] - 56.0*sig4[i] + 11.0*sig5[i]);
        
        // update signal double derivatives O(h^4)
        sigDotDot1[i] = 1.0/12.0*(45.0*sig1[i] - 154.0*sig2[i]
            + 214.0*sig3[i] - 156.0*sig4[i] + 61.0*sig5[i]
            - 10.0*sig6[i]);
    }
    
    return 0;
}


int PredictorCorrector::setNewSigDot(double* sigDot)
{
    for (i=0; i<nDOF; i++)  {
        sigDot2[i] = sigDot1[i];
        sigDot1[i] = dtInt*sigDot[i];
    }
    
    return 0;
}


int PredictorCorrector::setNewSigDotDot(double* sigDotDot)
{
    for (i=0; i<nDOF; i++)  {
        sigDotDot1[i] = dtInt*dtInt*sigDotDot[i];
    }
    
    return 0;
}


int PredictorCorrector::zeroSig(double* sig)
{
    for (i = 0; i < nDOF; i++) {
        sig[i] = 0;
    }
    
    return 0;
}


int PredictorCorrector::setSig(double* sig, double* x)
{
    for (i = 0; i < nDOF; i++) {
        sig[i] = x[i];
    }
    
    return 0;
}


int PredictorCorrector::setSigDot(double* sigDot, double* x)
{
    for (i = 0; i < nDOF; i++) {
        sigDot[i] = dtInt * x[i];
    }
    
    return 0;
}


int PredictorCorrector::setSigDotDot(double* sigDotDot, double* x)
{
    for (i = 0; i < nDOF; i++) {
        sigDotDot[i] = dtInt * dtInt * x[i];
    }
    
    return 0;
}


double PredictorCorrector::getNumSubSteps(double sigDot, int dofID, double sigDotMax, int Nmin)
{
    double Nmaster, Nslave;
    
    dofID--;  // change to zero indexing
    Nmaster = ceil(fabs(sig1[dofID] - sig2[dofID])/(sigDot*dtCon));
    
    // none of the slaved DOF should exceed sigDotMax
    for (i=0; i<nDOF; i++)  {
        Nslave  = ceil(fabs(sig1[i] - sig2[i])/(sigDotMax*dtCon));
        Nmaster = (Nmaster>Nslave) ? Nmaster : Nslave;
    }
    
    Nmaster = (Nmaster>Nmin) ? Nmaster : Nmin;
    Nmaster = (Nmaster>10) ? Nmaster : 10;      // need at least 10 substeps
    
    return Nmaster;
}


int PredictorCorrector::predictP0(double* sig, double x)
{
    for (i=0; i<nDOF; i++)  {
        sig[i] = sig1[i];
    }
    
    return 0;
}


int PredictorCorrector::predictP1(double* sig, double x)
{
    for (i=0; i<nDOF; i++)  {
        sig[i] = sig1[i]*(1.0+x)
            - sig2[i]*(x);
    }
    
    return 0;
}


int PredictorCorrector::correctP1(double* sig, double x)
{
    for (i=0; i<nDOF; i++)  {
        sig[i] = sig1[i]*(x)
            + sig2[i]*(1.0-x);
    }
    
    return 0;
}


int PredictorCorrector::predictP2(double* sig, double x)
{
    for (i=0; i<nDOF; i++)  {
        sig[i] = sig1[i]*(1.0+x)*(2.0+x)/(2.0)
            - sig2[i]*(x)*(2.0+x)
            + sig3[i]*(x)*(1.0+x)/(2.0);
    }
    
    return 0;
}


int PredictorCorrector::correctP2(double* sig, double x)
{
    for (i=0; i<nDOF; i++)  {
        sig[i] = sig1[i]*(x)*(1.0+x)/(2.0)
            + sig2[i]*(1.0-x)*(1.0+x)
            - sig3[i]*(1.0-x)*(x)/(2.0);
    }
    
    return 0;
}


int PredictorCorrector::predictP3(double* sig, double x)
{
    for (i=0; i<nDOF; i++)  {
        sig[i] = sig1[i]*(1.0+x)*(2.0+x)*(3.0+x)/(6.0)
            - sig2[i]*(x)*(2.0+x)*(3.0+x)/(2.0)
            + sig3[i]*(x)*(1.0+x)*(3.0+x)/(2.0)
            - sig4[i]*(x)*(1.0+x)*(2.0+x)/(6.0);
    }
    
    return 0;
}


int PredictorCorrector::correctP3(double* sig, double x)
{
    for (i=0; i<nDOF; i++)  {
        sig[i] = sig1[i]*(x)*(1.0+x)*(2.0+x)/(6.0)
            + sig2[i]*(1.0-x)*(1.0+x)*(2.0+x)/(2.0)
            - sig3[i]*(1.0-x)*(x)*(2.0+x)/(2.0)
            + sig4[i]*(1.0-x)*(x)*(1.0+x)/(6.0);
    }
    
    return 0;
}


int PredictorCorrector::predictD0(double* sig, double x)
{
    for (i=0; i<nDOF; i++)  {
        sig[i] = sig1[i];
    }
    
    return 0;
}


int PredictorCorrector::predictD1(double* sig, double x)
{
    for (i=0; i<nDOF; i++)  {
        sig[i] = sigXi[i]*(1.0+x)/(xi)
            - sig2[i]*(1.0+x-xi)/(xi);
    }
    
    return 0;
}


int PredictorCorrector::predictV1(double* sigDot, double x)
{
    for (i=0; i<nDOF; i++)  {
        sigDot[i] = (sigXi[i] - sig2[i])/(xi);
        sigDot[i] /= dtSim;
    }
    
    return 0;
}


int PredictorCorrector::predictA1(double* sigDotDot, double x)
{
    for (i=0; i<nDOF; i++)  {
        sigDotDot[i] = 0.0;
    }
    
    return 0;
}


int PredictorCorrector::correctD1(double* sig, double x)
{
    for (i=0; i<nDOF; i++)  {
        sig[i] = sig1[i]*(x-xi)/(1.0-xi)
            + sigXi[i]*(1.0-x)/(1.0-xi);
    }
    
    return 0;
}


int PredictorCorrector::correctV1(double* sigDot, double x)
{
    for (i=0; i<nDOF; i++)  {
        sigDot[i] = (sig1[i] - sigXi[i])/(1.0-xi);
        sigDot[i] /= dtSim;
    }
    
    return 0;
}


int PredictorCorrector::correctA1(double* sigDotDot, double x)
{
    for (i=0; i<nDOF; i++)  {
        sigDotDot[i] = 0.0;
    }
    
    return 0;
}


int PredictorCorrector::predictD2(double* sig, double x)
{
    for (i=0; i<nDOF; i++)  {
        sig[i] = sigXi[i]*(1.0+x)*(2.0+x)/(xi)/(1.0+xi)
            - sig2[i]*(1.0+x-xi)*(2.0+x)/(xi)
            + sig3[i]*(1.0+x-xi)*(1.0+x)/(1.0+xi);
    }
    
    return 0;
}


int PredictorCorrector::predictV2(double* sigDot, double x)
{
    for (i=0; i<nDOF; i++)  {
        sigDot[i] = sigXi[i]*(3.0+2.0*x)/(xi)/(1.0+xi)
            - sig2[i]*(3.0+2.0*x-xi)/(xi)
            + sig3[i]*(2.0+2.0*x-xi)/(1.0+xi);
        sigDot[i] /= dtSim;
    }
    
    return 0;
}


int PredictorCorrector::predictA2(double* sigDotDot, double x)
{
    for (i=0; i<nDOF; i++)  {
        sigDotDot[i] = sigXi[i]*(2.0)/(xi)/(1.0+xi)
            - sig2[i]*(2.0)/(xi)
            + sig3[i]*(2.0)/(1.0+xi);
        sigDotDot[i] /= (dtSim*dtSim);
    }
    
    return 0;
}


int PredictorCorrector::correctD2(double* sig, double x)
{
    for (i=0; i<nDOF; i++)  {
        sig[i] = sig1[i]*(x-xi)*(1.0+x)/(2.0)/(1.0-xi)
            + sigXi[i]*(1.0-x)*(1.0+x)/(1.0-xi)/(1.0+xi)
            - sig3[i]*(1.0-x)*(x-xi)/(2.0)/(1.0+xi);
    }
    
    return 0;
}


int PredictorCorrector::correctV2(double* sigDot, double x)
{
    for (i=0; i<nDOF; i++)  {
        sigDot[i] = sig1[i]*(1.0+2.0*x-xi)/(2.0)/(1.0-xi)
            - sigXi[i]*(2.0*x)/(1.0-xi)/(1.0+xi)
            - sig3[i]*(1.0-2.0*x+xi)/(2.0)/(1.0+xi);
        sigDot[i] /= dtSim;
    }
    
    return 0;
}


int PredictorCorrector::correctA2(double* sigDotDot, double x)
{
    for (i=0; i<nDOF; i++)  {
        sigDotDot[i] = sig1[i]/(1.0-xi)
            - sigXi[i]*(2.0)/(1.0-xi)/(1.0+xi)
            + sig3[i]/(1.0+xi);
        sigDotDot[i] /= (dtSim*dtSim);
    }
    
    return 0;
}


int PredictorCorrector::predictD3(double* sig, double x)
{
    for (i=0; i<nDOF; i++)  {
        sig[i] = sigXi[i]*(1.0+x)*(2.0+x)*(3.0+x)/(xi)/(1.0+xi)/(2.0+xi)
            - sig2[i]*(1.0+x-xi)*(2.0+x)*(3.0+x)/(2.0*xi)
            + sig3[i]*(1.0+x-xi)*(1.0+x)*(3.0+x)/(1.0+xi)
            - sig4[i]*(1.0+x-xi)*(1.0+x)*(2.0+x)/(2.0)/(2.0+xi);
    }
    
    return 0;
}


int PredictorCorrector::predictV3(double* sigDot, double x)
{
    for (i=0; i<nDOF; i++)  {
        sigDot[i] = sigXi[i]*(11.0+12.0*x+3.0*x*x)/(xi)/(1.0+xi)/(2.0+xi)
            - sig2[i]*(11.0+12.0*x+3.0*x*x-5.0*xi-2.0*x*xi)/(2.0*xi)
            + sig3[i]*(7.0+10.0*x+3.0*x*x-4.0*xi-2.0*x*xi)/(1.0+xi)
            - sig4[i]*(5.0+8.0*x+3.0*x*x-3.0*xi-2.0*x*xi)/(2.0)/(2.0+xi);
        sigDot[i] /= dtSim;
    }
    
    return 0;
}


int PredictorCorrector::predictA3(double* sigDotDot, double x)
{
    for (i=0; i<nDOF; i++)  {
        sigDotDot[i] = sigXi[i]*(6.0)*(2.0+x)/(xi)/(1.0+xi)/(2.0+xi)
            - sig2[i]*(6.0+3.0*x-xi)/(xi)
            + sig3[i]*(2.0)*(5.0+3.0*x-xi)/(1.0+xi)
            - sig4[i]*(4.0+3.0*x-xi)/(2.0+xi);
        sigDotDot[i] /= (dtSim*dtSim);
    }
    
    return 0;
}


int PredictorCorrector::correctD3(double* sig, double x)
{
    for (i=0; i<nDOF; i++)  {
        sig[i] = sig1[i]*(x-xi)*(1.0+x)*(2.0+x)/(6.0)/(1-xi)
            + sigXi[i]*(1.0-x)*(1.0+x)*(2.0+x)/(1.0-xi)/(1.0+xi)/(2.0+xi)
            - sig3[i]*(1.0-x)*(x-xi)*(2.0+x)/(2.0)/(1.0+xi)
            + sig4[i]*(1.0-x)*(x-xi)*(1.0+x)/(3.0)/(2.0+xi);
    }
    
    return 0;
}


int PredictorCorrector::correctV3(double* sigDot, double x)
{
    for (i=0; i<nDOF; i++)  {
        sigDot[i] = sig1[i]*(2.0+6.0*x+3.0*x*x-3.0*xi-2.0*x*xi)/(6.0)/(1-xi)
            + sigXi[i]*(1.0-4.0*x-3.0*x*x)/(1.0-xi)/(1.0+xi)/(2.0+xi)
            - sig3[i]*(2.0-2.0*x-3.0*x*x+xi+2.0*x*xi)/(2.0)/(1.0+xi)
            + sig4[i]*(1.0-3.0*x*x+2.0*x*xi)/(3.0)/(2.0+xi);
        sigDot[i] /= dtSim;
    }
    
    return 0;
}


int PredictorCorrector::correctA3(double* sigDotDot, double x)
{
    for (i=0; i<nDOF; i++)  {
        sigDotDot[i] = sig1[i]*(3.0+3.0*x-xi)/(3.0)/(1-xi)
            - sigXi[i]*(2.0)*(2.0+3.0*x)/(1.0-xi)/(1.0+xi)/(2.0+xi)
            + sig3[i]*(1.0+3.0*x-xi)/(1.0+xi)
            - sig4[i]*(2.0)*(3.0*x-xi)/(3.0)/(2.0+xi);
        sigDotDot[i] /= (dtSim*dtSim);
    }
    
    return 0;
}


int PredictorCorrector::predictDV(double* sig, double x)
{
    for (i=0; i<nDOF; i++)  {
        sig[i] = sig1[i]*(1.0+x)*(1.0+x)*(1.0-2.0*x)
            + sigDot1[i]*(x)*(1.0+x)*(1.0+x) 
            + sig2[i]*(x)*(x)*(3.0+2.0*x)
            + sigDot2[i]*(x)*(x)*(1.0+x);
    }
    
    return 0;
}


int PredictorCorrector::correctDV(double* sig, double x)
{
    for (i=0; i<nDOF; i++)  {
        sig[i] = sig1[i]*(x-xi)*(2.0-x-xi)/(1.0-xi)/(1.0-xi)
            - sigDot1[i]*(1.0-x)*(x-xi)/(1.0-xi)
            + sigXi[i]*(1.0-x)*(1.0-x)/(1.0-xi)/(1.0-xi);
    }
    
    return 0;
}


int PredictorCorrector::predictDVA(double* sig, double x)
{
    for (i=0; i<nDOF; i++)  {
        sig[i] = sig1[i]
            + sigDot1[i]*(x)
            + sigDotDot1[i]*(x)*(x)/(2.0);
    }
    
    return 0;
}


int PredictorCorrector::correctDVA(double* sig, double x)
{
    for (i=0; i<nDOF; i++)  {
        sig[i] = sig1[i]*(x-xi)*(3.0-3.0*x+x*x-3.0*xi+x*xi+xi*xi)/(1.0-xi)/(1.0-xi)/(1.0-xi)
            - sigDot1[i]*(1.0-x)*(x-xi)*(2.0-x-xi)/(1.0-xi)/(1.0-xi)
            + sigDotDot1[i]*(1.0-x)*(1.0-x)*(x-xi)/(2.0)/(1.0-xi)
            + sigXi[i]*(1.0-x)*(1.0-x)*(1.0-x)/(1.0-xi)/(1.0-xi)/(1.0-xi);
    }
    
    return 0;
}
