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
// Description: This file contains the implementation of the EEBeamColumn2d class.

#include "EEBeamColumn2d.h"

#include <Domain.h>
#include <Node.h>
#include <Channel.h>
#include <FEM_ObjectBroker.h>
#include <Renderer.h>
#include <Information.h>
#include <ElementResponse.h>
#include <CrdTransf.h>
#include <ElementalLoad.h>
#include <TCP_Socket.h>
#include <TCP_SocketSSL.h>
#include <UDP_Socket.h>
#include <elementAPI.h>

#include <math.h>
#include <stdlib.h>
#include <string.h>


// initialize the class wide variables
Matrix EEBeamColumn2d::theMatrix(6,6);
Vector EEBeamColumn2d::theVector(6);

void* OPF_EEBeamColumn2d()
{
    int ndf = OPS_GetNDF();
    if (ndf != 3) {
        opserr << "WARNING invalid ndf: " << ndf;
        opserr << ", for plane problem need 3 - expElement beamColumn\n";
        return 0;
    }
    
    // pointer to experimental element that will be returned
    ExperimentalElement* theExpElement = 0;
    
    if (OPS_GetNumRemainingInputArgs() < 16) {
        opserr << "WARNING invalid number of arguments\n";
        opserr << "Want: expElement beamColumn eleTag iNode jNode transTag -site siteTag -initStif Kij <-iMod> <-noRayleigh> <-rho rho> <-cMass>\n";
        opserr << "  or: expElement beamColumn eleTag iNode jNode transTag -server ipPort <ipAddr> <-ssl> <-udp> <-dataSize size> -initStif Kij <-iMod> <-noRayleigh> <-rho rho> <-cMass>\n";
        return 0;
    }
    
    // element tag
    int tag;
    int numdata = 1;
    if (OPS_GetIntInput(&numdata, &tag) != 0) {
        opserr << "WARNING invalid expElement beamColumn eleTag\n";
        return 0;
    }
    
    // nodes
    int node[2];
    numdata = 2;
    if (OPS_GetIntInput(&numdata, node) != 0) {
        opserr << "WARNING invalid iNode or jNode\n";
        opserr << "expElement beamColumn element: " << tag << endln;
        return 0;
    }
    
    // geometric transformation
    int transTag;
    CrdTransf* theTrans = 0;
    numdata = 1;
    if (OPS_GetIntInput(&numdata, &transTag) != 0) {
        opserr << "WARNING invalid transTag\n";
        opserr << "expElement beamColumn element: " << tag << endln;
        return 0;
    }
    theTrans = OPS_GetCrdTransf(transTag);
    if (theTrans == 0) {
        opserr << "WARNING geometric transformation object not found\n";
        opserr << "expElement beamColumn element: " << tag << endln;
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
            opserr << "expElement beamColumn element: " << tag << endln;
            return 0;
        }
        theSite = OPF_getExperimentalSite(siteTag);
        if (theSite == 0) {
            opserr << "WARNING experimental site not found\n";
            opserr << "expSite: " << siteTag << endln;
            opserr << "expElement beamColumn element: " << tag << endln;
            return 0;
        }
    }
    else if (strcmp(type, "-server") == 0) {
        // ip port
        numdata = 1;
        if (OPS_GetIntInput(&numdata, &ipPort) < 0) {
            opserr << "WARNING invalid ipPort\n";
            opserr << "expElement beamColumn element: " << tag << endln;
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
                    opserr << "expElement beamColumn element: " << tag << endln;
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
        opserr << "expElement beamColumn element: " << tag << endln;
        return 0;
    }
    
    // initial stiffness
    type = OPS_GetString();
    if (strcmp(type, "-initStif") != 0 &&
        strcmp(type, "-initStiff") != 0) {
        opserr << "WARNING expecting -initStif\n";
        opserr << "expElement beamColumn element: " << tag << endln;
    }
    Matrix theInitStif(3, 3);
    double stif[9];
    numdata = 9;
    if (OPS_GetDoubleInput(&numdata, stif) < 0) {
        opserr << "WARNING invalid initial stiffness term\n";
        opserr << "expElement beamColumn element: " << tag << endln;
        return 0;
    }
    theInitStif.setData(stif, 3, 3);
    
    // optional parameters
    ExperimentalTangentStiff* theTangStif = 0;
    bool iMod = false;
    int doRayleigh = 1;
    double rho = 0.0;
    bool cMass = false;
    while (OPS_GetNumRemainingInputArgs() > 0) {
        type = OPS_GetString();
        // tangent stiffness
        if (strcmp(type, "-tangStif") == 0 ||
            strcmp(type, "-tangStiff") == 0) {
            int tangStifTag;
            numdata = 1;
            if (OPS_GetIntInput(&numdata, &tangStifTag) < 0) {
                opserr << "WARNING invalid tangStifTag\n";
                opserr << "expElement beamColumn element: " << tag << endln;
                return 0;
            }
            theTangStif = OPF_getExperimentalTangentStiff(tangStifTag);
            if (theTangStif == 0) {
                opserr << "WARNING experimental tangent stiff not found\n";
                opserr << "expTangStiff: " << tangStifTag << endln;
                opserr << "expElement beamColumn element: " << tag << endln;
                return 0;
            }
        }
        else if (strcmp(type, "-iMod") == 0) {
            iMod = true;
        }
        else if (strcmp(type, "-doRayleigh") == 0) {
            doRayleigh = 1;
        }
        else if (strcmp(type, "-noRayleigh") == 0) {
            doRayleigh = 0;
        }
        else if (strcmp(type, "-rho") == 0) {
            numdata = 1;
            if (OPS_GetDoubleInput(&numdata, &rho) < 0) {
                opserr << "WARNING invalid rho\n";
                opserr << "expElement beamColumn element: " << tag << endln;
                return 0;
            }
        }
        else if (strcmp(type, "-cMass") == 0) {
            cMass = true;
        }
    }
    
    // now create the EEBeamColumn2d
    if (theSite != 0) {
        theExpElement = new EEBeamColumn2d(tag, node[0], node[1], *theTrans,
            theSite, iMod, doRayleigh, rho, cMass);
    }
    else {
        theExpElement = new EEBeamColumn2d(tag, node[0], node[1], *theTrans,
            ipPort, ipAddr, ssl, udp, dataSize, iMod, doRayleigh, rho, cMass);
    }
    if (theExpElement == 0) {
        opserr << "WARNING ran out of memory creating element\n";
        opserr << "expElement beamColumn element: " << tag << endln;
        return 0;
    }
    
    // add initial stiffness
    int setInitStif = theExpElement->setInitialStiff(theInitStif);
    if (setInitStif != 0) {
        opserr << "WARNING initial stiffness not set\n";
        opserr << "expElement beamColumn element: " << tag << endln;
        return 0;
    }
    
    return theExpElement;
}


