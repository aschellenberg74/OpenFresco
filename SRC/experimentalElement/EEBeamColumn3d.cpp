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
// Created: 09/06
// Revision: A
//
// Description: This file contains the implementation of the EEBeamColumn3d class.

#include "EEBeamColumn3d.h"

#include <ArrayOfTaggedObjects.h>
#include <Domain.h>
#include <Node.h>
#include <Channel.h>
#include <FEM_ObjectBroker.h>
#include <Renderer.h>
#include <Information.h>
#include <ElementResponse.h>
#include <CrdTransf3d.h>
#include <ElementalLoad.h>
#include <TCP_Socket.h>
#include <TCP_SocketSSL.h>

#include <math.h>
#include <stdlib.h>
#include <string.h>


// initialize the class wide variables
Matrix EEBeamColumn3d::theMatrix(12,12);
Vector EEBeamColumn3d::theVector(12);
Vector EEBeamColumn3d::theLoad(12);


// responsible for allocating the necessary space needed
// by each object and storing the tags of the end nodes.
EEBeamColumn3d::EEBeamColumn3d(int tag, int Nd1, int Nd2,
    CrdTransf3d &coordTransf,
    ExperimentalSite *site,
    bool iM, double r)
    : ExperimentalElement(tag, ELE_TAG_EEBeamColumn3d, site),
    connectedExternalNodes(2), theCoordTransf(0),
    iMod(iM), rho(r), L(0.0),
    db(0), vb(0), ab(0), t(0),
    dbMeas(0), vbMeas(0), abMeas(0), qMeas(0), tMeas(0),
    dbTarg(6), vbTarg(6), abTarg(6),
    dbPast(6), kbInit(6,6), T(6,6), Tinv(6,6),
    firstWarning(true)
{
    // ensure the connectedExternalNode ID is of correct size & set values
    if (connectedExternalNodes.Size() != 2)  {
        opserr << "EEBeamColumn3d::EEBeamColumn3d() - element: "
            <<  tag << " failed to create an ID of size 2\n";
        exit(-1);
    }
    
    connectedExternalNodes(0) = Nd1;
    connectedExternalNodes(1) = Nd2;
    
    // set node pointers to NULL
    int i;
    for (i=0; i<2; i++)
        theNodes[i] = 0;
    
    // set the data size for the experimental site
    sizeCtrl = new ID(OF_Resp_All);
    sizeDaq = new ID(OF_Resp_All);
    
    (*sizeCtrl)[OF_Resp_Disp]  = 6;
    (*sizeCtrl)[OF_Resp_Vel]   = 6;
    (*sizeCtrl)[OF_Resp_Accel] = 6;
    (*sizeCtrl)[OF_Resp_Time]  = 1;
    
    (*sizeDaq)[OF_Resp_Disp]   = 6;
    (*sizeDaq)[OF_Resp_Vel]    = 6;
    (*sizeDaq)[OF_Resp_Accel]  = 6;
    (*sizeDaq)[OF_Resp_Force]  = 6;
    (*sizeDaq)[OF_Resp_Time]   = 1;
    
    theSite->setSize(*sizeCtrl, *sizeDaq);
    
    // allocate memory for trial response vectors
    db = new Vector(6);
    vb = new Vector(6);
    ab = new Vector(6);
    t  = new Vector(1);

    // allocate memory for measured response vectors
    dbMeas = new Vector(6);
    vbMeas = new Vector(6);
    abMeas = new Vector(6);
    qMeas  = new Vector(6);
    tMeas  = new Vector(1);

    // set the initial stiffness matrix size
    theInitStiff.resize(12,12);
    
    // get a copy of the coordinate transformation
    theCoordTransf = coordTransf.getCopy();
    if (!theCoordTransf)  {
        opserr << "EEBeamColumn3d::EEBeamColumn3d() - "
            << "failed to get copy of the coordinate transformation\n";
        exit(-1);
    }
    
    // initialize additional vectors
    dbTarg.Zero();
    vbTarg.Zero();
    abTarg.Zero();
    dbPast.Zero();
    for (i=0; i<6; i++)  {
        qA0[i] = 0.0;
        pA0[i] = 0.0;
    }
}


