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

// $Revision$
// $Date$
// $URL$

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 01/14
// Revision: A
//
// Description: This file contains the implementation of the EEBearing2d class.

#include "EEBearing2d.h"

#include <Domain.h>
#include <Node.h>
#include <Channel.h>
#include <FEM_ObjectBroker.h>
#include <Renderer.h>
#include <Information.h>
#include <ElementResponse.h>
#include <UniaxialMaterial.h>
#include <TCP_Socket.h>
#include <TCP_SocketSSL.h>
#include <UDP_Socket.h>

#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>


// initialize the class wide variables
Matrix EEBearing2d::theMatrix(6,6);
Vector EEBearing2d::theVector(6);


// responsible for allocating the necessary space needed
// by each object and storing the tags of the end nodes.
EEBearing2d::EEBearing2d(int tag, int Nd1, int Nd2,
    int pfc, UniaxialMaterial **materials,
    ExperimentalSite *site, const Vector _y, const Vector _x,
    const Vector Mr, double sdI, bool iM, int addRay, double m)
    : ExperimentalElement(tag, ELE_TAG_EEBearing2d, site),
    connectedExternalNodes(2), pFrcCtrl(pfc), x(_x), y(_y), Mratio(Mr),
    shearDistI(sdI), iMod(iM), addRayleigh(addRay), mass(m), L(0.0),
    db(0), vb(0), ab(0), qb(0), t(0),
    dbDaq(0), vbDaq(0), abDaq(0), qbDaq(0), tDaq(0),
    dbCtrl(3), vbCtrl(3), abCtrl(3),
    dl(6), Tgl(6,6), Tlb(3,6), dbLast(3), tLast(0.0), kbInit(3,3),
    theLoad(6), firstWarning(true), onP0(true)
{
    // ensure the connectedExternalNode ID is of correct size & set values
    if (connectedExternalNodes.Size() != 2)  {
        opserr << "EEBearing2d::EEBearing2d() - element: "
            << this->getTag() << " failed to create an ID of size 2.\n";
        exit(OF_ReturnType_failed);
    }
    
    connectedExternalNodes(0) = Nd1;
    connectedExternalNodes(1) = Nd2;
    
    // set node pointers to NULL
    for (int i=0; i<2; i++)
        theNodes[i] = 0;
    
    // check material input
    if (materials == 0)  {
        opserr << "EEBearing2d::EEBearing2d() - "
            << "null material array passed.\n";
        exit(OF_ReturnType_failed);
    }

    // get copies of the uniaxial materials
    for (int i=0; i<2; i++)  {
        if (materials[i] == 0) {
            opserr << "EEBearing2d::EEBearing2d() - "
                "null uniaxial material pointer passed.\n";
            exit(OF_ReturnType_failed);
        }
        theMaterials[i] = materials[i]->getCopy();
        if (theMaterials[i] == 0) {
            opserr << "EEBearing2d::EEBearing2d() - "
                << "failed to copy uniaxial material.\n";
            exit(OF_ReturnType_failed);
        }
    }
    
    // check p-delta moment distribution ratios
    if (Mratio.Size() == 2)  {
        if (Mratio(0)+Mratio(1) > 1.0)  {
            opserr << "EEBearing2d::EEBearing2d() - "
                << "incorrect p-delta moment ratios:\nrMz1 + rMz2 = "
                << Mratio(0)+Mratio(1) << " > 1.0\n";
            exit(OF_ReturnType_failed);
        }
    }
    
    // set the data size for the experimental site
    int numDir = 3;
    sizeCtrl = new ID(OF_Resp_All);
    sizeDaq = new ID(OF_Resp_All);
    
    (*sizeCtrl)[OF_Resp_Disp]  = numDir;
    (*sizeCtrl)[OF_Resp_Vel]   = numDir;
    (*sizeCtrl)[OF_Resp_Accel] = numDir;
    (*sizeCtrl)[OF_Resp_Force] = numDir*pFrcCtrl;
    (*sizeCtrl)[OF_Resp_Time]  = 1;
    
    (*sizeDaq)[OF_Resp_Disp]   = numDir;
    (*sizeDaq)[OF_Resp_Vel]    = numDir;
    (*sizeDaq)[OF_Resp_Accel]  = numDir;
    (*sizeDaq)[OF_Resp_Force]  = numDir;
    (*sizeDaq)[OF_Resp_Time]   = 1;
    
    theSite->setSize(*sizeCtrl, *sizeDaq);
    
    // allocate memory for trial response vectors
    db = new Vector(numDir);
    vb = new Vector(numDir);
    ab = new Vector(numDir);
    qb = new Vector(numDir*pFrcCtrl);
    t  = new Vector(1);
    
    // allocate memory for daq response vectors
    dbDaq = new Vector(numDir);
    vbDaq = new Vector(numDir);
    abDaq = new Vector(numDir);
    qbDaq = new Vector(numDir);
    tDaq  = new Vector(1);
    
    // initialize additional vectors
    dbCtrl.Zero();
    vbCtrl.Zero();
    abCtrl.Zero();
    dbLast.Zero();
}


