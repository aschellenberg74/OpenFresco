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
// $Source$

// Written: Yoshi (yos@catfish.dpri.kyoto-u.ac.jp)
// Created: 09/06
// Revision: A
//
// Description: This file contains the implementation of ESOneActuator.

#include "ESOneActuator.h"


ESOneActuator::ESOneActuator(int tag, 
    int dir,
    ExperimentalControl* control)
    : ExperimentalSetup(tag, control),
    direction(dir)
{
    this->setup();
}


ESOneActuator::ESOneActuator(const ESOneActuator& es)
    : ExperimentalSetup(es)
{
    direction = es.direction;
}


ESOneActuator::~ESOneActuator()
{
    // does nothing
}


int ESOneActuator::setSize(ID sizeT, ID sizeO)
{
    // check sizeTrial and sizeOut
    // for ESOneActuator object
    
    // a component of sizeT/sizeO must be greater 
    // than "direction" if it is non-zero.
    
    int i;
    for(i=0; i<OF_Resp_Time; i++) {
        if((sizeT[i] != 0 && sizeT[i] <= direction) ||
            (sizeO[i] != 0 && sizeO[i] <= direction)) {
            opserr << "ESOneActuator::setSize - wrong sizeTrial/Out\n"; 
            opserr << "see User Manual.\n";
            opserr << "sizeT = " << sizeT;
            opserr << "sizeO = " << sizeO;
            exit(1);
        }
    }
    if((sizeT[OF_Resp_Time] != 0 && sizeT[OF_Resp_Time] != 1) ||
        (sizeO[OF_Resp_Time] != 0 && sizeO[OF_Resp_Time] != 1)) {
        opserr << "ESOneActuator::setSize - wrong sizeTrial/Out\n"; 
        opserr << "see User Manual.\n";
        opserr << "sizeT = " << sizeT;
        opserr << "sizeO = " << sizeO;
        exit(1);
    }
     
    return OF_ReturnType_completed;
}


int ESOneActuator::commitState()
{
    return theControl->commitState();
}


int ESOneActuator::setup()
{
    // setup for ctrl/daq vectors of ESOneActuator
    sizeCtrl->Zero();
    sizeDaq->Zero();
    for(int i=0; i<OF_Resp_Time; i++) {
        (*sizeCtrl)[i] = 1;
        (*sizeDaq)[i] = 1;
    }
    (*sizeCtrl)[OF_Resp_Time] = 1;
    (*sizeDaq)[OF_Resp_Time] = 1;
    
    this->setCtrlDaqSize();
    
    return OF_ReturnType_completed;
}


ExperimentalSetup* ESOneActuator::getCopy()
{
    ESOneActuator *theCopy = new ESOneActuator(*this);
    
    return theCopy;
}


void ESOneActuator::Print(OPS_Stream &s, int flag)
{
    s << "ExperimentalSetup: " << this->getTag(); 
    s << " type: ESOneActuator\n";
    s << " dir: " << direction << endln;
    if(theControl != 0)  {
        s << "\tExperimentalControl tag: " << theControl->getTag();
        s << *theControl;
    }
}


int ESOneActuator::transfTrialDisp(const Vector* disp)
{  
    (*cDisp)(0) = (*disp)(direction);
    
    return OF_ReturnType_completed;
}


int ESOneActuator::transfTrialVel(const Vector* vel)
{  
    (*cVel)(0) = (*vel)(direction);
    
    return OF_ReturnType_completed;
}


int ESOneActuator::transfTrialAccel(const Vector* accel)
{  
    (*cAccel)(0) = (*accel)(direction);
    
    return OF_ReturnType_completed;
}


int ESOneActuator::transfTrialForce(const Vector* force)
{  
    (*cForce)(0) = (*force)(direction);
    
    return OF_ReturnType_completed;
}


int ESOneActuator::transfTrialTime(const Vector* time)
{
    *cTime = *time;
    
    return OF_ReturnType_completed;
}


int ESOneActuator::transfDaqDisp(Vector* disp)
{  
    (*disp)(direction) = (*dDisp)(0);
    
    return OF_ReturnType_completed;
}


int ESOneActuator::transfDaqVel(Vector* vel)
{  
    (*vel)(direction) = (*dVel)(0);
    
    return OF_ReturnType_completed;
}


int ESOneActuator::transfDaqAccel(Vector* accel)
{  
    (*accel)(direction) = (*dAccel)(0);
    
    return OF_ReturnType_completed;
}


int ESOneActuator::transfDaqForce(Vector* force)
{  
    (*force)(direction) = (*dForce)(0);
    
    return OF_ReturnType_completed;
}


int ESOneActuator::transfDaqTime(Vector* time)
{  
    *time = *dTime;
    
    return OF_ReturnType_completed;
}

