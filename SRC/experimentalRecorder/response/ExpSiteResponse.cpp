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

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 08/08
// Revision: A
//
// Description: This file contains the implementation of ExpSiteResponse.

#include <ExpSiteResponse.h>
#include <ExperimentalSite.h>


ExpSiteResponse::ExpSiteResponse(ExperimentalSite *es,
    int id)
    : Response(), theSite(es), responseID(id)
{

}


ExpSiteResponse::ExpSiteResponse(ExperimentalSite *es,
    int id, int val)
    : Response(val), theSite(es), responseID(id)
{

}


ExpSiteResponse::ExpSiteResponse(ExperimentalSite *es,
    int id, double val)
    : Response(val), theSite(es), responseID(id)
{

}


ExpSiteResponse::ExpSiteResponse(ExperimentalSite *es,
    int id, const ID &val)
    : Response(val), theSite(es), responseID(id)
{

}


ExpSiteResponse::ExpSiteResponse(ExperimentalSite *es,
    int id, const Vector &val)
    : Response(val), theSite(es), responseID(id)
{

}


ExpSiteResponse::ExpSiteResponse(ExperimentalSite *es,
    int id, const Matrix &val)
    : Response(val), theSite(es), responseID(id)
{

}


ExpSiteResponse::~ExpSiteResponse()
{

}


int ExpSiteResponse::getResponse(void)
{
    return theSite->getResponse(responseID, myInfo);
}
