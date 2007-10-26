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
// Created: 01/07
// Revision: A
//
// Description: This file contains the implementation of the
// ECSimUniaxialMaterials class.

#include "ECSimUniaxialMaterials.h"

#include <UniaxialMaterial.h>


ECSimUniaxialMaterials::ECSimUniaxialMaterials(int tag,
    int nummats, UniaxialMaterial **specimen)
    : ECSimulation(tag),
    numMats(nummats), theSpecimen(0),
    targDisp(0), targVel(0),
    measDisp(0), measVel(0), measForce(0)
{
    if (specimen == 0)  {
      opserr << "ECSimUniaxialMaterials::ECSimUniaxialMaterials() - "
          << "null specimen array passed\n";
      exit(OF_ReturnType_failed);
    }

    // allocate memory for the uniaxial materials
    theSpecimen = new UniaxialMaterial* [numMats];
    if (theSpecimen == 0)  {
        opserr << "ECSimUniaxialMaterials::ECSimUniaxialMaterials() - "
            << "failed to allocate pointers for uniaxial materials\n";
        exit(OF_ReturnType_failed);
    }

    // get copies of the uniaxial matrials
    for (int i=0; i<numMats; i++)  {
        if (specimen[i] == 0) {
            opserr << "ECSimUniaxialMaterials::ECSimUniaxialMaterials() - "
                "null uniaxial material pointer passed\n";
            exit(OF_ReturnType_failed);
        }
        theSpecimen[i] = specimen[i]->getCopy();
        if (theSpecimen[i] == 0) {
            opserr << "ECSimUniaxialMaterials::ECSimUniaxialMaterials() - "
                << "failed to copy uniaxial material\n";
            exit(OF_ReturnType_failed);
        }
    }
}


ECSimUniaxialMaterials::ECSimUniaxialMaterials(const ECSimUniaxialMaterials& ec)
    : ECSimulation(ec),
    numMats(0), theSpecimen(0),
    targDisp(0), targVel(0),
    measDisp(0), measVel(0), measForce(0)
{
    if (theSpecimen != 0)
        delete theSpecimen;

    // allocate memory for the uniaxial materials
    numMats = ec.numMats;
    theSpecimen = new UniaxialMaterial* [numMats];
    if (theSpecimen == 0)  {
        opserr << "ECSimUniaxialMaterials::ECSimUniaxialMaterials() - "
            << "failed to allocate pointers for uniaxial materials\n";
        exit(OF_ReturnType_failed);
    }

    // get copies of the uniaxial matrials
    for (int i=0; i<numMats; i++)  {
        if (ec.theSpecimen[i] == 0) {
            opserr << "ECSimUniaxialMaterials::ECSimUniaxialMaterials() - "
                "null uniaxial material pointer passed\n";
            exit(OF_ReturnType_failed);
        }
        theSpecimen[i] = ec.theSpecimen[i]->getCopy();
        if (theSpecimen[i] == 0) {
            opserr << "ECSimUniaxialMaterials::ECSimUniaxialMaterials() - "
                << "failed to copy uniaxial material\n";
            exit(OF_ReturnType_failed);
        }
    }
}


ECSimUniaxialMaterials::~ECSimUniaxialMaterials()
{
    // delete memory of materials
    if (theSpecimen != 0)
        delete theSpecimen;
    
    // delete memory of target vectors
    if (targDisp != 0)
        delete targDisp;
    if (targVel != 0)
        delete targVel;
    
    // delete memory of measured vectors
    if (measDisp != 0)
        delete measDisp;
    if (measVel != 0)
        delete measVel;
    if (measForce != 0)
        delete measForce;
}

