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
// Description: This file contains the implementation of ExpSignalFilterResponse.

#include <ExpSignalFilterResponse.h>
#include <ExperimentalSignalFilter.h>


ExpSignalFilterResponse::ExpSignalFilterResponse(ExperimentalSignalFilter *esf,
    int id)
    : Response(), theFilter(esf), responseID(id)
{

}


ExpSignalFilterResponse::ExpSignalFilterResponse(ExperimentalSignalFilter *esf,
    int id, int val)
    : Response(val), theFilter(esf), responseID(id)
{

}


ExpSignalFilterResponse::ExpSignalFilterResponse(ExperimentalSignalFilter *esf,
    int id, double val)
    : Response(val), theFilter(esf), responseID(id)
{

}


ExpSignalFilterResponse::ExpSignalFilterResponse(ExperimentalSignalFilter *esf,
    int id, const ID &val)
    : Response(val), theFilter(esf), responseID(id)
{

}


ExpSignalFilterResponse::ExpSignalFilterResponse(ExperimentalSignalFilter *esf,
    int id, const Vector &val)
    : Response(val), theFilter(esf), responseID(id)
{

}


ExpSignalFilterResponse::ExpSignalFilterResponse(ExperimentalSignalFilter *esf,
    int id, const Matrix &val)
    : Response(val), theFilter(esf), responseID(id)
{

}


ExpSignalFilterResponse::~ExpSignalFilterResponse()
{

}


int ExpSignalFilterResponse::getResponse(void)
{
    return theFilter->getResponse(responseID, myInfo);
}
