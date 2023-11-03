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
**   Andreas Schellenberg (andreas.schellenberg@gmail.com)            **
**   Yoshikazu Takahashi (yos@catfish.dpri.kyoto-u.ac.jp)             **
**   Gregory L. Fenves (fenves@berkeley.edu)                          **
**   Stephen A. Mahin (mahin@berkeley.edu)                            **
**                                                                    **
** ****************************************************************** */

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 01/07
// Revision: A
//
// Description: This file contains the implementation of the
// ECSimUniaxialMaterials class.

#include "ECSimUniaxialMaterials.h"

#include <UniaxialMaterial.h>
#include <elementAPI.h>


void* OPF_ECSimUniaxialMaterial()
{
    // pointer to experimental control that will be returned
    ExperimentalControl* theControl = 0;
    
    if (OPS_GetNumRemainingInputArgs() < 2) {
        opserr << "WARNING invalid number of arguments\n";
        opserr << "Want: expControl SimUniaxialMaterials tag matTags "
            << "<-ctrlFilters (5 filterTag)> <-daqFilters (5 filterTag)>\n";
        return 0;
    }
    
    // control tag
    int tag;
    int numdata = 1;
    if (OPS_GetIntInput(&numdata, &tag) != 0) {
        opserr << "WARNING invalid expControl SimUniaxialMaterials tag\n";
        return 0;
    }
    
    // material tags
    ID matTags(32);
    int numMats = 0;
    while (OPS_GetNumRemainingInputArgs() > 0) {
        int matTag;
        numdata = 1;
        int numArgs = OPS_GetNumRemainingInputArgs();
        if (OPS_GetIntInput(&numdata, &matTag) < 0) {
            if (numArgs > OPS_GetNumRemainingInputArgs()) {
                // move current arg back by one
                OPS_ResetCurrentInputArg(-1);
            }
            break;
        }
        matTags(numMats++) = matTag;
    }
    if (numMats == 0) {
        opserr << "WARNING no uniaxial materials specified\n";
        opserr << "expControl SimUniaxialMaterials " << tag << endln;
        return 0;
    }
    matTags.resize(numMats);
    
    // create the array to hold the uniaxial materials
    UniaxialMaterial** theSpecimen = new UniaxialMaterial * [numMats];
    if (theSpecimen == 0) {
        opserr << "WARNING out of memory\n";
        opserr << "expControl SimUniaxialMaterials " << tag << endln;
        return 0;
    }
    // populate array with uniaxial materials
    for (int i = 0; i < numMats; i++) {
        theSpecimen[i] = 0;
        theSpecimen[i] = OPS_GetUniaxialMaterial(matTags(i));
        if (theSpecimen[i] == 0) {
            opserr << "WARNING uniaxial material not found\n";
            opserr << "uniaxialMaterial " << matTags(i) << endln;
            opserr << "expControl SimUniaxialMaterials " << tag << endln;
            return 0;
        }
    }
    
    // parsing was successful, allocate the control
    theControl = new ECSimUniaxialMaterials(tag, numMats, theSpecimen);
    if (theControl == 0) {
        opserr << "WARNING could not create experimental control of type SimUniaxialMaterials\n";
        return 0;
    }
    
    // cleanup dynamic memory
    if (theSpecimen != 0)
        delete[] theSpecimen;
    
    return theControl;
}


ECSimUniaxialMaterials::ECSimUniaxialMaterials(int tag,
    int nummats, UniaxialMaterial **specimen)
    : ECSimulation(tag),
    numMats(nummats), theSpecimen(0),
    ctrlDisp(0), ctrlVel(0),
    daqDisp(0), daqVel(0), daqForce(0)
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
    ctrlDisp(0), ctrlVel(0),
    daqDisp(0), daqVel(0), daqForce(0)
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
    if (theSpecimen != 0)  {
        for (int i=0; i<numMats; i++)
            if (theSpecimen[i] != 0)
                delete theSpecimen[i];
        delete [] theSpecimen;
    }
    
    // delete memory of ctrl vectors
    if (ctrlDisp != 0)
        delete ctrlDisp;
    if (ctrlVel != 0)
        delete ctrlVel;
    
    // delete memory of daq vectors
    if (daqDisp != 0)
        delete daqDisp;
    if (daqVel != 0)
        delete daqVel;
    if (daqForce != 0)
        delete daqForce;
}


