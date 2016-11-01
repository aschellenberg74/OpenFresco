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

// Written: Hong Kim (hongkim@berkeley.edu)
// Created: 04/10
// Revision: A
//
// Description: This file contains the implementation of 
// ExperimentalTangentStiff.

#include "ExperimentalTangentStiff.h"


ExperimentalTangentStiff::ExperimentalTangentStiff(int tag)
    : TaggedObject(tag)
{
    // does nothing
}


ExperimentalTangentStiff::ExperimentalTangentStiff(
    const ExperimentalTangentStiff& ets)
    : TaggedObject(ets)
{
    // does nothing
}


ExperimentalTangentStiff::~ExperimentalTangentStiff()
{
    // does nothing
}


const char* ExperimentalTangentStiff::getClassType() const
{
    return "UnknownExpTangentStiffObject";
}


Response* ExperimentalTangentStiff::setResponse(const char **argv,
    int argc, OPS_Stream &output)
{
    Response *theResponse = 0;
    
    output.tag("ExpTangStiffOutput");
    output.attr("tangentStiffType",this->getClassType());
    output.attr("tangentStiffTag",this->getTag());
    
    output.endTag();
    
    return theResponse;
}


int ExperimentalTangentStiff::getResponse(int responseID,
    Information &info)
{
    // each subclass must implement its own response
    return OF_ReturnType_failed;
}
