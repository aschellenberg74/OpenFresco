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
// Description: This file contains the implementation of the EETwoNodeLink class.

#include "EETwoNodeLink.h"

#include <Domain.h>
#include <Node.h>
#include <Channel.h>
#include <FEM_ObjectBroker.h>
#include <Renderer.h>
#include <Information.h>
#include <ElementResponse.h>
#include <TCP_Socket.h>
#include <TCP_SocketSSL.h>

#include <math.h>
#include <stdlib.h>
#include <string.h>


// initialise the class wide variables
Matrix EETwoNodeLink::EETwoNodeLinkM2(2,2);
Matrix EETwoNodeLink::EETwoNodeLinkM4(4,4);
Matrix EETwoNodeLink::EETwoNodeLinkM6(6,6);
Matrix EETwoNodeLink::EETwoNodeLinkM12(12,12);
Vector EETwoNodeLink::EETwoNodeLinkV2(2);
Vector EETwoNodeLink::EETwoNodeLinkV4(4);
Vector EETwoNodeLink::EETwoNodeLinkV6(6);
Vector EETwoNodeLink::EETwoNodeLinkV12(12);


// responsible for allocating the necessary space needed
// by each object and storing the tags of the end nodes.
EETwoNodeLink::EETwoNodeLink(int tag, int dim, int Nd1, int Nd2, 
    const ID &direction, const Vector &x, const Vector &yp,
    ExperimentalSite *site,
    bool iM, double m)
    : ExperimentalElement(tag, ELE_TAG_EETwoNodeLink, site),     
    dimension(dim), numDOF(0),
    connectedExternalNodes(2),
    numDir(direction.Size()), dir(0), transformation(3,3),
    iMod(iM), mass(m),
    theMatrix(0), theVector(0), theLoad(0),
    db(0), vb(0), ab(0), t(0),
    dbMeas(0), vbMeas(0), abMeas(0), qMeas(0), tMeas(0),
    dbTarg(direction.Size()), vbTarg(direction.Size()),
    abTarg(direction.Size()), dbPast(direction.Size()),
    kbInit(direction.Size(), direction.Size()), T(0,0)
{
    // establish the connected nodes and set up the transformation matrix for orientation
    this->setUp(Nd1, Nd2, x, yp);
    
    // allocate memory for direction array
    dir = new ID(numDir);
    
    if (dir == 0)  {
        opserr << "EETwoNodeLink::EETwoNodeLink() - failed to creat direction array\n";
        exit(-1);
    }
    
    // initialize directions and check for valid values
    (*dir) = direction;
    for (int i=0; i<numDir; i++)  {
        if ((*dir)(i) < 0 || (*dir)(i) > 5)  {
            opserr << "EETwoNodeLink::EETwoNodeLink() - incorrect direction "
                << (*dir)(i) << " is set to 0\n";
            (*dir)(i) = 0;
        }
    }
    
    // set the data size for the experimental site
    sizeCtrl = new ID(OF_Resp_All);
    sizeDaq = new ID(OF_Resp_All);
    
    (*sizeCtrl)[OF_Resp_Disp]  = numDir;
    (*sizeCtrl)[OF_Resp_Vel]   = numDir;
    (*sizeCtrl)[OF_Resp_Accel] = numDir;
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
    t  = new Vector(1);

    // allocate memory for measured response vectors
    dbMeas = new Vector(numDir);
    vbMeas = new Vector(numDir);
    abMeas = new Vector(numDir);
    qMeas  = new Vector(numDir);
    tMeas  = new Vector(1);

    // initialize additional vectors
    dbTarg.Zero();
    vbTarg.Zero();
    abTarg.Zero();
    dbPast.Zero();
}


