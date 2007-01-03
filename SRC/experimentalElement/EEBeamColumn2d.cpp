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
// Description: This file contains the implementation of the EEBeamColumn2d class.

#include "EEBeamColumn2d.h"

#include <ArrayOfTaggedObjects.h>
#include <Domain.h>
#include <Node.h>
#include <Channel.h>
#include <FEM_ObjectBroker.h>
#include <Renderer.h>
#include <Information.h>
#include <ElementResponse.h>
#include <CrdTransf2d.h>
#include <ElementalLoad.h>
#include <TCP_Socket.h>

#include <math.h>
#include <stdlib.h>
#include <string.h>


// initialize the class wide variables
Matrix EEBeamColumn2d::theMatrix(6,6);
Vector EEBeamColumn2d::theVector(6);
Vector EEBeamColumn2d::theLoad(6);


// responsible for allocating the necessary space needed
// by each object and storing the tags of the end nodes.
EEBeamColumn2d::EEBeamColumn2d(int tag, int Nd1, int Nd2,
    CrdTransf2d &coordTransf,
    ExperimentalSite *site,
    bool iM, double r)
    : ExperimentalElement(tag, ELE_TAG_EEBeamColumn2d, site),
    connectedExternalNodes(2), theCoordTransf(0),
    iMod(iM), rho(r), L(0.0),
    db(0), vb(0), ab(0), t(0),
    dbMeas(0), vbMeas(0), abMeas(0), qMeas(0), tMeas(0),
    dbTarg(3), dbPast(3), kbInit(3,3), T(3,3), Tinv(3,3)
{
    // ensure the connectedExternalNode ID is of correct size & set values
    if (connectedExternalNodes.Size() != 2)  {
        opserr << "EEBeamColumn2d::EEBeamColumn2d() - element: "
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
    
    (*sizeCtrl)[OF_Resp_Disp]  = 3;
    (*sizeCtrl)[OF_Resp_Vel]   = 3;
    (*sizeCtrl)[OF_Resp_Accel] = 3;
    (*sizeCtrl)[OF_Resp_Time]  = 1;
    
    (*sizeDaq)[OF_Resp_Disp]   = 3;
    (*sizeDaq)[OF_Resp_Vel]    = 3;
    (*sizeDaq)[OF_Resp_Accel]  = 3;
    (*sizeDaq)[OF_Resp_Force]  = 3;
    (*sizeDaq)[OF_Resp_Time]   = 1;
    
    theSite->setSize(*sizeCtrl, *sizeDaq);
    
    // allocate memory for trial response vectors
    db = new Vector(3);
    vb = new Vector(3);
    ab = new Vector(3);
    t  = new Vector(1);

    // allocate memory for measured response vectors
    dbMeas = new Vector(3);
    vbMeas = new Vector(3);
    abMeas = new Vector(3);
    qMeas  = new Vector(3);
    tMeas  = new Vector(1);

    // set the initial stiffness matrix size
    theInitStiff.resize(6,6);
    
    // get a copy of the coordinate transformation
    theCoordTransf = coordTransf.getCopy();
    if (!theCoordTransf)  {
        opserr << "EEBeamColumn2d::EEBeamColumn2d() - "
            << "failed to get copy of the coordinate transformation\n";
        exit(-1);
    }
    
    // initialize additional vectors
    dbTarg.Zero();
    dbPast.Zero();
    for (i=0; i<3; i++)  {
        qA0[i] = 0.0;
        pA0[i] = 0.0;
    }
}


// responsible for allocating the necessary space needed
// by each object and storing the tags of the end nodes.
EEBeamColumn2d::EEBeamColumn2d(int tag, int Nd1, int Nd2,
    CrdTransf2d &coordTransf,
    int port, char *machineInetAddr, int dataSize,
    bool iM, double r)
    : ExperimentalElement(tag, ELE_TAG_EEBeamColumn2d),
    connectedExternalNodes(2), theCoordTransf(0),
    iMod(iM), rho(r), L(0.0),
    theSocket(0), sData(0), sendData(0), rData(0), recvData(0),
    db(0), vb(0), ab(0), t(0),
    dbMeas(0), vbMeas(0), abMeas(0), qMeas(0), tMeas(0),
    dbTarg(3), dbPast(3), kbInit(3,3), T(3,3), Tinv(3,3)
{
    // ensure the connectedExternalNode ID is of correct size & set values
    if (connectedExternalNodes.Size() != 2)  {
        opserr << "EEBeamColumn2d::EEBeamColumn2d() - element: "
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
    if (machineInetAddr == 0)
        theSocket = new TCP_Socket(port, "127.0.0.1");
    else
        theSocket = new TCP_Socket(port, machineInetAddr);
    theSocket->setUpConnection();

    // set the data size for the experimental site
    int intData[2*OF_Resp_All+1];
    ID idData(intData, 2*OF_Resp_All+1);
    sizeCtrl = new ID(intData, OF_Resp_All);
    sizeDaq = new ID(&intData[OF_Resp_All], OF_Resp_All);
    idData.Zero();
    
    (*sizeCtrl)[OF_Resp_Disp]  = 3;
    (*sizeCtrl)[OF_Resp_Vel]   = 3;
    (*sizeCtrl)[OF_Resp_Accel] = 3;
    (*sizeCtrl)[OF_Resp_Time]  = 1;
    
    (*sizeDaq)[OF_Resp_Disp]   = 3;
    (*sizeDaq)[OF_Resp_Vel]    = 3;
    (*sizeDaq)[OF_Resp_Accel]  = 3;
    (*sizeDaq)[OF_Resp_Force]  = 3;
    (*sizeDaq)[OF_Resp_Time]   = 1;
    
    dataSize = (dataSize<13) ? 13 : dataSize;
    intData[2*OF_Resp_All] = dataSize;

    theSocket->sendID(0, 0, idData, 0);
    
    // allocate memory for the send vectors
    int id = 1;
    sData = new double [dataSize];
    sendData = new Vector(sData, dataSize);
    db = new Vector(&sData[id], 3);
    id += 3;
    vb = new Vector(&sData[id], 3);
    id += 3;
    ab = new Vector(&sData[id], 3);
    id += 3;
    t = new Vector(&sData[id], 1);
    sendData->Zero();

    // allocate memory for the receive vectors
    id = 0;
    rData = new double [dataSize];
    recvData = new Vector(rData, dataSize);
    dbMeas = new Vector(&rData[id], 1);
    id += 3;
    vbMeas = new Vector(&rData[id], 1);
    id += 3;
    abMeas = new Vector(&rData[id], 1);
    id += 3;
    qMeas = new Vector(&rData[id], 1);
    id += 3;
    tMeas = new Vector(&rData[id], 1);
    recvData->Zero();

    // set the initial stiffness matrix size
    theInitStiff.resize(6,6);
    
    // get a copy of the coordinate transformation
    theCoordTransf = coordTransf.getCopy();
    if (!theCoordTransf)  {
        opserr << "EEBeamColumn2d::EEBeamColumn2d() - "
            << "failed to get copy of the coordinate transformation\n";
        exit(-1);
    }
    
    // initialize additional vectors
    dbTarg.Zero();
    dbPast.Zero();
    for (i=0; i<3; i++)  {
        qA0[i] = 0.0;
        pA0[i] = 0.0;
    }
}


// delete must be invoked on any objects created by the object.
EEBeamColumn2d::~EEBeamColumn2d()
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
        theSocket->sendVector(0, 0, *sendData, 0);
        
        if (sendData != 0)
            delete sendData;
        if (sData != 0)
            delete [] sData;
        if (recvData != 0)
            delete recvData;
        if (rData != 0)
            delete [] rData;
        if (theSocket != 0)
            delete theSocket;
    }
}


