// File: PredictorCorrector.h
// 
// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 11/04
// Revision: A
//
// Description: This file contains the function definitions for the
//              Predictor-Corrector loop in the three-loop architecture
//              used in Hybrid Simulation

#ifndef PredictorCorrector_h
#define PredictorCorrector_h


// methods to initialize and zero data arrays
int initData(int nDOF, double dtCon, double dtSim, double dtInt);
int zeroData();
int zeroDsp(double *dsp);

// methods to update the data arrays
int setCurDsp(double *dsp, double x);
int setNewDsp(double *dsp);
int setNewVel(double *vel);
int setNewAcc(double *acc);

// methods to get data and parameters
double getNumSubSteps(double vDOF, int dofID, double vDOFmax, int Nmin);

// method for hold in ramp-and-hold procedure
int predictP0(double *dsp, double x);

// methods using linear Lagrange polynomials
int predictP1(double *dsp, double x);
int correctP1(double *dsp, double x);

// methods using quadratic Lagrange polynomials
int predictP2(double *dsp, double x);
int correctP2(double *dsp, double x);

// methods using cubic Lagrange polynomials
int predictP3(double *dsp, double x);
int correctP3(double *dsp, double x);

// method for hold in ramp-and-hold procedure (same as predictP0)
int predictD0(double *dsp, double x);

// methods using linear Lagrange polynomials and current disp
// when switching between prediction and correction
int predictD1(double *dsp, double x);
int predictV1(double *vel, double x);
int predictA1(double *acc, double x);
int correctD1(double *dsp, double x);
int correctV1(double *vel, double x);
int correctA1(double *acc, double x);

// methods using quadratic Lagrange polynomials and current disp
// when switching between prediction and correction
int predictD2(double *dsp, double x);
int predictV2(double *vel, double x);
int predictA2(double *acc, double x);
int correctD2(double *dsp, double x);
int correctV2(double *vel, double x);
int correctA2(double *acc, double x);

// methods using cubic Lagrange polynomials and current disp
// when switching between prediction and correction
int predictD3(double *dsp, double x);
int predictV3(double *vel, double x);
int predictA3(double *acc, double x);
int correctD3(double *dsp, double x);
int correctV3(double *vel, double x);
int correctA3(double *acc, double x);

// methods using cubic Hermite polynomials and current disp
// when switching between prediction and correction
int predictDV(double *dsp, double x);
int correctDV(double *dsp, double x);

// methods using disp, vel and accel and current disp
// when switching between prediction and correction
int predictDVA(double *dsp, double x);
int correctDVA(double *dsp, double x);

// global variables
extern int i, nDOF;
extern double dtCon, dtSim, dtInt, xi;

extern double *dsp1, *dsp2, *dsp3, *dsp4, *dsp5, *dsp6, *dspXi;
extern double *vel1, *vel2;
extern double *acc1;

#endif
