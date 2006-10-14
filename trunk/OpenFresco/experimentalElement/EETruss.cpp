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
// Created: 09/06
// Revision: A
//
// Description: This file contains the implementation of the EETruss class.

#include "EETruss.h"

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
Matrix EETruss::EETrussM2(2,2);
Matrix EETruss::EETrussM4(4,4);
Matrix EETruss::EETrussM6(6,6);
Matrix EETruss::EETrussM12(12,12);
Vector EETruss::EETrussV2(2);
Vector EETruss::EETrussV4(4);
Vector EETruss::EETrussV6(6);
Vector EETruss::EETrussV12(12);


// responsible for allocating the necessary space needed by each object
// and storing the tags of the EETruss end nodes
EETruss::EETruss(int tag, int dim, int Nd1, int Nd2, 
    ExperimentalSite *site,
    bool iM, bool copy, double r)
    : ExperimentalElement(tag, ELE_TAG_EETruss, site),
    dimension(dim), numDOF(0),
    connectedExternalNodes(2),
    iMod(iM), isCopy(copy), rho(r), L(0.0), 
    theMatrix(0), theVector(0), theLoad(0),
    q(1), targDisp(1), kbInit(1,1)
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
    for (int i=0; i<2; i++)
        theNodes[i] = 0;
    
    // set the data size for the experimental site
    sizeCtrl = new ID(OF_Resp_All);
    sizeDaq = new ID(OF_Resp_All);
    
    (*sizeCtrl)[OF_Resp_Disp] = 1;
    (*sizeCtrl)[OF_Resp_Vel] = 1;
    (*sizeCtrl)[OF_Resp_Accel] = 1;
    (*sizeCtrl)[OF_Resp_Time] = 1;
    
    (*sizeDaq)[OF_Resp_Disp] = 1;
    (*sizeDaq)[OF_Resp_Force] = 1;
    
    if (isCopy == false)
        theSite->setSize(*sizeCtrl, *sizeDaq);
    
    // initialize vectors
    q.Zero();
    targDisp.Zero();
    
    // open output file
    //outFile = fopen("elemDisp.out","w");
    //if (outFile==NULL)  {
    //	opserr << "EETruss::EETruss() - "
    //		<< "fopen: could not open output file\n";
    //  exit(-1);
    //}
}


// delete must be invoked on any objects created by the object
// and on the experimental object
EETruss::~EETruss()
{
    // close output file
    //fclose(outFile);
    
    // invoke the destructor on any objects created by the object
    // that the object still holds a pointer to
    if (theLoad)
        delete theLoad;
}


int EETruss::getNumExternalNodes(void) const
{
    return 2;
}


const ID& EETruss::getExternalNodes(void) 
{
    return connectedExternalNodes;
}


Node** EETruss::getNodePtrs(void) 
{
    return theNodes;
}


int EETruss::getNumDOF(void) 
{
    return numDOF;
}


int EETruss::getNumBasicDOF(void) 
{
    return 1;
}


