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

#ifndef ECSimulation_h
#define ECSimulation_h

// Written: Yoshi (yos@catfish.dpri.kyoto-u.ac.jp)
// Created: 09/06
// Revision: A
//
// Description: This file contains the class definition for 
// ECSimulation.

#include "ExperimentalControl.h"

class ECSimulation : public ExperimentalControl
{
public:
    // constructors
    ECSimulation(int tag);
    ECSimulation(const ECSimulation& ec);
    
    // destructor
    virtual ~ECSimulation();
    
    // method to get class type
    const char *getClassType() const {return "ECSimulation";};
    
    // public methods to set and to get response
    virtual int setup() = 0;
    virtual int setSize(ID sizeT, ID sizeO) = 0;
    
    virtual int setTrialResponse(const Vector* disp, 
        const Vector* vel,
        const Vector* accel,
        const Vector* force,
        const Vector* time) = 0;
    virtual int getDaqResponse(Vector* disp,
        Vector* vel,
        Vector* accel,
        Vector* force,
        Vector* time) = 0;
    
    virtual ExperimentalControl *getCopy() = 0;
    
protected:
    // protected methods to set and to get response
    virtual int control() = 0;
    virtual int acquire() = 0;
};

#endif
