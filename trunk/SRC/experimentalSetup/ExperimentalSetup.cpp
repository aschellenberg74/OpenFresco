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

// Written: Yoshi (yos@catfish.dpri.kyoto-u.ac.jp)
// Created: 09/06
// Revision: A
//
// Description: This file contains the implementation of ExperimentalSetup.

#include "ExperimentalSetup.h"
#include <ExperimentalControl.h>

#include <stdlib.h>
#include <ArrayOfTaggedObjects.h>
#include <Vector.h>


ExperimentalSetup::ExperimentalSetup(int tag,
    ExperimentalControl *control)
    : TaggedObject(tag),
    theControl(control),
    cDisp(0), cVel(0), cAccel(0), cForce(0), cTime(0),
    dDisp(0), dVel(0), dAccel(0), dForce(0), dTime(0),
    cDispFact(0), cVelFact(0), cAccelFact(0), 
    cForceFact(0), cTimeFact(0),
    dDispFact(0), dVelFact(0), dAccelFact(0), 
    dForceFact(0), dTimeFact(0),
    sizeCtrl(0), sizeDaq(0),
    cpsCtrl(0), cpsDaq(0)
{
    sizeCtrl = new ID(OF_Resp_All);
    sizeDaq = new ID(OF_Resp_All);
    if (sizeCtrl == 0 || sizeDaq == 0) {
        opserr << "FATAL ExperimentalSetup::ExperimentalSetup - "
            << "fail to create ID."
            << endln;
        exit(OF_ReturnType_failed);
    }
}


ExperimentalSetup::ExperimentalSetup(const ExperimentalSetup& es)
    : TaggedObject(es),
    theControl(0),
    cDisp(0), cVel(0), cAccel(0), cForce(0), cTime(0),
    dDisp(0), dVel(0), dAccel(0), dForce(0), dTime(0),
    cDispFact(0), cVelFact(0), cAccelFact(0), 
    cForceFact(0), cTimeFact(0),
    dDispFact(0), dVelFact(0), dAccelFact(0), 
    dForceFact(0), dTimeFact(0),
    sizeCtrl(0), sizeDaq(0),
    cpsCtrl(0), cpsDaq(0)
{
    if (es.theControl != 0) {
        theControl = (es.theControl)->getCopy();
        if (theControl == 0) {
            opserr << "FATAL ExperimentalSetup::ExperimentalSetup - "
                << "fail to get a copy of control system."
                << endln;
            exit(OF_ReturnType_failed);
        }
        theControl->setup();
    }
    
    if (es.cpsCtrl != 0) {
        //this->setCtrlCPs(*(es.cpsCtrl));
        //this->setDaqCPs(*(es.cpsDaq));
    } else {
        sizeCtrl = new ID(OF_Resp_All);
        sizeDaq = new ID(OF_Resp_All);
        if (sizeCtrl == 0 || sizeDaq == 0) {
            opserr << "FATAL ExperimentalSetup::ExperimentalSetup - "
                << "fail to create ID."
                << endln;
            exit(OF_ReturnType_failed);
        }
        *sizeCtrl = *(es.sizeCtrl);
        *sizeDaq = *(es.sizeDaq);
        
        this->setCtrl();
        this->setDaq();
        
        if (es.cDispFact != 0)
            *cDispFact = *(es.cDispFact);
        if (es.cVelFact != 0)
            *cVelFact = *(es.cVelFact);
        if (es.cAccelFact != 0)
            *cAccelFact = *(es.cAccelFact);
        if (es.cForceFact != 0)
            *cForceFact = *(es.cForceFact);
        if (es.cTimeFact != 0)
            *cTimeFact = *(es.cTimeFact);
        if (es.dDispFact != 0)
            *dDispFact = *(es.dDispFact);
        if (es.dVelFact != 0)
            *dVelFact = *(es.dVelFact);
        if (es.dAccelFact != 0)
            *dAccelFact = *(es.dAccelFact);
        if (es.dForceFact != 0)
            *dForceFact = *(es.dForceFact);
        if (es.dTimeFact != 0)
            *dTimeFact = *(es.dTimeFact);
    }
}


