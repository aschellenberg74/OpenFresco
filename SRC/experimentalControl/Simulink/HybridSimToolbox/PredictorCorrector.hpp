// File: PredictorCorrector.hpp
// 
// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 11/04
// Revision: A
//
// Description: This file contains the function definitions for the
//              Predictor-Corrector loop in the three-loop architecture
//              used in Hybrid Simulation
//
// Modified: Akiri Seki (sekia@stanford.edu)
// Date: 01/2023
// Description: This file is modified to be in cpp format.

#ifndef PredictorCorrector_hpp
#define PredictorCorrector_hpp


class PredictorCorrector
{
    public:
        // constructor and destructor
        PredictorCorrector();
        ~PredictorCorrector();
        
        // methods to initialize the data arrays
        int initData(int nDOF, double dtCon, double dtSim, double dtInt);
        int zeroData();
        int setData(double* sig);
        int setDataDot(double* sigDot);
        int setDataDotDot(double* sigDotDot);
        
        // methods to update the data arrays
        int setCurSig(double* sig, double x);
        int setNewSig(double* sig);
        int setNewSigDot(double* sigDot);
        int setNewSigDotDot(double* sigDotDot);
        
        // methods to initialize a signal
        int zeroSig(double* sig);
        int setSig(double* sig, double* x);
        int setSigDot(double* sigDot, double* x);
        int setSigDotDot(double* sigDotDot, double* x);
        
        // methods to get data and parameters
        double getNumSubSteps(double sigDotDOF, int dofID, double sigDotDOFmax, int Nmin);
        
        // method for hold in ramp-and-hold procedure
        int predictP0(double* sig, double x);
        
        // methods using linear Lagrange polynomials
        int predictP1(double* sig, double x);
        int correctP1(double* sig, double x);
        
        // methods using quadratic Lagrange polynomials
        int predictP2(double* sig, double x);
        int correctP2(double* sig, double x);
        
        // methods using cubic Lagrange polynomials
        int predictP3(double* sig, double x);
        int correctP3(double* sig, double x);
        
        // method for hold in ramp-and-hold procedure (same as predictP0)
        int predictD0(double* sig, double x);
        
        // methods using linear Lagrange polynomials and current signal
        // when switching between prediction and correction
        int predictD1(double* sig, double x);
        int predictV1(double* sigDot, double x);
        int predictA1(double* sigDotDot, double x);
        int correctD1(double* sig, double x);
        int correctV1(double* sigDot, double x);
        int correctA1(double* sigDotDot, double x);
        
        // methods using quadratic Lagrange polynomials and current signal
        // when switching between prediction and correction
        int predictD2(double* sig, double x);
        int predictV2(double* sigDot, double x);
        int predictA2(double* sigDotDot, double x);
        int correctD2(double* sig, double x);
        int correctV2(double* sigDot, double x);
        int correctA2(double* sigDotDot, double x);
        
        // methods using cubic Lagrange polynomials and current signal
        // when switching between prediction and correction
        int predictD3(double* sig, double x);
        int predictV3(double* sigDot, double x);
        int predictA3(double* sigDotDot, double x);
        int correctD3(double* sig, double x);
        int correctV3(double* sigDot, double x);
        int correctA3(double* sigDotDot, double x);
        
        // methods using cubic Hermite polynomials and current signal
        // when switching between prediction and correction
        int predictDV(double* sig, double x);
        int correctDV(double* sig, double x);
        
        // methods using sig, sigDot, sigDotDot and current signal
        // when switching between prediction and correction
        int predictDVA(double* sig, double x);
        int correctDVA(double* sig, double x);
        
        // parameters
        int i, nDOF;
        double dtCon, dtSim, dtInt, xi;
        
        // class data arrays that store the history
        double *sig1, *sig2, *sig3, *sig4, *sig5, *sig6, *sigXi;
        double *sigDot1, *sigDot2;
        double *sigDotDot1;
};

#endif
