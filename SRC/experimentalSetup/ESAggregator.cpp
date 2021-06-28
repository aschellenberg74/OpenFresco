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
// Description: This file contains the implementation of ESAggregator.

#include "ESAggregator.h"

#include <ExperimentalControl.h>

#include <elementAPI.h>


void* OPF_ESAggregator()
{
    // pointer to experimental setup that will be returned
    ExperimentalSetup* theSetup = 0;

    if (OPS_GetNumRemainingInputArgs() < 4) {
        opserr << "WARNING invalid number of arguments\n";
        opserr << "Want: expSetup Aggregator tag <-control ctrlTag> "
            << "-setup setupTagi ...\n";
        return 0;
    }
    
    // setup tag
    int tag;
    int numdata = 1;
    if (OPS_GetIntInput(&numdata, &tag) != 0) {
        opserr << "WARNING invalid expSetup Aggregator tag\n";
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
            opserr << "expSetup Aggregator " << tag << endln;
            return 0;
        }
        theControl = OPF_getExperimentalControl(ctrlTag);
        if (theControl == 0) {
            opserr << "WARNING experimental control not found\n";
            opserr << "expControl: " << ctrlTag << endln;
            opserr << "expSetup Aggregator " << tag << endln;
            return 0;
        }
    }
    else {
        // move current arg back by one
        OPS_ResetCurrentInputArg(-1);
    }
    
    // experimental setups
    type = OPS_GetString();
    if (strcmp(type, "-setup") != 0) {
        opserr << "WARNING expecting -setup setupTagi ...\n";
        opserr << "expSetup Aggregator " << tag << endln;
        return 0;
    }
    ID setupTags(32);
    int numSetups = 0;
    while (OPS_GetNumRemainingInputArgs() > 0) {
        int tagi;
        numdata = 1;
        int numArgs = OPS_GetNumRemainingInputArgs();
        if (OPS_GetIntInput(&numdata, &tagi) < 0) {
            if (numArgs > OPS_GetNumRemainingInputArgs()) {
                // move current arg back by one
                OPS_ResetCurrentInputArg(-1);
            }
            break;
        }
        setupTags(numSetups++) = tagi;
    }
    if (numSetups < 2) {
        opserr << "WARNING need at least 2 setups\n";
        opserr << "expSetup Aggregator " << tag << endln;
        return 0;
    }
    setupTags.resize(numSetups);
    
    // create the array to hold the experimental setups
    ExperimentalSetup** expSetups = new ExperimentalSetup * [numSetups];
    if (expSetups == 0) {
        opserr << "WARNING out of memory\n";
        opserr << "expSetup Aggregator " << tag << endln;
        return 0;
    }
    // populate array with experimental setups
    for (int i = 0; i < numSetups; i++) {
        expSetups[i] = 0;
        expSetups[i] = OPF_getExperimentalSetup(setupTags(i));
        if (expSetups[i] == 0) {
            opserr << "WARNING experimental setup not found\n";
            opserr << "expSetup " << setupTags(i) << endln;
            opserr << "expSetup Aggregator " << tag << endln;
            return 0;
        }
    }
    
    // parsing was successful, allocate the setup
    theSetup = new ESAggregator(tag, numSetups, expSetups, theControl);
    
    // cleanup dynamic memory
    if (expSetups != 0) {
        for (int i = 0; i < numSetups; i++)
            if (expSetups[i] != 0)
                delete expSetups[i];
        delete[] expSetups;
    }
    
    return theSetup;
}