int EEBeamColumn2d::getNumExternalNodes() const
{
    return 2;
}


const ID& EEBeamColumn2d::getExternalNodes() 
{
    return connectedExternalNodes;
}


Node** EEBeamColumn2d::getNodePtrs() 
{
    return theNodes;
}


int EEBeamColumn2d::getNumDOF() 
{
    return 6;
}


int EEBeamColumn2d::getNumBasicDOF() 
{
    return 3;
}


// to set a link to the enclosing Domain and to set the node pointers.
void EEBeamColumn2d::setDomain(Domain *theDomain)
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
            opserr << "EEBeamColumn2d::setDomain() - Nd1: " 
                << connectedExternalNodes(0) << " does not exist in the model for ";
        } else  {
            opserr << "EEBeamColumn2d::setDomain() - Nd2: " 
                << connectedExternalNodes(1) << " does not exist in the model for ";
        }
        opserr << "EEBeamColumn2d ele: " << this->getTag() << endln;
        
        return;
    }
    
    // now determine the number of dof and the dimension    
    int dofNd1 = theNodes[0]->getNumberDOF();
    int dofNd2 = theNodes[1]->getNumberDOF();	
    
    // if differing dof at the ends - print a warning message
    if (dofNd1 != 3)  {
        opserr << "EEBeamColumn2d::setDomain() - node 1: "
            << connectedExternalNodes(0) << " has incorrect number of DOF (not 3)\n";
        return;
    }
    if (dofNd2 != 3)  {
        opserr << "EEBeamColumn2d::setDomain() - node 2: "
            << connectedExternalNodes(1) << " has incorrect number of DOF (not 3)\n";
        return;
    }
    
    // call the base class method
    this->DomainComponent::setDomain(theDomain);
    
    // initialize the coordinate transformation
    if (theCoordTransf->initialize(theNodes[0], theNodes[1]) != 0)  {
        opserr << "EEBeamColumn2d::setDomain() - "
            << "error initializing coordinate transformation\n";
        return;
    }
    
    // get initial element length
    double L = theCoordTransf->getInitialLength();
    if (L == 0.0)  {
        opserr << "EEBeamColumn2d::setDomain() - element: " 
            << this->getTag() << " has zero length\n";
        return;
    }
    
    // set transformation matrix from basic sys A to basic sys B
    T.Zero();
    T(0,0) =  1;
    T(1,1) = -L;
    T(2,1) = -1;  T(2,2) = 1;
    
    // set transformation matrix from basic sys B to basic sys A
    Tinv.Zero();
    Tinv(0,0) =  1;
    Tinv(1,1) = -1/L;
    Tinv(2,1) = -1/L;  T(2,2) = 1;
}   	 


