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

// $Revision$
// $Date$
// $URL: $

// Written: Yoshi (yos@catfish.dpri.kyoto-u.ac.jp)
// Created: 09/06
// Revision: A
//
// Description: This file contains the implementation of  
// LocalExpSite.

#include "LocalExpSite.h"


LocalExpSite::LocalExpSite(int tag, 
    ExperimentalSetup *setup)
    : ExperimentalSite(tag, setup)
{
    if (theSetup == 0) {
        opserr << "LocalExpSite::LocalExpSite() - "
            << "must have an instance of ExperimentalSetup"
            << endln;
        exit(OF_ReturnType_failed);
    }
}


LocalExpSite::LocalExpSite(const LocalExpSite& site)
    : ExperimentalSite(site)
{
    // does nothing
}


LocalExpSite::~LocalExpSite()
{
    // does nothing
}


int LocalExpSite::setSize(ID sizeT, ID sizeO)
{
    this->ExperimentalSite::setSize(sizeT, sizeO);
    
    theSetup->setSize(sizeT, sizeO);
    
    return OF_ReturnType_completed;
}


int LocalExpSite::setup()
{
    // does nothing
    //theSetup->setup();
    
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
    if (rValue != OF_ReturnType_completed) {
        opserr << "LocalExpSite::setTrialResponse() - "
            << "failed to set trial response at the setup.";
        exit(OF_ReturnType_failed);
    }
    
    // transform daq response into output response
    theSetup->transfDaqResponse(Disp, Vel, Accel, Force, Time);
    
    this->ExperimentalSite::setDaqResponse(Disp, Vel, Accel, Force, Time);
    
    // set daq flag
    daqFlag = true;
    
    return OF_ReturnType_completed;
}


int LocalExpSite::checkDaqResponse()
{
    if (daqFlag == false) {
        // get daq response into output response
        theSetup->getDaqResponse(Disp, Vel, Accel, Force, Time);
        
        this->ExperimentalSite::setDaqResponse(Disp, Vel, Accel, Force, Time);
        
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
    s << "\tExperimentalSetup tag: " << theSetup->getTag() 
        << endln;
    s << *theSetup;
}
