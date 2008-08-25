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

// $Revision$
// $Date$
// $URL: $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 09/06
// Revision: A
//
// Description: This file contains the implementation of ESNoTransformation.

#include "ESNoTransformation.h"


ESNoTransformation::ESNoTransformation(int tag,
	const ID &dir, int sizet, int sizeo,
	ExperimentalControl* control)
	: ExperimentalSetup(tag, control),
	numDir(dir.Size()), direction(0), sizeT(sizet), sizeO(sizeo)
{
    // allocate memory for direction array
    direction = new ID(numDir);
    if (!direction)  {
        opserr << "ESNoTransformation::ESNoTransformation()"
            << " - failed to creat direction array\n";
        exit(OF_ReturnType_failed);
    }
    
    // initialize directions and check for valid values
    (*direction) = dir;
    for (int i=0; i<numDir; i++)  {
        if ((*direction)(i) < 0 || (*direction)(i) >= sizeT ||
            (*direction)(i) >= sizeO)  {
            opserr << "ESNoTransformation::ESNoTransformation()"
                << " - direction ID out of bound:"
                << (*direction)(i) << endln;
            exit(OF_ReturnType_failed);
        }
    }

    // call setup method
    this->setup();
}


ESNoTransformation::ESNoTransformation(const ESNoTransformation& es)
	: ExperimentalSetup(es),
    direction(0)
{
    numDir = es.numDir;

    direction = new ID(numDir);
    if (!direction)  {
        opserr << "ESNoTransformation::ESNoTransformation()"
            << " - failed to creat direction array\n";
        exit(OF_ReturnType_failed);
    }

    direction = es.direction;
    sizeT     = es.sizeT;
    sizeO     = es.sizeO;

    // call setup method
    this->setup();
}


ESNoTransformation::~ESNoTransformation()
{
    // invoke the destructor on any objects created by the object
    // that the object still holds a pointer to
    if (direction != 0 )
        delete direction;
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
        (*sizeCtrl)(i) = numDir;
        (*sizeDaq)(i) = numDir;
    }
    (*sizeCtrl)(OF_Resp_Time) = 1;
    (*sizeDaq)(OF_Resp_Time) = 1;
    
    this->setCtrlDaqSize();

	return OF_ReturnType_completed;
}


int ESNoTransformation::commitState()
{
	return theControl->commitState();
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
    s << " dir: " << (*direction) << endln;
	if (theControl != 0)  {
		s << "\tExperimentalControl tag: " << theControl->getTag();
		s << *theControl;
    }
}


int ESNoTransformation::transfTrialDisp(const Vector* disp)
{  
    for (int i=0; i<numDir; i++)
        (*cDisp)(i) = (*disp)((*direction)(i));
    
	return OF_ReturnType_completed;
}


int ESNoTransformation::transfTrialVel(const Vector* vel)
{  
    for (int i=0; i<numDir; i++)
        (*cVel)(i) = (*vel)((*direction)(i));

	return OF_ReturnType_completed;
}


int ESNoTransformation::transfTrialAccel(const Vector* accel)
{  
    for (int i=0; i<numDir; i++)
        (*cAccel)(i) = (*accel)((*direction)(i));

	return OF_ReturnType_completed;
}


int ESNoTransformation::transfTrialForce(const Vector* force)
{  
    for (int i=0; i<numDir; i++)
        (*cForce)(i) = (*force)((*direction)(i));

	return OF_ReturnType_completed;
}


int ESNoTransformation::transfTrialTime(const Vector* time)
{
	*cTime = *time;

	return OF_ReturnType_completed;
}


int ESNoTransformation::transfDaqDisp(Vector* disp)
{  
    for (int i=0; i<numDir; i++)
	    (*disp)((*direction)(i)) = (*dDisp)(i);

	return OF_ReturnType_completed;
}


int ESNoTransformation::transfDaqVel(Vector* vel)
{  
    for (int i=0; i<numDir; i++)
	    (*vel)((*direction)(i)) = (*dVel)(i);

	return OF_ReturnType_completed;
}


int ESNoTransformation::transfDaqAccel(Vector* accel)
{  
    for (int i=0; i<numDir; i++)
	    (*accel)((*direction)(i)) = (*dAccel)(i);

	return OF_ReturnType_completed;
}


int ESNoTransformation::transfDaqForce(Vector* force)
{  
    for (int i=0; i<numDir; i++)
	    (*force)((*direction)(i)) = (*dForce)(i);

	return OF_ReturnType_completed;
}


int ESNoTransformation::transfDaqTime(Vector* time)
{  
	*time = *dTime;

	return OF_ReturnType_completed;
}
