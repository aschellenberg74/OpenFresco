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

#ifndef ESAggregator_h
#define ESAggregator_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 09/06
// Revision: A
//
// Description: This file contains the class definition for 
// ESAggregator. ESAggregator combines differnt experimental
// setups into one.

#include <ExperimentalSetup.h>
#include <ExperimentalControl.h>

class ESAggregator : public ExperimentalSetup
{
public:
    // constructors
    ESAggregator(int tag, int nSetups,
        ExperimentalSetup** setups,
        ID *sizeTrialAll, ID *sizeOutAll,
        ExperimentalControl* control = 0);
    ESAggregator(const ESAggregator& es);
    
    // destructor
    virtual ~ESAggregator();
    
    // public methods
    virtual int setSize(ID sizeT, ID sizeO);
    virtual int commitState();
    virtual int setup();
    
    // public methods to transform the responses
    virtual int transfTrialResponse(const Vector* disp,
        const Vector* vel,
        const Vector* accel,
        const Vector* force,
        const Vector* time);
    virtual int transfDaqResponse(Vector* disp,
        Vector* vel,
        Vector* accel,
        Vector* force,
        Vector* time);

    virtual ExperimentalSetup *getCopy();
    
    // public methods for output
    void Print(OPS_Stream &s, int flag = 0);
    
protected:	
    // protected tranformation methods 
    virtual int transfTrialDisp(const Vector* disp);
    virtual int transfTrialVel(const Vector* vel);
    virtual int transfTrialAccel(const Vector* accel);
    virtual int transfTrialForce(const Vector* force);
    virtual int transfTrialTime(const Vector* time);
    
    virtual int transfDaqDisp(Vector* disp);
    virtual int transfDaqVel(Vector* vel);
    virtual int transfDaqAccel(Vector* accel);
    virtual int transfDaqForce(Vector* force);
    virtual int transfDaqTime(Vector* time);
    
private:
    int numSetups;                  // number of setups to aggregate
    ExperimentalSetup **theSetups;  // setups to aggregate

    ID *sizeTrialAll;       // sum of all setup trial responses
    ID *sizeCtrlAll;        // sum of all setup control responses
    ID *sizeDaqAll;         // sum of all setup daq responses
    ID *sizeOutAll;         // sum of all setup output responses

    // trial vectors for all setups
    Vector **tDispAll;
    Vector **tVelAll;
    Vector **tAccelAll;
    Vector **tForceAll;
    Vector **tTimeAll;

    // control vectors for all setups
    Vector **cDispAll;
    Vector **cVelAll;
    Vector **cAccelAll;
    Vector **cForceAll;
    Vector **cTimeAll;

    // daq vectors for all setups
    Vector **dDispAll;
    Vector **dVelAll;
    Vector **dAccelAll;
    Vector **dForceAll;
    Vector **dTimeAll;

    // output vectors for all setups
    Vector **oDispAll;
    Vector **oVelAll;
    Vector **oAccelAll;
    Vector **oForceAll;
    Vector **oTimeAll;
};

#endif
