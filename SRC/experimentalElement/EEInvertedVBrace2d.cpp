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
// Description: This file contains the implementation of the EEInvertedVBrace2d class.

#include "EEInvertedVBrace2d.h"

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


// initialize the class wide variables
Matrix EEInvertedVBrace2d::theMatrix(9,9);
Vector EEInvertedVBrace2d::theVector(9);
Vector EEInvertedVBrace2d::theLoad(9);


// responsible for allocating the necessary space needed
// by each object and storing the tags of the end nodes.
EEInvertedVBrace2d::EEInvertedVBrace2d(int tag, int Nd1, int Nd2, int Nd3,
    ExperimentalSite *site,
    bool iM, bool nlgeom, double r1, double r2)
    : ExperimentalElement(tag, ELE_TAG_EEInvertedVBrace2d, site),
    connectedExternalNodes(3),
    iMod(iM), nlGeom(nlgeom),
    rho1(r1), rho2(r2), L1(0.0), L2(0.0),
    db(0), vb(0), ab(0), t(0),
    dbMeas(0), vbMeas(0), abMeas(0), qMeas(0), tMeas(0),
    dbTarg(3), vbTarg(3), abTarg(3),
    dbPast(3), kbInit(3,3), tPast(0.0), T(3,9)
{
    // ensure the connectedExternalNode ID is of correct size & set values
    if (connectedExternalNodes.Size() != 3)  {
        opserr << "EEInvertedVBrace2d::EEInvertedVBrace2d() - element: "
            <<  tag << " failed to create an ID of size 3\n";
        exit(-1);
    }
    
    connectedExternalNodes(0) = Nd1;
    connectedExternalNodes(1) = Nd2;
    connectedExternalNodes(2) = Nd3;
    
    // set node pointers to NULL
    for (int i=0; i<3; i++)
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
    (*sizeDaq)[OF_Resp_Force]  = 6;
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
    qMeas  = new Vector(6);
    tMeas  = new Vector(1);

    // set the initial stiffness matrix size
    theInitStiff.resize(9,9);
    
    // initialize additional vectors
    dbTarg.Zero();
    vbTarg.Zero();
    abTarg.Zero();
    dbPast.Zero();
}


// responsible for allocating the necessary space needed
// by each object and storing the tags of the end nodes.
EEInvertedVBrace2d::EEInvertedVBrace2d(int tag, int Nd1, int Nd2, int Nd3,
    int port, char *machineInetAddr, int ssl, int dataSize,
    bool iM, bool nlGeomFlag, double r1, double r2)
    : ExperimentalElement(tag, ELE_TAG_EEInvertedVBrace2d),
    connectedExternalNodes(3),
    iMod(iM), nlGeom(nlGeomFlag),
    rho1(r1), rho2(r2), L1(0.0), L2(0.0),
    theChannel(0), sData(0), sendData(0), rData(0), recvData(0),
    db(0), vb(0), ab(0), t(0),
    dbMeas(0), vbMeas(0), abMeas(0), qMeas(0), tMeas(0),
    dbTarg(3), vbTarg(3), abTarg(3),
    dbPast(3), kbInit(3,3), tPast(0.0), T(3,9)
{
    // ensure the connectedExternalNode ID is of correct size & set values
    if (connectedExternalNodes.Size() != 3)  {
        opserr << "EEInvertedVBrace2d::EEInvertedVBrace2d() - element: "
            <<  tag << " failed to create an ID of size 3\n";
        exit(-1);
    }
    
    connectedExternalNodes(0) = Nd1;
    connectedExternalNodes(1) = Nd2;
    connectedExternalNodes(2) = Nd3;
    
    // set node pointers to NULL
    for (int i=0; i<3; i++)
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
        opserr << "EEInvertedVBrace2d::EEInvertedVBrace2d() "
            << "- failed to create channel\n";
        exit(-1);
    }
    if (theChannel->setUpConnection() != 0)  {
        opserr << "EEInvertedVBrace2d::EEInvertedVBrace2d() "
            << "- failed to setup connection\n";
        exit(-1);
    }

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
    (*sizeDaq)[OF_Resp_Force]  = 6;
    (*sizeDaq)[OF_Resp_Time]   = 1;
    
    if (dataSize < 16) dataSize = 16;
    intData[2*OF_Resp_All] = dataSize;

    theChannel->sendID(0, 0, idData, 0);
    
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
    id += 6;
    tMeas = new Vector(&rData[id], 1);
    recvData->Zero();

    // set the initial stiffness matrix size
    theInitStiff.resize(9,9);
    
    // initialize additional vectors
    dbTarg.Zero();
    vbTarg.Zero();
    abTarg.Zero();
    dbPast.Zero();
}


