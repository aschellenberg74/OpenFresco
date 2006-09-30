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
// $Source$

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 09/06
// Revision: A
//
// Description: This file contains the implementation of ESAggregator.

#include "ESAggregator.h"


ESAggregator::ESAggregator(int tag,
	int nSetups,
	ExperimentalSetup** setups,
    ExperimentalControl* control)
	: ExperimentalSetup(tag, control),
    theSetups(0), numSetups(nSetups),
    sizeTrialAll(0), sizeCtrlAll(0), sizeDaqAll(0), sizeOutAll(0),
    tDispAll(0), tVelAll(0), tAccelAll(0), tForceAll(0), tTimeAll(0),
    cDispAll(0), cVelAll(0), cAccelAll(0), cForceAll(0), cTimeAll(0),
    dDispAll(0), dVelAll(0), dAccelAll(0), dForceAll(0), dTimeAll(0),
    oDispAll(0), oVelAll(0), oAccelAll(0), oForceAll(0), oTimeAll(0)
{
    if (!setups)  {
      opserr << "ESAggregator::ESAggregator() - "
          << "null experimental setup array passed\n";
      exit(-1);
    }

    // allocate memory for the experimental setups
    theSetups = new ExperimentalSetup* [numSetups];

    if (!theSetups)  {
        opserr << "ESAggregator::ESAggregator() - "
            << "failed to allocate pointers\n";
        exit(-1);
    }

    int i;
    for (i = 0; i < numSetups; i++)  {
        if (!setups[i]) {
            opserr << "ESAggregator::ESAggregator() - "
                "null experimental setup pointer passed\n";
            exit(-1);
        }	
        theSetups[i] = setups[i]->getCopy();
        
        if (!theSetups[i]) {
            opserr << "ESAggregator::ESAggregator() - "
                << "failed to copy experimental setup\n";
            exit(-1);
        }
    }

    sizeTrialAll = new ID* [numSetups];
    sizeCtrlAll = new ID* [numSetups];
    sizeDaqAll = new ID* [numSetups];
    sizeOutAll = new ID* [numSetups];

    tDispAll = new Vector* [numSetups];
    tVelAll = new Vector* [numSetups];
    tAccelAll = new Vector* [numSetups];
    tForceAll = new Vector* [numSetups];
    tTimeAll = new Vector* [numSetups];

    cDispAll = new Vector* [numSetups];
    cVelAll = new Vector* [numSetups];
    cAccelAll = new Vector* [numSetups];
    cForceAll = new Vector* [numSetups];
    cTimeAll = new Vector* [numSetups];

    dDispAll = new Vector* [numSetups];
    dVelAll = new Vector* [numSetups];
    dAccelAll = new Vector* [numSetups];
    dForceAll = new Vector* [numSetups];
    dTimeAll = new Vector* [numSetups];

    oDispAll = new Vector* [numSetups];
    oVelAll = new Vector* [numSetups];
    oAccelAll = new Vector* [numSetups];
    oForceAll = new Vector* [numSetups];
    oTimeAll = new Vector* [numSetups];

    this->setup();
}


ESAggregator::ESAggregator(const ESAggregator& es)
	: ExperimentalSetup(es)
{
    if (theSetups != 0)
        delete theSetups;

    // allocate memory for the experimental setups
    theSetups = new ExperimentalSetup* [numSetups];
    
    if (!theSetups)  {
        opserr << "ESAggregator::ESAggregator() - "
            << "failed to allocate pointers\n";
        exit(-1);
    }

    int i;
    for (i = 0; i < numSetups; i++) {
        theSetups[i] = es.theSetups[i]->getCopy();
        
        if (!theSetups[i]) {
            opserr << "ESAggregator::ESAggregator() - "
                << "failed to copy experimental setup\n";
            exit(-1);
        }
    }
}


ESAggregator::~ESAggregator()
{
    // invoke the destructor on any objects created by the object
    // that the object still holds a pointer to
    if(theSetups != 0)
        delete theSetups;

    if(sizeTrialAll != 0)
        delete [] sizeTrialAll;
    if(sizeCtrlAll != 0)
        delete [] sizeCtrlAll;
    if(sizeDaqAll != 0)
        delete [] sizeDaqAll;
    if(sizeOutAll != 0)
        delete [] sizeOutAll;

    if(tDispAll != 0)
        delete [] tDispAll;
    if(tVelAll != 0)
        delete [] tVelAll;
    if(tAccelAll != 0)
        delete [] tAccelAll;
    if(tForceAll != 0)
        delete [] tForceAll;
    if(tTimeAll != 0)
        delete [] tTimeAll;

    if(cDispAll != 0)
        delete [] cDispAll;
    if(cVelAll != 0)
        delete [] cVelAll;
    if(cAccelAll != 0)
        delete [] cAccelAll;
    if(cForceAll != 0)
        delete [] cForceAll;
    if(cTimeAll != 0)
        delete [] cTimeAll;

    if(dDispAll != 0)
        delete [] dDispAll;
    if(dVelAll != 0)
        delete [] dVelAll;
    if(dAccelAll != 0)
        delete [] dAccelAll;
    if(dForceAll != 0)
        delete [] dForceAll;
    if(dTimeAll != 0)
        delete [] dTimeAll;

    if(oDispAll != 0)
        delete [] oDispAll;
    if(oVelAll != 0)
        delete [] oVelAll;
    if(oAccelAll != 0)
        delete [] oAccelAll;
    if(oForceAll != 0)
        delete [] oForceAll;
    if(oTimeAll != 0)
        delete [] oTimeAll;
}