ESAggregator::ESAggregator(int tag, int nSetups,
	ExperimentalSetup** setups,
    ExperimentalControl* control)
	: ExperimentalSetup(tag, control),
    numSetups(nSetups), theSetups(0),
    sizeTrialAll(0), sizeOutAll(0), sizeCtrlAll(0), sizeDaqAll(0), 
    tDispAll(0), tVelAll(0), tAccelAll(0), tForceAll(0), tTimeAll(0),
    oDispAll(0), oVelAll(0), oAccelAll(0), oForceAll(0), oTimeAll(0),
    cDispAll(0), cVelAll(0), cAccelAll(0), cForceAll(0), cTimeAll(0),
    dDispAll(0), dVelAll(0), dAccelAll(0), dForceAll(0), dTimeAll(0)
{
    if (setups == 0)  {
        opserr << "ESAggregator::ESAggregator() - "
            << "null experimental setup array passed\n";
        exit(OF_ReturnType_failed);
    }
    
    // allocate memory for the experimental setups
    theSetups = new ExperimentalSetup* [numSetups];
    if (theSetups == 0)  {
        opserr << "ESAggregator::ESAggregator() - "
            << "failed to allocate pointers\n";
        exit(OF_ReturnType_failed);
    }
    
    // get copies of the setups to be aggregated
    for (int i=0; i<numSetups; i++)  {
        if (setups[i] == 0)  {
            opserr << "ESAggregator::ESAggregator() - "
                "null experimental setup pointer passed\n";
            exit(OF_ReturnType_failed);
        }
        theSetups[i] = setups[i]->getCopy();
        if (theSetups[i] == 0)  {
            opserr << "ESAggregator::ESAggregator() - "
                << "failed to copy experimental setup\n";
            exit(OF_ReturnType_failed);
        }
    }
    
    // allocate memory for the IDs
    sizeTrialAll = new ID [numSetups];
    sizeOutAll = new ID [numSetups];
    if (!sizeTrialAll || !sizeOutAll)  {
        opserr << "ESAggregator::ESAggregator() - "
            << "failed to create size ID arrays\n";
        exit(OF_ReturnType_failed);
    }
    sizeCtrlAll = new ID [numSetups];
    sizeDaqAll = new ID [numSetups];
    if (!sizeCtrlAll || !sizeDaqAll)  {
        opserr << "ESAggregator::ESAggregator() - "
            << "failed to create size ID arrays\n";
        exit(OF_ReturnType_failed);
    }
    
    // allocate memory for the vectors
    tDispAll = new Vector* [numSetups];
    tVelAll = new Vector* [numSetups];
    tAccelAll = new Vector* [numSetups];
    tForceAll = new Vector* [numSetups];
    tTimeAll = new Vector* [numSetups];
    if (!tDispAll || !tVelAll || !tAccelAll || !tForceAll || !tTimeAll)  {
        opserr << "ESAggregator::ESAggregator() - "
            << "failed to create trial Vector arrays\n";
        exit(OF_ReturnType_failed);
    }
    oDispAll = new Vector* [numSetups];
    oVelAll = new Vector* [numSetups];
    oAccelAll = new Vector* [numSetups];
    oForceAll = new Vector* [numSetups];
    oTimeAll = new Vector* [numSetups];
    if (!oDispAll || !oVelAll || !oAccelAll || !oForceAll || !oTimeAll)  {
        opserr << "ESAggregator::ESAggregator() - "
            << "failed to create output Vector arrays\n";
        exit(OF_ReturnType_failed);
    }
    cDispAll = new Vector* [numSetups];
    cVelAll = new Vector* [numSetups];
    cAccelAll = new Vector* [numSetups];
    cForceAll = new Vector* [numSetups];
    cTimeAll = new Vector* [numSetups];
    if (!cDispAll || !cVelAll || !cAccelAll || !cForceAll || !cTimeAll)  {
        opserr << "ESAggregator::ESAggregator() - "
            << "failed to create control Vector arrays\n";
        exit(OF_ReturnType_failed);
    }
    dDispAll = new Vector* [numSetups];
    dVelAll = new Vector* [numSetups];
    dAccelAll = new Vector* [numSetups];
    dForceAll = new Vector* [numSetups];
    dTimeAll = new Vector* [numSetups];
    if (!dDispAll || !dVelAll || !dAccelAll || !dForceAll || !dTimeAll)  {
        opserr << "ESAggregator::ESAggregator() - "
            << "failed to create data acquisition Vector arrays\n";
        exit(OF_ReturnType_failed);
    }
    
    // call setup method
    this->setup();
    
    // finally initialize all the vectors
    for (int i=0; i<numSetups; i++)  {
        // trial vectors
        tDispAll[i] = 0;
        if (sizeTrialAll[i](OF_Resp_Disp) != 0)  {
            tDispAll[i] = new Vector(sizeTrialAll[i](OF_Resp_Disp));
        }
        tVelAll[i] = 0;
        if (sizeTrialAll[i](OF_Resp_Vel) != 0)  {
            tVelAll[i] = new Vector(sizeTrialAll[i](OF_Resp_Vel));
        }
        tAccelAll[i] = 0;
        if (sizeTrialAll[i](OF_Resp_Accel) != 0)  {
            tAccelAll[i] = new Vector(sizeTrialAll[i](OF_Resp_Accel));
        }
        tForceAll[i] = 0;
        if (sizeTrialAll[i](OF_Resp_Force) != 0)  {
            tForceAll[i] = new Vector(sizeTrialAll[i](OF_Resp_Force));
        }
        tTimeAll[i] = 0;
        if (sizeTrialAll[i](OF_Resp_Time) != 0)  {
            tTimeAll[i] = new Vector(sizeTrialAll[i](OF_Resp_Time));
        }
        // output vectors
        oDispAll[i] = 0;
        if (sizeOutAll[i](OF_Resp_Disp) != 0)  {
            oDispAll[i] = new Vector(sizeOutAll[i](OF_Resp_Disp));
        }
        oVelAll[i] = 0;
        if (sizeOutAll[i](OF_Resp_Vel) != 0)  {
            oVelAll[i] = new Vector(sizeOutAll[i](OF_Resp_Vel));
        }
        oAccelAll[i] = 0;
        if (sizeOutAll[i](OF_Resp_Accel) != 0)  {
            oAccelAll[i] = new Vector(sizeOutAll[i](OF_Resp_Accel));
        }
        oForceAll[i] = 0;
        if (sizeOutAll[i](OF_Resp_Force) != 0)  {
            oForceAll[i] = new Vector(sizeOutAll[i](OF_Resp_Force));
        }
        oTimeAll[i] = 0;
        if (sizeOutAll[i](OF_Resp_Time) != 0)  {
            oTimeAll[i] = new Vector(sizeOutAll[i](OF_Resp_Time));
        }
        // control vectors
        cDispAll[i] = 0;
        if (sizeCtrlAll[i](OF_Resp_Disp) != 0)  {
            cDispAll[i] = new Vector(sizeCtrlAll[i](OF_Resp_Disp));
        }
        cVelAll[i] = 0;
        if (sizeCtrlAll[i](OF_Resp_Vel) != 0)  {
            cVelAll[i] = new Vector(sizeCtrlAll[i](OF_Resp_Vel));
        }
        cAccelAll[i] = 0;
        if (sizeCtrlAll[i](OF_Resp_Accel) != 0)  {
            cAccelAll[i] = new Vector(sizeCtrlAll[i](OF_Resp_Accel));
        }
        cForceAll[i] = 0;
        if (sizeCtrlAll[i](OF_Resp_Force) != 0)  {
            cForceAll[i] = new Vector(sizeCtrlAll[i](OF_Resp_Force));
        }
        cTimeAll[i] = 0;
        if (sizeCtrlAll[i](OF_Resp_Time) != 0)  {
            cTimeAll[i] = new Vector(sizeCtrlAll[i](OF_Resp_Time));
        }
        // data acquisition vectors
        dDispAll[i] = 0;
        if (sizeDaqAll[i](OF_Resp_Disp) != 0)  {
            dDispAll[i] = new Vector(sizeDaqAll[i](OF_Resp_Disp));
        }
        dVelAll[i] = 0;
        if (sizeDaqAll[i](OF_Resp_Vel) != 0)  {
            dVelAll[i] = new Vector(sizeDaqAll[i](OF_Resp_Vel));
        }
        dAccelAll[i] = 0;
        if (sizeDaqAll[i](OF_Resp_Accel) != 0)  {
            dAccelAll[i] = new Vector(sizeDaqAll[i](OF_Resp_Accel));
        }
        dForceAll[i] = 0;
        if (sizeDaqAll[i](OF_Resp_Force) != 0)  {
            dForceAll[i] = new Vector(sizeDaqAll[i](OF_Resp_Force));
        }
        dTimeAll[i] = 0;
        if (sizeDaqAll[i](OF_Resp_Time) != 0)  {
            dTimeAll[i] = new Vector(sizeDaqAll[i](OF_Resp_Time));
        }
    }
}


