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

#ifndef ExpSiteResponse_h
#define ExpSiteResponse_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 08/08
// Revision: A
//
// Description: This file contains the class definition for
// ExpSiteResponse.

#include <Response.h>

class ExperimentalSite;
class ID;
class Vector;
class Matrix;
class Tensor;

class ExpSiteResponse : public Response
{
public:
    ExpSiteResponse(ExperimentalSite *es, int id);
    ExpSiteResponse(ExperimentalSite *es, int id, int val);
    ExpSiteResponse(ExperimentalSite *es, int id, double val);
    ExpSiteResponse(ExperimentalSite *es, int id, const ID &val);
    ExpSiteResponse(ExperimentalSite *es, int id, const Vector &val);
    ExpSiteResponse(ExperimentalSite *es, int id, const Matrix &val);
    ~ExpSiteResponse();
    
    int getResponse();
    
private:
    ExperimentalSite *theSite;
    int responseID;
};

#endif
