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

#ifndef ETTranspose_h
#define ETTranspose_h

// Written: Hong Kim (hongkim@berkeley.edu)
// Created: 10/10
// Revision: A
//
// Description: This file contains the class definition for 
// ETTranspose. This class uses the measured displacement vector and 
// resisting force vector to compute the new stiffness matrix. It uses 
// the BFGS method used by Thewalt and later by Igarashi 
// (Igarashi 1993 p. 10)

#include <ExperimentalTangentStiff.h>

class Response;

class ETTranspose : public ExperimentalTangentStiff
{
public:
    // constructors
    ETTranspose(int tag, int nC);
    ETTranspose(const ETTranspose& ets);
    
    // destructor
    virtual ~ETTranspose();
    
    // method to get class type
    const char *getClassType() const {return "ETTranspose";};
    
    virtual Matrix& updateTangentStiff(
        const Vector* incrDisp,
        const Vector* incrVel,
        const Vector* incrAccel,
        const Vector* incrForce,
        const Vector* time,
        const Matrix* kInit,
        const Matrix* kPrev);
    
    virtual ExperimentalTangentStiff *getCopy();
    
    // public methods for output
    void Print(OPS_Stream &s, int flag = 0);
    
    // public methods for tangent stiffness recorder
    Response *setResponse(const char **argv, int argc,
        OPS_Stream &output);
    int getResponse(int responseID, Information &info);

private:
    int numCol;         // number of cols to be used
    Matrix *theStiff;   // the tangent stiffness Matrix
    Matrix iDMatrix;    // the incremental displacement Matrix
    Matrix iFMatrix;    // the incremental force Matrix
    int MatTranspose(Matrix* kT, const Matrix* k);  // transpose method
};

#endif
