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

#ifndef ShadowExpSite_h
#define ShadowExpSite_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 09/06
// Revision: A
//
// Description: This file contains the class definition for 
// ShadowExpSite. A ShadowExpSite can communicate to an ActorExpSite.
// This object inherits from Shadow, and runs on the client program.

#include "ExperimentalSite.h"

#include <Shadow.h>

class ShadowExpSite : public ExperimentalSite, public Shadow
{
public:
    // constructors
    ShadowExpSite(int tag, 
        Channel &theChannel,
        int dataSize = OF_Network_dataSize,
        FEM_ObjectBroker *theObjectBroker = 0);
    ShadowExpSite(int tag, 
        ExperimentalSetup *setup,
        Channel &theChannel,
        int dataSize = OF_Network_dataSize,
        FEM_ObjectBroker *theObjectBroker = 0);
    ShadowExpSite(const ShadowExpSite& es);
    
    // destructor
    virtual ~ShadowExpSite();
    
    // method to get class type
    const char *getClassType() const {return "ShadowExpSite";};
    
    // public methods to set and to get response
    virtual int setup();
    virtual int setSize(ID sizeT, ID sizeO);
    
    virtual int setTrialResponse(const Vector* disp,
        const Vector* vel,
        const Vector* accel,
        const Vector* force,
        const Vector* time);
    
    virtual int checkDaqResponse();
    
    virtual int commitState(Vector *time = 0);
    
    virtual ExperimentalSite *getCopy();
    
    // public methods for output
    void Print(OPS_Stream &s, int flag = 0);
    
protected:
    // data size of vectors in Channel
    int dataSize;
    
    // vectors in Channel
    Vector sendV;
    Vector recvV;
    
    Vector *bDisp, *bVel, *bAccel, *bForce, *bTime;
    Vector *rDisp, *rVel, *rAccel, *rForce, *rTime;
};

#endif
