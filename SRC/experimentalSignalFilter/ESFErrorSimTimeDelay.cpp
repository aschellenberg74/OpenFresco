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

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 07/18
// Revision: A
//
// Purpose: This file contains the implementation of 
// ESFErrorSimTimeDelay.

#include "ESFErrorSimTimeDelay.h"

#include <elementAPI.h>

#include <math.h>

void* OPF_ESFErrorSimTimeDelay()
{
    // pointer to experimental control that will be returned
    ExperimentalSignalFilter* theFilter = 0;

    if (OPS_GetNumRemainingInputArgs() < 2) {
        opserr << "WARNING invalid number of arguments\n";
        opserr << "Want: expSignalFilter ErrorSimTimeDelay tag error\n";
        return 0;
    }

    // filter tag
    int tag;
    int numdata = 1;
    if (OPS_GetIntInput(&numdata, &tag) != 0) {
        opserr << "WARNING invalid expSignalFilter ErrorSimTimeDelay tag\n";
        return 0;
    }

    // time delay error
    double delay;
    numdata = 1;
    if (OPS_GetDoubleInput(&numdata, &delay) != 0) {
        opserr << "WARNING invalid undershoot error\n";
        opserr << "expSignalFilter ErrorSimTimeDelay " << tag << endln;
        return 0;
    }

    // parsing was successful, allocate the signal filter
    theFilter = new ESFErrorSimTimeDelay(tag, delay);
    if (theFilter == 0) {
        opserr << "WARNING could not create experimental signal filter "
            << "of type ESFErrorSimTimeDelay\n";
        return 0;
    }

    return theFilter;
}


ESFErrorSimTimeDelay::ESFErrorSimTimeDelay(int tag, double tau)
    : ESFErrorSimulation(tag),
    delay(tau), data(0.0), predata(0.0)
{
    // does nothing
}


ESFErrorSimTimeDelay::ESFErrorSimTimeDelay(const ESFErrorSimTimeDelay& esf)
    : ESFErrorSimulation(esf)
{
    delay = esf.delay;
    data = esf.data;
    predata = esf.predata;
}


ESFErrorSimTimeDelay::~ESFErrorSimTimeDelay()
{
    // does nothing
}


double ESFErrorSimTimeDelay::filtering(double d)
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


void ESFErrorSimTimeDelay::update()
{
    // does nothing
}


ExperimentalSignalFilter* ESFErrorSimTimeDelay::getCopy()
{
    return new ESFErrorSimTimeDelay(*this);
}


void ESFErrorSimTimeDelay::Print(OPS_Stream &s, int flag)
{
    s << "Filter: " << this->getTag(); 
    s << "  type: ESFErrorSimTimeDelay\n";
    s << "  time delay error: " << delay << endln;
}
