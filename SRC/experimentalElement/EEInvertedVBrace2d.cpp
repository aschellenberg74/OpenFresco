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

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
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
#include <UDP_Socket.h>
#include <elementAPI.h>

#include <math.h>
#include <stdlib.h>
#include <string.h>


// initialize the class wide variables
Matrix EEInvertedVBrace2d::theMatrix(9,9);
Vector EEInvertedVBrace2d::theVector(9);

void* OPF_EEInvertedVBrace2d()
{
    int ndf = OPS_GetNDF();
    if (ndf != 3) {
        opserr << "WARNING invalid ndf: " << ndf;
        opserr << ", for plane problem need 3 - expElement invertedVBrace\n";
        return 0;
    }
    
    // pointer to experimental element that will be returned
    ExperimentalElement* theExpElement = 0;
    
    if (OPS_GetNumRemainingInputArgs() < 16) {
        opserr << "WARNING invalid number of arguments\n";
        opserr << "Want: expElement invertedVBrace eleTag iNode jNode kNode -site siteTag -initStif Kij <-iMod> <-nlGeom> <-noRayleigh> <-rho1 rho1> <-rho2 rho2>\n";
        opserr << "  or: expElement invertedVBrace eleTag iNode jNode kNode -server ipPort <ipAddr> <-ssl> <-udp> <-dataSize size> -initStif Kij <-iMod> <-nlGeom> <-noRayleigh> <-rho1 rho1> <-rho2 rho2>\n";
        return 0;
    }
    
    // element tag
    int tag;
    int numdata = 1;
    if (OPS_GetIntInput(&numdata, &tag) != 0) {
        opserr << "WARNING invalid expElement invertedVBrace eleTag\n";
        return 0;
    }
    
    // nodes
    int node[3];
    numdata = 3;
    if (OPS_GetIntInput(&numdata, node) != 0) {
        opserr << "WARNING invalid iNode or jNode\n";
        opserr << "expElement invertedVBrace element: " << tag << endln;
        return 0;
    }
    
    // experimental site or server parameters
    ExperimentalSite* theSite = 0;
    int ipPort = 8090;
    char* ipAddr = new char[10];
    strcpy(ipAddr, "127.0.0.1");
    int ssl = 0, udp = 0;
    int dataSize = OF_Network_dataSize;
    const char* type = OPS_GetString();
    if (strcmp(type, "-site") == 0) {
        // site tag
        int siteTag;
        numdata = 1;
        if (OPS_GetIntInput(&numdata, &siteTag) < 0) {
            opserr << "WARNING invalid siteTag\n";
            opserr << "expElement invertedVBrace element: " << tag << endln;
            return 0;
        }
        theSite = OPF_getExperimentalSite(siteTag);
        if (theSite == 0) {
            opserr << "WARNING experimental site not found\n";
            opserr << "expSite: " << siteTag << endln;
            opserr << "expElement invertedVBrace element: " << tag << endln;
            return 0;
        }
    }
    else if (strcmp(type, "-server") == 0) {
        // ip port
        numdata = 1;
        if (OPS_GetIntInput(&numdata, &ipPort) < 0) {
            opserr << "WARNING invalid ipPort\n";
            opserr << "expElement invertedVBrace element: " << tag << endln;
            return 0;
        }
        // server options
        while (OPS_GetNumRemainingInputArgs() > 0) {
            type = OPS_GetString();
            if (strcmp(type, "-initStif") == 0 ||
                strcmp(type, "-initStiff") == 0) {
                // move current arg back by one
                OPS_ResetCurrentInputArg(-1);
                break;
            }
            else if (strcmp(type, "-ssl") == 0) {
                ssl = 1; udp = 0;
            }
            else if (strcmp(type, "-udp") == 0) {
                udp = 1; ssl = 0;
            }
            else if (strcmp(type, "-dataSize") == 0) {
                numdata = 1;
                if (OPS_GetIntInput(&numdata, &dataSize) < 0) {
                    opserr << "WARNING invalid dataSize\n";
                    opserr << "expElement invertedVBrace element: " << tag << endln;
                    return 0;
                }
            }
            else {
                delete[] ipAddr;
                ipAddr = new char[strlen(type) + 1];
                strcpy(ipAddr, type);
            }
        }
    }
    else {
        opserr << "WARNING expecting -site or -server string but got ";
        opserr << type << endln;
        opserr << "expElement invertedVBrace element: " << tag << endln;
        return 0;
    }
    
    // initial stiffness
    type = OPS_GetString();
    if (strcmp(type, "-initStif") != 0 &&
        strcmp(type, "-initStiff") != 0) {
        opserr << "WARNING expecting -initStif\n";
        opserr << "expElement invertedVBrace element: " << tag << endln;
    }
    Matrix theInitStif(3, 3);
    double stif[9];
    numdata = 9;
    if (OPS_GetDoubleInput(&numdata, stif) < 0) {
        opserr << "WARNING invalid initial stiffness term\n";
        opserr << "expElement invertedVBrace element: " << tag << endln;
        return 0;
    }
    theInitStif.setData(stif, 3, 3);
    
    // optional parameters
    ExperimentalTangentStiff* theTangStif = 0;
    bool iMod = false;
    bool nlGeom = false;
    int doRayleigh = 1;
    double rho[2] = { 0.0, 0.0 };
    while (OPS_GetNumRemainingInputArgs() > 0) {
        type = OPS_GetString();
        // tangent stiffness
        if (strcmp(type, "-tangStif") == 0 ||
            strcmp(type, "-tangStiff") == 0) {
            int tangStifTag;
            numdata = 1;
            if (OPS_GetIntInput(&numdata, &tangStifTag) < 0) {
                opserr << "WARNING invalid tangStifTag\n";
                opserr << "expElement invertedVBrace element: " << tag << endln;
                return 0;
            }
            theTangStif = OPF_getExperimentalTangentStiff(tangStifTag);
            if (theTangStif == 0) {
                opserr << "WARNING experimental tangent stiff not found\n";
                opserr << "expTangStiff: " << tangStifTag << endln;
                opserr << "expElement invertedVBrace element: " << tag << endln;
                return 0;
            }
        }
        else if (strcmp(type, "-iMod") == 0) {
            iMod = true;
        }
        else if (strcmp(type, "-nlGeom") == 0) {
            nlGeom = true;
        }
        else if (strcmp(type, "-doRayleigh") == 0) {
            doRayleigh = 1;
        }
        else if (strcmp(type, "-noRayleigh") == 0) {
            doRayleigh = 0;
        }
        else if (strcmp(type, "-rho") == 0) {
            numdata = 2;
            if (OPS_GetDoubleInput(&numdata, rho) < 0) {
                opserr << "WARNING invalid rho1 or rho2\n";
                opserr << "expElement invertedVBrace element: " << tag << endln;
                return 0;
            }
        }
    }
    
    // now create the EEInvertedVBrace2d
    if (theSite != 0) {
        theExpElement = new EEInvertedVBrace2d(tag, node[0], node[1], node[2],
            theSite, iMod, nlGeom, doRayleigh, rho[0], rho[1]);
    }
    else {
        theExpElement = new EEInvertedVBrace2d(tag, node[0], node[1], node[2],
            ipPort, ipAddr, ssl, udp, dataSize, iMod, nlGeom, doRayleigh, rho[0], rho[1]);
    }
    if (theExpElement == 0) {
        opserr << "WARNING ran out of memory creating element\n";
        opserr << "expElement invertedVBrace element: " << tag << endln;
        return 0;
    }
    
    // add initial stiffness
    int setInitStif = theExpElement->setInitialStiff(theInitStif);
    if (setInitStif != 0) {
        opserr << "WARNING initial stiffness not set\n";
        opserr << "expElement invertedVBrace element: " << tag << endln;
        return 0;
    }
    
    return theExpElement;
}


