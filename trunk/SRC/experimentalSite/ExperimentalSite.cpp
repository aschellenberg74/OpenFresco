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

#include <Recorder.h>


ExperimentalSite::ExperimentalSite(int tag, 
    ExperimentalSetup *setup)
    : TaggedObject(tag), theSetup(setup),
    sizeTrial(0), sizeOut(0),
    tDisp(0), tVel(0), tAccel(0), tForce(0), tTime(0),
    oDisp(0), oVel(0), oAccel(0), oForce(0), oTime(0),
    daqFlag(false), commitTag(0), numRecorders(0), theRecorders(0)
{
    sizeTrial = new ID(OF_Resp_All);
    sizeOut = new ID(OF_Resp_All);
    if (sizeTrial == 0 || sizeOut == 0)  {
        opserr << "ExperimentalSite::ExperimentalSite() - "
            << "fail to create ID.\n";
        exit(OF_ReturnType_failed);
    }
}


ExperimentalSite::ExperimentalSite(const ExperimentalSite& es)
    : TaggedObject(es.getTag()), theSetup(0),
    sizeTrial(0), sizeOut(0),
    tDisp(0), tVel(0), tAccel(0), tForce(0), tTime(0),
    oDisp(0), oVel(0), oAccel(0), oForce(0), oTime(0),
    daqFlag(false), commitTag(0), numRecorders(0), theRecorders(0)
{
    if (es.theSetup != 0)  {
        theSetup = (es.theSetup)->getCopy();
        if (theSetup == 0)  {
            opserr << "ExperimentalSite::ExperimentalSite() - "
                << "failed to get a copy of setup.\n";
            exit(OF_ReturnType_failed);
        }
    }
    
    sizeTrial = new ID(OF_Resp_All);
    sizeOut = new ID(OF_Resp_All);
    if (sizeTrial == 0 || sizeOut == 0)  {
        opserr << "ExperimentalSite::ExperimentalSite() - "
            << "failed to create ID.\n";
        exit(OF_ReturnType_failed);
    }
    *sizeTrial = *(es.sizeTrial);
    *sizeOut = *(es.sizeOut);
    
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
    
    if (sizeTrial != 0)
        delete sizeTrial;
    if (sizeOut != 0)
        delete sizeOut;
    
    if (theRecorders != 0)  {
        for (int i=0; i<numRecorders; i++)
            if (theRecorders[i] != 0)
                delete theRecorders[i];
        delete [] theRecorders;
    }
}


const char* ExperimentalSite::getClassType() const
{
    return "UnknownExpSiteObject";
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
    if (tDisp != 0)
        *tDisp = *disp;
    if (tVel != 0)
        *tVel = *vel;
    if (tAccel != 0)
        *tAccel = *accel;
    if (tForce != 0)
        *tForce = *force;
    if (tTime != 0)
        *tTime = *time;
    
    return OF_ReturnType_completed;
}


int ExperimentalSite::setDaqResponse(const Vector* disp,
    const Vector* vel,
    const Vector* accel,
    const Vector* force,
    const Vector* time)
{
    if (oDisp != 0)
        *oDisp = *disp;
    if (oVel != 0)
        *oVel = *vel;
    if (oAccel != 0)
        *oAccel = *accel;
    if (oForce != 0)
        *oForce = *force;
    if (oTime != 0)
        *oTime = *time;
    
    return OF_ReturnType_completed;
}


int ExperimentalSite::getDaqResponse(Vector* disp,
    Vector* vel,
    Vector* accel,
    Vector* force,
    Vector* time)
{
    this->checkDaqResponse();
    
    if (oDisp != 0)
        *disp = *oDisp;
    if (oVel != 0)
        *vel = *oVel;
    if (oAccel != 0)
        *accel = *oAccel;
    if (oForce != 0)
        *force = *oForce;
    if (oTime != 0)
        *time = *oTime;
    
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
    
    return *oDisp;
}


const Vector& ExperimentalSite::getVel()
{
    this->checkDaqResponse();
    
    return *oVel;
}


const Vector& ExperimentalSite::getAccel()
{
    this->checkDaqResponse();
    
    return *oAccel;
}


const Vector& ExperimentalSite::getForce()
{
    this->checkDaqResponse();
    
    return *oForce;
}


const Vector& ExperimentalSite::getTime()
{
    this->checkDaqResponse();
    
    return *oTime;
}


