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
// Description: This file contains the implementation of the EETruss class.

#include "EETruss.h"

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
Matrix EETruss::EETrussM2(2,2);
Matrix EETruss::EETrussM4(4,4);
Matrix EETruss::EETrussM6(6,6);
Matrix EETruss::EETrussM12(12,12);
Vector EETruss::EETrussV2(2);
Vector EETruss::EETrussV4(4);
Vector EETruss::EETrussV6(6);
Vector EETruss::EETrussV12(12);

void* OPF_EETruss()
{
    // pointer to experimental element that will be returned
    ExperimentalElement* theExpElement = 0;
    int ndm = OPS_GetNDM();
    
    if (OPS_GetNumRemainingInputArgs() < 7) {
        opserr << "WARNING invalid number of arguments\n";
        opserr << "Want: expElement truss eleTag iNode jNode -site siteTag -initStif Kij <-tangStif tangStifTag> <-iMod> <-noRayleigh> <-rho rho> <-cMass>\n";
        opserr << "  or: expElement truss eleTag iNode jNode -server ipPort <ipAddr> <-ssl> <-udp> <-dataSize size> -initStif Kij <-tangStif tangStifTag> <-iMod> <-noRayleigh> <-rho rho> <-cMass>\n";
        return 0;
    }
    
    // element tag
    int tag;
    int numdata = 1;
    if (OPS_GetIntInput(&numdata, &tag) != 0) {
        opserr << "WARNING invalid expElement truss eleTag\n";
        return 0;
    }
    
    // nodes
    int node[2];
    numdata = 2;
    if (OPS_GetIntInput(&numdata, node) != 0) {
        opserr << "WARNING invalid iNode or jNode\n";
        opserr << "expElement truss element: " << tag << endln;
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
            opserr << "expElement truss element: " << tag << endln;
            return 0;
        }
        theSite = OPF_getExperimentalSite(siteTag);
        if (theSite == 0) {
            opserr << "WARNING experimental site not found\n";
            opserr << "expSite: " << siteTag << endln;
            opserr << "expElement truss element: " << tag << endln;
            return 0;
        }
    }
    else if (strcmp(type, "-server") == 0) {
        // ip port
        numdata = 1;
        if (OPS_GetIntInput(&numdata, &ipPort) < 0) {
            opserr << "WARNING invalid ipPort\n";
            opserr << "expElement truss element: " << tag << endln;
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
                    opserr << "expElement truss element: " << tag << endln;
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
        opserr << "expElement truss element: " << tag << endln;
        return 0;
    }
    
    // initial stiffness
    type = OPS_GetString();
    if (strcmp(type, "-initStif") != 0 &&
        strcmp(type, "-initStiff") != 0) {
        opserr << "WARNING expecting -initStif\n";
        opserr << "expElement truss element: " << tag << endln;
    }
    Matrix theInitStif(1, 1);
    double stif;
    numdata = 1;
    if (OPS_GetDoubleInput(&numdata, &stif) < 0) {
        opserr << "WARNING invalid initial stiffness term\n";
        opserr << "expElement truss element: " << tag << endln;
        return 0;
    }
    theInitStif(0, 0) = stif;
    
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
                opserr << "expElement truss element: " << tag << endln;
                return 0;
            }
            theTangStif = OPF_getExperimentalTangentStiff(tangStifTag);
            if (theTangStif == 0) {
                opserr << "WARNING experimental tangent stiff not found\n";
                opserr << "expTangStiff: " << tangStifTag << endln;
                opserr << "expElement truss element: " << tag << endln;
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
                opserr << "expElement truss element: " << tag << endln;
                return 0;
            }
        }
        else if (strcmp(type, "-cMass") == 0) {
            cMass = true;
        }
    }
    
    // now create the EETruss
    if (theSite != 0) {
        theExpElement = new EETruss(tag, ndm, node[0], node[1],
            theSite, theTangStif, iMod, doRayleigh, rho, cMass);
    }
    else {
        theExpElement = new EETruss(tag, ndm, node[0], node[1],
            ipPort, ipAddr, ssl, udp, dataSize, theTangStif,
            iMod, doRayleigh, rho, cMass);
    }
    if (theExpElement == 0) {
        opserr << "WARNING ran out of memory creating element\n";
        opserr << "expElement truss element: " << tag << endln;
        return 0;
    }
    
    // add initial stiffness
    int setInitStif = theExpElement->setInitialStiff(theInitStif);
    if (setInitStif != 0) {
        opserr << "WARNING initial stiffness not set\n";
        opserr << "expElement truss element: " << tag << endln;
        return 0;
    }
    
    return theExpElement;
}