// responsible for allocating the necessary space needed
// by each object and storing the tags of the end nodes.
EEBearing2d::EEBearing2d(int tag, int Nd1, int Nd2,
    int pfc, UniaxialMaterial **materials,
    int port, char *machineInetAddr, int ssl, int udp, int dataSize,
    const Vector _y, const Vector _x,
    const Vector Mr, double sdI, bool iM, int addRay, double m)
    : ExperimentalElement(tag, ELE_TAG_EEBearing2d),
    connectedExternalNodes(2), pFrcCtrl(pfc), x(_x), y(_y), Mratio(Mr),
    shearDistI(sdI), iMod(iM), addRayleigh(addRay), mass(m), L(0.0),
    theChannel(0), sData(0), sendData(0), rData(0), recvData(0),
    db(0), vb(0), ab(0), qb(0), t(0),
    dbDaq(0), vbDaq(0), abDaq(0), qbDaq(0), tDaq(0),
    dbCtrl(3), vbCtrl(3), abCtrl(3),
    dl(6), Tgl(6,6), Tlb(3,6), dbLast(3), tLast(0.0), kbInit(3,3),
    theLoad(6), firstWarning(true), onP0(true)
{
    // ensure the connectedExternalNode ID is of correct size & set values
    if (connectedExternalNodes.Size() != 2)  {
        opserr << "EEBearing2d::EEBearing2d() - element: "
            << this->getTag() << " failed to create an ID of size 2.\n";
        exit(OF_ReturnType_failed);
    }
    
    connectedExternalNodes(0) = Nd1;
    connectedExternalNodes(1) = Nd2;
    
    // set node pointers to NULL
    for (int i=0; i<2; i++)
        theNodes[i] = 0;
    
     // check material input
    if (materials == 0)  {
        opserr << "EEBearing2d::EEBearing2d() - "
            << "null material array passed.\n";
        exit(OF_ReturnType_failed);
    }
    
    // get copies of the uniaxial materials
    for (int i=0; i<2; i++)  {
        if (materials[i] == 0) {
            opserr << "EEBearing2d::EEBearing2d() - "
                "null uniaxial material pointer passed.\n";
            exit(OF_ReturnType_failed);
        }
        theMaterials[i] = materials[i]->getCopy();
        if (theMaterials[i] == 0) {
            opserr << "EEBearing2d::EEBearing2d() - "
                << "failed to copy uniaxial material.\n";
            exit(OF_ReturnType_failed);
        }
    }
    
    // check p-delta moment distribution ratios
    if (Mratio.Size() == 2)  {
        if (Mratio(0)+Mratio(1) > 1.0)  {
            opserr << "EEBearing2d::EEBearing2d() - "
                << "incorrect p-delta moment ratios:\nrMz1 + rMz2 = "
                << Mratio(0)+Mratio(1) << " > 1.0\n";
            exit(OF_ReturnType_failed);
        }
    }
    
    // setup the connection
    if (ssl)  {
        if (machineInetAddr == 0)
            theChannel = new TCP_SocketSSL(port, "127.0.0.1");
        else
            theChannel = new TCP_SocketSSL(port, machineInetAddr);
    }
    else if (udp)  {
        if (machineInetAddr == 0)
            theChannel = new UDP_Socket(port, "127.0.0.1");
        else
            theChannel = new UDP_Socket(port, machineInetAddr);
    }
    else  {
        if (machineInetAddr == 0)
            theChannel = new TCP_Socket(port, "127.0.0.1");
        else
            theChannel = new TCP_Socket(port, machineInetAddr);
    }
    if (!theChannel)  {
        opserr << "EEBearing2d::EEBearing2d() "
            << "- failed to create channel.\n";
        exit(OF_ReturnType_failed);
    }
    if (theChannel->setUpConnection() != 0)  {
        opserr << "EEBearing2d::EEBearing2d() "
            << "- failed to setup connection.\n";
        exit(OF_ReturnType_failed);
    }
    delete [] machineInetAddr;
    
    // set the data size for the experimental site
    int numDir = 3;
    int intData[2*OF_Resp_All+1];
    ID idData(intData, 2*OF_Resp_All+1);
    sizeCtrl = new ID(intData, OF_Resp_All);
    sizeDaq = new ID(&intData[OF_Resp_All], OF_Resp_All);
    idData.Zero();
    
    (*sizeCtrl)[OF_Resp_Disp]  = numDir;
    (*sizeCtrl)[OF_Resp_Vel]   = numDir;
    (*sizeCtrl)[OF_Resp_Accel] = numDir;
    (*sizeCtrl)[OF_Resp_Force] = numDir*pFrcCtrl;
    (*sizeCtrl)[OF_Resp_Time]  = 1;
    
    (*sizeDaq)[OF_Resp_Disp]   = numDir;
    (*sizeDaq)[OF_Resp_Vel]    = numDir;
    (*sizeDaq)[OF_Resp_Accel]  = numDir;
    (*sizeDaq)[OF_Resp_Force]  = numDir;
    (*sizeDaq)[OF_Resp_Time]   = 1;
    
    if (dataSize < 4*numDir+1) dataSize = 4*numDir+1;
    intData[2*OF_Resp_All] = dataSize;
    
    theChannel->sendID(0, 0, idData, 0);
    
    // allocate memory for the send vectors
    int id = 1;
    sData = new double [dataSize];
    sendData = new Vector(sData, dataSize);
    db = new Vector(&sData[id], numDir);
    id += numDir;
    vb = new Vector(&sData[id], numDir);
    id += numDir;
    ab = new Vector(&sData[id], numDir);
    id += numDir;
    qb = new Vector(&sData[id], numDir*pFrcCtrl);
    id += numDir*pFrcCtrl;
    t = new Vector(&sData[id], 1);
    sendData->Zero();
    
    // allocate memory for the receive vectors
    id = 0;
    rData = new double [dataSize];
    recvData = new Vector(rData, dataSize);
    dbDaq = new Vector(&rData[id], numDir);
    id += numDir;
    vbDaq = new Vector(&rData[id], numDir);
    id += numDir;
    abDaq = new Vector(&rData[id], numDir);
    id += numDir;
    qbDaq = new Vector(&rData[id], numDir);
    id += numDir;
    tDaq = new Vector(&rData[id], 1);
    recvData->Zero();
    
    // initialize additional vectors
    dbCtrl.Zero();
    vbCtrl.Zero();
    abCtrl.Zero();
    dbLast.Zero();
}


