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

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 08/08
// Revision: A
//
// Description: This file contains the implementation of ExpControlResponse.

#include <ExpControlResponse.h>
#include <ExperimentalControl.h>


ExpControlResponse::ExpControlResponse(ExperimentalControl *ec,
    int id)
    : Response(), theControl(ec), responseID(id)
{

}


ExpControlResponse::ExpControlResponse(ExperimentalControl *ec,
    int id, int val)
    : Response(val), theControl(ec), responseID(id)
{

}


ExpControlResponse::ExpControlResponse(ExperimentalControl *ec,
    int id, double val)
    : Response(val), theControl(ec), responseID(id)
{

}


ExpControlResponse::ExpControlResponse(ExperimentalControl *ec,
    int id, const ID &val)
    : Response(val), theControl(ec), responseID(id)
{

}


ExpControlResponse::ExpControlResponse(ExperimentalControl *ec,
    int id, const Vector &val)
    : Response(val), theControl(ec), responseID(id)
{

}


ExpControlResponse::ExpControlResponse(ExperimentalControl *ec,
    int id, const Matrix &val)
    : Response(val), theControl(ec), responseID(id)
{

}


ExpControlResponse::~ExpControlResponse()
{

}


int ExpControlResponse::getResponse(void)
{
    return theControl->getResponse(responseID, myInfo);
}