// responsible for allocating the necessary space needed
// by each object and storing the tags of the end nodes.
EETruss::EETruss(int tag, int dim, int Nd1, int Nd2,
    ExperimentalSite *site, ExperimentalTangentStiff *tang,
    bool iM, int addRay, double r, bool cm)
    : ExperimentalElement(tag, ELE_TAG_EETruss, site, tang),
    numDIM(dim), numDOF(0), connectedExternalNodes(2),
    iMod(iM), addRayleigh(addRay), rho(r), cMass(cm), L(0.0),
    theMatrix(0), theVector(0), theLoad(0),
    db(0), vb(0), ab(0), t(0),
    dbDaq(0), vbDaq(0), abDaq(0), qbDaq(0), tDaq(0),
    dbCtrl(1), vbCtrl(1), abCtrl(1),
    kb(1,1), kbInit(1,1), kbLast(1,1),
    dbLast(1), dbDaqLast(1), qbDaqLast(1), tLast(0.0),
    firstWarning(true)
{
    // ensure the connectedExternalNode ID is of correct size & set values
    if (connectedExternalNodes.Size() != 2)  {
        opserr << "EETruss::EETruss() - element: "
            <<  tag << " failed to create an ID of size 2\n";
        exit(-1);
    }
    
    connectedExternalNodes(0) = Nd1;
    connectedExternalNodes(1) = Nd2;
    
    // set node pointers to NULL
    theNodes[0] = 0;
    theNodes[1] = 0;
    
    // zero direction cosines
    cosX[0] = 0.0;
    cosX[1] = 0.0;
    cosX[2] = 0.0;
    
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
    qbDaq = new Vector(1);
    tDaq  = new Vector(1);
    
    // initialize additional vectors
    dbCtrl.Zero();
    vbCtrl.Zero();
    abCtrl.Zero();
    dbLast.Zero();
    dbDaqLast.Zero();
    qbDaqLast.Zero();
}


// responsible for allocating the necessary space needed
// by each object and storing the tags of the end nodes.
EETruss::EETruss(int tag, int dim, int Nd1, int Nd2,
    int port, char *machineInetAddr, int ssl, int udp,
    int dataSize, ExperimentalTangentStiff *tang,
    bool iM, int addRay, double r, bool cm)
    : ExperimentalElement(tag, ELE_TAG_EETruss, NULL, tang),
    numDIM(dim), numDOF(0), connectedExternalNodes(2),
    iMod(iM), addRayleigh(addRay), rho(r), cMass(cm), L(0.0),
    theMatrix(0), theVector(0), theLoad(0),
    theChannel(0), sData(0), sendData(0), rData(0), recvData(0),
    db(0), vb(0), ab(0), t(0),
    dbDaq(0), vbDaq(0), abDaq(0), qbDaq(0), tDaq(0),
    dbCtrl(1), vbCtrl(1), abCtrl(1),
    kb(1,1), kbInit(1,1), kbLast(1,1),
    dbLast(1), dbDaqLast(1), qbDaqLast(1), tLast(0.0),
    firstWarning(true)
{
    // ensure the connectedExternalNode ID is of correct size & set values
    if (connectedExternalNodes.Size() != 2)  {
        opserr << "EETruss::EETruss() - element: "
            <<  tag << " failed to create an ID of size 2\n";
        exit(-1);
    }
    
    connectedExternalNodes(0) = Nd1;
    connectedExternalNodes(1) = Nd2;
    
    // set node pointers to NULL
    theNodes[0] = 0;
    theNodes[1] = 0;
    
    // zero direction cosines
    cosX[0] = 0.0;
    cosX[1] = 0.0;
    cosX[2] = 0.0;
    
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
        opserr << "EETruss::EETruss() "
            << "- failed to create channel\n";
        exit(-1);
    }
    if (theChannel->setUpConnection() != 0)  {
        opserr << "EETruss::EETruss() "
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
    qbDaq = new Vector(&rData[id], 1);
    id += 1;
    tDaq = new Vector(&rData[id], 1);
    recvData->Zero();
    
    // initialize additional vectors
    dbCtrl.Zero();
    vbCtrl.Zero();
    abCtrl.Zero();
    dbLast.Zero();
    dbDaqLast.Zero();
    qbDaqLast.Zero();
}