ESAggregator::ESAggregator(const ESAggregator& es)
	: ExperimentalSetup(es), theSetups(0),
    sizeTrialAll(0), sizeOutAll(0), sizeCtrlAll(0), sizeDaqAll(0), 
    tDispAll(0), tVelAll(0), tAccelAll(0), tForceAll(0), tTimeAll(0),
    oDispAll(0), oVelAll(0), oAccelAll(0), oForceAll(0), oTimeAll(0),
    cDispAll(0), cVelAll(0), cAccelAll(0), cForceAll(0), cTimeAll(0),
    dDispAll(0), dVelAll(0), dAccelAll(0), dForceAll(0), dTimeAll(0)
{
    // allocate memory for the experimental setups
    numSetups = es.numSetups;
    theSetups = new ExperimentalSetup* [numSetups];
    if (theSetups == 0)  {
        opserr << "ESAggregator::ESAggregator() - "
            << "failed to allocate pointers\n";
        exit(OF_ReturnType_failed);
    }
    
    // get copies of the setups to be aggregated
    for (int i=0; i<numSetups; i++)  {
        if (es.theSetups[i] == 0)  {
            opserr << "ESAggregator::ESAggregator() - "
                "null experimental setup pointer passed\n";
            exit(OF_ReturnType_failed);
        }
        theSetups[i] = es.theSetups[i]->getCopy();
        if (theSetups[i] == 0)  {
            opserr << "ESAggregator::ESAggregator() - "
                << "failed to copy experimental setup\n";
            exit(OF_ReturnType_failed);
        }
    }
    
    // allocate memory for the IDs
    sizeTrialAll = new ID [numSetups];
    sizeOutAll = new ID [numSetups];
    if (!sizeTrialAll || !sizeOutAll)  {
        opserr << "ESAggregator::ESAggregator() - "
            << "failed to create size ID arrays\n";
        exit(OF_ReturnType_failed);
    }
    sizeCtrlAll = new ID [numSetups];
    sizeDaqAll = new ID [numSetups];
    if (!sizeCtrlAll || !sizeDaqAll)  {
        opserr << "ESAggregator::ESAggregator() - "
            << "failed to create size ID arrays\n";
        exit(OF_ReturnType_failed);
    }
    
    // allocate memory for the vectors
    tDispAll = new Vector* [numSetups];
    tVelAll = new Vector* [numSetups];
    tAccelAll = new Vector* [numSetups];
    tForceAll = new Vector* [numSetups];
    tTimeAll = new Vector* [numSetups];
    if (!tDispAll || !tVelAll || !tAccelAll || !tForceAll || !tTimeAll)  {
        opserr << "ESAggregator::ESAggregator() - "
            << "failed to create trial Vector arrays\n";
        exit(OF_ReturnType_failed);
    }
    oDispAll = new Vector* [numSetups];
    oVelAll = new Vector* [numSetups];
    oAccelAll = new Vector* [numSetups];
    oForceAll = new Vector* [numSetups];
    oTimeAll = new Vector* [numSetups];
    if (!oDispAll || !oVelAll || !oAccelAll || !oForceAll || !oTimeAll)  {
        opserr << "ESAggregator::ESAggregator() - "
            << "failed to create output Vector arrays\n";
        exit(OF_ReturnType_failed);
    }
    cDispAll = new Vector* [numSetups];
    cVelAll = new Vector* [numSetups];
    cAccelAll = new Vector* [numSetups];
    cForceAll = new Vector* [numSetups];
    cTimeAll = new Vector* [numSetups];
    if (!cDispAll || !cVelAll || !cAccelAll || !cForceAll || !cTimeAll)  {
        opserr << "ESAggregator::ESAggregator() - "
            << "failed to create control Vector arrays\n";
        exit(OF_ReturnType_failed);
    }
    dDispAll = new Vector* [numSetups];
    dVelAll = new Vector* [numSetups];
    dAccelAll = new Vector* [numSetups];
    dForceAll = new Vector* [numSetups];
    dTimeAll = new Vector* [numSetups];
    if (!dDispAll || !dVelAll || !dAccelAll || !dForceAll || !dTimeAll)  {
        opserr << "ESAggregator::ESAggregator() - "
            << "failed to create data acquisition Vector arrays\n";
        exit(OF_ReturnType_failed);
    }
    
    // call setup method
    this->setup();
    
    // finally initialize all the vectors
    for (int i=0; i<numSetups; i++)  {
        // trial vectors
        tDispAll[i] = 0;
        if (sizeTrialAll[i](OF_Resp_Disp) != 0)  {
            tDispAll[i] = new Vector(sizeTrialAll[i](OF_Resp_Disp));
        }
        tVelAll[i] = 0;
        if (sizeTrialAll[i](OF_Resp_Vel) != 0)  {
            tVelAll[i] = new Vector(sizeTrialAll[i](OF_Resp_Vel));
        }
        tAccelAll[i] = 0;
        if (sizeTrialAll[i](OF_Resp_Accel) != 0)  {
            tAccelAll[i] = new Vector(sizeTrialAll[i](OF_Resp_Accel));
        }
        tForceAll[i] = 0;
        if (sizeTrialAll[i](OF_Resp_Force) != 0)  {
            tForceAll[i] = new Vector(sizeTrialAll[i](OF_Resp_Force));
        }
        tTimeAll[i] = 0;
        if (sizeTrialAll[i](OF_Resp_Time) != 0)  {
            tTimeAll[i] = new Vector(sizeTrialAll[i](OF_Resp_Time));
        }
        // output vectors
        oDispAll[i] = 0;
        if (sizeOutAll[i](OF_Resp_Disp) != 0)  {
            oDispAll[i] = new Vector(sizeOutAll[i](OF_Resp_Disp));
        }
        oVelAll[i] = 0;
        if (sizeOutAll[i](OF_Resp_Vel) != 0)  {
            oVelAll[i] = new Vector(sizeOutAll[i](OF_Resp_Vel));
        }
        oAccelAll[i] = 0;
        if (sizeOutAll[i](OF_Resp_Accel) != 0)  {
            oAccelAll[i] = new Vector(sizeOutAll[i](OF_Resp_Accel));
        }
        oForceAll[i] = 0;
        if (sizeOutAll[i](OF_Resp_Force) != 0)  {
            oForceAll[i] = new Vector(sizeOutAll[i](OF_Resp_Force));
        }
        oTimeAll[i] = 0;
        if (sizeOutAll[i](OF_Resp_Time) != 0)  {
            oTimeAll[i] = new Vector(sizeOutAll[i](OF_Resp_Time));
        }
        // control vectors
        cDispAll[i] = 0;
        if (sizeCtrlAll[i](OF_Resp_Disp) != 0)  {
            cDispAll[i] = new Vector(sizeCtrlAll[i](OF_Resp_Disp));
        }
        cVelAll[i] = 0;
        if (sizeCtrlAll[i](OF_Resp_Vel) != 0)  {
            cVelAll[i] = new Vector(sizeCtrlAll[i](OF_Resp_Vel));
        }
        cAccelAll[i] = 0;
        if (sizeCtrlAll[i](OF_Resp_Accel) != 0)  {
            cAccelAll[i] = new Vector(sizeCtrlAll[i](OF_Resp_Accel));
        }
        cForceAll[i] = 0;
        if (sizeCtrlAll[i](OF_Resp_Force) != 0)  {
            cForceAll[i] = new Vector(sizeCtrlAll[i](OF_Resp_Force));
        }
        cTimeAll[i] = 0;
        if (sizeCtrlAll[i](OF_Resp_Time) != 0)  {
            cTimeAll[i] = new Vector(sizeCtrlAll[i](OF_Resp_Time));
        }
        // data acquisition vectors
        dDispAll[i] = 0;
        if (sizeDaqAll[i](OF_Resp_Disp) != 0)  {
            dDispAll[i] = new Vector(sizeDaqAll[i](OF_Resp_Disp));
        }
        dVelAll[i] = 0;
        if (sizeDaqAll[i](OF_Resp_Vel) != 0)  {
            dVelAll[i] = new Vector(sizeDaqAll[i](OF_Resp_Vel));
        }
        dAccelAll[i] = 0;
        if (sizeDaqAll[i](OF_Resp_Accel) != 0)  {
            dAccelAll[i] = new Vector(sizeDaqAll[i](OF_Resp_Accel));
        }
        dForceAll[i] = 0;
        if (sizeDaqAll[i](OF_Resp_Force) != 0)  {
            dForceAll[i] = new Vector(sizeDaqAll[i](OF_Resp_Force));
        }
        dTimeAll[i] = 0;
        if (sizeDaqAll[i](OF_Resp_Time) != 0)  {
            dTimeAll[i] = new Vector(sizeDaqAll[i](OF_Resp_Time));
        }
    }
}


