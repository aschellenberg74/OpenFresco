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
// $URL$

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 09/06
// Revision: A
//
// Description: This file contains the implementation of the EETrussCorot class.

#include "EETrussCorot.h"

#include <Domain.h>
#include <Node.h>
#include <Channel.h>
#include <FEM_ObjectBroker.h>
#include <Renderer.h>
#include <Information.h>
#include <ElementResponse.h>
#include <TCP_Socket.h>
#include <TCP_SocketSSL.h>
#include <UDP_Socket.h>

#include <math.h>
#include <stdlib.h>
#include <string.h>


// initialize the class wide variables
Matrix EETrussCorot::EETrussCorotM2(2,2);
Matrix EETrussCorot::EETrussCorotM4(4,4);
Matrix EETrussCorot::EETrussCorotM6(6,6);
Matrix EETrussCorot::EETrussCorotM12(12,12);
Vector EETrussCorot::EETrussCorotV2(2);
Vector EETrussCorot::EETrussCorotV4(4);
Vector EETrussCorot::EETrussCorotV6(6);
Vector EETrussCorot::EETrussCorotV12(12);


// responsible for allocating the necessary space needed
// by each object and storing the tags of the end nodes.
EETrussCorot::EETrussCorot(int tag, int dim, int Nd1, int Nd2,
    ExperimentalSite *site,
    bool iM, int addRay, double r)
    : ExperimentalElement(tag, ELE_TAG_EETrussCorot, site),
    numDIM(dim), numDOF(0), connectedExternalNodes(2),
    iMod(iM), addRayleigh(addRay), rho(r), L(0.0), Ln(0.0), R(3,3),
    theMatrix(0), theVector(0), theLoad(0),
    db(0), vb(0), ab(0), t(0),
    dbDaq(0), vbDaq(0), abDaq(0), qDaq(0), tDaq(0),
    dbCtrl(1), vbCtrl(1), abCtrl(1),
    dbLast(1), tLast(0.0), kbInit(1,1),
    firstWarning(true)
{
    // ensure the connectedExternalNode ID is of correct size & set values
    if (connectedExternalNodes.Size() != 2)  {
        opserr << "EETrussCorot::EETrussCorot() - element: "
            <<  tag << " failed to create an ID of size 2\n";
        exit(-1);
    }
    
    connectedExternalNodes(0) = Nd1;
    connectedExternalNodes(1) = Nd2;
    
    // set node pointers to NULL
    theNodes[0] = 0;
    theNodes[1] = 0;
    
    // set the data size for the experimental site
    sizeCtrl = new ID(OF_Resp_All);
    sizeDaq = new ID(OF_Resp_All);
    
    (*sizeCtrl)[OF_Resp_Disp]  = 1;
    (*sizeCtrl)[OF_Resp_Vel]   = 1;
    (*sizeCtrl)[OF_Resp_Accel] = 1;
    (*sizeCtrl)[OF_Resp_Time]  = 1;
    
    (*sizeDaq)[OF_Resp_Disp]   = 1;
    (*sizeDaq)[OF_Resp_Vel]    = 1;
    (*sizeDaq)[OF_Resp_Accel]  = 1;
    (*sizeDaq)[OF_Resp_Force]  = 1;
    (*sizeDaq)[OF_Resp_Time]   = 1;
    
    theSite->setSize(*sizeCtrl, *sizeDaq);
    
    // allocate memory for trial response vectors
    db = new Vector(1);
    vb = new Vector(1);
    ab = new Vector(1);
    t  = new Vector(1);
    
    // allocate memory for daq response vectors
    dbDaq = new Vector(1);
    vbDaq = new Vector(1);
    abDaq = new Vector(1);
    qDaq  = new Vector(1);
    tDaq  = new Vector(1);
    
    // initialize additional vectors
    dbCtrl.Zero();
    vbCtrl.Zero();
    abCtrl.Zero();
    dbLast.Zero();
}


