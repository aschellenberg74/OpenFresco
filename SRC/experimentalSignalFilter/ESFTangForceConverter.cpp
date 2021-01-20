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

// Written: Hong Kim (hongkim@berkeley.edu)
// Created: 10/10
// Revision: A
//
// Description: This file contains the class definition for 
// ESFTangForceConverter. This class converts the trial displacements
// to trial forces and measured displacements to measured forces using 
// the Experimental Tangent Stiffness class. It used the converting 
// method and does nothing with the filtering method. 

#include "ESFTangForceConverter.h"

#include <elementAPI.h>


void* OPF_ESFTangForceConverter()
{
    // pointer to experimental control that will be returned
    ExperimentalSignalFilter* theFilter = 0;
    
    if (OPS_GetNumRemainingInputArgs() < 4) {
        opserr << "WARNING invalid number of arguments\n";
        opserr << "Want: expSignalFilter TangentForceConverter tag "
            << "-initStif Kij -tangStif tangStifTag\n";
        return 0;
    }
    
    // filter tag
    int tag;
    int numdata = 1;
    if (OPS_GetIntInput(&numdata, &tag) != 0) {
        opserr << "WARNING invalid expSignalFilter TangentForceConverter tag\n";
        return 0;
    }
    
    // initial stiffness terms
    const char* type = OPS_GetString();
    if (strcmp(type, "-initStif") != 0 &&
        strcmp(type, "-initStiff") != 0) {
        opserr << "WARNING expecting -initStif Kij\n";
        opserr << "expSignalFilter TangentForceConverter " << tag << endln;
        return 0;
    }
    // check size of stiffness matrix
    int numArg = OPS_GetNumRemainingInputArgs();
    double dim = sqrt(numArg);
    if (fmod(dim, 1.0) != 0.0) {
        opserr << "WARNING Kij is not a square matrix\n";
        opserr << "expSignalFilter TangentForceConverter " << tag << endln;
        return 0;
    }
    // set the initial stiffness
    int numDOF = (int)dim;
    Matrix theInitStif(numDOF, numDOF);
    double stif[1024];
    numdata = numDOF * numDOF;
    if (OPS_GetDoubleInput(&numdata, stif) != 0) {
        opserr << "WARNING invalid initial stiffness terms\n";
        opserr << "expSignalFilter TangentForceConverter " << tag << endln;
        return 0;
    }
    theInitStif.setData(stif, numDOF, numDOF);
    
    // tangent stiffness
    type = OPS_GetString();
    if (strcmp(type, "-tangStif") != 0 &&
        strcmp(type, "-tangStiff") != 0) {
        opserr << "WARNING expecting -tangStif tangStifTag\n";
        opserr << "expSignalFilter TangentForceConverter " << tag << endln;
        return 0;
    }
    int tangStifTag;
    ExperimentalTangentStiff* theTangStif = 0;
    numdata = 1;
    if (OPS_GetIntInput(&numdata, &tangStifTag) != 0) {
        opserr << "WARNING invalid tangentStiff Tag\n";
        opserr << "expSignalFilter TangentForceConverter " << tag << endln;
        return 0;
    }
    theTangStif = OPF_GetExperimentalTangentStiff(tangStifTag);
    
    // parsing was successful, allocate the signal filter
    theFilter = new ESFTangForceConverter(tag, theInitStif, theTangStif);
    if (theFilter == 0) {
        opserr << "WARNING could not create experimental signal filter "
            << "of type ESFTangForceConverter\n";
        return 0;
    }
    
    return theFilter;
}


ESFTangForceConverter::ESFTangForceConverter(int tag, Matrix& initStif,
    ExperimentalTangentStiff* tangStif)
    : ExperimentalSignalFilter(tag), theTangStiff(tangStif), firstWarning(true),
    size(0), dispPast(0), forcePast(0), convertFrc(0), incrDisp(0), incrForce(0),
    kInit(initStif), kPrev(0,0), thetangStiffMat(0,0)
{
    // does nothing
}