int EEBeamColumn2d::commitState()
{
    int rValue = 0;
    
    if (theSite != 0)  {
        rValue += theSite->commitState();
    }
    else  {
        sData[0] = OF_RemoteTest_commitState;
        rValue += theSocket->sendVector(0, 0, *sendData, 0);
    }
    rValue += theCoordTransf->commitState();
    
    return rValue;
}


int EEBeamColumn2d::update()
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
            rValue += theSocket->sendVector(0, 0, *sendData, 0);
        }
    }
    
    return rValue;
}


int EEBeamColumn2d::setInitialStiff(const Matrix &kbinit)
{
    if (kbinit.noRows() != 3 || kbinit.noCols() != 3)  {
        opserr << "EEBeamColumn2d::setInitialStiff() - " 
            << "matrix size is incorrect for element: "
            << this->getTag() << endln;
        return -1;
    }
    kbInit = kbinit;
        
    // transform the stiffness from basic sys B to basic sys A
    static Matrix kbAInit(3,3);
    kbAInit.addMatrixTripleProduct(0.0, T, kbInit, 1.0);
    
    // transform the stiffness from the basic to the global system
    theInitStiff.Zero();
    theInitStiff = theCoordTransf->getInitialGlobalStiffMatrix(kbAInit);
    
    return 0;
}