// responsible for allocating the necessary space needed
// by each object and storing the tags of the end nodes.
EEBeamColumn3d::EEBeamColumn3d(int tag, int Nd1, int Nd2,
    CrdTransf3d &coordTransf,
    int port, char *machineInetAddr, int ssl, int dataSize,
    bool iM, double r)
    : ExperimentalElement(tag, ELE_TAG_EEBeamColumn3d),
    connectedExternalNodes(2), theCoordTransf(0),
    iMod(iM), rho(r), L(0.0),
    theChannel(0), sData(0), sendData(0), rData(0), recvData(0),
    db(0), vb(0), ab(0), t(0),
    dbMeas(0), vbMeas(0), abMeas(0), qMeas(0), tMeas(0),
    dbTarg(6), vbTarg(6), abTarg(6),
    dbPast(6), kbInit(6,6), T(6,6), Tinv(6,6),
    firstWarning(true)
{
    // ensure the connectedExternalNode ID is of correct size & set values
    if (connectedExternalNodes.Size() != 2)  {
        opserr << "EEBeamColumn3d::EEBeamColumn3d() - element: "
            <<  tag << " failed to create an ID of size 2\n";
        exit(-1);
    }
    
    connectedExternalNodes(0) = Nd1;
    connectedExternalNodes(1) = Nd2;
    
    // set node pointers to NULL
    int i;
    for (i=0; i<2; i++)
        theNodes[i] = 0;
    
    // setup the connection
    if (!ssl)  {
        if (machineInetAddr == 0)
            theChannel = new TCP_Socket(port, "127.0.0.1");
        else
            theChannel = new TCP_Socket(port, machineInetAddr);
    }
    else  {
        if (machineInetAddr == 0)
            theChannel = new TCP_SocketSSL(port, "127.0.0.1");
        else
            theChannel = new TCP_SocketSSL(port, machineInetAddr);
    }
    if (!theChannel)  {
        opserr << "EEBeamColumn3d::EEBeamColumn3d() "
            << "- failed to create channel\n";
        exit(-1);
    }
    if (theChannel->setUpConnection() != 0)  {
        opserr << "EEBeamColumn3d::EEBeamColumn3d() "
            << "- failed to setup connection\n";
        exit(-1);
    }

    // set the data size for the experimental site
    int intData[2*OF_Resp_All+1];
    ID idData(intData, 2*OF_Resp_All+1);
    sizeCtrl = new ID(intData, OF_Resp_All);
    sizeDaq = new ID(&intData[OF_Resp_All], OF_Resp_All);
    idData.Zero();

    (*sizeCtrl)[OF_Resp_Disp]  = 6;
    (*sizeCtrl)[OF_Resp_Vel]   = 6;
    (*sizeCtrl)[OF_Resp_Accel] = 6;
    (*sizeCtrl)[OF_Resp_Time]  = 1;
    
    (*sizeDaq)[OF_Resp_Disp]   = 6;
    (*sizeDaq)[OF_Resp_Vel]    = 6;
    (*sizeDaq)[OF_Resp_Accel]  = 6;
    (*sizeDaq)[OF_Resp_Force]  = 6;
    (*sizeDaq)[OF_Resp_Time]   = 1;

    if (dataSize < 25) dataSize = 25;
    intData[2*OF_Resp_All] = dataSize;

    theChannel->sendID(0, 0, idData, 0);
    
    // allocate memory for the send vectors
    int id = 1;
    sData = new double [dataSize];
    sendData = new Vector(sData, dataSize);
    db = new Vector(&sData[id], 6);
    id += 6;
    vb = new Vector(&sData[id], 6);
    id += 6;
    ab = new Vector(&sData[id], 6);
    id += 6;
    t = new Vector(&sData[id], 1);
    sendData->Zero();

    // allocate memory for the receive vectors
    id = 0;
    rData = new double [dataSize];
    recvData = new Vector(rData, dataSize);
    dbMeas = new Vector(&rData[id], 1);
    id += 6;
    vbMeas = new Vector(&rData[id], 1);
    id += 6;
    abMeas = new Vector(&rData[id], 1);
    id += 6;
    qMeas = new Vector(&rData[id], 1);
    id += 6;
    tMeas = new Vector(&rData[id], 1);
    recvData->Zero();

    // set the initial stiffness matrix size
    theInitStiff.resize(12,12);
    
    // get a copy of the coordinate transformation
    theCoordTransf = coordTransf.getCopy();
    if (!theCoordTransf)  {
        opserr << "EEBeamColumn3d::EEBeamColumn3d() - "
            << "failed to get copy of the coordinate transformation\n";
        exit(-1);
    }
    
    // initialize additional vectors
    dbTarg.Zero();
    vbTarg.Zero();
    abTarg.Zero();
    dbPast.Zero();
    for (i=0; i<6; i++)  {
        qA0[i] = 0.0;
        pA0[i] = 0.0;
    }
}