// responsible for allocating the necessary space needed
// by each object and storing the tags of the end nodes.
EETrussCorot::EETrussCorot(int tag, int dim, int Nd1, int Nd2,
    int port, char *machineInetAddr, int ssl, int udp,
    int dataSize, bool iM, int addRay, double r)
    : ExperimentalElement(tag, ELE_TAG_EETrussCorot),
    numDIM(dim), numDOF(0), connectedExternalNodes(2),
    iMod(iM), addRayleigh(addRay), rho(r), L(0.0), Ln(0.0), R(3,3),
    theMatrix(0), theVector(0), theLoad(0),
    theChannel(0), sData(0), sendData(0), rData(0), recvData(0),
    db(0), vb(0), ab(0), t(0),
    dbDaq(0), vbDaq(0), abDaq(0), qDaq(0), tDaq(0),
    dbCtrl(1), vbCtrl(1), abCtrl(1),
    dbLast(1), tLast(0.0), kbInit(1,1),
    firstWarning(true)
{
    // ensure the connectedExternalNode ID is of correct size & set values
    if (connectedExternalNodes.Size() != 2)  {
        opserr << "EETrussCorot::EETrussCorot() - element: "
            <<  tag << " failed to create an ID of size 2\n";
        exit(-1);
    }
    
    connectedExternalNodes(0) = Nd1;
    connectedExternalNodes(1) = Nd2;
    
    // set node pointers to NULL
    theNodes[0] = 0;
    theNodes[1] = 0;
    
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
        opserr << "EETrussCorot::EETrussCorot() "
            << "- failed to create channel\n";
        exit(-1);
    }
    if (theChannel->setUpConnection() != 0)  {
        opserr << "EETrussCorot::EETrussCorot() "
            << "- failed to setup connection\n";
        exit(-1);
    }
    delete [] machineInetAddr;
    
    // set the data size for the experimental site
    int intData[2*OF_Resp_All+1];
    ID idData(intData, 2*OF_Resp_All+1);
    sizeCtrl = new ID(intData, OF_Resp_All);
    sizeDaq = new ID(&intData[OF_Resp_All], OF_Resp_All);
    idData.Zero();
    
    (*sizeCtrl)[OF_Resp_Disp]  = 1;
    (*sizeCtrl)[OF_Resp_Vel]   = 1;
    (*sizeCtrl)[OF_Resp_Accel] = 1;
    (*sizeCtrl)[OF_Resp_Time]  = 1;
    
    (*sizeDaq)[OF_Resp_Disp]   = 1;
    (*sizeDaq)[OF_Resp_Vel]    = 1;
    (*sizeDaq)[OF_Resp_Accel]  = 1;
    (*sizeDaq)[OF_Resp_Force]  = 1;
    (*sizeDaq)[OF_Resp_Time]   = 1;
    
    if (dataSize < 5) dataSize = 5;
    intData[2*OF_Resp_All] = dataSize;
    
    theChannel->sendID(0, 0, idData, 0);
    
    // allocate memory for the send vectors
    int id = 1;
    sData = new double [dataSize];
    sendData = new Vector(sData, dataSize);
    db = new Vector(&sData[id], 1);
    id += 1;
    vb = new Vector(&sData[id], 1);
    id += 1;
    ab = new Vector(&sData[id], 1);
    id += 1;
    t = new Vector(&sData[id], 1);
    sendData->Zero();
    
    // allocate memory for the receive vectors
    id = 0;
    rData = new double [dataSize];
    recvData = new Vector(rData, dataSize);
    dbDaq = new Vector(&rData[id], 1);
    id += 1;
    vbDaq = new Vector(&rData[id], 1);
    id += 1;
    abDaq = new Vector(&rData[id], 1);
    id += 1;
    qDaq = new Vector(&rData[id], 1);
    id += 1;
    tDaq = new Vector(&rData[id], 1);
    recvData->Zero();
    
    // initialize additional vectors
    dbCtrl.Zero();
    vbCtrl.Zero();
    abCtrl.Zero();
    dbLast.Zero();
}