// delete must be invoked on any objects created by the object.
EEBearing2d::~EEBearing2d()
{
    // invoke the destructor on any objects created by the object
    // that the object still holds a pointer to
    for (int i=0; i<2; i++)
        if (theMaterials[i] != 0)
            delete theMaterials[i];
    
    if (db != 0)
        delete db;
    if (vb != 0)
        delete vb;
    if (ab != 0)
        delete ab;
    if (qb != 0)
        delete qb;
    if (t != 0)
        delete t;
    
    if (dbDaq != 0)
        delete dbDaq;
    if (vbDaq != 0)
        delete vbDaq;
    if (abDaq != 0)
        delete abDaq;
    if (qbDaq != 0)
        delete qbDaq;
    if (tDaq != 0)
        delete tDaq;
    
    if (theSite == 0)  {
        sData[0] = OF_RemoteTest_DIE;
        theChannel->sendVector(0, 0, *sendData, 0);
        
        if (sendData != 0)
            delete sendData;
        if (sData != 0)
            delete [] sData;
        if (recvData != 0)
            delete recvData;
        if (rData != 0)
            delete [] rData;
        if (theChannel != 0)
            delete theChannel;
    }
}


int EEBearing2d::getNumExternalNodes() const
{
    return 2;
}


const ID& EEBearing2d::getExternalNodes()
{
    return connectedExternalNodes;
}


Node** EEBearing2d::getNodePtrs()
{
    return theNodes;
}


int EEBearing2d::getNumDOF()
{
    return 6;
}


int EEBearing2d::getNumBasicDOF()
{
    return 3;
}


// to set a link to the enclosing Domain and to set the node pointers.
void EEBearing2d::setDomain(Domain *theDomain)
{
    // check Domain is not null - invoked when object removed from a domain
    if (!theDomain)  {
        theNodes[0] = 0;
        theNodes[1] = 0;
        
        return;
    }
    
    // first set the node pointers
    theNodes[0] = theDomain->getNode(connectedExternalNodes(0));
    theNodes[1] = theDomain->getNode(connectedExternalNodes(1));
    
    // if can't find both - send a warning message
    if (!theNodes[0] || !theNodes[1])  {
        if (!theNodes[0])  {
            opserr << "EEBearing2d::setDomain() - Nd1: " 
                << connectedExternalNodes(0)
                << " does not exist in the model for ";
        } else  {
            opserr << "EEBearing2d::setDomain() - Nd2: " 
                << connectedExternalNodes(1)
                << " does not exist in the model for ";
        }
        opserr << "EEBearing2d ele: " << this->getTag() << ".\n";
        
        return;
    }
    
    // now determine the number of dof and the dimension
    int dofNd1 = theNodes[0]->getNumberDOF();
    int dofNd2 = theNodes[1]->getNumberDOF();
    
    // if differing dof at the ends - print a warning message
    if (dofNd1 != 3)  {
        opserr << "EEBearing2d::setDomain() - node 1: "
            << connectedExternalNodes(0)
            << " has incorrect number of DOF (not 3).\n";
        return;
    }
    if (dofNd2 != 3)  {
        opserr << "EEBearing2d::setDomain() - node 2: "
            << connectedExternalNodes(1)
            << " has incorrect number of DOF (not 3).\n";
        return;
    }
    
    // call the base class method
    this->DomainComponent::setDomain(theDomain);
    
    // setup the transformation matrix for orientation
    this->setUp();
}


int EEBearing2d::commitState()
{
    int rValue = 0;
    
    // commit the site
    if (theSite != 0)  {
        rValue += theSite->commitState(t);
    }
    else  {
        sData[0] = OF_RemoteTest_commitState;
        rValue += theChannel->sendVector(0, 0, *sendData, 0);
    }
    
    // commit material models
    for (int i=0; i<2; i++)
        rValue += theMaterials[i]->commitState();

    // commit the base class
    rValue += this->Element::commitState();
    
    return rValue;
}


