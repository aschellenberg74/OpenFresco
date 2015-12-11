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

// Written: Hong Kim (hongkim@berkeley.edu)
// Created: 11/09
// Revision: A
//
// Description: This file contains the implementation of 
// ETBroyden.  

#include "ETBroyden.h"


ETBroyden::ETBroyden(int tag)
    : ExperimentalTangentStiff(tag), theStiff(0)
{
    // does nothing
}


ETBroyden::ETBroyden(const ETBroyden& ets)
    : ExperimentalTangentStiff(ets), theStiff(0)
{
    // does nothing
}


ETBroyden::~ETBroyden()
{
    // invoke the destructor on any objects created by the object
    // that the object still holds a pointer to
    if (theStiff != 0)
        delete theStiff;
}


Matrix& ETBroyden::updateTangentStiff(
    const Vector* disp,
    const Vector* vel,
    const Vector* accel,
    const Vector* force,
    const Vector* time,
    const Matrix* kInit,
    const Matrix* kPrev)
{
    // Using incremental disp and force
    int dimR = kPrev->noRows();
    int dimC = kPrev->noCols();
    int szD	 = disp->Size();
    
    // FIX ME add size check
    theStiff = new Matrix(dimR, dimC);
    theStiff->Zero();
    
    double normD = disp->Norm();
    if (normD == 0.0) {
        *theStiff = *kPrev;
    } else {
        double factor = 1.0/(normD*normD);
        Vector tempV(szD);
        tempV.Zero();
        
        // Perform rank 1 update
        tempV.addMatrixVector(0.0, (*kPrev), (*disp), 1.0);
        tempV.addVector(-1.0, (*force), 1.0);
        
        for (int i = 0; i < dimR; i++) {
            for (int j = 0; j < dimC; j++) {
                (*theStiff)(i,j) = (*kPrev)(i,j) + factor * tempV(i) * (*disp)(j);
            }
        }
    }
    
    return *theStiff;
}


ExperimentalTangentStiff* ETBroyden::getCopy()
{
    return new ETBroyden(*this);
}


void ETBroyden::Print(OPS_Stream &s, int flag)
{
    s << "Experimental Tangent: " << this->getTag(); 
    s << "  type: ETBroyden\n";
}


Response* ETBroyden::setResponse(const char **argv,
    int argc, OPS_Stream &output)
{
    Response *theResponse = 0;
    
    // FIX ME
    
    return theResponse;
}


int ETBroyden::getResponse(int responseID,
    Information &info)
{
    // each subclass must implement its own response
    return -1;
}