// delete must be invoked on any objects created by the object.
EETrussCorot::~EETrussCorot()
{
    // invoke the destructor on any objects created by the object
    // that the object still holds a pointer to
    if (theLoad != 0)
        delete theLoad;
    
    if (db != 0)
        delete db;
    if (vb != 0)
        delete vb;
    if (ab != 0)
        delete ab;
    if (t != 0)
        delete t;
    
    if (dbDaq != 0)
        delete dbDaq;
    if (vbDaq != 0)
        delete vbDaq;
    if (abDaq != 0)
        delete abDaq;
    if (qDaq != 0)
        delete qDaq;
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


int EETrussCorot::getNumExternalNodes() const
{
    return 2;
}


const ID& EETrussCorot::getExternalNodes()
{
    return connectedExternalNodes;
}


Node** EETrussCorot::getNodePtrs()
{
    return theNodes;
}


int EETrussCorot::getNumDOF()
{
    return numDOF;
}


int EETrussCorot::getNumBasicDOF()
{
    return 1;
}


// to set a link to the enclosing Domain and to set the node pointers.
void EETrussCorot::setDomain(Domain *theDomain)
{
    // check Domain is not null - invoked when object removed from a domain
    if (!theDomain)  {
        theNodes[0] = 0;
        theNodes[1] = 0;
        L  = 0.0;
        Ln = 0.0;
        return;
    }
    
    // set default values for error conditions
    numDOF = 2;
    theMatrix = &EETrussCorotM2;
    theVector = &EETrussCorotV2;
    
    // first set the node pointers
    int Nd1 = connectedExternalNodes(0);
    int Nd2 = connectedExternalNodes(1);
    theNodes[0] = theDomain->getNode(Nd1);
    theNodes[1] = theDomain->getNode(Nd2);
    
    // if can't find both - send a warning message
    if (!theNodes[0] || !theNodes[1])  {
        if (!theNodes[0])  {
            opserr << "EETrussCorot::setDomain() - Nd1: "
                << Nd1 << "does not exist in the model for ";
        } else  {
            opserr << "EETrussCorot::setDomain() - Nd2: "
                << Nd2 << "does not exist in the model for ";
        }
        opserr << "EETrussCorot ele: " << this->getTag() << endln;
        
        return;
    }
    
    // now determine the number of dof and the dimension
    int dofNd1 = theNodes[0]->getNumberDOF();
    int dofNd2 = theNodes[1]->getNumberDOF();
    
    // if differing dof at the ends - print a warning message
    if (dofNd1 != dofNd2)  {
        opserr <<"EETrussCorot::setDomain(): nodes " << Nd1 << " and " << Nd2
            << "have differing dof at ends for element: " << this->getTag() << endln;
        
        return;
    }
    
    // call the base class method
    this->DomainComponent::setDomain(theDomain);
    
    // now set the number of dof for element and set matrix and vector pointer
    if (numDIM == 1 && dofNd1 == 1)  {
        numDOF = 2;
        theMatrix = &EETrussCorotM2;
        theVector = &EETrussCorotV2;
    }
    else if (numDIM == 2 && dofNd1 == 2)  {
        numDOF = 4;
        theMatrix = &EETrussCorotM4;
        theVector = &EETrussCorotV4;
    }
    else if (numDIM == 2 && dofNd1 == 3)  {
        numDOF = 6;
        theMatrix = &EETrussCorotM6;
        theVector = &EETrussCorotV6;
    }
    else if (numDIM == 3 && dofNd1 == 3)  {
        numDOF = 6;
        theMatrix = &EETrussCorotM6;
        theVector = &EETrussCorotV6;
    }
    else if (numDIM == 3 && dofNd1 == 6)  {
        numDOF = 12;
        theMatrix = &EETrussCorotM12;
        theVector = &EETrussCorotV12;
    }
    else  {
        opserr <<"EETrussCorot::setDomain() - can not handle "
            << numDIM << " dofs at nodes in " << dofNd1  << " d problem\n";
        
        return;
    }
    
    // set the initial stiffness matrix size
    theInitStiff.resize(numDOF,numDOF);
    
    if (!theLoad)
        theLoad = new Vector(numDOF);
    else if (theLoad->Size() != numDOF)  {
        delete theLoad;
        theLoad = new Vector(numDOF);
    }
    
    if (!theLoad)  {
        opserr << "EETrussCorot::setDomain() - element: " << this->getTag()
            << " out of memory creating vector of size: " << numDOF << endln;
        
        return;
    }
    
    // now determine the length, cosines and fill in the transformation
    // NOTE t = -t(every one else uses for residual calc)
    const Vector &end1Crd = theNodes[0]->getCrds();
    const Vector &end2Crd = theNodes[1]->getCrds();
    
    // initalize the cosines
    double cosX[3];
    cosX[0] = cosX[1] = cosX[2] = 0.0;
    for (int i=0; i<numDIM; i++)
        cosX[i] = end2Crd(i)-end1Crd(i);
    
    // get initial length
    L = sqrt(cosX[0]*cosX[0] + cosX[1]*cosX[1] + cosX[2]*cosX[2]);
    if (L == 0.0)  {
        opserr <<"EETrussCorot::setDomain() - element: "
            << this->getTag() << " has zero length\n";
        return;
    }
    Ln = L;
    
    // set global orientations
    cosX[0] /= L;
    cosX[1] /= L;
    cosX[2] /= L;
    
    // initialize rotation matrix
    R(0,0) = cosX[0];
    R(0,1) = cosX[1];
    R(0,2) = cosX[2];
    if (fabs(cosX[0]) > 0.0)  {  // element outside the YZ plane
        R(1,0) = -cosX[1];
        R(1,1) =  cosX[0];
        R(1,2) =  0.0;
        
        R(2,0) = -cosX[0]*cosX[2];
        R(2,1) = -cosX[1]*cosX[2];
        R(2,2) =  cosX[0]*cosX[0] + cosX[1]*cosX[1];
    }
    else  {  // element in the YZ plane
        R(1,0) =  0.0;
        R(1,1) = -cosX[2];
        R(1,2) =  cosX[1];
        
        R(2,0) =  1.0;
        R(2,1) =  0.0;
        R(2,2) =  0.0;
    }
    
    // orthonormalize last two rows of R
    double norm;
    for (int i=1; i<3; i++)  {
        norm = sqrt(R(i,0)*R(i,0) + R(i,1)*R(i,1) + R(i,2)*R(i,2));
        R(i,0) /= norm;
        R(i,1) /= norm;
        R(i,2) /= norm;
    }
    
    // set initial offsets
    d21[0] = L; d21[1] = d21[2] = 0.0;
    v21[0] = v21[1] = v21[2] = 0.0;
    a21[0] = a21[1] = a21[2] = 0.0;
}


int EETrussCorot::commitState()
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
    
    // commit the base class
    rValue += this->Element::commitState();
    
    return rValue;
}