int EEBearing2d::update()
{
    int rValue = 0;
    
    // get current time
    Domain *theDomain = this->getDomain();
    (*t)(0) = theDomain->getCurrentTime();
    
    // get global trial response
    const Vector &dsp1 = theNodes[0]->getTrialDisp();
    const Vector &dsp2 = theNodes[1]->getTrialDisp();
    const Vector &vel1 = theNodes[0]->getTrialVel();
    const Vector &vel2 = theNodes[1]->getTrialVel();
    const Vector &acc1 = theNodes[0]->getTrialAccel();
    const Vector &acc2 = theNodes[1]->getTrialAccel();
    
    static Vector dg(6), vg(6), ag(6), vl(6), al(6);
    for (int i=0; i<3; i++)  {
        dg(i)   = dsp1(i);  vg(i)   = vel1(i);  ag(i)   = acc1(i);
        dg(i+3) = dsp2(i);  vg(i+3) = vel2(i);  ag(i+3) = acc2(i);
    }
    
    // transform response from the global to the local system
    dl.addMatrixVector(0.0, Tgl, dg, 1.0);
    vl.addMatrixVector(0.0, Tgl, vg, 1.0);
    al.addMatrixVector(0.0, Tgl, ag, 1.0);
    
    // transform response from the local to the basic system
    db->addMatrixVector(0.0, Tlb, dl, 1.0);
    vb->addMatrixVector(0.0, Tlb, vl, 1.0);
    ab->addMatrixVector(0.0, Tlb, al, 1.0);
    
    // 1) set axial deformations in basic x-direction
    theMaterials[0]->setTrialStrain((*db)(0), (*vb)(0));
    if (pFrcCtrl == 1)
        (*qb)(0) = theMaterials[0]->getStress();
    
    // 2) set shear deformations in basic y-direction
    Vector dbDelta = (*db) - dbLast;
    // do not check time for right now because of transformation constraint
    // handler calling update at beginning of new step when applying load
    // if (dbDelta.pNorm(2) > DBL_EPSILON || (*t)(0) > tLast)  {
    if (dbDelta.pNorm(2) > DBL_EPSILON)  {
        // set the trial response at the site
        if (theSite != 0)  {
            theSite->setTrialResponse(db, vb, ab, qb, t);
        }
        else  {
            sData[0] = OF_RemoteTest_setTrialResponse;
            rValue += theChannel->sendVector(0, 0, *sendData, 0);
        }
    }
    
    // 3) set rotations about basic z-direction
    theMaterials[1]->setTrialStrain((*db)(2), (*vb)(2));
    
    // save the last displacements and time
    dbLast = (*db);
    tLast = (*t)(0);
    
    return rValue;
}


int EEBearing2d::setInitialStiff(const Matrix& kbinit)
{
    // set initial stiffness matrix in basic system
    if (kbinit.noRows() != 1 || kbinit.noCols() != 1)  {
        opserr << "EEBearing2d::setInitialStiff() - " 
            << "matrix size is incorrect for element: "
            << this->getTag() << ".\n";
        return OF_ReturnType_failed;
    }
    kbInit(0,0) = theMaterials[0]->getInitialTangent();
    kbInit(1,1) = kbinit(0,0);
    kbInit(2,2) = theMaterials[1]->getInitialTangent();
    
    // zero the global matrix
    theInitStiff.Zero();
    
    // transform from basic to local system
    static Matrix klInit(6,6);
    klInit.addMatrixTripleProduct(0.0, Tlb, kbInit, 1.0);
    
    // transform from local to global system
    theInitStiff.addMatrixTripleProduct(0.0, Tgl, klInit, 1.0);
    
    return OF_ReturnType_completed;
}


const Matrix& EEBearing2d::getTangentStiff()
{
    if (firstWarning == true)  {
        opserr << "\nWARNING EEBearing2d::getTangentStiff() - "
            << "Element: " << this->getTag() << endln
            << "TangentStiff cannot be calculated." << endln
            << "Return InitialStiff including GeometricStiff instead." 
            << endln;
        opserr << "Subsequent getTangentStiff warnings will be suppressed." 
            << endln;
        
        firstWarning = false;
    }
    
    // zero the global matrix
    theMatrix.Zero();
    
    // get stiffness matrix in basic system
    static Matrix kb(3,3);
    kb.Zero();
    kb(0,0) = theMaterials[0]->getTangent();
    kb(1,1) = kbInit(1,1);
    kb(2,2) = theMaterials[1]->getTangent();
    
    // transform from basic to local system
    static Matrix kl(6,6);
    kl.addMatrixTripleProduct(0.0, Tlb, kb, 1.0);
    
    if (Mratio.Size() == 2)  {
        // get daq resisting forces in basic system
        if (theSite != 0)  {
            (*qbDaq) = theSite->getForce();
        }
        else  {
            sData[0] = OF_RemoteTest_getForce;
            theChannel->sendVector(0, 0, *sendData, 0);
            theChannel->recvVector(0, 0, *recvData, 0);
        }
        
        // apply optional initial stiffness modification
        if (iMod == true)
            this->applyIMod();
        
        // use material force if force from test is zero
        if ((*qbDaq)(0) == 0.0)
            (*qbDaq)(0) = theMaterials[0]->getStress();
        if ((*qbDaq)(2) == 0.0)
            (*qbDaq)(2) = theMaterials[1]->getStress();
        
        // add geometric stiffness to local stiffness
        this->addPDeltaStiff(kl);
    }
    
    // transform from local to global system
    theMatrix.addMatrixTripleProduct(0.0, Tgl, kl, 1.0);
    
    return theMatrix;
}


const Matrix& EEBearing2d::getDamp()
{
    // zero the global matrix
    theMatrix.Zero();
    
    // call base class to setup Rayleigh damping
    double factThis = 0.0;
    if (addRayleigh == 1)  {
        theMatrix = this->Element::getDamp();
        factThis = 1.0;
    }
    
    // now add damping tangent from materials
    static Matrix cb(3,3);
    cb.Zero();
    cb(0,0) = theMaterials[0]->getDampTangent();
    cb(2,2) = theMaterials[1]->getDampTangent();
    
    // transform from basic to local system
    static Matrix cl(6,6);
    cl.addMatrixTripleProduct(0.0, Tlb, cb, 1.0);
    
    // transform from local to global system and add to cg
    theMatrix.addMatrixTripleProduct(factThis, Tgl, cl, 1.0);
    
    return theMatrix;
}


