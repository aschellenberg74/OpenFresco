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

// Written: Hong Kim (hongkim@berkeley.edu)
// Created: 04/10
// Revision: A
//
// Description: This file contains the implementation of ExpTangentStiffResponse.

#include <ExpTangentStiffResponse.h>
#include <ExperimentalTangentStiff.h>


ExpTangentStiffResponse::ExpTangentStiffResponse(ExperimentalTangentStiff *ets,
    int id)
    : Response(), theTangStiff(ets), responseID(id)
{

}


ExpTangentStiffResponse::ExpTangentStiffResponse(ExperimentalTangentStiff *ets,
    int id, int val)
    : Response(val), theTangStiff(ets), responseID(id)
{

}


ExpTangentStiffResponse::ExpTangentStiffResponse(ExperimentalTangentStiff *ets,
    int id, double val)
    : Response(val), theTangStiff(ets), responseID(id)
{

}


ExpTangentStiffResponse::ExpTangentStiffResponse(ExperimentalTangentStiff *ets,
    int id, const ID &val)
    : Response(val), theTangStiff(ets), responseID(id)
{

}


ExpTangentStiffResponse::ExpTangentStiffResponse(ExperimentalTangentStiff *ets,
    int id, const Vector &val)
    : Response(val), theTangStiff(ets), responseID(id)
{

}


ExpTangentStiffResponse::ExpTangentStiffResponse(ExperimentalTangentStiff *ets,
    int id, const Matrix &val)
    : Response(val), theTangStiff(ets), responseID(id)
{

}


ExpTangentStiffResponse::~ExpTangentStiffResponse()
{

}


int ExpTangentStiffResponse::getResponse(void)
{
    return theTangStiff->getResponse(responseID, myInfo);
}
