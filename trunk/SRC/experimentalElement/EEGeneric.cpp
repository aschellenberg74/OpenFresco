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

#include <math.h>
#include <stdlib.h>
#include <string.h>


// initialize the class wide variables
Matrix EEGeneric::theMatrix(1,1);
Vector EEGeneric::theVector(1);
Vector EEGeneric::theLoad(1);


// responsible for allocating the necessary space needed
// by each object and storing the tags of the end nodes.
EEGeneric::EEGeneric(int tag, ID nodes, ID *dof,
    ExperimentalSite *site,
    bool iM, Matrix *m)
    : ExperimentalElement(tag, ELE_TAG_EEGeneric, site),
    connectedExternalNodes(nodes), basicDOF(1),
    numExternalNodes(0), numDOF(0), numBasicDOF(0),
    iMod(iM), mass(m),
    db(0), vb(0), ab(0), t(0),
    dbMeas(0), vbMeas(0), abMeas(0), qMeas(0), tMeas(0),
    dbTarg(1), vbTarg(1), abTarg(1),
    dbPast(1), kbInit(1,1), tPast(0.0)
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

    // allocate memory for measured response vectors
    dbMeas = new Vector(numBasicDOF);
    vbMeas = new Vector(numBasicDOF);
    abMeas = new Vector(numBasicDOF);
    qMeas  = new Vector(numBasicDOF);
    tMeas  = new Vector(1);

    // set the vector and matrix sizes and zero them
    basicDOF.resize(numBasicDOF);
    basicDOF.Zero();
    dbTarg.resize(numBasicDOF);
    dbTarg.Zero();
    vbTarg.resize(numBasicDOF);
    vbTarg.Zero();
    abTarg.resize(numBasicDOF);
    abTarg.Zero();
    dbPast.resize(numBasicDOF);
    dbPast.Zero();
    kbInit.resize(numBasicDOF,numBasicDOF);
    kbInit.Zero();
}


// responsible for allocating the necessary space needed
// by each object and storing the tags of the end nodes.
EEGeneric::EEGeneric(int tag, ID nodes, ID *dof,
    int port, char *machineInetAddr, int ssl, int dataSize,
    bool iM, Matrix *m)
    : ExperimentalElement(tag, ELE_TAG_EEGeneric),
    connectedExternalNodes(nodes), basicDOF(1),
    numExternalNodes(0), numDOF(0), numBasicDOF(0),
    iMod(iM), mass(m),
    theChannel(0), sData(0), sendData(0), rData(0), recvData(0),
    db(0), vb(0), ab(0), t(0),
    dbMeas(0), vbMeas(0), abMeas(0), qMeas(0), tMeas(0),
    dbTarg(1), vbTarg(1), abTarg(1),
    dbPast(1), kbInit(1,1), tPast(0.0)
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
        opserr << "EEGeneric::EEGeneric() "
            << "- failed to create channel\n";
        exit(-1);
    }
    if (theChannel->setUpConnection() != 0)  {
        opserr << "EEGeneric::EEGeneric() "
            << "- failed to setup connection\n";
        exit(-1);
    }

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
    dbMeas = new Vector(&rData[id], 1);
    id += numBasicDOF;
    vbMeas = new Vector(&rData[id], 1);
    id += numBasicDOF;
    abMeas = new Vector(&rData[id], 1);
    id += numBasicDOF;
    qMeas = new Vector(&rData[id], 1);
    id += numBasicDOF;
    tMeas = new Vector(&rData[id], 1);
    recvData->Zero();

    // set the vector and matrix sizes and zero them
    basicDOF.resize(numBasicDOF);
    basicDOF.Zero();
    dbTarg.resize(numBasicDOF);
    dbTarg.Zero();
    vbTarg.resize(numBasicDOF);
    vbTarg.Zero();
    abTarg.resize(numBasicDOF);
    abTarg.Zero();
    dbPast.resize(numBasicDOF);
    dbPast.Zero();
    kbInit.resize(numBasicDOF,numBasicDOF);
    kbInit.Zero();
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
    
    if (theSite != 0)  {
        rValue += theSite->commitState();
    }
    else  {
        sData[0] = OF_RemoteTest_commitState;
        rValue += theChannel->sendVector(0, 0, *sendData, 0);
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
    theInitStiff.Assemble(kbInit,basicDOF,basicDOF);
    
    return 0;
}


const Matrix& EEGeneric::getMass()
{
    // zero the matrix
    theMatrix.Zero();

    // assemble mass matrix
    if (mass != 0)  {
        theMatrix.Assemble(*mass,basicDOF,basicDOF);
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
    static Vector Raccel(numDOF);
    Raccel.Zero();

    // get mass matrix
    Matrix M = this->getMass();
    // assemble Raccel vector
    for (i=0; i<numExternalNodes; i++ )  {
        Raccel.Assemble(theNodes[i]->getRV(accel), ndim);
        ndim += theNodes[i]->getNumberDOF();
    }
    
    // want to add ( - fact * M R * accel ) to unbalance
    theLoad -= M * Raccel;
    
    return 0;
}


const Vector& EEGeneric::getResistingForce()
{
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
    }
   
    // save corresponding target displacements for recorder
    dbTarg = (*db);
    vbTarg = (*vb);
    abTarg = (*ab);

    // determine resisting forces in global system
    theVector.Assemble(*qMeas,basicDOF);
    
    // subtract external load
    theVector.addVector(1.0, theLoad, -1.0);

    return theVector;
}