int EETrussCorot::update()
{
    int rValue = 0;
    
    // get current time
    Domain *theDomain = this->getDomain();
    (*t)(0) = theDomain->getCurrentTime();
    
    // determine dsp, vel and acc in basic system
    const Vector &dsp1 = theNodes[0]->getTrialDisp();
    const Vector &dsp2 = theNodes[1]->getTrialDisp();
    const Vector &vel1 = theNodes[0]->getTrialVel();
    const Vector &vel2 = theNodes[1]->getTrialVel();
    const Vector &acc1 = theNodes[0]->getTrialAccel();
    const Vector &acc2 = theNodes[1]->getTrialAccel();
    
    // initial offsets
    d21[0] = L; d21[1] = d21[2] = 0.0;
    v21[0] = v21[1] = v21[2] = 0.0;
    a21[0] = a21[1] = a21[2] = 0.0;
    
    // update offsets in basic system
    for (int i=0; i<numDIM; i++)  {
        double deltaDsp = dsp2(i) - dsp1(i);
        d21[0] += deltaDsp*R(0,i);
        d21[1] += deltaDsp*R(1,i);
        d21[2] += deltaDsp*R(2,i);
        double deltaVel = vel2(i) - vel1(i);
        v21[0] += deltaVel*R(0,i);
        v21[1] += deltaVel*R(1,i);
        v21[2] += deltaVel*R(2,i);
        double deltaAcc = acc2(i) - acc1(i);
        a21[0] += deltaAcc*R(0,i);
        a21[1] += deltaAcc*R(1,i);
        a21[2] += deltaAcc*R(2,i);
    }
    
    // compute new length and deformation
    Ln = sqrt(d21[0]*d21[0] + d21[1]*d21[1] + d21[2]*d21[2]);
    double c1 = d21[0]*v21[0] + d21[1]*v21[1] + d21[2]*v21[2];
    double c2 = v21[0]*v21[0] + v21[1]*v21[1] + v21[2]*v21[2]
              + d21[0]*a21[0] + d21[1]*a21[1] + d21[2]*a21[2];
    (*db)(0) = Ln - L;
    (*vb)(0) = c1/Ln;
    (*ab)(0) = c2/Ln - (c1*c1)/(Ln*Ln*Ln);
    
    Vector dbDelta = (*db) - dbLast;
    // do not check time for right now because of transformation constraint
    // handler calling update at beginning of new step when applying load
    // if (dbDelta.pNorm(2) > DBL_EPSILON || (*t)(0) > tLast)  {
    if (dbDelta.pNorm(2) > DBL_EPSILON)  {
        // set the trial response at the site
        if (theSite != 0)  {
            theSite->setTrialResponse(db, vb, ab, (Vector*)0, t);
        }
        else  {
            sData[0] = OF_RemoteTest_setTrialResponse;
            rValue += theChannel->sendVector(0, 0, *sendData, 0);
        }
    }
    
    // save the last displacements and time
    dbLast = (*db);
    tLast = (*t)(0);
    
    return rValue;
}