// responsible for allocating the necessary space needed
// by each object and storing the tags of the end nodes.
EEBeamColumn2d::EEBeamColumn2d(int tag, int Nd1, int Nd2,
    CrdTransf &coordTransf,
    ExperimentalSite *site,
    bool iM, int addRay, double r, bool cm)
    : ExperimentalElement(tag, ELE_TAG_EEBeamColumn2d, site),
    connectedExternalNodes(2), theCoordTransf(0),
    iMod(iM), addRayleigh(addRay), rho(r), cMass(cm),
    nlGeo(0), L(0.0), theLoad(6), db(0), vb(0), ab(0), t(0),
    dbDaq(0), vbDaq(0), abDaq(0), qbDaq(0), tDaq(0),
    dbCtrl(3), vbCtrl(3), abCtrl(3),
    kbInit(3,3), dbLast(3), tLast(0.0),
    firstWarning(true)
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
    
    // allocate memory for daq response vectors
    dbDaq = new Vector(3);
    vbDaq = new Vector(3);
    abDaq = new Vector(3);
    qbDaq = new Vector(3);
    tDaq  = new Vector(1);
    
    // set the initial stiffness matrix size
    theInitStiff.resize(6,6);
    
    // get a copy of the coordinate transformation
    theCoordTransf = coordTransf.getCopy2d();
    if (!theCoordTransf)  {
        opserr << "EEBeamColumn2d::EEBeamColumn2d() - "
            << "failed to get copy of the coordinate transformation\n";
        exit(-1);
    }
    
    // get coordinate transformation type and save flag
    if (strncmp(theCoordTransf->getClassType(),"Linear",6) == 0)
        nlGeo = 0;
    else if (strncmp(theCoordTransf->getClassType(),"PDelta",6) == 0)
        nlGeo = 1;
    else if (strncmp(theCoordTransf->getClassType(),"Corot",5) == 0)
        nlGeo = 2;
    
    // initialize additional vectors
    dbCtrl.Zero();
    vbCtrl.Zero();
    abCtrl.Zero();
    dbLast.Zero();
    for (i=0; i<3; i++)  {
        qA0[i] = 0.0;
        pA0[i] = 0.0;
    }
}


