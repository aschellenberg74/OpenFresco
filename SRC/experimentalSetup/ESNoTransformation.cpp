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
// Description: This file contains the implementation of the
// ESNoTransformation class.

#include "ESNoTransformation.h"

#include <ExperimentalControl.h>

#include <elementAPI.h>


void* OPF_ESNoTransformation()
{
    // pointer to experimental setup that will be returned
    ExperimentalSetup* theSetup = 0;
    
    if (OPS_GetNumRemainingInputArgs() < 6) {
        opserr << "WARNING invalid number of arguments\n";
        opserr << "Want: expSetup NoTransformation tag <-control ctrlTag> "
            << "-dof DOFs -sizeTrialOut t o\n";
        return 0;
    }
    
    // setup tag
    int tag;
    int numdata = 1;
    if (OPS_GetIntInput(&numdata, &tag) != 0) {
        opserr << "WARNING invalid expSetup NoTransformation tag\n";
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
            opserr << "expSetup NoTransformation " << tag << endln;
            return 0;
        }
        theControl = OPF_getExperimentalControl(ctrlTag);
        if (theControl == 0) {
            opserr << "WARNING experimental control not found\n";
            opserr << "expControl: " << ctrlTag << endln;
            opserr << "expSetup NoTransformation " << tag << endln;
            return 0;
        }
    }
    else {
        // move current arg back by one
        OPS_ResetCurrentInputArg(-1);
    }
    
    // dof IDs
    type = OPS_GetString();
    if (strcmp(type, "-dof") != 0 && strcmp(type, "-dir") != 0) {
        opserr << "WARNING expecting -dof DOFs\n";
        opserr << "expSetup NoTransformation " << tag << endln;
        return 0;
    }
    ID theDOF(32);
    int numDOF = 0;
    while (OPS_GetNumRemainingInputArgs() > 0) {
        int dof;
        numdata = 1;
        int numArgs = OPS_GetNumRemainingInputArgs();
        if (OPS_GetIntInput(&numdata, &dof) < 0) {
            if (numArgs > OPS_GetNumRemainingInputArgs()) {
                // move current arg back by one
                OPS_ResetCurrentInputArg(-1);
            }
            break;
        }
        theDOF(numDOF++) = dof-1;
    }
    if (numDOF == 0) {
        opserr << "WARNING no DOFs specified\n";
        opserr << "expSetup NoTransformation " << tag << endln;
        return 0;
    }
    theDOF.resize(numDOF);
    
    // size trial and size out
    int sizeTrial, sizeOut;
    type = OPS_GetString();
    if (strcmp(type, "-sizeTrialOut") != 0) {
        opserr << "WARNING expecting -sizeTrialOut t o\n";
        opserr << "expSetup NoTransformation " << tag << endln;
        return 0;
    }
    numdata = 1;
    if (OPS_GetIntInput(&numdata, &sizeTrial) != 0) {
        opserr << "WARNING invalid sizeTrial\n";
        opserr << "expSetup NoTransformation " << tag << endln;
        return 0;
    }
    numdata = 1;
    if (OPS_GetIntInput(&numdata, &sizeOut) != 0) {
        opserr << "WARNING invalid sizeOut\n";
        opserr << "expSetup NoTransformation " << tag << endln;
        return 0;
    }
    
    // parsing was successful, allocate the setup
    theSetup = new ESNoTransformation(tag, theDOF, sizeTrial, sizeOut, theControl);
    if (theSetup == 0) {
        opserr << "WARNING could not create experimental setup of type ESNoTransformation\n";
        return 0;
    }
    
    return theSetup;
}


ESNoTransformation::ESNoTransformation(int tag,
    const ID &dof, int sizet, int sizeo,
    ExperimentalControl* control)
    : ExperimentalSetup(tag, control),
    DOF(0), sizeT(sizet), sizeO(sizeo)
{
    // allocate memory for DOF array
    numDOF = dof.Size();
    DOF = new ID(numDOF);
    if (DOF == 0)  {
        opserr << "ESNoTransformation::ESNoTransformation()"
            << " - failed to create DOF array\n";
        exit(OF_ReturnType_failed);
    }
    
    // initialize DOFs and check for valid values
    (*DOF) = dof;
    for (int i=0; i<numDOF; i++)  {
        if ((*DOF)(i) < 0 || (*DOF)(i) >= sizeT ||
            (*DOF)(i) >= sizeO)  {
            opserr << "ESNoTransformation::ESNoTransformation()"
                << " - DOF ID out of bound:"
                << (*DOF)(i) << endln;
            exit(OF_ReturnType_failed);
        }
    }
    
    // call setup method
    this->setup();
}