const Matrix& EEBearing2d::getMass()
{
    // zero the global matrix
    theMatrix.Zero();
    
    // form mass matrix
    if (mass != 0.0)  {
        double m = 0.5*mass;
        for (int i=0; i<2; i++)  {
            theMatrix(i,i)     = m;
            theMatrix(i+3,i+3) = m;
        }
    }
    
    return theMatrix;
}


void EEBearing2d::zeroLoad()
{
    theLoad.Zero();
}


int EEBearing2d::addLoad(ElementalLoad *theLoad, double loadFactor)
{
    opserr <<"EEBearing2d::addLoad() - "
        << "load type unknown for element: "
        << this->getTag() << ".\n";
    
    return OF_ReturnType_failed;
}


int EEBearing2d::addInertiaLoadToUnbalance(const Vector &accel)
{
    // check for quick return
    if (mass == 0.0)  {
        return 0;
    }    
    
    // get R * accel from the nodes
    const Vector &Raccel1 = theNodes[0]->getRV(accel);
    const Vector &Raccel2 = theNodes[1]->getRV(accel);
    
    if (3 != Raccel1.Size() || 3 != Raccel2.Size())  {
        opserr << "EEBearing2d::addInertiaLoadToUnbalance() - "
            << "matrix and vector sizes are incompatible.\n";
        return OF_ReturnType_failed;
    }
    
    // want to add ( - fact * M R * accel ) to unbalance
    // take advantage of lumped mass matrix
    double m = 0.5*mass;
    for (int i=0; i<2; i++)  {
        theLoad(i)   -= m * Raccel1(i);
        theLoad(i+3) -= m * Raccel2(i);
    }
    
    return OF_ReturnType_completed;
}


const Vector& EEBearing2d::getResistingForce()
{
    // zero the global residual
    theVector.Zero();
    
    // get daq resisting forces
    if (theSite != 0)  {
        (*qbDaq) = theSite->getForce();
    }
    else  {
        sData[0] = OF_RemoteTest_getForce;
        theChannel->sendVector(0, 0, *sendData, 0);
        theChannel->recvVector(0, 0, *recvData, 0);
    }
    
    // apply optional initial stiffness modification
    if (iMod == true)
        this->applyIMod();
    
    // use material force if force from test is zero
    if ((*qbDaq)(0) == 0.0)
        (*qbDaq)(0) = theMaterials[0]->getStress();
    if ((*qbDaq)(2) == 0.0)
        (*qbDaq)(2) = theMaterials[1]->getStress();
    
    // save corresponding ctrl displacements for recorder
    dbCtrl = (*db);
    vbCtrl = (*vb);
    abCtrl = (*ab);
    
    // determine resisting forces in local system
    static Vector ql(6);
    ql.addMatrixTransposeVector(0.0, Tlb, *qbDaq, 1.0);
    
    // add P-Delta effects to local forces
    if (Mratio.Size() == 2)
        this->addPDeltaForces(ql);
    
    // determine resisting forces in global system
    theVector.addMatrixTransposeVector(0.0, Tgl, ql, 1.0);
    
    // subtract external load
    theVector.addVector(1.0, theLoad, -1.0);
    
    return theVector;
}


const Vector& EEBearing2d::getResistingForceIncInertia()
{
    // this already includes damping forces from specimen
    theVector = this->getResistingForce();
    
    // add the damping forces from rayleigh damping
    if (addRayleigh == 1)  {
        if (alphaM != 0.0 || betaK != 0.0 || betaK0 != 0.0 || betaKc != 0.0)
            theVector.addVector(1.0, this->getRayleighDampingForces(), 1.0);
    }
    
    // add inertia forces from element mass
    if (mass != 0.0)  {
        const Vector &accel1 = theNodes[0]->getTrialAccel();
        const Vector &accel2 = theNodes[1]->getTrialAccel();
        
        double m = 0.5*mass;
        for (int i=0; i<2; i++)  {
            theVector(i)   += m * accel1(i);
            theVector(i+3) += m * accel2(i);
        }
    }
    
    return theVector;
}


const Vector& EEBearing2d::getTime()
{
    if (theSite != 0)  {
        (*tDaq) = theSite->getTime();
    }
    else  {
        sData[0] = OF_RemoteTest_getTime;
        theChannel->sendVector(0, 0, *sendData, 0);
        theChannel->recvVector(0, 0, *recvData, 0);
    }
    
    return *tDaq;
}


const Vector& EEBearing2d::getBasicDisp()
{
    if (theSite != 0)  {
        (*dbDaq) = theSite->getDisp();
    }
    else  {
        sData[0] = OF_RemoteTest_getDisp;
        theChannel->sendVector(0, 0, *sendData, 0);
        theChannel->recvVector(0, 0, *recvData, 0);
    }
    
    return *dbDaq;
}