int ECSimUniaxialMaterials::setSize(ID sizeT, ID sizeO)
{
    // check sizeTrial and sizeOut
    // for ECSimUniaxialMaterials object
    
    // ECSimUniaxialMaterials objects only use 
    // disp, vel for trial and
    // disp, vel, force for output
    // check these are available in sizeT/sizeO.
    if (sizeT[OF_Resp_Disp] != numMats ||
        sizeT[OF_Resp_Vel] != numMats ||
        sizeO[OF_Resp_Disp] != numMats ||
        sizeO[OF_Resp_Vel] != numMats ||
        sizeO[OF_Resp_Force] != numMats) {
        opserr << "ECSimUniaxialMaterials::setSize() - wrong sizeTrial/Out\n"; 
        opserr << "see User Manual.\n";
        exit(OF_ReturnType_failed);
    }
    
    *sizeCtrl = sizeT;
    *sizeDaq = sizeO;
    
    return OF_ReturnType_completed;
}

int ECSimUniaxialMaterials::setup()
{
    if (targDisp != 0)
        delete targDisp;
    if (targVel != 0)
        delete targVel;

    if ((*sizeCtrl)[OF_Resp_Disp] != 0)  {
        targDisp = new Vector((*sizeCtrl)[OF_Resp_Disp]);
        targDisp->Zero();
    }
    if ((*sizeCtrl)[OF_Resp_Vel] != 0)  {
        targVel = new Vector((*sizeCtrl)[OF_Resp_Vel]);
        targVel->Zero();
    }

    if (measDisp != 0)
        delete measDisp;
    if (measVel != 0)
        delete measVel;
    if (measForce != 0)
        delete measForce;
    
    if ((*sizeDaq)[OF_Resp_Disp] != 0)  {
        measDisp = new Vector((*sizeDaq)[OF_Resp_Disp]);
        measDisp->Zero();
    }
    if ((*sizeDaq)[OF_Resp_Vel] != 0)  {
        measVel = new Vector((*sizeDaq)[OF_Resp_Vel]);
        measVel->Zero();
    }
    if ((*sizeDaq)[OF_Resp_Force] != 0)  {
        measForce = new Vector((*sizeDaq)[OF_Resp_Force]);
        measForce->Zero();
    }

    this->control();
    this->acquire();
    
    return OF_ReturnType_completed;
}


int ECSimUniaxialMaterials::setTrialResponse(const Vector* disp,
    const Vector* vel,
    const Vector* accel,
    const Vector* force,
    const Vector* time)
{
    *targDisp = *disp;
    if (theFilter != 0)  {
        for (int i=0; i<(*sizeCtrl)[OF_Resp_Disp]; i++)
            (*targDisp)(i) = theFilter->filtering((*targDisp)(i));
    }
    *targVel = *vel;
    
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
    
    *disp  = *measDisp;
    *vel   = *measVel;
    *force = *measForce;
    
    return OF_ReturnType_completed;
}


int ECSimUniaxialMaterials::commitState()
{
    int rValue = 0;

    for (int i=0; i<numMats; i++)  {
        rValue += theSpecimen[i]->commitState();
    }
    
    return rValue;
}


ExperimentalControl* ECSimUniaxialMaterials::getCopy()
{
    return new ECSimUniaxialMaterials(*this);
}


void ECSimUniaxialMaterials::Print(OPS_Stream &s, int flag)
{
    s << "****************************************************************\n";
    s << "* ExperimentalControl: " << this->getTag() << endln; 
    s << "* type: ECSimUniaxialMaterials\n";
    for (int i=0; i<numMats; i++)  {
        s << "*\tUniaxialMaterial, tag: " << theSpecimen[i]->getTag() << endln;
    }
    if (theFilter != 0) {
        s << "*\tFilter: " << *theFilter << endln;
    }
    s << "****************************************************************\n";
    s << endln;
}


int ECSimUniaxialMaterials::control()
{
    int rValue = 0;

    for (int i=0; i<numMats; i++)  {
        rValue += theSpecimen[i]->setTrialStrain((*targDisp)(i),(*targVel)(i));
    }
    
    return rValue;
}


int ECSimUniaxialMaterials::acquire()
{
    for (int i=0; i<numMats; i++)  {
        (*measDisp)(i)  = theSpecimen[i]->getStrain();
        (*measVel)(i)   = theSpecimen[i]->getStrainRate();
        (*measForce)(i) = theSpecimen[i]->getStress();
    }
    
    return OF_ReturnType_completed;
}