ExperimentalSetup::~ExperimentalSetup()
{
    if (theControl != 0) 
        delete theControl;
    
    if (cDisp != 0) 
        delete cDisp;
    if (cVel != 0) 
        delete cVel;
    if (cAccel != 0) 
        delete cAccel;
    if (cForce != 0) 
        delete cForce;
    if (cTime != 0) 
        delete cTime;
    
    if (dDisp != 0) 
        delete dDisp;
    if (dVel != 0) 
        delete dVel;
    if (dAccel != 0) 
        delete dAccel;
    if (dForce != 0) 
        delete dForce;
    if (dTime != 0)
        delete dTime;
    
    if (cDispFact != 0) 
        delete cDispFact;
    if (cVelFact != 0) 
        delete cVelFact;
    if (cAccelFact != 0) 
        delete cAccelFact;
    if (cForceFact != 0) 
        delete cForceFact;
    if (cTimeFact != 0)
        delete cTimeFact;
    
    if (dDispFact != 0) 
        delete dDispFact;
    if (dVelFact != 0) 
        delete dVelFact;
    if (dAccelFact != 0) 
        delete dAccelFact;
    if (dForceFact != 0) 
        delete dForceFact;
    if (dTimeFact != 0)
        delete dTimeFact;
    
    if (sizeCtrl != 0)
        delete sizeCtrl;
    if (sizeDaq != 0)
        delete sizeDaq;
    
    if (cpsCtrl != 0)
        delete cpsCtrl;
    if (cpsDaq != 0)
        delete cpsDaq;
}


// must be called in setup() of concrete classes
int ExperimentalSetup::setCtrlDaqSize()
{
    this->setCtrl();
    this->setDaq();
    
    if (theControl != 0) {
        theControl->setSize(*sizeCtrl, *sizeDaq);
        theControl->setup();
    }
    
    return OF_ReturnType_completed;
}


int ExperimentalSetup::setTrialResponse(const Vector* disp, 
    const Vector* vel,
    const Vector* accel,
    const Vector* force,
    const Vector* time)
{
    // transform data
    this->transfTrialResponse(disp, vel, accel, force, time);

    if (theControl != 0) {
        theControl->setTrialResponse(cDisp, cVel, cAccel, cForce, cTime);
        
        theControl->getDaqResponse(dDisp, dVel, dAccel, dForce, dTime);
    }
    
    return OF_ReturnType_completed;
}


int ExperimentalSetup::getTrialResponse(Vector* disp, 
    Vector* vel,
    Vector* accel,
    Vector* force,
    Vector* time)
{
    if (cDisp != 0) {
        *disp = *cDisp;
    }
    if (cVel != 0) {
        *vel = *cVel;
    }
    if (cAccel != 0) {
        *accel = *cAccel;
    }
    if (cForce != 0) {
        *force = *cForce;
    }
    if (cTime != 0) {
        *time = *cTime;
    }
    
    return OF_ReturnType_completed;
}


int ExperimentalSetup::setDaqResponse(const Vector* disp, 
    const Vector* vel,
    const Vector* accel,
    const Vector* force,
    const Vector* time)
{
    if (dDisp != 0) {
        *dDisp = *disp;
    }
    if (dVel != 0) {
        *dVel = *vel;
    }
    if (dAccel != 0) {
        *dAccel = *accel;
    }
    if (dForce != 0) {
        *dForce = *force;
    }
    if (dTime != 0) {
        *dTime = *time;
    }
    
    return OF_ReturnType_completed;
}


int ExperimentalSetup::getDaqResponse(Vector* disp, 
    Vector* vel,
    Vector* accel,
    Vector* force,
    Vector* time)
{
    if (theControl != 0)
        theControl->getDaqResponse(dDisp, dVel, dAccel, dForce, dTime);
    
    // transform data
    this->transfDaqResponse(disp, vel, accel, force, time);
    
    return OF_ReturnType_completed;
}