int ECSimUniaxialMaterials::setup()
{
    int rValue = 0;
    
    if (ctrlDisp != 0)
        delete ctrlDisp;
    if (ctrlVel != 0)
        delete ctrlVel;
    
    if ((*sizeCtrl)(OF_Resp_Disp) != 0)  {
        ctrlDisp = new Vector((*sizeCtrl)(OF_Resp_Disp));
        ctrlDisp->Zero();
    }
    if ((*sizeCtrl)(OF_Resp_Vel) != 0)  {
        ctrlVel = new Vector((*sizeCtrl)(OF_Resp_Vel));
        ctrlVel->Zero();
    }
    
    if (daqDisp != 0)
        delete daqDisp;
    if (daqVel != 0)
        delete daqVel;
    if (daqForce != 0)
        delete daqForce;
    
    if ((*sizeDaq)(OF_Resp_Disp) != 0)  {
        daqDisp = new Vector((*sizeDaq)(OF_Resp_Disp));
        daqDisp->Zero();
    }
    if ((*sizeDaq)(OF_Resp_Vel) != 0)  {
        daqVel = new Vector((*sizeDaq)(OF_Resp_Vel));
        daqVel->Zero();
    }
    if ((*sizeDaq)(OF_Resp_Force) != 0)  {
        daqForce = new Vector((*sizeDaq)(OF_Resp_Force));
        daqForce->Zero();
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
        opserr << "sizeT(Disp) = " << sizeT(OF_Resp_Disp) << " != " << numMats << endln;
        opserr << "sizeT(Vel) = " << sizeT(OF_Resp_Vel) << " != " << numMats << endln;
        opserr << "sizeO(Disp) = " << sizeO(OF_Resp_Disp) << " != " << numMats << endln;
        opserr << "sizeO(Vel) = " << sizeO(OF_Resp_Vel) << " != " << numMats << endln;
        opserr << "sizeO(Force) = " << sizeO(OF_Resp_Force) << " != " << numMats << endln;
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
        *ctrlDisp = *disp;
        if (theCtrlFilters[OF_Resp_Disp] != 0)  {
            for (i=0; i<(*sizeCtrl)(OF_Resp_Disp); i++)
                (*ctrlDisp)(i) = theCtrlFilters[OF_Resp_Disp]->filtering((*ctrlDisp)(i));
        }
    }
    if (vel != 0)  {
        *ctrlVel = *vel;
        if (theCtrlFilters[OF_Resp_Vel] != 0)  {
            for (i=0; i<(*sizeCtrl)(OF_Resp_Vel); i++)
                (*ctrlVel)(i) = theCtrlFilters[OF_Resp_Vel]->filtering((*ctrlVel)(i));
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
                (*daqDisp)(i) = theDaqFilters[OF_Resp_Disp]->filtering((*daqDisp)(i));
        }
        *disp = *daqDisp;
    }
    if (vel != 0)  {
        if (theDaqFilters[OF_Resp_Vel] != 0)  {
            for (i=0; i<(*sizeDaq)(OF_Resp_Vel); i++)
                (*daqVel)(i) = theDaqFilters[OF_Resp_Vel]->filtering((*daqVel)(i));
        }
        *vel = *daqVel;
    }
    if (force != 0)  {
        if (theDaqFilters[OF_Resp_Force] != 0)  {
            for (i=0; i<(*sizeDaq)(OF_Resp_Force); i++)
                (*daqForce)(i) = theDaqFilters[OF_Resp_Force]->filtering((*daqForce)(i));
        }
        *force = *daqForce;
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
    
    // ctrl displacements
    if (ctrlDisp != 0 && (
        strcmp(argv[0],"ctrlDisp") == 0 ||
        strcmp(argv[0],"ctrlDisplacement") == 0 ||
        strcmp(argv[0],"ctrlDisplacements") == 0))
    {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Disp); i++)  {
            sprintf(outputData,"ctrlDisp%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 1, *ctrlDisp);
    }
    
    // ctrl velocities
    else if (ctrlVel != 0 && (
        strcmp(argv[0],"ctrlVel") == 0 ||
        strcmp(argv[0],"ctrlVelocity") == 0 ||
        strcmp(argv[0],"ctrlVelocities") == 0))
    {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Vel); i++)  {
            sprintf(outputData,"ctrlVel%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 2, *ctrlVel);
    }
    
    // daq displacements
    else if (daqDisp != 0 && (
        strcmp(argv[0],"daqDisp") == 0 ||
        strcmp(argv[0],"daqDisplacement") == 0 ||
        strcmp(argv[0],"daqDisplacements") == 0))
    {
        for (i=0; i<(*sizeDaq)(OF_Resp_Disp); i++)  {
            sprintf(outputData,"daqDisp%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 3, *daqDisp);
    }
    
    // daq velocities
    else if (daqVel != 0 && (
        strcmp(argv[0],"daqVel") == 0 ||
        strcmp(argv[0],"daqVelocity") == 0 ||
        strcmp(argv[0],"daqVelocities") == 0))
    {
        for (i=0; i<(*sizeDaq)(OF_Resp_Vel); i++)  {
            sprintf(outputData,"daqVel%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 4, *daqVel);
    }
    
    // daq forces
    else if (daqForce != 0 && (
        strcmp(argv[0],"daqForce") == 0 ||
        strcmp(argv[0],"daqForces") == 0))
    {
        for (i=0; i<(*sizeDaq)(OF_Resp_Force); i++)  {
            sprintf(outputData,"daqForce%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 5, *daqForce);
    }
    
    output.endTag();
    
    return theResponse;
}


int ECSimUniaxialMaterials::getResponse(int responseID, Information &info)
{
    switch (responseID)  {
    case 1:  // ctrl displacements
        return info.setVector(*ctrlDisp);
        
    case 2:  // ctrl velocities
        return info.setVector(*ctrlVel);
        
    case 3:  // daq displacements
        return info.setVector(*daqDisp);
        
    case 4:  // daq velocities
        return info.setVector(*daqVel);
        
    case 5:  // daq forces
        return info.setVector(*daqForce);
        
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
        if (theDaqFilters[i] != 0)
            s << " " << theDaqFilters[i]->getTag();
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
        rValue += theSpecimen[i]->setTrialStrain((*ctrlDisp)(i),(*ctrlVel)(i));
    }
    
    return rValue;
}


int ECSimUniaxialMaterials::acquire()
{
    for (int i=0; i<numMats; i++)  {
        (*daqDisp)(i)  = theSpecimen[i]->getStrain();
        (*daqVel)(i)   = theSpecimen[i]->getStrainRate();
        (*daqForce)(i) = theSpecimen[i]->getStress();
    }
    
    return OF_ReturnType_completed;
}
