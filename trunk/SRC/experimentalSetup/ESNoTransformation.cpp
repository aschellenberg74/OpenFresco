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
	const ID &dir,
	ExperimentalControl* control)
	: ExperimentalSetup(tag, control),
	numDir(dir.Size()), direction(0)
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
        if ((*direction)(i) < 0 || (*direction)(i) > 5)  {
            opserr << "ESNoTransformation::ESNoTransformation()"
                << " - incorrect direction " << (*direction)(i) << " is set to 0\n";
            (*direction)(i) = 0;
        }
    }

    // call setup method
    this->setup();
}


ESNoTransformation::ESNoTransformation(const ESNoTransformation& es)
	: ExperimentalSetup(es)
{
    numDir = es.numDir;

    direction = new ID(numDir);
    if (!direction)  {
        opserr << "ESNoTransformation::ESNoTransformation()"
            << " - failed to creat direction array\n";
        exit(OF_ReturnType_failed);
    }

    direction = es.direction;

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


int ESNoTransformation::setSize(ID sizeT, ID sizeO)
{
    // check sizeTrial and sizeOut
    // for ESNoTransformation object
    
    // a component of sizeT/sizeO must be greater than 
    // the components of "direction" if it is non-zero.
    for (int i=0; i<OF_Resp_Time; i++) {
        for (int j=0; j<numDir; j++) {
            if ((sizeT[i] != 0 && sizeT[i] <= (*direction)(j)) ||
                (sizeO[i] != 0 && sizeO[i] <= (*direction)(j))) {
                opserr << "ESNoTransformation::setSize - wrong sizeTrial/Out\n"; 
                opserr << "see User Manual.\n";
                opserr << "sizeT = " << sizeT;
                opserr << "sizeO = " << sizeO;
                return OF_ReturnType_failed;
            }
        }
    }
    if ((sizeT[OF_Resp_Time] != 0 && sizeT[OF_Resp_Time] != 1) ||
        (sizeO[OF_Resp_Time] != 0 && sizeO[OF_Resp_Time] != 1)) {
        opserr << "ESNoTransformation::setSize - wrong sizeTrial/Out\n"; 
        opserr << "see User Manual.\n";
        opserr << "sizeT = " << sizeT;
        opserr << "sizeO = " << sizeO;
        return OF_ReturnType_failed;
    }

	return OF_ReturnType_completed;
}


int ESNoTransformation::commitState()
{
	return theControl->commitState();
}


int ESNoTransformation::setup()
{
    // setup for ctrl/daq vectors of ESNoTransformation
    sizeCtrl->Zero();
    sizeDaq->Zero();
    for (int i=0; i<OF_Resp_Time; i++) {
        (*sizeCtrl)[i] = numDir;
        (*sizeDaq)[i] = numDir;
    }
    (*sizeCtrl)[OF_Resp_Time] = 1;
    (*sizeDaq)[OF_Resp_Time] = 1;
    
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
    s << " dir: " << (*direction) << endln;
	if (theControl != 0) {
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
