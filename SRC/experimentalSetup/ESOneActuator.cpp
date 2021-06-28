/* ****************************************************************** **
**    OpenFRESCO - Open Framework                                     **
**                 for Experimental Setup and Control                 **
**                                                                    **
**                                                                    **
** Copyright (c) 2006, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited. See    **
** file 'COPYRIGHT_UCB' in main directory for information on usage    **
** and redistribution, and for a DISCLAIMER OF ALL WARRANTIES.        **
**                                                                    **
** Developed by:                                                      **
**   Andreas Schellenberg (andreas.schellenberg@gmx.net)              **
**   Yoshikazu Takahashi (yos@catfish.dpri.kyoto-u.ac.jp)             **
**   Gregory L. Fenves (fenves@berkeley.edu)                          **
**   Stephen A. Mahin (mahin@berkeley.edu)                            **
**                                                                    **
** ****************************************************************** */

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 09/06
// Revision: A
//
// Description: This file contains the implementation of ESOneActuator.

#include "ESOneActuator.h"

#include <ExperimentalControl.h>

#include <elementAPI.h>


void* OPF_ESOneActuator()
{
    // pointer to experimental setup that will be returned
    ExperimentalSetup* theSetup = 0;
    
    if (OPS_GetNumRemainingInputArgs() < 5) {
        opserr << "WARNING invalid number of arguments\n";
        opserr << "Want: expSetup OneActuator tag <-control ctrlTag> "
            << "DOF -sizeTrialOut t o\n";
        return 0;
    }
    
    // setup tag
    int tag;
    int numdata = 1;
    if (OPS_GetIntInput(&numdata, &tag) != 0) {
        opserr << "WARNING invalid expSetup OneActuator tag\n";
        return 0;
    }
    
    // control tag (optional)
    ExperimentalControl* theControl = 0;
    const char* type = OPS_GetString();
    if (strcmp(type, "-control") == 0 || strcmp(type, "-ctrl") == 0) {
        int ctrlTag;
        numdata = 1;
        if (OPS_GetIntInput(&numdata, &ctrlTag) < 0) {
            opserr << "WARNING invalid ctrlTag\n";
            opserr << "expSetup OneActuator " << tag << endln;
            return 0;
        }
        theControl = OPF_getExperimentalControl(ctrlTag);
        if (theControl == 0) {
            opserr << "WARNING experimental control not found\n";
            opserr << "expControl: " << ctrlTag << endln;
            opserr << "expSetup OneActuator " << tag << endln;
            return 0;
        }
    }
    else {
        // move current arg back by one
        OPS_ResetCurrentInputArg(-1);
    }
    
    // dof
    int dof;
    numdata = 1;
    if (OPS_GetIntInput(&numdata, &dof) != 0) {
        opserr << "WARNING invalid DOF\n";
        opserr << "expSetup OneActuator " << tag << endln;
        return 0;
    }
    dof--;
    
    // size trial and size out
    int sizeTrial, sizeOut;
    type = OPS_GetString();
    if (strcmp(type, "-sizeTrialOut") != 0) {
        opserr << "WARNING expecting -sizeTrialOut t o\n";
        opserr << "expSetup OneActuator " << tag << endln;
        return 0;
    }
    numdata = 1;
    if (OPS_GetIntInput(&numdata, &sizeTrial) != 0) {
        opserr << "WARNING invalid sizeTrial\n";
        opserr << "expSetup OneActuator " << tag << endln;
        return 0;
    }
    numdata = 1;
    if (OPS_GetIntInput(&numdata, &sizeOut) != 0) {
        opserr << "WARNING invalid sizeOut\n";
        opserr << "expSetup OneActuator " << tag << endln;
        return 0;
    }
    
    // parsing was successful, allocate the setup
    theSetup = new ESOneActuator(tag, dof, sizeTrial, sizeOut, theControl);
    if (theSetup == 0) {
        opserr << "WARNING could not create experimental setup of type ESOneActuator\n";
        return 0;
    }
    
    return theSetup;
}