int ExperimentalSetup::transfTrialResponse(const Vector* disp, 
    const Vector* vel,
    const Vector* accel,
    const Vector* force,
    const Vector* time)
{
    // transform data
    if (disp != 0) {
        this->transfTrialDisp(disp);
        for (int i=0; i<(*sizeCtrl)[OF_Resp_Disp]; i++)
            (*cDisp)[i] *= (*cDispFact)[i];
    }
    if (vel != 0) {
        this->transfTrialVel(vel);
        for (int i=0; i<(*sizeCtrl)[OF_Resp_Vel]; i++)
            (*cVel)[i] *= (*cVelFact)[i];
    }
    if (accel != 0) {
        this->transfTrialAccel(accel);
        for (int i=0; i<(*sizeCtrl)[OF_Resp_Accel]; i++)
            (*cAccel)[i] *= (*cAccelFact)[i];
    }
    if (force != 0) {
        this->transfTrialForce(force);
        for (int i=0; i<(*sizeCtrl)[OF_Resp_Force]; i++)
            (*cForce)[i] *= (*cForceFact)[i];
    }
    if (time != 0) {
        this->transfTrialTime(time);
        for (int i=0; i<(*sizeCtrl)[OF_Resp_Time]; i++)
            (*cTime)[i] *= (*cTimeFact)[i];
    }
    
    return OF_ReturnType_completed;
}


int ExperimentalSetup::transfDaqResponse(Vector* disp,
    Vector* vel,
    Vector* accel,
    Vector* force,
    Vector* time)
{
    // transform data
    if (disp != 0) {
        for (int i=0; i<(*sizeDaq)[OF_Resp_Disp]; i++)
            (*dDisp)[i] *= (*dDispFact)[i];
        this->transfDaqDisp(disp);
    }
    if (vel != 0) {
        for (int i=0; i<(*sizeDaq)[OF_Resp_Vel]; i++)
            (*dVel)[i]  *= (*dVelFact)[i];
        this->transfDaqVel(vel);
    }
    if (accel != 0) {
        for (int i=0; i<(*sizeDaq)[OF_Resp_Accel]; i++)
            (*dAccel)[i] *= (*dAccelFact)[i];
        this->transfDaqAccel(accel);
    }
    if (force != 0) {
        for (int i=0; i<(*sizeDaq)[OF_Resp_Force]; i++)
            (*dForce)[i] *= (*dForceFact)[i];
        this->transfDaqForce(force);
    }
    if (time != 0) {
        for (int i=0; i<(*sizeDaq)[OF_Resp_Time]; i++)
            (*dTime)[i] *= (*dTimeFact)[i];
        this->transfDaqTime(time);
    }
    
    return OF_ReturnType_completed;
}


int ExperimentalSetup::commitState()
{
    if (theControl != 0) {
        theControl->commitState();
    }
    
    return OF_ReturnType_completed;
}


void ExperimentalSetup::setCtrlDispFactor(const Vector& f)
{
    if (f.Size() != getCtrlSize(OF_Resp_Disp)) {
        opserr << "FATAL ExperimentalSetup::setCtrlDispFactor - "
            << "invalid size\n";
        exit(OF_ReturnType_failed);
    }
    *cDispFact = f;
}


void ExperimentalSetup::setCtrlVelFactor(const Vector& f)
{
    if (f.Size() != getCtrlSize(OF_Resp_Vel)) {
        opserr << "FATAL ExperimentalSetup::setCtrlVelFactor - "
            << "invalid size\n";
        exit(OF_ReturnType_failed);
    }
    *cVelFact = f;
}


void ExperimentalSetup::setCtrlAccelFactor(const Vector& f)
{
    if (f.Size() != getCtrlSize(OF_Resp_Accel)) {
        opserr << "FATAL ExperimentalSetup::setCtrlAccelFactor - "
            << "invalid size\n";
        exit(OF_ReturnType_failed);
    }
    *cAccelFact = f;
}


