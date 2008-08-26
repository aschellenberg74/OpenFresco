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

#ifndef ECxPCtarget_h
#define ECxPCtarget_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 09/06
// Revision: A
//
// Description: This file contains the class definition for ECxPCtarget.
// ECxPCtarget is a controller class for communicating with a xPC Target
// digital signal processor.

#include "ExperimentalControl.h"

class ECxPCtarget : public ExperimentalControl
{
public:
    // constructors
    ECxPCtarget(int tag, int pcType, char *ipAddress,
        char *ipPort, char *appName, char *appPath = 0);
    ECxPCtarget(const ECxPCtarget &ec);
    
    // destructor
    virtual ~ECxPCtarget();
    
    // method to get class type
    const char *getClassType() const {return "ECxPCtarget";};
    
    // public methods to set and to get response
    virtual int setup();
    virtual int setSize(ID sizeT, ID sizeO);
    
    virtual int setTrialResponse(const Vector* disp, 
        const Vector* vel,
        const Vector* accel,
        const Vector* force,
        const Vector* time);
    virtual int getDaqResponse(Vector* disp,
        Vector* vel,
        Vector* accel,
        Vector* force,
        Vector* time);
    
    virtual int commitState();
    
    virtual ExperimentalControl *getCopy();
    
    // public methods for output
    void Print(OPS_Stream &s, int flag = 0);    
    
protected:
    // protected methods to set and to get response
    virtual int control();
    virtual int acquire();
    
private:
    void sleep(const clock_t wait);
    
    int pcType, port;
    char *ipAddress, *ipPort, *appName, *appPath;
    char errMsg[80];

    double newTarget, switchPC, atTarget;
    double *targDisp, *targVel, *targAccel;
    double *measDisp, *measForce;
    
    int newTargetId, switchPCId, atTargetId;
    int targDispId, targVelId, targAccelId;
    int *measDispId, *measForceId;
};

#endif
