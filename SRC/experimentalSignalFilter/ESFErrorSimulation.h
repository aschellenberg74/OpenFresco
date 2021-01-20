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

#ifndef ESFErrorSimulation_h
#define ESFErrorSimulation_h

// Written: Yoshi (yos@catfish.dpri.kyoto-u.ac.jp)
// Created: 09/06
// Revision: A
//
// Description: This file contains the class definition for 
// ESFErrorSimulation.

#include "ExperimentalSignalFilter.h"

class ESFErrorSimulation : public ExperimentalSignalFilter
{
public:
    // constructors
    ESFErrorSimulation(int tag);
    ESFErrorSimulation(const ESFErrorSimulation& esf);
    
    // destructor
    virtual ~ESFErrorSimulation();
    
    // method to get class type
    const char *getClassType() const {return "ESFErrorSimulation";};
    
    virtual double filtering(double data) = 0;
    virtual Vector& converting(Vector* td);
    virtual Vector& converting(Vector* td, Vector* tf);
    virtual int setSize(const int sz);
    virtual void update() = 0;
    
    virtual ExperimentalSignalFilter *getCopy() = 0;
};

#endif
