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

#ifndef ActorExpSite_h
#define ActorExpSite_h

// Written: Yoshi (yos@catfish.dpri.kyoto-u.ac.jp)
// Created: 09/06
// Revision: A
//
// Description: This file contains the class definition for 
// ActorExpSite. An ActorExpSite can communicate with a RemoteExpSite.
// This object inherits from Actor, and runs on the server program.

#ifndef _bool_h
#include "bool.h"
#endif

#include <ID.h>
#include <Actor.h>
#include <ExperimentalSite.h>
#include <ExperimentalSetup.h>

class ActorExpSite : public ExperimentalSite, public Actor
{
public:
    // constructors
    ActorExpSite(int tag, 
        ExperimentalSetup *setup,
        Channel &theChannel,
        FEM_ObjectBroker *theObjectBroker = 0);
    ActorExpSite(int tag, 
        ExperimentalControl *control,
        Channel &theChannel,
        FEM_ObjectBroker *theObjectBroker = 0);
    ActorExpSite(const ActorExpSite& es);
    
    // destructor
    virtual ~ActorExpSite();
    
    // server loop
    int run();
    
    // public methods to set and obtain responses
    virtual int setSize(ID sizeT, ID sizeO);
    virtual int setup();
    
    virtual int setTrialResponse(const Vector* disp,
        const Vector* vel,
        const Vector* accel,
        const Vector* force,
        const Vector* time);
    
    virtual int checkDaqResponse();    
    virtual int setSendDaqResponse();
    
    virtual int commitState();
    
    virtual ExperimentalSite *getCopy();
    
    // public methods for output
    void Print(OPS_Stream &s, int flag);
    
protected:
    // pointer of ExperimentalControl
    ExperimentalControl* theControl;
        
    // data size of vectors in Channel
    int dataSize;
    
    // vectors in Channel
    Vector sendV;
    Vector recvV;
};

#endif
