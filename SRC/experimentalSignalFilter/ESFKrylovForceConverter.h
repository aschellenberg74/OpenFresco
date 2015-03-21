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

// $Revision$
// $Date$
// $URL$

#ifndef ESFKrylovForceConverter_h
#define ESFKrylovForceConverter_h

// Written: Hong Kim (hongkim@berkeley.edu)
// Created: 10/10
// Revision: A
//
// Description: This file contains the class definition for 
// ESFKrylovForceConverter.  This class converts the trial displacements
// to trial forces and measured displacements to measured forces using 
// the Krylov subspaces.  It used the converting method and does nothing 
// with the filtering method. 

#include "ExperimentalSignalFilter.h"
#include "ExperimentalTangentStiff.h"
#include <Matrix.h>

class ESFKrylovForceConverter : public ExperimentalSignalFilter
{
public:
    // constructors
    ESFKrylovForceConverter(int tag, int ss, Matrix& initStif);
    ESFKrylovForceConverter(const ESFKrylovForceConverter& esf);
    
    // destructor
    virtual ~ESFKrylovForceConverter();
    
    // method to get class type
    const char *getClassType() const {return "ESFKrylovForceConverter";};
    
    virtual int setSize(const int sz);
    virtual double filtering(double data);
    virtual Vector& converting(Vector* trialDisp);
    virtual Vector& converting(Vector* daqDisp, Vector* daqForce);
    virtual void update();
    
    virtual ExperimentalSignalFilter *getCopy();
    
    // public methods for output
    void Print(OPS_Stream &s, int flag = 0);

private:
    bool firstWarning;	// flag for updating the stiffness matrix
    int size;			// size of ctrl and daq vectors
    int szSubspace;		// number of spaces vectors to use
    Vector dispPast, forcePast, convertFrc, incrDisp, incrForce, cn;
    Matrix kInit;	    // tangent stiffness matrix
    Matrix iDMatrix;	// the incremental displacement Matrix
    Matrix iFMatrix;	// the incremental force Matrix
    
    int setInitialStiff();
    int updateIncrMat(const Vector* daqDisp, const Vector* daqForce);
    int matTranspose(Matrix* kT, const Matrix* k);
};

#endif