void ExperimentalSetup::setCtrlForceFactor(const Vector& f)
{
    if (f.Size() != getCtrlSize(OF_Resp_Force)) {
        opserr << "FATAL ExperimentalSetup::setCtrlForceFactor - "
            << "invalid size\n";
        exit(OF_ReturnType_failed);
    }
    *cForceFact = f;
}


void ExperimentalSetup::setCtrlTimeFactor(const Vector& f)
{
    if (f.Size() != getCtrlSize(OF_Resp_Time)) {
        opserr << "FATAL ExperimentalSetup::setCtrlTimeFactor - "
            << "invalid size\n";
        exit(OF_ReturnType_failed);
    }
    *cTimeFact = f;
}


void ExperimentalSetup::setDaqDispFactor(const Vector& f)
{
    if (f.Size() != getDaqSize(OF_Resp_Disp)) {
        opserr << "FATAL ExperimentalSetup::setDaqDispFactor - "
            << "invalid size\n";
        exit(OF_ReturnType_failed);
    }
    *dDispFact = f;
}


void ExperimentalSetup::setDaqVelFactor(const Vector& f)
{
    if (f.Size() != getDaqSize(OF_Resp_Vel)) {
        opserr << "FATAL ExperimentalSetup::setDaqVelFactor - "
            << "invalid size\n";
        exit(OF_ReturnType_failed);
    }
    *dVelFact = f;
}


void ExperimentalSetup::setDaqAccelFactor(const Vector& f)
{
    if (f.Size() != getDaqSize(OF_Resp_Accel)) {
        opserr << "FATAL ExperimentalSetup::setDaqAccelFactor - "
            << "invalid size\n";
        exit(OF_ReturnType_failed);
    }
    *dAccelFact = f;
}


void ExperimentalSetup::setDaqForceFactor(const Vector& f)
{
    if (f.Size() != getDaqSize(OF_Resp_Force)) {
        opserr << "FATAL ExperimentalSetup::setDaqForceFactor - "
            << "invalid size\n";
        exit(OF_ReturnType_failed);
    }
    *dForceFact = f;
}


void ExperimentalSetup::setDaqTimeFactor(const Vector& f)
{
    if (f.Size() != getDaqSize(OF_Resp_Time)) {
        opserr << "FATAL ExperimentalSetup::setDaqTimeFactor - "
            << "invalid size\n";
        exit(OF_ReturnType_failed);
    }
    *dTimeFact = f;
}


ID ExperimentalSetup::getCtrlSize()
{
    return *sizeCtrl;
}


ID ExperimentalSetup::getDaqSize()
{
    return *sizeDaq;
}


int ExperimentalSetup::getCtrlSize(int rType)
{
    return (*sizeCtrl)[rType];
}


int ExperimentalSetup::getDaqSize(int rType)
{
    return (*sizeDaq)[rType];
}


ArrayOfTaggedObjects* ExperimentalSetup::getCPsCtrl()
{
    return cpsCtrl;
}


ArrayOfTaggedObjects* ExperimentalSetup::getCPsDaq()
{
    return cpsDaq;
}