int ExperimentalSite::commitState()
{
    int rValue = 0;
    
    // first commit the setup
    if (theSetup != 0)  {
        rValue += theSetup->commitState();
        if (rValue != OF_ReturnType_completed)  {
            opserr << "ExperimentalSite::commitState() - "
                << "failed to commit state for the setup.\n";
            exit(OF_ReturnType_failed);
        }
    }
    
    // invoke record on all recorders
    for (int i=0; i<numRecorders; i++)
        if (theRecorders[i] != 0)
            theRecorders[i]->record(commitTag, (*tTime)(0));
    
    // update the commitTag
    commitTag++;
    
    return rValue;
}


Response* ExperimentalSite::setResponse(const char **argv, int argc,
    OPS_Stream &output)
{
    int i;
    char outputData[15];
    Response *theResponse = 0;
    
    output.tag("ExpSiteOutput");
    output.attr("siteType",this->getClassType());
    output.attr("siteTag",this->getTag());
    
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
        theResponse = new ExpSiteResponse(this, 1, *tDisp);
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
        theResponse = new ExpSiteResponse(this, 2, *tVel);
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
        theResponse = new ExpSiteResponse(this, 3, *tAccel);
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
        theResponse = new ExpSiteResponse(this, 4, *tForce);
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
        theResponse = new ExpSiteResponse(this, 5, *tTime);
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
        theResponse = new ExpSiteResponse(this, 6, *oDisp);
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
        theResponse = new ExpSiteResponse(this, 7, *oVel);
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
        theResponse = new ExpSiteResponse(this, 8, *oAccel);
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
        theResponse = new ExpSiteResponse(this, 9, *oForce);
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
        theResponse = new ExpSiteResponse(this, 10, *oTime);
    }
    
    output.endTag();
    
    return theResponse;
}


int ExperimentalSite::getResponse(int responseID, Information &info)
{
    switch (responseID)  {
    case 1:  // trial displacements
        return info.setVector(*tDisp);
        
    case 2:  // trial velocities
        return info.setVector(*tVel);
        
    case 3:  // trial accelerations
        return info.setVector(*tAccel);
        
    case 4:  // trial forces
        return info.setVector(*tForce);
        
    case 5:  // trial times
        return info.setVector(*tTime);
        
    case 6:  // output displacements
        return info.setVector(*oDisp);
        
    case 7:  // output velocities
        return info.setVector(*oVel);
        
    case 8:  // output accelerations
        return info.setVector(*oAccel);
        
    case 9:  // output forces
        return info.setVector(*oForce);
        
    case 10:  // output times
        return info.setVector(*oTime);
        
    default:
        return -1;
    }
}


int ExperimentalSite::addRecorder(Recorder &theRecorder)
{
    Recorder **newRecorders = new Recorder* [numRecorders + 1]; 
    if (newRecorders == 0)  {
        opserr << "ExperimentalSite::addRecorder() - "
            << "could not add recorder ran out of memory\n";
        return -1;
    }
    
    for (int i=0; i<numRecorders; i++)
        newRecorders[i] = theRecorders[i];
    newRecorders[numRecorders] = &theRecorder;
    
    if (theRecorders != 0)
        delete [] theRecorders;
    
    theRecorders = newRecorders;
    numRecorders++;
    
    return 0;
}


int ExperimentalSite::removeRecorders()
{
    if (theRecorders != 0)  {
        for (int i=0; i<numRecorders; i++)
            if (theRecorders[i] != 0)
                delete theRecorders[i];
        delete [] theRecorders;
    }
    
    theRecorders = 0;
    numRecorders = 0;
    
    return 0;
}

int ExperimentalSite::removeRecorder(int tag)
{
    for (int i=0; i<numRecorders; i++)  {
        if (theRecorders[i] != 0)  {
            if (theRecorders[i]->getTag() == tag)  {
                delete theRecorders[i];
                theRecorders[i] = 0;
                return 0;
            }
        }    
    }
    
    return -1;
}


ID ExperimentalSite::getTrialSize()
{
    return *sizeTrial;
}


ID ExperimentalSite::getOutSize()
{
    return *sizeOut;
}


ID ExperimentalSite::getCtrlSize()
{
    if (theSetup == 0)
        return *sizeTrial;
    else
        return theSetup->getCtrlSize();
}


ID ExperimentalSite::getDaqSize()
{
    if (theSetup == 0)
        return *sizeOut;
    else
        return theSetup->getDaqSize();
}


int ExperimentalSite::getTrialSize(int rType)
{
    return (*sizeTrial)(rType);
}