ESAggregator::~ESAggregator()
{
    // invoke the destructor on any objects created by the object
    // that the object still holds a pointer to
    int i;
    if (theSetups != 0)  {
        for (i=0; i<numSetups; i++)
            if (theSetups[i] != 0)
                delete theSetups[i];
        delete [] theSetups;
    }
    
    if (sizeTrialAll != 0)
        delete [] sizeTrialAll;
    if (sizeOutAll != 0)
        delete [] sizeOutAll;
    if (sizeCtrlAll != 0)
        delete [] sizeCtrlAll;
    if (sizeDaqAll != 0)
        delete [] sizeDaqAll;

    if (tDispAll != 0)  {
        for (i=0; i<numSetups; i++)
            if (tDispAll[i] != 0)
                delete tDispAll[i];
        delete [] tDispAll;
    }
    if (tVelAll != 0)  {
        for (i=0; i<numSetups; i++)
            if (tVelAll[i] != 0)
                delete tVelAll[i];
        delete [] tVelAll;
    }
    if (tAccelAll != 0)  {
        for (i=0; i<numSetups; i++)
            if (tAccelAll[i] != 0)
                delete tAccelAll[i];
        delete [] tAccelAll;
    }
    if (tForceAll != 0)  {
        for (i=0; i<numSetups; i++)
            if (tForceAll[i] != 0)
                delete tForceAll[i];
        delete [] tForceAll;
    }
    if (tTimeAll != 0)  {
        for (i=0; i<numSetups; i++)
            if (tTimeAll[i] != 0)
                delete tTimeAll[i];
        delete [] tTimeAll;
    }

    if (oDispAll != 0)  {
        for (i=0; i<numSetups; i++)
            if (oDispAll[i] != 0)
                delete oDispAll[i];
        delete [] oDispAll;
    }
    if (oVelAll != 0)  {
        for (i=0; i<numSetups; i++)
            if (oVelAll[i] != 0)
                delete oVelAll[i];
        delete [] oVelAll;
    }
    if (oAccelAll != 0)  {
        for (i=0; i<numSetups; i++)
            if (oAccelAll[i] != 0)
                delete oAccelAll[i];
        delete [] oAccelAll;
    }
    if (oForceAll != 0)  {
        for (i=0; i<numSetups; i++)
            if (oForceAll[i] != 0)
                delete oForceAll[i];
        delete [] oForceAll;
    }
    if (oTimeAll != 0)  {
        for (i=0; i<numSetups; i++)
            if (oTimeAll[i] != 0)
                delete oTimeAll[i];
        delete [] oTimeAll;
    }

    if (cDispAll != 0)  {
        for (i=0; i<numSetups; i++)
            if (cDispAll[i] != 0)
                delete cDispAll[i];
        delete [] cDispAll;
    }
    if (cVelAll != 0)  {
        for (i=0; i<numSetups; i++)
            if (cVelAll[i] != 0)
                delete cVelAll[i];
        delete [] cVelAll;
    }
    if (cAccelAll != 0)  {
        for (i=0; i<numSetups; i++)
            if (cAccelAll[i] != 0)
                delete cAccelAll[i];
        delete [] cAccelAll;
    }
    if (cForceAll != 0)  {
        for (i=0; i<numSetups; i++)
            if (cForceAll[i] != 0)
                delete cForceAll[i];
        delete [] cForceAll;
    }
    if (cTimeAll != 0)  {
        for (i=0; i<numSetups; i++)
            if (cTimeAll[i] != 0)
                delete cTimeAll[i];
        delete [] cTimeAll;
    }

    if (dDispAll != 0)  {
        for (i=0; i<numSetups; i++)
            if (dDispAll[i] != 0)
                delete dDispAll[i];
        delete [] dDispAll;
    }
    if (dVelAll != 0)  {
        for (i=0; i<numSetups; i++)
            if (dVelAll[i] != 0)
                delete dVelAll[i];
        delete [] dVelAll;
    }
    if (dAccelAll != 0)  {
        for (i=0; i<numSetups; i++)
            if (dAccelAll[i] != 0)
                delete dAccelAll[i];
        delete [] dAccelAll;
    }
    if (dForceAll != 0)  {
        for (i=0; i<numSetups; i++)
            if (dForceAll[i] != 0)
                delete dForceAll[i];
        delete [] dForceAll;
    }
    if (dTimeAll != 0)  {
        for (i=0; i<numSetups; i++)
            if (dTimeAll[i] != 0)
                delete dTimeAll[i];
        delete [] dTimeAll;
    }
}