ESNoTransformation::ESNoTransformation(const ESNoTransformation& es)
    : ExperimentalSetup(es),
    DOF(0)
{
    numDOF = es.numDOF;
    DOF = new ID(numDOF);
    if (DOF == 0)  {
        opserr << "ESNoTransformation::ESNoTransformation()"
            << " - failed to create DOF array\n";
        exit(OF_ReturnType_failed);
    }
    *DOF = *(es.DOF);
    sizeT = es.sizeT;
    sizeO = es.sizeO;
    
    // call setup method
    this->setup();
}


ESNoTransformation::~ESNoTransformation()
{
    // invoke the destructor on any objects created by the object
    // that the object still holds a pointer to
    if (DOF != 0 )
        delete DOF;
}


int ESNoTransformation::setup()
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
        (*sizeCtrl)(i) = numDOF;
        (*sizeDaq)(i) = numDOF;
    }
    (*sizeCtrl)(OF_Resp_Time) = 1;
    (*sizeDaq)(OF_Resp_Time) = 1;
    
    this->setCtrlDaqSize();
    
    return OF_ReturnType_completed;
}


ExperimentalSetup* ESNoTransformation::getCopy()
{
    ESNoTransformation *theCopy = new ESNoTransformation(*this);
    
    return theCopy;
}


void ESNoTransformation::Print(OPS_Stream &s, int flag)
{
    s << "ExperimentalSetup: " << this->getTag(); 
    s << " type: ESNoTransformation\n";
    s << " dof: " << (*DOF) << endln;
    if (theControl != 0)  {
        s << "\tExperimentalControl tag: " << theControl->getTag();
        s << *theControl;
    }
}


int ESNoTransformation::transfTrialDisp(const Vector* disp)
{
    for (int i=0; i<numDOF; i++)
        (*cDisp)(i) = (*disp)((*DOF)(i));
    
    return OF_ReturnType_completed;
}


int ESNoTransformation::transfTrialVel(const Vector* vel)
{
    for (int i=0; i<numDOF; i++)
        (*cVel)(i) = (*vel)((*DOF)(i));
    
    return OF_ReturnType_completed;
}


int ESNoTransformation::transfTrialAccel(const Vector* accel)
{
    for (int i=0; i<numDOF; i++)
        (*cAccel)(i) = (*accel)((*DOF)(i));
    
    return OF_ReturnType_completed;
}


int ESNoTransformation::transfTrialForce(const Vector* force)
{
    for (int i=0; i<numDOF; i++)
        (*cForce)(i) = (*force)((*DOF)(i));
    
    return OF_ReturnType_completed;
}


int ESNoTransformation::transfTrialTime(const Vector* time)
{
    *cTime = *time;
    
    return OF_ReturnType_completed;
}


int ESNoTransformation::transfDaqDisp(Vector* disp)
{
    for (int i=0; i<numDOF; i++)
        (*disp)((*DOF)(i)) = (*dDisp)(i);
    
    return OF_ReturnType_completed;
}


int ESNoTransformation::transfDaqVel(Vector* vel)
{
    for (int i=0; i<numDOF; i++)
        (*vel)((*DOF)(i)) = (*dVel)(i);
    
    return OF_ReturnType_completed;
}


int ESNoTransformation::transfDaqAccel(Vector* accel)
{
    for (int i=0; i<numDOF; i++)
        (*accel)((*DOF)(i)) = (*dAccel)(i);
    
    return OF_ReturnType_completed;
}


int ESNoTransformation::transfDaqForce(Vector* force)
{
    for (int i=0; i<numDOF; i++)
        (*force)((*DOF)(i)) = (*dForce)(i);
    
    return OF_ReturnType_completed;
}


int ESNoTransformation::transfDaqTime(Vector* time)
{
    *time = *dTime;
    
    return OF_ReturnType_completed;
}
