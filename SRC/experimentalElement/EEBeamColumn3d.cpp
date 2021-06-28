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
// Description: This file contains the implementation of the EEBeamColumn3d class.

#include "EEBeamColumn3d.h"

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
Matrix EEBeamColumn3d::theMatrix(12,12);
Vector EEBeamColumn3d::theVector(12);

void* OPF_EEBeamColumn3d()
{
    int ndf = OPS_GetNDF();
    if (ndf != 6) {
        opserr << "WARNING invalid ndf: " << ndf;
        opserr << ", for space problem need 6 - expElement beamColumn\n";
        return 0;
    }
    
    // pointer to experimental element that will be returned
    ExperimentalElement* theExpElement = 0;
    
    if (OPS_GetNumRemainingInputArgs() < 43) {
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
    Matrix theInitStif(6, 6);
    double stif[36];
    numdata = 36;
    if (OPS_GetDoubleInput(&numdata, stif) < 0) {
        opserr << "WARNING invalid initial stiffness term\n";
        opserr << "expElement beamColumn element: " << tag << endln;
        return 0;
    }
    theInitStif.setData(stif, 6, 6);
    
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
    
    // now create the EEBeamColumn3d
    if (theSite != 0) {
        theExpElement = new EEBeamColumn3d(tag, node[0], node[1], *theTrans,
            theSite, iMod, doRayleigh, rho, cMass);
    }
    else {
        theExpElement = new EEBeamColumn3d(tag, node[0], node[1], *theTrans,
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
EEBeamColumn3d::EEBeamColumn3d(int tag, int Nd1, int Nd2,
    CrdTransf &coordTransf,
    ExperimentalSite *site,
    bool iM, int addRay, double r, bool cm)
    : ExperimentalElement(tag, ELE_TAG_EEBeamColumn3d, site),
    connectedExternalNodes(2), theCoordTransf(0),
    iMod(iM), addRayleigh(addRay), rho(r), cMass(cm),
    nlGeo(0), L(0.0), theLoad(12), db(0), vb(0), ab(0), t(0),
    dbDaq(0), vbDaq(0), abDaq(0), qbDaq(0), tDaq(0),
    dbCtrl(6), vbCtrl(6), abCtrl(6),
    kbInit(6,6), dbLast(6), tLast(0.0),
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
    
    // allocate memory for daq response vectors
    dbDaq = new Vector(6);
    vbDaq = new Vector(6);
    abDaq = new Vector(6);
    qbDaq = new Vector(6);
    tDaq  = new Vector(1);
    
    // set the initial stiffness matrix size
    theInitStiff.resize(12,12);
    
    // get a copy of the coordinate transformation
    theCoordTransf = coordTransf.getCopy3d();
    if (!theCoordTransf)  {
        opserr << "EEBeamColumn3d::EEBeamColumn3d() - "
            << "failed to get copy of the coordinate transformation\n";
        exit(-1);
    }
    
    // get coordinate transformation type and save flag
    if (strncmp(theCoordTransf->getClassType(),"Linear",6) == 0)
        nlGeo = 0;
    else if (strncmp(theCoordTransf->getClassType(),"PDelta",6) == 0)
        nlGeo = 1;
    else if (strncmp(theCoordTransf->getClassType(),"Corot",5) == 0)  {
        nlGeo = 0;
        opserr << "\nWARNING EEBeamColumn3d::EEBeamColumn3d()"
            << " - Element: " << this->getTag() << endln
            << "Corotational transformation from basic sys A to B not "
            << "implemented yet. Using Linear transformation instead.\n";
    }
    
    // initialize additional vectors
    dbCtrl.Zero();
    vbCtrl.Zero();
    abCtrl.Zero();
    dbLast.Zero();
    for (i=0; i<6; i++)  {
        qA0[i] = 0.0;
        pA0[i] = 0.0;
    }
}


// responsible for allocating the necessary space needed
// by each object and storing the tags of the end nodes.
EEBeamColumn3d::EEBeamColumn3d(int tag, int Nd1, int Nd2,
    CrdTransf &coordTransf,
    int port, char *machineInetAddr, int ssl, int udp,
    int dataSize, bool iM, int addRay, double r, bool cm)
    : ExperimentalElement(tag, ELE_TAG_EEBeamColumn3d),
    connectedExternalNodes(2), theCoordTransf(0),
    iMod(iM), addRayleigh(addRay), rho(r), cMass(cm),
    nlGeo(0), L(0.0), theLoad(12), theChannel(0),
    sData(0), sendData(0), rData(0), recvData(0),
    db(0), vb(0), ab(0), t(0),
    dbDaq(0), vbDaq(0), abDaq(0), qbDaq(0), tDaq(0),
    dbCtrl(6), vbCtrl(6), abCtrl(6),
    kbInit(6,6), dbLast(6), tLast(0.0),
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
        opserr << "EEBeamColumn3d::EEBeamColumn3d() "
            << "- failed to create channel\n";
        exit(-1);
    }
    if (theChannel->setUpConnection() != 0)  {
        opserr << "EEBeamColumn3d::EEBeamColumn3d() "
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
    dbDaq = new Vector(&rData[id], 6);
    id += 6;
    vbDaq = new Vector(&rData[id], 6);
    id += 6;
    abDaq = new Vector(&rData[id], 6);
    id += 6;
    qbDaq = new Vector(&rData[id], 6);
    id += 6;
    tDaq = new Vector(&rData[id], 1);
    recvData->Zero();
    
    // set the initial stiffness matrix size
    theInitStiff.resize(12,12);
    
    // get a copy of the coordinate transformation
    theCoordTransf = coordTransf.getCopy3d();
    if (!theCoordTransf)  {
        opserr << "EEBeamColumn3d::EEBeamColumn3d() - "
            << "failed to get copy of the coordinate transformation\n";
        exit(-1);
    }
    
    // get coordinate transformation type and save flag
    if (strncmp(theCoordTransf->getClassType(),"Linear",6) == 0)
        nlGeo = 0;
    else if (strncmp(theCoordTransf->getClassType(),"PDelta",6) == 0)
        nlGeo = 1;
    else if (strncmp(theCoordTransf->getClassType(),"Corot",5) == 0)  {
        nlGeo = 0;
        opserr << "\nWARNING EEBeamColumn3d::EEBeamColumn3d()"
            << " - Element: " << this->getTag() << endln
            << "Corotational transformation from basic sys A to B not "
            << "implemented yet. Using Linear transformation instead.\n";
    }
    
    // initialize additional vectors
    dbCtrl.Zero();
    vbCtrl.Zero();
    abCtrl.Zero();
    dbLast.Zero();
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
    L = theCoordTransf->getInitialLength();
    if (L == 0.0)  {
        opserr << "EEBeamColumn3d::setDomain() - element: " 
            << this->getTag() << " has zero length\n";
        return;
    }
}


int EEBeamColumn3d::commitState()
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
    dbLast(0) = dbA(0);
    dbLast(1) = -L*dbA(1);
    dbLast(2) = -dbA(1)+dbA(2);
    dbLast(3) = L*dbA(3);
    dbLast(4) = -dbA(3)+dbA(4);
    dbLast(5) = dbA(5);
    
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
    //const Vector &vbA = theCoordTransf->getBasicTrialVel();    // not implemented yet
    //const Vector &abA = theCoordTransf->getBasicTrialAccel();  // not implemented yet
    Vector vbA(6), abA(6);
    
    if (nlGeo == 0 || nlGeo == 1)  {
        // transform displacements from basic sys A to basic sys B (linear)
        (*db)[0] = dbA(0);
        (*db)[1] = -L*dbA(1);
        (*db)[2] = -dbA(1)+dbA(2);
        (*db)[3] = L*dbA(3);
        (*db)[4] = -dbA(3)+dbA(4);
        (*db)[5] = dbA(5);
        (*vb)[0] = vbA(0);
        (*vb)[1] = -L*vbA(1);
        (*vb)[2] = -vbA(1)+vbA(2);
        (*vb)[3] = L*vbA(3);
        (*vb)[4] = -vbA(3)+vbA(4);
        (*vb)[5] = vbA(5);
        (*ab)[0] = abA(0);
        (*ab)[1] = -L*abA(1);
        (*ab)[2] = -abA(1)+abA(2);
        (*ab)[3] = L*abA(3);
        (*ab)[4] = -abA(3)+abA(4);
        (*ab)[5] = abA(5);
    } else if (nlGeo == 2)  {
        /* transform displacements from basic sys A to basic sys B (nonlinear)
        (*db)[0] = 
        (*db)[1] = 
        (*db)[2] = 
        (*db)[3] = 
        (*db)[4] = 
        (*db)[5] = 
        (*vb)[0] = 
        (*vb)[1] = 
        (*vb)[2] = 
        (*vb)[3] = 
        (*vb)[4] = 
        (*vb)[5] = 
        (*ab)[0] = 
        (*ab)[1] = 
        (*ab)[2] = 
        (*ab)[3] = 
        (*ab)[4] = 
        (*ab)[5] = */
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


int EEBeamColumn3d::setInitialStiff(const Matrix& kbinit)
{
    if (kbinit.noRows() != 6 || kbinit.noCols() != 6)  {
        opserr << "EEBeamColumn3d::setInitialStiff() - " 
            << "matrix size is incorrect for element: "
            << this->getTag() << endln;
        return -1;
    }
    kbInit = kbinit;
    
    // transform stiffness from basic sys B to basic sys A
    static Matrix kbAInit(6,6);
    kbAInit.Zero();
    kbAInit(0,0) = kbInit(0,0);
    kbAInit(1,1) = L*L*kbInit(1,1) + L*(kbInit(1,2)+kbInit(2,1)) + kbInit(2,2);
    kbAInit(1,2) = -L*kbInit(1,2) - kbInit(2,2);
    kbAInit(2,1) = -L*kbInit(2,1) - kbInit(2,2);
    kbAInit(2,2) = kbInit(2,2);
    kbAInit(3,3) = L*L*kbInit(3,3) - L*(kbInit(3,4)+kbInit(4,3)) + kbInit(4,4);
    kbAInit(3,4) = L*kbInit(3,4) - kbInit(4,4);
    kbAInit(4,3) = L*kbInit(4,3) - kbInit(4,4);
    kbAInit(4,4) = kbInit(4,4);
    kbAInit(5,5) = kbInit(5,5);
    
    // transform stiffness from the basic to the global system
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
    
    // get current daq resisting force
    this->getBasicForce();
    
    // apply optional initial stiffness modification
    if (iMod == true)
        this->applyIMod();
    
    // use elastic axial force if axial force from test is zero
    if (fabs((*qbDaq)[0]) < 1.0E-12)
        (*qbDaq)[0] = kbInit(0,0)*(*db)[0];
    // use elastic torsion if torsion from test is zero
    if (fabs((*qbDaq)[5]) < 1.0E-12)
        (*qbDaq)[5] = kbInit(5,5)*(*db)[5];
    
    static Vector qA(6);
    if (nlGeo == 0 || nlGeo == 1)  {
        // transform forces from basic sys B to basic sys A (linear)
        qA(0) = (*qbDaq)[0];
        qA(1) = -L*(*qbDaq)[1] - (*qbDaq)[2];
        qA(2) = (*qbDaq)[2];
        qA(3) = L*(*qbDaq)[3] - (*qbDaq)[4];
        qA(4) = (*qbDaq)[4];
        qA(5) = (*qbDaq)[5];
    } else if (nlGeo == 2)  {
        /* transform forces from basic sys B to basic sys A (nonlinear)
        qA(0) = 
        qA(1) = 
        qA(2) = 
        qA(3) = 
        qA(4) = 
        qA(5) = */
    }
    
    // add initial forces
    for (int i=0; i<6; i++)
        qA(i) += qA0[i];
    
    // transform stiffness from basic sys B to basic sys A
    static Matrix kbAInit(6,6);
    kbAInit.Zero();
    kbAInit(0,0) = kbInit(0,0);
    kbAInit(1,1) = L*L*kbInit(1,1) + L*(kbInit(1,2)+kbInit(2,1)) + kbInit(2,2);
    kbAInit(1,2) = -L*kbInit(1,2) - kbInit(2,2);
    kbAInit(2,1) = -L*kbInit(2,1) - kbInit(2,2);
    kbAInit(2,2) = kbInit(2,2);
    kbAInit(3,3) = L*L*kbInit(3,3) - L*(kbInit(3,4)+kbInit(4,3)) + kbInit(4,4);
    kbAInit(3,4) = L*kbInit(3,4) - kbInit(4,4);
    kbAInit(4,3) = L*kbInit(4,3) - kbInit(4,4);
    kbAInit(4,4) = kbInit(4,4);
    kbAInit(5,5) = kbInit(5,5);
    
    return theCoordTransf->getGlobalStiffMatrix(kbAInit, qA);
}


const Matrix& EEBeamColumn3d::getDamp()
{
    // zero the global matrix
    theMatrix.Zero();
    
    // call base class to setup Rayleigh damping
    if (addRayleigh == 1)
        theMatrix = this->Element::getDamp();
    
    return theMatrix;
}


const Matrix& EEBeamColumn3d::getMass()
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
            theMatrix(2,2) = m;
            theMatrix(6,6) = m;
            theMatrix(7,7) = m;
            theMatrix(8,8) = m;
        } else  {
            // consistent mass matrix
            static Matrix ml(12,12);
            double m = rho*L/420.0;
            ml(0,0) = ml(6,6) = m*140.0;
            ml(0,6) = ml(6,0) = m*70.0;
            //ml(3,3) = ml(9,9) = m*(Jx/A)*140.0;  // CURRENTLY NO TORSIONAL MASS 
            //ml(3,9) = ml(9,3) = m*(Jx/A)*70.0;   // CURRENTLY NO TORSIONAL MASS 
            
            ml(2,2) = ml(8,8) = m*156.0;
            ml(2,8) = ml(8,2) = m*54.0;
            ml(4,4) = ml(10,10) = m*4.0*L*L;
            ml(4,10) = ml(10,4) = -m*3.0*L*L;
            ml(2,4) = ml(4,2) = -m*22.0*L;
            ml(8,10) = ml(10,8) = -ml(2,4);
            ml(2,10) = ml(10,2) = m*13.0*L;
            ml(4,8) = ml(8,4) = -ml(2,10);
            
            ml(1,1) = ml(7,7) = m*156.0;
            ml(1,7) = ml(7,1) = m*54.0;
            ml(5,5) = ml(11,11) = m*4.0*L*L;
            ml(5,11) = ml(11,5) = -m*3.0*L*L;
            ml(1,5) = ml(5,1) = m*22.0*L;
            ml(7,11) = ml(11,7) = -ml(1,5);
            ml(1,11) = ml(11,1) = -m*13.0*L;
            ml(5,7) = ml(7,5) = -ml(1,11);
            
            // transform local mass matrix to global system
            theMatrix = theCoordTransf->getGlobalMatrixFromLocal(ml);
        }
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
        double wy = data(0)*loadFactor;  // transverse
        double wz = data(1)*loadFactor;  // transverse
        double wx = data(2)*loadFactor;  // axial (+ve from node I to J)
        
        double Vy = 0.5*wy*L;
        double Mz = Vy*L/6.0; // wy*L*L/12
        double Vz = 0.5*wz*L;
        double My = Vz*L/6.0; // wz*L*L/12
        double P  = wx*L;
        
        // reactions in basic system
        pA0[0] -= P;
        pA0[1] -= Vy;
        pA0[2] -= Vy;
        pA0[3] -= Vz;
        pA0[4] -= Vz;
        
        // fixed end forces in basic system
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
        
        // reactions in basic system
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
        
        // fixed end forces in basic system
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
    if (L == 0.0 || rho == 0.0)
        return 0;
    
    // get R * accel from the nodes
    const Vector &Raccel1 = theNodes[0]->getRV(accel);
    const Vector &Raccel2 = theNodes[1]->getRV(accel);
    
    if (6 != Raccel1.Size() || 6 != Raccel2.Size())  {
        opserr << "EEBeamColumn3d::addInertiaLoadToUnbalance() - "
            << "matrix and vector sizes are incompatible\n";
        return -1;
    }
    
    // want to add ( - fact * M R * accel ) to unbalance
    if (cMass == false)  {
        // take advantage of lumped mass matrix
        double m = 0.5*rho*L;
        theLoad(0) -= m * Raccel1(0);
        theLoad(1) -= m * Raccel1(1);
        theLoad(2) -= m * Raccel1(2);
        theLoad(6) -= m * Raccel2(0);
        theLoad(7) -= m * Raccel2(1);
        theLoad(8) -= m * Raccel2(2);
    } else  {
        // use matrix vector multip. for consistent mass matrix
        static Vector Raccel(12);
        for (int i=0; i<6; i++)  {
            Raccel(i)   = Raccel1(i);
            Raccel(i+6) = Raccel2(i);
        }
        theLoad.addMatrixVector(1.0, this->getMass(), Raccel, -1.0);
    }
    
    return 0;
}


const Vector& EEBeamColumn3d::getResistingForce()
{
    // make sure the coordinate transformation is updated
    theCoordTransf->update();
    
    // zero the global residual
    theVector.Zero();
    
    // get current daq resisting force
    this->getBasicForce();
    
    // apply optional initial stiffness modification
    if (iMod == true)
        this->applyIMod();
    
    // use elastic axial force if axial force from test is zero
    if (fabs((*qbDaq)[0]) < 1.0E-12)
        (*qbDaq)[0] = kbInit(0,0)*(*db)[0];
    // use elastic torsion if torsion from test is zero
    if (fabs((*qbDaq)[5]) < 1.0E-12)
        (*qbDaq)[5] = kbInit(5,5)*(*db)[5];
    
    // save corresponding ctrl displacements for recorder
    dbCtrl = (*db);
    vbCtrl = (*vb);
    abCtrl = (*ab);
    
    static Vector qA(6);
    if (nlGeo == 0 || nlGeo == 1)  {
        // transform forces from basic sys B to basic sys A (linear)
        qA(0) = (*qbDaq)[0];
        qA(1) = -L*(*qbDaq)[1] - (*qbDaq)[2];
        qA(2) = (*qbDaq)[2];
        qA(3) = L*(*qbDaq)[3] - (*qbDaq)[4];
        qA(4) = (*qbDaq)[4];
        qA(5) = (*qbDaq)[5];
    } else if (nlGeo == 2)  {
    /* transform forces from basic sys B to basic sys A (nonlinear)
        static Vector qA(6);
        qA(0) = 
        qA(1) = 
        qA(2) = 
        qA(3) = 
        qA(4) = 
        qA(5) = */
    }
    
    // add initial forces
    for (int i=0; i<6; i++)
        qA(i) += qA0[i];
    
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
        theVector(2) += m * accel1(2);
        theVector(6) += m * accel2(0);
        theVector(7) += m * accel2(1);
        theVector(8) += m * accel2(2);
    } else  {
        // use matrix vector multip. for consistent mass matrix
        static Vector accel(12);
        for (int i=0; i<6; i++)  {
            accel(i)   = accel1(i);
            accel(i+6) = accel2(i);
        }
        theVector.addMatrixVector(1.0, this->getMass(), accel, 1.0);
    }
    
    return theVector;
}


