/* ****************************************************************** **
**    OpenFRESCO - Open Framework                                     **
**                 for Experimental Setup and Control                 **
**                                                                    **
**                                                                    **
** Copyright (c) 2006, Yoshikazu Takahashi, Kyoto University          **
** All rights reserved.                                               **
**                                                                    **
** Licensed under the modified BSD License (the "License");           **
** you may not use this file except in compliance with the License.   **
** You may obtain a copy of the License in main directory.            **
** Unless required by applicable law or agreed to in writing,         **
** software distributed under the License is distributed on an        **
** "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,       **
** either express or implied. See the License for the specific        **
** language governing permissions and limitations under the License.  **
**                                                                    **
** Developed by:                                                      **
**   Yoshikazu Takahashi (yos@catfish.dpri.kyoto-u.ac.jp)             **
**   Andreas Schellenberg (andreas.schellenberg@gmx.net)              **
**   Gregory L. Fenves (fenves@berkeley.edu)                          **
**                                                                    **
** ****************************************************************** */

// Written: Yoshi (yos@catfish.dpri.kyoto-u.ac.jp)
// Created: 09/06
// Revision: A
//
// Description: This file contains the implementation of 
// ExperimentalSignalFilter.

#include "ExperimentalSignalFilter.h"

#include <TaggedObject.h>
#include <MapOfTaggedObjects.h>

static MapOfTaggedObjects theExperimentalSignalFilters;


bool OPF_addExperimentalSignalFilter(ExperimentalSignalFilter* newComponent)
{
    return theExperimentalSignalFilters.addComponent(newComponent);
}


bool OPF_removeExperimentalSignalFilter(int tag)
{
    TaggedObject* obj = theExperimentalSignalFilters.removeComponent(tag);
    if (obj != 0) {
        delete obj;
        return true;
    }
    return false;
}


ExperimentalSignalFilter* OPF_getExperimentalSignalFilter(int tag)
{
    TaggedObject* theResult = theExperimentalSignalFilters.getComponentPtr(tag);
    if (theResult == 0) {
        opserr << "OPF_GetExperimentalSignalFilter() - "
            << "none found with tag: " << tag << endln;
        return 0;
    }
    ExperimentalSignalFilter* theFilter = (ExperimentalSignalFilter*)theResult;
    
    return theFilter;
}


void OPF_clearExperimentalSignalFilters()
{
    theExperimentalSignalFilters.clearAll();
}


ExperimentalSignalFilter::ExperimentalSignalFilter(int tag)
    : TaggedObject(tag)
{
    // does nothing
}


ExperimentalSignalFilter::ExperimentalSignalFilter(
    const ExperimentalSignalFilter& esf)
    : TaggedObject(esf)
{
    // does nothing
}


ExperimentalSignalFilter::~ExperimentalSignalFilter()
{
    // does nothing
}


const char* ExperimentalSignalFilter::getClassType() const
{
    return "UnknownExpSignalFilterObject";
}


Response* ExperimentalSignalFilter::setResponse(const char **argv,
    int argc, OPS_Stream &output)
{
    Response *theResponse = 0;
    
    output.tag("ExpSignalFilterOutput");
    output.attr("signalFilterType",this->getClassType());
    output.attr("signalFilterTag",this->getTag());
    
    output.endTag();
    
    return theResponse;
}


int ExperimentalSignalFilter::getResponse(int responseID,
    Information &info)
{
    // each subclass must implement its own response
    return -1;
}