// responsible for allocating the necessary space needed
// by each object and storing the tags of the end nodes.
EEBeamColumn2d::EEBeamColumn2d(int tag, int Nd1, int Nd2,
    CrdTransf &coordTransf,
    int port, char *machineInetAddr, int ssl, int udp,
    int dataSize, bool iM, int addRay, double r, bool cm)
    : ExperimentalElement(tag, ELE_TAG_EEBeamColumn2d),
    connectedExternalNodes(2), theCoordTransf(0),
    iMod(iM), addRayleigh(addRay), rho(r), cMass(cm),
    nlGeo(0), L(0.0), theLoad(6), theChannel(0),
    sData(0), sendData(0), rData(0), recvData(0),
    db(0), vb(0), ab(0), t(0),
    dbDaq(0), vbDaq(0), abDaq(0), qbDaq(0), tDaq(0),
    dbCtrl(3), vbCtrl(3), abCtrl(3),
    kbInit(3,3), dbLast(3), tLast(0.0),
    firstWarning(true)
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
        opserr << "EEBeamColumn2d::EEBeamColumn2d() "
            << "- failed to create channel\n";
        exit(-1);
    }
    if (theChannel->setUpConnection() != 0)  {
        opserr << "EEBeamColumn2d::EEBeamColumn2d() "
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
    (*sizeDaq)[OF_Resp_Force]  = 3;
    (*sizeDaq)[OF_Resp_Time]   = 1;
    
    if (dataSize < 13) dataSize = 13;
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
    qbDaq = new Vector(&rData[id], 3);
    id += 3;
    tDaq = new Vector(&rData[id], 1);
    recvData->Zero();
    
    // set the initial stiffness matrix size
    theInitStiff.resize(6,6);
    
    // get a copy of the coordinate transformation
    theCoordTransf = coordTransf.getCopy2d();
    if (!theCoordTransf)  {
        opserr << "EEBeamColumn2d::EEBeamColumn2d() - "
            << "failed to get copy of the coordinate transformation\n";
        exit(-1);
    }
    
    // get coordinate transformation type and save flag
    if (strncmp(theCoordTransf->getClassType(),"Linear",6) == 0)
        nlGeo = 0;
    else if (strncmp(theCoordTransf->getClassType(),"PDelta",6) == 0)
        nlGeo = 1;
    else if (strncmp(theCoordTransf->getClassType(),"Corot",5) == 0)
        nlGeo = 2;
    
    // initialize additional vectors
    dbCtrl.Zero();
    vbCtrl.Zero();
    abCtrl.Zero();
    dbLast.Zero();
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
    L = theCoordTransf->getInitialLength();
    if (L == 0.0)  {
        opserr << "EEBeamColumn2d::setDomain() - element: "
            << this->getTag() << " has zero length\n";
        return;
    }
}


