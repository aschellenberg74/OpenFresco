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
#include <UDP_Socket.h>
#include <elementAPI.h>

#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>


// initialize the class wide variables
Matrix EETwoNodeLink::EETwoNodeLinkM2(2,2);
Matrix EETwoNodeLink::EETwoNodeLinkM4(4,4);
Matrix EETwoNodeLink::EETwoNodeLinkM6(6,6);
Matrix EETwoNodeLink::EETwoNodeLinkM12(12,12);
Vector EETwoNodeLink::EETwoNodeLinkV2(2);
Vector EETwoNodeLink::EETwoNodeLinkV4(4);
Vector EETwoNodeLink::EETwoNodeLinkV6(6);
Vector EETwoNodeLink::EETwoNodeLinkV12(12);

void* OPF_EETwoNodeLink()
{
    // pointer to experimental element that will be returned
    ExperimentalElement* theExpElement = 0;
    int ndm = OPS_GetNDM();
    
    if (OPS_GetNumRemainingInputArgs() < 9) {
        opserr << "WARNING invalid number of arguments\n";
        opserr << "Want: expElement twoNodeLink eleTag iNode jNode -dof dofs -site siteTag -initStif Kij <-tangStif tangStifTag> <-orient <x1 x2 x3> y1 y2 y3> <-pDelta Mratios> <-shearDist sDratios> <-iMod> <-noRayleigh> <-mass m>\n";
        opserr << "  or: expElement twoNodeLink eleTag iNode jNode -dof dofs -server ipPort <ipAddr> <-ssl> <-udp> <-dataSize size> -initStif Kij <-tangStif tangStifTag> <-orient <x1 x2 x3> y1 y2 y3> <-pDelta Mratios> <-shearDist sDratios> <-iMod> <-noRayleigh> <-mass m>\n";
        return 0;
    }
    
    // element tag
    int tag;
    int numdata = 1;
    if (OPS_GetIntInput(&numdata, &tag) != 0) {
        opserr << "WARNING invalid expElement twoNodeLink eleTag\n";
        return 0;
    }
    
    // nodes
    int node[2];
    numdata = 2;
    if (OPS_GetIntInput(&numdata, node) != 0) {
        opserr << "WARNING invalid iNode or jNode\n";
        opserr << "expElement twoNodeLink element: " << tag << endln;
        return 0;
    }
    
    // dof IDs
    const char* type = OPS_GetString();
    if (strcmp(type, "-dof") != 0 && strcmp(type, "-dir") != 0) {
        opserr << "WARNING expecting -dof dofs\n";
        opserr << "expElement twoNodeLink element: " << tag << endln;
        return 0;
    }
    ID theDOF(32);
    int numDOF = 0;
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
        theDOF(numDOF++) = dof-1;
    }
    if (numDOF == 0) {
        opserr << "WARNING no directions/dofs specified\n";
        opserr << "expElement twoNodeLink element: " << tag << endln;
        return 0;
    }
    theDOF.resize(numDOF);
    
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
            opserr << "expElement twoNodeLink element: " << tag << endln;
            return 0;
        }
        theSite = OPF_getExperimentalSite(siteTag);
        if (theSite == 0) {
            opserr << "WARNING experimental site not found\n";
            opserr << "expSite: " << siteTag << endln;
            opserr << "expElement twoNodeLink element: " << tag << endln;
            return 0;
        }
    }
    else if (strcmp(type, "-server") == 0) {
        // ip port
        numdata = 1;
        if (OPS_GetIntInput(&numdata, &ipPort) < 0) {
            opserr << "WARNING invalid ipPort\n";
            opserr << "expElement twoNodeLink element: " << tag << endln;
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
                    opserr << "expElement twoNodeLink element: " << tag << endln;
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
        opserr << "expElement twoNodeLink element: " << tag << endln;
        return 0;
    }
    
    // initial stiffness
    type = OPS_GetString();
    if (strcmp(type, "-initStif") != 0 &&
        strcmp(type, "-initStiff") != 0) {
        opserr << "WARNING expecting -initStif\n";
        opserr << "expElement twoNodeLink element: " << tag << endln;
    }
    Matrix theInitStif(numDOF, numDOF);
    double stif[36];
    numdata = numDOF*numDOF;
    if (OPS_GetDoubleInput(&numdata, stif) < 0) {
        opserr << "WARNING invalid initial stiffness term\n";
        opserr << "expElement twoNodeLink element: " << tag << endln;
        return 0;
    }
    theInitStif.setData(stif, numDOF, numDOF);
    
    // optional parameters
    ExperimentalTangentStiff* theTangStif = 0;
    Vector x, y, Mratio, sDistI;
    bool iMod = false;
    int doRayleigh = 1;
    double mass = 0.0;
    while (OPS_GetNumRemainingInputArgs() > 0) {
        type = OPS_GetString();
        // tangent stiffness
        if (strcmp(type, "-tangStif") == 0 ||
            strcmp(type, "-tangStiff") == 0) {
            int tangStifTag;
            numdata = 1;
            if (OPS_GetIntInput(&numdata, &tangStifTag) < 0) {
                opserr << "WARNING invalid tangStifTag\n";
                opserr << "expElement twoNodeLink element: " << tag << endln;
                return 0;
            }
            theTangStif = OPF_getExperimentalTangentStiff(tangStifTag);
            if (theTangStif == 0) {
                opserr << "WARNING experimental tangent stiff not found\n";
                opserr << "expTangStiff: " << tangStifTag << endln;
                opserr << "expElement twoNodeLink element: " << tag << endln;
                return 0;
            }
        }
        else if (strcmp(type, "-orient") == 0) {
            if (OPS_GetNumRemainingInputArgs() < 3) {
                opserr << "WARNING: insufficient arguments after -orient\n";
                opserr << "expElement twoNodeLink element: " << tag << endln;
                return 0;
            }
            x.resize(3);
            numdata = 3;
            if (OPS_GetDoubleInput(&numdata, &x(0)) < 0) {
                opserr << "WARNING: invalid -orient values\n";
                opserr << "expElement twoNodeLink element: " << tag << endln;
                return 0;
            }
            if (OPS_GetNumRemainingInputArgs() < 3) {
                y = x;
                x = Vector();
                continue;
            }
            y.resize(3);
            numdata = 3;
            if (OPS_GetDoubleInput(&numdata, &y(0)) < 0) {
                y = x;
                x = Vector();
                continue;
            }
        }
        else if (strcmp(type, "-pDelta") == 0) {
            Mratio.resize(4);
            Mratio.Zero();
            numdata = 4;
            double* ptr = &Mratio(0);
            if (ndm == 2) {
                numdata = 2;
                ptr += 2;
            }
            if (OPS_GetNumRemainingInputArgs() < numdata) {
                opserr << "WARNING: insufficient data for -pDelta\n";
                opserr << "expElement twoNodeLink element: " << tag << endln;
                return 0;
            }
            if (OPS_GetDoubleInput(&numdata, ptr) < 0) {
                opserr << "WARNING: invalid -pDelta values\n";
                opserr << "expElement twoNodeLink element: " << tag << endln;
                return 0;
            }
        }
        else if (strcmp(type, "-shearDist") == 0) {
            sDistI.resize(2);
            numdata = 2;
            if (ndm == 2) {
                numdata = 1;
                sDistI(1) = 0.5;
            }
            if (OPS_GetNumRemainingInputArgs() < numdata) {
                opserr << "WARNING: insufficient data for -shearDist\n";
                opserr << "expElement twoNodeLink element: " << tag << endln;
                return 0;
            }
            if (OPS_GetDoubleInput(&numdata, &sDistI(0)) < 0) {
                opserr << "WARNING: invalid -shearDist values\n";
                opserr << "expElement twoNodeLink element: " << tag << endln;
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
            if (OPS_GetNumRemainingInputArgs() < 1) {
                opserr << "WARNING: insufficient mass value\n";
                opserr << "expElement twoNodeLink element: " << tag << endln;
                return 0;
            }
            numdata = 1;
            if (OPS_GetDoubleInput(&numdata, &mass) < 0) {
                opserr << "WARNING: invalid -mass value\n";
                opserr << "expElement twoNodeLink element: " << tag << endln;
                return 0;
            }
        }
    }
    
    // now create the EETwoNodeLink
    if (theSite != 0) {
        theExpElement = new EETwoNodeLink(tag, ndm, node[0], node[1], theDOF,
            theSite, theTangStif, y, x, Mratio, sDistI, iMod, doRayleigh, mass);
    }
    else {
        theExpElement = new EETwoNodeLink(tag, ndm, node[0], node[1], theDOF,
            ipPort, ipAddr, ssl, udp, dataSize, theTangStif, y, x, Mratio,
            sDistI, iMod, doRayleigh, mass);
    }
    if (theExpElement == 0) {
        opserr << "WARNING ran out of memory creating element\n";
        opserr << "expElement twoNodeLink element: " << tag << endln;
        return 0;
    }
    
    // add initial stiffness
    int setInitStif = theExpElement->setInitialStiff(theInitStif);
    if (setInitStif != 0) {
        opserr << "WARNING initial stiffness not set\n";
        opserr << "expElement twoNodeLink element: " << tag << endln;
        return 0;
    }
    
    return theExpElement;
}


// responsible for allocating the necessary space needed
// by each object and storing the tags of the end nodes.
EETwoNodeLink::EETwoNodeLink(int tag, int dim, int Nd1, int Nd2,
    const ID &direction, ExperimentalSite *site,
    ExperimentalTangentStiff *tang, const Vector _y, const Vector _x,
    const Vector Mr, const Vector sdI, bool iM, int addRay, double m)
    : ExperimentalElement(tag, ELE_TAG_EETwoNodeLink, site, tang),
    dimension(dim), numDOF(0), connectedExternalNodes(2),
    numDir(direction.Size()), dir(0), trans(3,3), x(_x), y(_y),
    Mratio(Mr), shearDistI(sdI), iMod(iM), addRayleigh(addRay), mass(m),
    L(0.0), db(0), vb(0), ab(0), t(0),
    dbDaq(0), vbDaq(0), abDaq(0), qbDaq(0), tDaq(0),
    dbCtrl(numDir), vbCtrl(numDir), abCtrl(numDir),
    dl(0), Tgl(0,0), Tlb(0,0),
    kb(numDir,numDir), kbInit(numDir,numDir), kbLast(numDir,numDir),
    dbLast(numDir), dbDaqLast(numDir), qbDaqLast(numDir), tLast(0.0),
    theMatrix(0), theVector(0), theLoad(0),
    firstWarning(true)
{
    // ensure the connectedExternalNode ID is of correct size & set values
    if (connectedExternalNodes.Size() != 2)  {
        opserr << "EETwoNodeLink::EETwoNodeLink() - element: "
            << this->getTag() << " failed to create an ID of size 2\n";
        exit(-1);
    }
    
    connectedExternalNodes(0) = Nd1;
    connectedExternalNodes(1) = Nd2;
    
    // set node pointers to NULL
    for (int i=0; i<2; i++)
        theNodes[i] = 0;
    
    // check the number of directions
    if (numDir < 1 || numDir > 6)  {
        opserr << "EETwoNodeLink::EETwoNodeLink() - element: "
            << this->getTag() << " wrong number of directions\n";
        exit(-1);
    }
    
    // allocate memory for direction array
    dir = new ID(numDir);
    if (dir == 0)  {
        opserr << "EETwoNodeLink::EETwoNodeLink() - "
            << "failed to creat direction array\n";
        exit(-1);
    }
    
    // initialize directions and check for valid values
    (*dir) = direction;
    for (int i=0; i<numDir; i++)  {
        if ((*dir)(i) < 0 ||
            (dimension == 1 && (*dir)(i) > 0) ||
            (dimension == 2 && (*dir)(i) > 2) ||
            (dimension == 3 && (*dir)(i) > 5))  {
            opserr << "EETwoNodeLink::EETwoNodeLink() - "
                << "incorrect direction " << (*dir)(i)
                << " is set to 0\n";
            (*dir)(i) = 0;
        }
    }
    
    // check p-delta moment distribution ratios
    if (Mratio.Size() == 4)  {
        if (Mratio(0)+Mratio(1) > 1.0)  {
            opserr << "EETwoNodeLink::EETwoNodeLink() - "
                << "incorrect p-delta moment ratios:\nrMy1 + rMy2 = "
                << Mratio(0)+Mratio(1) << " > 1.0\n";
            exit(-1);
        }
        if (Mratio(2)+Mratio(3) > 1.0)  {
            opserr << "EETwoNodeLink::EETwoNodeLink() - "
                << "incorrect p-delta moment ratios:\nrMz1 + rMz2 = "
                << Mratio(2)+Mratio(3) << " > 1.0\n";
            exit(-1);
        }
    }
    
    // check or initialize shear distance ratios
    if (shearDistI.Size() == 2)  {
        if (shearDistI(0) < 0.0 || shearDistI(0) > 1.0)  {
            opserr << "EETwoNodeLink::EETwoNodeLink() - "
                << "incorrect shear distance ratio:\n shearDistIy = "
                << shearDistI(0) << " < 0.0 or > 1.0\n";
            exit(-1);
        }
        if (shearDistI(1) < 0.0 || shearDistI(1) > 1.0)  {
            opserr << "EETwoNodeLink::EETwoNodeLink() - "
                << "incorrect shear distance ratio:\n shearDistIz = "
                << shearDistI(1) << " < 0.0 or > 1.0\n";
            exit(-1);
        }
    } else  {
        shearDistI.resize(2);
        shearDistI(0) = 0.5;
        shearDistI(1) = 0.5;
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
    
    // allocate memory for daq response vectors
    dbDaq = new Vector(numDir);
    vbDaq = new Vector(numDir);
    abDaq = new Vector(numDir);
    qbDaq = new Vector(numDir);
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
EETwoNodeLink::EETwoNodeLink(int tag, int dim, int Nd1, int Nd2,
    const ID &direction, int port, char *machineInetAddr,
    int ssl, int udp, int dataSize, ExperimentalTangentStiff *tang,
    const Vector _y, const Vector _x, const Vector Mr,
    const Vector sdI, bool iM, int addRay, double m)
    : ExperimentalElement(tag, ELE_TAG_EETwoNodeLink, NULL, tang),
    dimension(dim), numDOF(0), connectedExternalNodes(2),
    numDir(direction.Size()), dir(0), trans(3,3), x(_x), y(_y),
    Mratio(Mr), shearDistI(sdI), iMod(iM),
    addRayleigh(addRay), mass(m), L(0.0),
    theChannel(0), sData(0), sendData(0), rData(0), recvData(0),
    db(0), vb(0), ab(0), t(0),
    dbDaq(0), vbDaq(0), abDaq(0), qbDaq(0), tDaq(0),
    dbCtrl(numDir), vbCtrl(numDir), abCtrl(numDir),
    dl(0), Tgl(0,0), Tlb(0,0),
    kb(numDir,numDir), kbInit(numDir,numDir), kbLast(numDir,numDir),
    dbLast(numDir), dbDaqLast(numDir), qbDaqLast(numDir), tLast(0.0),
    theMatrix(0), theVector(0), theLoad(0),
    firstWarning(true)
{
    // ensure the connectedExternalNode ID is of correct size & set values
    if (connectedExternalNodes.Size() != 2)  {
        opserr << "EETwoNodeLink::EETwoNodeLink() - element: "
            << this->getTag() << " failed to create an ID of size 2\n";
        exit(-1);
    }
    
    connectedExternalNodes(0) = Nd1;
    connectedExternalNodes(1) = Nd2;
    
    // set node pointers to NULL
    for (int i=0; i<2; i++)
        theNodes[i] = 0;
    
    // check the number of directions
    if (numDir < 1 || numDir > 6)  {
        opserr << "EETwoNodeLink::EETwoNodeLink() - element: "
            << this->getTag() << " wrong number of directions\n";
        exit(-1);
    }
    
    // allocate memory for direction array
    dir = new ID(numDir);
    if (dir == 0)  {
        opserr << "EETwoNodeLink::EETwoNodeLink() - "
            << "failed to creat direction array\n";
        exit(-1);
    }
    
    // initialize directions and check for valid values
    (*dir) = direction;
    for (int i=0; i<numDir; i++)  {
        if ((*dir)(i) < 0 ||
            (dimension == 1 && (*dir)(i) > 0) ||
            (dimension == 2 && (*dir)(i) > 2) ||
            (dimension == 3 && (*dir)(i) > 5))  {
            opserr << "EETwoNodeLink::EETwoNodeLink() - "
                << "incorrect direction " << (*dir)(i)
                << " is set to 0\n";
            (*dir)(i) = 0;
        }
    }
    
    // check p-delta moment distribution ratios
    if (Mratio.Size() == 4)  {
        if (Mratio(0)+Mratio(1) > 1.0)  {
            opserr << "EETwoNodeLink::EETwoNodeLink() - "
                << "incorrect p-delta moment ratios:\nrMy1 + rMy2 = "
                << Mratio(0)+Mratio(1) << " > 1.0\n";
            exit(-1);
        }
        if (Mratio(2)+Mratio(3) > 1.0)  {
            opserr << "EETwoNodeLink::EETwoNodeLink() - "
                << "incorrect p-delta moment ratios:\nrMz1 + rMz2 = "
                << Mratio(2)+Mratio(3) << " > 1.0\n";
            exit(-1);
        }
    }
    
    // check or initialize shear distance ratios
    if (shearDistI.Size() == 2)  {
        if (shearDistI(0) < 0.0 || shearDistI(0) > 1.0)  {
            opserr << "EETwoNodeLink::EETwoNodeLink() - "
                << "incorrect shear distance ratio:\n shearDistIy = "
                << shearDistI(0) << " < 0.0 or > 1.0\n";
            exit(-1);
        }
        if (shearDistI(1) < 0.0 || shearDistI(1) > 1.0)  {
            opserr << "EETwoNodeLink::EETwoNodeLink() - "
                << "incorrect shear distance ratio:\n shearDistIz = "
                << shearDistI(1) << " < 0.0 or > 1.0\n";
            exit(-1);
        }
    } else  {
        shearDistI.resize(2);
        shearDistI(0) = 0.5;
        shearDistI(1) = 0.5;
    }
    
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
        opserr << "EETwoNodeLink::EETwoNodeLink() "
            << "- failed to create channel\n";
        exit(-1);
    }
    if (theChannel->setUpConnection() != 0)  {
        opserr << "EETwoNodeLink::EETwoNodeLink() "
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
    dbDaq = new Vector(&rData[id], numDir);
    id += numDir;
    vbDaq = new Vector(&rData[id], numDir);
    id += numDir;
    abDaq = new Vector(&rData[id], numDir);
    id += numDir;
    qbDaq = new Vector(&rData[id], numDir);
    id += numDir;
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
    theInitStiff.resize(numDOF, numDOF);
    theInitStiff.Zero();
    
    // set the local displacement vector size
    dl.resize(numDOF);
    dl.Zero();
    
    // allocate memory for the load vector
    if (theLoad == 0)
        theLoad = new Vector(numDOF);
    else if (theLoad->Size() != numDOF)  {
        delete theLoad;
        theLoad = new Vector(numDOF);
    }
    if (theLoad == 0)  {
        opserr << "EETwoNodeLink::setDomain() - element: " << this->getTag()
            << " out of memory creating vector of size: " << numDOF << endln;
        return;
    }
    
    // setup the transformation matrix for orientation
    this->setUp();
    
    // set transformation matrix from global to local system
    this->setTranGlobalLocal();
    
    // set transformation matrix from local to basic system
    this->setTranLocalBasic();
}


int EETwoNodeLink::commitState()
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
    Vector dgLast(numDOF), dlLast(numDOF);
    for (i=0; i<2; i++)  {
        Vector disp = theNodes[i]->getTrialDisp();
        dgLast.Assemble(disp, ndim);
        ndim += numDOF/2;
    }
    dlLast.addMatrixVector(0.0, Tgl, dgLast, 1.0);
    dbLast.addMatrixVector(0.0, Tlb, dlLast, 1.0);
    
    return rValue;
}


int EETwoNodeLink::update()
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
    
    // get global trial response
    int ndim = 0, i;
    Vector dg(numDOF), vg(numDOF), ag(numDOF);
    for (i=0; i<2; i++)  {
        Vector disp = theNodes[i]->getTrialDisp();
        Vector vel = theNodes[i]->getTrialVel();
        Vector accel = theNodes[i]->getTrialAccel();
        dg.Assemble(disp, ndim);
        vg.Assemble(vel, ndim);
        ag.Assemble(accel, ndim);
        ndim += numDOF/2;
    }
    
    // transform response from the global to the local system
    Vector vl(numDOF), al(numDOF);
    dl.addMatrixVector(0.0, Tgl, dg, 1.0);
    vl.addMatrixVector(0.0, Tgl, vg, 1.0);
    al.addMatrixVector(0.0, Tgl, ag, 1.0);
    
    // transform response from the local to the basic system
    db->addMatrixVector(0.0, Tlb, dl, 1.0);
    vb->addMatrixVector(0.0, Tlb, vl, 1.0);
    ab->addMatrixVector(0.0, Tlb, al, 1.0);
    
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


int EETwoNodeLink::setInitialStiff(const Matrix& kbinit)
{
    if (kbinit.noRows() != numDir || kbinit.noCols() != numDir)  {
        opserr << "EETwoNodeLink::setInitialStiff() - " 
            << "matrix size is incorrect for element: "
            << this->getTag() << endln;
        return -1;
    }
    kb = kbInit = kbLast = kbinit;
    
    // zero the global matrix
    theInitStiff.Zero();
    
    // transform from basic to local system
    Matrix klInit(numDOF, numDOF);
    klInit.addMatrixTripleProduct(0.0, Tlb, kbInit, 1.0);
    
    // transform from local to global system
    theInitStiff.addMatrixTripleProduct(0.0, Tgl, klInit, 1.0);
    
    return 0;
}


const Matrix& EETwoNodeLink::getTangentStiff()
{
    // zero the global matrix
    theMatrix->Zero();
    
    if (theTangStiff != 0)  {
        // get current daq displacement and resisting force
        this->getBasicDisp();
        this->getBasicForce();
        
        // calculate incremental displacement and force vectors
        Vector dbDaqIncr = (*dbDaq) - dbDaqLast;
        Vector qbDaqIncr = (*qbDaq) - qbDaqLast;
        
        // get updated kb matrix
        kb = theTangStiff->updateTangentStiff(&dbDaqIncr, (Vector*)0,
            (Vector*)0, &qbDaqIncr, (Vector*)0, &kbInit, &kbLast);
        
        // apply optional initial stiffness modification
        if (iMod == true)  {
            // correct for displacement control errors using I-Modification
            qbDaq->addMatrixVector(1.0, kbInit, (*dbDaq) - (*db), -1.0);
        }
    }
    else  {
        if (firstWarning == true)  {
            opserr << "\nWARNING EETwoNodeLink::getTangentStiff() - "
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
        if (iMod == true)  {
            // get daq displacement
            this->getBasicDisp();
            
            // correct for displacement control errors using I-Modification
            qbDaq->addMatrixVector(1.0, kbInit, (*dbDaq) - (*db), -1.0);
        }
    }
    
    // use elastic force if force from test is zero
    for (int i=0; i<numDir; i++)  {
        if ((*qbDaq)(i) == 0.0)
            (*qbDaq)(i) = kbInit(i,i) * (*db)(i);
    }
    
    // transform from basic to local system
    Matrix kl(numDOF, numDOF);
    kl.addMatrixTripleProduct(0.0, Tlb, kbInit, 1.0);
    
    // add geometric stiffness to local stiffness
    if (Mratio.Size() == 4)
        this->addPDeltaStiff(kl);
    
    // transform from local to global system
    theMatrix->addMatrixTripleProduct(0.0, Tgl, kl, 1.0);
    
    return *theMatrix;
}


const Matrix& EETwoNodeLink::getDamp()
{
    // zero the global matrix
    theMatrix->Zero();
    
    // call base class to setup Rayleigh damping
    if (addRayleigh == 1)  {
        (*theMatrix) = this->Element::getDamp();
    }
    
    return *theMatrix;
}


const Matrix& EETwoNodeLink::getMass()
{
    // zero the global matrix
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
    
    // use elastic force if force from test is zero
    for (int i=0; i<numDir; i++)  {
        if ((*qbDaq)(i) == 0.0)
            (*qbDaq)(i) = kbInit(i,i) * (*db)(i);
    }
    
    // save corresponding ctrl values for recorder
    dbCtrl = (*db);
    vbCtrl = (*vb);
    abCtrl = (*ab);
    
    // determine resisting forces in local system
    Vector ql(numDOF);
    ql.addMatrixTransposeVector(0.0, Tlb, *qbDaq, 1.0);
    
    // add P-Delta effects to local forces
    if (Mratio.Size() == 4)
        this->addPDeltaForces(ql);
    
    // determine resisting forces in global system
    theVector->addMatrixTransposeVector(0.0, Tgl, ql, 1.0);
    
    // subtract external load
    theVector->addVector(1.0, *theLoad, -1.0);
    
    return *theVector;
}


const Vector& EETwoNodeLink::getResistingForceIncInertia()
{
    // this already includes damping forces from specimen
    this->getResistingForce();
    
    // add the damping forces from rayleigh damping
    if (addRayleigh == 1)  {
        if (alphaM != 0.0 || betaK != 0.0 || betaK0 != 0.0 || betaKc != 0.0)
            theVector->addVector(1.0, this->getRayleighDampingForces(), 1.0);
    }
    
    // add inertia forces from element mass
    if (mass != 0.0)  {
        const Vector &accel1 = theNodes[0]->getTrialAccel();
        const Vector &accel2 = theNodes[1]->getTrialAccel();
        
        int numDOF2 = numDOF/2;
        double m = 0.5*mass;
        for (int i=0; i<dimension; i++)  {
            (*theVector)(i)         += m * accel1(i);
            (*theVector)(i+numDOF2) += m * accel2(i);
        }
    }
    
    return *theVector;
}


const Vector& EETwoNodeLink::getTime()
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


const Vector& EETwoNodeLink::getBasicDisp()
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


const Vector& EETwoNodeLink::getBasicVel()
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


const Vector& EETwoNodeLink::getBasicAccel()
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


const Vector& EETwoNodeLink::getBasicForce()
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
        
        for (int i=0; i<dimension; i++)  {
            v1(i) = end1Crd(i) + end1Disp(i)*fact;
            v2(i) = end2Crd(i) + end2Disp(i)*fact;
        }
    } else  {
        int mode = displayMode * -1;
        const Matrix &eigen1 = theNodes[0]->getEigenvectors();
        const Matrix &eigen2 = theNodes[1]->getEigenvectors();
        
        if (eigen1.noCols() >= mode)  {
            for (int i=0; i<dimension; i++)  {
                v1(i) = end1Crd(i) + eigen1(i,mode-1)*fact;
                v2(i) = end2Crd(i) + eigen2(i,mode-1)*fact;
            }
        } else  {
            for (int i=0; i<dimension; i++)  {
                v1(i) = end1Crd(i);
                v2(i) = end2Crd(i);
            }
        }
    }
    
    return theViewer.drawLine(v1, v2, 1.0, 1.0, this->getTag(), 0);
}


void EETwoNodeLink::Print(OPS_Stream &s, int flag)
{
    if (flag == 0)  {
        // print everything
        s << "Element: " << this->getTag() << endln;
        s << "  type: EETwoNodeLink" << endln;
        s << "  iNode: " << connectedExternalNodes(0) 
            << ", jNode: " << connectedExternalNodes(1) << endln;
        if (theSite != 0)
            s << "  ExperimentalSite: " << theSite->getTag() << endln;
        if (theTangStiff != 0)
            s << "  ExperimentalTangStiff: " << theTangStiff->getTag() << endln;
        s << "  Mratio: " << Mratio << "  shearDistI: " << shearDistI << endln;
        s << "  addRayleigh: " << addRayleigh << "  mass: " << mass << endln;
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
    
    // basic forces
    else if (strcmp(argv[0],"basicForce") == 0 ||
        strcmp(argv[0],"basicForces") == 0 ||
        strcmp(argv[0],"daqForce") == 0 ||
        strcmp(argv[0],"daqForces") == 0)
    {
        for (int i=0; i<numDir; i++)  {
            sprintf(outputData,"qb%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 3, Vector(numDir));
    }
    
    // ctrl local displacements
    else if (strcmp(argv[0],"localDisp") == 0 ||
        strcmp(argv[0],"localDisplacement") == 0 ||
        strcmp(argv[0],"localDisplacements") == 0)
    {
        for (int i=0; i<numDOF; i++)  {
            sprintf(outputData,"dl%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 4, Vector(numDOF));
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
        for (int i=0; i<numDir; i++)  {
            sprintf(outputData,"db%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 5, Vector(numDir));
    }
    
    // ctrl basic velocities
    else if (strcmp(argv[0],"ctrlVel") == 0 ||
        strcmp(argv[0],"ctrlVelocity") == 0 ||
        strcmp(argv[0],"ctrlVelocities") == 0)
    {
        for (int i=0; i<numDir; i++)  {
            sprintf(outputData,"vb%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 6, Vector(numDir));
    }
    
    // ctrl basic accelerations
    else if (strcmp(argv[0],"ctrlAccel") == 0 ||
        strcmp(argv[0],"ctrlAcceleration") == 0 ||
        strcmp(argv[0],"ctrlAccelerations") == 0)
    {
        for (int i=0; i<numDir; i++)  {
            sprintf(outputData,"ab%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 7, Vector(numDir));
    }
    
    // daq basic displacements
    else if (strcmp(argv[0],"daqDisp") == 0 ||
        strcmp(argv[0],"daqDisplacement") == 0 ||
        strcmp(argv[0],"daqDisplacements") == 0)
    {
        for (int i=0; i<numDir; i++)  {
            sprintf(outputData,"dbm%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 8, Vector(numDir));
    }
    
    // daq basic velocities
    else if (strcmp(argv[0],"daqVel") == 0 ||
        strcmp(argv[0],"daqVelocity") == 0 ||
        strcmp(argv[0],"daqVelocities") == 0)
    {
        for (int i=0; i<numDir; i++)  {
            sprintf(outputData,"vbm%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 9, Vector(numDir));
    }
    
    // daq basic accelerations
    else if (strcmp(argv[0],"daqAccel") == 0 ||
        strcmp(argv[0],"daqAcceleration") == 0 ||
        strcmp(argv[0],"daqAccelerations") == 0)
    {
        for (int i=0; i<numDir; i++)  {
            sprintf(outputData,"abm%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 10, Vector(numDir));
    }
    
    // basic deformations and basic forces
    else if (strcmp(argv[0],"defoANDforce") == 0 ||
        strcmp(argv[0],"deformationANDforce") == 0 ||
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


int EETwoNodeLink::getResponse(int responseID, Information &eleInfo)
{
    Vector defoAndForce(numDir*2);
    
    switch (responseID)  {
    case 1:  // global forces
        return eleInfo.setVector(this->getResistingForce());
        
    case 2:  // local forces
        theVector->Zero();
        // determine resisting forces in local system
        theVector->addMatrixTransposeVector(0.0, Tlb, *qbDaq, 1.0);
        // add P-Delta effects to local forces
        if (Mratio.Size() == 4)
            this->addPDeltaForces(*theVector);
        
        return eleInfo.setVector(*theVector);
        
    case 3:  // basic forces (qbDaq is already current)
        return eleInfo.setVector(*qbDaq);
        
    case 4:  // ctrl local displacements
        return eleInfo.setVector(dl);
        
    case 5:  // ctrl basic displacements
        return eleInfo.setVector(dbCtrl);
        
    case 6:  // ctrl basic velocities
        return eleInfo.setVector(vbCtrl);
        
    case 7:  // ctrl basic accelerations
        return eleInfo.setVector(abCtrl);
        
    case 8:  // daq basic displacements
        return eleInfo.setVector(this->getBasicDisp());
        
    case 9:  // daq basic velocities
        return eleInfo.setVector(this->getBasicVel());
        
    case 10:  // daq basic accelerations
        return eleInfo.setVector(this->getBasicAccel());
        
    case 11:  // basic deformations and basic forces
        defoAndForce.Zero();
        defoAndForce.Assemble(dbCtrl, 0);
        defoAndForce.Assemble(*qbDaq, numDir);
        
        return eleInfo.setVector(defoAndForce);
        
    default:
        return 0;
    }
}


// set up the transformation matrix for orientation
void EETwoNodeLink::setUp()
{
    const Vector &end1Crd = theNodes[0]->getCrds();
    const Vector &end2Crd = theNodes[1]->getCrds();
    Vector xp = end2Crd - end1Crd;
    L = xp.Norm();
    
    // setup x and y orientation vectors
    if (L > DBL_EPSILON)  {
        if (x.Size() == 0)  {
            x.resize(3);
            x.Zero();
            x(0) = xp(0);
            if (xp.Size() > 1)
                x(1) = xp(1);
            if (xp.Size() > 2)
                x(2) = xp(2);
        } else  {
            opserr << "WARNING EETwoNodeLink::setUp() - "
                << "element: " << this->getTag() << endln
                << "ignoring nodes and using specified "
                << "local x vector to determine orientation\n";
        }
        if (y.Size() == 0)  {
            y.resize(3);
            y.Zero();
            y(0) = -xp(1);
            if (xp.Size() > 1)
                y(1) = xp(0);
            if (xp.Size() > 2)
                opserr << "WARNING EETwoNodeLink::setUp() - " 
                    << "element: " << this->getTag() << endln
                    << "no local y vector specified\n";
        }
    } else  {
        if (x.Size() == 0)  {
            x.resize(3);
            x(0) = 1.0; x(1) = 0.0; x(2) = 0.0;
        }
        if (y.Size() == 0)  {
            y.resize(3);
            y(0) = 0.0; y(1) = 1.0; y(2) = 0.0;
        }
    }
    
    // check that vectors for orientation are of correct size
    if (x.Size() != 3 || y.Size() != 3)  {
        opserr << "EETwoNodeLink::setUp() - "
            << "element: " << this->getTag() << endln
            << "incorrect dimension of orientation vectors\n";
        exit(-1);
    }
    
    // establish orientation of element for the tranformation matrix
    // z = x cross yp
    static Vector z(3);
    z(0) = x(1)*y(2) - x(2)*y(1);
    z(1) = x(2)*y(0) - x(0)*y(2);
    z(2) = x(0)*y(1) - x(1)*y(0);
    
    // y = z cross x
    y(0) = z(1)*x(2) - z(2)*x(1);
    y(1) = z(2)*x(0) - z(0)*x(2);
    y(2) = z(0)*x(1) - z(1)*x(0);
    
    // compute length(norm) of vectors
    double xn = x.Norm();
    double yn = y.Norm();
    double zn = z.Norm();
    
    // check valid x and y vectors, i.e. not parallel and of zero length
    if (xn == 0 || yn == 0 || zn == 0)  {
        opserr << "EETwoNodeLink::setUp() - "
            << "element: " << this->getTag() << endln
            << "invalid orientation vectors\n";
        exit(-1);
    }
    
    // create transformation matrix of direction cosines
    for (int i=0; i<3; i++)  {
        trans(0,i) = x(i)/xn;
        trans(1,i) = y(i)/yn;
        trans(2,i) = z(i)/zn;
    }
}


// set transformation matrix from global to local system
void EETwoNodeLink::setTranGlobalLocal()
{
    // resize transformation matrix and zero it
    Tgl.resize(numDOF, numDOF);
    Tgl.Zero();
    
    // switch on dimensionality of element
    switch (elemType)  {
    case D1N2:
        Tgl(0,0) = Tgl(1,1) = trans(0,0);
        break;
    case D2N4:
        Tgl(0,0) = Tgl(2,2) = trans(0,0);
        Tgl(0,1) = Tgl(2,3) = trans(0,1);
        Tgl(1,0) = Tgl(3,2) = trans(1,0);
        Tgl(1,1) = Tgl(3,3) = trans(1,1);
        break;
    case D2N6:
        Tgl(0,0) = Tgl(3,3) = trans(0,0);
        Tgl(0,1) = Tgl(3,4) = trans(0,1);
        Tgl(1,0) = Tgl(4,3) = trans(1,0);
        Tgl(1,1) = Tgl(4,4) = trans(1,1);
        Tgl(2,2) = Tgl(5,5) = trans(2,2);
        break;
    case D3N6:
        Tgl(0,0) = Tgl(3,3) = trans(0,0);
        Tgl(0,1) = Tgl(3,4) = trans(0,1);
        Tgl(0,2) = Tgl(3,5) = trans(0,2);
        Tgl(1,0) = Tgl(4,3) = trans(1,0);
        Tgl(1,1) = Tgl(4,4) = trans(1,1);
        Tgl(1,2) = Tgl(4,5) = trans(1,2);
        Tgl(2,0) = Tgl(5,3) = trans(2,0);
        Tgl(2,1) = Tgl(5,4) = trans(2,1);
        Tgl(2,2) = Tgl(5,5) = trans(2,2);
        break;
    case D3N12:
        Tgl(0,0) = Tgl(3,3) = Tgl(6,6) = Tgl(9,9)   = trans(0,0);
        Tgl(0,1) = Tgl(3,4) = Tgl(6,7) = Tgl(9,10)  = trans(0,1);
        Tgl(0,2) = Tgl(3,5) = Tgl(6,8) = Tgl(9,11)  = trans(0,2);
        Tgl(1,0) = Tgl(4,3) = Tgl(7,6) = Tgl(10,9)  = trans(1,0);
        Tgl(1,1) = Tgl(4,4) = Tgl(7,7) = Tgl(10,10) = trans(1,1);
        Tgl(1,2) = Tgl(4,5) = Tgl(7,8) = Tgl(10,11) = trans(1,2);
        Tgl(2,0) = Tgl(5,3) = Tgl(8,6) = Tgl(11,9)  = trans(2,0);
        Tgl(2,1) = Tgl(5,4) = Tgl(8,7) = Tgl(11,10) = trans(2,1);
        Tgl(2,2) = Tgl(5,5) = Tgl(8,8) = Tgl(11,11) = trans(2,2);
        break;
    }
}


// set transformation matrix from local to basic system
void EETwoNodeLink::setTranLocalBasic()
{
    // resize transformation matrix and zero it
    Tlb.resize(numDir, numDOF);
    Tlb.Zero();
    
    for (int i=0; i<numDir; i++)  {
        
        int dirID = (*dir)(i);     // direction 0 to 5;
        Tlb(i,dirID) = -1.0;
        Tlb(i,dirID+numDOF/2) = 1.0;
        
        // switch on dimensionality of element
        switch (elemType)  {
        case D2N6:
            if (dirID == 1)  {
                Tlb(i,2) = -shearDistI(0)*L;
                Tlb(i,5) = -(1.0 - shearDistI(0))*L;
            }
            break;
        case D3N12:
            if (dirID == 1)  {
                Tlb(i,5)  = -shearDistI(0)*L;
                Tlb(i,11) = -(1.0-shearDistI(0))*L;
            }
            else if (dirID == 2)  {
                Tlb(i,4)  = shearDistI(1)*L;
                Tlb(i,10) = (1.0-shearDistI(1))*L;
            }
            break;
        default :
            // do nothing
            break;
        }
    }
}


void EETwoNodeLink::addPDeltaForces(Vector &pLocal)
{
    int dirID;
    double N = 0.0;
    double deltal1 = 0.0;
    double deltal2 = 0.0;
    
    for (int i=0; i<numDir; i++)  {
        dirID = (*dir)(i);  // direction 0 to 5;
        
        // get axial force and local disp differences
        if (dirID == 0)
            N = (*qbDaq)(i);
        else if (dirID == 1 && dimension > 1)
            deltal1 = dl(1+numDOF/2) - dl(1);
        else if (dirID == 2 && dimension > 2)
            deltal2 = dl(2+numDOF/2) - dl(2);
    }
    
    if (N != 0.0 && (deltal1 != 0.0 || deltal2 != 0.0))  {
        for (int i=0; i<numDir; i++)  {
            dirID = (*dir)(i);  // direction 0 to 5;
            
            // switch on dimensionality of element
            switch (elemType)  {
            case D2N4:
                if (dirID == 1)  {
                    double VpDelta = N*deltal1/L;
                    VpDelta *= 1.0-Mratio(2)-Mratio(3);
                    pLocal(1) -= VpDelta;
                    pLocal(3) += VpDelta;
                }
                break;
            case D2N6: 
                if (dirID == 1)  {
                    double VpDelta = N*deltal1/L;
                    VpDelta *= 1.0-Mratio(2)-Mratio(3);
                    pLocal(1) -= VpDelta;
                    pLocal(4) += VpDelta;
                }
                else if (dirID == 2)  {
                    double MpDelta = N*deltal1;
                    pLocal(2) += Mratio(2)*MpDelta;
                    pLocal(5) += Mratio(3)*MpDelta;
                }
                break;
            case D3N6:
                if (dirID == 1)  {
                    double VpDelta = N*deltal1/L;
                    VpDelta *= 1.0-Mratio(2)-Mratio(3);
                    pLocal(1) -= VpDelta;
                    pLocal(4) += VpDelta;
                }
                else if (dirID == 2)  {
                    double VpDelta = N*deltal2/L;
                    VpDelta *= 1.0-Mratio(0)-Mratio(1);
                    pLocal(2) -= VpDelta;
                    pLocal(5) += VpDelta;
                }
                break;
            case D3N12:
                if (dirID == 1)  {
                    double VpDelta = N*deltal1/L;
                    VpDelta *= 1.0-Mratio(2)-Mratio(3);
                    pLocal(1) -= VpDelta;
                    pLocal(7) += VpDelta;
                }
                else if (dirID == 2)  {
                    double VpDelta = N*deltal2/L;
                    VpDelta *= 1.0-Mratio(0)-Mratio(1);
                    pLocal(2) -= VpDelta;
                    pLocal(8) += VpDelta;
                }
                else if (dirID == 4)  {
                    double MpDelta = N*deltal2;
                    pLocal(4) -= Mratio(0)*MpDelta;
                    pLocal(10) -= Mratio(1)*MpDelta;
                }
                else if (dirID == 5)  {
                    double MpDelta = N*deltal1;
                    pLocal(5) += Mratio(2)*MpDelta;
                    pLocal(11) += Mratio(3)*MpDelta;
                }
                break;
            default :
                // do nothing
                break;
            }
        }
    }
}


void EETwoNodeLink::addPDeltaStiff(Matrix &kLocal)
{
    int dirID;
    double N = 0.0;
    
    // get axial force
    for (int i=0; i<numDir; i++)  {
        if ((*dir)(i) == 0)
            N = (*qbDaq)(i);
    }
    
    if (N != 0.0)  {
        for (int i=0; i<numDir; i++)  {
            dirID = (*dir)(i);  // direction 0 to 5;
            
            // switch on dimensionality of element
            switch (elemType)  {
            case D2N4:
                if (dirID == 1)  {
                    double NoverL = N/L;
                    NoverL *= 1.0-Mratio(2)-Mratio(3);
                    kLocal(1,1) += NoverL;
                    kLocal(1,3) -= NoverL;
                    kLocal(3,1) -= NoverL;
                    kLocal(3,3) += NoverL;
                }
                break;
            case D2N6: 
                if (dirID == 1)  {
                    double NoverL = N/L;
                    NoverL *= 1.0-Mratio(2)-Mratio(3);
                    kLocal(1,1) += NoverL;
                    kLocal(1,4) -= NoverL;
                    kLocal(4,1) -= NoverL;
                    kLocal(4,4) += NoverL;
                }
                else if (dirID == 2)  {
                    kLocal(2,1) -= Mratio(2)*N;
                    kLocal(2,4) += Mratio(2)*N;
                    kLocal(5,1) -= Mratio(3)*N;
                    kLocal(5,4) += Mratio(3)*N;
                }
                break;
            case D3N6:
                if (dirID == 1)  {
                    double NoverL = N/L;
                    NoverL *= 1.0-Mratio(2)-Mratio(3);
                    kLocal(1,1) += NoverL;
                    kLocal(1,4) -= NoverL;
                    kLocal(4,1) -= NoverL;
                    kLocal(4,4) += NoverL;
                }
                else if (dirID == 2)  {
                    double NoverL = N/L;
                    NoverL *= 1.0-Mratio(0)-Mratio(1);
                    kLocal(2,2) += NoverL;
                    kLocal(2,5) -= NoverL;
                    kLocal(5,2) -= NoverL;
                    kLocal(5,5) += NoverL;
                }
                break;
            case D3N12:
                if (dirID == 1)  {
                    double NoverL = N/L;
                    NoverL *= 1.0-Mratio(2)-Mratio(3);
                    kLocal(1,1) += NoverL;
                    kLocal(1,7) -= NoverL;
                    kLocal(7,1) -= NoverL;
                    kLocal(7,7) += NoverL;
                }
                else if (dirID == 2)  {
                    double NoverL = N/L;
                    NoverL *= 1.0-Mratio(0)-Mratio(1);
                    kLocal(2,2) += NoverL;
                    kLocal(2,8) -= NoverL;
                    kLocal(8,2) -= NoverL;
                    kLocal(8,8) += NoverL;
                }
                else if (dirID == 4)  {
                    kLocal(4,2) += Mratio(0)*N;
                    kLocal(4,8) -= Mratio(0)*N;
                    kLocal(10,2) += Mratio(1)*N;
                    kLocal(10,8) -= Mratio(1)*N;
                }
                else if (dirID == 5)  {
                    kLocal(5,1) -= Mratio(2)*N;
                    kLocal(5,7) += Mratio(2)*N;
                    kLocal(11,1) -= Mratio(3)*N;
                    kLocal(11,7) += Mratio(3)*N;
                }
                break;
            default :
                // do nothing
                break;
            }
        }
    }
}