int EETrussCorot::setInitialStiff(const Matrix& kbinit)
{
    if (kbinit.noRows() != 1 || kbinit.noCols() != 1)  {
        opserr << "EETrussCorot::setInitialStiff(): " 
            << "matrix size is incorrect for element: "
            << this->getTag() << endln;
        return -1;
    }
    kbInit = kbinit;
    
    // transform the stiffness from the basic to the local system
    static Matrix kl(3,3);
    kl.Zero();
    kl(0,0) = kbInit(0,0);
    
    // transform the stiffness from the local to the global system
    static Matrix kg(3,3);
    kg.addMatrixTripleProduct(0.0, R, kl, 1.0);
    
    // copy stiffness into appropriate blocks in element stiffness
    theInitStiff.Zero();
    int numDOF2 = numDOF/2;
    for (int i=0; i<numDIM; i++)  {
        for (int j=0; j<numDIM; j++)  {
            theInitStiff(i,j) = kg(i,j);
            theInitStiff(i,j+numDOF2) = -kg(i,j);
            theInitStiff(i+numDOF2,j) = -kg(i,j);
            theInitStiff(i+numDOF2,j+numDOF2) = kg(i,j);
        }
    }
    
    return 0;
}


const Matrix& EETrussCorot::getTangentStiff()
{
    // zero the global matrix
    theMatrix->Zero();
    
    if (firstWarning == true)  {
        opserr << "\nWARNING EETrussCorot::getTangentStiff() - "
            << "Element: " << this->getTag() << endln
            << "TangentStiff cannot be calculated." << endln
            << "Return InitialStiff including GeometricStiff instead."
            << endln;
        opserr << "Subsequent getTangentStiff warnings will be suppressed."
            << endln;
        
        firstWarning = false;
    }
    
    // get daq resisting forces
    if (theSite != 0)  {
        (*qDaq) = theSite->getForce();
    }
    else  {
        sData[0] = OF_RemoteTest_getForce;
        theChannel->sendVector(0, 0, *sendData, 0);
        theChannel->recvVector(0, 0, *recvData, 0);
    }
    
    // apply optional initial stiffness modification
    if (iMod == true)  {
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
        qDaq->addMatrixVector(1.0, kbInit, (*dbDaq) - (*db), -1.0);
    }
    
    // transform the stiffness from the basic to the local system
    int i,j;
    static Matrix kl(3,3);
    double EAoverL3 = kbInit(0,0)/(Ln*Ln);
    for (i=0; i<3; i++)
        for (j=0; j<3; j++)
            kl(i,j) = EAoverL3*d21[i]*d21[j];
    
    // add geometric stiffness portion
    double SL = (*qDaq)(0)/Ln;
    double SA = (*qDaq)(0)/(Ln*Ln*Ln);
    for (i=0; i<3; i++)  {
        kl(i,i) += SL;
        for (j=0; j<3; j++)
            kl(i,j) -= SA*d21[i]*d21[j];
    }
    
    // transform the stiffness from the local to the global system
    static Matrix kg(3,3);
    kg.addMatrixTripleProduct(0.0, R, kl, 1.0);
    
    // copy stiffness into appropriate blocks in element stiffness
    int numDOF2 = numDOF/2;
    for (i=0; i<numDIM; i++)  {
        for (j=0; j<numDIM; j++)  {
            (*theMatrix)(i,j) = kg(i,j);
            (*theMatrix)(i,j+numDOF2) = -kg(i,j);
            (*theMatrix)(i+numDOF2,j) = -kg(i,j);
            (*theMatrix)(i+numDOF2,j+numDOF2) = kg(i,j);
        }
    }
    
    return *theMatrix;
}