int ESAggregator::setup()
{
    // setup the trial/out vectors
    sizeTrial->Zero();
    sizeOut->Zero();
    for (int i=0; i<numSetups; i++)  {
        sizeTrialAll[i] = theSetups[i]->getTrialSize();
        sizeOutAll[i] = theSetups[i]->getOutSize();
        
        for (int j=0; j<OF_Resp_Time; j++)  {
            (*sizeTrial)(j) += sizeTrialAll[i](j);
            (*sizeOut)(j) += sizeOutAll[i](j);
        }
    }
    (*sizeTrial)(OF_Resp_Time) = 1;
    (*sizeOut)(OF_Resp_Time) = 1;
    
    this->setTrialOutSize();
    
    // setup the ctrl/daq vectors
    sizeCtrl->Zero();
    sizeDaq->Zero();
    for (int i=0; i<numSetups; i++)  {
        sizeCtrlAll[i] = theSetups[i]->getCtrlSize();
        sizeDaqAll[i] = theSetups[i]->getDaqSize();
        
        for (int j=0; j<OF_Resp_Time; j++)  {
            (*sizeCtrl)(j) += sizeCtrlAll[i](j);
            (*sizeDaq)(j) += sizeDaqAll[i](j);
        }
    }
    (*sizeCtrl)(OF_Resp_Time) = 1;
    (*sizeDaq)(OF_Resp_Time) = 1;
    
    this->setCtrlDaqSize();
    
	return OF_ReturnType_completed;
}