const Matrix& EEBeamColumn2d::getMass()
{
    // zero the matrix
    theMatrix.Zero();
    
    // form mass matrix
    if (L != 0.0 && rho != 0.0)  {
        double m = 0.5*rho*L;
        theMatrix(0,0) = m;
        theMatrix(1,1) = m;	
        theMatrix(3,3) = m;
        theMatrix(4,4) = m;
    }
    
    return theMatrix; 
}


void EEBeamColumn2d::zeroLoad()
{
    theLoad.Zero();
    
    for (int i=0; i<3; i++)  {
        qA0[i] = 0.0;
        pA0[i] = 0.0;
    }
}


int EEBeamColumn2d::addLoad(ElementalLoad *theEleLoad, double loadFactor)
{  
    int type;
    const Vector &data = theEleLoad->getData(type, loadFactor);

    if (type == LOAD_TAG_Beam2dUniformLoad) {
        double wt = data(0)*loadFactor;  // Transverse (+ve upward)
        double wa = data(1)*loadFactor;  // Axial (+ve from node I to J)

        double V = 0.5*wt*L;
        double M = V*L/6.0; // wt*L*L/12
        double P = wa*L;

        // Reactions in basic system
        pA0[0] -= P;
        pA0[1] -= V;
        pA0[2] -= V;

        // Fixed end forces in basic system
        qA0[0] -= 0.5*P;
        qA0[1] -= M;
        qA0[2] += M;
    }

    else if (type == LOAD_TAG_Beam2dPointLoad) {
        double P = data(0)*loadFactor;
        double N = data(1)*loadFactor;
        double aOverL = data(2);

        if (aOverL < 0.0 || aOverL > 1.0)
            return 0;

        double a = aOverL*L;
        double b = L-a;

        // Reactions in basic system
        pA0[0] -= N;
        double V1 = P*(1.0-aOverL);
        double V2 = P*aOverL;
        pA0[1] -= V1;
        pA0[2] -= V2;

        double L2 = 1.0/(L*L);
        double a2 = a*a;
        double b2 = b*b;

        // Fixed end forces in basic system
        qA0[0] -= N*aOverL;
        double M1 = -a*b2*P*L2;
        double M2 = a2*b*P*L2;
        qA0[1] += M1;
        qA0[2] += M2;
    }

    else {
        opserr << "EEBeamColumn2d::addLoad() - "
            << "load type unknown for element: "
            << this->getTag() << endln;
        return -1;
    }

    return 0;
}


int EEBeamColumn2d::addInertiaLoadToUnbalance(const Vector &accel)
{
    // check for quick return
    if (L == 0.0 || rho == 0.0)  {
        return 0;
    }    
    
    // get R * accel from the nodes
    const Vector &Raccel1 = theNodes[0]->getRV(accel);
    const Vector &Raccel2 = theNodes[1]->getRV(accel);
    
    if (3 != Raccel1.Size() || 3 != Raccel2.Size())  {
        opserr << "EEBeamColumn2d::addInertiaLoadToUnbalance() - "
            << "matrix and vector sizes are incompatible\n";
        return -1;
    }
    
    // want to add ( - fact * M R * accel ) to unbalance
    // take advantage of lumped mass matrix
    double m = 0.5*rho*L;
    theLoad(0) -= m * Raccel1(0);
    theLoad(1) -= m * Raccel1(1);
    theLoad(3) -= m * Raccel2(0);    
    theLoad(4) -= m * Raccel2(1);    
    
    return 0;
}