void ExperimentalSetup::setCtrl()
{
    if (cDisp != 0) {
        delete cDisp;
        cDisp = 0;
    }
    if (cVel != 0) {
        delete cVel;
        cVel = 0;
    }
    if (cAccel != 0) {
        delete cAccel;
        cAccel = 0;
    }
    if (cForce != 0) {
        delete cForce;
        cForce = 0;
    }
    if (cTime != 0) {
        delete cTime;
        cTime = 0;
    }
    if (cDispFact != 0) {
        delete cDispFact;
        cDispFact = 0;
    }
    if (cVelFact != 0) {
        delete cVelFact;
        cVelFact = 0;
    }
    if (cAccelFact != 0) {
        delete cAccelFact;
        cAccelFact = 0;
    }
    if (cForceFact != 0) {
        delete cForceFact;
        cForceFact = 0;
    }
    if (cTimeFact != 0) {
        delete cTimeFact;
        cTimeFact = 0;
    }

    int size, i;
    size = (*sizeCtrl)[OF_Resp_Disp];
    if (size != 0) {
        cDisp = new Vector(size);
        if (cDispFact == 0) {
            cDispFact = new Vector(size);
            for (i=0; i<size; i++)
                (*cDispFact)[i] = 1.0;
        }
        if (cDisp == 0 || cDispFact == 0) {
            opserr << "FATAL ExperimentalSetup::setCtrl - "
                << "failed to create Vector\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeCtrl)[OF_Resp_Vel];
    if (size != 0) {
        cVel = new Vector(size);
        if (cVelFact == 0) {
            cVelFact = new Vector(size);
            for (i=0; i<size; i++)
                (*cVelFact)[i] = 1.0;
        }
        if (cVel == 0 || cVelFact == 0) {
            opserr << "FATAL ExperimentalSetup::setCtrl - "
                << "failed to create Vector\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeCtrl)[OF_Resp_Accel];
    if (size != 0) {
        cAccel = new Vector(size);
        if (cAccelFact == 0) {
            cAccelFact = new Vector(size);
            for (i=0; i<size; i++)
                (*cAccelFact)[i] = 1.0;
        }
        if (cAccel == 0 || cAccelFact == 0) {
            opserr << "FATAL ExperimentalSetup::setCtrl - "
                << "failed to create Vector\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeCtrl)[OF_Resp_Force];
    if (size != 0) {
        cForce = new Vector(size);
        if (cForceFact == 0) {
            cForceFact = new Vector(size);
            for (i=0; i<size; i++)
                (*cForceFact)[i] = 1.0;
        }
        if (cForce == 0 || cForceFact == 0) {
            opserr << "FATAL ExperimentalSetup::setCtrl - "
                << "failed to create Vector\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeCtrl)[OF_Resp_Time];
    if (size != 0) {
        cTime = new Vector(size);
        if (cTimeFact == 0) {
            cTimeFact = new Vector(size);
            for (i=0; i<size; i++)
                (*cTimeFact)[i] = 1.0;
        }
        if (cTime == 0 || cTimeFact == 0) {
            opserr << "FATAL ExperimentalSetup::setCtrl - "
                << "failed to create Vector\n";
            exit(OF_ReturnType_failed);
        }
    }
}


void ExperimentalSetup::setDaq()
{
    if (dDisp != 0) {
        delete dDisp;
        dDisp = 0;
    }
    if (dVel != 0) {
        delete dVel;
        dVel = 0;
    }
    if (dAccel != 0) {
        delete dAccel;
        dAccel = 0;
    }
    if (dForce != 0) {
        delete dForce;
        dForce = 0;
    }
    if (dTime != 0) {
        delete dTime;
        dTime = 0;
    }
    if (dDispFact != 0) {
        delete dDispFact;
        dDispFact = 0;
    }
    if (dVelFact != 0) {
        delete dVelFact;
        dVelFact = 0;
    }
    if (dAccelFact != 0) {
        delete dAccelFact;
        dAccelFact = 0;
    }
    if (dForceFact != 0) {
        delete dForceFact;
        dForceFact = 0;
    }
    if (dTimeFact != 0) {
        delete dTimeFact;
        dTimeFact = 0;
    }

    int size, i;
    size = (*sizeDaq)[OF_Resp_Disp];
    if (size != 0) {
        dDisp = new Vector(size);
        if (dDispFact == 0) {
            dDispFact = new Vector(size);
            for (i=0; i<size; i++)
                (*dDispFact)[i] = 1.0;
        }
        if (dDisp == 0 || dDispFact == 0) {
            opserr << "FATAL ExperimentalSetup::setDaq - "
                << "failed to create Vector\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeDaq)[OF_Resp_Vel];
    if (size != 0) {
        dVel = new Vector(size);
        if (dVelFact == 0) {
            dVelFact = new Vector(size);
            for (i=0; i<size; i++)
                (*dVelFact)[i] = 1.0;
        }
        if (dVel == 0 || dVelFact == 0) {
            opserr << "FATAL ExperimentalSetup::setDaq - "
                << "failed to create Vector\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeDaq)[OF_Resp_Accel];
    if (size != 0) {
        dAccel = new Vector(size);
        if (dAccelFact == 0) {
            dAccelFact = new Vector(size);
            for (i=0; i<size; i++)
                (*dAccelFact)[i] = 1.0;
        }
        if (dAccel == 0 || dAccelFact == 0) {
            opserr << "FATAL ExperimentalSetup::setDaq - "
                << "failed to create Vector\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeDaq)[OF_Resp_Force];
    if (size != 0) {
        dForce = new Vector(size);
        if (dForceFact == 0) {
            dForceFact = new Vector(size);
            for (i=0; i<size; i++)
                (*dForceFact)[i] = 1.0;
        }
        if (dForce == 0 || dForceFact == 0) {
            opserr << "FATAL ExperimentalSetup::setDaq - "
                << "failed to create Vector\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeDaq)[OF_Resp_Time];
    if (size != 0) {
        dTime = new Vector(size);
        if (dTimeFact == 0) {
            dTimeFact = new Vector(size);
            for (i=0; i<size; i++)
                (*dTimeFact)[i] = 1.0;
        }
        if (dTime == 0 || dTimeFact == 0) {
            opserr << "FATAL ExperimentalSetup::setDaq - "
                << "failed to create Vector\n";
            exit(OF_ReturnType_failed);
        }
    }
}


/*void ExperimentalSetup::setCtrlCPs(ArrayOfTaggedObjects &theCPs)
{
    int nCtrl = theCPs.getNumComponents();
    if (cpsCtrl != 0)
        delete cpsCtrl;
    if (nCtrl == 0) {
        opserr << "invalid size of cpsCtrl";
    }
    cpsCtrl = new ArrayOfTaggedObjects(nCtrl*OF_Resp_All);
    
    if (sizeCtrl != 0)
        delete [] sizeCtrl;
    sizeCtrl = new ID(OF_Resp_All);
    sizeCtrl->Zero();
    
    int i;
    for (i=1; i<=nCtrl; i++) {
        TaggedObject *mc = theCPs.getComponentPtr(i);
        if (mc == 0) {
            opserr << "FATAL ExperimentalSetup::setCtrlCPs - "
                << "failed to get ExperimentalCP\n";
            exit(OF_ReturnType_failed);
        }
        ExperimentalCP *theCP = (ExperimentalCP*)mc;
        cpsCtrl->addComponent(theCP->getCopy());
        
        int respType = theCP->getResponseType();
        switch(respType) {
        case OF_Resp_Disp:
            (*sizeCtrl)[OF_Resp_Disp]++;
            break;
        case OF_Resp_Vel:
            (*sizeCtrl)[OF_Resp_Vel]++;
            break;
        case OF_Resp_Accel:
            (*sizeCtrl)[OF_Resp_Accel]++;
            break;
        case OF_Resp_Force:
            (*sizeCtrl)[OF_Resp_Force]++;
            break;
        case OF_Resp_Time:
            (*sizeCtrl)[OF_Resp_Time]++;
            break;
        case OF_Resp_All:
            for (int i=0; i<OF_Resp_All; i++)
                (*sizeCtrl)[i]++;
            break;
        }
    }
    
    this->setCtrl();
    
    // set factor
    int di = 0, vi = 0, ai = 0, fi = 0, ti = 0;
    for (i=1; i<=nCtrl; i++) {
        ExperimentalCP *theCP = (ExperimentalCP*)(cpsCtrl->getComponentPtr(i));
        int respType = theCP->getResponseType();
        switch(respType) {
        case OF_Resp_Disp:
            (*cDispFact)[di++] = theCP->getFactor();
            break;
        case OF_Resp_Vel:
            (*cVelFact)[vi++] = theCP->getFactor();
            break;
        case OF_Resp_Accel:
            (*cAccelFact)[ai++] = theCP->getFactor();
            break;
        case OF_Resp_Force:
            (*cForceFact)[fi++] = theCP->getFactor();
            break;
        case OF_Resp_Time:
            (*cTimeFact)[ti++] = theCP->getFactor();
            break;
        case OF_Resp_All:
            (*cDispFact)[di++] = theCP->getFactor();
            (*cVelFact)[vi++] = theCP->getFactor();
            (*cAccelFact)[ai++] = theCP->getFactor();
            (*cForceFact)[fi++] = theCP->getFactor();
            (*cTimeFact)[ti++] = theCP->getFactor();
            break;
        }
    }
}


void ExperimentalSetup::setDaqCPs(ArrayOfTaggedObjects &theCPs)
{
    int nDaq = theCPs.getNumComponents();
    if (cpsDaq != 0)
        delete cpsDaq;
    if (nDaq == 0) {
        opserr << "invalid size of cpsDaq";
    }
    cpsDaq = new ArrayOfTaggedObjects(nDaq*OF_Resp_All);
    
    if (sizeDaq != 0)
        delete [] sizeDaq;
    sizeDaq = new ID(OF_Resp_All);
    sizeDaq->Zero();
    
    int i;
    for (i=1; i<=nDaq; i++) {
        TaggedObject *mc = theCPs.getComponentPtr(i);
        if (mc == 0) {
            opserr << "FATAL ExperimentalSetup::setDaqCPs - "
                << "failed to get ExperimentalCP\n";
            exit(OF_ReturnType_failed);
        }
        ExperimentalCP *theCP = (ExperimentalCP*)mc;
        cpsDaq->addComponent(theCP->getCopy());
        
        int respType = theCP->getResponseType();
        switch(respType) {
        case OF_Resp_Disp:
            (*sizeDaq)[OF_Resp_Disp]++;
            break;
        case OF_Resp_Vel:
            (*sizeDaq)[OF_Resp_Vel]++;
            break;
        case OF_Resp_Accel:
            (*sizeDaq)[OF_Resp_Accel]++;
            break;
        case OF_Resp_Force:
            (*sizeDaq)[OF_Resp_Force]++;
            break;
        case OF_Resp_Time:
            (*sizeDaq)[OF_Resp_Time]++;
            break;
        case OF_Resp_All:
            for (int i=0; i<OF_Resp_All; i++)
                (*sizeDaq)[i]++;
            break;
        }
    }
    
    this->setDaq();
    
    // set factor
    int di = 0, vi = 0, ai = 0, fi = 0, ti = 0;
    for (i=1; i<=nDaq; i++) {
        ExperimentalCP *theCP = (ExperimentalCP*)(cpsDaq->getComponentPtr(i));
        int respType = theCP->getResponseType();
        switch(respType) {
        case OF_Resp_Disp:
            (*dDispFact)[di++] = theCP->getFactor();
            break;
        case OF_Resp_Vel:
            (*dVelFact)[vi++] = theCP->getFactor();
            break;
        case OF_Resp_Accel:
            (*dAccelFact)[ai++] = theCP->getFactor();
            break;
        case OF_Resp_Force:
            (*dForceFact)[fi++] = theCP->getFactor();
            break;
        case OF_Resp_Time:
            (*dTimeFact)[ti++] = theCP->getFactor();
            break;
        case OF_Resp_All:
            (*dDispFact)[di++] = theCP->getFactor();
            (*dVelFact)[vi++] = theCP->getFactor();
            (*dAccelFact)[ai++] = theCP->getFactor();
            (*dForceFact)[fi++] = theCP->getFactor();
            (*dTimeFact)[ti++] = theCP->getFactor();
            break;
        }
    }
}*/
