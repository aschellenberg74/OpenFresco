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
            << "null specimen array passed.\n";
        exit(OF_ReturnType_failed);
    }

    // allocate memory for the uniaxial materials
    theSpecimen = new UniaxialMaterial* [numMats];
    if (theSpecimen == 0)  {
        opserr << "ECSimUniaxialMaterials::ECSimUniaxialMaterials() - "
            << "failed to allocate pointers for uniaxial materials.\n";
        exit(OF_ReturnType_failed);
    }

    // get copies of the uniaxial materials
    for (int i=0; i<numMats; i++)  {
        if (specimen[i] == 0) {
            opserr << "ECSimUniaxialMaterials::ECSimUniaxialMaterials() - "
                "null uniaxial material pointer passed.\n";
            exit(OF_ReturnType_failed);
        }
        theSpecimen[i] = specimen[i]->getCopy();
        if (theSpecimen[i] == 0) {
            opserr << "ECSimUniaxialMaterials::ECSimUniaxialMaterials() - "
                << "failed to copy uniaxial material.\n";
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
    // allocate memory for the uniaxial materials
    numMats = ec.numMats;
    theSpecimen = new UniaxialMaterial* [numMats];
    if (theSpecimen == 0)  {
        opserr << "ECSimUniaxialMaterials::ECSimUniaxialMaterials() - "
            << "failed to allocate pointers for uniaxial materials.\n";
        exit(OF_ReturnType_failed);
    }

    // get copies of the uniaxial materials
    for (int i=0; i<numMats; i++)  {
        if (ec.theSpecimen[i] == 0) {
            opserr << "ECSimUniaxialMaterials::ECSimUniaxialMaterials() - "
                "null uniaxial material pointer passed.\n";
            exit(OF_ReturnType_failed);
        }
        theSpecimen[i] = ec.theSpecimen[i]->getCopy();
        if (theSpecimen[i] == 0) {
            opserr << "ECSimUniaxialMaterials::ECSimUniaxialMaterials() - "
                << "failed to copy uniaxial material.\n";
            exit(OF_ReturnType_failed);
        }
    }
}


ECSimUniaxialMaterials::~ECSimUniaxialMaterials()
{
    // delete memory of materials
    if (theSpecimen != 0)
        delete [] theSpecimen;
    
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


int ECSimUniaxialMaterials::setup()
{
    int rValue = 0;
    
    if (targDisp != 0)
        delete targDisp;
    if (targVel != 0)
        delete targVel;
    
    if ((*sizeCtrl)(OF_Resp_Disp) != 0)  {
        targDisp = new Vector((*sizeCtrl)(OF_Resp_Disp));
        targDisp->Zero();
    }
    if ((*sizeCtrl)(OF_Resp_Vel) != 0)  {
        targVel = new Vector((*sizeCtrl)(OF_Resp_Vel));
        targVel->Zero();
    }
    
    if (measDisp != 0)
        delete measDisp;
    if (measVel != 0)
        delete measVel;
    if (measForce != 0)
        delete measForce;
    
    if ((*sizeDaq)(OF_Resp_Disp) != 0)  {
        measDisp = new Vector((*sizeDaq)(OF_Resp_Disp));
        measDisp->Zero();
    }
    if ((*sizeDaq)(OF_Resp_Vel) != 0)  {
        measVel = new Vector((*sizeDaq)(OF_Resp_Vel));
        measVel->Zero();
    }
    if ((*sizeDaq)(OF_Resp_Force) != 0)  {
        measForce = new Vector((*sizeDaq)(OF_Resp_Force));
        measForce->Zero();
    }
    
    // print experimental control information
    //this->Print(opserr);
    
    rValue += this->control();
    rValue += this->acquire();
    
    return rValue;
}


int ECSimUniaxialMaterials::setSize(ID sizeT, ID sizeO)
{
    // check sizeTrial and sizeOut
    // for ECSimUniaxialMaterials object
    
    // ECSimUniaxialMaterials objects only use 
    // disp, vel for trial and
    // disp, vel, force for output
    // check these are available in sizeT/sizeO.
    if (sizeT(OF_Resp_Disp) != numMats ||
        sizeT(OF_Resp_Vel) != numMats ||
        sizeO(OF_Resp_Disp) != numMats ||
        sizeO(OF_Resp_Vel) != numMats ||
        sizeO(OF_Resp_Force) != numMats) {
        opserr << "ECSimUniaxialMaterials::setSize() - wrong sizeTrial/Out\n"; 
        opserr << "see User Manual.\n";
        exit(OF_ReturnType_failed);
    }
    
    *sizeCtrl = sizeT;
    *sizeDaq = sizeO;
    
    return OF_ReturnType_completed;
}


int ECSimUniaxialMaterials::setTrialResponse(const Vector* disp,
    const Vector* vel,
    const Vector* accel,
    const Vector* force,
    const Vector* time)
{
    int i, rValue = 0;
    if (disp != 0)  {
        *targDisp = *disp;
        if (theCtrlFilters[OF_Resp_Disp] != 0)  {
            for (i=0; i<(*sizeCtrl)(OF_Resp_Disp); i++)
                (*targDisp)(i) = theCtrlFilters[OF_Resp_Disp]->filtering((*targDisp)(i));
        }
    }
    if (vel != 0)  {
        *targVel = *vel;
        if (theCtrlFilters[OF_Resp_Vel] != 0)  {
            for (i=0; i<(*sizeCtrl)(OF_Resp_Vel); i++)
                (*targVel)(i) = theCtrlFilters[OF_Resp_Vel]->filtering((*targVel)(i));
        }
    }
    
    rValue = this->control();
    
    return rValue;
}


int ECSimUniaxialMaterials::getDaqResponse(Vector* disp,
    Vector* vel,
    Vector* accel,
    Vector* force,
    Vector* time)
{
    this->acquire();
    
    int i;
    if (disp != 0)  {
        if (theDaqFilters[OF_Resp_Disp] != 0)  {
            for (i=0; i<(*sizeDaq)(OF_Resp_Disp); i++)
                (*measDisp)(i) = theDaqFilters[OF_Resp_Disp]->filtering((*measDisp)(i));
        }
        *disp = *measDisp;
    }
    if (vel != 0)  {
        if (theDaqFilters[OF_Resp_Vel] != 0)  {
            for (i=0; i<(*sizeDaq)(OF_Resp_Vel); i++)
                (*measVel)(i) = theDaqFilters[OF_Resp_Vel]->filtering((*measVel)(i));
        }
        *vel = *measVel;
    }
    if (force != 0)  {
        if (theDaqFilters[OF_Resp_Force] != 0)  {
            for (i=0; i<(*sizeDaq)(OF_Resp_Force); i++)
                (*measForce)(i) = theDaqFilters[OF_Resp_Force]->filtering((*measForce)(i));
        }
        *force = *measForce;
    }
    
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


Response* ECSimUniaxialMaterials::setResponse(const char **argv, int argc,
    OPS_Stream &output)
{
    int i;
    char outputData[15];
    Response *theResponse = 0;
    
    output.tag("ExpControlOutput");
    output.attr("ctrlType",this->getClassType());
    output.attr("ctrlTag",this->getTag());
        
    // target displacements
    if (strcmp(argv[0],"targDisp") == 0 ||
        strcmp(argv[0],"targetDisp") == 0 ||
        strcmp(argv[0],"targetDisplacement") == 0 ||
        strcmp(argv[0],"targetDisplacements") == 0)
    {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Disp); i++)  {
            sprintf(outputData,"targDisp%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 1,
            Vector((*sizeCtrl)(OF_Resp_Disp)));
    }
    
    // target velocities
    if (strcmp(argv[0],"targVel") == 0 ||
        strcmp(argv[0],"targetVel") == 0 ||
        strcmp(argv[0],"targetVelocity") == 0 ||
        strcmp(argv[0],"targetVelocities") == 0)
    {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Vel); i++)  {
            sprintf(outputData,"targVel%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 2,
            Vector((*sizeCtrl)(OF_Resp_Vel)));
    }
    
    // measured displacements
    if (strcmp(argv[0],"measDisp") == 0 ||
        strcmp(argv[0],"measuredDisp") == 0 ||
        strcmp(argv[0],"measuredDisplacement") == 0 ||
        strcmp(argv[0],"measuredDisplacements") == 0)
    {
        for (i=0; i<(*sizeDaq)(OF_Resp_Disp); i++)  {
            sprintf(outputData,"measDisp%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 3,
            Vector((*sizeDaq)(OF_Resp_Disp)));
    }
    
    // measured velocities
    if (strcmp(argv[0],"measVel") == 0 ||
        strcmp(argv[0],"measuredVel") == 0 ||
        strcmp(argv[0],"measuredVelocity") == 0 ||
        strcmp(argv[0],"measuredVelocities") == 0)
    {
        for (i=0; i<(*sizeDaq)(OF_Resp_Vel); i++)  {
            sprintf(outputData,"measVel%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 4,
            Vector((*sizeDaq)(OF_Resp_Vel)));
    }
    
    // measured forces
    if (strcmp(argv[0],"measForce") == 0 ||
        strcmp(argv[0],"measuredForce") == 0 ||
        strcmp(argv[0],"measuredForces") == 0)
    {
        for (i=0; i<(*sizeDaq)(OF_Resp_Force); i++)  {
            sprintf(outputData,"measForce%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 5,
            Vector((*sizeDaq)(OF_Resp_Force)));
    }
    
    output.endTag();
    
    return theResponse;
}


int ECSimUniaxialMaterials::getResponse(int responseID, Information &info)
{
    switch (responseID)  {
    case 1:  // target displacements
        return info.setVector(*targDisp);
        
    case 2:  // target velocities
        return info.setVector(*targVel);
        
    case 3:  // measured displacements
        return info.setVector(*measDisp);
        
    case 4:  // measured velocities
        return info.setVector(*measVel);
        
    case 5:  // measured forces
        return info.setVector(*measForce);
        
    default:
        return -1;
    }
}


void ECSimUniaxialMaterials::Print(OPS_Stream &s, int flag)
{
    s << "****************************************************************\n";
    s << "* ExperimentalControl: " << this->getTag() << endln; 
    s << "*   type: ECSimUniaxialMaterials\n";
    for (int i=0; i<numMats; i++)  {
        s << "*   UniaxialMaterial: " << theSpecimen[i]->getTag() << endln;
    }
    s << "*   ctrlFilters:";
    for (int i=0; i<OF_Resp_All; i++)  {
        if (theCtrlFilters[i] != 0)
            s << " " << theCtrlFilters[i]->getTag();
        else
            s << " 0";
    }
    s << "\n*   daqFilters:";
    for (int i=0; i<OF_Resp_All; i++)  {
        if (theCtrlFilters[i] != 0)
            s << " " << theCtrlFilters[i]->getTag();
        else
            s << " 0";
    }
    s << endln;
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
