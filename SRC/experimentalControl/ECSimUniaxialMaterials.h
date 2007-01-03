/* ****************************************************************** **
**    OpenFRESCO - Open Framework                                     **
**                 for Experimental Setup and Control                 **
**                                                                    **
**                                                                    **
** Copyright (c) 2006, Yoshikazu Takahashi, Kyoto University          **
** All rights reserved.                                               **
**                                                                    **
** Licensed under the modified BSD License (the "License");           **
** you may not use this file except in compliance with the License.   **
** You may obtain a copy of the License in main directory.            **
** Unless required by applicable law or agreed to in writing,         **
** software distributed under the License is distributed on an        **
** "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,       **
** either express or implied. See the License for the specific        **
** language governing permissions and limitations under the License.  **
**                                                                    **
** Developed by:                                                      **
**   Yoshikazu Takahashi (yos@catfish.dpri.kyoto-u.ac.jp)             **
**   Andreas Schellenberg (andreas.schellenberg@gmx.net)              **
**   Gregory L. Fenves (fenves@berkeley.edu)                          **
**                                                                    **
** ****************************************************************** */

// $Revision$
// $Date$
// $URL: $

#ifndef ECSimUniaxialMaterials_h
#define ECSimUniaxialMaterials_h

// Written: Yoshi (yos@catfish.dpri.kyoto-u.ac.jp)
// Created: 09/06
// Revision: A
//
// Description: This file contains the class definition for 
// ECSimUniaxialMaterials.

#include "ECSimulation.h"

class UniaxialMaterial;

class ECSimUniaxialMaterials : public ECSimulation
{
public:
    // constructors
    ECSimUniaxialMaterials(int tag);
    ECSimUniaxialMaterials(const ECSimUniaxialMaterials& ec);
    
    // destructor
    virtual ~ECSimUniaxialMaterials();
    
    // public methods to set and to get response
    virtual int setSize(ID sizeT, ID sizeO);
    virtual int setup();
    
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

    virtual ExperimentalControl *getCopy (void);
    
    int addDummySpecimen(UniaxialMaterial *theSpecimen);
    
    // public methods for output
    void Print(OPS_Stream &s, int flag = 0);    
    
protected:
    // protected methods to set and to get response
    virtual int control();
    virtual int acquire();
    
private:
    ArrayOfTaggedObjects *theSpecimens;
    
    // minimum variable set for UniaxialMaterial
    Vector *cDispV;
    Vector *cVelV;
    
    Vector *dDispV;
    Vector *dVelV;
    Vector *dForceV;
};


#endif