int ESAggregator::transfTrialResponse(const Vector* disp, 
    const Vector* vel,
    const Vector* accel,
    const Vector* force,
    const Vector* time)
{
    int rValue = 0;
    int tDispID = 0, tVelID = 0, tAccelID = 0, tForceID = 0;
    int cDispID = 0, cVelID = 0, cAccelID = 0, cForceID = 0;
    
    if (disp != 0)  {
        for (int i=0; i<(*sizeTrial)(OF_Resp_Disp); i++)
            (*tDisp)(i) = (*disp)(i) * (*tDispFact)(i);
        cDisp->Zero();
    }
    if (vel != 0)  {
        for (int i=0; i<(*sizeTrial)(OF_Resp_Vel); i++)
            (*tVel)(i) = (*vel)(i) * (*tVelFact)(i);
        cVel->Zero();
    }
    if (accel != 0)  {
        for (int i=0; i<(*sizeTrial)(OF_Resp_Accel); i++)
            (*tAccel)(i) = (*accel)(i) * (*tAccelFact)(i);
        cAccel->Zero();
    }
    if (force != 0)  {
        for (int i=0; i<(*sizeTrial)(OF_Resp_Force); i++)
            (*tForce)(i) = (*force)(i) * (*tForceFact)(i);
        cForce->Zero();
    }
    if (time != 0)  {
        for (int i=0; i<(*sizeTrial)(OF_Resp_Time); i++)
            (*tTime)(i) = (*time)(i) * (*tTimeFact)(i);
        cTime->Zero();
    }
    
    for (int i=0; i<numSetups; i++)  {
        // extract trial response from aggregated setup
        if (disp != 0 && sizeTrialAll[i](OF_Resp_Disp) != 0)  {
            tDispAll[i]->Extract(*tDisp,tDispID);
            tDispID += sizeTrialAll[i](OF_Resp_Disp);
        }
        if (vel != 0 && sizeTrialAll[i](OF_Resp_Vel) != 0)  {
            tVelAll[i]->Extract(*tVel,tVelID);
            tVelID += sizeTrialAll[i](OF_Resp_Vel);
        }
        if (accel != 0 && sizeTrialAll[i](OF_Resp_Accel) != 0)  {
            tAccelAll[i]->Extract(*tAccel,tAccelID);
            tAccelID += sizeTrialAll[i](OF_Resp_Accel);
        }
        if (force != 0 && sizeTrialAll[i](OF_Resp_Force) != 0)  {
            tForceAll[i]->Extract(*tForce,tForceID);
            tForceID += sizeTrialAll[i](OF_Resp_Force);
        }
        if (time != 0 && sizeTrialAll[i](OF_Resp_Time) != 0)  {
            *tTimeAll[i] = *tTime;
        }
        
        // transform trial response of individual setup
        rValue = theSetups[i]->transfTrialResponse(tDispAll[i],
            tVelAll[i], tAccelAll[i], tForceAll[i], tTimeAll[i]);
        if (rValue != OF_ReturnType_completed)  {
            opserr << "ESAggregator::transfTrialResponse() - "
                << "fail to transform trial response of setup "
                << theSetups[i]->getTag() << endln;
            return OF_ReturnType_failed;
        }
        
        // get control response from individual setup
        rValue = theSetups[i]->getTrialResponse(cDispAll[i],
            cVelAll[i], cAccelAll[i], cForceAll[i], cTimeAll[i]);
        if (rValue != OF_ReturnType_completed)  {
            opserr << "ESAggregator::transfTrialResponse() - "
                << "fail to get trial response from setup "
                << theSetups[i]->getTag() << endln;
            return OF_ReturnType_failed;
        }
        
        // assemble control response of aggregated setup
        if (disp != 0 && sizeCtrlAll[i](OF_Resp_Disp) != 0)  {
            cDisp->Assemble(*cDispAll[i], cDispID);
            cDispID += sizeCtrlAll[i](OF_Resp_Disp);
        }
        if (vel != 0 && sizeCtrlAll[i](OF_Resp_Vel) != 0)  {
            cVel->Assemble(*cVelAll[i], cVelID);
            cVelID += sizeCtrlAll[i](OF_Resp_Vel);
        }
        if (accel != 0 && sizeCtrlAll[i](OF_Resp_Accel) != 0)  {
            cAccel->Assemble(*cAccelAll[i], cAccelID);
            cAccelID += sizeCtrlAll[i](OF_Resp_Accel);
        }
        if (force != 0 && sizeCtrlAll[i](OF_Resp_Force) != 0)  {
            cForce->Assemble(*cForceAll[i], cForceID);
            cForceID += sizeCtrlAll[i](OF_Resp_Force);
        }
        if (time != 0 && sizeCtrlAll[i](OF_Resp_Time) != 0)  {
            *cTime = *cTimeAll[i];
        }
    }
    
    if (disp != 0)  {
        for (int i=0; i<(*sizeCtrl)(OF_Resp_Disp); i++)
            (*cDisp)(i) *= (*cDispFact)(i);
    }
    if (vel != 0)  {
        for (int i=0; i<(*sizeCtrl)(OF_Resp_Vel); i++)
            (*cVel)(i) *= (*cVelFact)(i);
    }
    if (accel != 0)  {
        for (int i=0; i<(*sizeCtrl)(OF_Resp_Accel); i++)
            (*cAccel)(i) *= (*cAccelFact)(i);
    }
    if (force != 0)  {
        for (int i=0; i<(*sizeCtrl)(OF_Resp_Force); i++)
            (*cForce)(i) *= (*cForceFact)(i);
    }
    if (time != 0)  {
        for (int i=0; i<(*sizeCtrl)(OF_Resp_Time); i++)
            (*cTime)(i) *= (*cTimeFact)(i);
    }
    
    return OF_ReturnType_completed;
}


