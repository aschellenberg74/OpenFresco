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

#ifndef ExperimentalSetup_h
#define ExperimentalSetup_h

// Written: Yoshi (yos@catfish.dpri.kyoto-u.ac.jp)
// Created: 09/06
// Revision: A
//
// Description: This file contains the class definition for 
// ExperimentalSetup. 

#include <FrescoGlobals.h>
#include <ExpSetupResponse.h>
#include <ExperimentalControl.h>

#include <TaggedObject.h>
#include <ID.h>
#include <Vector.h>

class Response;

class ExperimentalSetup : public TaggedObject
{
public:
    // constructors
    ExperimentalSetup(int tag,
        ExperimentalControl* control = 0);
    ExperimentalSetup(const ExperimentalSetup& es);
    
    // destructor
    virtual ~ExperimentalSetup();

    // method to get class type
    virtual const char *getClassType() const;
    
    // public methods to set and to get response
    virtual int setup() = 0;
    virtual int setTrialOutSize();
    virtual int setCtrlDaqSize();
    virtual int checkSize(ID sizeT, ID sizeO);
    
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
    
    virtual ExperimentalSetup *getCopy() = 0;
    
    // public methods for experimental setup recorder
    virtual Response *setResponse(const char **argv, int argc,
        OPS_Stream &output);
    virtual int getResponse(int responseID, Information &info);
    
    void setTrialDispFactor(const Vector& f);
    void setTrialVelFactor(const Vector& f);
    void setTrialAccelFactor(const Vector& f);
    void setTrialForceFactor(const Vector& f);
    void setTrialTimeFactor(const Vector& f);
    
    void setOutDispFactor(const Vector& f);
    void setOutVelFactor(const Vector& f);
    void setOutAccelFactor(const Vector& f);
    void setOutForceFactor(const Vector& f);
    void setOutTimeFactor(const Vector& f);
    
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
    
    virtual ID getTrialSize();
    virtual ID getOutSize();
    virtual ID getCtrlSize();
    virtual ID getDaqSize();

    virtual int getTrialSize(int rType);
    virtual int getOutSize(int rType);
    virtual int getCtrlSize(int rType);
    virtual int getDaqSize(int rType);
    
protected:
    // pointer to experimental control
    ExperimentalControl *theControl;
    
    // size of trial/out data
    // [0]:disp, [1]:vel, [2]:accel, [3]:force, [4]:time
    ID* sizeTrial;
    ID* sizeOut;
    
    // size of ctrl/daq data
    // [0]:disp, [1]:vel, [2]:accel, [3]:force, [4]:time
    ID *sizeCtrl;
    ID *sizeDaq;
    
    // trial/out data
    Vector *tDisp;
    Vector *tVel;
    Vector *tAccel;
    Vector *tForce;
    Vector *tTime;
    
    Vector *oDisp;
    Vector *oVel;
    Vector *oAccel;
    Vector *oForce;
    Vector *oTime;
    
    // ctrl/daq data
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
    
    // trial/out factor data
    Vector *tDispFact;
    Vector *tVelFact;
    Vector *tAccelFact;
    Vector *tForceFact;
    Vector *tTimeFact;
    
    Vector *oDispFact;
    Vector *oVelFact;
    Vector *oAccelFact;
    Vector *oForceFact;
    Vector *oTimeFact;
    
    // ctrl/daq factor data
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
    
    virtual void setTrial();
    virtual void setOut();
    virtual void setCtrl();
    virtual void setDaq();
};

extern bool OPF_addExperimentalSetup(ExperimentalSetup* newComponent);
extern bool OPF_removeExperimentalSetup(int tag);
extern ExperimentalSetup* OPF_getExperimentalSetup(int tag);
extern void OPF_clearExperimentalSetups();

#endif
