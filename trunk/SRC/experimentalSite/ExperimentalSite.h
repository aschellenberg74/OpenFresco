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

#ifndef ExperimentalSite_h
#define ExperimentalSite_h

// Written: Yoshi (yos@catfish.dpri.kyoto-u.ac.jp)
// Created: 09/06
// Revision: A
//
// Description: This file contains the class definition for 
// ExperimentalSite. An ExperimentalSite is an interface class
// for OpenFresco and responsible for managing data and 
// communicating between computational and experimental sites.

#include <FrescoGlobals.h>
#include <ExperimentalSetup.h>

#include <TaggedObject.h>
#include <ID.h>
#include <Vector.h>

class ExperimentalSite : public TaggedObject
{
public:
    // constructors
    ExperimentalSite(int tag, 
        ExperimentalSetup *setup = 0);
    ExperimentalSite(const ExperimentalSite &site);
    
    // destructor
    virtual ~ExperimentalSite();
    
    // public methods to set and to get responses
    virtual int setSize(ID sizeT, ID sizeO);
    virtual int setup() = 0;
    
    virtual int setTrialResponse(const Vector* disp,
        const Vector* vel,
        const Vector* accel,
        const Vector* force,
        const Vector* time);
    virtual int setTrialDisp(const Vector* disp);
    
    virtual int getDaqResponse(Vector* disp,
        Vector* vel,
        Vector* accel,
        Vector* force,
        Vector* time);
    virtual int setDaqResponse(const Vector* disp,
        const Vector* vel,
        const Vector* accel,
        const Vector* force,
        const Vector* time);
    virtual int checkDaqResponse() = 0;
    
    virtual const Vector& getTrialDisp();
    virtual const Vector& getTrialVel();
    virtual const Vector& getTrialAccel();
    virtual const Vector& getTrialForce();
    virtual const Vector& getTrialTime();
    
    virtual const Vector& getDisp();
    virtual const Vector& getVel();
    virtual const Vector& getAccel();
    virtual const Vector& getForce();
    virtual const Vector& getTime();
    
    virtual int commitState();
    
    virtual ExperimentalSite *getCopy() = 0;
    
    virtual int getTrialSize(int rType);
    virtual int getOutSize(int rType);
    virtual int getCtrlSize(int rType);
    virtual int getDaqSize(int rType);
        
protected:
    // pointer of ExperimentalSetup
    ExperimentalSetup* theSetup;
    
    // trial data
    Vector* tDisp;
    Vector* tVel;
    Vector* tAccel;
    Vector* tForce;
    Vector* tTime;
    
    // output data
    Vector* Disp;
    Vector* Vel;
    Vector* Accel;
    Vector* Force;
    Vector* Time;
    
    // size of trial/out data
    // [0]:disp, [1]:vel, [2]:accel, [3]:force, [4]:time
    ID* sizeTrial;
    ID* sizeOut;
        
    // daqFlag = false (first time) / true (NOT first time)
    bool daqFlag;
    
    virtual void setTrial();
    virtual void setOut();
};

#endif