// responsible for allocating the necessary space needed
// by each object and storing the tags of the end nodes.
EETwoNodeLink::EETwoNodeLink(int tag, int dim, int Nd1, int Nd2, 
    const ID &direction, const Vector &x, const Vector &yp,
    int port, char *machineInetAddr, int ssl, int dataSize,
    bool iM, double m)
    : ExperimentalElement(tag, ELE_TAG_EETwoNodeLink),     
    dimension(dim), numDOF(0),
    connectedExternalNodes(2),
    numDir(direction.Size()), dir(0), transformation(3,3),
    iMod(iM), mass(m),
    theMatrix(0), theVector(0), theLoad(0),
    theChannel(0), sData(0), sendData(0), rData(0), recvData(0),
    db(0), vb(0), ab(0), t(0),
    dbMeas(0), vbMeas(0), abMeas(0), qMeas(0), tMeas(0),
    dbTarg(direction.Size()), vbTarg(direction.Size()),
    abTarg(direction.Size()), dbPast(direction.Size()),
    kbInit(direction.Size(), direction.Size()), T(0,0)
{
    // establish the connected nodes and set up the transformation matrix for orientation
    this->setUp(Nd1, Nd2, x, yp);
    
    // allocate memory for direction array
    dir = new ID(numDir);
    
    if (dir == 0)  {
        opserr << "EETwoNodeLink::EETwoNodeLink() - failed to creat direction array\n";
        exit(-1);
    }
    
    // initialize directions and check for valid values
    (*dir) = direction;
    for (int i=0; i<numDir; i++)  {
        if ((*dir)(i) < 0 || (*dir)(i) > 5)  {
            opserr << "EETwoNodeLink::EETwoNodeLink() - incorrect direction "
                << (*dir)(i) << " is set to 0\n";
            (*dir)(i) = 0;
        }
    }
    
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
        opserr << "EETwoNodeLink::EETwoNodeLink() "
            << "- failed to create channel\n";
        exit(-1);
    }
    if (theChannel->setUpConnection() != 0)  {
        opserr << "EETwoNodeLink::EETwoNodeLink() "
            << "- failed to setup connection\n";
        exit(-1);
    }

    // set the data size for the experimental site
    int intData[2*OF_Resp_All+1];
    ID idData(intData, 2*OF_Resp_All+1);
    sizeCtrl = new ID(intData, OF_Resp_All);
    sizeDaq = new ID(&intData[OF_Resp_All], OF_Resp_All);
    idData.Zero();

    (*sizeCtrl)[OF_Resp_Disp]  = numDir;
    (*sizeCtrl)[OF_Resp_Vel]   = numDir;
    (*sizeCtrl)[OF_Resp_Accel] = numDir;
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
    t = new Vector(&sData[id], 1);
    sendData->Zero();

    // allocate memory for the receive vectors
    id = 0;
    rData = new double [dataSize];
    recvData = new Vector(rData, dataSize);
    dbMeas = new Vector(&rData[id], 1);
    id += numDir;
    vbMeas = new Vector(&rData[id], 1);
    id += numDir;
    abMeas = new Vector(&rData[id], 1);
    id += numDir;
    qMeas = new Vector(&rData[id], 1);
    id += numDir;
    tMeas = new Vector(&rData[id], 1);
    recvData->Zero();

    // initialize additional vectors
    dbTarg.Zero();
    vbTarg.Zero();
    abTarg.Zero();
    dbPast.Zero();
}