// delete must be invoked on any objects created by the object.
EETruss::~EETruss()
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


int EETruss::getNumExternalNodes() const
{
    return 2;
}


const ID& EETruss::getExternalNodes()
{
    return connectedExternalNodes;
}


Node** EETruss::getNodePtrs()
{
    return theNodes;
}


int EETruss::getNumDOF()
{
    return numDOF;
}


int EETruss::getNumBasicDOF()
{
    return 1;
}


// to set a link to the enclosing Domain and to set the node pointers.
void EETruss::setDomain(Domain *theDomain)
{
    // check Domain is not null - invoked when object removed from a domain
    if (!theDomain)  {
        theNodes[0] = 0;
        theNodes[1] = 0;
        L = 0.0;
        return;
    }
    
    // set default values for error conditions
    numDOF = 2;
    theMatrix = &EETrussM2;
    theVector = &EETrussV2;	
    
    // first set the node pointers
    int Nd1 = connectedExternalNodes(0);
    int Nd2 = connectedExternalNodes(1);
    theNodes[0] = theDomain->getNode(Nd1);
    theNodes[1] = theDomain->getNode(Nd2);
    
    // if can't find both - send a warning message
    if (!theNodes[0] || !theNodes[1])  {
        if (!theNodes[0])  {
            opserr << "EETruss::setDomain() - Nd1: "
                << Nd1 << "does not exist in the model for ";
        } else  {
            opserr << "EETruss::setDomain() - Nd2: "
                << Nd2 << "does not exist in the model for ";
        }
        opserr << "EETruss ele: " << this->getTag() << endln;
        
        return;
    }
    
    // now determine the number of dof and the dimension
    int dofNd1 = theNodes[0]->getNumberDOF();
    int dofNd2 = theNodes[1]->getNumberDOF();
    
    // if differing dof at the ends - print a warning message
    if (dofNd1 != dofNd2)  {
        opserr <<"EETruss::setDomain(): nodes " << Nd1 << " and " << Nd2
            << "have differing dof at ends for element: " << this->getTag() << endln;
        
        return;
    }
    
    // call the base class method
    this->DomainComponent::setDomain(theDomain);
    
    // now set the number of dof for element and set matrix and vector pointer
    if (numDIM == 1 && dofNd1 == 1)  {
        numDOF = 2;
        theMatrix = &EETrussM2;
        theVector = &EETrussV2;
    }
    else if (numDIM == 2 && dofNd1 == 2)  {
        numDOF = 4;
        theMatrix = &EETrussM4;
        theVector = &EETrussV4;
    }
    else if (numDIM == 2 && dofNd1 == 3)  {
        numDOF = 6;
        theMatrix = &EETrussM6;
        theVector = &EETrussV6;
    }
    else if (numDIM == 3 && dofNd1 == 3)  {
        numDOF = 6;
        theMatrix = &EETrussM6;
        theVector = &EETrussV6;
    }
    else if (numDIM == 3 && dofNd1 == 6)  {
        numDOF = 12;
        theMatrix = &EETrussM12;
        theVector = &EETrussV12;
    }
    else  {
        opserr <<"EETruss::setDomain() - can not handle "
            << numDIM << " dofs at nodes in " << dofNd1  << " d problem\n";
        
        return;
    }
    
    // set the initial stiffness matrix size
    theInitStiff.resize(numDOF, numDOF);
    
    if (!theLoad)
        theLoad = new Vector(numDOF);
    else if (theLoad->Size() != numDOF)  {
        delete theLoad;
        theLoad = new Vector(numDOF);
    }
    
    if (!theLoad)  {
        opserr << "EETruss::setDomain() - element: " << this->getTag()
            << " out of memory creating vector of size: " << numDOF << endln;
        
        return;
    }
    
    // now determine the length, cosines and fill in the transformation
    // NOTE t = -t(every one else uses for residual calc)
    const Vector &end1Crd = theNodes[0]->getCrds();
    const Vector &end2Crd = theNodes[1]->getCrds();
    
    // initalize the cosines
    cosX[0] = cosX[1] = cosX[2] = 0.0;
    for (int i=0; i<numDIM; i++)
        cosX[i] = end2Crd(i)-end1Crd(i);
    
    // get initial length
    L = sqrt(cosX[0]*cosX[0] + cosX[1]*cosX[1] + cosX[2]*cosX[2]);
    if (L == 0.0)  {
        opserr <<"EETruss::setDomain() - element: "
            << this->getTag() << " has zero length\n";
        return;
    }
    
    // set global orientations
    cosX[0] /= L;
    cosX[1] /= L;
    cosX[2] /= L;
}


