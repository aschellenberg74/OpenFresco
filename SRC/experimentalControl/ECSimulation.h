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

#ifndef ECSimulation_h
#define ECSimulation_h

// Written: Yoshi (yos@catfish.dpri.kyoto-u.ac.jp)
// Created: 09/06
// Revision: A
//
// Description: This file contains the class definition for 
// ECSimulation.

#include "ExperimentalControl.h"

class ECSimulation : public ExperimentalControl
{
public:
    // constructors
    ECSimulation(int tag);
    ECSimulation(const ECSimulation& ec);
    
    // destructor
    virtual ~ECSimulation();
    
    // method to get class type
    const char *getClassType() const {return "ECSimulation";};
    
    // public methods to set and to get response
    virtual int setup() = 0;
    virtual int setSize(ID sizeT, ID sizeO) = 0;
    
    virtual int setTrialResponse(const Vector* disp,
        const Vector* vel,
        const Vector* accel,
        const Vector* force,
        const Vector* time) = 0;
    virtual int getDaqResponse(Vector* disp,
        Vector* vel,
        Vector* accel,
        Vector* force,
        Vector* time) = 0;
    
    virtual ExperimentalControl *getCopy() = 0;

protected:
    // protected methods to set and to get response
    virtual int control() = 0;
    virtual int acquire() = 0;
};

#endif
