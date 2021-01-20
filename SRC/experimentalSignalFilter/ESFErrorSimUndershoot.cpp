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

// Written: Yoshi
// Created: 09/06
// Revision: A
//
// Purpose: This file contains the implementation of 
// ESFErrorSimUndershoot.

#include "ESFErrorSimUndershoot.h"

#include <elementAPI.h>

#include <math.h>

void* OPF_ESFErrorSimUndershoot()
{
    // pointer to experimental control that will be returned
    ExperimentalSignalFilter* theFilter = 0;
    
    if (OPS_GetNumRemainingInputArgs() < 2) {
        opserr << "WARNING invalid number of arguments\n";
        opserr << "Want: expSignalFilter ErrorSimUndershoot tag error\n";
        return 0;
    }
    
    // filter tag
    int tag;
    int numdata = 1;
    if (OPS_GetIntInput(&numdata, &tag) != 0) {
        opserr << "WARNING invalid expSignalFilter ErrorSimUndershoot tag\n";
        return 0;
    }
    
    // undershoot error
    double error;
    numdata = 1;
    if (OPS_GetDoubleInput(&numdata, &error) != 0) {
        opserr << "WARNING invalid undershoot error\n";
        opserr << "expSignalFilter ErrorSimUndershoot " << tag << endln;
        return 0;
    }
    
    // parsing was successful, allocate the signal filter
    theFilter = new ESFErrorSimUndershoot(tag, error);
    if (theFilter == 0) {
        opserr << "WARNING could not create experimental signal filter "
            << "of type ESFErrorSimUndershoot\n";
        return 0;
    }
    
    return theFilter;
}


ESFErrorSimUndershoot::ESFErrorSimUndershoot(int tag, double error)
    : ESFErrorSimulation(tag),
    undershoot(error), data(0.0), predata(0.0)
{
    // does nothing
}


ESFErrorSimUndershoot::ESFErrorSimUndershoot(const ESFErrorSimUndershoot& esf)
    : ESFErrorSimulation(esf)
{
    undershoot = esf.undershoot;
    data = esf.data;
    predata = esf.predata;
}


ESFErrorSimUndershoot::~ESFErrorSimUndershoot()
{
    // does nothing
}


double ESFErrorSimUndershoot::filtering(double d)
{
    data = d;
    if (d > predata)
        data -= undershoot;
    else if (d < predata)
        data += undershoot;
    if (fabs(d) < 1.0e-6)
        data += undershoot;
    predata = d;
    
    return data;
}


void ESFErrorSimUndershoot::update()
{
    // does nothing
}


ExperimentalSignalFilter* ESFErrorSimUndershoot::getCopy()
{
    return new ESFErrorSimUndershoot(*this);
}


void ESFErrorSimUndershoot::Print(OPS_Stream &s, int flag)
{
    s << "Filter: " << this->getTag(); 
    s << "  type: ESFErrorSimUndershoot\n";
    s << "  undershoot error: " << undershoot << endln;
}
