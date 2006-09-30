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
// $Source$

// Written: Yoshi (yos@catfish.dpri.kyoto-u.ac.jp)
// Created: 09/06
// Revision: A
//
// Purpose: This file contains the implementation of 
// ECSimUniaxialMaterials.

#include "ECSimUniaxialMaterials.h"
#include <ArrayOfTaggedObjectsIter.h>
#include <UniaxialMaterial.h>


ECSimUniaxialMaterials::ECSimUniaxialMaterials(int tag)
    : ECSimulation(tag),
    theSpecimens(0),
    cDispV(0), cVelV(0), 
    dDispV(0), dVelV(0), dForceV(0)
{
    theSpecimens = new ArrayOfTaggedObjects(100);
}


ECSimUniaxialMaterials::ECSimUniaxialMaterials(const ECSimUniaxialMaterials& ec)
    : ECSimulation(ec)
{
    if(theSpecimens != 0)
        delete theSpecimens;
    if(ec.theSpecimens != 0) {
        int ndim = (ec.theSpecimens)->getNumComponents();
        theSpecimens = new ArrayOfTaggedObjects(ndim);
        for(int i=1; i<=ndim; i++) {
            TaggedObject *tmp = (ec.theSpecimens)->getComponentPtr(i);
            theSpecimens->addComponent(((UniaxialMaterial*)tmp)->getCopy());
        }
    }
    
    cDispV = new Vector((*sizeCtrl)[OF_Resp_Disp]);
    cVelV = new Vector((*sizeCtrl)[OF_Resp_Vel]); 
    
    dDispV = new Vector((*sizeDaq)[OF_Resp_Disp]);
    dVelV = new Vector((*sizeDaq)[OF_Resp_Vel]);
    dForceV = new Vector((*sizeDaq)[OF_Resp_Force]);
}


ECSimUniaxialMaterials::~ECSimUniaxialMaterials()
{
    if(theSpecimens != 0)
        delete theSpecimens;
    
    if(cDispV != 0)
        delete cDispV;
    if(cVelV != 0)
        delete cVelV;
    
    if(dDispV != 0)
        delete dDispV;
    if(dVelV != 0)
        delete dVelV;
    if(dForceV != 0)
        delete dForceV;
}

int ECSimUniaxialMaterials::setSize(ID sizeT, ID sizeO)
{
    // check sizeTrial and sizeOut
    // for ECSimUniaxialMaterials object
    
    // ECSimUniaxialMaterials object only use 
    // disp, vel for trial
    // disp, vel, force for output
    // check these are available in sizeT/sizeO.
    
    int numMaterial = theSpecimens->getNumComponents();
    if(sizeT[OF_Resp_Disp] != numMaterial ||
        sizeT[OF_Resp_Vel] != numMaterial ||
        sizeO[OF_Resp_Disp] != numMaterial ||
        sizeO[OF_Resp_Vel] != numMaterial ||
        sizeO[OF_Resp_Force] != numMaterial) {
        opserr << "ECSimUniaxialMaterials::setSize - wrong sizeTrial/Out\n"; 
        opserr << "see User Manual.\n";
        exit(1);
    }
    
    *sizeCtrl = sizeT;
    *sizeDaq = sizeO;
    
    return OF_ReturnType_completed;
}

int ECSimUniaxialMaterials::setup()
{
    // setup for ctrl/daq vectors of ECSimUniaxialMaterials
    cDispV = new Vector((*sizeCtrl)[OF_Resp_Disp]);
    cVelV = new Vector((*sizeCtrl)[OF_Resp_Vel]); 
    
    dDispV = new Vector((*sizeDaq)[OF_Resp_Disp]);
    dVelV = new Vector((*sizeDaq)[OF_Resp_Vel]);
    dForceV = new Vector((*sizeDaq)[OF_Resp_Force]);
    
    
    cDispV->Zero();
    cVelV->Zero();
    
    ArrayOfTaggedObjectsIter *iter = new ArrayOfTaggedObjectsIter(*theSpecimens);
    UniaxialMaterial *theSpecimen;
    int i = 0;
    while((theSpecimen = (UniaxialMaterial*)((*iter)())) != 0) {
        theSpecimen->setTrialStrain((*cDispV)[i], (*cVelV)[i]);
        theSpecimen->getStress();
        (*dDispV)[i] = theSpecimen->getStrain();
        (*dVelV)[i] = theSpecimen->getStrainRate();
        i++;
    }
    delete iter;
    
    return OF_ReturnType_completed;
}


int ECSimUniaxialMaterials::setTrialResponse(const Vector* disp,
    const Vector* vel,
    const Vector* accel,
    const Vector* force,
    const Vector* time)
{
    *cDispV = *disp;
    *cVelV = *vel;
    
    this->control();
    
    return OF_ReturnType_completed;
}


int ECSimUniaxialMaterials::getDaqResponse(Vector* disp,
    Vector* vel,
    Vector* accel,
    Vector* force,
    Vector* time)
{
    this->acquire();
    
    *disp = *dDispV;
    *vel = *dVelV;
    *force = *dForceV;
    
    return OF_ReturnType_completed;
}


int ECSimUniaxialMaterials::commitState()
{
    ExperimentalControl::commitState();
    ArrayOfTaggedObjectsIter *iter = new ArrayOfTaggedObjectsIter(*theSpecimens);
    UniaxialMaterial *theSpecimen;
    while((theSpecimen = (UniaxialMaterial*)((*iter)())) != 0) {
        theSpecimen->commitState();
    }
    delete iter;
    
    return OF_ReturnType_completed;
}


ExperimentalControl* ECSimUniaxialMaterials::getCopy()
{
    return new ECSimUniaxialMaterials(*this);
}


int ECSimUniaxialMaterials::addDummySpecimen(UniaxialMaterial *specimen)
{
    int result = theSpecimens->addComponent(specimen);
    
    return result;
}


void ECSimUniaxialMaterials::Print(OPS_Stream &s, int flag)
{
    s << "ExperimentalControl: " << this->getTag(); 
    s << " type: ECSimUniaxialMaterials\n";
    ArrayOfTaggedObjectsIter *iter = new ArrayOfTaggedObjectsIter(*theSpecimens);
    UniaxialMaterial *theSpecimen;
    while((theSpecimen = (UniaxialMaterial*)((*iter)())) != 0) {
        s << "\tUniaxialMaterial, tag: " << theSpecimen->getTag() << endln;
    }
    delete iter;
}


int ECSimUniaxialMaterials::control()
{
    ArrayOfTaggedObjectsIter *iter = new ArrayOfTaggedObjectsIter(*theSpecimens);
    UniaxialMaterial *theSpecimen;
    int i = 0;
    while((theSpecimen = (UniaxialMaterial*)((*iter)())) != 0) {
        theSpecimen->setTrialStrain((*cDispV)[i], (*cVelV)[i]);
        i++;
    }
    delete iter;
    
    return OF_ReturnType_completed;
}


int ECSimUniaxialMaterials::acquire()
{
    ArrayOfTaggedObjectsIter *iter = new ArrayOfTaggedObjectsIter(*theSpecimens);
    UniaxialMaterial *theSpecimen;
    int i = 0;
    while((theSpecimen = (UniaxialMaterial*)((*iter)())) != 0) {
        (*dDispV)[i] = theSpecimen->getStrain();
        (*dVelV)[i] = theSpecimen->getStrainRate();
        (*dForceV)[i] = theSpecimen->getStress();
        i++;
    }
    delete iter;
    
    return OF_ReturnType_completed;
}
