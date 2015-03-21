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

#ifndef ESFTangForceConverter_h
#define ESFTangForceConverter_h

// Written: Hong Kim (hongkim@berkeley.edu)
// Created: 10/10
// Revision: A
//
// Description: This file contains the class definition for 
// ESFTangForceConverter.  This class converts the trial displacements
// to trial forces and measured displacements to measured forces using 
// the Experimental Tangent Stiffness class.  It used the converting 
// method and does nothing with the filtering method. 

#include "ExperimentalSignalFilter.h"
#include "ExperimentalTangentStiff.h"
#include <Matrix.h>

class ESFTangForceConverter : public ExperimentalSignalFilter
{
public:
    // constructors
    ESFTangForceConverter(int tag, Matrix& initStif,
        ExperimentalTangentStiff* tangStif = 0);
    ESFTangForceConverter(const ESFTangForceConverter& esf);
    
    // destructor
    virtual ~ESFTangForceConverter();
    
    // method to get class type
    const char *getClassType() const {return "ESFTangForceConverter";};
    
    virtual int setSize(const int sz);
    virtual double filtering(double data);
    virtual Vector& converting(Vector* trialDisp);
    virtual Vector& converting(Vector* daqDisp, Vector* daqForce);
    virtual void update();
    
    virtual ExperimentalSignalFilter *getCopy();
    
    // public methods for output
    void Print(OPS_Stream &s, int flag = 0);

private:
    ExperimentalTangentStiff *theTangStiff;
    bool firstWarning;  // flag for updating the stiffness matrix
    int size;           // size of ctrl and daq vectors
    Vector dispPast, forcePast, convertFrc, incrDisp, incrForce;
    Matrix kInit, kPrev, thetangStiffMat;  // tangent stiffness matrix
    
    int setInitialStiff();
    int updateMatrix(const Vector* daqDisp, const Vector* daqForce);
};

#endif