const Vector& EEBearing2d::getBasicVel()
{
    if (theSite != 0)  {
        (*vbDaq) = theSite->getVel();
    }
    else  {
        sData[0] = OF_RemoteTest_getVel;
        theChannel->sendVector(0, 0, *sendData, 0);
        theChannel->recvVector(0, 0, *recvData, 0);
    }
    
    return *vbDaq;
}


const Vector& EEBearing2d::getBasicAccel()
{
    if (theSite != 0)  {
        (*abDaq) = theSite->getAccel();
    }
    else  {
        sData[0] = OF_RemoteTest_getAccel;
        theChannel->sendVector(0, 0, *sendData, 0);
        theChannel->recvVector(0, 0, *recvData, 0);
    }
    
    return *abDaq;
}


int EEBearing2d::sendSelf(int commitTag, Channel &theChannel)
{
    // has not been implemented yet.....
    return OF_ReturnType_completed;
}


int EEBearing2d::recvSelf(int commitTag, Channel &theChannel,
    FEM_ObjectBroker &theBroker)
{
    // has not been implemented yet.....
    onP0 = false;

    return OF_ReturnType_completed;
}


int EEBearing2d::displaySelf(Renderer &theViewer,
    int displayMode, float fact)
{
    // first determine the end points of the element based on
    // the display factor (a measure of the distorted image)
    const Vector &end1Crd = theNodes[0]->getCrds();
    const Vector &end2Crd = theNodes[1]->getCrds();
    
    static Vector v1(3);
    static Vector v2(3);
    
    if (displayMode >= 0)  {
        const Vector &end1Disp = theNodes[0]->getDisp();
        const Vector &end2Disp = theNodes[1]->getDisp();
        
        for (int i=0; i<2; i++)  {
            v1(i) = end1Crd(i) + end1Disp(i)*fact;
            v2(i) = end2Crd(i) + end2Disp(i)*fact;
        }
    } else  {
        int mode = displayMode * -1;
        const Matrix &eigen1 = theNodes[0]->getEigenvectors();
        const Matrix &eigen2 = theNodes[1]->getEigenvectors();
        
        if (eigen1.noCols() >= mode)  {
            for (int i=0; i<2; i++)  {
                v1(i) = end1Crd(i) + eigen1(i,mode-1)*fact;
                v2(i) = end2Crd(i) + eigen2(i,mode-1)*fact;
            }
        } else  {
            for (int i=0; i<2; i++)  {
                v1(i) = end1Crd(i);
                v2(i) = end2Crd(i);
            }
        }
    }
    
    return theViewer.drawLine (v1, v2, 1.0, 1.0);
}


void EEBearing2d::Print(OPS_Stream &s, int flag)
{
    if (flag == 0)  {
        // print everything
        s << "Element: " << this->getTag() << endln;
        s << "  type: EEBearing2d" << endln;
        s << "  iNode: " << connectedExternalNodes(0) 
            << ", jNode: " << connectedExternalNodes(1) << endln;
        s << "  Material ux: " << theMaterials[0]->getTag();
        s << "  Material rz: " << theMaterials[1]->getTag() << endln;
        if (theSite != 0)
            s << "  ExperimentalSite: " << theSite->getTag() << endln;
        s << "  Mratio: " << Mratio << "  shearDistI: " << shearDistI << endln;
        s << "  addRayleigh: " << addRayleigh << "  mass: " << mass << endln;
        // determine resisting forces in global system
        s << "  resisting force: " << this->getResistingForce() << endln;
    } else if (flag == 1)  {
        // does nothing
    }
}