int ESAggregator::setSize(ID sizeT, ID sizeO)
{
    // check sizeTrial and sizeOut
    // for ESAggregator object
    
    // a component of sizeT/sizeO must be greater than 
    // the components of "direction" if it is non-zero.
    
    /*int i;
    for (i=0; i<OF_Resp_All; i++) {
        if ((sizeT[i] != 0 && sizeT[i] != (*sizeCtrl)[i]) ||
            (sizeO[i] != 0 && sizeO[i] != (*sizeDaq)[i])) {
            opserr << "ESAggregator::setSize - wrong sizeTrial/Out\n";
            opserr << "see User Manual.\n";
            opserr << "sizeT = " << sizeT;
            opserr << "sizeO = " << sizeO;
            exit(1);
        }
    }*/

    return OF_ReturnType_completed;
}


int ESAggregator::commitState()
{
	return theControl->commitState();
}


int ESAggregator::setup()
{
    // setup for ctrl/daq vectors of ESAggregator
    sizeCtrl->Zero();
    sizeDaq->Zero();
    for (int i=0; i<numSetups; i++) {
        *sizeCtrlAll[i] = theSetups[i]->getCtrlSize();
        *sizeDaqAll[i] = theSetups[i]->getDaqSize();

        for (int j=0; j<OF_Resp_Time; j++) {
            (*sizeCtrl)[j] += (*sizeCtrlAll[i])[j];
            (*sizeDaq)[j] += (*sizeDaqAll[i])[j];
        }
    }
    (*sizeCtrl)[OF_Resp_Time] = 1;
    (*sizeDaq)[OF_Resp_Time] = 1;
    
    this->setCtrlDaqSize();

	return OF_ReturnType_completed;
}


int ESAggregator::transfTrialResponse(const Vector* disp, 
    const Vector* vel,
    const Vector* accel,
    const Vector* force,
    const Vector* time)
{
    int rValue;
    int tDispID = 0, tVelID = 0, tAccelID = 0, tForceID = 0;
    int cDispID = 0, cVelID = 0, cAccelID = 0, cForceID = 0;

    for (int i=0; i<numSetups; i++) {
        // extract control data
        if (disp != 0) {
            tDispAll[i]->Extract(*disp,tDispID);
            tDispID += (*sizeTrialAll[i])[OF_Resp_Disp];
        }
        if (vel != 0) {
            tVelAll[i]->Extract(*vel,tVelID);
            tVelID += (*sizeTrialAll[i])[OF_Resp_Vel];
        }
        if (accel != 0) {
            tAccelAll[i]->Extract(*accel,tAccelID);
            tAccelID += (*sizeTrialAll[i])[OF_Resp_Accel];
        }
        if (force != 0) {
            tForceAll[i]->Extract(*force,tForceID);
            tForceID += (*sizeTrialAll[i])[OF_Resp_Force];
        }
        if (time != 0) {
            *tTimeAll[i] = *time;
        }

        // transform control data of experimental setup
        rValue = theSetups[i]->transfTrialResponse(tDispAll[i],
            tVelAll[i], tAccelAll[i], tForceAll[i], tTimeAll[i]);
        if (rValue != OF_ReturnType_completed) {
            opserr << "Fail to set trial response to the setup.";
            exit(OF_ReturnType_failed);
        }
        // get control data from experimental setup
        rValue = theSetups[i]->getTrialResponse(cDispAll[i],
            cVelAll[i], cAccelAll[i], cForceAll[i], cTimeAll[i]);
        if (rValue != OF_ReturnType_completed) {
            opserr << "Fail to set trial response to the setup.";
            exit(OF_ReturnType_failed);
        }

        // assemble control data
        if (cDispAll[i] != 0) {
            cDisp->Assemble(*cDispAll[i], cDispID);
            cDispID += (*sizeCtrlAll[i])[OF_Resp_Disp];
        }
        if (cVelAll[i] != 0) {
            cVel->Assemble(*cVelAll[i], cVelID);
            cVelID += (*sizeCtrlAll[i])[OF_Resp_Vel];
        }
        if (cAccelAll[i] != 0) {
            cAccel->Assemble(*cAccelAll[i], cAccelID);
            cAccelID += (*sizeCtrlAll[i])[OF_Resp_Accel];
        }
        if (cForceAll[i] != 0) {
            cForce->Assemble(*cForceAll[i], cForceID);
            cForceID += (*sizeCtrlAll[i])[OF_Resp_Force];
        }
        if (cTimeAll[i] != 0) {
            *cTime = *cTimeAll[i];
        }
    }
    
    return OF_ReturnType_completed;
}


