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
// Created: 09/06
// Revision: A
//
// Description: This file contains the implementation of  
// LocalExpSite.

#include "LocalExpSite.h"

#include <elementAPI.h>


void* OPF_LocalExpSite()
{
    // pointer to experimental site that will be returned
    ExperimentalSite* theSite = 0;
    
    if (OPS_GetNumRemainingInputArgs() < 2) {
        opserr << "WARNING invalid number of arguments\n";
        opserr << "Want: expSite LocalSite tag setupTag\n";
        return 0;
    }
    
    // site tag
    int tag;
    int numdata = 1;
    if (OPS_GetIntInput(&numdata, &tag) != 0) {
        opserr << "WARNING invalid expSite LocalSite tag\n";
        return 0;
    }
    
    // experimental setup
    int setupTag;
    numdata = 1;
    if (OPS_GetIntInput(&numdata, &setupTag) != 0) {
        opserr << "WARNING invalid setupTag\n";
        opserr << "expSite LocalSite " << tag << endln;
        return 0;
    }
    ExperimentalSetup* theSetup = OPF_getExperimentalSetup(setupTag);
    if (theSetup == 0) {
        opserr << "WARNING experimental setup not found\n";
        opserr << "expSetup: " << setupTag << endln;
        opserr << "expSite LocalSite " << tag << endln;
        return 0;
    }
    
    // parsing was successful, allocate the site
    theSite = new LocalExpSite(tag, theSetup);
    if (theSite == 0) {
        opserr << "WARNING could not create experimental site of type LocalSite\n";
        return 0;
    }
    
    return theSite;
}


LocalExpSite::LocalExpSite(int tag,
    ExperimentalSetup *setup)
    : ExperimentalSite(tag, setup)
{
    if (theSetup == 0)  {
        opserr << "LocalExpSite::LocalExpSite() - "
            << "must have an instance of ExperimentalSetup.\n";
        exit(OF_ReturnType_failed);
    }
}


LocalExpSite::LocalExpSite(const LocalExpSite& es)
    : ExperimentalSite(es)
{
    // does nothing
}


LocalExpSite::~LocalExpSite()
{
    // does nothing
}


int LocalExpSite::setup()
{
    // does nothing
    return OF_ReturnType_completed;
}


int LocalExpSite::setSize(ID sizeT, ID sizeO)
{
    // call the base class method
    this->ExperimentalSite::setSize(sizeT, sizeO);
    
    theSetup->checkSize(sizeT, sizeO);
    
    return OF_ReturnType_completed;
}


int LocalExpSite::setTrialResponse(const Vector* disp, 
    const Vector* vel,
    const Vector* accel,
    const Vector* force,
    const Vector* time)
{
    // save data in basic sys
    this->ExperimentalSite::setTrialResponse(disp, vel, accel, force, time);
    
    // set daq flag
    daqFlag = false;
    
    int rValue;
    // set trial response at the setup
    rValue = theSetup->setTrialResponse(tDisp, tVel, tAccel, tForce, tTime);
    if (rValue != OF_ReturnType_completed)  {
        opserr << "LocalExpSite::setTrialResponse() - "
            << "failed to set trial response at the setup.\n";
        exit(OF_ReturnType_failed);
    }
    
    return OF_ReturnType_completed;
}


int LocalExpSite::checkDaqResponse()
{
    if (daqFlag == false)  {
        int rValue;
        // get daq response from the setup
        rValue = theSetup->getDaqResponse(oDisp, oVel, oAccel, oForce, oTime);
        if (rValue != OF_ReturnType_completed)  {
            opserr << "LocalExpSite::checkDaqResponse() - "
                << "failed to get daq response from the setup.\n";
            exit(OF_ReturnType_failed);
        }
        
        // save data in basic sys
        this->ExperimentalSite::setDaqResponse(oDisp, oVel, oAccel, oForce, oTime);
        
        // set daq flag
        daqFlag = true;
    }
    
    return OF_ReturnType_completed;
}


ExperimentalSite* LocalExpSite::getCopy()
{
    LocalExpSite *theCopy = new LocalExpSite(*this);
    
    return theCopy;
}


void LocalExpSite::Print(OPS_Stream &s, int flag)
{
    s << "ExperimentalSite: " << this->getTag(); 
    s << " type: LocalExpSite\n";
    s << "\tExperimentalSetup tag: " << theSetup->getTag() << endln;
    s << *theSetup;
}