// delete must be invoked on any objects created by the object.
EEBeamColumn3d::~EEBeamColumn3d()
{
    // invoke the destructor on any objects created by the object
    // that the object still holds a pointer to
    if (theCoordTransf != 0)
        delete theCoordTransf;

    if (db != 0)
        delete db;
    if (vb != 0)
        delete vb;
    if (ab != 0)
        delete ab;
    if (t != 0)
        delete t;

    if (dbMeas != 0)
        delete dbMeas;
    if (vbMeas != 0)
        delete vbMeas;
    if (abMeas != 0)
        delete abMeas;
    if (qMeas != 0)
        delete qMeas;
    if (tMeas != 0)
        delete tMeas;

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


int EEBeamColumn3d::getNumExternalNodes() const
{
    return 2;
}


const ID& EEBeamColumn3d::getExternalNodes() 
{
    return connectedExternalNodes;
}


Node** EEBeamColumn3d::getNodePtrs() 
{
    return theNodes;
}


int EEBeamColumn3d::getNumDOF() 
{
    return 12;
}


int EEBeamColumn3d::getNumBasicDOF() 
{
    return 6;
}


// to set a link to the enclosing Domain and to set the node pointers.
void EEBeamColumn3d::setDomain(Domain *theDomain)
{
    // check Domain is not null - invoked when object removed from a domain
    if (!theDomain)  {
        theNodes[0] = 0;
        theNodes[1] = 0;
        
        return;
    }
    
    // first set the node pointers
    int Nd1 = connectedExternalNodes(0);
    int Nd2 = connectedExternalNodes(1);
    theNodes[0] = theDomain->getNode(Nd1);
    theNodes[1] = theDomain->getNode(Nd2);	
    
    // if can't find both - send a warning message
    if (!theNodes[0] || !theNodes[1])  {
        if (!theNodes[0])  {
            opserr << "EEBeamColumn3d::setDomain() - Nd1: " 
                << Nd1 << " does not exist in the model for ";
        } else  {
            opserr << "EEBeamColumn3d::setDomain() - Nd2: " 
                << Nd2 << " does not exist in the model for ";
        }
        opserr << "EEBeamColumn3d ele: " << this->getTag() << endln;
        
        return;
    }
    
    // now determine the number of dof and the dimension    
    int dofNd1 = theNodes[0]->getNumberDOF();
    int dofNd2 = theNodes[1]->getNumberDOF();	
    
    // if differing dof at the ends - print a warning message
    if (dofNd1 != 6)  {
        opserr << "EEBeamColumn3d::setDomain() - node 1: "
            << connectedExternalNodes(0) << " has incorrect number of DOF (not 6)\n";
        return;
    }
    if (dofNd2 != 6)  {
        opserr << "EEBeamColumn3d::setDomain() - node 2: "
            << connectedExternalNodes(1) << " has incorrect number of DOF (not 6)\n";
        return;
    }
    
    // call the base class method
    this->DomainComponent::setDomain(theDomain);
    
    // initialize the coordinate transformation
    if (theCoordTransf->initialize(theNodes[0], theNodes[1]) != 0)  {
        opserr << "EEBeamColumn3d::setDomain() - "
            << "error initializing coordinate transformation\n";
        return;
    }
    
    // get initial element length
    double L = theCoordTransf->getInitialLength();
    if (L == 0.0)  {
        opserr << "EEBeamColumn3d::setDomain() - element: " 
            << this->getTag() << " has zero length\n";
        return;
    }
    
    // set transformation matrix from basic sys A to basic sys B
    T.Zero();
    T(0,0) =  1;
    T(1,1) = -L;
    T(2,1) = -1;  T(2,2) = 1;
    T(3,3) =  L;
    T(4,3) = -1;  T(4,4) = 1;
    T(5,5) =  1;
    
    // set transformation matrix from basic sys B to basic sys A
    Tinv.Zero();
    Tinv(0,0) =  1;
    Tinv(1,1) = -1/L;
    Tinv(2,1) = -1/L;  Tinv(2,2) = 1;
    Tinv(3,3) =  1/L;
    Tinv(4,3) =  1/L;  Tinv(4,4) = 1;
    Tinv(5,5) =  1;
}   	 


int EEBeamColumn3d::commitState()
{
    int rValue = 0;
    
    if (theSite != 0)  {
        rValue += theSite->commitState();
    }
    else  {
        sData[0] = OF_RemoteTest_commitState;
        rValue += theChannel->sendVector(0, 0, *sendData, 0);
    }
    rValue += theCoordTransf->commitState();
    
    return rValue;
}


int EEBeamColumn3d::update()
{
    int rValue = 0;

    // get current time
    Domain *theDomain = this->getDomain();
    (*t)(0) = theDomain->getCurrentTime();
    
    // update the coordinate transformation
    theCoordTransf->update();
    
    // determine dsp, vel and acc in basic system A
    const Vector &dbA = theCoordTransf->getBasicTrialDisp();
    const Vector &vbA = theCoordTransf->getBasicTrialVel();
    const Vector &abA = theCoordTransf->getBasicTrialAccel();
    
    // transform displacements from basic sys A to basic sys B
    (*db) = T*dbA;
    (*vb) = T*vbA;
    (*ab) = T*abA;
    
    if ((*db) != dbPast)  {
        // save the displacements
        dbPast = (*db);
        // set the trial response at the site
        if (theSite != 0)  {
            theSite->setTrialResponse(db, vb, ab, (Vector*)0, t);
        }
        else  {
            sData[0] = OF_RemoteTest_setTrialResponse;
            rValue += theChannel->sendVector(0, 0, *sendData, 0);
        }
    }
    
    return rValue;
}


int EEBeamColumn3d::setInitialStiff(const Matrix& kbinit)
{
    if (kbinit.noRows() != 6 || kbinit.noCols() != 6)  {
        opserr << "EEBeamColumn3d::setInitialStiff() - " 
            << "matrix size is incorrect for element: "
            << this->getTag() << endln;
        return -1;
    }
    kbInit = kbinit;
        
    // transform the stiffness from basic sys B to basic sys A
    static Matrix kbAInit(6,6);
    kbAInit.addMatrixTripleProduct(0.0, T, kbInit, 1.0);
    
    // transform the stiffness from the basic to the global system
    theInitStiff.Zero();
    theInitStiff = theCoordTransf->getInitialGlobalStiffMatrix(kbAInit);
    
    return 0;
}


const Matrix& EEBeamColumn3d::getTangentStiff()
{
    if (firstWarning == true)  {
        opserr << "\nWARNING EEBeamColumn3d::getTangentStiff() - "
            << "Element: " << this->getTag() << endln
            << "TangentStiff cannot be calculated." << endln
            << "Return InitialStiff including GeometricStiff instead." 
            << endln;
        opserr << "Subsequent getTangentStiff warnings will be suppressed." 
            << endln;
        
        firstWarning = false;
    }

    // get measured resisting forces
    if (theSite != 0)  {
        (*qMeas) = theSite->getForce();
    }
    else  {
        sData[0] = OF_RemoteTest_getForce;
        theChannel->sendVector(0, 0, *sendData, 0);
        theChannel->recvVector(0, 0, *recvData, 0);
    }
    
    // apply optional initial stiffness modification
    if (iMod == true)  {
        // get measured displacements
        if (theSite != 0)  {
            (*dbMeas) = theSite->getDisp();
        }
        else  {
            sData[0] = OF_RemoteTest_getDisp;
            theChannel->sendVector(0, 0, *sendData, 0);
            theChannel->recvVector(0, 0, *recvData, 0);
        }

        // correct for displacement control errors using I-Modification
        (*qMeas) -= kbInit*((*dbMeas) - (*db));
    } else  {
        // use elastic axial force if axial force from test is zero
        if ((*qMeas)(0) == 0.0)
            (*qMeas)(0) = kbInit(0,0) * (*db)(0);
        // use elastic torsion if torsion from test is zero
        if ((*qMeas)(5) == 0.0)
            (*qMeas)(5) = kbInit(5,5) * (*db)(5);
    }
    
    // transform the forces from basic sys B to basic sys A
    static Vector qA(6);
    qA = T^(*qMeas);
    
    // add initial forces
    for (int i=0; i<6; i++)  {
        qA(i) += qA0[i];
    }

    // transform the stiffness from basic sys B to basic sys A
    static Matrix kbAInit(6,6);
    kbAInit.addMatrixTripleProduct(0.0, T, kbInit, 1.0);

    return theCoordTransf->getGlobalStiffMatrix(kbAInit, qA);
}


const Matrix& EEBeamColumn3d::getMass()
{
    // zero the matrix
    theMatrix.Zero();
    
    // form mass matrix
    if (L != 0.0 && rho != 0.0)  {
        double m = 0.5*rho*L;
        theMatrix(0,0) = m;
        theMatrix(1,1) = m;
        theMatrix(2,2) = m;
        theMatrix(6,6) = m;
        theMatrix(7,7) = m;
        theMatrix(8,8) = m;
    }
    
    return theMatrix; 
}


void EEBeamColumn3d::zeroLoad()
{
    theLoad.Zero();
    
    for (int i=0; i<6; i++)  {
        qA0[i] = 0.0;
        pA0[i] = 0.0;
    }
}


int EEBeamColumn3d::addLoad(ElementalLoad *theEleLoad, double loadFactor)
{  
    int type;
    const Vector &data = theEleLoad->getData(type, loadFactor);

    if (type == LOAD_TAG_Beam3dUniformLoad) {
        double wy = data(0)*loadFactor;  // Transverse
        double wz = data(1)*loadFactor;  // Transverse
        double wx = data(2)*loadFactor;  // Axial (+ve from node I to J)

        double Vy = 0.5*wy*L;
        double Mz = Vy*L/6.0; // wy*L*L/12
        double Vz = 0.5*wz*L;
        double My = Vz*L/6.0; // wz*L*L/12
        double P  = wx*L;

        // Reactions in basic system
        pA0[0] -= P;
        pA0[1] -= Vy;
        pA0[2] -= Vy;
        pA0[3] -= Vz;
        pA0[4] -= Vz;

        // Fixed end forces in basic system
        qA0[0] -= 0.5*P;
        qA0[1] -= Mz;
        qA0[2] += Mz;
        qA0[3] += My;
        qA0[4] -= My;
    }

    else if (type == LOAD_TAG_Beam3dPointLoad) {
        double Py = data(0)*loadFactor;
        double Pz = data(1)*loadFactor;
        double N  = data(2)*loadFactor;
        double aOverL = data(3);

        if (aOverL < 0.0 || aOverL > 1.0)
            return 0;

        double a = aOverL*L;
        double b = L-a;

        // Reactions in basic system
        pA0[0] -= N;
        double V1, V2;
        V1 = Py*(1.0-aOverL);
        V2 = Py*aOverL;
        pA0[1] -= V1;
        pA0[2] -= V2;
        V1 = Pz*(1.0-aOverL);
        V2 = Pz*aOverL;
        pA0[3] -= V1;
        pA0[4] -= V2;

        double L2 = 1.0/(L*L);
        double a2 = a*a;
        double b2 = b*b;

        // Fixed end forces in basic system
        qA0[0] -= N*aOverL;
        double M1, M2;
        M1 = -a * b2 * Py * L2;
        M2 = a2 * b * Py * L2;
        qA0[1] += M1;
        qA0[2] += M2;
        M1 = -a * b2 * Pz * L2;
        M2 = a2 * b * Pz * L2;
        qA0[3] -= M1;
        qA0[4] -= M2;
    }
    else {
        opserr << "EEBeamColumn3d::addLoad() - "
            << "load type unknown for element: "
            << this->getTag() << endln;
        return -1;
    }

    return 0;
}


int EEBeamColumn3d::addInertiaLoadToUnbalance(const Vector &accel)
{
    // check for quick return
    if (L == 0.0 || rho == 0.0)  {
        return 0;
    }    
    
    // get R * accel from the nodes
    const Vector &Raccel1 = theNodes[0]->getRV(accel);
    const Vector &Raccel2 = theNodes[1]->getRV(accel);
    
    if (6 != Raccel1.Size() || 6 != Raccel2.Size())  {
        opserr << "EEBeamColumn3d::addInertiaLoadToUnbalance() - "
            << "matrix and vector sizes are incompatible\n";
        return -1;
    }
    
    // want to add ( - fact * M R * accel ) to unbalance
    // take advantage of lumped mass matrix
    double m = 0.5*rho*L;
    theLoad(0) -= m * Raccel1(0);
    theLoad(1) -= m * Raccel1(1);
    theLoad(2) -= m * Raccel1(2);
    theLoad(6) -= m * Raccel2(0);    
    theLoad(7) -= m * Raccel2(1);
    theLoad(8) -= m * Raccel2(2);
    
    return 0;
}


const Vector& EEBeamColumn3d::getResistingForce()
{
    theCoordTransf->update();
    
    // zero the residual
    theVector.Zero();
    
    // get measured resisting forces
    if (theSite != 0)  {
        (*qMeas) = theSite->getForce();
    }
    else  {
        sData[0] = OF_RemoteTest_getForce;
        theChannel->sendVector(0, 0, *sendData, 0);
        theChannel->recvVector(0, 0, *recvData, 0);
    }
    
    // apply optional initial stiffness modification
    if (iMod == true)  {
        // get measured displacements
        if (theSite != 0)  {
            (*dbMeas) = theSite->getDisp();
        }
        else  {
            sData[0] = OF_RemoteTest_getDisp;
            theChannel->sendVector(0, 0, *sendData, 0);
            theChannel->recvVector(0, 0, *recvData, 0);
        }

        // correct for displacement control errors using I-Modification
        (*qMeas) -= kbInit*((*dbMeas) - (*db));
    } else  {
        // use elastic axial force if axial force from test is zero
        if ((*qMeas)(0) == 0.0)
            (*qMeas)(0) = kbInit(0,0) * (*db)(0);
        // use elastic torsion if torsion from test is zero
        if ((*qMeas)(5) == 0.0)
            (*qMeas)(5) = kbInit(5,5) * (*db)(5);
    }
    
    // save corresponding target displacements for recorder
    dbTarg = (*db);
    vbTarg = (*vb);
    abTarg = (*ab);

    // transform from basic sys B to basic sys A
    static Vector qA(6);
    qA = T^(*qMeas);
    
    // add initial forces
    for (int i=0; i<6; i++)  {
        qA(i) += qA0[i];
    }

    // Vector for reactions in basic system A
    Vector pA0Vec(pA0, 6);
    
    // determine resisting forces in global system
    theVector = theCoordTransf->getGlobalResistingForce(qA, pA0Vec);
    
    // subtract external load
    theVector.addVector(1.0, theLoad, -1.0);
    
    return theVector;
}


const Vector& EEBeamColumn3d::getResistingForceIncInertia()
{	
    theVector = this->getResistingForce();
    
    // add the damping forces if rayleigh damping
    if (alphaM != 0.0 || betaK != 0.0 || betaK0 != 0.0 || betaKc != 0.0)
        theVector += this->getRayleighDampingForces();
    
    // now include the mass portion
    if (L != 0.0 && rho != 0.0)  {
        const Vector &accel1 = theNodes[0]->getTrialAccel();
        const Vector &accel2 = theNodes[1]->getTrialAccel();    
        
        double m = 0.5*rho*L;
        theVector(0) += m * accel1(0);
        theVector(1) += m * accel1(1);
        theVector(2) += m * accel1(2);
        theVector(6) += m * accel2(0);    
        theVector(7) += m * accel2(1);
        theVector(8) += m * accel2(2);
    }
    
    return theVector;
}


const Vector& EEBeamColumn3d::getTime()
{	
    if (theSite != 0)  {
        (*tMeas) = theSite->getTime();
    }
    else  {
        sData[0] = OF_RemoteTest_getTime;
        theChannel->sendVector(0, 0, *sendData, 0);
        theChannel->recvVector(0, 0, *recvData, 0);
    }

    return *tMeas;
}


const Vector& EEBeamColumn3d::getBasicDisp()
{	
    if (theSite != 0)  {
        (*dbMeas) = theSite->getDisp();
    }
    else  {
        sData[0] = OF_RemoteTest_getDisp;
        theChannel->sendVector(0, 0, *sendData, 0);
        theChannel->recvVector(0, 0, *recvData, 0);
    }

    return *dbMeas;
}


const Vector& EEBeamColumn3d::getBasicVel()
{	
    if (theSite != 0)  {
        (*vbMeas) = theSite->getVel();
    }
    else  {
        sData[0] = OF_RemoteTest_getVel;
        theChannel->sendVector(0, 0, *sendData, 0);
        theChannel->recvVector(0, 0, *recvData, 0);
    }

    return *vbMeas;
}


const Vector& EEBeamColumn3d::getBasicAccel()
{	
    if (theSite != 0)  {
        (*abMeas) = theSite->getAccel();
    }
    else  {
        sData[0] = OF_RemoteTest_getAccel;
        theChannel->sendVector(0, 0, *sendData, 0);
        theChannel->recvVector(0, 0, *recvData, 0);
    }

    return *abMeas;
}


int EEBeamColumn3d::sendSelf(int commitTag, Channel &theChannel)
{
    // has not been implemented yet.....
    return 0;
}


int EEBeamColumn3d::recvSelf(int commitTag, Channel &theChannel,
    FEM_ObjectBroker &theBroker)
{
    // has not been implemented yet.....
    return 0;
}


int EEBeamColumn3d::displaySelf(Renderer &theViewer,
    int displayMode, float fact)
{
    // first determine the end points of the beam based on
    // the display factor (a measure of the distorted image)
    const Vector &end1Crd = theNodes[0]->getCrds();
    const Vector &end2Crd = theNodes[1]->getCrds();	
    
    const Vector &end1Disp = theNodes[0]->getDisp();
    const Vector &end2Disp = theNodes[1]->getDisp();
    
    static Vector v1(3);
    static Vector v2(3);
    
    for (int i=0; i<3; i++)  {
        v1(i) = end1Crd(i) + end1Disp(i)*fact;
        v2(i) = end2Crd(i) + end2Disp(i)*fact;
    }
    
    return theViewer.drawLine (v1, v2, 1.0, 1.0);
}


void EEBeamColumn3d::Print(OPS_Stream &s, int flag)
{
    if (flag == 0)  {
        // print everything
        s << "Element: " << this->getTag() << endln;
        s << "  type: EEBeamColumn3d  iNode: " << connectedExternalNodes(0);
        s << "  jNode: " << connectedExternalNodes(1) << endln;
        s << "  CoordTransf: " << theCoordTransf->getTag() << endln;
        if (theSite != 0)
            s << "  ExperimentalSite, tag: " << theSite->getTag() << endln;
        s << "  mass per unit length:  " << rho << endln;
        // determine resisting forces in global system
        s << "  resisting force: " << this->getResistingForce() << endln;
    } else if (flag == 1)  {
        // does nothing
    }
}


Response* EEBeamColumn3d::setResponse(const char **argv, int argc,
    OPS_Stream &output)
{
    Response *theResponse = 0;

    output.tag("ElementOutput");
    output.attr("eleType","EEBeamColumn3d");
    output.attr("eleTag",this->getTag());
    output.attr("node1",connectedExternalNodes[0]);
    output.attr("node2",connectedExternalNodes[1]);

    // global forces
    if (strcmp(argv[0],"force") == 0 || strcmp(argv[0],"forces") == 0 ||
        strcmp(argv[0],"globalForce") == 0 || strcmp(argv[0],"globalForces") == 0)
    {
        output.tag("ResponseType","Px_1");
        output.tag("ResponseType","Py_1");
        output.tag("ResponseType","Pz_1");
        output.tag("ResponseType","Mx_1");
        output.tag("ResponseType","My_1");
        output.tag("ResponseType","Mz_1");
        output.tag("ResponseType","Px_2");
        output.tag("ResponseType","Py_2");
        output.tag("ResponseType","Pz_2");
        output.tag("ResponseType","Mx_2");
        output.tag("ResponseType","My_2");
        output.tag("ResponseType","Mz_2");

        theResponse = new ElementResponse(this, 2, theVector);
    }
    // local forces
    else if (strcmp(argv[0],"localForce") == 0 || strcmp(argv[0],"localForces") == 0)
    {
        output.tag("ResponseType","N_ 1");
        output.tag("ResponseType","Vy_1");
        output.tag("ResponseType","Vz_1");
        output.tag("ResponseType","T_1");
        output.tag("ResponseType","My_1");
        output.tag("ResponseType","Tz_1");
        output.tag("ResponseType","N_2");
        output.tag("ResponseType","Py_2");
        output.tag("ResponseType","Pz_2");
        output.tag("ResponseType","T_2");
        output.tag("ResponseType","My_2");
        output.tag("ResponseType","Mz_2");

        theResponse = new ElementResponse(this, 3, theVector);
    }
    // forces in basic system B
    else if (strcmp(argv[0],"basicForce") == 0 || strcmp(argv[0],"basicForces") == 0)
    {
        output.tag("ResponseType","q1");
        output.tag("ResponseType","q2");
        output.tag("ResponseType","q3");
        output.tag("ResponseType","q4");
        output.tag("ResponseType","q5");
        output.tag("ResponseType","q6");

        theResponse = new ElementResponse(this, 4, Vector(6));
    }
    // target displacements in basic system B
    else if (strcmp(argv[0],"deformation") == 0 || strcmp(argv[0],"deformations") == 0 || 
        strcmp(argv[0],"basicDeformation") == 0 || strcmp(argv[0],"basicDeformations") == 0 ||
        strcmp(argv[0],"targetDisplacement") == 0 || strcmp(argv[0],"targetDisplacements") == 0)
    {
        output.tag("ResponseType","db1");
        output.tag("ResponseType","db2");
        output.tag("ResponseType","db3");
        output.tag("ResponseType","db4");
        output.tag("ResponseType","db5");
        output.tag("ResponseType","db6");

        theResponse = new ElementResponse(this, 5, Vector(6));
    }
    // target velocities in basic system B
    else if (strcmp(argv[0],"targetVelocity") == 0 ||
        strcmp(argv[0],"targetVelocities") == 0)
    {
        output.tag("ResponseType","vb1");
        output.tag("ResponseType","vb2");
        output.tag("ResponseType","vb3");
        output.tag("ResponseType","vb4");
        output.tag("ResponseType","vb5");
        output.tag("ResponseType","vb6");

        theResponse = new ElementResponse(this, 6, Vector(6));
    }
    // target accelerations in basic system B
    else if (strcmp(argv[0],"targetAcceleration") == 0 ||
        strcmp(argv[0],"targetAccelerations") == 0)
    {
        output.tag("ResponseType","ab1");
        output.tag("ResponseType","ab2");
        output.tag("ResponseType","ab3");
        output.tag("ResponseType","ab4");
        output.tag("ResponseType","ab5");
        output.tag("ResponseType","ab6");

        theResponse = new ElementResponse(this, 7, Vector(6));
    }
    // measured displacements in basic system B
    else if (strcmp(argv[0],"measuredDisplacement") == 0 || 
        strcmp(argv[0],"measuredDisplacements") == 0)
    {
        output.tag("ResponseType","dbm1");
        output.tag("ResponseType","dbm2");
        output.tag("ResponseType","dbm3");
        output.tag("ResponseType","dbm4");
        output.tag("ResponseType","dbm5");
        output.tag("ResponseType","dbm6");

        theResponse = new ElementResponse(this, 8, Vector(6));
    }
    // measured velocities in basic system B
    else if (strcmp(argv[0],"measuredVelocity") == 0 || 
        strcmp(argv[0],"measuredVelocities") == 0)
    {
        output.tag("ResponseType","vbm1");
        output.tag("ResponseType","vbm2");
        output.tag("ResponseType","vbm3");
        output.tag("ResponseType","vbm4");
        output.tag("ResponseType","vbm5");
        output.tag("ResponseType","vbm6");

        theResponse = new ElementResponse(this, 9, Vector(6));
    }
    // measured accelerations in basic system B
    else if (strcmp(argv[0],"measuredAcceleration") == 0 || 
        strcmp(argv[0],"measuredAccelerations") == 0)
    {
        output.tag("ResponseType","abm1");
        output.tag("ResponseType","abm2");
        output.tag("ResponseType","abm3");
        output.tag("ResponseType","abm4");
        output.tag("ResponseType","abm5");
        output.tag("ResponseType","abm6");

        theResponse = new ElementResponse(this, 10, Vector(6));
    }

    output.endTag(); // ElementOutput

    return theResponse;
}


int EEBeamColumn3d::getResponse(int responseID, Information &eleInformation)
{
    double L = theCoordTransf->getInitialLength();
    
    switch (responseID)  {
    case -1:
        return -1;
        
    case 1:  // initial stiffness
        if (eleInformation.theMatrix != 0)  {
            *(eleInformation.theMatrix) = theInitStiff;
        }
        return 0;
        
    case 2:  // global forces
        if (eleInformation.theVector != 0)  {
            *(eleInformation.theVector) = this->getResistingForce();
        }
        return 0;      
        
    case 3:  // local forces
        if (eleInformation.theVector != 0)  {
            // transform from basic sys B to basic sys A
            static Vector qA(6);
            qA = T^(*qMeas);
            // Axial
            theVector(0)  = -qA(0) + pA0[0];
            theVector(6)  =  qA(0);
            // Torsion
            theVector(3)  = -qA(5) + pA0[5];
            theVector(9)  =  qA(5);
            // Shear Vy
            theVector(1)  =  (qA(1)+qA(2))/L + pA0[1];
            theVector(7)  = -(qA(1)+qA(2))/L + pA0[2];
            // Moment Mz
            theVector(5)  =  qA(1);
            theVector(11) =  qA(2);
            // Shear Vz
            theVector(2)  = -(qA(3)+qA(4))/L + pA0[3];
            theVector(8)  =  (qA(3)+qA(4))/L + pA0[4];
            // Moment My
            theVector(4)  =  qA(3);
            theVector(10) =  qA(4);
            
            *(eleInformation.theVector) = theVector;
        }
        return 0;      
        
    case 4:  // forces in basic system B
        if (eleInformation.theVector != 0)  {
            *(eleInformation.theVector) = (*qMeas);
        }
        return 0;      
        
    case 5:  // target displacements in basic system B
        if (eleInformation.theVector != 0)  {
            *(eleInformation.theVector) = dbTarg;
        }
        return 0;      
        
    case 6:  // target velocities in basic system B
        if (eleInformation.theVector != 0)  {
            *(eleInformation.theVector) = vbTarg;
        }
        return 0;      
        
    case 7:  // target accelerations in basic system B
        if (eleInformation.theVector != 0)  {
            *(eleInformation.theVector) = abTarg;
        }
        return 0;      
        
    case 8:  // measured displacements in basic system B
        if (eleInformation.theVector != 0)  {
            *(eleInformation.theVector) = this->getBasicDisp();
        }
        return 0;

    case 9:  // measured velocities in basic system B
        if (eleInformation.theVector != 0)  {
            *(eleInformation.theVector) = this->getBasicVel();
        }
        return 0;

    case 10:  // measured accelerations in basic system B
        if (eleInformation.theVector != 0)  {
            *(eleInformation.theVector) = this->getBasicAccel();
        }
        return 0;

    default:
        return -1;
    }
}