int ESAggregator::transfDaqResponse(Vector* disp,
    Vector* vel,
    Vector* accel,
    Vector* force,
    Vector* time)
{
    int rValue = 0;
    int dDispID = 0, dVelID = 0, dAccelID = 0, dForceID = 0;
    int oDispID = 0, oVelID = 0, oAccelID = 0, oForceID = 0;

    if (disp != 0)  {
        for (int i=0; i<(*sizeDaq)(OF_Resp_Disp); i++)
            (*dDisp)(i) *= (*dDispFact)(i);
        oDisp->Zero();
    }
    if (vel != 0)  {
        for (int i=0; i<(*sizeDaq)(OF_Resp_Vel); i++)
            (*dVel)(i) *= (*dVelFact)(i);
        oVel->Zero();
    }
    if (accel != 0)  {
        for (int i=0; i<(*sizeDaq)(OF_Resp_Accel); i++)
            (*dAccel)(i) *= (*dAccelFact)(i);
        oAccel->Zero();
    }
    if (force != 0)  {
        for (int i=0; i<(*sizeDaq)(OF_Resp_Force); i++)
            (*dForce)(i) *= (*dForceFact)(i);
        oForce->Zero();
    }
    if (time != 0)  {
        for (int i=0; i<(*sizeDaq)(OF_Resp_Time); i++)
            (*dTime)(i) *= (*dTimeFact)(i);
        oTime->Zero();
    }

    for (int i=0; i<numSetups; i++)  {
        // extract daq response from aggregated setup
        if (disp != 0 && sizeDaqAll[i](OF_Resp_Disp) != 0)  {
            dDispAll[i]->Extract(*dDisp,dDispID);
            dDispID += sizeDaqAll[i](OF_Resp_Disp);
        }
        if (vel != 0 && sizeDaqAll[i](OF_Resp_Vel) != 0)  {
            dVelAll[i]->Extract(*dVel,dVelID);
            dVelID += sizeDaqAll[i](OF_Resp_Vel);
        }
        if (accel != 0 && sizeDaqAll[i](OF_Resp_Accel) != 0)  {
            dAccelAll[i]->Extract(*dAccel,dAccelID);
            dAccelID += sizeDaqAll[i](OF_Resp_Accel);
        }
        if (force != 0 && sizeDaqAll[i](OF_Resp_Force) != 0)  {
            dForceAll[i]->Extract(*dForce,dForceID);
            dForceID += sizeDaqAll[i](OF_Resp_Force);
        }
        if (time != 0 && sizeDaqAll[i](OF_Resp_Time) != 0)  {
            *dTimeAll[i] = *dTime;
        }
        
        // set daq response in individual setup
        rValue = theSetups[i]->setDaqResponse(dDispAll[i],
            dVelAll[i], dAccelAll[i], dForceAll[i], dTimeAll[i]);
        if (rValue != OF_ReturnType_completed)  {
            opserr << "ESAggregator::transfDaqResponse() - "
                << "fail to set daq response in setup "
                << theSetups[i]->getTag() << endln;
            return OF_ReturnType_failed;
        }
        
        // transform daq response of individual setup
        rValue = theSetups[i]->transfDaqResponse(oDispAll[i],
            oVelAll[i], oAccelAll[i], oForceAll[i], oTimeAll[i]);
        if (rValue != OF_ReturnType_completed)  {
            opserr << "ESAggregator::transfDaqResponse() - "
                << "fail to transform daq response of setup "
                << theSetups[i]->getTag() << endln;
            return OF_ReturnType_failed;
        }

        // assemble output response of aggregated setup
        if (disp != 0 && sizeOutAll[i](OF_Resp_Disp) != 0) {
            oDisp->Assemble(*oDispAll[i], oDispID);
            oDispID += sizeOutAll[i](OF_Resp_Disp);
        }
        if (vel != 0 && sizeOutAll[i](OF_Resp_Vel) != 0) {
            oVel->Assemble(*oVelAll[i], oVelID);
            oVelID += sizeOutAll[i](OF_Resp_Vel);
        }
        if (accel != 0 && sizeOutAll[i](OF_Resp_Accel) != 0) {
            oAccel->Assemble(*oAccelAll[i], oAccelID);
            oAccelID += sizeOutAll[i](OF_Resp_Accel);
        }
        if (force != 0 && sizeOutAll[i](OF_Resp_Force) != 0) {
            oForce->Assemble(*oForceAll[i], oForceID);
            oForceID += sizeOutAll[i](OF_Resp_Force);
        }
        if (time != 0 && sizeOutAll[i](OF_Resp_Time) != 0) {
            *oTime = *oTimeAll[i];
        }
    }
    
    if (disp != 0)  {
        for (int i=0; i<(*sizeOut)(OF_Resp_Disp); i++)
            (*disp)(i) = (*oDisp)(i) * (*oDispFact)(i);
    }
    if (vel != 0)  {
        for (int i=0; i<(*sizeOut)(OF_Resp_Vel); i++)
            (*vel)(i) = (*oVel)(i) * (*oVelFact)(i);
    }
    if (accel != 0)  {
        for (int i=0; i<(*sizeOut)(OF_Resp_Accel); i++)
            (*accel)(i) = (*oAccel)(i) * (*oAccelFact)(i);
    }
    if (force != 0)  {
        for (int i=0; i<(*sizeOut)(OF_Resp_Force); i++)
            (*force)(i) = (*oForce)(i) * (*oForceFact)(i);
    }
    if (time != 0)  {
        for (int i=0; i<(*sizeOut)(OF_Resp_Time); i++)
            (*time)(i) = (*oTime)(i) * (*oTimeFact)(i);
    }
    
    return OF_ReturnType_completed;
}


