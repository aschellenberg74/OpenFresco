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
// Created: 10/06
// Revision: A
//
// Description: This file contains the implementation of the EEGeneric class.

#include "EEGeneric.h"

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


void* OPF_EEGeneric()
{
    // pointer to experimental element that will be returned
    ExperimentalElement* theExpElement = 0;
    int ndf = OPS_GetNDF();
    
    if (OPS_GetNumRemainingInputArgs() < 9) {
        opserr << "WARNING invalid number of arguments\n";
        opserr << "Want: expElement generic eleTag -node Ndi -dof dofNdi -dof dofNdj ... -site siteTag -initStif Kij <-iMod> <-noRayleigh> <-mass Mij> <-checkTime>\n";
        opserr << "  or: expElement generic eleTag -node Ndi -dof dofNdi -dof dofNdj ... -server ipPort <ipAddr> <-ssl> <-udp> <-dataSize size> -initStif Kij <-iMod> <-noRayleigh> <-mass Mij> <-checkTime>\n";
        return 0;
    }
    
    // element tag
    int tag;
    int numdata = 1;
    if (OPS_GetIntInput(&numdata, &tag) != 0) {
        opserr << "WARNING invalid expElement generic eleTag\n";
        return 0;
    }
    
    // nodes
    const char* type = OPS_GetString();
    if (strcmp(type, "-node") != 0) {
        opserr << "WARNING expecting -node Ndi Ndj ...\n";
        opserr << "expElement generic element: " << tag << endln;
        return 0;
    }
    ID nodes(32);
    int numNodes = 0;
    while (OPS_GetNumRemainingInputArgs() > 0) {
        int node;
        numdata = 1;
        int numArgs = OPS_GetNumRemainingInputArgs();
        if (OPS_GetIntInput(&numdata, &node) < 0) {
            if (numArgs > OPS_GetNumRemainingInputArgs()) {
                // move current arg back by one
                OPS_ResetCurrentInputArg(-1);
            }
            break;
        }
        nodes(numNodes++) = node;
    }
    nodes.resize(numNodes);
    
    // dofs
    int numDOF = 0;
    ID* dofs = new ID[numNodes];
    for (int i = 0; i < numNodes; i++) {
        type = OPS_GetString();
        if (strcmp(type, "-dof") != 0 && strcmp(type, "-dir") != 0) {
            opserr << "WARNING expecting -dof dofNd"
                << i + 1 << ", but got " << type << endln;
            opserr << "expElement generic element: " << tag << endln;
            return 0;
        }
        ID dofsi(ndf);
        int numDOFi = 0;
        while (OPS_GetNumRemainingInputArgs() > 0) {
            int dof;
            numdata = 1;
            int numArgs = OPS_GetNumRemainingInputArgs();
            if (OPS_GetIntInput(&numdata, &dof) < 0) {
                if (numArgs > OPS_GetNumRemainingInputArgs()) {
                    // move current arg back by one
                    OPS_ResetCurrentInputArg(-1);
                }
                break;
            }
            if (dof < 1 || ndf < dof) {
                opserr << "WARNING invalid dof ID\n";
                opserr << "expElement generic element: " << tag << endln;
                return 0;
            }
            dofsi(numDOFi++) = dof - 1;
            numDOF++;
        }
        dofsi.resize(numDOFi);
        dofs[i] = dofsi;
    }
    
    // experimental site or server parameters
    ExperimentalSite* theSite = 0;
    int ipPort = 8090;
    char* ipAddr = new char[10];
    strcpy(ipAddr, "127.0.0.1");
    int ssl = 0, udp = 0;
    int dataSize = OF_Network_dataSize;
    type = OPS_GetString();
    if (strcmp(type, "-site") == 0) {
        // site tag
        int siteTag;
        numdata = 1;
        if (OPS_GetIntInput(&numdata, &siteTag) < 0) {
            opserr << "WARNING invalid siteTag\n";
            opserr << "expElement generic element: " << tag << endln;
            return 0;
        }
        theSite = OPF_getExperimentalSite(siteTag);
        if (theSite == 0) {
            opserr << "WARNING experimental site not found\n";
            opserr << "expSite: " << siteTag << endln;
            opserr << "expElement generic element: " << tag << endln;
            return 0;
        }
    }
    else if (strcmp(type, "-server") == 0) {
        // ip port
        numdata = 1;
        if (OPS_GetIntInput(&numdata, &ipPort) < 0) {
            opserr << "WARNING invalid ipPort\n";
            opserr << "expElement generic element: " << tag << endln;
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
                    opserr << "expElement generic element: " << tag << endln;
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
        opserr << "expElement generic element: " << tag << endln;
        return 0;
    }
    
    // initial stiffness
    type = OPS_GetString();
    if (strcmp(type, "-initStif") != 0 &&
        strcmp(type, "-initStiff") != 0) {
        opserr << "WARNING expecting -initStif\n";
        opserr << "expElement generic element: " << tag << endln;
    }
    Matrix theInitStif(numDOF, numDOF);
    double stif[1024];
    numdata = numDOF * numDOF;
    if (OPS_GetDoubleInput(&numdata, stif) < 0) {
        opserr << "WARNING invalid initial stiffness term\n";
        opserr << "expElement generic element: " << tag << endln;
        return 0;
    }
    theInitStif.setData(stif, numDOF, numDOF);
    
    // optional parameters
    ExperimentalTangentStiff* theTangStif = 0;
    bool iMod = false;
    int doRayleigh = 1;
    Matrix* mass = 0;
    int checkTime = 0;
    while (OPS_GetNumRemainingInputArgs() > 0) {
        type = OPS_GetString();
        // tangent stiffness
        if (strcmp(type, "-tangStif") == 0 ||
            strcmp(type, "-tangStiff") == 0) {
            int tangStifTag;
            numdata = 1;
            if (OPS_GetIntInput(&numdata, &tangStifTag) < 0) {
                opserr << "WARNING invalid tangStifTag\n";
                opserr << "expElement generic element: " << tag << endln;
                return 0;
            }
            theTangStif = OPF_getExperimentalTangentStiff(tangStifTag);
            if (theTangStif == 0) {
                opserr << "WARNING experimental tangent stiff not found\n";
                opserr << "expTangStiff: " << tangStifTag << endln;
                opserr << "expElement generic element: " << tag << endln;
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
        else if (strcmp(type, "-mass") == 0) {
            if (OPS_GetNumRemainingInputArgs() < numDOF*numDOF) {
                opserr << "WARNING: insufficient mass values\n";
                opserr << "expElement generic element: " << tag << endln;
                return 0;
            }
            mass = new Matrix(numDOF, numDOF);
            double m[1024];
            numdata = numDOF*numDOF;
            if (OPS_GetDoubleInput(&numdata, m) < 0) {
                opserr << "WARNING: invalid -mass value\n";
                opserr << "expElement generic element: " << tag << endln;
                return 0;
            }
            mass->setData(m, numDOF, numDOF);
        }
        else if (strcmp(type, "-checkTime") == 0) {
            checkTime = 1;
        }
    }
    
    // now create the EEGeneric
    if (theSite != 0) {
        theExpElement = new EEGeneric(tag, nodes, dofs, theSite,
            iMod, doRayleigh, mass, checkTime);
    }
    else {
        theExpElement = new EEGeneric(tag, nodes, dofs, ipPort,
            ipAddr, ssl, udp, dataSize, iMod, doRayleigh, mass,
            checkTime);
    }
    if (theExpElement == 0) {
        opserr << "WARNING ran out of memory creating element\n";
        opserr << "expElement generic element: " << tag << endln;
        return 0;
    }
    
    // cleanup dynamic memory
    if (dofs != 0)
        delete[] dofs;
    if (mass != 0)
        delete mass;
    
    // add initial stiffness
    int setInitStif = theExpElement->setInitialStiff(theInitStif);
    if (setInitStif != 0) {
        opserr << "WARNING initial stiffness not set\n";
        opserr << "expElement generic element: " << tag << endln;
        return 0;
    }
    
    return theExpElement;
}


// responsible for allocating the necessary space needed
// by each object and storing the tags of the end nodes.
EEGeneric::EEGeneric(int tag, ID nodes, ID *dof,
    ExperimentalSite *site,
    bool iM, int addRay, const Matrix *m, int checktime)
    : ExperimentalElement(tag, ELE_TAG_EEGeneric, site),
    connectedExternalNodes(nodes), basicDOF(1),
    numExternalNodes(0), numDOF(0), numBasicDOF(0),
    iMod(iM), addRayleigh(addRay), mass(0),
    checkTime(checktime),
    theMatrix(1,1), theVector(1), theLoad(1),
    db(0), vb(0), ab(0), t(0),
    dbDaq(0), vbDaq(0), abDaq(0), qDaq(0), tDaq(0),
    dbCtrl(1), vbCtrl(1), abCtrl(1),
    kbInit(1,1), dbLast(1), tLast(0.0) 
{
    // initialize nodes
    numExternalNodes = connectedExternalNodes.Size();
    theNodes = new Node* [numExternalNodes];
    if (!theNodes)  {
        opserr << "EEGeneric::EEGeneric() "
            << "- failed to create node array\n";
        exit(-1);
    }
    
    // set node pointers to NULL
    int i;
    for (i=0; i<numExternalNodes; i++)
        theNodes[i] = 0;
    
    // initialize dof
    theDOF = new ID [numExternalNodes];
    if (!theDOF)  {
        opserr << "EEGeneric::EEGeneric() "
            << "- failed to create dof array\n";
        exit(-1);
    }
    numBasicDOF = 0;
    for (i=0; i<numExternalNodes; i++)  {
        numBasicDOF += dof[i].Size();
        theDOF[i] = dof[i];
    }
    
    // initialize mass matrix
    if (m != 0)
        mass = new Matrix(*m);
    
    // set the data size for the experimental site
    sizeCtrl = new ID(OF_Resp_All);
    sizeDaq = new ID(OF_Resp_All);
    
    (*sizeCtrl)[OF_Resp_Disp]  = numBasicDOF;
    (*sizeCtrl)[OF_Resp_Vel]   = numBasicDOF;
    (*sizeCtrl)[OF_Resp_Accel] = numBasicDOF;
    (*sizeCtrl)[OF_Resp_Time]  = 1;
    
    (*sizeDaq)[OF_Resp_Disp]   = numBasicDOF;
    (*sizeDaq)[OF_Resp_Vel]    = numBasicDOF;
    (*sizeDaq)[OF_Resp_Accel]  = numBasicDOF;
    (*sizeDaq)[OF_Resp_Force]  = numBasicDOF;
    (*sizeDaq)[OF_Resp_Time]   = 1;
    
    theSite->setSize(*sizeCtrl, *sizeDaq);
    
    // allocate memory for trial response vectors
    db = new Vector(numBasicDOF);
    vb = new Vector(numBasicDOF);
    ab = new Vector(numBasicDOF);
    t  = new Vector(1);
    
    // allocate memory for daq response vectors
    dbDaq = new Vector(numBasicDOF);
    vbDaq = new Vector(numBasicDOF);
    abDaq = new Vector(numBasicDOF);
    qDaq  = new Vector(numBasicDOF);
    tDaq  = new Vector(1);
    
    // set the vector and matrix sizes and zero them
    basicDOF.resize(numBasicDOF);
    basicDOF.Zero();
    dbCtrl.resize(numBasicDOF);
    dbCtrl.Zero();
    vbCtrl.resize(numBasicDOF);
    vbCtrl.Zero();
    abCtrl.resize(numBasicDOF);
    abCtrl.Zero();
    kbInit.resize(numBasicDOF,numBasicDOF);
    kbInit.Zero();
    dbLast.resize(numBasicDOF);
    dbLast.Zero();
}


// responsible for allocating the necessary space needed
// by each object and storing the tags of the end nodes.
EEGeneric::EEGeneric(int tag, ID nodes, ID *dof,
    int port, char *machineInetAddr, int ssl, int udp,
    int dataSize, bool iM, int addRay, const Matrix *m,
    int checktime)
    : ExperimentalElement(tag, ELE_TAG_EEGeneric),
    connectedExternalNodes(nodes), basicDOF(1),
    numExternalNodes(0), numDOF(0), numBasicDOF(0),
    iMod(iM), addRayleigh(addRay), mass(0),
    checkTime(checktime),
    theMatrix(1,1), theVector(1), theLoad(1),
    theChannel(0), sData(0), sendData(0), rData(0), recvData(0),
    db(0), vb(0), ab(0), t(0),
    dbDaq(0), vbDaq(0), abDaq(0), qDaq(0), tDaq(0),
    dbCtrl(1), vbCtrl(1), abCtrl(1),
    kbInit(1,1), dbLast(1), tLast(0.0)
{
    // initialize nodes
    numExternalNodes = connectedExternalNodes.Size();
    theNodes = new Node* [numExternalNodes];
    if (!theNodes)  {
        opserr << "EEGeneric::EEGeneric() "
            << "- failed to create node array\n";
        exit(-1);
    }
    
    // set node pointers to NULL
    int i;
    for (i=0; i<numExternalNodes; i++)
        theNodes[i] = 0;
    
    // initialize dof
    theDOF = new ID [numExternalNodes];
    if (!theDOF)  {
        opserr << "EEGeneric::EEGeneric() "
            << "- failed to create dof array\n";
        exit(-1);
    }
    numBasicDOF = 0;
    for (i=0; i<numExternalNodes; i++)  {
        numBasicDOF += dof[i].Size();
        theDOF[i] = dof[i];
    }
    
    // initialize mass matrix
    if (m != 0)
        mass = new Matrix(*m);
    
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
        opserr << "EEGeneric::EEGeneric() "
            << "- failed to create channel\n";
        exit(-1);
    }
    if (theChannel->setUpConnection() != 0)  {
        opserr << "EEGeneric::EEGeneric() "
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
    
    (*sizeCtrl)[OF_Resp_Disp]  = numBasicDOF;
    (*sizeCtrl)[OF_Resp_Vel]   = numBasicDOF;
    (*sizeCtrl)[OF_Resp_Accel] = numBasicDOF;
    (*sizeCtrl)[OF_Resp_Time]  = 1;
    
    (*sizeDaq)[OF_Resp_Disp]   = numBasicDOF;
    (*sizeDaq)[OF_Resp_Vel]    = numBasicDOF;
    (*sizeDaq)[OF_Resp_Accel]  = numBasicDOF;
    (*sizeDaq)[OF_Resp_Force]  = numBasicDOF;
    (*sizeDaq)[OF_Resp_Time]   = 1;
    
    if (dataSize < 4*numBasicDOF+1) dataSize = 4*numBasicDOF+1;
    intData[2*OF_Resp_All] = dataSize;
    theChannel->sendID(0, 0, idData, 0);
    
    // allocate memory for the send vectors
    int id = 1;
    sData = new double [dataSize];
    sendData = new Vector(sData, dataSize);
    db = new Vector(&sData[id], numBasicDOF);
    id += numBasicDOF;
    vb = new Vector(&sData[id], numBasicDOF);
    id += numBasicDOF;
    ab = new Vector(&sData[id], numBasicDOF);
    id += numBasicDOF;
    t = new Vector(&sData[id], 1);
    sendData->Zero();
    
    // allocate memory for the receive vectors
    id = 0;
    rData = new double [dataSize];
    recvData = new Vector(rData, dataSize);
    dbDaq = new Vector(&rData[id], numBasicDOF);
    id += numBasicDOF;
    vbDaq = new Vector(&rData[id], numBasicDOF);
    id += numBasicDOF;
    abDaq = new Vector(&rData[id], numBasicDOF);
    id += numBasicDOF;
    qDaq = new Vector(&rData[id], numBasicDOF);
    id += numBasicDOF;
    tDaq = new Vector(&rData[id], 1);
    recvData->Zero();
    
    // set the vector and matrix sizes and zero them
    basicDOF.resize(numBasicDOF);
    basicDOF.Zero();
    dbCtrl.resize(numBasicDOF);
    dbCtrl.Zero();
    vbCtrl.resize(numBasicDOF);
    vbCtrl.Zero();
    abCtrl.resize(numBasicDOF);
    abCtrl.Zero();
    kbInit.resize(numBasicDOF,numBasicDOF);
    kbInit.Zero();
    dbLast.resize(numBasicDOF);
    dbLast.Zero();
}


// delete must be invoked on any objects created by the object.
EEGeneric::~EEGeneric()
{
    // invoke the destructor on any objects created by the object
    // that the object still holds a pointer to
    if (theNodes != 0)
        delete [] theNodes;
    if (theDOF != 0)
        delete [] theDOF;
    if (mass != 0)
        delete mass;
    
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


int EEGeneric::getNumExternalNodes() const
{
    return numExternalNodes;
}


const ID& EEGeneric::getExternalNodes()
{
    return connectedExternalNodes;
}


Node** EEGeneric::getNodePtrs()
{
    return theNodes;
}


int EEGeneric::getNumDOF()
{
    return numDOF;
}


int EEGeneric::getNumBasicDOF()
{
    return numBasicDOF;
}


// to set a link to the enclosing Domain and to set the node pointers.
void EEGeneric::setDomain(Domain *theDomain)
{
    // check Domain is not null - invoked when object removed from a domain
    int i;
    if (!theDomain)  {
        for (i=0; i<numExternalNodes; i++)
            theNodes[i] = 0;
        return;
    }
    
    // first set the node pointers
    for (i=0; i<numExternalNodes; i++)
        theNodes[i] = theDomain->getNode(connectedExternalNodes(i));
    
    // if can't find all - send a warning message
    for (i=0; i<numExternalNodes; i++)  {
        if (!theNodes[i])  {
            opserr << "EEGeneric::setDomain() - Nd" << i << ": " 
                << connectedExternalNodes(i) << " does not exist in the "
                << "model for EEGeneric ele: " << this->getTag() << endln;
            return;
        }
    }
    
    // now determine the number of dof
    numDOF = 0;
    for (i=0; i<numExternalNodes; i++)  {
        numDOF += theNodes[i]->getNumberDOF();
    }
    
    // set the basicDOF ID
    int j, k = 0, ndf = 0;
    for (i=0; i<numExternalNodes; i++)  {
        for (j=0; j<theDOF[i].Size(); j++)  {
            basicDOF(k) = ndf + theDOF[i](j);
            k++;
        }
        ndf += theNodes[i]->getNumberDOF();
    }
    
    // set the matrix and vector sizes and zero them
    theInitStiff.resize(numDOF,numDOF);
    theInitStiff.Zero();
    theMatrix.resize(numDOF,numDOF);
    theMatrix.Zero();
    theVector.resize(numDOF);
    theVector.Zero();
    theLoad.resize(numDOF);
    theLoad.Zero();
    
    // call the base class method
    this->DomainComponent::setDomain(theDomain);
}


int EEGeneric::commitState()
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
    dbLast.Zero();
    for (i=0; i<numExternalNodes; i++)  {
        Vector disp = theNodes[i]->getTrialDisp();
        dbLast.Assemble(disp(theDOF[i]), ndim);
        ndim += theDOF[i].Size();
    }
    
    return rValue;
}


int EEGeneric::update()
{
    int rValue = 0;
    
    // get current time
    Domain *theDomain = this->getDomain();
    (*t)(0) = theDomain->getCurrentTime();
    
    // assemble response vectors
    int ndim = 0, i;
    db->Zero(); vb->Zero(); ab->Zero();
    for (i=0; i<numExternalNodes; i++)  {
        Vector disp = theNodes[i]->getTrialDisp();
        Vector vel = theNodes[i]->getTrialVel();
        Vector accel = theNodes[i]->getTrialAccel();
        db->Assemble(disp(theDOF[i]), ndim);
        vb->Assemble(vel(theDOF[i]), ndim);
        ab->Assemble(accel(theDOF[i]), ndim);
        ndim += theDOF[i].Size();
    }
    
    Vector dbDelta = (*db) - dbLast;
    // do not check time for right now because of transformation constraint
    // handler calling update at beginning of new step when applying load
    // if (dbDelta.pNorm(0) > DBL_EPSILON || (*t)(0) > tLast)  {
    if (dbDelta.pNorm(0) > DBL_EPSILON || (checkTime && (*t)(0) > tLast))  {
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


int EEGeneric::setInitialStiff(const Matrix &kbinit)
{
    if (kbinit.noRows() != numBasicDOF || kbinit.noCols() != numBasicDOF)  {
        opserr << "EEGeneric::setInitialStiff() - " 
            << "matrix size is incorrect for element: "
            << this->getTag() << endln;
        return -1;
    }
    kbInit = kbinit;
    
    theInitStiff.Zero();
    theInitStiff.Assemble(kbInit, basicDOF, basicDOF);
    
    return 0;
}


const Matrix& EEGeneric::getDamp()
{
    // zero the global matrix
    theMatrix.Zero();
    
    // call base class to setup Rayleigh damping
    if (addRayleigh == 1)  {
        theMatrix = this->Element::getDamp();
    }
    
    return theMatrix;
}


const Matrix& EEGeneric::getMass()
{
    // zero the global matrix
    theMatrix.Zero();
    
    // assemble mass matrix
    if (mass != 0)  {
        theMatrix.Assemble(*mass, basicDOF, basicDOF);
    }
    
    return theMatrix;
}


void EEGeneric::zeroLoad()
{
    theLoad.Zero();
}


int EEGeneric::addLoad(ElementalLoad *theLoad, double loadFactor)
{
    opserr <<"EEGeneric::addLoad() - "
        << "load type unknown for element: "
        << this->getTag() << endln;
    
    return -1;
}


int EEGeneric::addInertiaLoadToUnbalance(const Vector &accel)
{
    // check for quick return
    if (mass == 0)  {
        return 0;
    }
    
    int ndim = 0, i;
    Vector Raccel(numDOF);
    
    // get mass matrix
    Matrix M = this->getMass();
    // assemble Raccel vector
    for (i=0; i<numExternalNodes; i++ )  {
        Raccel.Assemble(theNodes[i]->getRV(accel), ndim);
        ndim += theNodes[i]->getNumberDOF();
    }
    
    // want to add ( - fact * M R * accel ) to unbalance
    theLoad.addMatrixVector(1.0, M, Raccel, -1.0);
    
    return 0;
}


const Vector& EEGeneric::getResistingForce()
{
    // zero the global residual
    theVector.Zero();
    
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
    
    // determine resisting forces in global system
    theVector.Assemble(*qDaq, basicDOF);
    
    // subtract external load
    theVector.addVector(1.0, theLoad, -1.0);
    
    return theVector;
}


const Vector& EEGeneric::getResistingForceIncInertia()
{
    // this already includes damping forces from specimen
    theVector = this->getResistingForce();
    
    // add the damping forces from rayleigh damping
    if (addRayleigh == 1)  {
        if (alphaM != 0.0 || betaK != 0.0 || betaK0 != 0.0 || betaKc != 0.0)
            theVector.addVector(1.0, this->getRayleighDampingForces(), 1.0);
    }
    
    // add inertia forces from element mass
    if (mass != 0)  {
        int ndim = 0, i;
        Vector accel(numDOF);
        
        // get mass matrix
        Matrix M = this->getMass();
        // assemble accel vector
        for (i=0; i<numExternalNodes; i++ )  {
            accel.Assemble(theNodes[i]->getTrialAccel(), ndim);
            ndim += theNodes[i]->getNumberDOF();
        }
        
        theVector.addMatrixVector(1.0, M, accel, 1.0);
    }
    
    return theVector;
}


const Vector& EEGeneric::getTime()
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


const Vector& EEGeneric::getBasicDisp()
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


const Vector& EEGeneric::getBasicVel()
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


const Vector& EEGeneric::getBasicAccel()
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


int EEGeneric::sendSelf(int commitTag, Channel &theChannel)
{
    // has not been implemented yet.....
    return 0;
}


int EEGeneric::recvSelf(int commitTag, Channel &theChannel,
    FEM_ObjectBroker &theBroker)
{
    // has not been implemented yet.....
    return 0;
}


int EEGeneric::displaySelf(Renderer &theViewer,
    int displayMode, float fact, const char **modes, int numMode)
{
    int rValue = 0, i, j;
    
    static Vector v1(3);
    static Vector v2(3);
    
    if (numExternalNodes > 1)  {
        if (displayMode >= 0)  {
            for (i=0; i<numExternalNodes-1; i++)  {
                const Vector &end1Crd = theNodes[i]->getCrds();
                const Vector &end2Crd = theNodes[i+1]->getCrds();
                
                const Vector &end1Disp = theNodes[i]->getDisp();
                const Vector &end2Disp = theNodes[i+1]->getDisp();
                
                int end1NumCrds = end1Crd.Size();
                int end2NumCrds = end2Crd.Size();
                
                for (j=0; j<end1NumCrds; j++)
                    v1(j) = end1Crd(j) + end1Disp(j)*fact;
                for (j=0; j<end2NumCrds; j++)
                    v2(j) = end2Crd(j) + end2Disp(j)*fact;
                
                rValue += theViewer.drawLine(v1, v2, 1.0, 1.0, this->getTag(), 0);
            }
        } else  {
            int mode = displayMode * -1;
            for (i=0; i<numExternalNodes-1; i++)  {
                const Vector &end1Crd = theNodes[i]->getCrds();
                const Vector &end2Crd = theNodes[i+1]->getCrds();
                
                const Matrix &eigen1 = theNodes[i]->getEigenvectors();
                const Matrix &eigen2 = theNodes[i+1]->getEigenvectors();
                
                int end1NumCrds = end1Crd.Size();
                int end2NumCrds = end2Crd.Size();
                
                if (eigen1.noCols() >= mode)  {
                    for (j=0; j<end1NumCrds; j++)
                        v1(j) = end1Crd(j) + eigen1(j,mode-1)*fact;
                    for (j=0; j<end2NumCrds; j++)
                        v2(j) = end2Crd(j) + eigen2(j,mode-1)*fact;
                } else  {
                    for (j=0; j<end1NumCrds; j++)
                        v1(j) = end1Crd(j);
                    for (j=0; j<end2NumCrds; j++)
                        v2(j) = end2Crd(j);
                }
                
                rValue += theViewer.drawLine(v1, v2, 1.0, 1.0, this->getTag(), 0);
            }
        }
    }
    
    return rValue;
}


void EEGeneric::Print(OPS_Stream &s, int flag)
{
    int i;
    if (flag == 0)  {
        // print everything
        s << "Element: " << this->getTag() << endln;
        s << "  type: EEGeneric" << endln;
        for (i=0; i<numExternalNodes; i++ )
            s << "  Node" << i+1 << ": " << connectedExternalNodes(i);
        s << endln;
        if (theSite != 0)
            s << "  ExperimentalSite: " << theSite->getTag() << endln;
        s << "  addRayleigh: " << addRayleigh;
        s << "  mass matrix: " << mass << endln;
        // determine resisting forces in global system
        s << "  resisting force: " << this->getResistingForce() << endln;
    } else if (flag == 1)  {
        // does nothing
    }
}


Response* EEGeneric::setResponse(const char **argv, int argc,
    OPS_Stream &output)
{
    int i;
    char outputData[10];
    Response *theResponse = 0;
    
    output.tag("ElementOutput");
    output.attr("eleType","EEGeneric");
    output.attr("eleTag",this->getTag());
    for (i=0; i<numExternalNodes; i++ )  {
        sprintf(outputData,"node%d",i+1);
        output.attr(outputData,connectedExternalNodes[i]);
    }
    
    // global forces
    if (strcmp(argv[0],"force") == 0 ||
        strcmp(argv[0],"forces") == 0 ||
        strcmp(argv[0],"globalForce") == 0 ||
        strcmp(argv[0],"globalForces") == 0)
    {
        for (i=0; i<numDOF; i++)  {
            sprintf(outputData,"P%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 1, theVector);
    }
    
    // local forces
    else if (strcmp(argv[0],"localForce") == 0 ||
        strcmp(argv[0],"localForces") == 0)
    {
        for (i=0; i<numDOF; i++)  {
            sprintf(outputData,"p%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 2, theVector);
    }
    
    // basic forces
    else if (strcmp(argv[0],"basicForce") == 0 ||
        strcmp(argv[0],"basicForces") == 0 ||
        strcmp(argv[0],"daqForce") == 0 ||
        strcmp(argv[0],"daqForces") == 0)
    {
        for (i=0; i<numBasicDOF; i++)  {
            sprintf(outputData,"q%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 3, Vector(numBasicDOF));
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
        for (i=0; i<numBasicDOF; i++)  {
            sprintf(outputData,"db%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 4, Vector(numBasicDOF));
    }
    
    // ctrl basic velocities
    else if (strcmp(argv[0],"ctrlVel") == 0 ||
        strcmp(argv[0],"ctrlVelocity") == 0 ||
        strcmp(argv[0],"ctrlVelocities") == 0)
    {
        for (i=0; i<numBasicDOF; i++)  {
            sprintf(outputData,"vb%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 5, Vector(numBasicDOF));
    }
    
    // ctrl basic accelerations
    else if (strcmp(argv[0],"ctrlAccel") == 0 ||
        strcmp(argv[0],"ctrlAcceleration") == 0 ||
        strcmp(argv[0],"ctrlAccelerations") == 0)
    {
        for (i=0; i<numBasicDOF; i++)  {
            sprintf(outputData,"ab%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 6, Vector(numBasicDOF));
    }
    
    // daq basic displacements
    else if (strcmp(argv[0],"daqDisp") == 0 ||
        strcmp(argv[0],"daqDisplacement") == 0 ||
        strcmp(argv[0],"daqDisplacements") == 0)
    {
        for (i=0; i<numBasicDOF; i++)  {
            sprintf(outputData,"dbm%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 7, Vector(numBasicDOF));
    }
    
    // daq basic velocities
    else if (strcmp(argv[0],"daqVel") == 0 ||
        strcmp(argv[0],"daqVelocity") == 0 ||
        strcmp(argv[0],"daqVelocities") == 0)
    {
        for (i=0; i<numBasicDOF; i++)  {
            sprintf(outputData,"vbm%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 8, Vector(numBasicDOF));
    }
    
    // daq basic accelerations
    else if (strcmp(argv[0],"daqAccel") == 0 ||
        strcmp(argv[0],"daqAcceleration") == 0 ||
        strcmp(argv[0],"daqAccelerations") == 0)
    {
        for (i=0; i<numBasicDOF; i++)  {
            sprintf(outputData,"abm%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 9, Vector(numBasicDOF));
    }
    
    output.endTag(); // ElementOutput
    
    return theResponse;
}


int EEGeneric::getResponse(int responseID, Information &eleInfo)
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
