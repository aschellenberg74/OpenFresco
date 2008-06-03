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
// Description: This file contains the implementation of  
// ExperimentalSite.

#include "ExperimentalSite.h"


ExperimentalSite::ExperimentalSite(int tag, 
    ExperimentalSetup *setup)
    : TaggedObject(tag),
    theSetup(setup),
    tDisp(0), tVel(0), tAccel(0), tForce(0), tTime(0),
    Disp(0), Vel(0), Accel(0), Force(0), Time(0),
    sizeTrial(0), sizeOut(0),
    daqFlag(false)
{
    sizeTrial = new ID(OF_Resp_All);
    sizeOut = new ID(OF_Resp_All);
    if (sizeTrial == 0 || sizeOut == 0) {
        opserr << "ExperimentalSite::ExperimentalSite() - "
            << "fail to create ID"
            << endln;
        exit(OF_ReturnType_failed);
    }
}


ExperimentalSite::ExperimentalSite(const ExperimentalSite& site)
    : TaggedObject(site.getTag()),
    theSetup(0),
    tDisp(0), tVel(0), tAccel(0), tForce(0), tTime(0),
    Disp(0), Vel(0), Accel(0), Force(0), Time(0),
    sizeTrial(0), sizeOut(0),
    daqFlag(false)
{
    if (site.theSetup != 0) {
        theSetup = (site.theSetup)->getCopy();
        if (theSetup == 0) {
            opserr << "ExperimentalSite::ExperimentalSite() - "
                << "failed to get a copy of setup"
                << endln;
            exit(OF_ReturnType_failed);
        }
    }
    
    sizeTrial = new ID(OF_Resp_All);
    sizeOut = new ID(OF_Resp_All);
    if (sizeTrial == 0 || sizeOut == 0) {
        opserr << "ExperimentalSite::ExperimentalSite() - "
            << "failed to create IDs" << endln;
        exit(OF_ReturnType_failed);
    }
    *sizeTrial = *(site.sizeTrial);
    *sizeOut = *(site.sizeOut);

    this->setTrial();
    this->setOut();
}


ExperimentalSite::~ExperimentalSite()
{
    if (theSetup != 0) 
        delete theSetup;
    
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
    
    if (Disp != 0) 
        delete Disp;
    if (Vel != 0) 
        delete Vel;
    if (Accel != 0) 
        delete Accel;
    if (Force != 0) 
        delete Force;
    if (Time != 0)
        delete Time;
    
    if (sizeTrial != 0)
        delete sizeTrial;
    if (sizeOut != 0)
        delete sizeOut;
}


int ExperimentalSite::setSize(ID sizeT, ID sizeO) 
{
    *sizeTrial = sizeT;
    *sizeOut = sizeO;
    
    this->setTrial();
    this->setOut();
    
    return OF_ReturnType_completed;
}


int ExperimentalSite::setTrialResponse(const Vector* disp, 
    const Vector* vel,
    const Vector* accel,
    const Vector* force,
    const Vector* time)
{
    if (tDisp != 0) {
        *tDisp = *disp;
    }
    if (tVel != 0) {
        *tVel = *vel;
    }
    if (tAccel != 0) {
        *tAccel = *accel;
    }
    if (tForce != 0) {
        *tForce = *force;
    } 
    if (tTime != 0) {
        *tTime = *time;
    } 
    
    return OF_ReturnType_completed;
}


int ExperimentalSite::setTrialDisp(const Vector* disp)
{
    return setTrialResponse(disp, (Vector*)0, (Vector*)0, (Vector*)0, (Vector*)0);
}


int ExperimentalSite::getDaqResponse(Vector* disp,
    Vector* vel,
    Vector* accel,
    Vector* force,
    Vector* time)
{
    this->checkDaqResponse();
    
    if (Disp != 0) {
        *disp = *Disp;
    }
    if (Vel != 0) {
        *vel = *Vel;
    }
    if (Accel != 0) {
        *accel = *Accel;
    }
    if (Force != 0) {
        *force = *Force;
    }
    if (Time != 0) {
        *time = *Time;
    }
    
    return OF_ReturnType_completed;
}


int ExperimentalSite::setDaqResponse(const Vector* disp,
    const Vector* vel,
    const Vector* accel,
    const Vector* force,
    const Vector* time)
{
    if (Disp != 0) {
        *Disp = *disp;
    }
    if (Vel != 0) {
        *Vel = *vel;
    }
    if (Accel != 0) {
        *Accel = *accel;
    }
    if (Force != 0) {
        *Force = *force;
    }
    if (Time != 0) {
        *Time = *time;
    }
    
    return OF_ReturnType_completed;
}