// delete must be invoked on any objects created by the object.
EETwoNodeLink::~EETwoNodeLink()
{
    // invoke the destructor on any objects created by the object
    // that the object still holds a pointer to
    if (dir != 0)
        delete dir;
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


int EETwoNodeLink::getNumExternalNodes() const
{
    return 2;
}


const ID& EETwoNodeLink::getExternalNodes() 
{
    return connectedExternalNodes;
}


Node** EETwoNodeLink::getNodePtrs() 
{
    return theNodes;
}


int EETwoNodeLink::getNumDOF() 
{
    return numDOF;
}


int EETwoNodeLink::getNumBasicDOF() 
{
    return numDir;
}


// to set a link to the enclosing Domain and to set the node pointers.
void EETwoNodeLink::setDomain(Domain *theDomain)
{
    // check Domain is not null - invoked when object removed from a domain
    if (theDomain == 0)  {
        theNodes[0] = 0;
        theNodes[1] = 0;
        
        return;
    }
    
    // set default values for error conditions
    numDOF = 2;
    theMatrix = &EETwoNodeLinkM2;
    theVector = &EETwoNodeLinkV2;
    
    // first set the node pointers
    int Nd1 = connectedExternalNodes(0);
    int Nd2 = connectedExternalNodes(1);
    theNodes[0] = theDomain->getNode(Nd1);
    theNodes[1] = theDomain->getNode(Nd2);
    
    // if can't find both - send a warning message
    if (!theNodes[0] || !theNodes[1])  {
        if (!theNodes[0])  {
            opserr << "EETwoNodeLink::setDomain() - Nd1: "
                << Nd1 << " does not exist in the model for ";
        } else  {
            opserr << "EETwoNodeLink::setDomain() - Nd2: " 
                << Nd2 << " does not exist in the model for ";
        }
        opserr << "EETwoNodeLink ele: " << this->getTag() << endln;
        
        return;
    }
    
    // now determine the number of dof and the dimension
    int dofNd1 = theNodes[0]->getNumberDOF();
    int dofNd2 = theNodes[1]->getNumberDOF();
    
    // if differing dof at the ends - print a warning message
    if (dofNd1 != dofNd2)  {
        opserr << "EETwoNodeLink::setDomain(): nodes " << Nd1 << " and " << Nd2
            << "have differing dof at ends for element: " << this->getTag() << endln;
        return;
    }	
    
    /* do NOT check on length, so that element can
    // also be used if nodes are not in same location
    const Vector &end1Crd = theNodes[0]->getCrds();
    const Vector &end2Crd = theNodes[1]->getCrds();	
    Vector diff = end1Crd - end2Crd;
    double L  = diff.Norm();*/
    
    // call the base class method
    this->DomainComponent::setDomain(theDomain);
    
    // now set the number of dof for element and set matrix and vector pointer
    if (dimension == 1 && dofNd1 == 1)  {
        numDOF = 2;
        theMatrix = &EETwoNodeLinkM2;
        theVector = &EETwoNodeLinkV2;
        elemType  = D1N2;
    }
    else if (dimension == 2 && dofNd1 == 2)  {
        numDOF = 4;
        theMatrix = &EETwoNodeLinkM4;
        theVector = &EETwoNodeLinkV4;
        elemType  = D2N4;
    }
    else if (dimension == 2 && dofNd1 == 3)  {
        numDOF = 6;
        theMatrix = &EETwoNodeLinkM6;
        theVector = &EETwoNodeLinkV6;
        elemType  = D2N6;
    }
    else if (dimension == 3 && dofNd1 == 3)  {
        numDOF = 6;
        theMatrix = &EETwoNodeLinkM6;
        theVector = &EETwoNodeLinkV6;
        elemType  = D3N6;
    }
    else if (dimension == 3 && dofNd1 == 6)  {
        numDOF = 12;
        theMatrix = &EETwoNodeLinkM12;
        theVector = &EETwoNodeLinkV12;
        elemType  = D3N12;
    }
    else  {
        opserr << "EETwoNodeLink::setDomain() can not handle "
            << dimension << "dofs at nodes in " << dofNd1 << " d problem\n";
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
        opserr << "EETwoNodeLink::setDomain() - element: " << this->getTag()
            << " out of memory creating vector of size: " << numDOF << endln;
        return;
    }          
    
    // set transformation matrix from global to basic
    if (numDir > 0)
        this->setTranGlobalBasic(elemType, numDir);
}


int EETwoNodeLink::commitState()
{
    int rValue = 0;
    
    if (theSite != 0)  {
        rValue += theSite->commitState();
    }
    else  {
        sData[0] = OF_RemoteTest_commitState;
        rValue += theChannel->sendVector(0, 0, *sendData, 0);
    }
    
    return rValue;
}


int EETwoNodeLink::update()
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
    
    static Vector dg(numDOF), vg(numDOF), ag(numDOF);
    for (int i=0; i<numDOF/2; i++)  {
        dg(i)          = dsp1(i);  vg(i)          = vel1(i);  ag(i)          = acc1(i);
        dg(i+numDOF/2) = dsp2(i);  vg(i+numDOF/2) = vel2(i);  ag(i+numDOF/2) = acc2(i);
    }
    
    // transform response from the global to the basic system
    (*db) = T*dg;
    (*vb) = T*vg;
    (*ab) = T*ag;
    
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


int EETwoNodeLink::setInitialStiff(const Matrix& kbinit)
{
    if (kbinit.noRows() != numDir || kbinit.noCols() != numDir)  {
        opserr << "EETwoNodeLink::setInitialStiff() - " 
            << "matrix size is incorrect for element: "
            << this->getTag() << endln;
        return -1;
    }
    kbInit = kbinit;
        
    // transform stiffness matrix from the basic to the global system
    theInitStiff.Zero();
    theInitStiff.addMatrixTripleProduct(0.0, T, kbInit, 1.0);
    
    return 0;
}


const Matrix& EETwoNodeLink::getMass()
{
    // zero the matrix
    theMatrix->Zero();
    
    // form mass matrix
    if (mass != 0.0)  {
        double m = 0.5*mass;
        int numDOF2 = numDOF/2;
        for (int i=0; i<dimension; i++)  {
            (*theMatrix)(i,i) = m;
            (*theMatrix)(i+numDOF2,i+numDOF2) = m;
        }
    }
    
    return *theMatrix; 
}


void EETwoNodeLink::zeroLoad()
{
    theLoad->Zero();
}


int EETwoNodeLink::addLoad(ElementalLoad *theLoad, double loadFactor)
{
    opserr <<"EETwoNodeLink::addLoad() - "
        << "load type unknown for element: "
        << this->getTag() << endln;
    
    return -1;
}


int EETwoNodeLink::addInertiaLoadToUnbalance(const Vector &accel)
{
    // check for quick return
    if (mass == 0.0)  {
        return 0;
    }    
    
    // get R * accel from the nodes
    const Vector &Raccel1 = theNodes[0]->getRV(accel);
    const Vector &Raccel2 = theNodes[1]->getRV(accel);
    
    int numDOF2 = numDOF/2;
    
    if (numDOF2 != Raccel1.Size() || numDOF2 != Raccel2.Size())  {
        opserr << "EETwoNodeLink::addInertiaLoadToUnbalance() - "
            << "matrix and vector sizes are incompatible\n";
        return -1;
    }
    
    // want to add ( - fact * M R * accel ) to unbalance
    // take advantage of lumped mass matrix
    double m = 0.5*mass;
    for (int i=0; i<dimension; i++)  {
        (*theLoad)(i) -= m * Raccel1(i);
        (*theLoad)(i+numDOF2) -= m * Raccel2(i);
    }
    
    return 0;
}


const Vector& EETwoNodeLink::getResistingForce()
{
    // zero the residual
    theVector->Zero();
    
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
        // use elastic force if force from test is zero
        for (int i=0; i<numDir; i++) {
            if ((*qMeas)(i) == 0.0)
                (*qMeas)(i) = kbInit(i,i) * (*db)(i);
        }
    }
    
    // save corresponding target displacements for recorder
    dbTarg = (*db);
    vbTarg = (*vb);
    abTarg = (*ab);

    // determine resisting forces in global system
    (*theVector) = T^(*qMeas);
    
    // subtract external load
    (*theVector) -= *theLoad;
    
    return *theVector;
}