int EEBeamColumn2d::commitState()
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
    
    // commit the coordinate transformation
    rValue += theCoordTransf->commitState();
    
    // commit the base class
    rValue += this->Element::commitState();
    
    // update dbLast
    const Vector &dbA = theCoordTransf->getBasicTrialDisp();
    if (nlGeo == 0 || nlGeo == 1) {
        dbLast(0) = dbA(0);
        dbLast(1) = -L*dbA(1);
        dbLast(2) = -dbA(1)+dbA(2);
    }
    else if (nlGeo == 2) {
        dbLast(0) = (L+dbA(0))*cos(dbA(1))-L;
        dbLast(1) = -(L+dbA(0))*sin(dbA(1));
        dbLast(2) = -dbA(1)+dbA(2);
    }
    
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
    
    if (nlGeo == 0 || nlGeo == 1)  {
        // transform displacements from basic sys A to basic sys B (linear)
        (*db)[0] = dbA(0);
        (*db)[1] = -L*dbA(1);
        (*db)[2] = -dbA(1)+dbA(2);
        (*vb)[0] = vbA(0);
        (*vb)[1] = -L*vbA(1);
        (*vb)[2] = -vbA(1)+vbA(2);
        (*ab)[0] = abA(0);
        (*ab)[1] = -L*abA(1);
        (*ab)[2] = -abA(1)+abA(2);
    } else if (nlGeo == 2)  {
        // transform displacements from basic sys A to basic sys B (nonlinear)
        (*db)[0] = (L+dbA(0))*cos(dbA(1))-L;
        (*db)[1] = -(L+dbA(0))*sin(dbA(1));
        (*db)[2] = -dbA(1)+dbA(2);
        (*vb)[0] = vbA(0)*cos(dbA(1))-(L+dbA(0))*sin(dbA(1))*vbA(1);
        (*vb)[1] = -vbA(0)*sin(dbA(1))-(L+dbA(0))*cos(dbA(1))*vbA(1);
        (*vb)[2] = -vbA(1)+vbA(2);
        (*ab)[0] = abA(0)*cos(dbA(1))-2*vbA(0)*sin(dbA(1))*vbA(1)-(L+dbA(0))*cos(dbA(1))*pow(vbA(1),2)-(L+dbA(0))*sin(dbA(1))*abA(1);
        (*ab)[1] = -abA(0)*sin(dbA(1))-2*vbA(0)*cos(dbA(1))*vbA(1)+(L+dbA(0))*sin(dbA(1))*pow(vbA(1),2)-(L+dbA(0))*cos(dbA(1))*abA(1);
        (*ab)[2] = -abA(1)+abA(2);
    }
    
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
    
    // save the last displacements and time
    dbLast = (*db);
    tLast = (*t)(0);
    
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
    
    // transform stiffness from basic sys B to basic sys A
    static Matrix kbAInit(3,3);
    kbAInit.Zero();
    kbAInit(0,0) = kbInit(0,0);
    kbAInit(1,1) = L*L*kbInit(1,1) + L*(kbInit(1,2)+kbInit(2,1)) + kbInit(2,2);
    kbAInit(1,2) = -L*kbInit(1,2) - kbInit(2,2);
    kbAInit(2,1) = -L*kbInit(2,1) - kbInit(2,2);
    kbAInit(2,2) = kbInit(2,2);
    
    // transform stiffness from the basic to the global system
    theInitStiff.Zero();
    theInitStiff = theCoordTransf->getInitialGlobalStiffMatrix(kbAInit);
    
    return 0;
}


