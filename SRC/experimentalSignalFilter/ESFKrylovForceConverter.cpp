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
// ESFKrylovForceConverter. This class converts the trial displacements
// to trial forces and measured displacements to measured forces using 
// the Krylov subspaces.  It used the converting method and does nothing 
// with the filtering method.

#include "ESFKrylovForceConverter.h"

#include <elementAPI.h>

void* OPF_ESFKrylovForceConverter()
{
    // pointer to experimental control that will be returned
    ExperimentalSignalFilter* theFilter = 0;
    
    if (OPS_GetNumRemainingInputArgs() < 4) {
        opserr << "WARNING invalid number of arguments\n";
        opserr << "Want: expSignalFilter KrylovForceConverter tag "
            << "numSubspace -initStif Kij\n";
        return 0;
    }
    
    // filter tag
    int tag;
    int numdata = 1;
    if (OPS_GetIntInput(&numdata, &tag) != 0) {
        opserr << "WARNING invalid expSignalFilter KrylovForceConverter tag\n";
        return 0;
    }
    
    // number of subspaces
    int numSubspace;
    numdata = 1;
    if (OPS_GetIntInput(&numdata, &numSubspace) != 0) {
        opserr << "WARNING invalid numSubspace\n";
        opserr << "expSignalFilter KrylovForceConverter " << tag << endln;
        return 0;
    }
    
    // initial stiffness terms
    const char* type = OPS_GetString();
    if (strcmp(type, "-initStif") != 0 &&
        strcmp(type, "-initStiff") != 0) {
        opserr << "WARNING expecting -initStif Kij\n";
        opserr << "expSignalFilter KrylovForceConverter " << tag << endln;
        return 0;
    }
    // check size of stiffness matrix
    int numArg = OPS_GetNumRemainingInputArgs();
    double dim = sqrt(numArg);
    if (fmod(dim, 1.0) != 0.0) {
        opserr << "WARNING Kij is not a square matrix\n";
        opserr << "expSignalFilter KrylovForceConverter " << tag << endln;
        return 0;
    }
    // set the initial stiffness
    int numDOF = (int)dim;
    Matrix theInitStif(numDOF, numDOF);
    double stif[1024];
    numdata = numDOF * numDOF;
    if (OPS_GetDoubleInput(&numdata, stif) != 0) {
        opserr << "WARNING invalid initial stiffness terms\n";
        opserr << "expSignalFilter KrylovForceConverter " << tag << endln;
        return 0;
    }
    theInitStif.setData(stif, numDOF, numDOF);
    
    // parsing was successful, allocate the signal filter
    theFilter = new ESFKrylovForceConverter(tag, numSubspace, theInitStif);
    if (theFilter == 0) {
        opserr << "WARNING could not create experimental signal filter "
            << "of type ESFKrylovForceConverter\n";
        return 0;
    }
    
    return theFilter;
}


ESFKrylovForceConverter::ESFKrylovForceConverter(int tag,
    int ss, Matrix& initStif)
    : ExperimentalSignalFilter(tag), firstWarning(true),
    size(0), szSubspace(ss), dispPast(0), forcePast(0), convertFrc(0), incrDisp(0), 
    incrForce(0), cn(0), kInit(initStif), iDMatrix(0,0), iFMatrix(0,0)
{
    // does nothing
}


ESFKrylovForceConverter::ESFKrylovForceConverter(const ESFKrylovForceConverter& esf)
    : ExperimentalSignalFilter(esf), firstWarning(0), size(0), szSubspace(0), 
    dispPast(0), forcePast(0), convertFrc(0), incrDisp(0), incrForce(0), cn(0), kInit(0,0), 
    iDMatrix(0,0), iFMatrix(0,0)
{
    szSubspace = esf.szSubspace;
    firstWarning = esf.firstWarning;
    kInit = esf.kInit;
}


ESFKrylovForceConverter::~ESFKrylovForceConverter()
{
    // Does nothing
}


int ESFKrylovForceConverter::setSize(const int sz)
{
    size = sz;
    // resize variables
    convertFrc.resize(size);
    dispPast.resize(size);
    forcePast.resize(size);
    incrDisp.resize(size);
    incrForce.resize(size);
    
    // check stiffness matrix size
    this->setInitialStiff();
    
    return OF_ReturnType_completed;
}


int ESFKrylovForceConverter::setInitialStiff()
{
    if (kInit.noRows() != size || kInit.noCols() != size)  {
        opserr << "ESFKrylovForceConverter::setInitialStiff() - " 
            << "matrix size is incorrect for element: "
            << this->getTag() << endln;
        return -1;
    }
    
    return OF_ReturnType_completed;
}


double ESFKrylovForceConverter::filtering(double data)
{
    if (firstWarning == true)  {
        opserr << "\nWARNING ESFKrylovForceConverter::filtering(double data) - " 
            << endln
            << "No filtering performed. The input data is returned instead." 
            << endln;
        firstWarning = false;
    }
    
    return data;
}