const Vector& EETwoNodeLink::getResistingForceIncInertia()
{	
    this->getResistingForce();
    
    // add the damping forces if rayleigh damping
    if (alphaM != 0.0 || betaK != 0.0 || betaK0 != 0.0 || betaKc != 0.0)
        (*theVector) += this->getRayleighDampingForces();
    
    // now include the mass portion
    if (mass != 0.0)  {
        const Vector &accel1 = theNodes[0]->getTrialAccel();
        const Vector &accel2 = theNodes[1]->getTrialAccel();    
        
        int numDOF2 = numDOF/2;
        double m = 0.5*mass;
        for (int i=0; i<dimension; i++)  {
            (*theVector)(i) += m * accel1(i);
            (*theVector)(i+numDOF2) += m * accel2(i);
        }
    }
    
    return *theVector;
}


const Vector& EETwoNodeLink::getTime()
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


const Vector& EETwoNodeLink::getBasicDisp()
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


const Vector& EETwoNodeLink::getBasicVel()
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


const Vector& EETwoNodeLink::getBasicAccel()
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


int EETwoNodeLink::sendSelf(int commitTag, Channel &theChannel)
{
    // has not been implemented yet.....
    return 0;
}


int EETwoNodeLink::recvSelf(int commitTag, Channel &theChannel,
    FEM_ObjectBroker &theBroker)
{
    // has not been implemented yet.....
    return 0;
}


