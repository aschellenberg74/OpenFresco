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

#ifndef LocalExpSite_h
#define LocalExpSite_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 09/06
// Revision: A
//
// Description: This file contains the class definition for 
// LocalExpSite. A LocalExpSite is a concrete class
// for OpenFresco and responsible for managing data and 
// communicating between computational and experimental sites.

#include "ExperimentalSite.h"

class LocalExpSite : public ExperimentalSite
{
public:
    // constructors
    LocalExpSite(int tag, 
        ExperimentalSetup* setup);
    LocalExpSite(const LocalExpSite &es);
    
    // destructor
    virtual ~LocalExpSite();
    
    // method to get class type
    const char *getClassType() const {return "LocalExpSite";};
    
    // public methods to set and to get response
    virtual int setup();
    virtual int setSize(ID sizeT, ID sizeO);
    
    virtual int setTrialResponse(const Vector* disp,
        const Vector* vel,
        const Vector* accel,
        const Vector* force,
        const Vector* time);
    
    virtual int checkDaqResponse();
    
    virtual ExperimentalSite *getCopy();
    
    // public methods for output
    void Print(OPS_Stream &s, int flag = 0);
};

#endif