const Vector& EEGeneric::getResistingForceIncInertia()
{	
    theVector = this->getResistingForce();
    
    // add the damping forces if rayleigh damping
    if (alphaM != 0.0 || betaK != 0.0 || betaK0 != 0.0 || betaKc != 0.0)
        theVector += this->getRayleighDampingForces();
    
    // now include the mass portion
    if (mass != 0)  {
        int ndim = 0, i;
        static Vector accel(numDOF);
        accel.Zero();

        // get mass matrix
        Matrix M = this->getMass();
        // assemble accel vector
        for (i=0; i<numExternalNodes; i++ )  {
            accel.Assemble(theNodes[i]->getTrialAccel(), ndim);
            ndim += theNodes[i]->getNumberDOF();
        }
        
        theVector += M * accel;
    }
    
    return theVector;
}


const Vector& EEGeneric::getTime()
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


const Vector& EEGeneric::getBasicDisp()
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


const Vector& EEGeneric::getBasicVel()
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


const Vector& EEGeneric::getBasicAccel()
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
    int displayMode, float fact)
{
    int rValue = 0, i, j;

    if (numExternalNodes > 1)  {
        Vector *v = new Vector [numExternalNodes];

        // first determine the end points of the beam based on
        // the display factor (a measure of the distorted image)
        for (i=0; i<numExternalNodes; i++)  {
            const Vector &endCrd = theNodes[i]->getCrds();
            const Vector &endDisp = theNodes[i]->getDisp();
            int numCrds = endCrd.Size();
            for (j=0; j<numCrds; i++)
                v[i](j) = endCrd(j) + endDisp(j)*fact;
        }

        for (i=0; i<numExternalNodes-1; i++)
            rValue += theViewer.drawLine (v[i], v[i+1], 1.0, 1.0);
        //rValue += theViewer.drawLine (v[i+1], v[0], 1.0, 1.0);
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
    if (strcmp(argv[0],"force") == 0 || strcmp(argv[0],"forces") == 0 ||
        strcmp(argv[0],"globalForce") == 0 || strcmp(argv[0],"globalForces") == 0)
    {
        for (i=0; i<numDOF; i++)  {
            sprintf(outputData,"P%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 1, theVector);
    }
    // local forces
    else if (strcmp(argv[0],"localForce") == 0 || strcmp(argv[0],"localForces") == 0)
    {
        for (i=0; i<numDOF; i++)  {
            sprintf(outputData,"p%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 2, theVector);
    }
    // basic forces
    else if (strcmp(argv[0],"basicForce") == 0 || strcmp(argv[0],"basicForces") == 0)
    {
        for (i=0; i<numBasicDOF; i++)  {
            sprintf(outputData,"q%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 3, Vector(numBasicDOF));
    }
    // target basic displacements
    else if (strcmp(argv[0],"deformation") == 0 || strcmp(argv[0],"deformations") == 0 || 
        strcmp(argv[0],"basicDeformation") == 0 || strcmp(argv[0],"basicDeformations") == 0 ||
        strcmp(argv[0],"targetDisplacement") == 0 || strcmp(argv[0],"targetDisplacements") == 0)
    {
        for (i=0; i<numBasicDOF; i++)  {
            sprintf(outputData,"db%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 4, Vector(numBasicDOF));
    }
    // target basic velocities
    else if (strcmp(argv[0],"targetVelocity") == 0 || 
        strcmp(argv[0],"targetVelocities") == 0)
    {
        for (i=0; i<numBasicDOF; i++)  {
            sprintf(outputData,"vb%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 5, Vector(numBasicDOF));
    }
    // target basic accelerations
    else if (strcmp(argv[0],"targetAcceleration") == 0 || 
        strcmp(argv[0],"targetAccelerations") == 0)
    {
        for (i=0; i<numBasicDOF; i++)  {
            sprintf(outputData,"ab%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 6, Vector(numBasicDOF));
    }
    // measured basic displacements
    else if (strcmp(argv[0],"measuredDisplacement") == 0 || 
        strcmp(argv[0],"measuredDisplacements") == 0)
    {
        for (i=0; i<numBasicDOF; i++)  {
            sprintf(outputData,"dbm%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 7, Vector(numBasicDOF));
    }
    // measured basic velocities
    else if (strcmp(argv[0],"measuredVelocity") == 0 || 
        strcmp(argv[0],"measuredVelocities") == 0)
    {
        for (i=0; i<numBasicDOF; i++)  {
            sprintf(outputData,"vbm%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 8, Vector(numBasicDOF));
    }
    // measured basic accelerations
    else if (strcmp(argv[0],"measuredAcceleration") == 0 || 
        strcmp(argv[0],"measuredAccelerations") == 0)
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