// responsible for allocating the necessary space needed
// by each object and storing the tags of the end nodes.
EEInvertedVBrace2d::EEInvertedVBrace2d(int tag, int Nd1, int Nd2, int Nd3,
    ExperimentalSite *site,
    bool iM, bool nlGeomFlag, int addRay, double r1, double r2)
    : ExperimentalElement(tag, ELE_TAG_EEInvertedVBrace2d, site),
    connectedExternalNodes(3), iMod(iM), nlGeom(nlGeomFlag),
    addRayleigh(addRay), rho1(r1), rho2(r2), L1(0.0), L2(0.0), theLoad(9),
    db(0), vb(0), ab(0), t(0),
    dbDaq(0), vbDaq(0), abDaq(0), qDaq(0), tDaq(0),
    dbCtrl(3), vbCtrl(3), abCtrl(3),
    T(3,9), kbInit(3,3), dbLast(3), tLast(0.0)
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
    
    // allocate memory for daq response vectors
    dbDaq = new Vector(3);
    vbDaq = new Vector(3);
    abDaq = new Vector(3);
    qDaq  = new Vector(6);
    tDaq  = new Vector(1);
    
    // set the initial stiffness matrix size
    theInitStiff.resize(9,9);
    
    // initialize additional vectors
    dbCtrl.Zero();
    vbCtrl.Zero();
    abCtrl.Zero();
    dbLast.Zero();
}