int EETwoNodeLink::displaySelf(Renderer &theViewer,
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
    
    for (int i=0; i<dimension; i++) {
        v1(i) = end1Crd(i) + end1Disp(i)*fact;
        v2(i) = end2Crd(i) + end2Disp(i)*fact;    
    }
    
    return theViewer.drawLine (v1, v2, 1.0, 1.0);
}


void EETwoNodeLink::Print(OPS_Stream &s, int flag)
{
    if (flag == 0)  {
        // print everything
        s << "Element: " << this->getTag() << endln;
        s << "  type: EETwoNodeLink  iNode: " << connectedExternalNodes(0);
        s << "  jNode: " << connectedExternalNodes(1) << endln;
        if (theSite != 0)
            s << "  ExperimentalSite, tag: " << theSite->getTag() << endln;
        s << "  mass:  " << mass << endln;
        // determine resisting forces in global system
        s << "  resisting force: " << this->getResistingForce() << endln;
    } else if (flag == 1)  {
        // does nothing
    }
}


Response* EETwoNodeLink::setResponse(const char **argv, int argc,
    OPS_Stream &output)
{
    Response *theResponse = 0;

    output.tag("ElementOutput");
    output.attr("eleType","EETwoNodeLink");
    output.attr("eleTag",this->getTag());
    output.attr("node1",connectedExternalNodes[0]);
    output.attr("node2",connectedExternalNodes[1]);

    char outputData[10];

    // global forces
    if (strcmp(argv[0],"force") == 0 || strcmp(argv[0],"forces") == 0 ||
        strcmp(argv[0],"globalForce") == 0 || strcmp(argv[0],"globalForces") == 0)
    {
        for (int i=0; i<numDOF; i++)  {
            sprintf(outputData,"P%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 2, *theVector);
    }
    // local forces
    else if (strcmp(argv[0],"localForce") == 0 || strcmp(argv[0],"localForces") == 0)
    {
        for (int i=0; i<numDOF; i++)  {
            sprintf(outputData,"p%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 3, *theVector);
    }
    // basic forces
    else if (strcmp(argv[0],"basicForce") == 0 || strcmp(argv[0],"basicForces") == 0)
    {
        for (int i=0; i<numDir; i++)  {
            sprintf(outputData,"q%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 4, Vector(numDir));
    }
    // target basic displacements
    else if (strcmp(argv[0],"deformation") == 0 || strcmp(argv[0],"deformations") == 0 || 
        strcmp(argv[0],"basicDeformation") == 0 || strcmp(argv[0],"basicDeformations") == 0 ||
        strcmp(argv[0],"targetDisplacement") == 0 || strcmp(argv[0],"targetDisplacements") == 0)
    {
        for (int i=0; i<numDir; i++)  {
            sprintf(outputData,"db%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 5, Vector(numDir));
    }
    // target basic velocities
    else if (strcmp(argv[0],"targetVelocity") == 0 || 
        strcmp(argv[0],"targetVelocities") == 0)
    {
        for (int i=0; i<numDir; i++)  {
            sprintf(outputData,"vb%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 6, Vector(numDir));
    }
    // target basic accelerations
    else if (strcmp(argv[0],"targetAcceleration") == 0 || 
        strcmp(argv[0],"targetAccelerations") == 0)
    {
        for (int i=0; i<numDir; i++)  {
            sprintf(outputData,"ab%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 7, Vector(numDir));
    }
    // measured basic displacements
    else if (strcmp(argv[0],"measuredDisplacement") == 0 || 
        strcmp(argv[0],"measuredDisplacements") == 0)
    {
        for (int i=0; i<numDir; i++)  {
            sprintf(outputData,"dbm%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 8, Vector(numDir));
    }
    // measured basic velocities
    else if (strcmp(argv[0],"measuredVelocity") == 0 || 
        strcmp(argv[0],"measuredVelocities") == 0)
    {
        for (int i=0; i<numDir; i++)  {
            sprintf(outputData,"vbm%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 9, Vector(numDir));
    }
    // measured basic accelerations
    else if (strcmp(argv[0],"measuredAcceleration") == 0 || 
        strcmp(argv[0],"measuredAccelerations") == 0)
    {
        for (int i=0; i<numDir; i++)  {
            sprintf(outputData,"abm%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 10, Vector(numDir));
    }
    // basic deformations and basic forces
    else if (strcmp(argv[0],"defoANDforce") == 0 || strcmp(argv[0],"deformationANDforce") == 0 ||
        strcmp(argv[0],"deformationsANDforces") == 0)
    {
        int i;
        for (i=0; i<numDir; i++)  {
            sprintf(outputData,"db%d",i+1);
            output.tag("ResponseType",outputData);
        }
        for (i=0; i<numDir; i++)  {
            sprintf(outputData,"q%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 11, Vector(numDir*2));
    }

    output.endTag(); // ElementOutput

    return theResponse;
}


int EETwoNodeLink::getResponse(int responseID, Information &eleInformation)
{
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
            theVector->Zero();
            int numDOF2 = numDOF/2;
            for (int i=0; i<numDir; i++) {
                (*theVector)((*dir)(i))         = -(*qMeas)(i);
                (*theVector)((*dir)(i)+numDOF2) =  (*qMeas)(i);
            }
            
            *(eleInformation.theVector) = *theVector;
        }
        return 0;
        
    case 4:  // basic forces
        if (eleInformation.theVector != 0)  {
            *(eleInformation.theVector) = (*qMeas);
        }
        return 0;
        
    case 5:  // target basic displacements
        if (eleInformation.theVector != 0)  {
            *(eleInformation.theVector) = dbTarg;
        }
        return 0;
        
    case 6:  // target basic velocities
        if (eleInformation.theVector != 0)  {
            *(eleInformation.theVector) = vbTarg;
        }
        return 0;
        
    case 7:  // target basic accelerations
        if (eleInformation.theVector != 0)  {
            *(eleInformation.theVector) = abTarg;
        }
        return 0;
        
    case 8:  // measured basic displacements
        if (eleInformation.theVector != 0)  {
            *(eleInformation.theVector) = this->getBasicDisp();
        }
        return 0;

    case 9:  // measured basic velocities
        if (eleInformation.theVector != 0)  {
            *(eleInformation.theVector) = this->getBasicVel();
        }
        return 0;

    case 10:  // measured basic accelerations
        if (eleInformation.theVector != 0)  {
            *(eleInformation.theVector) = this->getBasicAccel();
        }
        return 0;

    case 11:  // basic deformations and basic forces
        if (eleInformation.theVector != 0) {
            int i;
            for (i=0; i<numDir; i++) {
                (*(eleInformation.theVector))(i) = dbTarg(i);
            }
            for (i=0; i<numDir; i++) {
                (*(eleInformation.theVector))(i+numDir) = (*qMeas)(i);
            }
        }
        return 0;
        
    default:
        return 0;
    }
}


// Establish the external nodes and set up the transformation matrix for orientation
void EETwoNodeLink::setUp(int Nd1, int Nd2, const Vector &x, const Vector &yp)
{ 
    // ensure the connectedExternalNode ID is of correct size & set values
    if (connectedExternalNodes.Size() != 2)  {
        opserr << "EETwoNodeLink::setUp() - element: "
            << this->getTag() << " failed to create an ID of size 2\n";
    }
    
    connectedExternalNodes(0) = Nd1;
    connectedExternalNodes(1) = Nd2;
    
    // set node pointers to NULL
    int i;
    for (i=0; i<2; i++)
        theNodes[i] = 0;
    
    // check that vectors for orientation are of correct size
    if ( x.Size() != 3 || yp.Size() != 3 )  {
        opserr << "EETwoNodeLink::setUp() - element: "
            << this->getTag() << " incorrect dimension of orientation vectors\n";
    }
    
    // establish orientation of element for the tranformation matrix
    // z = x cross yp
    Vector z(3);
    z(0) = x(1)*yp(2) - x(2)*yp(1);
    z(1) = x(2)*yp(0) - x(0)*yp(2);
    z(2) = x(0)*yp(1) - x(1)*yp(0);
    
    // y = z cross x
    Vector y(3);
    y(0) = z(1)*x(2) - z(2)*x(1);
    y(1) = z(2)*x(0) - z(0)*x(2);
    y(2) = z(0)*x(1) - z(1)*x(0);
    
    // compute length(norm) of vectors
    double xn = x.Norm();
    double yn = y.Norm();
    double zn = z.Norm();
    
    // check valid x and y vectors, i.e. not parallel and of zero length
    if (xn == 0 || yn == 0 || zn == 0)  {
        opserr << "EETwoNodeLink::setUp() - element: "
            << this->getTag() << " invalid orientation vectors\n";
    }
    
    // create transformation matrix of direction cosines
    for (i=0; i<3; i++)  {
        transformation(0,i) = x(i)/xn;
        transformation(1,i) = y(i)/yn;
        transformation(2,i) = z(i)/zn;
    }    
}


// set basic deformation-displacement transformation matrix
void EETwoNodeLink::setTranGlobalBasic(Etype elemType, int numDir)
{
    enum Dtype { TRANS, ROTATE };
    
    int axisID, dirID;
    Dtype dirType;
    
    // resize transformation matrix and zero it
    T.resize(numDir,numDOF);
    T.Zero();
    
    // loop over directions, setting row in tran for each direction
    // depending on dimensionality of element
    for (int i=0; i<numDir; i++)  {
        
        dirID  = (*dir)(i);     // direction 0 to 5;
        axisID = dirID % 3;     // 0, 1, 2 for axis of translation or rotation
        
        // set direction type to translation or rotation
        dirType = (dirID<3) ? TRANS : ROTATE;
        
        // now switch on dimensionality of element
        switch (elemType)  {
            
        case D1N2:
            if (dirType == TRANS)
                T(i,1) = transformation(axisID,0);
            break;
            
        case D2N4:
            if (dirType == TRANS)  {
                T(i,2) = transformation(axisID,0);  
                T(i,3) = transformation(axisID,1);
            }
            break;
            
        case D2N6: 
            if (dirType == TRANS)  {
                T(i,3) = transformation(axisID,0);  
                T(i,4) = transformation(axisID,1);
                T(i,5) = 0.0;
            } else if (dirType == ROTATE)  {
                T(i,3) = 0.0;
                T(i,4) = 0.0;
                T(i,5) = transformation(axisID,2);
            }
            break;
            
        case D3N6:
            if (dirType == TRANS)  {
                T(i,3) = transformation(axisID,0);  
                T(i,4) = transformation(axisID,1);
                T(i,5) = transformation(axisID,2);
            }
            break;
            
        case D3N12:
            if (dirType == TRANS)  {
                T(i,6)  = transformation(axisID,0);  
                T(i,7)  = transformation(axisID,1);
                T(i,8)  = transformation(axisID,2);
                T(i,9)  = 0.0;
                T(i,10) = 0.0;
                T(i,11) = 0.0;
            } else if (dirType == ROTATE)  {
                T(i,6)  = 0.0;
                T(i,7)  = 0.0;
                T(i,8)  = 0.0;
                T(i,9)  = transformation(axisID,0);
                T(i,10) = transformation(axisID,1);
                T(i,11) = transformation(axisID,2);
            }
            break;
            
        }  // end switch
        
        // fill in first half of transformation matrix with negative sign
        for (int j=0; j<numDOF/2; j++)
            T(i,j) = -T(i,j+numDOF/2);   
    }
}
