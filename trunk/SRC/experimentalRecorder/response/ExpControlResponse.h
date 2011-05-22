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

#ifndef ExpControlResponse_h
#define ExpControlResponse_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 08/08
// Revision: A
//
// Description: This file contains the class definition for
// ExpControlResponse.

#include <Response.h>

class ExperimentalControl;
class ID;
class Vector;
class Matrix;
class Tensor;

class ExpControlResponse : public Response
{
public:
    ExpControlResponse(ExperimentalControl *ec, int id);
    ExpControlResponse(ExperimentalControl *ec, int id, int val);
    ExpControlResponse(ExperimentalControl *ec, int id, double val);
    ExpControlResponse(ExperimentalControl *ec, int id, const ID &val);
    ExpControlResponse(ExperimentalControl *ec, int id, const Vector &val);
    ExpControlResponse(ExperimentalControl *ec, int id, const Matrix &val);
    ExpControlResponse(ExperimentalControl *ec, int id, const Tensor &val);
    ~ExpControlResponse();
    
    int getResponse();
    
private:
    ExperimentalControl *theControl;
    int responseID;
};

#endif