const Matrix& EETrussCorot::getDamp()
{
    // zero the global matrix
    theMatrix->Zero();
    
    // call base class to setup Rayleigh damping
    if (addRayleigh == 1)  {
        (*theMatrix) = this->Element::getDamp();
    }
    
    return *theMatrix;
}


const Matrix& EETrussCorot::getMass()
{
    // zero the global matrix
    theMatrix->Zero();
    
    // form mass matrix
    if (L != 0.0 && rho != 0.0)  {
        double m = 0.5*rho*L;
        int numDOF2 = numDOF/2;
        for (int i=0; i<numDIM; i++)  {
            (*theMatrix)(i,i) = m;
            (*theMatrix)(i+numDOF2,i+numDOF2) = m;
        }
    }
    
    return *theMatrix;
}


void EETrussCorot::zeroLoad()
{
    theLoad->Zero();
}


int EETrussCorot::addLoad(ElementalLoad *theLoad, double loadFactor)
{
    opserr <<"EETrussCorot::addLoad() - "
        << "load type unknown for element: "
        << this->getTag() << endln;
    
    return -1;
}


int EETrussCorot::addInertiaLoadToUnbalance(const Vector &accel)
{
    // check for a quick return
    if (L == 0.0 || rho == 0.0)  {
        return 0;
    }
    
    // get R * accel from the nodes
    const Vector &Raccel1 = theNodes[0]->getRV(accel);
    const Vector &Raccel2 = theNodes[1]->getRV(accel);
    
    int nodalDOF = numDOF/2;
    
    if (nodalDOF != Raccel1.Size() || nodalDOF != Raccel2.Size()) {
        opserr <<"EETrussCorot::addInertiaLoadToUnbalance() - "
            << "matrix and vector sizes are incompatible\n";
        return -1;
    }
    
    // want to add ( - fact * M R * accel ) to unbalance
    double m = 0.5*rho*L;
    for (int i=0; i<numDIM; i++) {
        double val1 = Raccel1(i);
        double val2 = Raccel2(i);
        
        // perform - fact * M*(R * accel) // remember M a diagonal matrix
        val1 *= -m;
        val2 *= -m;
        
        (*theLoad)(i) += val1;
        (*theLoad)(i+nodalDOF) += val2;
    }
    
    return 0;
}


const Vector& EETrussCorot::getResistingForce()
{
    // zero the global residual
    theVector->Zero();
    
    // get daq resisting forces
    if (theSite != 0)  {
        (*qDaq) = theSite->getForce();
    }
    else  {
        sData[0] = OF_RemoteTest_getForce;
        theChannel->sendVector(0, 0, *sendData, 0);
        theChannel->recvVector(0, 0, *recvData, 0);
    }
    
    // apply optional initial stiffness modification
    if (iMod == true)  {
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
        qDaq->addMatrixVector(1.0, kbInit, (*dbDaq) - (*db), -1.0);
    }
    
    // save corresponding ctrl displacements for recorder
    dbCtrl = (*db);
    vbCtrl = (*vb);
    abCtrl = (*ab);
    
    // transform the forces from the basic to the local system
    static Vector ql(3);
    ql(0) = d21[0]/Ln*(*qDaq)(0);
    ql(1) = d21[1]/Ln*(*qDaq)(0);
    ql(2) = d21[2]/Ln*(*qDaq)(0);
    
    // transform the forces from the local to the global system
    static Vector qg(3);
    qg.addMatrixTransposeVector(0.0, R, ql, 1.0);
    
    // copy forces into appropriate places
    int numDOF2 = numDOF/2;
    for (int i=0; i<numDIM; i++)  {
        (*theVector)(i) = -qg(i);
        (*theVector)(i+numDOF2) = qg(i);
    }
    
    // subtract external load
    theVector->addVector(1.0, *theLoad, -1.0);
    
    return *theVector;
}


