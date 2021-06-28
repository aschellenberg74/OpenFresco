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
// Description: This file contains the implementation of 
// ExperimentalTangentStiff.

#include "ExperimentalTangentStiff.h"

#include <TaggedObject.h>
#include <MapOfTaggedObjects.h>

static MapOfTaggedObjects theExperimentalTangentStiffs;


bool OPF_addExperimentalTangentStiff(ExperimentalTangentStiff* newComponent)
{
    return theExperimentalTangentStiffs.addComponent(newComponent);
}


bool OPF_removeExperimentalTangentStiff(int tag)
{
    TaggedObject* obj = theExperimentalTangentStiffs.removeComponent(tag);
    if (obj != 0) {
        delete obj;
        return true;
    }
    return false;
}


ExperimentalTangentStiff* OPF_getExperimentalTangentStiff(int tag)
{
    TaggedObject* theResult = theExperimentalTangentStiffs.getComponentPtr(tag);
    if (theResult == 0) {
        opserr << "OPF_GetExperimentalTangentStiff() - "
            << "none found with tag: " << tag << endln;
        return 0;
    }
    ExperimentalTangentStiff* theTangStiff = (ExperimentalTangentStiff*)theResult;
    
    return theTangStiff;
}


void OPF_clearExperimentalTangentStiffs()
{
    theExperimentalTangentStiffs.clearAll();
}


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