const Vector& EEBeamColumn2d::getResistingForce()
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
        theSocket->sendVector(0, 0, *sendData, 0);
        theSocket->recvVector(0, 0, *recvData, 0);
    }

    // apply optional initial stiffness modification
    if (iMod == true)  {
        // get measured displacements
        if (theSite != 0)  {
            (*dbMeas) = theSite->getDisp();
        }
        else  {
            sData[0] = OF_RemoteTest_getDisp;
            theSocket->sendVector(0, 0, *sendData, 0);
            theSocket->recvVector(0, 0, *recvData, 0);
        }

        // correct for displacement control errors using I-Modification
        (*qMeas) -= kbInit*((*dbMeas) - (*db));
    } else  {
        // use elastic axial force if axial force from test is zero
        if ((*qMeas)(0) == 0.0)
            (*qMeas)(0) = kbInit(0,0) * (*db)(0);
    }
    
    // save corresponding target displacements for recorder
    dbTarg = (*db);

    // transform from basic sys B to basic sys A
    static Vector qA(3);
    qA = T^(*qMeas);
    
    // add fixed end forces
    for (int i=0; i<3; i++)  {
        qA(i) += qA0[i];
    }

    // Vector for reactions in basic system A
    Vector pA0Vec(pA0, 3);
    
    // determine resisting forces in global system
    theVector = theCoordTransf->getGlobalResistingForce(qA, pA0Vec);
    
    // subtract external load
    theVector.addVector(1.0, theLoad, -1.0);
    
    return theVector;
}


const Vector& EEBeamColumn2d::getResistingForceIncInertia()
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
        theVector(3) += m * accel2(0);    
        theVector(4) += m * accel2(1);
    }
    
    return theVector;
}


const Vector& EEBeamColumn2d::getTime()
{	
    if (theSite != 0)  {
        (*tMeas) = theSite->getTime();
    }
    else  {
        sData[0] = OF_RemoteTest_getTime;
        theSocket->sendVector(0, 0, *sendData, 0);
        theSocket->recvVector(0, 0, *recvData, 0);
    }

    return *tMeas;
}


const Vector& EEBeamColumn2d::getBasicDisp()
{	
    if (theSite != 0)  {
        (*dbMeas) = theSite->getDisp();
    }
    else  {
        sData[0] = OF_RemoteTest_getDisp;
        theSocket->sendVector(0, 0, *sendData, 0);
        theSocket->recvVector(0, 0, *recvData, 0);
    }

    return *dbMeas;
}


const Vector& EEBeamColumn2d::getBasicVel()
{	
    if (theSite != 0)  {
        (*vbMeas) = theSite->getVel();
    }
    else  {
        sData[0] = OF_RemoteTest_getVel;
        theSocket->sendVector(0, 0, *sendData, 0);
        theSocket->recvVector(0, 0, *recvData, 0);
    }

    return *vbMeas;
}


const Vector& EEBeamColumn2d::getBasicAccel()
{	
    if (theSite != 0)  {
        (*abMeas) = theSite->getAccel();
    }
    else  {
        sData[0] = OF_RemoteTest_getAccel;
        theSocket->sendVector(0, 0, *sendData, 0);
        theSocket->recvVector(0, 0, *recvData, 0);
    }

    return *abMeas;
}


int EEBeamColumn2d::sendSelf(int commitTag, Channel &theChannel)
{
    // has not been implemented yet.....
    return 0;
}


int EEBeamColumn2d::recvSelf(int commitTag, Channel &theChannel,
    FEM_ObjectBroker &theBroker)
{
    // has not been implemented yet.....
    return 0;
}


int EEBeamColumn2d::displaySelf(Renderer &theViewer,
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
    
    for (int i=0; i<2; i++)  {
        v1(i) = end1Crd(i) + end1Disp(i)*fact;
        v2(i) = end2Crd(i) + end2Disp(i)*fact;    
    }
    
    return theViewer.drawLine (v1, v2, 1.0, 1.0);
}