const Vector& EETrussCorot::getResistingForceIncInertia()
{
    // this already includes damping forces from specimen
    *theVector = this->getResistingForce();
    
    // add the damping forces from rayleigh damping
    if (addRayleigh == 1)  {
        if (alphaM != 0.0 || betaK != 0.0 || betaK0 != 0.0 || betaKc != 0.0)
            theVector->addVector(1.0, this->getRayleighDampingForces(), 1.0);
    }
    
    // add inertia forces from element mass
    if (L != 0.0 && rho != 0.0)  {
        const Vector &accel1 = theNodes[0]->getTrialAccel();
        const Vector &accel2 = theNodes[1]->getTrialAccel();
        
        int numDOF2 = numDOF/2;
        double m = 0.5*rho*L;
        for (int i=0; i<numDIM; i++) {
            (*theVector)(i) += m * accel1(i);
            (*theVector)(i+numDOF2) += m * accel2(i);
        }
    }
    
    return *theVector;
}


const Vector& EETrussCorot::getTime()
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


const Vector& EETrussCorot::getBasicDisp()
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


const Vector& EETrussCorot::getBasicVel()
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


const Vector& EETrussCorot::getBasicAccel()
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


int EETrussCorot::sendSelf(int commitTag, Channel &theChannel)
{
    // has not been implemented yet.....
    return 0;
}


int EETrussCorot::recvSelf(int commitTag, Channel &theChannel,
    FEM_ObjectBroker &theBroker)
{
    // has not been implemented yet.....
    return 0;
}


int EETrussCorot::displaySelf(Renderer &theViewer,
    int displayMode, float fact)
{
    // first determine the end points of the element based on
    // the display factor (a measure of the distorted image)
    const Vector &end1Crd = theNodes[0]->getCrds();
    const Vector &end2Crd = theNodes[1]->getCrds();
    
    const Vector &end1Disp = theNodes[0]->getDisp();
    const Vector &end2Disp = theNodes[1]->getDisp();
    
    static Vector v1(3);
    static Vector v2(3);
    
    for (int i=0; i<numDIM; i++)  {
        v1(i) = end1Crd(i) + end1Disp(i)*fact;
        v2(i) = end2Crd(i) + end2Disp(i)*fact;
    }
    
    return theViewer.drawLine (v1, v2, 1.0, 1.0);
}


void EETrussCorot::Print(OPS_Stream &s, int flag)
{
    if (flag == 0)  {
        // print everything
        s << "Element: " << this->getTag() << endln;
        s << "  type: EETrussCorot" << endln;
        s << "  iNode: " << connectedExternalNodes(0)
            << ", jNode: " << connectedExternalNodes(1) << endln;
        if (theSite != 0)
            s << "  ExperimentalSite: " << theSite->getTag() << endln;
        s << "  addRayleigh: " << addRayleigh;
        s << "  mass per unit length: " << rho << endln;
        // determine resisting forces in global system
        s << "  resisting force: " << this->getResistingForce() << endln;
    } else if (flag == 1)  {
        // does nothing
    }
}


