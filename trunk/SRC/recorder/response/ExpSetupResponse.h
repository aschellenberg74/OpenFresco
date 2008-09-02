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

// $Revision: $
// $Date: $
// $URL: $

#ifndef ExpSetupResponse_h
#define ExpSetupResponse_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 08/08
// Revision: A
//
// Description: This file contains the class definition for
// ExpSetupResponse.

#include <Response.h>

class ExperimentalSetup;
class ID;
class Vector;
class Matrix;
class Tensor;

class ExpSetupResponse : public Response
{
public:
    ExpSetupResponse(ExperimentalSetup *es, int id);
    ExpSetupResponse(ExperimentalSetup *es, int id, int val);
    ExpSetupResponse(ExperimentalSetup *es, int id, double val);
    ExpSetupResponse(ExperimentalSetup *es, int id, const ID &val);
    ExpSetupResponse(ExperimentalSetup *es, int id, const Vector &val);
    ExpSetupResponse(ExperimentalSetup *es, int id, const Matrix &val);
    ExpSetupResponse(ExperimentalSetup *es, int id, const Tensor &val);
    ~ExpSetupResponse();
    
    int getResponse();
    
private:
    ExperimentalSetup *theSetup;
    int responseID;
};

#endif
