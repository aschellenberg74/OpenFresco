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

#ifndef RemoteExpSite_h
#define RemoteExpSite_h

// Written: Yoshi (yos@catfish.dpri.kyoto-u.ac.jp)
// Created: 09/06
// Revision: A
//
// Description: This file contains the class definition for 
// RemoteExpSite. A RemoteExpSite can communicate to an ActorExpSite.
// This object inherits from Shadow, and runs on the client program.

#include <ID.h>
#include <Shadow.h>
#include <ExperimentalSite.h>
#include <ExperimentalSetup.h>
#include <ExperimentalControl.h>

class RemoteExpSite : public ExperimentalSite, public Shadow
{
public:
    // constructors
    RemoteExpSite(int tag, 
        Channel &theChannel,
        int dataSize = 10,
        FEM_ObjectBroker *theObjectBroker = 0);
    RemoteExpSite(int tag, 
        ExperimentalSetup *setup,
        Channel &theChannel,
        int dataSize = 10,
        FEM_ObjectBroker *theObjectBroker = 0); 
    RemoteExpSite(const RemoteExpSite& es);

    // destructor
    virtual ~RemoteExpSite();
    
    // public methods to set and to obtain responses
    virtual int setSize(ID sizeT, ID sizeO);
    virtual int setup();
    
    virtual int setTrialResponse(const Vector* disp,
        const Vector* vel,
        const Vector* accel,
        const Vector* force,
        const Vector* time);
    
    virtual int checkDaqResponse();
    
    virtual int commitState();
    
    virtual ExperimentalSite *getCopy();
    
    // public methods for output
    void Print(OPS_Stream &s, int flag);
    
protected:
    // for communicating through Channel
    ID msgData;

    // data size of vector through Channel
    int sendDataSize, recvDataSize;

    // vector in Channel
    Vector *sendV;
    Vector *recvV;
    
    Vector *bDisp, *bVel, *bAccel, *bForce, *bTime;
    Vector *rDisp, *rVel, *rAccel, *rForce, *rTime;
};

#endif