Response* EETrussCorot::setResponse(const char **argv, int argc,
    OPS_Stream &output)
{
    Response *theResponse = 0;
    
    output.tag("ElementOutput");
    output.attr("eleType","EETrussCorot");
    output.attr("eleTag",this->getTag());
    output.attr("node1",connectedExternalNodes[0]);
    output.attr("node2",connectedExternalNodes[1]);
    
    char outputData[10];
    
    // global forces
    if (strcmp(argv[0],"force") == 0 ||
        strcmp(argv[0],"forces") == 0 ||
        strcmp(argv[0],"globalForce") == 0 ||
        strcmp(argv[0],"globalForces") == 0)
    {
        for (int i=0; i<numDOF; i++)  {
            sprintf(outputData,"P%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 1, *theVector);
    }
    
    // local forces
    else if (strcmp(argv[0],"localForce") == 0 ||
        strcmp(argv[0],"localForces") == 0)
    {
        for (int i=0; i<numDOF; i++)  {
            sprintf(outputData,"p%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 2, *theVector);
    }
    
    // basic force
    else if (strcmp(argv[0],"basicForce") == 0 ||
        strcmp(argv[0],"basicForces") == 0 ||
        strcmp(argv[0],"daqForce") == 0 ||
        strcmp(argv[0],"daqForces") == 0)
    {
        output.tag("ResponseType","q1");
        
        theResponse = new ElementResponse(this, 3, Vector(1));
    }
    
    // ctrl basic displacement
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
        
        theResponse = new ElementResponse(this, 4, Vector(1));
    }
    
    // ctrl basic velocity
    else if (strcmp(argv[0],"ctrlVel") == 0 ||
        strcmp(argv[0],"ctrlVelocity") == 0 ||
        strcmp(argv[0],"ctrlVelocities") == 0)
    {
        output.tag("ResponseType","vb1");
        
        theResponse = new ElementResponse(this, 5, Vector(1));
    }
    
    // ctrl basic acceleration
    else if (strcmp(argv[0],"ctrlAccel") == 0 ||
        strcmp(argv[0],"ctrlAcceleration") == 0 ||
        strcmp(argv[0],"ctrlAccelerations") == 0)
    {
        output.tag("ResponseType","ab1");
        
        theResponse = new ElementResponse(this, 6, Vector(1));
    }    
    
    // daq basic displacement
    else if (strcmp(argv[0],"daqDisp") == 0 ||
        strcmp(argv[0],"daqDisplacement") == 0 ||
        strcmp(argv[0],"daqDisplacements") == 0)
    {
        output.tag("ResponseType","dbDaq1");
        
        theResponse = new ElementResponse(this, 7, Vector(1));
    }
    
    // daq basic velocity
    else if (strcmp(argv[0],"daqVel") == 0 ||
        strcmp(argv[0],"daqVelocity") == 0 ||
        strcmp(argv[0],"daqVelocities") == 0)
    {
        output.tag("ResponseType","vbDaq1");
        
        theResponse = new ElementResponse(this, 8, Vector(1));
    }
    
    // daq basic acceleration
    else if (strcmp(argv[0],"daqAccel") == 0 ||
        strcmp(argv[0],"daqAcceleration") == 0 ||
        strcmp(argv[0],"daqAccelerations") == 0)
    {
        output.tag("ResponseType","abDaq1");
        
        theResponse = new ElementResponse(this, 9, Vector(1));
    }
    
    output.endTag(); // ElementOutput
    
    return theResponse;
}


int EETrussCorot::getResponse(int responseID, Information &eleInfo)
{
    switch (responseID)  {
    case 1:  // global forces
        return eleInfo.setVector(this->getResistingForce());
        
    case 2:  // local forces
        theVector->Zero();
        // Axial
        (*theVector)(0)        = -(*qDaq)(0);
        (*theVector)(numDOF/2) =  (*qDaq)(0);
        
        return eleInfo.setVector(*theVector);
        
    case 3:  // basic force
        return eleInfo.setVector(*qDaq);
        
    case 4:  // ctrl basic displacement
        return eleInfo.setVector(dbCtrl);
        
    case 5:  // ctrl basic velocity
        return eleInfo.setVector(vbCtrl);
        
    case 6:  // ctrl basic acceleration
        return eleInfo.setVector(abCtrl);
        
    case 7:  // daq basic displacement
        return eleInfo.setVector(this->getBasicDisp());
        
    case 8:  // daq basic velocitie
        return eleInfo.setVector(this->getBasicVel());
        
    case 9:  // daq basic acceleration
        return eleInfo.setVector(this->getBasicAccel());
        
    default:
        return 0;
    }
}