int ESAggregator::transfDaqResponse(Vector* disp,
    Vector* vel,
    Vector* accel,
    Vector* force,
    Vector* time)
{
    int rValue;
    int dDispID = 0, dVelID = 0, dAccelID = 0, dForceID = 0;
    int oDispID = 0, oVelID = 0, oAccelID = 0, oForceID = 0;

    for (int i=0; i<numSetups; i++) {
        // extract daq data
        if (dDisp != 0) {
            dDispAll[i]->Extract(*dDisp,dDispID);
            dDispID += (*sizeDaqAll[i])[OF_Resp_Disp];
        }
        if (dVel != 0) {
            dVelAll[i]->Extract(*dVel,dVelID);
            dVelID += (*sizeDaqAll[i])[OF_Resp_Vel];
        }
        if (dAccel != 0) {
            dAccelAll[i]->Extract(*dAccel,dAccelID);
            dAccelID += (*sizeDaqAll[i])[OF_Resp_Accel];
        }
        if (dForce != 0) {
            dForceAll[i]->Extract(*dForce,dForceID);
            dForceID += (*sizeDaqAll[i])[OF_Resp_Force];
        }
        if (dTime != 0) {
            *dTimeAll[i] = *dTime;
        }
        
        // set daq data in experimental setup
        rValue = theSetups[i]->setDaqResponse(dDispAll[i],
            dVelAll[i], dAccelAll[i], dForceAll[i], dTimeAll[i]);
        if (rValue != OF_ReturnType_completed) {
            opserr << "Fail to set trial response to the setup.";
            exit(OF_ReturnType_failed);
        }
        
        // transform daq data of experimental setup
        rValue = theSetups[i]->transfDaqResponse(oDispAll[i],
            oVelAll[i], oAccelAll[i], oForceAll[i], oTimeAll[i]);
        if (rValue != OF_ReturnType_completed) {
            opserr << "Fail to set trial response to the setup.";
            exit(OF_ReturnType_failed);
        }

        // assemble daq data
        if (oDispAll[i] != 0) {
            disp->Assemble(*oDispAll[i], oDispID);
            oDispID += (*sizeOutAll[i])[OF_Resp_Disp];
        }
        if (oVelAll[i] != 0) {
            vel->Assemble(*oVelAll[i], oVelID);
            oVelID += (*sizeOutAll[i])[OF_Resp_Vel];
        }
        if (oAccelAll[i] != 0) {
            accel->Assemble(*oAccelAll[i], oAccelID);
            oAccelID += (*sizeOutAll[i])[OF_Resp_Accel];
        }
        if (oForceAll[i] != 0) {
            force->Assemble(*oForceAll[i], oForceID);
            oForceID += (*sizeOutAll[i])[OF_Resp_Force];
        }
        if (oTimeAll[i] != 0) {
            *time = *oTimeAll[i];
        }
    }
    
    return OF_ReturnType_completed;
}


ExperimentalSetup* ESAggregator::getCopy()
{
	ESAggregator *theCopy = new ESAggregator(*this);

	return theCopy;
}


void ESAggregator::Print(OPS_Stream &s, int flag)
{
	s << "ExperimentalSetup: " << this->getTag(); 
	s << " type: ESAggregator\n";
	if(theControl != 0) {
		s << "\tExperimentalControl tag: " << theControl->getTag();
		s << *theControl;
    }
}


int ESAggregator::transfTrialDisp(const Vector* disp)
{      
	return OF_ReturnType_completed;
}


int ESAggregator::transfTrialVel(const Vector* vel)
{  
	return OF_ReturnType_completed;
}


int ESAggregator::transfTrialAccel(const Vector* accel)
{  
	return OF_ReturnType_completed;
}


int ESAggregator::transfTrialForce(const Vector* force)
{  
	return OF_ReturnType_completed;
}


int ESAggregator::transfTrialTime(const Vector* time)
{
	return OF_ReturnType_completed;
}


int ESAggregator::transfDaqDisp(Vector* disp)
{  
	return OF_ReturnType_completed;
}


int ESAggregator::transfDaqVel(Vector* vel)
{  
	return OF_ReturnType_completed;
}


int ESAggregator::transfDaqAccel(Vector* accel)
{  
	return OF_ReturnType_completed;
}


int ESAggregator::transfDaqForce(Vector* force)
{  
	return OF_ReturnType_completed;
}


int ESAggregator::transfDaqTime(Vector* time)
{  
	return OF_ReturnType_completed;
}

