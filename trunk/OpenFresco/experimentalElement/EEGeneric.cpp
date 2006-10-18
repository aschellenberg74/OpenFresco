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
// $Source$

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 10/06
// Revision: A
//
// Description: This file contains the implementation of the EEGeneric class.

#include "EEGeneric.h"

#include <ArrayOfTaggedObjects.h>
#include <Domain.h>
#include <Node.h>
#include <Channel.h>
#include <FEM_ObjectBroker.h>
#include <Renderer.h>
#include <Information.h>
#include <ElementResponse.h>

#include <math.h>
#include <stdlib.h>
#include <string.h>


// initialize the class wide variables
Matrix EEGeneric::theMatrix(1,1);
Vector EEGeneric::theVector(1);
Vector EEGeneric::theLoad(1);


// responsible for allocating the necessary space needed by each object
// and storing the tags of the ExperimentalBeamColumn end nodes.
EEGeneric::EEGeneric(int tag, ExperimentalSite *site,
    ID nodes, ID *dof, bool iM, Matrix *m)
    : ExperimentalElement(tag, ELE_TAG_EEGeneric, site),
    connectedExternalNodes(nodes), basicDOF(1),
    numExternalNodes(0), numDOF(0), numBasicDOF(0),
    iMod(iM), mass(m),
    q(1), targDisp(1), kbInit(1,1)
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
    
    (*sizeCtrl)[OF_Resp_Disp] = numBasicDOF;
    (*sizeCtrl)[OF_Resp_Vel] = numBasicDOF;
    (*sizeCtrl)[OF_Resp_Accel] = numBasicDOF;
    (*sizeCtrl)[OF_Resp_Time] = 1;
    
    (*sizeDaq)[OF_Resp_Disp] = numBasicDOF;
    (*sizeDaq)[OF_Resp_Force] = numBasicDOF;
    
    theSite->setSize(*sizeCtrl, *sizeDaq);
    
    // set the vector and matrix sizes and zero them
    basicDOF.resize(numBasicDOF);
    basicDOF.Zero();
    q.resize(numBasicDOF);
    q.Zero();
    targDisp.resize(numBasicDOF);
    targDisp.Zero();
    kbInit.resize(numBasicDOF,numBasicDOF);
    kbInit.Zero();
}


// delete must be invoked on any objects created by the object
// and on the experimental object.
EEGeneric::~EEGeneric()
{
    // invoke the destructor on any objects created by the object
    // that the object still holds a pointer to
    if (theNodes != 0)
        delete [] theNodes;
    if (theDOF != 0)
        delete [] theDOF;
}


int EEGeneric::getNumExternalNodes(void) const
{
    return numExternalNodes;
}


const ID& EEGeneric::getExternalNodes(void) 
{
    return connectedExternalNodes;
}


Node** EEGeneric::getNodePtrs(void) 
{
    return theNodes;
}


int EEGeneric::getNumDOF(void) 
{
    return numDOF;
}


int EEGeneric::getNumBasicDOF(void) 
{
    return numBasicDOF;
}


// to set a link to the enclosing Domain and to set the node pointers.
// also determines the number of dof associated
// with the ExperimentalBeamColumn element, we set matrix and 
// vector pointers, allocate space for t matrix and define it 
// as the basic deformation-displacement transformation matrix.
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
            opserr << "WARNING EEGeneric::setDomain() - Nd" << i << ": " 
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
    theInitStif.resize(numDOF,numDOF);
    theInitStif.Zero();
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
    return theSite->commitState();
}


int EEGeneric::update(void)
{
    // get current time
    static Vector time(1);
    Domain *theDomain = this->getDomain();
    time(0) = theDomain->getCurrentTime();

    // assemble response vectors
    int ndim = 0, i;
    static Vector db(numBasicDOF);
    static Vector vb(numBasicDOF);
    static Vector ab(numBasicDOF);
    db.Zero(); vb.Zero(); ab.Zero();

    for (i=0; i<numExternalNodes; i++)  {
        Vector disp = theNodes[i]->getTrialDisp();
        Vector vel = theNodes[i]->getTrialVel();
        Vector accel = theNodes[i]->getTrialAccel();
        db.Assemble(disp(theDOF[i]), ndim);
        vb.Assemble(vel(theDOF[i]), ndim);
        ab.Assemble(accel(theDOF[i]), ndim);
        ndim += theDOF[i].Size();
    }
 
    if (db != targDisp)  {
        // save the target displacement
        targDisp = db;
        // set the trial response at the site
        theSite->setTrialResponse(&db, &vb, &ab, (Vector*)0, &time);
    }
    
    return 0;
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
    
    theInitStif.Zero();
    theInitStif.Assemble(kbInit,basicDOF,basicDOF);
    
    return 0;
}