// responsible for allocating the necessary space needed
// by each object and storing the tags of the end nodes.
EEInvertedVBrace2d::EEInvertedVBrace2d(int tag, int Nd1, int Nd2, int Nd3,
    int port, char *machineInetAddr, int ssl, int udp, int dataSize,
    bool iM, bool nlGeomFlag, int addRay, double r1, double r2)
    : ExperimentalElement(tag, ELE_TAG_EEInvertedVBrace2d),
    connectedExternalNodes(3), iMod(iM), nlGeom(nlGeomFlag),
    addRayleigh(addRay), rho1(r1), rho2(r2), L1(0.0), L2(0.0), theLoad(9),
    theChannel(0), sData(0), sendData(0), rData(0), recvData(0),
    db(0), vb(0), ab(0), t(0),
    dbDaq(0), vbDaq(0), abDaq(0), qDaq(0), tDaq(0),
    dbCtrl(3), vbCtrl(3), abCtrl(3),
    T(3,9), kbInit(3,3), dbLast(3), tLast(0.0)
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
        opserr << "EEInvertedVBrace2d::EEInvertedVBrace2d() "
            << "- failed to create channel\n";
        exit(-1);
    }
    if (theChannel->setUpConnection() != 0)  {
        opserr << "EEInvertedVBrace2d::EEInvertedVBrace2d() "
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
    dbDaq = new Vector(&rData[id], 3);
    id += 3;
    vbDaq = new Vector(&rData[id], 3);
    id += 3;
    abDaq = new Vector(&rData[id], 3);
    id += 3;
    qDaq = new Vector(&rData[id], 6);
    id += 6;
    tDaq = new Vector(&rData[id], 1);
    recvData->Zero();
    
    // set the initial stiffness matrix size
    theInitStiff.resize(9,9);
    
    // initialize additional vectors
    dbCtrl.Zero();
    vbCtrl.Zero();
    abCtrl.Zero();
    dbLast.Zero();
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
    
    // update dbLast
    int ndim = 0, i;
    Vector dgLast(9);
    for (i=0; i<3; i++)  {
        Vector disp = theNodes[i]->getTrialDisp();
        dgLast.Assemble(disp, ndim);
        ndim += 3;
    }
    dbLast.addMatrixVector(0.0, T, dgLast, 1.0);
    
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
        // get global trial response
        int ndim = 0, i;
        Vector dg(9), vg(9), ag(9);
        for (i=0; i<3; i++)  {
            Vector disp = theNodes[i]->getTrialDisp();
            Vector vel = theNodes[i]->getTrialVel();
            Vector accel = theNodes[i]->getTrialAccel();
            dg.Assemble(disp, ndim);
            vg.Assemble(vel, ndim);
            ag.Assemble(accel, ndim);
            ndim += 3;
        }
        
        // transform displacements from the global to the basic system
        db->addMatrixVector(0.0, T, dg, 1.0);
        vb->addMatrixVector(0.0, T, vg, 1.0);
        ab->addMatrixVector(0.0, T, ag, 1.0);
        
        Vector dbDelta = (*db) - dbLast;
        // do not check time for right now because of transformation constraint
        // handler calling update at beginning of new step when applying load
        // if (dbDelta.pNorm(0) > DBL_EPSILON || (*t)(0) > tLast)  {
        if (dbDelta.pNorm(0) > DBL_EPSILON)  {
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
        opserr << "EEInvertedVBrace2d::update() - " 
            << "nonlinear geometry not implemented yet for element: "
            << this->getTag() << endln;
        return -1;
    }
    
    // save the last displacements and time
    dbLast = (*db);
    tLast = (*t)(0);
    
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


const Matrix& EEInvertedVBrace2d::getDamp()
{
    // zero the global matrix
    theMatrix.Zero();
    
    // call base class to setup Rayleigh damping
    if (addRayleigh == 1)  {
        theMatrix = this->Element::getDamp();
    }
    
    return theMatrix;
}


const Matrix& EEInvertedVBrace2d::getMass()
{
    // zero the global matrix
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
    
    // zero the global residual
    theVector.Zero();
    
    // determine resisting forces in basic system
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
        static Vector qb(3);
        qb.addMatrixVector(0.0, kbInit, (*dbDaq) - (*db), 1.0);
        
        //double ratioX1 = (q(0)+q(3)!=0) ? q(0)/(q(0) + q(3)) : 0.5;
        //double ratioX2 = (q(0)+q(3)!=0) ? q(3)/(q(0) + q(3)) : 0.5;
        //double ratioY1 = (q(1)+q(4)!=0) ? q(1)/(q(1) + q(4)) : 0.5;
        //double ratioY2 = (q(1)+q(4)!=0) ? q(4)/(q(1) + q(4)) : 0.5;
        //q(0) += qb(0)*ratioX1;
        //q(1) += qb(1)*ratioY1;
        //q(3) += qb(0)*ratioX2;
        //q(4) += qb(1)*ratioY2;
        
        (*qDaq)(0) += qb(0)/2 + qb(1)/2*dx1[0]/dx1[1];
        (*qDaq)(1) += qb(0)/2*dx1[1]/dx1[0] + qb(1)/2;
        (*qDaq)(3) += qb(0)/2 + qb(1)/2*dx2[0]/dx2[1];
        (*qDaq)(4) += qb(0)/2*dx2[1]/dx2[0] + qb(1)/2;
    }
    
    // save corresponding ctrl displacements for recorder
    dbCtrl = (*db);
    vbCtrl = (*vb);
    abCtrl = (*ab);
    
    // determine resisting forces in global system and account for load-cell
    // cross-talk by averaging between shear and axial loads
    theVector(0) = 0.5*((*qDaq)(0) + dx1[0]/dx1[1]*(*qDaq)(1));
    theVector(1) = 0.5*(dx1[1]/dx1[0]*(*qDaq)(0) + (*qDaq)(1));
    //theVector(2) = (*qDaq)(2);    // with moments
    theVector(2) = 0;       // w/o moments
    theVector(3) = 0.5*((*qDaq)(3) + dx2[0]/dx2[1]*(*qDaq)(4));
    theVector(4) = 0.5*(dx2[1]/dx2[0]*(*qDaq)(3) + (*qDaq)(4));
    //theVector(5) = (*qDaq)(5);    // with moments
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
    // this already includes damping forces from specimen
    theVector = this->getResistingForce();
    
    // add the damping forces from rayleigh damping
    if (addRayleigh == 1)  {
        if (alphaM != 0.0 || betaK != 0.0 || betaK0 != 0.0 || betaKc != 0.0)
            theVector.addVector(1.0, this->getRayleighDampingForces(), 1.0);
    }
    
    // add inertia forces from element mass
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
        theVector(6) += (m1+m2) * accel3(0);
        theVector(7) += (m1+m2) * accel3(1);
    }
    
    return theVector;
}


