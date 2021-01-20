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

#ifndef ECSimUniaxialMaterials_h
#define ECSimUniaxialMaterials_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 10/07
// Revision: A
//
// Description: This file contains the class definition for 
// ECSimUniaxialMaterials. ECSimUniaxialMaterials is a controller
// class for simulating the behavior of a specimen using any number
// of OpenSees uniaxial material objects. The materials are uncoupled.

#include "ECSimulation.h"

class UniaxialMaterial;

class ECSimUniaxialMaterials : public ECSimulation
{
public:
    // constructors
    ECSimUniaxialMaterials(int tag, int numMats,
        UniaxialMaterial **theSpecimen);
    ECSimUniaxialMaterials(const ECSimUniaxialMaterials& ec);
    
    // destructor
    virtual ~ECSimUniaxialMaterials();
    
    // method to get class type
    const char *getClassType() const {return "ECSimUniaxialMaterials";};
    
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
    
    // public methods for experimental control recorder
    virtual Response *setResponse(const char **argv, int argc,
        OPS_Stream &output);
    virtual int getResponse(int responseID, Information &info);
    
    // public methods for output
    void Print(OPS_Stream &s, int flag = 0);

protected:
    // protected methods to set and to get response
    virtual int control();
    virtual int acquire();

private:
    int numMats;                     // number of uniaxial materials
    UniaxialMaterial **theSpecimen;  // uniaxial materials
    
    Vector *ctrlDisp, *ctrlVel;
    Vector *daqDisp, *daqVel, *daqForce;
};

#endif