// to set a link to the enclosing Domain and to set the node pointers
// also determines the number of dof associated
// with the EETruss element, we set matrix and vector pointers,
// allocate space for t matrix, determine the length
// and set the transformation matrix
void EETruss::setDomain(Domain *theDomain)
{
    // check Domain is not null - invoked when object removed from a domain
    if (!theDomain)  {
        theNodes[0] = 0;
        theNodes[1] = 0;
        L = 0;
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
    if (dimension == 1 && dofNd1 == 1)  {
        numDOF = 2;    
        theMatrix = &EETrussM2;
        theVector = &EETrussV2;
    }
    else if (dimension == 2 && dofNd1 == 2)  {
        numDOF = 4;
        theMatrix = &EETrussM4;
        theVector = &EETrussV4;	
    }
    else if (dimension == 2 && dofNd1 == 3)  {
        numDOF = 6;	
        theMatrix = &EETrussM6;
        theVector = &EETrussV6;		
    }
    else if (dimension == 3 && dofNd1 == 3)  {
        numDOF = 6;	
        theMatrix = &EETrussM6;
        theVector = &EETrussV6;			
    }
    else if (dimension == 3 && dofNd1 == 6)  {
        numDOF = 12;	    
        theMatrix = &EETrussM12;
        theVector = &EETrussV12;			
    }
    else  {
        opserr <<"EETruss::setDomain() - can not handle "
            << dimension << " dofs at nodes in " << dofNd1  << " d problem\n";
        
        return;
    }
    
    // set the initial stiffness matrix size
    theInitStif.resize(numDOF,numDOF);
    
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
    
    // zero the cosines
    cosX[0] = cosX[1] = cosX[2] = 0.0;
    
    if (dimension == 1)  {
        double dx = end2Crd(0)-end1Crd(0);	
        
        L = sqrt(dx*dx);
        
        if (L == 0.0)  {
            opserr <<"EETruss::setDomain() - element: "
                << this->getTag() << " has zero length\n";
            return;
        }
        
        cosX[0] = 1.0;
    }
    else if (dimension == 2)  {
        double dx = end2Crd(0)-end1Crd(0);
        double dy = end2Crd(1)-end1Crd(1);	
        
        L = sqrt(dx*dx + dy*dy);
        
        if (L == 0.0) {
            opserr <<"EETruss::setDomain() - element: "
                << this->getTag() << " has zero length\n";
            return;
        }
        
        cosX[0] = dx/L;
        cosX[1] = dy/L;
    }
    else  {
        double dx = end2Crd(0)-end1Crd(0);
        double dy = end2Crd(1)-end1Crd(1);	
        double dz = end2Crd(2)-end1Crd(2);		
        
        L = sqrt(dx*dx + dy*dy + dz*dz);
        
        if (L == 0.0)  {
            opserr <<"EETruss::setDomain() - element: "
                << this->getTag() << " has zero length\n";
            return;
        }
        
        cosX[0] = dx/L;
        cosX[1] = dy/L;
        cosX[2] = dz/L;
    }
}   	 


int EETruss::commitState()
{
    int rValue = 0;
    
    if (isCopy == false)
        rValue += theSite->commitState();
    
    return rValue;
}


int EETruss::update(void)
{
    // get current time
    Vector time(1);
    Domain *theDomain = this->getDomain();
    time(0) = theDomain->getCurrentTime();
    
    // determine dsp, vel and acc in basic system
    const Vector &dsp1 = theNodes[0]->getTrialDisp();
    const Vector &dsp2 = theNodes[1]->getTrialDisp();	
    const Vector &vel1 = theNodes[0]->getTrialVel();
    const Vector &vel2 = theNodes[1]->getTrialVel();	
    const Vector &acc1 = theNodes[0]->getTrialAccel();
    const Vector &acc2 = theNodes[1]->getTrialAccel();	
    
    static Vector db(1), vb(1), ab(1);
    db(0) = vb(0) = ab(0) = 0.0;
    for (int i = 0; i < dimension; i++)  {
        db(0) += (dsp2(i)-dsp1(i))*cosX[i];
        vb(0) += (vel2(i)-vel1(i))*cosX[i];
        ab(0) += (acc2(i)-acc1(i))*cosX[i];
    }
    
    if (db != targDisp)  {
        // save the target displacement
        targDisp = db;
        // set the trial response at the site
        if (isCopy == false)
            theSite->setTrialResponse(&db, &vb, &ab, (Vector*)0, &time);
    }
    
    return 0;
}


int EETruss::setInitialStiff(const Matrix& kbinit)
{
    kbInit = kbinit;
    
    if (kbInit.noRows() != 1 || kbInit.noCols() != 1)  {
        opserr << "EETruss::setInitialStiff(): " 
            << "matrix size is incorrect for element: "
            << this->getTag() << endln;
        return -1;
    }
    
    // transform the stiffness from the basic to the global system
    theInitStif.Zero();
    int numDOF2 = numDOF/2;
    double temp;
    for (int i = 0; i < dimension; i++)  {
        for (int j = 0; j < dimension; j++)  {
            temp = cosX[i]*cosX[j]*kbInit(0,0);
            theInitStif(i,j) = temp;
            theInitStif(i+numDOF2,j) = -temp;
            theInitStif(i,j+numDOF2) = -temp;
            theInitStif(i+numDOF2,j+numDOF2) = temp;
        }
    }
    
    return 0;
}


const Matrix& EETruss::getMass(void)
{   
    // zero the matrix
    theMatrix->Zero();    
    
    // check for quick return
    if (L == 0.0 || rho == 0.0)  {
        return *theMatrix;
    }    
    
    double m = 0.5*rho*L;
    int numDOF2 = numDOF/2;
    for (int i = 0; i < dimension; i++)  {
        (*theMatrix)(i,i) = m;
        (*theMatrix)(i+numDOF2,i+numDOF2) = m;
    }
    
    return *theMatrix;
}


void EETruss::zeroLoad(void)
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
    if (L == 0.0 || rho == 0.0)  {
        return 0;
    }
    
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
    double m = 0.5*rho*L;
    for (int i=0; i<dimension; i++) {
        double val1 = Raccel1(i);
        double val2 = Raccel2(i);	
        
        // perform - fact * M*(R * accel) // remember M a diagonal matrix
        val1 *= -m;
        val2 *= -m;
        
        (*theLoad)(i) += val1;
        (*theLoad)(i+nodalDOF) += val2;
    }	
    
    return 0;
}