const Matrix& EEGeneric::getMass(void)
{
    // zero the matrix
    theMatrix.Zero();

    // assemble mass matrix
    if (mass != 0)  {
        theMatrix.Assemble(*mass,basicDOF,basicDOF);
    }
    
    return theMatrix; 
}


void EEGeneric::zeroLoad(void)
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
    q = theSite->getForce();
    
    if (iMod == true)  {
        // get measured displacements
        static Vector measDisp(numBasicDOF);
        measDisp = theSite->getDisp();
        
        // correct for displacement control errors using I-Modification
        q -= kbInit*(measDisp - targDisp);
    }
   
    // determine resisting forces in global system
    theVector.Assemble(q,basicDOF);
    
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
    }
    
    return rValue;
}


void EEGeneric::Print(OPS_Stream &s, int flag)
{
    int i;
    if (flag == 0)  {
        // print everything
        s << "Element: " << this->getTag(); 
        s << "  type: EEGeneric";
        for (i=0; i<numExternalNodes; i++ )
            s << "  Node" << i+1 << ": " << connectedExternalNodes(i) << endln;
        s << "  ExperimentalSite, tag: " << theSite->getTag() << endln;
    } else if (flag == 1)  {
        // does nothing
    }
}


Response* EEGeneric::setResponse(const char **argv, int argc,
    Information &eleInformation, OPS_Stream &output)
{
    Response *theResponse = 0;

    int i;
    char outputData[10];

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
         for (int i=0; i<numDOF; i++)  {
            sprintf(outputData,"P%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 2, theVector);
    }
    // local forces
    else if (strcmp(argv[0],"localForce") == 0 || strcmp(argv[0],"localForces") == 0)
    {
        for (int i=0; i<numDOF; i++)  {
            sprintf(outputData,"p%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 3, theVector);
    }
    // forces in basic system B
    else if (strcmp(argv[0],"basicForce") == 0 || strcmp(argv[0],"basicForces") == 0)
    {
        for (int i=0; i<numBasicDOF; i++)  {
            sprintf(outputData,"q%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 4, Vector(numBasicDOF));
    }
    // deformations in basic system B
    else if (strcmp(argv[0],"deformation") == 0 || strcmp(argv[0],"deformations") == 0 || 
        strcmp(argv[0],"basicDeformation") == 0 || strcmp(argv[0],"basicDeformations") == 0)
    {
        for (int i=0; i<numBasicDOF; i++)  {
            sprintf(outputData,"v%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 5, Vector(numBasicDOF));
    }

    output.endTag(); // ElementOutput

    return theResponse;
}


int EEGeneric::getResponse(int responseID, Information &eleInformation)
{    
    switch (responseID)  {
    case -1:
        return -1;
        
    case 1:  // initial stiffness
        if (eleInformation.theMatrix != 0)  {
            *(eleInformation.theMatrix) = theInitStif;
        }
        return 0;
        
    case 2:  // global forces
        if (eleInformation.theVector != 0)  {
            *(eleInformation.theVector) = this->getResistingForce();
        }
        return 0;      
        
    case 3:  // local forces
        if (eleInformation.theVector != 0)  {
            *(eleInformation.theVector) = this->getResistingForce();
        }
        return 0;
        
    case 4:  // forces in basic system
        if (eleInformation.theVector != 0)  {
            *(eleInformation.theVector) = q;
        }
        return 0;      
        
    case 5:  // deformations in basic system
        if (eleInformation.theVector != 0)  {
            *(eleInformation.theVector) = targDisp;
        }
        return 0;      
        
    default:
        return -1;
    }
}