int EETruss::commitState()
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
    const Vector &disp1 = theNodes[0]->getTrialDisp();
    const Vector &disp2 = theNodes[1]->getTrialDisp();
    dbLast(0) = 0.0;
    for (int i=0; i<numDIM; i++)  {
        dbLast(0) += (disp2(i)-disp1(i))*cosX[i];
    }
    
    return rValue;
}


int EETruss::update()
{
    int rValue = 0;
    
    // save the last response parameters
    tLast = (*t)(0);
    dbLast = (*db);
    dbDaqLast = (*dbDaq);
    qbDaqLast = (*qbDaq);
    kbLast = kb;
    
    // get current time
    Domain *theDomain = this->getDomain();
    (*t)(0) = theDomain->getCurrentTime();
    
    // determine current dsp, vel and acc in basic system
    const Vector &disp1 = theNodes[0]->getTrialDisp();
    const Vector &disp2 = theNodes[1]->getTrialDisp();
    const Vector &vel1 = theNodes[0]->getTrialVel();
    const Vector &vel2 = theNodes[1]->getTrialVel();
    const Vector &accel1 = theNodes[0]->getTrialAccel();
    const Vector &accel2 = theNodes[1]->getTrialAccel();
    
    (*db)(0) = (*vb)(0) = (*ab)(0) = 0.0;
    for (int i=0; i<numDIM; i++)  {
        (*db)(0) += (disp2(i)-disp1(i))*cosX[i];
        (*vb)(0) += (vel2(i)-vel1(i))*cosX[i];
        (*ab)(0) += (accel2(i)-accel1(i))*cosX[i];
    }
    
    // calculate incremental displacement command
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
    
    return rValue;
}


