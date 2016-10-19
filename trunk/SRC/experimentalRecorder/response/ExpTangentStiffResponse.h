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

#ifndef ExpTangentStiffResponse_h
#define ExpTangentStiffResponse_h

// Written: Hong Kim (hongkim@berkeley.edu)
// Created: 04/10
// Revision: A
//
// Description: This file contains the class definition for
// ExpTangentStiffResponse.

#include <Response.h>

class ExperimentalTangentStiff;
class ID;
class Vector;
class Matrix;

class ExpTangentStiffResponse : public Response
{
public:
    ExpTangentStiffResponse(ExperimentalTangentStiff *ets, int id);
    ExpTangentStiffResponse(ExperimentalTangentStiff *ets, int id, int val);
    ExpTangentStiffResponse(ExperimentalTangentStiff *ets, int id, double val);
    ExpTangentStiffResponse(ExperimentalTangentStiff *ets, int id, const ID &val);
    ExpTangentStiffResponse(ExperimentalTangentStiff *ets, int id, const Vector &val);
    ExpTangentStiffResponse(ExperimentalTangentStiff *ets, int id, const Matrix &val);
    ~ExpTangentStiffResponse();
    
    int getResponse();
    
private:
    ExperimentalTangentStiff *theTangStiff;
    int responseID;
};

#endif