Response* EEBearing2d::setResponse(const char **argv, int argc,
    OPS_Stream &output)
{
    Response *theResponse = 0;
    
    output.tag("ElementOutput");
    output.attr("eleType","EEBearing2d");
    output.attr("eleTag",this->getTag());
    output.attr("node1",connectedExternalNodes[0]);
    output.attr("node2",connectedExternalNodes[1]);
    
    // global forces
    if (strcmp(argv[0],"force") == 0 || strcmp(argv[0],"forces") == 0 ||
        strcmp(argv[0],"globalForce") == 0 || strcmp(argv[0],"globalForces") == 0)
    {
        output.tag("ResponseType","Px_1");
        output.tag("ResponseType","Py_1");
        output.tag("ResponseType","Mz_1");
        output.tag("ResponseType","Px_2");
        output.tag("ResponseType","Py_2");
        output.tag("ResponseType","Mz_2");
        
        theResponse = new ElementResponse(this, 1, theVector);
    }
    
    // local forces
    else if (strcmp(argv[0],"localForce") == 0 || strcmp(argv[0],"localForces") == 0)
    {
        output.tag("ResponseType","N_1");
        output.tag("ResponseType","V_1");
        output.tag("ResponseType","M_1");
        output.tag("ResponseType","N_2");
        output.tag("ResponseType","V_2");
        output.tag("ResponseType","M_2");
        
        theResponse = new ElementResponse(this, 2, theVector);
    }
    
    // basic forces
    else if (strcmp(argv[0],"basicForce") == 0 || strcmp(argv[0],"basicForces") == 0)
    {
        output.tag("ResponseType","qb1");
        output.tag("ResponseType","qb2");
        output.tag("ResponseType","qb3");
        
        theResponse = new ElementResponse(this, 3, Vector(3));
    }
    
    // ctrl local displacements
    else if (strcmp(argv[0],"localDisp") == 0 ||
        strcmp(argv[0],"localDisplacement") == 0 ||
        strcmp(argv[0],"localDisplacements") == 0)
    {
        output.tag("ResponseType","ux_1");
        output.tag("ResponseType","uy_1");
        output.tag("ResponseType","rz_1");
        output.tag("ResponseType","ux_2");
        output.tag("ResponseType","uy_2");
        output.tag("ResponseType","rz_2");
        
        theResponse = new ElementResponse(this, 4, theVector);
    }
    
    // ctrl basic displacements
    else if (strcmp(argv[0],"defo") == 0 ||
        strcmp(argv[0],"deformation") == 0 ||
        strcmp(argv[0],"deformations") == 0 ||
        strcmp(argv[0],"basicDefo") == 0 ||
        strcmp(argv[0],"basicDeformation") == 0 ||
        strcmp(argv[0],"basicDeformations") == 0 ||
        strcmp(argv[0],"ctrlDisp") == 0 ||
        strcmp(argv[0],"ctrlDisplacement") == 0 ||
        strcmp(argv[0],"ctrlDisplacements") == 0)
    {
        output.tag("ResponseType","db1");
        output.tag("ResponseType","db2");
        output.tag("ResponseType","db3");
        
        theResponse = new ElementResponse(this, 5, Vector(3));
    }
    
    // ctrl basic velocities
    else if (strcmp(argv[0],"ctrlVel") == 0 ||
        strcmp(argv[0],"ctrlVelocity") == 0 ||
        strcmp(argv[0],"ctrlVelocities") == 0)
    {
        output.tag("ResponseType","vb1");
        output.tag("ResponseType","vb2");
        output.tag("ResponseType","vb3");
        
        theResponse = new ElementResponse(this, 6, Vector(3));
    }
    
    // ctrl basic accelerations
    else if (strcmp(argv[0],"ctrlAccel") == 0 ||
        strcmp(argv[0],"ctrlAcceleration") == 0 ||
        strcmp(argv[0],"ctrlAccelerations") == 0)
    {
        output.tag("ResponseType","ab1");
        output.tag("ResponseType","ab2");
        output.tag("ResponseType","ab3");
        
        theResponse = new ElementResponse(this, 7, Vector(3));
    }
    
    // daq basic displacements
    else if (strcmp(argv[0],"daqDisp") == 0 ||
        strcmp(argv[0],"daqDisplacement") == 0 ||
        strcmp(argv[0],"daqDisplacements") == 0)
    {
        output.tag("ResponseType","dbDaq1");
        output.tag("ResponseType","dbDaq2");
        output.tag("ResponseType","dbDaq3");
        
        theResponse = new ElementResponse(this, 8, Vector(3));
    }
    
    // daq basic velocities
    else if (strcmp(argv[0],"daqVel") == 0 ||
        strcmp(argv[0],"daqVelocity") == 0 ||
        strcmp(argv[0],"daqVelocities") == 0)
    {
        output.tag("ResponseType","vbDaq1");
        output.tag("ResponseType","vbDaq2");
        output.tag("ResponseType","vbDaq3");
        
        theResponse = new ElementResponse(this, 9, Vector(3));
    }
    
    // daq basic accelerations
    else if (strcmp(argv[0],"daqAccel") == 0 ||
        strcmp(argv[0],"daqAcceleration") == 0 ||
        strcmp(argv[0],"daqAccelerations") == 0)
    {
        output.tag("ResponseType","abDaq1");
        output.tag("ResponseType","abDaq2");
        output.tag("ResponseType","abDaq3");
        
        theResponse = new ElementResponse(this, 10, Vector(3));
    }
    
    // material output
    else if (strcmp(argv[0],"material") == 0)  {
        if (argc > 2)  {
            int matNum = atoi(argv[1]);
            if (matNum >= 1 && matNum <= 2)
                theResponse =  theMaterials[matNum-1]->setResponse(&argv[2], argc-2, output);
        }
    }
    
    output.endTag(); // ElementOutput
    
    return theResponse;
}


int EEBearing2d::getResponse(int responseID, Information &eleInfo)
{
    switch (responseID)  {
    case 1:  // global forces
        return eleInfo.setVector(this->getResistingForce());
        
    case 2:  // local forces
        theVector.Zero();
        // determine resisting forces in local system
        theVector.addMatrixTransposeVector(0.0, Tlb, *qbDaq, 1.0);
        // add P-Delta effects to local forces
        if (Mratio.Size() == 2)
            this->addPDeltaForces(theVector);
        return eleInfo.setVector(theVector);
        
    case 3:  // basic forces
        return eleInfo.setVector(*qbDaq);
        
    case 4:  // ctrl local displacements
        return eleInfo.setVector(dl);
        
    case 5:  // ctrl basic displacements
        return eleInfo.setVector(dbCtrl);
        
    case 6:  // ctrl basic velocities
        return eleInfo.setVector(vbCtrl);
        
    case 7:  // ctrl basic accelerations
        return eleInfo.setVector(abCtrl);
        
    case 8:  // daq basic displacements
        return eleInfo.setVector(this->getBasicDisp());
        
    case 9:  // daq basic velocities
        return eleInfo.setVector(this->getBasicVel());
        
    case 10:  // daq basic accelerations
        return eleInfo.setVector(this->getBasicAccel());
        
    default:
        return 0;
    }
}


