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

#ifndef UndershootErrorFilter_h
#define UndershootErrorFilter_h

// Written: Yoshi (yos@catfish.dpri.kyoto-u.ac.jp)
// Created: 09/06
// Revision: A
//
// Purpose: This file contains the class definition for 
// UndershootErrorFilter.

#include "ErrorFilter.h"
#include <OPS_Globals.h>

class UndershootErrorFilter : public ErrorFilter
{
public:
    // constructors
    UndershootErrorFilter(int tag, double error);
    UndershootErrorFilter(const UndershootErrorFilter& uef);
    
    // destructor
    virtual ~UndershootErrorFilter();
    
    virtual double filtering(double data);
    virtual void update();
    
    virtual SignalFilter *getCopy();
    void Print(OPS_Stream &s, int flag);
    
private:
    double undershoot;
    double data;
    double predata;
};

#endif