const Vector& EEInvertedVBrace2d::getTime()
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


const Vector& EEInvertedVBrace2d::getBasicDisp()
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


const Vector& EEInvertedVBrace2d::getBasicVel()
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


const Vector& EEInvertedVBrace2d::getBasicAccel()
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
    int displayMode, float fact, const char **modes, int numMode)
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
    
    static Matrix coords(3,3);
    
    if (displayMode >= 0)  {
        const Vector &end1Disp = theNodes[0]->getDisp();
        const Vector &end2Disp = theNodes[1]->getDisp();
        const Vector &end3Disp = theNodes[2]->getDisp();
        
        for (int i=0; i<2; i++)  {
            coords(0,i) = end1Crd(i) + end1Disp(i)*fact;
            coords(1,i) = end2Crd(i) + end2Disp(i)*fact;
            coords(2,i) = end3Crd(i) + end3Disp(i)*fact;
        }
    } else  {
        int mode = displayMode * -1;
        const Matrix &eigen1 = theNodes[0]->getEigenvectors();
        const Matrix &eigen2 = theNodes[1]->getEigenvectors();
        const Matrix &eigen3 = theNodes[2]->getEigenvectors();
        
        if (eigen1.noCols() >= mode)  {
            for (int i=0; i<2; i++)  {
                coords(0,i) = end1Crd(i) + eigen1(i,mode-1)*fact;
                coords(1,i) = end2Crd(i) + eigen2(i,mode-1)*fact;
                coords(2,i) = end3Crd(i) + eigen3(i,mode-1)*fact;
            }
        } else  {
            for (int i=0; i<2; i++)  {
                coords(0,i) = end1Crd(i);
                coords(1,i) = end2Crd(i);
                coords(2,i) = end3Crd(i);
            }
        }
    }
    
    return theViewer.drawPolygon(coords, values, this->getTag(), 0);
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
        s << "  addRayleigh: " << addRayleigh << endln;
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
    if (strcmp(argv[0],"force") == 0 ||
        strcmp(argv[0],"forces") == 0 ||
        strcmp(argv[0],"globalForce") == 0 ||
        strcmp(argv[0],"globalForces") == 0)
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
    else if (strcmp(argv[0],"localForce") == 0 ||
        strcmp(argv[0],"localForces") == 0)
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
    else if (strcmp(argv[0],"basicForce") == 0 ||
        strcmp(argv[0],"basicForces") == 0 ||
        strcmp(argv[0],"daqForce") == 0 ||
        strcmp(argv[0],"daqForces") == 0)
    {
        output.tag("ResponseType","q1");
        output.tag("ResponseType","q2");
        output.tag("ResponseType","q3");
        output.tag("ResponseType","q4");
        output.tag("ResponseType","q5");
        output.tag("ResponseType","q6");
        
        theResponse = new ElementResponse(this, 3, Vector(6));
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
        
        theResponse = new ElementResponse(this, 4, Vector(3));
    }
    
    // ctrl basic velocities
    else if (strcmp(argv[0],"ctrlVel") == 0 ||
        strcmp(argv[0],"ctrlVelocity") == 0 ||
        strcmp(argv[0],"ctrlVelocities") == 0)
    {
        output.tag("ResponseType","vb1");
        output.tag("ResponseType","vb2");
        output.tag("ResponseType","vb3");
        
        theResponse = new ElementResponse(this, 5, Vector(3));
    }
    
    // ctrl basic accelerations
    else if (strcmp(argv[0],"ctrlAccel") == 0 ||
        strcmp(argv[0],"ctrlAcceleration") == 0 ||
        strcmp(argv[0],"ctrlAccelerations") == 0)
    {
        output.tag("ResponseType","ab1");
        output.tag("ResponseType","ab2");
        output.tag("ResponseType","ab3");
        
        theResponse = new ElementResponse(this, 6, Vector(3));
    }
    
    // daq basic displacements
    else if (strcmp(argv[0],"daqDisp") == 0 ||
        strcmp(argv[0],"daqDisplacement") == 0 ||
        strcmp(argv[0],"daqDisplacements") == 0)
    {
        output.tag("ResponseType","dbDaq1");
        output.tag("ResponseType","dbDaq2");
        output.tag("ResponseType","dbDaq3");
        
        theResponse = new ElementResponse(this, 7, Vector(3));
    }
    
    // daq basic velocities
    else if (strcmp(argv[0],"daqVel") == 0 ||
        strcmp(argv[0],"daqVelocity") == 0 ||
        strcmp(argv[0],"daqVelocities") == 0)
    {
        output.tag("ResponseType","vbDaq1");
        output.tag("ResponseType","vbDaq2");
        output.tag("ResponseType","vbDaq3");
        
        theResponse = new ElementResponse(this, 8, Vector(3));
    }
    
    // daq basic accelerations
    else if (strcmp(argv[0],"daqAccel") == 0 ||
        strcmp(argv[0],"daqAcceleration") == 0 ||
        strcmp(argv[0],"daqAccelerations") == 0)
    {
        output.tag("ResponseType","abDaq1");
        output.tag("ResponseType","abDaq2");
        output.tag("ResponseType","abDaq3");
        
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
        return eleInfo.setVector(*qDaq);
        
    case 4:  // ctrl basic displacements
        return eleInfo.setVector(dbCtrl);
        
    case 5:  // ctrl basic velocities
        return eleInfo.setVector(vbCtrl);
        
    case 6:  // ctrl basic accelerations
        return eleInfo.setVector(abCtrl);
        
    case 7:  // daq basic displacements
        return eleInfo.setVector(this->getBasicDisp());
        
    case 8:  // daq basic velocities
        return eleInfo.setVector(this->getBasicVel());
        
    case 9:  // daq basic accelerations
        return eleInfo.setVector(this->getBasicAccel());
        
    default:
        return -1;
    }
}