// set up the transformation matrix for orientation
void EEBearing2d::setUp()
{
    const Vector &end1Crd = theNodes[0]->getCrds();
    const Vector &end2Crd = theNodes[1]->getCrds();	
    Vector xp = end2Crd - end1Crd;
    L = xp.Norm();
    
    // setup x and y orientation vectors
    if (L > DBL_EPSILON)  {
        if (x.Size() == 0)  {
            x.resize(3);
            x(0) = xp(0);  x(1) = xp(1);  x(2) = 0.0;
            y.resize(3);
            y(0) = -x(1);  y(1) = x(0);  y(2) = 0.0;
        } else if (onP0)  {
            opserr << "WARNING EEBearing2d::setUp() - " 
                << "element: " << this->getTag()
                << " - ignoring nodes and using specified "
                << "local x vector to determine orientation.\n";
        }
    }
    // check that vectors for orientation are of correct size
    if (x.Size() != 3 || y.Size() != 3)  {
        opserr << "EEBearing2d::setUp() - "
            << "element: " << this->getTag()
            << " - incorrect dimension of orientation vectors.\n";
        exit(OF_ReturnType_failed);
    }
    
    // establish orientation of element for the tranformation matrix
    // z = x cross y
    static Vector z(3);
    z(0) = x(1)*y(2) - x(2)*y(1);
    z(1) = x(2)*y(0) - x(0)*y(2);
    z(2) = x(0)*y(1) - x(1)*y(0);
    
    // y = z cross x
    y(0) = z(1)*x(2) - z(2)*x(1);
    y(1) = z(2)*x(0) - z(0)*x(2);
    y(2) = z(0)*x(1) - z(1)*x(0);
    
    // compute length(norm) of vectors
    double xn = x.Norm();
    double yn = y.Norm();
    double zn = z.Norm();
    
    // check valid x and y vectors, i.e. not parallel and of zero length
    if (xn == 0 || yn == 0 || zn == 0)  {
        opserr << "EEBearing2d::setUp() - "
            << "element: " << this->getTag()
            << " - invalid orientation vectors.\n";
        exit(OF_ReturnType_failed);
    }
    
    // create transformation matrix from global to local system
    Tgl.Zero();
    Tgl(0,0) = Tgl(3,3) = x(0)/xn;
    Tgl(0,1) = Tgl(3,4) = x(1)/xn;
    Tgl(1,0) = Tgl(4,3) = y(0)/yn;
    Tgl(1,1) = Tgl(4,4) = y(1)/yn;
    Tgl(2,2) = Tgl(5,5) = z(2)/zn;
    
    // create transformation matrix from local to basic system (linear)
    Tlb.Zero();
    Tlb(0,0) = Tlb(1,1) = Tlb(2,2) = -1.0;
    Tlb(0,3) = Tlb(1,4) = Tlb(2,5) = 1.0;
    Tlb(1,2) = -shearDistI*L;
    Tlb(1,5) = -(1.0 - shearDistI)*L;
}


void EEBearing2d::addPDeltaForces(Vector &pLocal)
{
    // add P-Delta moments to local forces
    double kGeo1 = (*qbDaq)(0);
    
    if (kGeo1 != 0.0)  {
        double MpDelta1 = kGeo1*(dl(4)-dl(1));
        double VpDelta1 = (1.0-Mratio(0)-Mratio(1))*MpDelta1/L;
        pLocal(1) -= VpDelta1;
        pLocal(4) += VpDelta1;
        pLocal(2) += Mratio(0)*MpDelta1;
        pLocal(5) += Mratio(1)*MpDelta1;
        double MpDelta2 = kGeo1*shearDistI*L*dl(2);
        pLocal(2) += MpDelta2;
        pLocal(5) -= MpDelta2;
        double MpDelta3 = kGeo1*(1.0 - shearDistI)*L*dl(5);
        pLocal(2) -= MpDelta3;
        pLocal(5) += MpDelta3;
    }
}


void EEBearing2d::addPDeltaStiff(Matrix &kLocal)
{
    // add geometric stiffness to local stiffness
    double kGeo1 = (*qbDaq)(0);
    
    if (kGeo1 != 0.0)  {
        double kGeoOverL = (1.0-Mratio(0)-Mratio(1))*kGeo1/L;
        kLocal(1,1) += kGeoOverL;
        kLocal(1,4) -= kGeoOverL;
        kLocal(4,1) -= kGeoOverL;
        kLocal(4,4) += kGeoOverL;
        kLocal(2,1) -= Mratio(0)*kGeo1;
        kLocal(2,4) += Mratio(0)*kGeo1;
        kLocal(5,1) -= Mratio(1)*kGeo1;
        kLocal(5,4) += Mratio(1)*kGeo1;
        double kGeo2 = kGeo1*shearDistI*L;
        kLocal(2,2) += kGeo2;
        kLocal(5,2) -= kGeo2;
        double kGeo3 = kGeo1*(1.0 - shearDistI)*L;
        kLocal(2,5) -= kGeo3;
        kLocal(5,5) += kGeo3;
    }
}


void EEBearing2d::applyIMod()
{    
    // get daq displacements
    if (theSite != 0)  {
        (*dbDaq) = theSite->getDisp();
    }
    else  {
        sData[0] = OF_RemoteTest_getDisp;
        theChannel->sendVector(0, 0, *sendData, 0);
        theChannel->recvVector(0, 0, *recvData, 0);
    }
    
    // correct for displacement control errors using I-Modification
    (*qbDaq) -= kbInit*((*dbDaq) - (*db));
}