int EETruss::setInitialStiff(const Matrix& kbinit)
{
    if (kbinit.noRows() != 1 || kbinit.noCols() != 1)  {
        opserr << "EETruss::setInitialStiff(): " 
            << "matrix size is incorrect for element: "
            << this->getTag() << endln;
        return -1;
    }
    kb = kbInit = kbLast = kbinit;
    
    // transform the stiffness from the basic to the global system
    theInitStiff.Zero();
    int numDOF2 = numDOF/2;
    double temp;
    for (int i=0; i<numDIM; i++)  {
        for (int j=0; j<numDIM; j++)  {
            temp = cosX[i]*cosX[j]*kbInit(0,0);
            theInitStiff(i,j) = temp;
            theInitStiff(i+numDOF2,j) = -temp;
            theInitStiff(i,j+numDOF2) = -temp;
            theInitStiff(i+numDOF2,j+numDOF2) = temp;
        }
    }
    
    return 0;
}


const Matrix& EETruss::getTangentStiff()
{
    if (theTangStiff != 0)  {
        // zero the global matrix
        theMatrix->Zero();
        
        // get current daq displacement and resisting force
        this->getBasicDisp();
        this->getBasicForce();
        
        // calculate incremental displacement and force vectors
        Vector dbDaqIncr = (*dbDaq) - dbDaqLast;
        Vector qbDaqIncr = (*qbDaq) - qbDaqLast;
        
        // get updated kb matrix
        kb = theTangStiff->updateTangentStiff(&dbDaqIncr, (Vector*)0,
            (Vector*)0, &qbDaqIncr, (Vector*)0, &kbInit, &kbLast);
        
        // transform the kb from the basic to the global system
        int numDOF2 = numDOF/2;
        double temp;
        for (int i=0; i<numDIM; i++)  {
            for (int j=0; j<numDIM; j++)  {
                temp = cosX[i]*cosX[j]*kb(0,0);
                (*theMatrix)(i,j) = temp;
                (*theMatrix)(i+numDOF2,j) = -temp;
                (*theMatrix)(i,j+numDOF2) = -temp;
                (*theMatrix)(i+numDOF2,j+numDOF2) = temp;
            }
        }
        
        return *theMatrix;
    }
    else  {
        if (firstWarning == true)  {
            opserr << "\nWARNING EETruss::getTangentStiff() - "
                << "Element: " << this->getTag() << endln
                << "TangentStiff cannot be calculated. Return InitialStiff instead."
                << endln;
            opserr << "Subsequent getTangentStiff warnings will be suppressed."
                << endln;
            
            firstWarning = false;
        }
        
        return theInitStiff;
    }
}


const Matrix& EETruss::getDamp()
{
    // zero the global matrix
    theMatrix->Zero();
    
    // call base class to setup Rayleigh damping
    if (addRayleigh == 1)
        (*theMatrix) = this->Element::getDamp();
    
    return *theMatrix;
}


const Matrix& EETruss::getMass()
{
    // zero the global matrix
    theMatrix->Zero();
    int numDOF2 = numDOF/2;
    
    // form mass matrix
    if (L != 0.0 && rho != 0.0)  {
        if (cMass == false) {
            // lumped mass matrix
            double m = 0.5*rho*L;
            for (int i=0; i<numDIM; i++)  {
                (*theMatrix)(i,i) = m;
                (*theMatrix)(i+numDOF2,i+numDOF2) = m;
            }
        } else  {
            // consistent mass matrix
            double m = rho*L/6.0;
            for (int i=0; i<numDIM; i++)  {
                (*theMatrix)(i,i) = 2.0*m;
                (*theMatrix)(i,i+numDOF2) = m;
                (*theMatrix)(i+numDOF2,i) = m;
                (*theMatrix)(i+numDOF2,i+numDOF2) = 2.0*m;
            }
        }
    }
    
    return *theMatrix;
}


void EETruss::zeroLoad()
{
    theLoad->Zero();
}


int EETruss::addLoad(ElementalLoad *theLoad, double loadFactor)
{
    opserr <<"EETruss::addLoad() - "
        << "load type unknown for element: "
        << this->getTag() << endln;
    
    return -1;
}


