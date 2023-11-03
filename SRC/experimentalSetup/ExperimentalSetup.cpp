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

// Written: Yoshi (yos@catfish.dpri.kyoto-u.ac.jp)
// Created: 09/06
// Revision: A
//
// Description: This file contains the implementation of ExperimentalSetup.

#include "ExperimentalSetup.h"

#include <TaggedObject.h>
#include <MapOfTaggedObjects.h>

static MapOfTaggedObjects theExperimentalSetups;


bool OPF_addExperimentalSetup(ExperimentalSetup* newComponent)
{
    return theExperimentalSetups.addComponent(newComponent);
}


bool OPF_removeExperimentalSetup(int tag)
{
    TaggedObject* obj = theExperimentalSetups.removeComponent(tag);
    if (obj != 0) {
        delete obj;
        return true;
    }
    return false;
}


ExperimentalSetup* OPF_getExperimentalSetup(int tag)
{
    TaggedObject* theResult = theExperimentalSetups.getComponentPtr(tag);
    if (theResult == 0) {
        opserr << "OPF_GetExperimentalSetup() - "
            << "none found with tag: " << tag << endln;
        return 0;
    }
    ExperimentalSetup* theSetup = (ExperimentalSetup*)theResult;
    
    return theSetup;
}


void OPF_clearExperimentalSetups()
{
    theExperimentalSetups.clearAll();
}


ExperimentalSetup::ExperimentalSetup(int tag,
    ExperimentalControl *control)
    : TaggedObject(tag), theControl(control),
    sizeTrial(0), sizeOut(0), sizeCtrl(0), sizeDaq(0),
    tDisp(0), tVel(0), tAccel(0), tForce(0), tTime(0),
    oDisp(0), oVel(0), oAccel(0), oForce(0), oTime(0),
    cDisp(0), cVel(0), cAccel(0), cForce(0), cTime(0),
    dDisp(0), dVel(0), dAccel(0), dForce(0), dTime(0),
    tDispFact(0), tVelFact(0), tAccelFact(0), tForceFact(0), tTimeFact(0),
    oDispFact(0), oVelFact(0), oAccelFact(0), oForceFact(0), oTimeFact(0),
    cDispFact(0), cVelFact(0), cAccelFact(0), cForceFact(0), cTimeFact(0),
    dDispFact(0), dVelFact(0), dAccelFact(0), dForceFact(0), dTimeFact(0)
{
    sizeTrial = new ID(OF_Resp_All);
    sizeOut = new ID(OF_Resp_All);
    if (sizeTrial == 0 || sizeOut == 0)  {
        opserr << "ExperimentalSetup::ExperimentalSetup() - "
            << "failed to create ID.\n";
        exit(OF_ReturnType_failed);
    }

    sizeCtrl = new ID(OF_Resp_All);
    sizeDaq = new ID(OF_Resp_All);
    if (sizeCtrl == 0 || sizeDaq == 0)  {
        opserr << "ExperimentalSetup::ExperimentalSetup() - "
            << "failed to create ID.\n";
        exit(OF_ReturnType_failed);
    }
}