const Vector& ExperimentalSite::getTrialDisp()
{
    return *tDisp;
}


const Vector& ExperimentalSite::getTrialVel()
{
    return *tVel;
}


const Vector& ExperimentalSite::getTrialAccel()
{
    return *tAccel;
}


const Vector& ExperimentalSite::getTrialForce()
{
    return *tForce;
}


const Vector& ExperimentalSite::getTrialTime()
{
    return *tTime;
}


const Vector& ExperimentalSite::getDisp()
{
    this->checkDaqResponse();
    
    return *Disp;
}


const Vector& ExperimentalSite::getVel()
{
    this->checkDaqResponse();
    
    return *Vel;
}


const Vector& ExperimentalSite::getAccel()
{
    this->checkDaqResponse();
    
    return *Accel;
}


const Vector& ExperimentalSite::getForce()
{
    this->checkDaqResponse();
    
    return *Force;
}


const Vector& ExperimentalSite::getTime()
{
    this->checkDaqResponse();
    
    return *Time;
}


int ExperimentalSite::commitState()
{
    return theSetup->commitState();
}


int ExperimentalSite::getTrialSize(int rType)
{
    return (*sizeTrial)[rType];
}


int ExperimentalSite::getOutSize(int rType)
{
    return (*sizeOut)[rType];
}


int ExperimentalSite::getCtrlSize(int rType)
{
    if (theSetup == 0) {
        return (*sizeTrial)[rType];
    } else {
        return theSetup->getCtrlSize(rType);
    }
}


int ExperimentalSite::getDaqSize(int rType)
{
    if (theSetup == 0) {
        return (*sizeOut)[rType];
    } else {
        return theSetup->getDaqSize(rType);
    }
}


