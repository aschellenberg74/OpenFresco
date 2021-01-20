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

#ifndef ESFErrorSimTimeDelay_h
#define ESFErrorSimTimeDelay_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 07/18
// Revision: A
//
// Purpose: This file contains the class definition for 
// ESFErrorSimTimeDelay.

#include "ESFErrorSimulation.h"

class ESFErrorSimTimeDelay : public ESFErrorSimulation
{
public:
    // constructors
    ESFErrorSimTimeDelay(int tag, double delay);
    ESFErrorSimTimeDelay(const ESFErrorSimTimeDelay& esf);
    
    // destructor
    virtual ~ESFErrorSimTimeDelay();
    
    // method to get class type
    const char *getClassType() const {return "ESFErrorSimTimeDelay";};
    
    virtual double filtering(double data);
    virtual void update();
    
    virtual ExperimentalSignalFilter *getCopy();
    
    // public methods for output
    void Print(OPS_Stream &s, int flag = 0);
    
private:
    double delay;
    double data;
    double predata;
};

#endif