ESFTangForceConverter::ESFTangForceConverter(const ESFTangForceConverter& esf)
    : ExperimentalSignalFilter(esf), theTangStiff(0), firstWarning(0),
    size(0), dispPast(0), forcePast(0), convertFrc(0), incrDisp(0), incrForce(0),
    kInit(0,0), kPrev(0,0), thetangStiffMat(0,0)
{
    if (esf.theTangStiff != 0)  {
        theTangStiff = (esf.theTangStiff)->getCopy();
        if (theTangStiff == 0)  {
            opserr << "ESFTangForceConverter::ESFTangForceConverter() - "
                << "failed to get a copy of experimental tangent stiff.\n";
            exit(OF_ReturnType_failed);
        }
    }
    firstWarning = esf.firstWarning;
    kInit = esf.kInit;
}


ESFTangForceConverter::~ESFTangForceConverter()
{
    if (theTangStiff != 0) 
        delete theTangStiff;
}


int ESFTangForceConverter::setSize(const int sz)
{
    size = sz;
    // resize variables
    convertFrc.resize(size);
    dispPast.resize(size);
    forcePast.resize(size);
    incrDisp.resize(size);
    incrForce.resize(size);
    kPrev.resize(size,size);
    thetangStiffMat.resize(size,size);
    
    // check stiffness matrix size
    this->setInitialStiff();
    
    return OF_ReturnType_completed;
}


int ESFTangForceConverter::setInitialStiff()
{
    if (kInit.noRows() != size || kInit.noCols() != size)  {
        opserr << "ESFTangForceConverter::setInitialStiff() - " 
            << "matrix size is incorrect for tangent stiff: "
            << this->getTag() << endln;
        return -1;
    }
    
    // copy the stiffness matrix to kPrev
    int i, j;
    for (i=0; i<size; i++) {
        for (j=0; j<size; j++){
            kPrev(i,j) = kInit(i,j);
            thetangStiffMat(i,j) = kPrev(i,j);
        }
    }
    
    return OF_ReturnType_completed;
}


double ESFTangForceConverter::filtering(double data)
{
    if (firstWarning == true)  {
        opserr << "\nWARNING ESFTangForceConverter::filtering(double data) - " 
            << endln
            << "No filtering performed. The input data is returned instead." 
            << endln;

        firstWarning = false;
    }
    
    return data;
}


Vector& ESFTangForceConverter::converting(Vector* trialDisp)
{
    int i;
    for (i=0; i<size; i++) {
        incrDisp(i) = (*trialDisp)(i) - dispPast(i);
    }
    
    incrForce.addMatrixVector(0.0, thetangStiffMat, incrDisp, 1.0);
    
    for (i=0; i<size; i++) {
        convertFrc(i) = forcePast(i) + incrForce(i);
    }
    
    return convertFrc;
}


Vector& ESFTangForceConverter::converting(Vector* daqDisp, Vector* daqForce)
{
    this->updateMatrix(daqDisp, daqForce);
    int i;
    for (i=0; i<size; i++) {
        incrDisp(i) = (*daqDisp)(i) - dispPast(i);
    }
    
    incrForce.addMatrixVector(0.0, thetangStiffMat, incrDisp, 1.0);
    
    for (i=0; i<size; i++) {
        convertFrc(i) = forcePast(i) + incrForce(i);
    }
    
    // set the current values to past values
    for (i=0; i<size; i++) {
        dispPast(i) = (*daqDisp)(i);
        forcePast(i) = (*daqForce)(i);
    }
    
    return convertFrc;
}


ExperimentalSignalFilter* ESFTangForceConverter::getCopy()
{
    return new ESFTangForceConverter(*this);
}


void ESFTangForceConverter::Print(OPS_Stream &s, int flag)
{
    s << "Experimental Signal Filter: " << this->getTag(); 
    s << "  type: ESFTangForceConverter\n";
}


void ESFTangForceConverter::update()
{
    // does nothing
}


int ESFTangForceConverter::updateMatrix(const Vector* daqDisp, const Vector* daqForce)
{
    // calculate the incremental values
    int i, j;
    for (i=0; i<size; i++) {
        incrDisp(i) = (*daqDisp)(i) - dispPast(i);
        incrForce(i) = (*daqForce)(i) - forcePast(i);
    }
    
    thetangStiffMat = theTangStiff->updateTangentStiff(&incrDisp, 
        (Vector*)0, 
        (Vector*)0, 
        &incrForce, 
        (Vector*)0, 
        &kInit, 
        &kPrev);
    
    // copy the stiffness matrix to kPrev
    for (i=0; i<size; i++) {
        for (j=0; j<size; j++){
            kPrev(i,j) = thetangStiffMat(i,j);
        }
    }
    
    return OF_ReturnType_completed;
}