void EEBeamColumn2d::Print(OPS_Stream &s, int flag)
{
    if (flag == 0)  {
        // print everything
        s << "Element: " << this->getTag() << endln;
        s << "  type: EEBeamColumn2d  iNode: " << connectedExternalNodes(0);
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


Response* EEBeamColumn2d::setResponse(const char **argv, int argc,
    Information &eleInformation, OPS_Stream &output)
{
    Response *theResponse = 0;

    output.tag("ElementOutput");
    output.attr("eleType","EEBeamColumn2d");
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

        theResponse = new ElementResponse(this, 2, theVector);
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

        theResponse = new ElementResponse(this, 3, theVector);
    }
    // forces in basic system B
    else if (strcmp(argv[0],"basicForce") == 0 || strcmp(argv[0],"basicForces") == 0)
    {
        output.tag("ResponseType","q1");
        output.tag("ResponseType","q2");
        output.tag("ResponseType","q3");

        theResponse = new ElementResponse(this, 4, Vector(3));
    }
    // deformations in basic system B
    else if (strcmp(argv[0],"deformation") == 0 || strcmp(argv[0],"deformations") == 0 || 
        strcmp(argv[0],"basicDeformation") == 0 || strcmp(argv[0],"basicDeformations") == 0 ||
        strcmp(argv[0],"targetDisplacement") == 0 || strcmp(argv[0],"targetDisplacements") == 0)
    {
        output.tag("ResponseType","db1");
        output.tag("ResponseType","db2");
        output.tag("ResponseType","db3");

        theResponse = new ElementResponse(this, 5, Vector(3));
    }
    // measured displacements in basic system B
    else if (strcmp(argv[0],"measuredDisplacement") == 0 || 
        strcmp(argv[0],"measuredDisplacements") == 0)
    {
        output.tag("ResponseType","dbm1");
        output.tag("ResponseType","dbm2");
        output.tag("ResponseType","dbm3");

        theResponse = new ElementResponse(this, 6, Vector(3));
    }
    // measured velocities in basic system B
    else if (strcmp(argv[0],"measuredVelocity") == 0 || 
        strcmp(argv[0],"measuredVelocities") == 0)
    {
        output.tag("ResponseType","vbm1");
        output.tag("ResponseType","vbm2");
        output.tag("ResponseType","vbm3");

        theResponse = new ElementResponse(this, 7, Vector(3));
    }
    // measured accelerations in basic system B
    else if (strcmp(argv[0],"measuredAcceleration") == 0 || 
        strcmp(argv[0],"measuredAccelerations") == 0)
    {
        output.tag("ResponseType","abm1");
        output.tag("ResponseType","abm2");
        output.tag("ResponseType","abm3");

        theResponse = new ElementResponse(this, 8, Vector(3));
    }

    output.endTag(); // ElementOutput

    return theResponse;
}


int EEBeamColumn2d::getResponse(int responseID, Information &eleInformation)
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
            static Vector qA(3);
            qA = T^(*qMeas);
            // Axial
            theVector(0) = -qA(0) + pA0[0];
            theVector(3) =  qA(0);
            // Shear
            theVector(1) =  (qA(1)+qA(2))/L + pA0[1];
            theVector(4) = -(qA(1)+qA(2))/L + pA0[2];
            // Moment
            theVector(2) =  qA(1);
            theVector(5) =  qA(2);
            
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
        
    case 6:  // measured displacements in basic system B
        if (eleInformation.theVector != 0)  {
            *(eleInformation.theVector) = this->getBasicDisp();
        }
        return 0;

    case 7:  // measured velocities in basic system B
        if (eleInformation.theVector != 0)  {
            *(eleInformation.theVector) = this->getBasicVel();
        }
        return 0;

    case 8:  // measured accelerations in basic system B
        if (eleInformation.theVector != 0)  {
            *(eleInformation.theVector) = this->getBasicAccel();
        }
        return 0;

    default:
        return -1;
    }
}