ESOneActuator::ESOneActuator(int tag,
    int dir, int sizet, int sizeo,
    ExperimentalControl* control)
    : ExperimentalSetup(tag, control),
    direction(dir), sizeT(sizet), sizeO(sizeo)
{
    // check if direction agrees with sizeT and sizeO
    if (dir < 0 || dir >= sizeT || dir >= sizeO)  {
        opserr << "ESOneActuator::ESOneActuator() - "
            << "direction of actuator is out of bound:"
            << dir << endln;
        exit(OF_ReturnType_failed);
    }
    
    // call setup method
    this->setup();
}


ESOneActuator::ESOneActuator(const ESOneActuator& es)
    : ExperimentalSetup(es)
{
    direction = es.direction;
    sizeT     = es.sizeT;
    sizeO     = es.sizeO;
    
    // call setup method
    this->setup();
}


ESOneActuator::~ESOneActuator()
{
    // does nothing
}


int ESOneActuator::setup()
{
    // setup the trial/out vectors
    sizeTrial->Zero();
    sizeOut->Zero();
    for (int i=0; i<OF_Resp_Time; i++)  {
        (*sizeTrial)(i) = sizeT;
        (*sizeOut)(i) = sizeO;
    }
    (*sizeTrial)(OF_Resp_Time) = 1;
    (*sizeOut)(OF_Resp_Time) = 1;
    
    this->setTrialOutSize();
    
    // setup the ctrl/daq vectors
    sizeCtrl->Zero();
    sizeDaq->Zero();
    for (int i=0; i<OF_Resp_Time; i++)  {
        (*sizeCtrl)(i) = 1;
        (*sizeDaq)(i) = 1;
    }
    (*sizeCtrl)(OF_Resp_Time) = 1;
    (*sizeDaq)(OF_Resp_Time) = 1;
    
    this->setCtrlDaqSize();
    
    return OF_ReturnType_completed;
}


ExperimentalSetup* ESOneActuator::getCopy()
{
    ESOneActuator *theCopy = new ESOneActuator(*this);
    
    return theCopy;
}


void ESOneActuator::Print(OPS_Stream &s, int flag)
{
    s << "ExperimentalSetup: " << this->getTag(); 
    s << " type: ESOneActuator\n";
    s << " dir: " << direction << endln;
    if (theControl != 0)  {
        s << "\tExperimentalControl tag: " << theControl->getTag();
        s << *theControl;
    }
}


int ESOneActuator::transfTrialDisp(const Vector* disp)
{  
    (*cDisp)(0) = (*disp)(direction);
    
    return OF_ReturnType_completed;
}


int ESOneActuator::transfTrialVel(const Vector* vel)
{  
    (*cVel)(0) = (*vel)(direction);
    
    return OF_ReturnType_completed;
}


int ESOneActuator::transfTrialAccel(const Vector* accel)
{  
    (*cAccel)(0) = (*accel)(direction);
    
    return OF_ReturnType_completed;
}


int ESOneActuator::transfTrialForce(const Vector* force)
{  
    (*cForce)(0) = (*force)(direction);
    
    return OF_ReturnType_completed;
}


int ESOneActuator::transfTrialTime(const Vector* time)
{
    *cTime = *time;
    
    return OF_ReturnType_completed;
}


int ESOneActuator::transfDaqDisp(Vector* disp)
{  
    (*disp)(direction) = (*dDisp)(0);
    
    return OF_ReturnType_completed;
}


int ESOneActuator::transfDaqVel(Vector* vel)
{  
    (*vel)(direction) = (*dVel)(0);
    
    return OF_ReturnType_completed;
}


int ESOneActuator::transfDaqAccel(Vector* accel)
{  
    (*accel)(direction) = (*dAccel)(0);
    
    return OF_ReturnType_completed;
}


int ESOneActuator::transfDaqForce(Vector* force)
{  
    (*force)(direction) = (*dForce)(0);
    
    return OF_ReturnType_completed;
}


int ESOneActuator::transfDaqTime(Vector* time)
{  
    *time = *dTime;
    
    return OF_ReturnType_completed;
}