void ExperimentalSite::setTrial()
{
    if (tDisp != 0) {
        delete tDisp;
        tDisp = 0;
    }
    if (tVel != 0) {
        delete tVel;
        tVel = 0;
    }
    if (tAccel != 0) {
        delete tAccel;
        tAccel = 0;
    }
    if (tForce != 0) {
        delete tForce;
        tForce = 0;
    }
    if (tTime != 0) {
        delete tTime;
        tTime = 0;
    }
    
    int size;
    size = (*sizeTrial)[OF_Resp_Disp];
    if (size != 0) {
        tDisp = new Vector(size);
        if (tDisp == 0) {
            opserr << "ExperimentalSite::setTrial() - "
                << "failed to create tDisp Vector\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeTrial)[OF_Resp_Vel];
    if (size != 0) {
        tVel = new Vector(size);
        if (tVel == 0) {
            opserr << "ExperimentalSite::setTrial() - "
                << "failed to create tVel Vector\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeTrial)[OF_Resp_Accel];
    if (size != 0) {
        tAccel = new Vector(size);
        if (tAccel == 0) {
            opserr << "ExperimentalSite::setTrial() - "
                << "failed to create tAccel Vector\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeTrial)[OF_Resp_Force];
    if (size != 0) {
        tForce = new Vector(size);
        if (tForce == 0) {
            opserr << "ExperimentalSite::setTrial() - "
                << "failed to create tForce Vector\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeTrial)[OF_Resp_Time];
    if (size != 0) {
        tTime = new Vector(size);
        if (tTime == 0) {
            opserr << "ExperimentalSite::setTrial() - "
                << "failed to create tTime Vector\n";
            exit(OF_ReturnType_failed);
        }
    }
}


void ExperimentalSite::setOut()
{
    if (Disp != 0) {
        delete Disp;
        Disp = 0;
    }
    if (Vel != 0) {
        delete Vel;
        Vel = 0;
    }
    if (Accel != 0) {
        delete Accel;
        Accel = 0;
    }
    if (Force != 0) {
        delete Force;
        Force = 0;
    }
    if (Time != 0) {
        delete Time;
        Time = 0;
    }
    
    int size;
    size = (*sizeOut)[OF_Resp_Disp];
    if (size != 0) {
        Disp = new Vector(size);
        if (Disp == 0) {
            opserr << "ExperimentalSite::setOut() - "
                << "failed to create Disp Vector\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeOut)[OF_Resp_Vel];
    if (size != 0) {
        Vel = new Vector(size);
        if (Vel == 0) {
            opserr << "ExperimentalSite::setOut() - "
                << "failed to create Vel Vector\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeOut)[OF_Resp_Accel];
    if (size != 0) {
        Accel = new Vector(size);
        if (Accel == 0) {
            opserr << "ExperimentalSite::setOut() - "
                << "failed to create Accel Vector\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeOut)[OF_Resp_Force];
    if (size != 0) {
        Force = new Vector(size);
        if (Force == 0) {
            opserr << "ExperimentalSite::setOut() - "
                << "failed to create Force Vector\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeOut)[OF_Resp_Time];
    if (size != 0) {
        Time = new Vector(size);
        if (Time == 0) {
            opserr << "ExperimentalSite::setOut() - "
                << "failed to create Time Vector\n";
            exit(OF_ReturnType_failed);
        }
    }
}


/*void ExperimentalSite::setTrialCPs(ArrayOfTaggedObjects &theCPs)
{
    int nTrial = theCPs.getNumComponents();
    if (cpsTrial != 0)
        delete cpsTrial;
    if (nTrial == 0) {
        opserr << "ExperimentalSite::setTrialCPs() - "
            << "invalid size of cpsTrial";
    }
    cpsTrial = new ArrayOfTaggedObjects(nTrial*OF_Resp_All);
    
    if (sizeTrial != 0)
        delete [] sizeTrial;
    sizeTrial = new ID[OF_Resp_All];
    sizeTrial->Zero();
    
    for (int i=1; i<=nTrial; i++) {
        TaggedObject *mc = theCPs.getComponentPtr(i);
        if (mc == 0) {
            opserr << "ExperimentalSite::setTrialCPs() - "
                << "failed to get ExperimentalCP\n";
            exit(OF_ReturnType_failed);
        }
        ExperimentalCP *theCP = (ExperimentalCP*)mc;
        cpsTrial->addComponent(theCP->getCopy());
        
        int respType = theCP->getResponseType();
        switch(respType) {
        case OF_Resp_Disp:
            (*sizeTrial)[OF_Resp_Disp]++;
            break;
        case OF_Resp_Vel:
            (*sizeTrial)[OF_Resp_Vel]++;
            break;
        case OF_Resp_Accel:
            (*sizeTrial)[OF_Resp_Accel]++;
            break;
        case OF_Resp_Force:
            (*sizeTrial)[OF_Resp_Force]++;
            break;
        case OF_Resp_Time:
            (*sizeTrial)[OF_Resp_Time]++;
            break;
        case OF_Resp_All:
            for (int i=0; i<OF_Resp_All; i++)
                (*sizeTrial)[i]++;
            break;
        }
    }
    
    this->setTrial();
}


void ExperimentalSite::setOutCPs(ArrayOfTaggedObjects &theCPs)
{
    int nOutput = theCPs.getNumComponents();
    if (cpsOut != 0)
        delete cpsOut;
    if (nOutput == 0) {
        opserr << "ExperimentalSite::setOutCPs() - "
            << "invalid size of cpsOut";
    }
    cpsOut = new ArrayOfTaggedObjects(nOutput*OF_Resp_All);
    
    if (sizeOut != 0)
        delete [] sizeOut;
    sizeOut = new ID(OF_Resp_All);
    sizeOut->Zero();
    
    for (int i=1; i<=nOutput; i++) {
        TaggedObject *mc = theCPs.getComponentPtr(i);
        if (mc == 0) {
            opserr << "ExperimentalSite::setOutCPs() - "
                << "failed to get ExperimentalCP\n";
            exit(OF_ReturnType_failed);
        }
        ExperimentalCP *theCP = (ExperimentalCP*)mc;
        cpsOut->addComponent(theCP->getCopy());
        
        int respType = theCP->getResponseType();
        switch(respType) {
        case OF_Resp_Disp:
            (*sizeOut)[OF_Resp_Disp]++;
            break;
        case OF_Resp_Vel:
            (*sizeOut)[OF_Resp_Vel]++;
            break;
        case OF_Resp_Accel:
            (*sizeOut)[OF_Resp_Accel]++;
            break;
        case OF_Resp_Force:
            (*sizeOut)[OF_Resp_Force]++;
            break;
        case OF_Resp_Time:
            (*sizeOut)[OF_Resp_Time]++;
            break;
        case OF_Resp_All:
            for (int i=0; i<OF_Resp_All; i++)
                (*sizeOut)[i]++;
            break;
        }
    }
    
    this->setOut();
}*/