ExperimentalSetup::ExperimentalSetup(const ExperimentalSetup& es)
    : TaggedObject(es), theControl(0),
    sizeTrial(0), sizeOut(0), sizeCtrl(0), sizeDaq(0),
    tDisp(0), tVel(0), tAccel(0), tForce(0), tTime(0),
    oDisp(0), oVel(0), oAccel(0), oForce(0), oTime(0),
    cDisp(0), cVel(0), cAccel(0), cForce(0), cTime(0),
    dDisp(0), dVel(0), dAccel(0), dForce(0), dTime(0),
    tDispFact(0), tVelFact(0), tAccelFact(0), tForceFact(0), tTimeFact(0),
    oDispFact(0), oVelFact(0), oAccelFact(0), oForceFact(0), oTimeFact(0),
    cDispFact(0), cVelFact(0), cAccelFact(0), cForceFact(0), cTimeFact(0),
    dDispFact(0), dVelFact(0), dAccelFact(0), dForceFact(0), dTimeFact(0)
{
    if (es.theControl != 0)  {
        theControl = (es.theControl)->getCopy();
        if (theControl == 0)  {
            opserr << "ExperimentalSetup::ExperimentalSetup() - "
                << "failed to get a copy of control system.\n";
            exit(OF_ReturnType_failed);
        }
        theControl->setup();
    }
    
    sizeTrial = new ID(OF_Resp_All);
    sizeOut = new ID(OF_Resp_All);
    if (sizeTrial == 0 || sizeOut == 0)  {
        opserr << "ExperimentalSetup::ExperimentalSetup() - "
            << "failed to create ID.\n";
        exit(OF_ReturnType_failed);
    }
    *sizeTrial = *(es.sizeTrial);
    *sizeOut = *(es.sizeOut);
    
    this->setTrial();
    this->setOut();
    
    if (es.tDispFact != 0)
        *tDispFact = *(es.tDispFact);
    if (es.tVelFact != 0)
        *tVelFact = *(es.tVelFact);
    if (es.tAccelFact != 0)
        *tAccelFact = *(es.tAccelFact);
    if (es.tForceFact != 0)
        *tForceFact = *(es.tForceFact);
    if (es.tTimeFact != 0)
        *tTimeFact = *(es.tTimeFact);
    if (es.oDispFact != 0)
        *oDispFact = *(es.oDispFact);
    if (es.oVelFact != 0)
        *oVelFact = *(es.oVelFact);
    if (es.oAccelFact != 0)
        *oAccelFact = *(es.oAccelFact);
    if (es.oForceFact != 0)
        *oForceFact = *(es.oForceFact);
    if (es.oTimeFact != 0)
        *oTimeFact = *(es.oTimeFact);

    sizeCtrl = new ID(OF_Resp_All);
    sizeDaq = new ID(OF_Resp_All);
    if (sizeCtrl == 0 || sizeDaq == 0)  {
        opserr << "ExperimentalSetup::ExperimentalSetup() - "
            << "failed to create ID.\n";
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


ExperimentalSetup::~ExperimentalSetup()
{
    // theControl is not a copy, so do not clean it up here
    //if (theControl != 0)
    //    delete theControl;
    
    if (tDisp != 0) 
        delete tDisp;
    if (tVel != 0) 
        delete tVel;
    if (tAccel != 0) 
        delete tAccel;
    if (tForce != 0) 
        delete tForce;
    if (tTime != 0) 
        delete tTime;
    
    if (oDisp != 0) 
        delete oDisp;
    if (oVel != 0) 
        delete oVel;
    if (oAccel != 0) 
        delete oAccel;
    if (oForce != 0) 
        delete oForce;
    if (oTime != 0)
        delete oTime;
    
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
    
    if (tDispFact != 0) 
        delete tDispFact;
    if (tVelFact != 0) 
        delete tVelFact;
    if (tAccelFact != 0) 
        delete tAccelFact;
    if (tForceFact != 0) 
        delete tForceFact;
    if (tTimeFact != 0)
        delete tTimeFact;
    
    if (oDispFact != 0) 
        delete oDispFact;
    if (oVelFact != 0) 
        delete oVelFact;
    if (oAccelFact != 0) 
        delete oAccelFact;
    if (oForceFact != 0) 
        delete oForceFact;
    if (oTimeFact != 0)
        delete oTimeFact;
    
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
    
    if (sizeTrial != 0)
        delete sizeTrial;
    if (sizeOut != 0)
        delete sizeOut;
    if (sizeCtrl != 0)
        delete sizeCtrl;
    if (sizeDaq != 0)
        delete sizeDaq;
}


const char* ExperimentalSetup::getClassType() const
{
    return "UnknownExpSetupObject";
}


// must be called in setup() of concrete classes
int ExperimentalSetup::setTrialOutSize()
{
    this->setTrial();
    this->setOut();
    
    return OF_ReturnType_completed;
}


// must be called in setup() of concrete classes
int ExperimentalSetup::setCtrlDaqSize()
{
    this->setCtrl();
    this->setDaq();
    
    if (theControl != 0)  {
        theControl->setSize(*sizeCtrl, *sizeDaq);
        theControl->setup();
    }
    
    return OF_ReturnType_completed;
}


int ExperimentalSetup::checkSize(ID sizeT, ID sizeO)
{
    // check for correct data sizes
    for (int i=0; i<OF_Resp_All; i++)  {
        if ((sizeT(i) != 0 && sizeT(i) != (*sizeTrial)(i)) ||
            (sizeO(i) != 0 && sizeO(i) != (*sizeOut)(i)))  {
            opserr << "ExperimentalSetup::checkSize() - "
                << "wrong sizeTrial/Out from site received\n";
            opserr << "sizeTrial: site  = " << sizeT 
                   << "           setup = " << *sizeTrial;
            opserr << "sizeOut: site  = " << sizeO 
                   << "         setup = " << *sizeOut << endln;
            exit(OF_ReturnType_failed);
        }
    }
    
    // cleanup of data vectors that are no longer required
    if (sizeT(OF_Resp_Disp) == 0)  {
        (*sizeTrial)(OF_Resp_Disp) = 0;
        if (tDisp != 0)  {
            delete tDisp;
            tDisp = 0;
        }
        (*sizeCtrl)(OF_Resp_Disp) = 0;
        if (cDisp != 0)  {
            delete cDisp;
            cDisp = 0;
        }
    }
    if (sizeT(OF_Resp_Vel) == 0)  {
        (*sizeTrial)(OF_Resp_Vel) = 0;
        if (tVel != 0)  {
            delete tVel;
            tVel = 0;
        }
        (*sizeCtrl)(OF_Resp_Vel) = 0;
        if (cVel != 0)  {
            delete cVel;
            cVel = 0;
        }
    }
    if (sizeT(OF_Resp_Accel) == 0)  {
        (*sizeTrial)(OF_Resp_Accel) = 0;
        if (tAccel != 0)  {
            delete tAccel;
            tAccel = 0;
        }
        (*sizeCtrl)(OF_Resp_Accel) = 0;
        if (cAccel != 0)  {
            delete cAccel;
            cAccel = 0;
        }
    }
    if (sizeT(OF_Resp_Force) == 0)  {
        (*sizeTrial)(OF_Resp_Force) = 0;
        if (tForce != 0)  {
            delete tForce;
            tForce = 0;
        }
        (*sizeCtrl)(OF_Resp_Force) = 0;
        if (cForce != 0)  {
            delete cForce;
            cForce = 0;
        }
    }
    if (sizeT(OF_Resp_Time) == 0)  {
        (*sizeTrial)(OF_Resp_Time) = 0;
        if (tTime != 0)  {
            delete tTime;
            tTime = 0;
        }
        (*sizeCtrl)(OF_Resp_Time) = 0;
        if (cTime != 0)  {
            delete cTime;
            cTime = 0;
        }
    }
    if (sizeO(OF_Resp_Disp) == 0)  {
        (*sizeOut)(OF_Resp_Disp) = 0;
        if (oDisp != 0)  {
            delete oDisp;
            oDisp = 0;
        }
        (*sizeDaq)(OF_Resp_Disp) = 0;
        if (dDisp != 0)  {
            delete dDisp;
            dDisp = 0;
        }
    }
    if (sizeO(OF_Resp_Vel) == 0)  {
        (*sizeOut)(OF_Resp_Vel) = 0;
        if (oVel != 0)  {
            delete oVel;
            oVel = 0;
        }
        (*sizeDaq)(OF_Resp_Vel) = 0;
        if (dVel != 0)  {
            delete dVel;
            dVel = 0;
        }
    }
    if (sizeO(OF_Resp_Accel) == 0)  {
        (*sizeOut)(OF_Resp_Accel) = 0;
        if (oAccel != 0)  {
            delete oAccel;
            oAccel = 0;
        }
        (*sizeDaq)(OF_Resp_Accel) = 0;
        if (dAccel != 0)  {
            delete dAccel;
            dAccel = 0;
        }
    }
    if (sizeO(OF_Resp_Force) == 0)  {
        (*sizeOut)(OF_Resp_Force) = 0;
        if (oForce != 0)  {
            delete oForce;
            oForce = 0;
        }
        (*sizeDaq)(OF_Resp_Force) = 0;
        if (dForce != 0)  {
            delete dForce;
            dForce = 0;
        }
    }
    if (sizeO(OF_Resp_Time) == 0)  {
        (*sizeOut)(OF_Resp_Time) = 0;
        if (oTime != 0)  {
            delete oTime;
            oTime = 0;
        }
        (*sizeDaq)(OF_Resp_Time) = 0;
        if (dTime != 0)  {
            delete dTime;
            dTime = 0;
        }
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
    
    if (theControl != 0)  {
        int rValue;
        // set trial response at the control
        rValue = theControl->setTrialResponse(cDisp, cVel, cAccel, cForce, cTime);
        if (rValue != OF_ReturnType_completed)  {
            opserr << "ExperimentalSetup::setTrialResponse() - "
                << "failed to set trial response at the control.\n";
            exit(OF_ReturnType_failed);
        }
    }
    
    return OF_ReturnType_completed;
}


int ExperimentalSetup::getTrialResponse(Vector* disp, 
    Vector* vel,
    Vector* accel,
    Vector* force,
    Vector* time)
{
    if (cDisp != 0)
        *disp = *cDisp;
    if (cVel != 0)
        *vel = *cVel;
    if (cAccel != 0)
        *accel = *cAccel;
    if (cForce != 0)
        *force = *cForce;
    if (cTime != 0)
        *time = *cTime;
    
    return OF_ReturnType_completed;
}


int ExperimentalSetup::setDaqResponse(const Vector* disp, 
    const Vector* vel,
    const Vector* accel,
    const Vector* force,
    const Vector* time)
{
    if (dDisp != 0)
        *dDisp = *disp;
    if (dVel != 0)
        *dVel = *vel;
    if (dAccel != 0)
        *dAccel = *accel;
    if (dForce != 0)
        *dForce = *force;
    if (dTime != 0)
        *dTime = *time;
    
    return OF_ReturnType_completed;
}


int ExperimentalSetup::getDaqResponse(Vector* disp, 
    Vector* vel,
    Vector* accel,
    Vector* force,
    Vector* time)
{
    if (theControl != 0)  {
        int rValue;
        // get daq response from the control
        rValue = theControl->getDaqResponse(dDisp, dVel, dAccel, dForce, dTime);
        if (rValue != OF_ReturnType_completed)  {
            opserr << "ExperimentalSetup::getDaqResponse() - "
                << "failed to get daq response from the control.\n";
            exit(OF_ReturnType_failed);
        }
    }
    
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
    if (disp != 0)  {
        for (int i=0; i<(*sizeTrial)(OF_Resp_Disp); i++)
            (*tDisp)(i) = (*disp)(i) * (*tDispFact)(i);
        this->transfTrialDisp(tDisp);
        for (int i=0; i<(*sizeCtrl)(OF_Resp_Disp); i++)
            (*cDisp)(i) *= (*cDispFact)(i);
    }
    if (vel != 0)  {
        for (int i=0; i<(*sizeTrial)(OF_Resp_Vel); i++)
            (*tVel)(i) = (*vel)(i) * (*tVelFact)(i);
        this->transfTrialVel(tVel);
        for (int i=0; i<(*sizeCtrl)(OF_Resp_Vel); i++)
            (*cVel)(i) *= (*cVelFact)(i);
    }
    if (accel != 0)  {
        for (int i=0; i<(*sizeTrial)(OF_Resp_Accel); i++)
            (*tAccel)(i) = (*accel)(i) * (*tAccelFact)(i);
        this->transfTrialAccel(tAccel);
        for (int i=0; i<(*sizeCtrl)(OF_Resp_Accel); i++)
            (*cAccel)(i) *= (*cAccelFact)(i);
    }
    if (force != 0)  {
        for (int i=0; i<(*sizeTrial)(OF_Resp_Force); i++)
            (*tForce)(i) = (*force)(i) * (*tForceFact)(i);
        this->transfTrialForce(tForce);
        for (int i=0; i<(*sizeCtrl)(OF_Resp_Force); i++)
            (*cForce)(i) *= (*cForceFact)(i);
    }
    if (time != 0)  {
        for (int i=0; i<(*sizeTrial)(OF_Resp_Time); i++)
            (*tTime)(i) = (*time)(i) * (*tTimeFact)(i);
        this->transfTrialTime(tTime);
        for (int i=0; i<(*sizeCtrl)(OF_Resp_Time); i++)
            (*cTime)(i) *= (*cTimeFact)(i);
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
    if (disp != 0)  {
        for (int i=0; i<(*sizeDaq)(OF_Resp_Disp); i++)
            (*dDisp)(i) *= (*dDispFact)(i);
        this->transfDaqDisp(oDisp);
        for (int i=0; i<(*sizeOut)(OF_Resp_Disp); i++)
            (*disp)(i) = (*oDisp)(i) * (*oDispFact)(i);
    }
    if (vel != 0)  {
        for (int i=0; i<(*sizeDaq)(OF_Resp_Vel); i++)
            (*dVel)(i) *= (*dVelFact)(i);
        this->transfDaqVel(oVel);
        for (int i=0; i<(*sizeOut)(OF_Resp_Vel); i++)
            (*vel)(i) = (*oVel)(i) * (*oVelFact)(i);
    }
    if (accel != 0)  {
        for (int i=0; i<(*sizeDaq)(OF_Resp_Accel); i++)
            (*dAccel)(i) *= (*dAccelFact)(i);
        this->transfDaqAccel(oAccel);
        for (int i=0; i<(*sizeOut)(OF_Resp_Accel); i++)
            (*accel)(i) = (*oAccel)(i) * (*oAccelFact)(i);
    }
    if (force != 0)  {
        for (int i=0; i<(*sizeDaq)(OF_Resp_Force); i++)
            (*dForce)(i) *= (*dForceFact)(i);
        this->transfDaqForce(oForce);
        for (int i=0; i<(*sizeOut)(OF_Resp_Force); i++)
            (*force)(i) = (*oForce)(i) * (*oForceFact)(i);
    }
    if (time != 0)  {
        for (int i=0; i<(*sizeDaq)(OF_Resp_Time); i++)
            (*dTime)(i) *= (*dTimeFact)(i);
        this->transfDaqTime(oTime);
        for (int i=0; i<(*sizeOut)(OF_Resp_Time); i++)
            (*time)(i) = (*oTime)(i) * (*oTimeFact)(i);
    }
    
    return OF_ReturnType_completed;
}


int ExperimentalSetup::commitState()
{
    if (theControl != 0)  {
        int rValue;
        rValue = theControl->commitState();
        if (rValue != OF_ReturnType_completed)  {
            opserr << "ExperimentalSetup::commitState() - "
                << "failed to commit state for the control.\n";
            exit(OF_ReturnType_failed);
        }
    }
    
    return OF_ReturnType_completed;
}


Response* ExperimentalSetup::setResponse(const char **argv, int argc,
    OPS_Stream &output)
{
    int i;
    char outputData[15];
    Response *theResponse = 0;
    
    output.tag("ExpSetupOutput");
    output.attr("setupType",this->getClassType());
    output.attr("setupTag",this->getTag());
    
    // trial displacements
    if (tDisp != 0 && (
        strcmp(argv[0],"trialDisp") == 0 ||
        strcmp(argv[0],"trialDisplacement") == 0 ||
        strcmp(argv[0],"trialDisplacements") == 0))
    {
        for (i=0; i<(*sizeTrial)(OF_Resp_Disp); i++)  {
            sprintf(outputData,"trialDisp%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpSetupResponse(this, 1, *tDisp);
    }
    
    // trial velocities
    else if (tVel != 0 && (
        strcmp(argv[0],"trialVel") == 0 ||
        strcmp(argv[0],"trialVelocity") == 0 ||
        strcmp(argv[0],"trialVelocities") == 0))
    {
        for (i=0; i<(*sizeTrial)(OF_Resp_Vel); i++)  {
            sprintf(outputData,"trialVel%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpSetupResponse(this, 2, *tVel);
    }
    
    // trial accelerations
    else if (tAccel != 0 && (
        strcmp(argv[0],"trialAccel") == 0 ||
        strcmp(argv[0],"trialAcceleration") == 0 ||
        strcmp(argv[0],"trialAccelerations") == 0))
    {
        for (i=0; i<(*sizeTrial)(OF_Resp_Accel); i++)  {
            sprintf(outputData,"trialAccel%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpSetupResponse(this, 3, *tAccel);
    }
    
    // trial forces
    else if (tForce != 0 && (
        strcmp(argv[0],"trialForce") == 0 ||
        strcmp(argv[0],"trialForces") == 0))
    {
        for (i=0; i<(*sizeTrial)(OF_Resp_Force); i++)  {
            sprintf(outputData,"trialForce%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpSetupResponse(this, 4, *tForce);
    }
    
    // trial times
    else if (tTime != 0 && (
        strcmp(argv[0],"trialTime") == 0 ||
        strcmp(argv[0],"trialTimes") == 0))
    {
        for (i=0; i<(*sizeTrial)(OF_Resp_Time); i++)  {
            sprintf(outputData,"trialTime%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpSetupResponse(this, 5, *tTime);
    }
    
    // output displacements
    else if (oDisp != 0 && (
        strcmp(argv[0],"outDisp") == 0 ||
        strcmp(argv[0],"outDisplacement") == 0 ||
        strcmp(argv[0],"outDisplacements") == 0))
    {
        for (i=0; i<(*sizeOut)(OF_Resp_Disp); i++)  {
            sprintf(outputData,"outDisp%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpSetupResponse(this, 6, *oDisp);
    }
    
    // output velocities
    else if (oVel != 0 && (
        strcmp(argv[0],"outVel") == 0 ||
        strcmp(argv[0],"outVelocity") == 0 ||
        strcmp(argv[0],"outVelocities") == 0))
    {
        for (i=0; i<(*sizeOut)(OF_Resp_Vel); i++)  {
            sprintf(outputData,"outVel%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpSetupResponse(this, 7, *oVel);
    }
    
    // output accelerations
    else if (oAccel != 0 && (
        strcmp(argv[0],"outAccel") == 0 ||
        strcmp(argv[0],"outAcceleration") == 0 ||
        strcmp(argv[0],"outAccelerations") == 0))
    {
        for (i=0; i<(*sizeOut)(OF_Resp_Accel); i++)  {
            sprintf(outputData,"outAccel%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpSetupResponse(this, 8, *oAccel);
    }
    
    // output forces
    else if (oForce != 0 && (
        strcmp(argv[0],"outForce") == 0 ||
        strcmp(argv[0],"outForces") == 0))
    {
        for (i=0; i<(*sizeOut)(OF_Resp_Force); i++)  {
            sprintf(outputData,"outForce%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpSetupResponse(this, 9, *oForce);
    }
    
    // output times
    else if (oTime != 0 && (
        strcmp(argv[0],"outTime") == 0 ||
        strcmp(argv[0],"outTimes") == 0))
    {
        for (i=0; i<(*sizeOut)(OF_Resp_Time); i++)  {
            sprintf(outputData,"outTime%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpSetupResponse(this, 10, *oTime);
    }
    
    // control displacements
    else if (cDisp != 0 && (
        strcmp(argv[0],"ctrlDisp") == 0 ||
        strcmp(argv[0],"ctrlDisplacement") == 0 ||
        strcmp(argv[0],"ctrlDisplacements") == 0))
    {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Disp); i++)  {
            sprintf(outputData,"ctrlDisp%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpSetupResponse(this, 11, *cDisp);
    }
    
    // control velocities
    else if (cVel != 0 && (
        strcmp(argv[0],"ctrlVel") == 0 ||
        strcmp(argv[0],"ctrlVelocity") == 0 ||
        strcmp(argv[0],"ctrlVelocities") == 0))
    {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Vel); i++)  {
            sprintf(outputData,"ctrlVel%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpSetupResponse(this, 12, *cVel);
    }
    
    // control accelerations
    else if (cAccel != 0 && (
        strcmp(argv[0],"ctrlAccel") == 0 ||
        strcmp(argv[0],"ctrlAcceleration") == 0 ||
        strcmp(argv[0],"ctrlAccelerations") == 0))
    {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Accel); i++)  {
            sprintf(outputData,"ctrlAccel%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpSetupResponse(this, 13, *cAccel);
    }
    
    // control forces
    else if (cForce != 0 && (
        strcmp(argv[0],"ctrlForce") == 0 ||
        strcmp(argv[0],"ctrlForces") == 0))
    {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Force); i++)  {
            sprintf(outputData,"ctrlForce%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpSetupResponse(this, 14, *cForce);
    }
    
    // control times
    else if (cTime != 0 && (
        strcmp(argv[0],"ctrlTime") == 0 ||
        strcmp(argv[0],"ctrlTimes") == 0))
    {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Time); i++)  {
            sprintf(outputData,"ctrlTime%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpSetupResponse(this, 15, *cTime);
    }
    
    // daq displacements
    else if (dDisp != 0 && (
        strcmp(argv[0],"daqDisp") == 0 ||
        strcmp(argv[0],"daqDisplacement") == 0 ||
        strcmp(argv[0],"daqDisplacements") == 0))
    {
        for (i=0; i<(*sizeDaq)(OF_Resp_Disp); i++)  {
            sprintf(outputData,"daqDisp%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpSetupResponse(this, 16, *dDisp);
    }
    
    // daq velocities
    else if (dVel != 0 && (
        strcmp(argv[0],"daqVel") == 0 ||
        strcmp(argv[0],"daqVelocity") == 0 ||
        strcmp(argv[0],"daqVelocities") == 0))
    {
        for (i=0; i<(*sizeDaq)(OF_Resp_Vel); i++)  {
            sprintf(outputData,"daqVel%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpSetupResponse(this, 17, *dVel);
    }
    
    // daq accelerations
    else if (dAccel != 0 && (
        strcmp(argv[0],"daqAccel") == 0 ||
        strcmp(argv[0],"daqAcceleration") == 0 ||
        strcmp(argv[0],"daqAccelerations") == 0))
    {
        for (i=0; i<(*sizeDaq)(OF_Resp_Accel); i++)  {
            sprintf(outputData,"daqAccel%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpSetupResponse(this, 18, *dAccel);
    }
    
    // daq forces
    else if (dForce != 0 && (
        strcmp(argv[0],"daqForce") == 0 ||
        strcmp(argv[0],"daqForces") == 0))
    {
        for (i=0; i<(*sizeDaq)(OF_Resp_Force); i++)  {
            sprintf(outputData,"daqForce%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpSetupResponse(this, 19, *dForce);
    }
    
    // daq times
    else if (dTime != 0 && (
        strcmp(argv[0],"daqTime") == 0 ||
        strcmp(argv[0],"daqTimes") == 0))
    {
        for (i=0; i<(*sizeDaq)(OF_Resp_Time); i++)  {
            sprintf(outputData,"daqTime%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpSetupResponse(this, 20, *dTime);
    }
    
    output.endTag();
    
    return theResponse;
}


int ExperimentalSetup::getResponse(int responseID, Information &info)
{
    switch (responseID)  {
    case 1:  // trial displacements
        if (tDisp != 0)
            return info.setVector(*tDisp);
        else
            return -1;
        
    case 2:  // trial velocities
        if (tVel != 0)
            return info.setVector(*tVel);
        else
            return -1;
        
    case 3:  // trial accelerations
        if (tAccel != 0)
            return info.setVector(*tAccel);
        else
            return -1;
        
    case 4:  // trial forces
        if (tForce != 0)
            return info.setVector(*tForce);
        else
            return -1;
        
    case 5:  // trial times
        if (tTime != 0)
            return info.setVector(*tTime);
        else
            return -1;
        
    case 6:  // output displacements
        if (oDisp != 0)
            return info.setVector(*oDisp);
        else
            return -1;
        
    case 7:  // output velocities
        if (oVel != 0)
            return info.setVector(*oVel);
        else
            return -1;
        
    case 8:  // output accelerations
        if (oAccel != 0)
            return info.setVector(*oAccel);
        else
            return -1;
        
    case 9:  // output forces
        if (oForce != 0)
            return info.setVector(*oForce);
        else
            return -1;
        
    case 10:  // output times
        if (oTime != 0)
            return info.setVector(*oTime);
        else
            return -1;
        
    case 11:  // control displacements
        if (cDisp != 0)
            return info.setVector(*cDisp);
        else
            return -1;
        
    case 12:  // control velocities
        if (cVel != 0)
            return info.setVector(*cVel);
        else
            return -1;
        
    case 13:  // control accelerations
        if (cAccel != 0)
            return info.setVector(*cAccel);
        else
            return -1;
        
    case 14:  // control forces
        if (cForce != 0)
            return info.setVector(*cForce);
        else
            return -1;
        
    case 15:  // control times
        if (cTime != 0)
            return info.setVector(*cTime);
        else
            return -1;
        
    case 16:  // daq displacements
        if (dDisp != 0)
            return info.setVector(*dDisp);
        else
            return -1;
        
    case 17:  // daq velocities
        if (dVel != 0)
            return info.setVector(*dVel);
        else
            return -1;
        
    case 18:  // daq accelerations
        if (dAccel != 0)
            return info.setVector(*dAccel);
        else
            return -1;
        
    case 19:  // daq forces
        if (dForce != 0)
            return info.setVector(*dForce);
        else
            return -1;
        
    case 20:  // daq times
        if (dTime != 0)
            return info.setVector(*dTime);
        else
            return -1;
        
    default:
        return -1;
    }
}


void ExperimentalSetup::setTrialDispFactor(const Vector& f)
{
    if (f.Size() != getTrialSize(OF_Resp_Disp))  {
        opserr << "ExperimentalSetup::setTrialDispFactor() - "
            << "invalid size.\n";
        exit(OF_ReturnType_failed);
    }
    *tDispFact = f;
}


void ExperimentalSetup::setTrialVelFactor(const Vector& f)
{
    if (f.Size() != getTrialSize(OF_Resp_Vel))  {
        opserr << "ExperimentalSetup::setTrialVelFactor() - "
            << "invalid size.\n";
        exit(OF_ReturnType_failed);
    }
    *tVelFact = f;
}


void ExperimentalSetup::setTrialAccelFactor(const Vector& f)
{
    if (f.Size() != getTrialSize(OF_Resp_Accel))  {
        opserr << "ExperimentalSetup::setTrialAccelFactor() - "
            << "invalid size.\n";
        exit(OF_ReturnType_failed);
    }
    *tAccelFact = f;
}


void ExperimentalSetup::setTrialForceFactor(const Vector& f)
{
    if (f.Size() != getTrialSize(OF_Resp_Force))  {
        opserr << "ExperimentalSetup::setTrialForceFactor() - "
            << "invalid size.\n";
        exit(OF_ReturnType_failed);
    }
    *tForceFact = f;
}


void ExperimentalSetup::setTrialTimeFactor(const Vector& f)
{
    if (f.Size() != getTrialSize(OF_Resp_Time))  {
        opserr << "ExperimentalSetup::setTrialTimeFactor() - "
            << "invalid size.\n";
        exit(OF_ReturnType_failed);
    }
    *tTimeFact = f;
}


void ExperimentalSetup::setOutDispFactor(const Vector& f)
{
    if (f.Size() != getOutSize(OF_Resp_Disp))  {
        opserr << "ExperimentalSetup::setOutDispFactor() - "
            << "invalid size.\n";
        exit(OF_ReturnType_failed);
    }
    *oDispFact = f;
}


void ExperimentalSetup::setOutVelFactor(const Vector& f)
{
    if (f.Size() != getOutSize(OF_Resp_Vel))  {
        opserr << "ExperimentalSetup::setOutVelFactor() - "
            << "invalid size.\n";
        exit(OF_ReturnType_failed);
    }
    *oVelFact = f;
}


void ExperimentalSetup::setOutAccelFactor(const Vector& f)
{
    if (f.Size() != getOutSize(OF_Resp_Accel))  {
        opserr << "ExperimentalSetup::setOutAccelFactor() - "
            << "invalid size.\n";
        exit(OF_ReturnType_failed);
    }
    *oAccelFact = f;
}


void ExperimentalSetup::setOutForceFactor(const Vector& f)
{
    if (f.Size() != getOutSize(OF_Resp_Force))  {
        opserr << "ExperimentalSetup::setOutForceFactor() - "
            << "invalid size.\n";
        exit(OF_ReturnType_failed);
    }
    *oForceFact = f;
}


void ExperimentalSetup::setOutTimeFactor(const Vector& f)
{
    if (f.Size() != getOutSize(OF_Resp_Time))  {
        opserr << "ExperimentalSetup::setOutTimeFactor() - "
            << "invalid size.\n";
        exit(OF_ReturnType_failed);
    }
    *oTimeFact = f;
}


void ExperimentalSetup::setCtrlDispFactor(const Vector& f)
{
    if (f.Size() != getCtrlSize(OF_Resp_Disp))  {
        opserr << "ExperimentalSetup::setCtrlDispFactor() - "
            << "invalid size.\n";
        exit(OF_ReturnType_failed);
    }
    *cDispFact = f;
}


void ExperimentalSetup::setCtrlVelFactor(const Vector& f)
{
    if (f.Size() != getCtrlSize(OF_Resp_Vel))  {
        opserr << "ExperimentalSetup::setCtrlVelFactor() - "
            << "invalid size.\n";
        exit(OF_ReturnType_failed);
    }
    *cVelFact = f;
}


void ExperimentalSetup::setCtrlAccelFactor(const Vector& f)
{
    if (f.Size() != getCtrlSize(OF_Resp_Accel))  {
        opserr << "ExperimentalSetup::setCtrlAccelFactor() - "
            << "invalid size.\n";
        exit(OF_ReturnType_failed);
    }
    *cAccelFact = f;
}


void ExperimentalSetup::setCtrlForceFactor(const Vector& f)
{
    if (f.Size() != getCtrlSize(OF_Resp_Force))  {
        opserr << "ExperimentalSetup::setCtrlForceFactor() - "
            << "invalid size.\n";
        exit(OF_ReturnType_failed);
    }
    *cForceFact = f;
}


void ExperimentalSetup::setCtrlTimeFactor(const Vector& f)
{
    if (f.Size() != getCtrlSize(OF_Resp_Time))  {
        opserr << "ExperimentalSetup::setCtrlTimeFactor() - "
            << "invalid size.\n";
        exit(OF_ReturnType_failed);
    }
    *cTimeFact = f;
}


void ExperimentalSetup::setDaqDispFactor(const Vector& f)
{
    if (f.Size() != getDaqSize(OF_Resp_Disp))  {
        opserr << "ExperimentalSetup::setDaqDispFactor() - "
            << "invalid size.\n";
        exit(OF_ReturnType_failed);
    }
    *dDispFact = f;
}


void ExperimentalSetup::setDaqVelFactor(const Vector& f)
{
    if (f.Size() != getDaqSize(OF_Resp_Vel))  {
        opserr << "ExperimentalSetup::setDaqVelFactor() - "
            << "invalid size.\n";
        exit(OF_ReturnType_failed);
    }
    *dVelFact = f;
}


void ExperimentalSetup::setDaqAccelFactor(const Vector& f)
{
    if (f.Size() != getDaqSize(OF_Resp_Accel))  {
        opserr << "ExperimentalSetup::setDaqAccelFactor() - "
            << "invalid size.\n";
        exit(OF_ReturnType_failed);
    }
    *dAccelFact = f;
}


void ExperimentalSetup::setDaqForceFactor(const Vector& f)
{
    if (f.Size() != getDaqSize(OF_Resp_Force))  {
        opserr << "ExperimentalSetup::setDaqForceFactor() - "
            << "invalid size.\n";
        exit(OF_ReturnType_failed);
    }
    *dForceFact = f;
}


void ExperimentalSetup::setDaqTimeFactor(const Vector& f)
{
    if (f.Size() != getDaqSize(OF_Resp_Time))  {
        opserr << "ExperimentalSetup::setDaqTimeFactor() - "
            << "invalid size.\n";
        exit(OF_ReturnType_failed);
    }
    *dTimeFact = f;
}


ID ExperimentalSetup::getTrialSize()
{
    return *sizeTrial;
}


ID ExperimentalSetup::getOutSize()
{
    return *sizeOut;
}


ID ExperimentalSetup::getCtrlSize()
{
    return *sizeCtrl;
}


ID ExperimentalSetup::getDaqSize()
{
    return *sizeDaq;
}


int ExperimentalSetup::getTrialSize(int rType)
{
    return (*sizeTrial)(rType);
}


int ExperimentalSetup::getOutSize(int rType)
{
    return (*sizeOut)(rType);
}


int ExperimentalSetup::getCtrlSize(int rType)
{
    return (*sizeCtrl)(rType);
}


int ExperimentalSetup::getDaqSize(int rType)
{
    return (*sizeDaq)(rType);
}


void ExperimentalSetup::setTrial()
{
    if (tDisp != 0)  {
        delete tDisp;
        tDisp = 0;
    }
    if (tVel != 0)  {
        delete tVel;
        tVel = 0;
    }
    if (tAccel != 0)  {
        delete tAccel;
        tAccel = 0;
    }
    if (tForce != 0)  {
        delete tForce;
        tForce = 0;
    }
    if (tTime != 0)  {
        delete tTime;
        tTime = 0;
    }
    if (tDispFact != 0)  {
        delete tDispFact;
        tDispFact = 0;
    }
    if (tVelFact != 0)  {
        delete tVelFact;
        tVelFact = 0;
    }
    if (tAccelFact != 0)  {
        delete tAccelFact;
        tAccelFact = 0;
    }
    if (tForceFact != 0)  {
        delete tForceFact;
        tForceFact = 0;
    }
    if (tTimeFact != 0)  {
        delete tTimeFact;
        tTimeFact = 0;
    }
    
    int size, i;
    size = (*sizeTrial)(OF_Resp_Disp);
    if (size != 0)  {
        tDisp = new Vector(size);
        if (tDispFact == 0)  {
            tDispFact = new Vector(size);
            for (i=0; i<size; i++)
                (*tDispFact)(i) = 1.0;
        }
        if (tDisp == 0 || tDispFact == 0)  {
            opserr << "ExperimentalSetup::setTrial() - "
                << "failed to create Vector.\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeTrial)(OF_Resp_Vel);
    if (size != 0)  {
        tVel = new Vector(size);
        if (tVelFact == 0)  {
            tVelFact = new Vector(size);
            for (i=0; i<size; i++)
                (*tVelFact)(i) = 1.0;
        }
        if (tVel == 0 || tVelFact == 0)  {
            opserr << "ExperimentalSetup::setTrial() - "
                << "failed to create Vector.\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeTrial)(OF_Resp_Accel);
    if (size != 0)  {
        tAccel = new Vector(size);
        if (tAccelFact == 0)  {
            tAccelFact = new Vector(size);
            for (i=0; i<size; i++)
                (*tAccelFact)(i) = 1.0;
        }
        if (tAccel == 0 || tAccelFact == 0)  {
            opserr << "ExperimentalSetup::setTrial() - "
                << "failed to create Vector.\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeTrial)(OF_Resp_Force);
    if (size != 0) {
        tForce = new Vector(size);
        if (tForceFact == 0) {
            tForceFact = new Vector(size);
            for (i=0; i<size; i++)
                (*tForceFact)(i) = 1.0;
        }
        if (tForce == 0 || tForceFact == 0)  {
            opserr << "ExperimentalSetup::setTrial() - "
                << "failed to create Vector.\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeTrial)(OF_Resp_Time);
    if (size != 0)  {
        tTime = new Vector(size);
        if (tTimeFact == 0)  {
            tTimeFact = new Vector(size);
            for (i=0; i<size; i++)
                (*tTimeFact)(i) = 1.0;
        }
        if (tTime == 0 || tTimeFact == 0)  {
            opserr << "ExperimentalSetup::setTrial() - "
                << "failed to create Vector.\n";
            exit(OF_ReturnType_failed);
        }
    }
}


void ExperimentalSetup::setOut()
{
    if (oDisp != 0)  {
        delete oDisp;
        oDisp = 0;
    }
    if (oVel != 0)  {
        delete oVel;
        oVel = 0;
    }
    if (oAccel != 0)  {
        delete oAccel;
        oAccel = 0;
    }
    if (oForce != 0)  {
        delete oForce;
        oForce = 0;
    }
    if (oTime != 0)  {
        delete oTime;
        oTime = 0;
    }
    if (oDispFact != 0)  {
        delete oDispFact;
        oDispFact = 0;
    }
    if (oVelFact != 0)  {
        delete oVelFact;
        oVelFact = 0;
    }
    if (oAccelFact != 0)  {
        delete oAccelFact;
        oAccelFact = 0;
    }
    if (oForceFact != 0)  {
        delete oForceFact;
        oForceFact = 0;
    }
    if (oTimeFact != 0)  {
        delete oTimeFact;
        oTimeFact = 0;
    }
    
    int size, i;
    size = (*sizeOut)(OF_Resp_Disp);
    if (size != 0)  {
        oDisp = new Vector(size);
        if (oDispFact == 0)  {
            oDispFact = new Vector(size);
            for (i=0; i<size; i++)
                (*oDispFact)(i) = 1.0;
        }
        if (oDisp == 0 || oDispFact == 0)  {
            opserr << "ExperimentalSetup::setOut() - "
                << "failed to create Vector.\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeOut)(OF_Resp_Vel);
    if (size != 0)  {
        oVel = new Vector(size);
        if (oVelFact == 0)  {
            oVelFact = new Vector(size);
            for (i=0; i<size; i++)
                (*oVelFact)(i) = 1.0;
        }
        if (oVel == 0 || oVelFact == 0)  {
            opserr << "ExperimentalSetup::setOut() - "
                << "failed to create Vector.\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeOut)(OF_Resp_Accel);
    if (size != 0)  {
        oAccel = new Vector(size);
        if (oAccelFact == 0)  {
            oAccelFact = new Vector(size);
            for (i=0; i<size; i++)
                (*oAccelFact)(i) = 1.0;
        }
        if (oAccel == 0 || oAccelFact == 0)  {
            opserr << "ExperimentalSetup::setOut() - "
                << "failed to create Vector.\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeOut)(OF_Resp_Force);
    if (size != 0)  {
        oForce = new Vector(size);
        if (oForceFact == 0)  {
            oForceFact = new Vector(size);
            for (i=0; i<size; i++)
                (*oForceFact)(i) = 1.0;
        }
        if (oForce == 0 || oForceFact == 0)  {
            opserr << "ExperimentalSetup::setOut() - "
                << "failed to create Vector.\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeOut)(OF_Resp_Time);
    if (size != 0)  {
        oTime = new Vector(size);
        if (oTimeFact == 0)  {
            oTimeFact = new Vector(size);
            for (i=0; i<size; i++)
                (*oTimeFact)(i) = 1.0;
        }
        if (oTime == 0 || oTimeFact == 0)  {
            opserr << "ExperimentalSetup::setOut() - "
                << "failed to create Vector.\n";
            exit(OF_ReturnType_failed);
        }
    }
}


void ExperimentalSetup::setCtrl()
{
    if (cDisp != 0)  {
        delete cDisp;
        cDisp = 0;
    }
    if (cVel != 0)  {
        delete cVel;
        cVel = 0;
    }
    if (cAccel != 0)  {
        delete cAccel;
        cAccel = 0;
    }
    if (cForce != 0)  {
        delete cForce;
        cForce = 0;
    }
    if (cTime != 0)  {
        delete cTime;
        cTime = 0;
    }
    if (cDispFact != 0)  {
        delete cDispFact;
        cDispFact = 0;
    }
    if (cVelFact != 0)  {
        delete cVelFact;
        cVelFact = 0;
    }
    if (cAccelFact != 0)  {
        delete cAccelFact;
        cAccelFact = 0;
    }
    if (cForceFact != 0)  {
        delete cForceFact;
        cForceFact = 0;
    }
    if (cTimeFact != 0)  {
        delete cTimeFact;
        cTimeFact = 0;
    }
    
    int size, i;
    size = (*sizeCtrl)(OF_Resp_Disp);
    if (size != 0)  {
        cDisp = new Vector(size);
        if (cDispFact == 0)  {
            cDispFact = new Vector(size);
            for (i=0; i<size; i++)
                (*cDispFact)(i) = 1.0;
        }
        if (cDisp == 0 || cDispFact == 0)  {
            opserr << "ExperimentalSetup::setCtrl() - "
                << "failed to create Vector.\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeCtrl)(OF_Resp_Vel);
    if (size != 0)  {
        cVel = new Vector(size);
        if (cVelFact == 0)  {
            cVelFact = new Vector(size);
            for (i=0; i<size; i++)
                (*cVelFact)(i) = 1.0;
        }
        if (cVel == 0 || cVelFact == 0)  {
            opserr << "ExperimentalSetup::setCtrl() - "
                << "failed to create Vector.\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeCtrl)(OF_Resp_Accel);
    if (size != 0)  {
        cAccel = new Vector(size);
        if (cAccelFact == 0)  {
            cAccelFact = new Vector(size);
            for (i=0; i<size; i++)
                (*cAccelFact)(i) = 1.0;
        }
        if (cAccel == 0 || cAccelFact == 0)  {
            opserr << "ExperimentalSetup::setCtrl() - "
                << "failed to create Vector.\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeCtrl)(OF_Resp_Force);
    if (size != 0) {
        cForce = new Vector(size);
        if (cForceFact == 0) {
            cForceFact = new Vector(size);
            for (i=0; i<size; i++)
                (*cForceFact)(i) = 1.0;
        }
        if (cForce == 0 || cForceFact == 0)  {
            opserr << "ExperimentalSetup::setCtrl() - "
                << "failed to create Vector.\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeCtrl)(OF_Resp_Time);
    if (size != 0)  {
        cTime = new Vector(size);
        if (cTimeFact == 0)  {
            cTimeFact = new Vector(size);
            for (i=0; i<size; i++)
                (*cTimeFact)(i) = 1.0;
        }
        if (cTime == 0 || cTimeFact == 0)  {
            opserr << "ExperimentalSetup::setCtrl() - "
                << "failed to create Vector.\n";
            exit(OF_ReturnType_failed);
        }
    }
}


void ExperimentalSetup::setDaq()
{
    if (dDisp != 0)  {
        delete dDisp;
        dDisp = 0;
    }
    if (dVel != 0)  {
        delete dVel;
        dVel = 0;
    }
    if (dAccel != 0)  {
        delete dAccel;
        dAccel = 0;
    }
    if (dForce != 0)  {
        delete dForce;
        dForce = 0;
    }
    if (dTime != 0)  {
        delete dTime;
        dTime = 0;
    }
    if (dDispFact != 0)  {
        delete dDispFact;
        dDispFact = 0;
    }
    if (dVelFact != 0)  {
        delete dVelFact;
        dVelFact = 0;
    }
    if (dAccelFact != 0)  {
        delete dAccelFact;
        dAccelFact = 0;
    }
    if (dForceFact != 0)  {
        delete dForceFact;
        dForceFact = 0;
    }
    if (dTimeFact != 0)  {
        delete dTimeFact;
        dTimeFact = 0;
    }
    
    int size, i;
    size = (*sizeDaq)(OF_Resp_Disp);
    if (size != 0)  {
        dDisp = new Vector(size);
        if (dDispFact == 0)  {
            dDispFact = new Vector(size);
            for (i=0; i<size; i++)
                (*dDispFact)(i) = 1.0;
        }
        if (dDisp == 0 || dDispFact == 0)  {
            opserr << "ExperimentalSetup::setDaq() - "
                << "failed to create Vector.\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeDaq)(OF_Resp_Vel);
    if (size != 0)  {
        dVel = new Vector(size);
        if (dVelFact == 0)  {
            dVelFact = new Vector(size);
            for (i=0; i<size; i++)
                (*dVelFact)(i) = 1.0;
        }
        if (dVel == 0 || dVelFact == 0)  {
            opserr << "ExperimentalSetup::setDaq() - "
                << "failed to create Vector.\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeDaq)(OF_Resp_Accel);
    if (size != 0)  {
        dAccel = new Vector(size);
        if (dAccelFact == 0)  {
            dAccelFact = new Vector(size);
            for (i=0; i<size; i++)
                (*dAccelFact)(i) = 1.0;
        }
        if (dAccel == 0 || dAccelFact == 0)  {
            opserr << "ExperimentalSetup::setDaq() - "
                << "failed to create Vector.\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeDaq)(OF_Resp_Force);
    if (size != 0)  {
        dForce = new Vector(size);
        if (dForceFact == 0)  {
            dForceFact = new Vector(size);
            for (i=0; i<size; i++)
                (*dForceFact)(i) = 1.0;
        }
        if (dForce == 0 || dForceFact == 0)  {
            opserr << "ExperimentalSetup::setDaq() - "
                << "failed to create Vector.\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeDaq)(OF_Resp_Time);
    if (size != 0)  {
        dTime = new Vector(size);
        if (dTimeFact == 0)  {
            dTimeFact = new Vector(size);
            for (i=0; i<size; i++)
                (*dTimeFact)(i) = 1.0;
        }
        if (dTime == 0 || dTimeFact == 0)  {
            opserr << "ExperimentalSetup::setDaq() - "
                << "failed to create Vector.\n";
            exit(OF_ReturnType_failed);
        }
    }
}