const Vector& EETruss::getResistingForce()
{	
    // zero the residual
    theVector->Zero();
    
    // get measured resisting forces
    q = theSite->getForce();
    
    if (iMod == true)  {
        // get measured displacements
        static Vector measDisp(1);
        measDisp = theSite->getDisp();
        
        // correct for displacement control errors using I-Modification
        q -= kbInit*(measDisp - targDisp);
    }
    
    // determine resisting forces in global system
    int numDOF2 = numDOF/2;
    for (int i = 0; i < dimension; i++)  {
        (*theVector)(i) = -cosX[i]*q(0);
        (*theVector)(i+numDOF2) = cosX[i]*q(0);
    }
    
    // subtract external load
    (*theVector) -= *theLoad;
    
    return *theVector;
}


const Vector& EETruss::getResistingForceIncInertia()
{	
    this->getResistingForce();
    
    // add the damping forces if rayleigh damping
    if (alphaM != 0.0 || betaK != 0.0 || betaK0 != 0.0 || betaKc != 0.0)
        (*theVector) += this->getRayleighDampingForces();
    
    // now include the mass portion
    if (L != 0.0 && rho != 0.0)  {
        const Vector &accel1 = theNodes[0]->getTrialAccel();
        const Vector &accel2 = theNodes[1]->getTrialAccel();	
        
        int numDOF2 = numDOF/2;
        double m = 0.5*rho*L;
        for (int i = 0; i < dimension; i++) {
            (*theVector)(i) += m * accel1(i);
            (*theVector)(i+numDOF2) += m * accel2(i);
        }
    }
    
    return *theVector;
}


int EETruss::sendSelf(int commitTag, Channel &theChannel)
{
    // has not been implemented yet.....
    return 0;
}


int EETruss::recvSelf(int commitTag, Channel &theChannel, FEM_ObjectBroker &theBroker)
{
    // has not been implemented yet.....
    return 0;
}


int EETruss::displaySelf(Renderer &theViewer, int displayMode, float fact)
{
    // first determine the end points of the element based on
    // the display factor (a measure of the distorted image)
    const Vector &end1Crd = theNodes[0]->getCrds();
    const Vector &end2Crd = theNodes[1]->getCrds();	
    
    const Vector &end1Disp = theNodes[0]->getDisp();
    const Vector &end2Disp = theNodes[1]->getDisp();
    
    static Vector v1(3);
    static Vector v2(3);
    
    for (int i = 0; i < 2; i++)  {
        v1(i) = end1Crd(i) + end1Disp(i)*fact;
        v2(i) = end2Crd(i) + end2Disp(i)*fact;    
    }
    
    return theViewer.drawLine (v1, v2, 1.0, 1.0);
}


void EETruss::Print(OPS_Stream &s, int flag)
{    
    if (flag == 0)  {
        // print everything
        s << "Element: " << this->getTag(); 
        s << "  type: EETruss  iNode: " << connectedExternalNodes(0);
        s << "  jNode: " << connectedExternalNodes(1) << endln;
        s << "  mass per unit length:  " << rho << endln;
        s << "  ExperimentalSite, tag: " << theSite->getTag() << endln;
    } else if (flag == 1)  {
        // does nothing
    }
}


Response* EETruss::setResponse(const char **argv, int argc,
    Information &eleInformation, OPS_Stream &output)
{
    Response *theResponse = 0;

    output.tag("ElementOutput");
    output.attr("eleType","EETruss");
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
        output.tag("ResponseType","q1");

        theResponse = new ElementResponse(this, 4, Vector(1));
    }
    // basic deformations
    else if (strcmp(argv[0],"deformation") == 0 || strcmp(argv[0],"deformations") == 0 || 
        strcmp(argv[0],"basicDeformation") == 0 || strcmp(argv[0],"basicDeformations") == 0)
    {
        output.tag("ResponseType","v1");

        theResponse = new ElementResponse(this, 5, Vector(1));
    }

    output.endTag(); // ElementOutput

    return theResponse;
}


int EETruss::getResponse(int responseID, Information &eleInformation)
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
            theVector->Zero();
            // Axial
            (*theVector)(0)        = -q(0);
            (*theVector)(numDOF/2) =  q(0);
            
            *(eleInformation.theVector) = *theVector;
        }
        return 0;      
        
    case 4:  // basic forces
        if (eleInformation.theVector != 0)  {
            *(eleInformation.theVector) = q;
        }
        return 0;      
        
    case 5:  // basic deformations
        if (eleInformation.theVector != 0)  {
            *(eleInformation.theVector) = targDisp;
        }
        return 0;      
        
    default:
        return 0;
    }
}