int EETruss::addInertiaLoadToUnbalance(const Vector &accel)
{
    // check for a quick return
    if (L == 0.0 || rho == 0.0)
        return 0;
    
    // get R * accel from the nodes
    const Vector &Raccel1 = theNodes[0]->getRV(accel);
    const Vector &Raccel2 = theNodes[1]->getRV(accel);
    
    int nodalDOF = numDOF/2;
    if (nodalDOF != Raccel1.Size() || nodalDOF != Raccel2.Size()) {
        opserr <<"EETruss::addInertiaLoadToUnbalance() - "
            << "matrix and vector sizes are incompatible\n";
        return -1;
    }
    
    // want to add ( - fact * M R * accel ) to unbalance
    if (cMass == false)  {
        // lumped mass matrix
        double m = 0.5*rho*L;
        for (int i=0; i<numDIM; i++) {
            (*theLoad)(i) -= m*Raccel1(i);
            (*theLoad)(i+nodalDOF) -= m*Raccel2(i);
        }
    } else  {
        // consistent mass matrix
        double m = rho*L/6.0;
        for (int i=0; i<numDIM; i++) {
            (*theLoad)(i) -= 2.0*m*Raccel1(i) + m*Raccel2(i);
            (*theLoad)(i+nodalDOF) -= m*Raccel1(i) + 2.0*m*Raccel2(i);
        }
    }
    
    return 0;
}


const Vector& EETruss::getResistingForce()
{
    // zero the global residual
    theVector->Zero();
    
    // get current daq resisting force
    this->getBasicForce();
    
    // apply optional initial stiffness modification
    if (iMod == true)  {
        // get current daq displacement
        this->getBasicDisp();
        
        // correct for displacement control errors using I-Modification
        qbDaq->addMatrixVector(1.0, kbInit, (*dbDaq) - (*db), -1.0);
    }
    
    // save corresponding ctrl values for recorder
    dbCtrl = (*db);
    vbCtrl = (*vb);
    abCtrl = (*ab);
    
    // determine resisting forces in global system
    int numDOF2 = numDOF/2;
    for (int i=0; i<numDIM; i++)  {
        (*theVector)(i) = -cosX[i]*(*qbDaq)(0);
        (*theVector)(i+numDOF2) = cosX[i]*(*qbDaq)(0);
    }
    
    // subtract external load
    theVector->addVector(1.0, *theLoad, -1.0);
    
    return *theVector;
}


const Vector& EETruss::getResistingForceIncInertia()
{
    // this already includes damping forces from specimen
    this->getResistingForce();
    
    // add the damping forces from rayleigh damping
    if (addRayleigh == 1)  {
        if (alphaM != 0.0 || betaK != 0.0 || betaK0 != 0.0 || betaKc != 0.0)
            theVector->addVector(1.0, this->getRayleighDampingForces(), 1.0);
    }
    
    // check for quick return
    if (L == 0.0 || rho == 0.0)
        return *theVector;
    
    // add inertia forces from element mass
    const Vector &accel1 = theNodes[0]->getTrialAccel();
    const Vector &accel2 = theNodes[1]->getTrialAccel();
    int numDOF2 = numDOF/2;
    
    if (cMass == false)  {
        // lumped mass matrix
        double m = 0.5*rho*L;
        for (int i=0; i<numDIM; i++) {
            (*theVector)(i) += m * accel1(i);
            (*theVector)(i+numDOF2) += m * accel2(i);
        }
    } else  {
        // consistent mass matrix
        double m = rho*L/6.0;
        for (int i=0; i<numDIM; i++) {
            (*theVector)(i) += 2.0*m*accel1(i) + m*accel2(i);
            (*theVector)(i+numDOF2) += m*accel1(i) + 2.0*m*accel2(i);
        }
    }
    
    return *theVector;
}


