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

#ifndef ExperimentalTangentStiff_h
#define ExperimentalTangentStiff_h

// Written: Hong Kim (hongkim@berkeley.edu)
// Created: 9/09
// Revision: A
//
// Description: This file contains the class definition for 
// ExperimentalTangentStiff.

#include <FrescoGlobals.h>
#include <ExpTangentStiffResponse.h>

#include <TaggedObject.h>
#include <ID.h>
#include <Vector.h>
#include <Matrix.h>

class Response;

class ExperimentalTangentStiff : public TaggedObject
{
public:
    // constructors
    ExperimentalTangentStiff(int tag);
    ExperimentalTangentStiff(const ExperimentalTangentStiff& ets);
    
    // method to get class type
    virtual const char *getClassType() const;
    
    // destructor
    virtual ~ExperimentalTangentStiff();
    
    virtual Matrix& updateTangentStiff(
        const Vector* disp,
        const Vector* vel,
        const Vector* accel,
        const Vector* force,
        const Vector* time,
        const Matrix* kInit,
        const Matrix* kPrev) = 0;
    
    virtual ExperimentalTangentStiff *getCopy() = 0;
    
    // public methods for experimental tangent stiffness recorder
    virtual Response *setResponse(const char **argv, int argc,
        OPS_Stream &output);
    virtual int getResponse(int responseID, Information &info);
};

extern bool OPF_AddExperimentalTangentStiff(ExperimentalTangentStiff* newComponent);
extern ExperimentalTangentStiff* OPF_GetExperimentalTangentStiff(int tag);
extern bool OPF_RemoveExperimentalTangentStiff(int tag);
extern void OPF_ClearExperimentalTangentStiffs();

#endif