int ESAggregator::commitState()
{
    int rValue = 0;
    
    // first commit individual setups
    for (int i=0; i<numSetups; i++)
        rValue += theSetups[i]->commitState();
    
    // then commit base class
    rValue += this->ExperimentalSetup::commitState();
    
	return rValue;
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
    s << " setups: ";
    for (int i=0; i<numSetups; i++)
        s << theSetups[i]->getTag() << ", ";
    s << endln;
	if (theControl != 0) {
		s << "\tExperimentalControl tag: " << theControl->getTag();
		s << *theControl;
    }
}


int ESAggregator::transfTrialDisp(const Vector* disp)
{      
    // does nothing
	return OF_ReturnType_completed;
}


int ESAggregator::transfTrialVel(const Vector* vel)
{  
    // does nothing
	return OF_ReturnType_completed;
}


int ESAggregator::transfTrialAccel(const Vector* accel)
{  
    // does nothing
	return OF_ReturnType_completed;
}


int ESAggregator::transfTrialForce(const Vector* force)
{  
    // does nothing
	return OF_ReturnType_completed;
}


int ESAggregator::transfTrialTime(const Vector* time)
{
    // does nothing
	return OF_ReturnType_completed;
}


int ESAggregator::transfDaqDisp(Vector* disp)
{  
    // does nothing
	return OF_ReturnType_completed;
}


int ESAggregator::transfDaqVel(Vector* vel)
{  
    // does nothing
	return OF_ReturnType_completed;
}


int ESAggregator::transfDaqAccel(Vector* accel)
{  
    // does nothing
	return OF_ReturnType_completed;
}


int ESAggregator::transfDaqForce(Vector* force)
{  
    // does nothing
	return OF_ReturnType_completed;
}


int ESAggregator::transfDaqTime(Vector* time)
{  
    // does nothing
	return OF_ReturnType_completed;
}
