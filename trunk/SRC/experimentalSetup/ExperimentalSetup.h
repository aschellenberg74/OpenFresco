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

#ifndef ExperimentalSetup_h
#define ExperimentalSetup_h

// Written: Yoshi (yos@catfish.dpri.kyoto-u.ac.jp)
// Created: 09/06
// Revision: A
//
// Description: This file contains the class definition for 
// ExperimentalSetup. 

#include <FrescoGlobals.h>
#include <TaggedObject.h>

class ArrayOfTaggedObjects;
class Vector;
class ID;

class ExperimentalCP;
class ExperimentalControl;

class ExperimentalSetup : public TaggedObject
{
public:
    // constructors
    ExperimentalSetup(int tag,
        ExperimentalControl* control = 0);
    ExperimentalSetup(const ExperimentalSetup& es);
    
    // destructor
    virtual ~ExperimentalSetup();
    
    // public methods to set and to obtain responses
    virtual int setSize(ID sizeT, ID sizeO) = 0;
    virtual int setup() = 0;
    virtual int setCtrlDaqSize();
    
    virtual int setTrialResponse(const Vector* disp,
        const Vector* vel,
        const Vector* accel,
        const Vector* force,
        const Vector* time);
    virtual int getTrialResponse(Vector* disp,
        Vector* vel,
        Vector* accel,
        Vector* force,
        Vector* time);
    virtual int setDaqResponse(const Vector* disp,
        const Vector* vel,
        const Vector* accel,
        const Vector* force,
        const Vector* time);
    virtual int getDaqResponse(Vector* disp,
        Vector* vel,
        Vector* accel,
        Vector* force,
        Vector* time);
    
    // public methods to transform the responses
    virtual int transfTrialResponse(const Vector* disp,
        const Vector* vel,
        const Vector* accel,
        const Vector* force,
        const Vector* time);
    virtual int transfDaqResponse(Vector* disp,
        Vector* vel,
        Vector* accel,
        Vector* force,
        Vector* time);
    
    virtual int commitState();
    
    virtual ExperimentalSetup *getCopy (void) = 0;
    
    void setCtrlDispFactor(const Vector& f);
    void setCtrlVelFactor(const Vector& f);
    void setCtrlAccelFactor(const Vector& f);
    void setCtrlForceFactor(const Vector& f);
    void setCtrlTimeFactor(const Vector& f);
    
    void setDaqDispFactor(const Vector& f);
    void setDaqVelFactor(const Vector& f);
    void setDaqAccelFactor(const Vector& f);
    void setDaqForceFactor(const Vector& f);
    void setDaqTimeFactor(const Vector& f);
    
    virtual ID getCtrlSize();
    virtual ID getDaqSize();
    virtual int getCtrlSize(int rType);
    virtual int getDaqSize(int rType);
    
    virtual ArrayOfTaggedObjects* getCPsCtrl();
    virtual ArrayOfTaggedObjects* getCPsDaq();
    
protected:
    // pointer to experimental control
    ExperimentalControl *theControl;
    
    // control data
    Vector *cDisp;
    Vector *cVel;
    Vector *cAccel;
    Vector *cForce;
    Vector *cTime;
    
    Vector *dDisp;
    Vector *dVel;
    Vector *dAccel;
    Vector *dForce;
    Vector *dTime;
    
    // factor data
    Vector *cDispFact;
    Vector *cVelFact;
    Vector *cAccelFact;
    Vector *cForceFact;
    Vector *cTimeFact;
    
    Vector *dDispFact;
    Vector *dVelFact;
    Vector *dAccelFact;
    Vector *dForceFact;
    Vector *dTimeFact;
    
    // size of ctrl/daq data
    // sizeCtrl/Daq[0]:disp, [1]:vel, [2]:accel, [3]:force, [4]:time
    ID *sizeCtrl; // size of ctrlV
    ID *sizeDaq; // size of daqV
    
    // Array of ExperimentalCP objects for trial and output:
    // if unused, the pointers are set to NULL.
    ArrayOfTaggedObjects *cpsCtrl;
    ArrayOfTaggedObjects *cpsDaq;
    
    // protected tranformation methods 
    virtual int transfTrialDisp(const Vector* disp) = 0;
    virtual int transfTrialVel(const Vector* vel) = 0;
    virtual int transfTrialAccel(const Vector* accel) = 0;
    virtual int transfTrialForce(const Vector* force) = 0;
    virtual int transfTrialTime(const Vector* time) = 0;
    
    virtual int transfDaqDisp(Vector* disp) = 0;
    virtual int transfDaqVel(Vector* vel) = 0;
    virtual int transfDaqAccel(Vector* accel) = 0;
    virtual int transfDaqForce(Vector* force) = 0;
    virtual int transfDaqTime(Vector* time) = 0;
    
    virtual void setCtrl();
    virtual void setDaq();
    virtual void setCtrlCPs(ArrayOfTaggedObjects &theCPs);
    virtual void setDaqCPs(ArrayOfTaggedObjects &theCPs);
};

#endif