const Matrix& EEBeamColumn2d::getTangentStiff()
{
    if (firstWarning == true)  {
        opserr << "\nWARNING EEBeamColumn2d::getTangentStiff() - "
            << "Element: " << this->getTag() << endln
            << "TangentStiff cannot be calculated." << endln
            << "Return InitialStiff including GeometricStiff instead." 
            << endln;
        opserr << "Subsequent getTangentStiff warnings will be suppressed."
            << endln;
        
        firstWarning = false;
    }
    
    // get current daq resisting force
    this->getBasicForce();
    
    // get chord rotation from basic sys A to B
    double alpha = atan2((*db)[1],L+(*db)[0]);
    
    // apply optional initial stiffness modification
    if (iMod == true)
        this->applyIMod();
    
    // use elastic axial force if axial force from test is zero
    if (fabs((*qbDaq)[0]) < 1.0E-12)  {
        double qA0 = kbInit(0,0)*(sqrt(pow(L+(*db)[0],2)+pow((*db)[1],2))-L);
        (*qbDaq)[0] = cos(alpha)*qA0;
        (*qbDaq)[1] += sin(alpha)*qA0;
    }
    
    static Vector qA(3);
    if (nlGeo == 0 || nlGeo == 1)  {
        // transform forces from basic sys B to basic sys A (linear)
        qA(0) = (*qbDaq)[0];
        qA(1) = -L*(*qbDaq)[1] - (*qbDaq)[2];
        qA(2) = (*qbDaq)[2];
    } else if (nlGeo == 2)  {
        // transform forces from basic sys B to basic sys A (nonlinear)
        qA(0) = cos(alpha)*(*qbDaq)[0] + sin(alpha)*(*qbDaq)[1];
        qA(1) = (*db)[1]*(*qbDaq)[0] - (L+(*db)[0])*(*qbDaq)[1] - (*qbDaq)[2];
        qA(2) = (*qbDaq)[2];
    }
    
    // add fixed end forces
    for (int i=0; i<3; i++)
        qA(i) += qA0[i];
    
    // transform stiffness from basic sys B to basic sys A
    static Matrix kbAInit(3,3);
    kbAInit.Zero();
    kbAInit(0,0) = kbInit(0,0);
    kbAInit(1,1) = L*L*kbInit(1,1) + L*(kbInit(1,2)+kbInit(2,1)) + kbInit(2,2);
    kbAInit(1,2) = -L*kbInit(1,2) - kbInit(2,2);
    kbAInit(2,1) = -L*kbInit(2,1) - kbInit(2,2);
    kbAInit(2,2) = kbInit(2,2);
    
    return theCoordTransf->getGlobalStiffMatrix(kbAInit, qA);
}


const Matrix& EEBeamColumn2d::getDamp()
{
    // zero the global matrix
    theMatrix.Zero();
    
    // call base class to setup Rayleigh damping
    if (addRayleigh == 1)
        theMatrix = this->Element::getDamp();
    
    return theMatrix;
}