// delete must be invoked on any objects created by the object.
EEInvertedVBrace2d::~EEInvertedVBrace2d()
{
    // invoke the destructor on any objects created by the object
    // that the object still holds a pointer to
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


int EEInvertedVBrace2d::getNumExternalNodes() const
{
    return 3;
}


const ID& EEInvertedVBrace2d::getExternalNodes() 
{
    return connectedExternalNodes;
}


Node** EEInvertedVBrace2d::getNodePtrs() 
{
    return theNodes;
}


int EEInvertedVBrace2d::getNumDOF() 
{
    return 9;
}


int EEInvertedVBrace2d::getNumBasicDOF() 
{
    return 3;
}


// to set a link to the enclosing Domain and to set the node pointers.
void EEInvertedVBrace2d::setDomain(Domain *theDomain)
{
    // check Domain is not null - invoked when object removed from a domain
    if (!theDomain)  {
        theNodes[0] = 0;
        theNodes[1] = 0;
        theNodes[2] = 0;
        
        return;
    }
    
    // first set the node pointers
    int Nd1 = connectedExternalNodes(0);
    int Nd2 = connectedExternalNodes(1);
    int Nd3 = connectedExternalNodes(2);
    theNodes[0] = theDomain->getNode(Nd1);
    theNodes[1] = theDomain->getNode(Nd2);
    theNodes[2] = theDomain->getNode(Nd3);
    
    // if can't find all three - send a warning message
    if (!theNodes[0] || !theNodes[1] || !theNodes[2])  {
        if (!theNodes[0])  {
            opserr << "EEInvertedVBrace2d::setDomain() - Nd1: " 
                << Nd1 << " does not exist in the model for ";
        } else if (!theNodes[1])  {
            opserr << "EEInvertedVBrace2d::setDomain() - Nd2: " 
                << Nd2 << " does not exist in the model for ";
        } else  {
            opserr << "EEInvertedVBrace2d::setDomain() - Nd3: " 
                << Nd3 << " does not exist in the model for ";
        }
        opserr << "EEInvertedVBrace2d ele: " << this->getTag() << endln;
        
        return;
    }
    
    // now determine the number of dof and the dimension    
    int dofNd1 = theNodes[0]->getNumberDOF();
    int dofNd2 = theNodes[1]->getNumberDOF();	
    int dofNd3 = theNodes[2]->getNumberDOF();	
    
    // if differing dof at the ends - print a warning message
    if (dofNd1 != 3)  {
        opserr << "EEInvertedVBrace2d::setDomain() - node 1: "
            << connectedExternalNodes(0) << " has incorrect number of DOF (not 3)\n";
        return;
    }
    if (dofNd2 != 3)  {
        opserr << "EEInvertedVBrace2d::setDomain() - node 2: "
            << connectedExternalNodes(1) << " has incorrect number of DOF (not 3)\n";
        return;
    }
    if (dofNd3 != 3)  {
        opserr << "EEInvertedVBrace2d::setDomain() - node 3: "
            << connectedExternalNodes(1) << " has incorrect number of DOF (not 3)\n";
        return;
    }
    
    // call the base class method
    this->DomainComponent::setDomain(theDomain);
    
    // now determine the length, cosines and fill in the transformation
    // NOTE t = -t(every one else uses for residual calc)
    const Vector &end1Crd = theNodes[0]->getCrds();
    const Vector &end2Crd = theNodes[1]->getCrds();
    const Vector &end3Crd = theNodes[2]->getCrds();
    
    dx1[0] = end3Crd(0)-end1Crd(0);
    dx1[1] = end3Crd(1)-end1Crd(1);	
    dx2[0] = end3Crd(0)-end2Crd(0);
    dx2[1] = end3Crd(1)-end2Crd(1);	
    
    L1 = sqrt(dx1[0]*dx1[0] + dx1[1]*dx1[1]);
    L2 = sqrt(dx2[0]*dx2[0] + dx2[1]*dx2[1]);
    
    if (L1 == 0.0)  {
        opserr <<"EEInvertedVBrace2d::setDomain() - element: "
            << this->getTag() << " has diagonal 1 with zero length\n";
        return;
    }
    if (L2 == 0.0)  {
        opserr <<"EEInvertedVBrace2d::setDomain() - element: "
            << this->getTag() << " has diagonal 2 with zero length\n";
        return;
    }
    
    // set transformation matrix from global to basic
    T.Zero();
    double D = dx1[0]*dx2[1] - dx2[0]*dx1[1];
    T(0,0) = -dx1[0]*dx2[1]/D;  T(0,1) = -dx1[1]*dx2[1]/D;
    T(0,3) =  dx2[0]*dx1[1]/D;  T(0,4) =  dx1[1]*dx2[1]/D;  T(0,6) = 1;
    T(1,0) =  dx1[0]*dx2[0]/D;  T(1,1) = -dx2[0]*dx1[1]/D;
    T(1,3) = -dx1[0]*dx2[0]/D;  T(1,4) = -dx1[0]*dx2[1]/D;  T(1,7) = 1;
    //T(2,8) = 1;  // include this to apply rotation at node 3
}   	 


int EEInvertedVBrace2d::commitState()
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


int EEInvertedVBrace2d::update()
{
    int rValue = 0;

    // get current time
    Domain *theDomain = this->getDomain();
    (*t)(0) = theDomain->getCurrentTime();
    
    // linear geometry
    if (nlGeom == false)  {
        // determine global displacements
        const Vector &dsp1 = theNodes[0]->getTrialDisp();
        const Vector &dsp2 = theNodes[1]->getTrialDisp();
        const Vector &dsp3 = theNodes[2]->getTrialDisp();
        
        const Vector &vel1 = theNodes[0]->getTrialVel();
        const Vector &vel2 = theNodes[1]->getTrialVel();
        const Vector &vel3 = theNodes[2]->getTrialVel();
        
        const Vector &acc1 = theNodes[0]->getTrialAccel();
        const Vector &acc2 = theNodes[1]->getTrialAccel();
        const Vector &acc3 = theNodes[2]->getTrialAccel();
        
        static Vector dg(9), vg(9), ag(9);
        for (int i=0; i<3; i++)  {
            dg(i)   = dsp1(i);  vg(i)   = vel1(i);  ag(i)   = acc1(i);
            dg(i+3) = dsp2(i);  vg(i+3) = vel2(i);  ag(i+3) = acc2(i);
            dg(i+6) = dsp3(i);  vg(i+6) = vel3(i);  ag(i+6) = acc3(i);
        }
        
        // transform displacements from the global to the basic system
        (*db) = T*dg;
        (*vb) = T*vg;
        (*ab) = T*ag;
                
        if ((*db) != dbPast || (*t)(0) != tPast)  {
            // save the displacements and the time
            dbPast = (*db);
            tPast = (*t)(0);
            // set the trial response at the site
            if (theSite != 0)  {
                theSite->setTrialResponse(db, vb, ab, (Vector*)0, t);
            }
            else  {
                sData[0] = OF_RemoteTest_setTrialResponse;
                rValue += theChannel->sendVector(0, 0, *sendData, 0);
            }
         }
        
    // nonlinear geometry
    } else  {
        // not implemented yet
    }
    
    return rValue;
}


int EEInvertedVBrace2d::setInitialStiff(const Matrix& kbinit)
{
    if (kbinit.noRows() != 3 || kbinit.noCols() != 3)  {
        opserr << "EEInvertedVBrace2d::setInitialStiff() - " 
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


const Matrix& EEInvertedVBrace2d::getMass()
{
    // zero the matrix
    theMatrix.Zero();
    
    // form mass matrix
    if ((L1 != 0.0 && rho1 != 0.0) || (L2 != 0 && rho2 != 0.0))  {
        double m1 = 0.5*rho1*L1;
        double m2 = 0.5*rho2*L2;
        theMatrix(0,0) = m1;
        theMatrix(1,1) = m1;	
        theMatrix(3,3) = m2;
        theMatrix(4,4) = m2;
        theMatrix(6,6) = m1+m2;
        theMatrix(7,7) = m1+m2;
    }
    
    return theMatrix; 
}


void EEInvertedVBrace2d::zeroLoad()
{
    theLoad.Zero();
}


int EEInvertedVBrace2d::addLoad(ElementalLoad *theLoad, double loadFactor)
{  
    opserr <<"EEInvertedVBrace2d::addLoad() - "
        << "load type unknown for element: "
        << this->getTag() << endln;
    
    return -1;
}


int EEInvertedVBrace2d::addInertiaLoadToUnbalance(const Vector &accel)
{
    // check for quick return
    if ((L1 == 0.0 || rho1 == 0) && (L2 == 0 || rho2 == 0.0))  {
        return 0;
    }    
    
    // get R * accel from the nodes
    const Vector &Raccel1 = theNodes[0]->getRV(accel);
    const Vector &Raccel2 = theNodes[1]->getRV(accel);
    const Vector &Raccel3 = theNodes[2]->getRV(accel);
    
    if (3 != Raccel1.Size() || 3 != Raccel2.Size() || 3 != Raccel3.Size())  {
        opserr << "EEInvertedVBrace2d::addInertiaLoadToUnbalance() - "
            << "matrix and vector sizes are incompatible\n";
        return -1;
    }
    
    // want to add ( - fact * M R * accel ) to unbalance
    // take advantage of lumped mass matrix
    double m1 = 0.5*rho1*L1;
    double m2 = 0.5*rho2*L2;
    theLoad(0) -= m1 * Raccel1(0);
    theLoad(1) -= m1 * Raccel1(1);
    theLoad(3) -= m2 * Raccel2(0);    
    theLoad(4) -= m2 * Raccel2(1);    
    theLoad(6) -= (m1+m2) * Raccel3(0);    
    theLoad(7) -= (m1+m2) * Raccel3(1);    
    
    return 0;
}


const Vector& EEInvertedVBrace2d::getResistingForce()
{
    // get the nodal coordinates
    const Vector &end1Crd = theNodes[0]->getCrds();
    const Vector &end2Crd = theNodes[1]->getCrds();
    const Vector &end3Crd = theNodes[2]->getCrds();
    
    // get the nodal displacements
    const Vector &end1Dsp = theNodes[0]->getDisp();
    const Vector &end2Dsp = theNodes[1]->getDisp();
    const Vector &end3Dsp = theNodes[2]->getDisp();
    
    // calculate the new direction cosines
    dx1[0] = (end3Crd(0) + end3Dsp(0)) - (end1Crd(0) + end1Dsp(0));
    dx1[1] = (end3Crd(1) + end3Dsp(1)) - (end1Crd(1) + end1Dsp(1));	
    dx2[0] = (end3Crd(0) + end3Dsp(0)) - (end2Crd(0) + end2Dsp(0));
    dx2[1] = (end3Crd(1) + end3Dsp(1)) - (end2Crd(1) + end2Dsp(1));	
    
    // zero the residual
    theVector.Zero();
    
    // determine resisting forces in basic system
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
        static Vector qb;
        qb = kbInit*((*dbMeas) - (*db));
        
        //double ratioX1 = (q(0)+q(3)!=0) ? q(0)/(q(0) + q(3)) : 0.5;
        //double ratioX2 = (q(0)+q(3)!=0) ? q(3)/(q(0) + q(3)) : 0.5;
        //double ratioY1 = (q(1)+q(4)!=0) ? q(1)/(q(1) + q(4)) : 0.5;
        //double ratioY2 = (q(1)+q(4)!=0) ? q(4)/(q(1) + q(4)) : 0.5;        
        //q(0) += qb(0)*ratioX1;
        //q(1) += qb(1)*ratioY1;
        //q(3) += qb(0)*ratioX2;
        //q(4) += qb(1)*ratioY2;
        
        (*qMeas)(0) += qb(0)/2 + qb(1)/2*dx1[0]/dx1[1];
        (*qMeas)(1) += qb(0)/2*dx1[1]/dx1[0] + qb(1)/2;
        (*qMeas)(3) += qb(0)/2 + qb(1)/2*dx2[0]/dx2[1];
        (*qMeas)(4) += qb(0)/2*dx2[1]/dx2[0] + qb(1)/2;
    }
    
    // save corresponding target displacements for recorder
    dbTarg = (*db);
    vbTarg = (*vb);
    abTarg = (*ab);

    // determine resisting forces in global system and account for load-cell
    // cross-talk by averaging between shear and axial loads
    theVector(0) = 0.5*((*qMeas)(0) + dx1[0]/dx1[1]*(*qMeas)(1));
    theVector(1) = 0.5*(dx1[1]/dx1[0]*(*qMeas)(0) + (*qMeas)(1));
    //theVector(2) = (*qMeas)(2);    // with moments
    theVector(2) = 0;       // w/o moments
    theVector(3) = 0.5*((*qMeas)(3) + dx2[0]/dx2[1]*(*qMeas)(4));
    theVector(4) = 0.5*(dx2[1]/dx2[0]*(*qMeas)(3) + (*qMeas)(4));
    //theVector(5) = (*qMeas)(5);    // with moments
    theVector(5) = 0;       // w/o moments
    theVector(6) = -theVector(0) - theVector(3);
    theVector(7) = -theVector(1) - theVector(4);
    //theVector(8) = -theVector(0)*dx1[1] + theVector(1)*dx1[0] - theVector(2) - theVector(3)*dx2[1] + theVector(4)*dx2[0] - theVector(5);    // with moments
    theVector(8) = 0;       // w/o moments
    
    // subtract external load
    theVector.addVector(1.0, theLoad, -1.0);
    
    return theVector;
}


const Vector& EEInvertedVBrace2d::getResistingForceIncInertia()
{	
    theVector = this->getResistingForce();
    
    // add the damping forces if rayleigh damping
    if (alphaM != 0.0 || betaK != 0.0 || betaK0 != 0.0 || betaKc != 0.0)
        theVector += this->getRayleighDampingForces();
    
    // now include the mass portion
    if ((L1 != 0.0 && rho1 != 0.0) || (L2 != 0.0 && rho2 != 0.0))  {
        const Vector &accel1 = theNodes[0]->getTrialAccel();
        const Vector &accel2 = theNodes[1]->getTrialAccel();
        const Vector &accel3 = theNodes[2]->getTrialAccel();
        
        double m1 = 0.5*rho1*L1;
        double m2 = 0.5*rho2*L2;
        theVector(0) += m1 * accel1(0);
        theVector(1) += m1 * accel1(1);
        theVector(3) += m2 * accel2(0);    
        theVector(4) += m2 * accel2(1);
        theVector(6) += (m1+m2) * accel2(0);    
        theVector(7) += (m1+m2) * accel2(1);
    }
    
    return theVector;
}


const Vector& EEInvertedVBrace2d::getTime()
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


const Vector& EEInvertedVBrace2d::getBasicDisp()
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


const Vector& EEInvertedVBrace2d::getBasicVel()
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


const Vector& EEInvertedVBrace2d::getBasicAccel()
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


int EEInvertedVBrace2d::sendSelf(int commitTag, Channel &theChannel)
{
    // has not been implemented yet.....
    return 0;
}


int EEInvertedVBrace2d::recvSelf(int commitTag, Channel &theChannel,
    FEM_ObjectBroker &theBroker)
{
    // has not been implemented yet.....
    return 0;
}


int EEInvertedVBrace2d::displaySelf(Renderer &theViewer,
    int displayMode, float fact)
{
    // first set the quantity to be displayed at the nodes
    static Vector values(3);
    for (int j=0; j<3; j++)
        values(j) = 0.0;
    
    // now  determine the end points of the brace based on
    // the display factor (a measure of the distorted image)
    const Vector &end1Crd = theNodes[0]->getCrds();
    const Vector &end2Crd = theNodes[1]->getCrds();	
    const Vector &end3Crd = theNodes[2]->getCrds();	
    
    const Vector &end1Disp = theNodes[0]->getDisp();
    const Vector &end2Disp = theNodes[1]->getDisp();
    const Vector &end3Disp = theNodes[2]->getDisp();
    
    static Matrix coords(3,3);
    
    for (int i=0; i<2; i++)  {
        coords(0,i) = end1Crd(i) + end1Disp(i)*fact;
        coords(1,i) = end2Crd(i) + end2Disp(i)*fact;    
        coords(2,i) = end3Crd(i) + end3Disp(i)*fact;    
    }
    
    return theViewer.drawPolygon (coords, values);
}


void EEInvertedVBrace2d::Print(OPS_Stream &s, int flag)
{
    if (flag == 0)  {
        // print everything
        s << "Element: " << this->getTag() << endln;
        s << "  type: EEInvertedVBrace2d" << endln;
        s << "  iNode: " << connectedExternalNodes(0)
            << ", jNode: " << connectedExternalNodes(1)
            << ", kNode: " << connectedExternalNodes(2) << endln;
        if (theSite != 0)
            s << "  ExperimentalSite: " << theSite->getTag() << endln;
        s << "  mass per unit length diagonal 1: " << rho1 << endln;
        s << "  mass per unit length diagonal 2: " << rho2 << endln;
        // determine resisting forces in global system
        s << "  resisting force: " << this->getResistingForce() << endln;
    } else if (flag == 1)  {
        // does nothing
    }
}


Response* EEInvertedVBrace2d::setResponse(const char **argv, int argc,
    OPS_Stream &output)
{
    Response *theResponse = 0;

    output.tag("ElementOutput");
    output.attr("eleType","EEInvertedVBrace2d");
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
        output.tag("ResponseType","Px_3");
        output.tag("ResponseType","Py_3");
        output.tag("ResponseType","Mz_3");

        theResponse = new ElementResponse(this, 1, theVector);
    }
    // local forces
    else if (strcmp(argv[0],"localForce") == 0 || strcmp(argv[0],"localForces") == 0)
    {
        output.tag("ResponseType","px_1");
        output.tag("ResponseType","py_1");
        output.tag("ResponseType","mz_1");
        output.tag("ResponseType","px_2");
        output.tag("ResponseType","py_2");
        output.tag("ResponseType","mz_2");
        output.tag("ResponseType","px_3");
        output.tag("ResponseType","py_3");
        output.tag("ResponseType","mz_3");

        theResponse = new ElementResponse(this, 2, theVector);
    }
    // basic forces
    else if (strcmp(argv[0],"basicForce") == 0 || strcmp(argv[0],"basicForces") == 0)
    {
        output.tag("ResponseType","q1");
        output.tag("ResponseType","q2");
        output.tag("ResponseType","q3");
        output.tag("ResponseType","q4");
        output.tag("ResponseType","q5");
        output.tag("ResponseType","q6");

        theResponse = new ElementResponse(this, 3, Vector(6));
    }
    // target basic displacements
    else if (strcmp(argv[0],"deformation") == 0 || strcmp(argv[0],"deformations") == 0 || 
        strcmp(argv[0],"basicDeformation") == 0 || strcmp(argv[0],"basicDeformations") == 0 ||
        strcmp(argv[0],"targetDisplacement") == 0 || strcmp(argv[0],"targetDisplacements") == 0)
    {
        output.tag("ResponseType","db1");
        output.tag("ResponseType","db2");
        output.tag("ResponseType","db3");

        theResponse = new ElementResponse(this, 4, Vector(3));
    }
    // target basic velocities
    else if (strcmp(argv[0],"targetVelocity") == 0 ||
        strcmp(argv[0],"targetVelocities") == 0)
    {
        output.tag("ResponseType","vb1");
        output.tag("ResponseType","vb2");
        output.tag("ResponseType","vb3");

        theResponse = new ElementResponse(this, 5, Vector(3));
    }
    // target basic accelerations
    else if (strcmp(argv[0],"targetAcceleration") == 0 ||
        strcmp(argv[0],"targetAccelerations") == 0)
    {
        output.tag("ResponseType","ab1");
        output.tag("ResponseType","ab2");
        output.tag("ResponseType","ab3");

        theResponse = new ElementResponse(this, 6, Vector(3));
    }
    // measured basic displacements
    else if (strcmp(argv[0],"measuredDisplacement") == 0 || 
        strcmp(argv[0],"measuredDisplacements") == 0)
    {
        output.tag("ResponseType","dbm1");
        output.tag("ResponseType","dbm2");
        output.tag("ResponseType","dbm3");

        theResponse = new ElementResponse(this, 7, Vector(3));
    }
    // measured basic velocities
    else if (strcmp(argv[0],"measuredVelocity") == 0 || 
        strcmp(argv[0],"measuredVelocities") == 0)
    {
        output.tag("ResponseType","vbm1");
        output.tag("ResponseType","vbm2");
        output.tag("ResponseType","vbm3");

        theResponse = new ElementResponse(this, 8, Vector(3));
    }
    // measured basic accelerations
    else if (strcmp(argv[0],"measuredAcceleration") == 0 || 
        strcmp(argv[0],"measuredAccelerations") == 0)
    {
        output.tag("ResponseType","abm1");
        output.tag("ResponseType","abm2");
        output.tag("ResponseType","abm3");

        theResponse = new ElementResponse(this, 9, Vector(3));
    }

    output.endTag(); // ElementOutput

    return theResponse;
}


int EEInvertedVBrace2d::getResponse(int responseID, Information &eleInfo)
{
    switch (responseID)  {
    case 1:  // global forces
        return eleInfo.setVector(this->getResistingForce());
        
    case 2:  // local forces
        return eleInfo.setVector(this->getResistingForce());
        
    case 3:  // basic forces
        return eleInfo.setVector(*qMeas);
        
    case 4:  // target basic displacements
        return eleInfo.setVector(dbTarg);
        
    case 5:  // target basic velocities
        return eleInfo.setVector(vbTarg);
        
    case 6:  // target basic accelerations
        return eleInfo.setVector(abTarg);
        
    case 7:  // measured basic displacements
        return eleInfo.setVector(this->getBasicDisp());
        
    case 8:  // measured basic velocities
        return eleInfo.setVector(this->getBasicVel());
        
    case 9:  // measured basic accelerations
        return eleInfo.setVector(this->getBasicAccel());
        
    default:
        return -1;
    }
}