const Vector& EEBeamColumn3d::getTime()
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


const Vector& EEBeamColumn3d::getBasicDisp()
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


const Vector& EEBeamColumn3d::getBasicVel()
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


const Vector& EEBeamColumn3d::getBasicAccel()
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


const Vector& EEBeamColumn3d::getBasicForce()
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
        
        for (int i=0; i<3; i++)  {
            v1(i) = end1Crd(i) + end1Disp(i)*fact;
            v2(i) = end2Crd(i) + end2Disp(i)*fact;
        }
    } else  {
        int mode = displayMode * -1;
        const Matrix &eigen1 = theNodes[0]->getEigenvectors();
        const Matrix &eigen2 = theNodes[1]->getEigenvectors();
        
        if (eigen1.noCols() >= mode)  {
            for (int i=0; i<3; i++)  {
                v1(i) = end1Crd(i) + eigen1(i,mode-1)*fact;
                v2(i) = end2Crd(i) + eigen2(i,mode-1)*fact;
            }
        } else  {
            for (int i=0; i<3; i++)  {
                v1(i) = end1Crd(i);
                v2(i) = end2Crd(i);
            }
        }
    }
    
    return theViewer.drawLine (v1, v2, 1.0, 1.0, this->getTag(), 0);
}


void EEBeamColumn3d::Print(OPS_Stream &s, int flag)
{
    if (flag == 0)  {
        // print everything
        s << "Element: " << this->getTag() << endln;
        s << "  type: EEBeamColumn3d" << endln;
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
    if (strcmp(argv[0],"force") == 0 ||
        strcmp(argv[0],"forces") == 0 ||
        strcmp(argv[0],"globalForce") == 0 ||
        strcmp(argv[0],"globalForces") == 0)
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
        
        theResponse = new ElementResponse(this, 1, theVector);
    }
    
    // local forces
    else if (strcmp(argv[0],"localForce") == 0 ||
        strcmp(argv[0],"localForces") == 0)
    {
        output.tag("ResponseType","N_ 1");
        output.tag("ResponseType","Vy_1");
        output.tag("ResponseType","Vz_1");
        output.tag("ResponseType","T_1");
        output.tag("ResponseType","My_1");
        output.tag("ResponseType","Mz_1");
        output.tag("ResponseType","N_2");
        output.tag("ResponseType","Vy_2");
        output.tag("ResponseType","Vz_2");
        output.tag("ResponseType","T_2");
        output.tag("ResponseType","My_2");
        output.tag("ResponseType","Mz_2");
        
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
        output.tag("ResponseType","qb4");
        output.tag("ResponseType","qb5");
        output.tag("ResponseType","qb6");
        
        theResponse = new ElementResponse(this, 3, Vector(6));
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
        output.tag("ResponseType","db4");
        output.tag("ResponseType","db5");
        output.tag("ResponseType","db6");
        
        theResponse = new ElementResponse(this, 4, Vector(6));
    }
    
    // ctrl velocities in basic system B
    else if (strcmp(argv[0],"ctrlVel") == 0 ||
        strcmp(argv[0],"ctrlVelocity") == 0 ||
        strcmp(argv[0],"ctrlVelocities") == 0)
    {
        output.tag("ResponseType","vb1");
        output.tag("ResponseType","vb2");
        output.tag("ResponseType","vb3");
        output.tag("ResponseType","vb4");
        output.tag("ResponseType","vb5");
        output.tag("ResponseType","vb6");
        
        theResponse = new ElementResponse(this, 5, Vector(6));
    }
    
    // ctrl accelerations in basic system B
    else if (strcmp(argv[0],"ctrlAccel") == 0 ||
        strcmp(argv[0],"ctrlAcceleration") == 0 ||
        strcmp(argv[0],"ctrlAccelerations") == 0)
    {
        output.tag("ResponseType","ab1");
        output.tag("ResponseType","ab2");
        output.tag("ResponseType","ab3");
        output.tag("ResponseType","ab4");
        output.tag("ResponseType","ab5");
        output.tag("ResponseType","ab6");
        
        theResponse = new ElementResponse(this, 6, Vector(6));
    }
    
    // daq displacements in basic system B
    else if (strcmp(argv[0],"daqDisp") == 0 ||
        strcmp(argv[0],"daqDisplacement") == 0 ||
        strcmp(argv[0],"daqDisplacements") == 0)
    {
        output.tag("ResponseType","dbDaq1");
        output.tag("ResponseType","dbDaq2");
        output.tag("ResponseType","dbDaq3");
        output.tag("ResponseType","dbDaq4");
        output.tag("ResponseType","dbDaq5");
        output.tag("ResponseType","dbDaq6");
        
        theResponse = new ElementResponse(this, 7, Vector(6));
    }
    
    // daq velocities in basic system B
    else if (strcmp(argv[0],"daqVel") == 0 ||
        strcmp(argv[0],"daqVelocity") == 0 ||
        strcmp(argv[0],"daqVelocities") == 0)
    {
        output.tag("ResponseType","vbDaq1");
        output.tag("ResponseType","vbDaq2");
        output.tag("ResponseType","vbDaq3");
        output.tag("ResponseType","vbDaq4");
        output.tag("ResponseType","vbDaq5");
        output.tag("ResponseType","vbDaq6");
        
        theResponse = new ElementResponse(this, 8, Vector(6));
    }
    
    // daq accelerations in basic system B
    else if (strcmp(argv[0],"daqAccel") == 0 ||
        strcmp(argv[0],"daqAcceleration") == 0 ||
        strcmp(argv[0],"daqAccelerations") == 0)
    {
        output.tag("ResponseType","abDaq1");
        output.tag("ResponseType","abDaq2");
        output.tag("ResponseType","abDaq3");
        output.tag("ResponseType","abDaq4");
        output.tag("ResponseType","abDaq5");
        output.tag("ResponseType","abDaq6");
        
        theResponse = new ElementResponse(this, 9, Vector(6));
    }
    
    output.endTag(); // ElementOutput
    
    return theResponse;
}