Vector& ESFKrylovForceConverter::converting(Vector* trialDisp)
{
    int szCn = cn.Size();
    Matrix iDMatrixT;
    Vector v(size), incrV(size), incrP(size), incrQ(size);
    
    int i;
    for (i=0; i<size; i++)
        incrDisp(i) = (*trialDisp)(i) - dispPast(i);
    if (szCn == 0) {
        // initial condition
        convertFrc.addMatrixVector(0.0, kInit, incrDisp, 1.0);
    } else if (szCn <= size) {
        // least squares for over-determined system
        Matrix tempDTD(szCn, szCn);
        Vector incrDT(szCn);
        
        this->matTranspose(&iDMatrixT, &iDMatrix);
        tempDTD.addMatrixProduct(0.0, iDMatrixT, iDMatrix, 1.0);
        incrDT.addMatrixVector(0.0, iDMatrixT, incrDisp, 1.0);
        tempDTD.Solve(incrDT, cn);
        
        incrV.addMatrixVector(0.0, iDMatrix, cn, 1.0);
        for (i=0; i<size; i++)
            v(i) = incrDisp(i) - incrV(i);
        incrQ.addMatrixVector(0.0, kInit, v, 1.0);
        incrP.addMatrixVector(0.0, iFMatrix, cn, 1.0);
        
        for (i=0; i<size; i++)
            convertFrc(i) = forcePast(i) + incrP(i) + incrQ(i);
    } else {
        // Lagrange multiplier to find the min sol for under-determined system
        Matrix tempDDT(size, size);
        Vector incrDT(size);
        int i;
        for (i=0; i<size; i++)
            incrDisp(i) = (*trialDisp)(i) - dispPast(i);
        
        this->matTranspose(&iDMatrixT, &iDMatrix);
        tempDDT.addMatrixProduct(0.0, iDMatrix, iDMatrixT, 1.0);
        tempDDT.Solve(incrDisp, incrDT);
        cn.addMatrixVector(0.0, iDMatrix, incrDT, 1.0);
        
        incrV.addMatrixVector(0.0, iDMatrix, cn, 1.0);
        for (i=0; i<size; i++)
            v(i) = incrDisp(i) - incrV(i);
        incrQ.addMatrixVector(0.0, kInit, v, 1.0);
        incrP.addMatrixVector(0.0, iFMatrix, cn, 1.0);
        
        for (i=0; i<size; i++)
            convertFrc(i) = forcePast(i) + incrP(i) + incrQ(i);
    }
    
    return convertFrc;
}


Vector& ESFKrylovForceConverter::converting(Vector* daqDisp, Vector* daqForce)
{	
    convertFrc = this->converting(daqDisp);
    
    this->updateIncrMat(daqDisp, daqForce);
    
    // set the daq values to past values
    int i;
    for (i=0; i<size; i++) {
        dispPast(i) = (*daqDisp)(i);
        forcePast(i) = (*daqForce)(i);
    }
    
    return convertFrc;
}


ExperimentalSignalFilter* ESFKrylovForceConverter::getCopy()
{
    return new ESFKrylovForceConverter(*this);
}


void ESFKrylovForceConverter::Print(OPS_Stream &s, int flag)
{
    s << "Experimental Signal Filter: " << this->getTag(); 
    s << "  type: ESFKrylovForceConverter\n";
}


void ESFKrylovForceConverter::update()
{
    // does nothing
}


int ESFKrylovForceConverter::updateIncrMat(const Vector* daqDisp, const Vector* daqForce)
{
    // calculate the incremental values
    int i, j;
    for (i=0; i<size; i++) {
        incrDisp(i) = (*daqDisp)(i) - dispPast(i);
        incrForce(i) = (*daqForce)(i) - forcePast(i);
    }
    
    // using incremental disp and force
    int dimCiDM = iDMatrix.noCols();
    
    Matrix tempDM(size, dimCiDM);
    Matrix tempFM(size, dimCiDM);
    // FIX ME add size check
    
    // append the incremental vectors to matrices
    if (dimCiDM < szSubspace) {
        // copy all entries from the existing matrices
        for (i=0; i<size; i++) {
            for (j=0; j<dimCiDM; j++) {
                tempDM(i,j) = iDMatrix(i,j);
                tempFM(i,j) = iFMatrix(i,j);
            }
        }
        iDMatrix.resize(size, dimCiDM +1);
        iFMatrix.resize(size, dimCiDM +1);
        cn.resize(dimCiDM+1);
        for (i=0; i<size; i++) {
            for (j=0; j<dimCiDM; j++) {
                iDMatrix(i,j) = tempDM(i,j);
                iFMatrix(i,j) = tempFM(i,j);
            }
        }
        for (i=0; i<size; i++) {
            iDMatrix(i, dimCiDM) = incrDisp(i);
            iFMatrix(i, dimCiDM) = incrForce(i);
        }
    } else if (dimCiDM == szSubspace) {
        for (i=0; i<size; i++) {
            for (j=0; j<(dimCiDM-1); j++) {
                iDMatrix(i,j) = iDMatrix(i,j+1);
                iFMatrix(i,j) = iFMatrix(i,j+1);
            }
        }
        for (i=0; i<size; i++) {
            iDMatrix(i, dimCiDM-1) = incrDisp(i);
            iFMatrix(i, dimCiDM-1) = incrForce(i);
        }
    }
    
    return OF_ReturnType_completed;
}


int ESFKrylovForceConverter::matTranspose(Matrix* kT, const Matrix* k)
{
    int dimR = k->noRows();
    int dimC = k->noCols();
    kT->resize(dimC,dimR);
    
    int i, j;
    //transpose matrix
    for (i=0; i<dimC; i++) {
        for (j=0; j<dimR; j++) {
            (*kT)(i,j) = (*k)(j,i);
        }
    }
    
    return OF_ReturnType_completed;
}
