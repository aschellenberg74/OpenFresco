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

#ifndef ETBroyden_h
#define ETBroyden_h

// Written: Hong Kim (hongkim@berkeley.edu)
// Created: 11/09
// Revision: A
//
// Description: This file contains the class definition for 
// ETBroyden.

#include <ExperimentalTangentStiff.h>

class Response;

class ETBroyden : public ExperimentalTangentStiff
{
public:
    // constructors
    ETBroyden(int tag);
    ETBroyden(const ETBroyden& ets);
    
    // destructor
    virtual ~ETBroyden();
    
    // method to get class type
    const char *getClassType() const {return "ETBroyden";};
    
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
    Matrix *theStiff;  // the tangent stiffness Matrix
};

#endif