const Vector& EETruss::getTime()
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


const Vector& EETruss::getBasicDisp()
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


const Vector& EETruss::getBasicVel()
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


const Vector& EETruss::getBasicAccel()
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


const Vector& EETruss::getBasicForce()
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


int EETruss::sendSelf(int commitTag, Channel &theChannel)
{
    // has not been implemented yet.....
    return 0;
}


int EETruss::recvSelf(int commitTag, Channel &theChannel,
    FEM_ObjectBroker &theBroker)
{
    // has not been implemented yet.....
    return 0;
}


int EETruss::displaySelf(Renderer &theViewer,
    int displayMode, float fact, const char **modes, int numMode)
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
        
        for (int i=0; i<numDIM; i++)  {
            v1(i) = end1Crd(i) + end1Disp(i)*fact;
            v2(i) = end2Crd(i) + end2Disp(i)*fact;
        }
    } else  {
        int mode = displayMode * -1;
        const Matrix &eigen1 = theNodes[0]->getEigenvectors();
        const Matrix &eigen2 = theNodes[1]->getEigenvectors();
        
        if (eigen1.noCols() >= mode)  {
            for (int i=0; i<numDIM; i++)  {
                v1(i) = end1Crd(i) + eigen1(i,mode-1)*fact;
                v2(i) = end2Crd(i) + eigen2(i,mode-1)*fact;
            }
        } else  {
            for (int i=0; i<numDIM; i++)  {
                v1(i) = end1Crd(i);
                v2(i) = end2Crd(i);
            }
        }
    }
    
    return theViewer.drawLine(v1, v2, 1.0, 1.0, this->getTag(), 0);
}


void EETruss::Print(OPS_Stream &s, int flag)
{
    if (flag == 0)  {
        // print everything
        s << "Element: " << this->getTag() << endln;
        s << "  type: EETruss" << endln;
        s << "  iNode: " << connectedExternalNodes(0)
            << ", jNode: " << connectedExternalNodes(1) << endln;
        if (theSite != 0)
            s << "  ExperimentalSite: " << theSite->getTag() << endln;
        if (theTangStiff != 0)
            s << "  ExperimentalTangStiff: " << theTangStiff->getTag() << endln;
        if (iMod)
            s << "  iMod: 1";
        else
            s << "  iMod: 0";
        s << ", addRayleigh: " << addRayleigh << endln;
        s << "  mass/length: " << rho;
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


Response* EETruss::setResponse(const char **argv, int argc,
    OPS_Stream &output)
{
    Response *theResponse = 0;
    
    output.tag("ElementOutput");
    output.attr("eleType","EETruss");
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
        output.tag("ResponseType","qb1");
        
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
    
    // tangent stiffness output
    else if (strcmp(argv[0],"tangStif") == 0 ||
        strcmp(argv[0],"tangStiff") == 0 ||
        strcmp(argv[0],"expTangStif") == 0 ||
        strcmp(argv[0],"expTangStiff") == 0 ||
        strcmp(argv[0],"expTangentStif") == 0 ||
        strcmp(argv[0],"expTangentStiff") == 0)  {
        if (theTangStiff != 0)  {
            theResponse = theTangStiff->setResponse(&argv[1], argc-1, output);
        }
    }
    
    output.endTag(); // ElementOutput
    
    return theResponse;
}


int EETruss::getResponse(int responseID, Information &eleInfo)
{
    switch (responseID)  {
    case 1:  // global forces
        return eleInfo.setVector(this->getResistingForce());
        
    case 2:  // local forces
        theVector->Zero();
        // axial (qbDaq is already current)
        (*theVector)(0)        = -(*qbDaq)(0);
        (*theVector)(numDOF/2) =  (*qbDaq)(0);
        
        return eleInfo.setVector(*theVector);
        
    case 3:  // basic force (qbDaq is already current)
        return eleInfo.setVector(*qbDaq);
        
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