int EEBeamColumn3d::getResponse(int responseID, Information &eleInfo)
{
    double L = theCoordTransf->getInitialLength();
    static Vector qA(6);
    
    switch (responseID)  {
    case 1:  // global forces
        return eleInfo.setVector(this->getResistingForce());
        
    case 2:  // local forces
        // transform forces from basic sys B to basic sys A (linear)
        qA(0) = (*qbDaq)[0];
        qA(1) = -L*(*qbDaq)[1] - (*qbDaq)[2];
        qA(2) = (*qbDaq)[2];
        qA(3) = L*(*qbDaq)[3] - (*qbDaq)[4];
        qA(4) = (*qbDaq)[4];
        qA(5) = (*qbDaq)[5];
        
        /* transform forces from basic sys B to basic sys A (nonlinear)
        qA(0) = 
        qA(1) = 
        qA(2) = 
        qA(3) = 
        qA(4) = 
        qA(5) = */
        
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


void EEBeamColumn3d::applyIMod()
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
    if ((*dbDaq)[3] != 0.0)  {
        (*qbDaq)[3] -= kbInit(3,3)*((*dbDaq)[3] - (*db)[3]);
        (*qbDaq)[4] -= kbInit(4,3)*((*dbDaq)[3] - (*db)[3]);
    }
    if ((*dbDaq)[4] != 0.0)  {
        (*qbDaq)[3] -= kbInit(3,4)*((*dbDaq)[4] - (*db)[4]);
        (*qbDaq)[4] -= kbInit(4,4)*((*dbDaq)[4] - (*db)[4]);
    }
    if ((*dbDaq)[5] != 0.0)  {
        (*qbDaq)[5] -= kbInit(5,5)*((*dbDaq)[5] - (*db)[5]);
    }
}