int ExperimentalSite::getOutSize(int rType)
{
    return (*sizeOut)(rType);
}


int ExperimentalSite::getCtrlSize(int rType)
{
    if (theSetup == 0)
        return (*sizeTrial)(rType);
    else
        return theSetup->getCtrlSize(rType);
}


int ExperimentalSite::getDaqSize(int rType)
{
    if (theSetup == 0)
        return (*sizeOut)(rType);
    else
        return theSetup->getDaqSize(rType);
}


void ExperimentalSite::setTrial()
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
    
    int size;
    size = (*sizeTrial)(OF_Resp_Disp);
    if (size != 0)  {
        tDisp = new Vector(size);
        if (tDisp == 0)  {
            opserr << "ExperimentalSite::setTrial() - "
                << "failed to create tDisp Vector.\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeTrial)(OF_Resp_Vel);
    if (size != 0)  {
        tVel = new Vector(size);
        if (tVel == 0)  {
            opserr << "ExperimentalSite::setTrial() - "
                << "failed to create tVel Vector.\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeTrial)(OF_Resp_Accel);
    if (size != 0)  {
        tAccel = new Vector(size);
        if (tAccel == 0)  {
            opserr << "ExperimentalSite::setTrial() - "
                << "failed to create tAccel Vector.\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeTrial)(OF_Resp_Force);
    if (size != 0)  {
        tForce = new Vector(size);
        if (tForce == 0)  {
            opserr << "ExperimentalSite::setTrial() - "
                << "failed to create tForce Vector.\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeTrial)(OF_Resp_Time);
    if (size != 0)  {
        tTime = new Vector(size);
        if (tTime == 0)  {
            opserr << "ExperimentalSite::setTrial() - "
                << "failed to create tTime Vector.\n";
            exit(OF_ReturnType_failed);
        }
    }
}


void ExperimentalSite::setOut()
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
    
    int size;
    size = (*sizeOut)(OF_Resp_Disp);
    if (size != 0)  {
        oDisp = new Vector(size);
        if (oDisp == 0)  {
            opserr << "ExperimentalSite::setOut() - "
                << "failed to create Disp Vector.\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeOut)(OF_Resp_Vel);
    if (size != 0)  {
        oVel = new Vector(size);
        if (oVel == 0)  {
            opserr << "ExperimentalSite::setOut() - "
                << "failed to create Vel Vector.\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeOut)(OF_Resp_Accel);
    if (size != 0)  {
        oAccel = new Vector(size);
        if (oAccel == 0)  {
            opserr << "ExperimentalSite::setOut() - "
                << "failed to create Accel Vector.\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeOut)(OF_Resp_Force);
    if (size != 0)  {
        oForce = new Vector(size);
        if (oForce == 0)  {
            opserr << "ExperimentalSite::setOut() - "
                << "failed to create Force Vector.\n";
            exit(OF_ReturnType_failed);
        }
    }
    size = (*sizeOut)(OF_Resp_Time);
    if (size != 0)  {
        oTime = new Vector(size);
        if (oTime == 0)  {
            opserr << "ExperimentalSite::setOut() - "
                << "failed to create Time Vector.\n";
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
    sizeTrial = new ID(OF_Resp_All);
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
            (*sizeTrial)(OF_Resp_Disp)++;
            break;
        case OF_Resp_Vel:
            (*sizeTrial)(OF_Resp_Vel)++;
            break;
        case OF_Resp_Accel:
            (*sizeTrial)(OF_Resp_Accel)++;
            break;
        case OF_Resp_Force:
            (*sizeTrial)(OF_Resp_Force)++;
            break;
        case OF_Resp_Time:
            (*sizeTrial)(OF_Resp_Time)++;
            break;
        case OF_Resp_All:
            for (int i=0; i<OF_Resp_All; i++)
                (*sizeTrial)(i)++;
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
            (*sizeOut)(OF_Resp_Disp)++;
            break;
        case OF_Resp_Vel:
            (*sizeOut)(OF_Resp_Vel)++;
            break;
        case OF_Resp_Accel:
            (*sizeOut)(OF_Resp_Accel)++;
            break;
        case OF_Resp_Force:
            (*sizeOut)(OF_Resp_Force)++;
            break;
        case OF_Resp_Time:
            (*sizeOut)(OF_Resp_Time)++;
            break;
        case OF_Resp_All:
            for (int i=0; i<OF_Resp_All; i++)
                (*sizeOut)(i)++;
            break;
        }
    }
    
    this->setOut();
}*/