const Matrix& EEBeamColumn2d::getMass()
{
    // zero the global matrix
    theMatrix.Zero();
    
    // form mass matrix
    if (L != 0.0 && rho != 0.0)  {
        if (cMass == false)  {
            // lumped mass matrix
            double m = 0.5*rho*L;
            theMatrix(0,0) = m;
            theMatrix(1,1) = m;	
            theMatrix(3,3) = m;
            theMatrix(4,4) = m;
        } else  {
            // consistent mass matrix
            static Matrix ml(6,6);
            double m = rho*L/420.0;
            ml(0,0) = ml(3,3) = m*140.0;
            ml(0,3) = ml(3,0) = m*70.0;
            
            ml(1,1) = ml(4,4) = m*156.0;
            ml(1,4) = ml(4,1) = m*54.0;
            ml(2,2) = ml(5,5) = m*4.0*L*L;
            ml(2,5) = ml(5,2) = -m*3.0*L*L;
            ml(1,2) = ml(2,1) = m*22.0*L;
            ml(4,5) = ml(5,4) = -ml(1,2);
            ml(1,5) = ml(5,1) = -m*13.0*L;
            ml(2,4) = ml(4,2) = -ml(1,5);
            
            // transform local mass matrix to global system
            theMatrix = theCoordTransf->getGlobalMatrixFromLocal(ml);
        }
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
        double wt = data(0)*loadFactor;  // transverse (+ve upward)
        double wa = data(1)*loadFactor;  // axial (+ve from node I to J)
        
        double V = 0.5*wt*L;
        double M = V*L/6.0; // wt*L*L/12
        double P = wa*L;
        
        // reactions in basic system
        pA0[0] -= P;
        pA0[1] -= V;
        pA0[2] -= V;
        
        // fixed end forces in basic system
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
        
        // reactions in basic system
        pA0[0] -= N;
        double V1 = P*(1.0-aOverL);
        double V2 = P*aOverL;
        pA0[1] -= V1;
        pA0[2] -= V2;
        
        double L2 = 1.0/(L*L);
        double a2 = a*a;
        double b2 = b*b;
        
        // fixed end forces in basic system
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
    if (L == 0.0 || rho == 0.0)
        return 0; 
    
    // get R * accel from the nodes
    const Vector &Raccel1 = theNodes[0]->getRV(accel);
    const Vector &Raccel2 = theNodes[1]->getRV(accel);
    
    if (3 != Raccel1.Size() || 3 != Raccel2.Size())  {
        opserr << "EEBeamColumn2d::addInertiaLoadToUnbalance() - "
            << "matrix and vector sizes are incompatible\n";
        return -1;
    }
    
    // want to add ( - fact * M R * accel ) to unbalance
    if (cMass == false)  {
        // take advantage of lumped mass matrix
        double m = 0.5*rho*L;
        theLoad(0) -= m * Raccel1(0);
        theLoad(1) -= m * Raccel1(1);
        theLoad(3) -= m * Raccel2(0);
        theLoad(4) -= m * Raccel2(1);
    } else  {
        // use matrix vector multip. for consistent mass matrix
        static Vector Raccel(6);
        for (int i=0; i<3; i++)  {
            Raccel(i)   = Raccel1(i);
            Raccel(i+3) = Raccel2(i);
        }
        theLoad.addMatrixVector(1.0, this->getMass(), Raccel, -1.0);
    }
    
    return 0;
}


const Vector& EEBeamColumn2d::getResistingForce()
{
    // make sure the coordinate transformation is updated
    theCoordTransf->update();
    
    // zero the global residual
    theVector.Zero();
    
    // get current daq resisting force
    this->getBasicForce();
    
    // get chord rotation from basic sys A to B
    double alpha = atan2((*db)[1],L+(*db)[0]);
    
    // apply optional initial stiffness modification
    if (iMod == true)
        this->applyIMod();
    
    // use elastic axial force if axial force from test is zero
    if (fabs((*qbDaq)[0]) < 1.0E-12)  {
        double qA0 = kbInit(0,0)*(sqrt(pow(L+(*db)[0],2)+pow((*db)[1],2))-L);
        (*qbDaq)[0] = cos(alpha)*qA0;
        (*qbDaq)[1] += sin(alpha)*qA0;
    }
    
    // save corresponding ctrl response for recorder
    dbCtrl = (*db);
    vbCtrl = (*vb);
    abCtrl = (*ab);
    
    static Vector qA(3);
    if (nlGeo == 0 || nlGeo == 1)  {
        // transform forces from basic sys B to basic sys A (linear)
        qA(0) = (*qbDaq)[0];
        qA(1) = -L*(*qbDaq)[1] - (*qbDaq)[2];
        qA(2) = (*qbDaq)[2];
    } else if (nlGeo == 2)  {
        // transform forces from basic sys B to basic sys A (nonlinear)
        qA(0) = cos(alpha)*(*qbDaq)[0] + sin(alpha)*(*qbDaq)[1];
        qA(1) = (*db)[1]*(*qbDaq)[0] - (L+(*db)[0])*(*qbDaq)[1] - (*qbDaq)[2];
        qA(2) = (*qbDaq)[2];
    }
    
    // add fixed end forces
    for (int i=0; i<3; i++)
        qA(i) += qA0[i];
    
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
    // this already includes damping forces from specimen
    this->getResistingForce();
    
    // add the damping forces from Rayleigh damping
    if (addRayleigh == 1)  {
        if (alphaM != 0.0 || betaK != 0.0 || betaK0 != 0.0 || betaKc != 0.0)
            theVector.addVector(1.0, this->getRayleighDampingForces(), 1.0);
    }
    
    // check for quick return
    if (L == 0.0 || rho == 0.0)
        return theVector;
    
    // add inertia forces from element mass
    const Vector &accel1 = theNodes[0]->getTrialAccel();
    const Vector &accel2 = theNodes[1]->getTrialAccel();
    
    if (cMass == false)  {
        // take advantage of lumped mass matrix
        double m = 0.5*rho*L;
        theVector(0) += m * accel1(0);
        theVector(1) += m * accel1(1);
        theVector(3) += m * accel2(0);
        theVector(4) += m * accel2(1);
    } else  {
        // use matrix vector multip. for consistent mass matrix
        static Vector accel(6);
        for (int i=0; i<3; i++)  {
            accel(i)   = accel1(i);
            accel(i+3) = accel2(i);
        }
        theVector.addMatrixVector(1.0, this->getMass(), accel, 1.0);
    }
    
    return theVector;
}


const Vector& EEBeamColumn2d::getTime()
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


const Vector& EEBeamColumn2d::getBasicDisp()
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


const Vector& EEBeamColumn2d::getBasicVel()
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


const Vector& EEBeamColumn2d::getBasicAccel()
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


const Vector& EEBeamColumn2d::getBasicForce()
{
    if (theSite != 0)  {
        (*qbDaq) = theSite->getForce();
    }
    else  {
        sData[0] = OF_RemoteTest_getForce;
        theChannel->sendVector(0, 0, *sendData, 0);
        theChannel->recvVector(0, 0, *recvData, 0);
    }
    
    return *qbDaq;
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
    int displayMode, float fact, const char **modes, int numMode)
{
    // first determine the end points of the beam based on
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
    
    return theViewer.drawLine (v1, v2, 1.0, 1.0, this->getTag(), 0);
}


void EEBeamColumn2d::Print(OPS_Stream &s, int flag)
{
    if (flag == 0)  {
        // print everything
        s << "Element: " << this->getTag() << endln;
        s << "  type: EEBeamColumn2d" << endln;
        s << "  iNode: " << connectedExternalNodes(0) 
            << ", jNode: " << connectedExternalNodes(1) << endln;
        s << "  CoordTransf: " << theCoordTransf->getTag() << endln;
        if (theSite != 0)
            s << "  ExperimentalSite: " << theSite->getTag() << endln;
        if (iMod)
            s << "  iMod: 1";
        else
            s << "  iMod: 0";
        s << ", addRayleigh: " << addRayleigh << endln;
        s << "  mass per unit length: " << rho;
        if (cMass)
            s << ", cMass: 1\n";
        else
            s << ", cMass: 0\n";
        // determine resisting forces in global system
        s << "  resisting force: " << this->getResistingForce() << endln;
    } else if (flag == 1)  {
        // does nothing
    }
}


Response* EEBeamColumn2d::setResponse(const char **argv, int argc,
    OPS_Stream &output)
{
    Response *theResponse = 0;
    
    output.tag("ElementOutput");
    output.attr("eleType","EEBeamColumn2d");
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
        
        theResponse = new ElementResponse(this, 1, theVector);
    }
    
    // local forces
    else if (strcmp(argv[0],"localForce") == 0 ||
        strcmp(argv[0],"localForces") == 0)
    {
        output.tag("ResponseType","N_1");
        output.tag("ResponseType","V_1");
        output.tag("ResponseType","M_1");
        output.tag("ResponseType","N_2");
        output.tag("ResponseType","V_2");
        output.tag("ResponseType","M_2");
        
        theResponse = new ElementResponse(this, 2, theVector);
    }
    
    // forces in basic system B
    else if (strcmp(argv[0],"basicForce") == 0 ||
        strcmp(argv[0],"basicForces") == 0 ||
        strcmp(argv[0],"daqForce") == 0 ||
        strcmp(argv[0],"daqForces") == 0)
    {
        output.tag("ResponseType","qb1");
        output.tag("ResponseType","qb2");
        output.tag("ResponseType","qb3");
        
        theResponse = new ElementResponse(this, 3, Vector(3));
    }
    
    // ctrl displacements in basic system B
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
    
    // ctrl velocities in basic system B
    else if (strcmp(argv[0],"ctrlVel") == 0 ||
        strcmp(argv[0],"ctrlVelocity") == 0 ||
        strcmp(argv[0],"ctrlVelocities") == 0)
    {
        output.tag("ResponseType","vb1");
        output.tag("ResponseType","vb2");
        output.tag("ResponseType","vb3");
        
        theResponse = new ElementResponse(this, 5, Vector(3));
    }
    
    // ctrl accelerations in basic system B
    else if (strcmp(argv[0],"ctrlAccel") == 0 ||
        strcmp(argv[0],"ctrlAcceleration") == 0 ||
        strcmp(argv[0],"ctrlAccelerations") == 0)
    {
        output.tag("ResponseType","ab1");
        output.tag("ResponseType","ab2");
        output.tag("ResponseType","ab3");
        
        theResponse = new ElementResponse(this, 6, Vector(3));
    }
    
    // daq displacements in basic system B
    else if (strcmp(argv[0],"daqDisp") == 0 ||
        strcmp(argv[0],"daqDisplacement") == 0 ||
        strcmp(argv[0],"daqDisplacements") == 0)
    {
        output.tag("ResponseType","dbDaq1");
        output.tag("ResponseType","dbDaq2");
        output.tag("ResponseType","dbDaq3");
        
        theResponse = new ElementResponse(this, 7, Vector(3));
    }
    
    // daq velocities in basic system B
    else if (strcmp(argv[0],"daqVel") == 0 ||
        strcmp(argv[0],"daqVelocity") == 0 ||
        strcmp(argv[0],"daqVelocities") == 0)
    {
        output.tag("ResponseType","vbDaq1");
        output.tag("ResponseType","vbDaq2");
        output.tag("ResponseType","vbDaq3");
        
        theResponse = new ElementResponse(this, 8, Vector(3));
    }
    
    // daq accelerations in basic system B
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


int EEBeamColumn2d::getResponse(int responseID, Information &eleInfo)
{
    double L = theCoordTransf->getInitialLength();
    double alpha;
    static Vector qA(3);
    
    switch (responseID)  {
    case 1:  // global forces
        return eleInfo.setVector(this->getResistingForce());
        
    case 2:  // local forces
        /* transform forces from basic sys B to basic sys A (linear)
        qA(0) = (*qbDaq)[0];
        qA(1) = -L*(*qbDaq)[1] - (*qbDaq)[2];
        qA(2) = (*qbDaq)[2];*/
        
        // transform forces from basic sys B to basic sys A (nonlinear)
        alpha = atan2((*db)[1],L+(*db)[0]);
        qA(0) = cos(alpha)*(*qbDaq)[0] + sin(alpha)*(*qbDaq)[1];
        qA(1) = (*db)[1]*(*qbDaq)[0] - (L+(*db)[0])*(*qbDaq)[1] - (*qbDaq)[2];
        qA(2) = (*qbDaq)[2];
        
        // Axial
        theVector(0) = -qA(0) + pA0[0];
        theVector(3) =  qA(0);
        // Shear
        theVector(1) =  (qA(1)+qA(2))/L + pA0[1];
        theVector(4) = -(qA(1)+qA(2))/L + pA0[2];
        // Moment
        theVector(2) =  qA(1);
        theVector(5) =  qA(2);
        
        return eleInfo.setVector(theVector);
        
    case 3:  // forces in basic system B (already current)
        return eleInfo.setVector(*qbDaq);
        
    case 4:  // ctrl displacements in basic system B
        return eleInfo.setVector(dbCtrl);
        
    case 5:  // ctrl velocities in basic system B
        return eleInfo.setVector(vbCtrl);
        
    case 6:  // ctrl accelerations in basic system B
        return eleInfo.setVector(abCtrl);
        
    case 7:  // daq displacements in basic system B
        return eleInfo.setVector(this->getBasicDisp());
        
    case 8:  // daq velocities in basic system B
        return eleInfo.setVector(this->getBasicVel());
        
    case 9:  // daq accelerations in basic system B
        return eleInfo.setVector(this->getBasicAccel());
        
    default:
        return -1;
    }
}


void EEBeamColumn2d::applyIMod()
{
    // get current daq displacement
    this->getBasicDisp();
    
    // correct for displacement control errors using I-Modification
    if ((*dbDaq)[0] != 0.0)  {
        (*qbDaq)[0] -= kbInit(0,0)*((*dbDaq)[0] - (*db)[0]);
    }
    if ((*dbDaq)[1] != 0.0)  {
        (*qbDaq)[1] -= kbInit(1,1)*((*dbDaq)[1] - (*db)[1]);
        (*qbDaq)[2] -= kbInit(2,1)*((*dbDaq)[1] - (*db)[1]);
    }
    if ((*dbDaq)[2] != 0.0)  {
        (*qbDaq)[1] -= kbInit(1,2)*((*dbDaq)[2] - (*db)[2]);
        (*qbDaq)[2] -= kbInit(2,2)*((*dbDaq)[2] - (*db)[2]);
    }
}
