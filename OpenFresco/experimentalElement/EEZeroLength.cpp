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
// Description: This file contains the implementation of the EEZeroLength class.

#include "EEZeroLength.h"

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


// initialise the class wide variables
Matrix EEZeroLength::EEZeroLengthM2(2,2);
Matrix EEZeroLength::EEZeroLengthM4(4,4);
Matrix EEZeroLength::EEZeroLengthM6(6,6);
Matrix EEZeroLength::EEZeroLengthM12(12,12);
Vector EEZeroLength::EEZeroLengthV2(2);
Vector EEZeroLength::EEZeroLengthV4(4);
Vector EEZeroLength::EEZeroLengthV6(6);
Vector EEZeroLength::EEZeroLengthV12(12);


// responsible for allocating the necessary space needed by each object
// and storing the tags of the EEZeroLength end nodes
EEZeroLength::EEZeroLength(int tag, int dim, int Nd1, int Nd2, 
    ExperimentalSite *site,
    const ID &direction,
    const Vector &x, const Vector &yp,
    bool iM, bool copy, double m)
    : ExperimentalElement(tag, ELE_TAG_EEZeroLength, site),     
    dimension(dim), numDOF(0),
    connectedExternalNodes(2),
    numDir(direction.Size()), dir(0), transformation(3,3),
    iMod(iM), isCopy(copy), mass(m),
    theMatrix(0), theVector(0), theLoad(0),
    q(direction.Size()), targDisp(direction.Size()),
    kbInit(direction.Size(),direction.Size()), T(0,0)
{
    // establish the connected nodes and set up the transformation matrix for orientation
    this->setUp(Nd1, Nd2, x, yp);
    
    // allocate memory for direction array
    dir = new ID(numDir);
    
    if (dir == 0)  {
        opserr << "EEZeroLength::EEZeroLength() - failed to creat direction array\n";
        exit(-1);
    }
    
    // initialize directions and check for valid values
    (*dir) = direction;
    for (int i=0; i<numDir; i++)  {
        if ((*dir)(i) < 0 || (*dir)(i) > 5)  {
            opserr << "EEZeroLength::EEZeroLength() - incorrect direction "
                << (*dir)(i) << " is set to 0\n";
            (*dir)(i) = 0;
        }
    }
    
    // set the data size for the experimental site
    sizeCtrl = new ID(OF_Resp_All);
    sizeDaq = new ID(OF_Resp_All);
    
    (*sizeCtrl)[OF_Resp_Disp] = numDir;
    (*sizeCtrl)[OF_Resp_Vel] = numDir;
    (*sizeCtrl)[OF_Resp_Accel] = numDir;
    (*sizeCtrl)[OF_Resp_Time] = 1;
    
    (*sizeDaq)[OF_Resp_Disp] = numDir;
    (*sizeDaq)[OF_Resp_Force] = numDir;
    
    if (isCopy == false)
        theSite->setSize(*sizeCtrl, *sizeDaq);
    
    // initialize vectors
    q.Zero();
    targDisp.Zero();
}


// delete must be invoked on any objects created by the object
// and on the experimental object
EEZeroLength::~EEZeroLength()
{
    // invoke the destructor on any objects created by the object
    // that the object still holds a pointer to
    if (dir)
        delete dir;
    if (theLoad)
        delete theLoad;
}


int EEZeroLength::getNumExternalNodes(void) const
{
    return 2;
}


const ID& EEZeroLength::getExternalNodes(void) 
{
    return connectedExternalNodes;
}


Node** EEZeroLength::getNodePtrs(void) 
{
    return theNodes;
}


int EEZeroLength::getNumDOF(void) 
{
    return numDOF;
}


int EEZeroLength::getNumBasicDOF(void) 
{
    return numDir;
}


// to set a link to the enclosing Domain and to set the node pointers
// also determines the number of dof associated
// with the EEZeroLength element, we set matrix and vector pointers,
// allocate space for t matrix and define it as the basic deformation-
// displacement transformation matrix.
void EEZeroLength::setDomain(Domain *theDomain)
{
    // check Domain is not null - invoked when object removed from a domain
    if (theDomain == 0)  {
        theNodes[0] = 0;
        theNodes[1] = 0;
        
        return;
    }
    
    // set default values for error conditions
    numDOF = 2;
    theMatrix = &EEZeroLengthM2;
    theVector = &EEZeroLengthV2;
    
    // first set the node pointers
    int Nd1 = connectedExternalNodes(0);
    int Nd2 = connectedExternalNodes(1);
    theNodes[0] = theDomain->getNode(Nd1);
    theNodes[1] = theDomain->getNode(Nd2);	
    
    // if can't find both - send a warning message
    if (!theNodes[0] || !theNodes[1])  {
        if (!theNodes[0])  { 
            opserr << "EEZeroLength::setDomain() - Nd1: "
                << Nd1 << " does not exist in the model for ";
        } else  {
            opserr << "EEZeroLength::setDomain() - Nd2: " 
                << Nd2 << " does not exist in the model for ";
        }
        opserr << "EEZeroLength ele: " << this->getTag() << endln;
        
        return;
    }
    
    // now determine the number of dof and the dimension    
    int dofNd1 = theNodes[0]->getNumberDOF();
    int dofNd2 = theNodes[1]->getNumberDOF();	
    
    // if differing dof at the ends - print a warning message
    if (dofNd1 != dofNd2)  {
        opserr << "EEZeroLength::setDomain(): nodes " << Nd1 << " and " << Nd2
            << "have differing dof at ends for element: " << this->getTag() << endln;
        return;
    }	
    
    // Check that length is zero within tolerance
    const Vector &end1Crd = theNodes[0]->getCrds();
    const Vector &end2Crd = theNodes[1]->getCrds();	
    Vector diff = end1Crd - end2Crd;
    double L  = diff.Norm();
    double v1 = end1Crd.Norm();
    double v2 = end2Crd.Norm();
    double vm;
    
    vm = (v1<v2) ? v2 : v1;
    
    if (L > LENTOL*vm)  {
        opserr << "EEZeroLength::setDomain(): element " << this->getTag()
            << " has L = " << L << ", which is greater than the tolerance\n";
    }
    
    // call the base class method
    this->DomainComponent::setDomain(theDomain);
    
    // now set the number of dof for element and set matrix and vector pointer
    if (dimension == 1 && dofNd1 == 1)  {
        numDOF = 2;    
        theMatrix = &EEZeroLengthM2;
        theVector = &EEZeroLengthV2;
        elemType  = D1N2;
    }
    else if (dimension == 2 && dofNd1 == 2)  {
        numDOF = 4;
        theMatrix = &EEZeroLengthM4;
        theVector = &EEZeroLengthV4;
        elemType  = D2N4;
    }
    else if (dimension == 2 && dofNd1 == 3)  {
        numDOF = 6;	
        theMatrix = &EEZeroLengthM6;
        theVector = &EEZeroLengthV6;
        elemType  = D2N6;
    }
    else if (dimension == 3 && dofNd1 == 3)  {
        numDOF = 6;	
        theMatrix = &EEZeroLengthM6;
        theVector = &EEZeroLengthV6;
        elemType  = D3N6;
    }
    else if (dimension == 3 && dofNd1 == 6)  {
        numDOF = 12;	    
        theMatrix = &EEZeroLengthM12;
        theVector = &EEZeroLengthV12;
        elemType  = D3N12;
    }
    else  {
        opserr << "EEZeroLength::setDomain() can not handle "
            << dimension << "dofs at nodes in " << dofNd1 << " d problem\n";
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
        opserr << "EEZeroLength::setDomain() - element: " << this->getTag()
            << " out of memory creating vector of size: " << numDOF << endln;
        return;
    }          
    
    // set transformation matrix from global to basic
    if (numDir > 0)
        this->setTranGlobalBasic(elemType, numDir);
}   	 


int EEZeroLength::commitState()
{
    int rValue = 0;
    
    if (isCopy == false)
        rValue += theSite->commitState();
    
    return rValue;
}


int EEZeroLength::update(void)
{
    // get current time
    Vector time(1);
    Domain *theDomain = this->getDomain();
    time(0) = theDomain->getCurrentTime();
    
    // get global trial response
    const Vector &dsp1 = theNodes[0]->getTrialDisp();
    const Vector &dsp2 = theNodes[1]->getTrialDisp();
    const Vector &vel1 = theNodes[0]->getTrialVel();
    const Vector &vel2 = theNodes[1]->getTrialVel();
    const Vector &acc1 = theNodes[0]->getTrialAccel();
    const Vector &acc2 = theNodes[1]->getTrialAccel();
    
    int i;
    static Vector dg(numDOF), vg(numDOF), ag(numDOF);
    for (i = 0; i < numDOF/2; i++)  {
        dg(i)          = dsp1(i);  vg(i)          = vel1(i);  ag(i)          = acc1(i);
        dg(i+numDOF/2) = dsp2(i);  vg(i+numDOF/2) = vel2(i);  ag(i+numDOF/2) = acc2(i);
    }
    
    // transform response from the global to the basic system
    static Vector db(numDir), vb(numDir), ab(numDir);
    db = T*dg;
    vb = T*vg;
    ab = T*ag;
    
    if (db != targDisp)  {
        // save the target displacement
        targDisp = db;
        // set the trial response at the site
        if (isCopy == false)
            theSite->setTrialResponse(&db, &vb, &ab, (Vector*)0, &time);
    }
    
    return 0;
}


int EEZeroLength::setInitialStiff(const Matrix& kbinit)
{
    if (kbinit.noRows() != numDir || kbinit.noCols() != numDir)  {
        opserr << "EEZeroLength::setInitialStiff() - " 
            << "matrix size is incorrect for element: "
            << this->getTag() << endln;
        return -1;
    }
    kbInit = kbinit;
        
    // transform stiffness matrix from the basic to the global system
    theInitStif.Zero();
    theInitStif.addMatrixTripleProduct(0.0, T, kbInit, 1.0);
    
    return 0;
}


const Matrix& EEZeroLength::getMass(void)
{
    // zero the matrix
    theMatrix->Zero();
    
    // form mass matrix
    if (mass != 0.0)  {
        double m = 0.5*mass;
        int numDOF2 = numDOF/2;
        for (int i = 0; i < dimension; i++)  {
            (*theMatrix)(i,i) = m;
            (*theMatrix)(i+numDOF2,i+numDOF2) = m;
        }
    }
    
    return *theMatrix; 
}


void EEZeroLength::zeroLoad(void)
{
    theLoad->Zero();
}


int EEZeroLength::addLoad(ElementalLoad *theLoad, double loadFactor)
{
    opserr <<"EEZeroLength::addLoad() - "
        << "load type unknown for element: "
        << this->getTag() << endln;
    
    return -1;
}


int EEZeroLength::addInertiaLoadToUnbalance(const Vector &accel)
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
        opserr << "EEZeroLength::addInertiaLoadToUnbalance() - "
            << "matrix and vector sizes are incompatible\n";
        return -1;
    }
    
    // want to add ( - fact * M R * accel ) to unbalance
    // take advantage of lumped mass matrix
    double m = 0.5*mass;
    for (int i = 0; i < dimension; i++)  {
        (*theLoad)(i) -= m * Raccel1(i);
        (*theLoad)(i+numDOF2) -= m * Raccel2(i);
    }
    
    return 0;
}


const Vector& EEZeroLength::getResistingForce()
{
    // zero the residual
    theVector->Zero();
    
    // get measured resisting forces
    q = theSite->getForce();
    
    if (iMod == true)  {
        // get measured displacements
        static Vector measDisp(numDir);
        measDisp = theSite->getDisp();
        
        // correct for displacement control errors using I-Modification
        q -= kbInit*(measDisp - targDisp);
    }
    
    // determine resisting forces in global system
    (*theVector) = T^q;
    
    // subtract external load
    (*theVector) -= *theLoad;
    
    return *theVector;
}


const Vector& EEZeroLength::getResistingForceIncInertia()
{	
    this->getResistingForce();
    
    // add the damping forces if rayleigh damping
    if (alphaM != 0.0 || betaK != 0.0 || betaK0 != 0.0 || betaKc != 0.0)
        (*theVector) += this->getRayleighDampingForces();
    
    // now include the mass portion
    if (mass != 0.0)  {
        const Vector &accel1 = theNodes[0]->getTrialAccel();
        const Vector &accel2 = theNodes[1]->getTrialAccel();    
        
        int numDOF2 = numDOF/2;
        double m = 0.5*mass;
        for (int i = 0; i < dimension; i++)  {
            (*theVector)(i) += m * accel1(i);
            (*theVector)(i+numDOF2) += m * accel2(i);
        }
    }
    
    return *theVector;
}


int EEZeroLength::sendSelf(int commitTag, Channel &theChannel)
{
    // has not been implemented yet.....
    return 0;
}


int EEZeroLength::recvSelf(int commitTag, Channel &theChannel,
    FEM_ObjectBroker &theBroker)
{
    // has not been implemented yet.....
    return 0;
}


int EEZeroLength::displaySelf(Renderer &theViewer,
    int displayMode, float fact)
{    
    // first determine the end points of the element based on
    // the display factor (a measure of the distorted image)
    const Vector &end1Crd = theNodes[0]->getCrds();
    const Vector &end2Crd = theNodes[1]->getCrds();
    
    const Vector &end1Disp = theNodes[0]->getDisp();
    const Vector &end2Disp = theNodes[1]->getDisp();    
    
    static Vector v1(3);
    static Vector v2(3);
    
    for (int i=0; i<dimension; i++) {
        v1(i) = end1Crd(i) + end1Disp(i)*fact;
        v2(i) = end2Crd(i) + end2Disp(i)*fact;    
    }
    
    return theViewer.drawLine (v1, v2, 1.0, 1.0);
}


void EEZeroLength::Print(OPS_Stream &s, int flag)
{
    if (flag == 0)  {
        // print everything
        s << "Element: " << this->getTag(); 
        s << "  type: EEZeroLength  iNode: " << connectedExternalNodes(0);
        s << "  jNode: " << connectedExternalNodes(1) << endln;
        s << "  mass:  " << mass << endln;
        s << "  ExperimentalSite, tag: " << theSite->getTag() << endln;
    } else if (flag == 1)  {
        // does nothing
    }
}


Response* EEZeroLength::setResponse(const char **argv, int argc,
    Information &eleInformation, OPS_Stream &output)
{
    Response *theResponse = 0;

    output.tag("ElementOutput");
    output.attr("eleType","EEZeroLength");
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
        for (int i=0; i<numDir; i++)  {
            sprintf(outputData,"q%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 4, Vector(numDir));
    }
    // basic deformations
    else if (strcmp(argv[0],"deformation") == 0 || strcmp(argv[0],"deformations") == 0 || 
        strcmp(argv[0],"basicDeformation") == 0 || strcmp(argv[0],"basicDeformations") == 0)
    {
        for (int i=0; i<numDir; i++)  {
            sprintf(outputData,"v%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 5, Vector(numDir));
    }
    // basic deformations and basic forces
    else if (strcmp(argv[0],"defoANDforce") == 0 || strcmp(argv[0],"deformationANDforces") == 0 ||
        strcmp(argv[0],"deformationsANDforces") == 0)
    {
        int i;
        for (i=0; i<numDir; i++)  {
            sprintf(outputData,"v%d",i+1);
            output.tag("ResponseType",outputData);
        }
        for (i=0; i<numDir; i++)  {
            sprintf(outputData,"q%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ElementResponse(this, 6, Vector(numDir*2));
    }

    output.endTag(); // ElementOutput

    return theResponse;
}


int EEZeroLength::getResponse(int responseID, Information &eleInformation)
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
            int numDOF2 = numDOF/2;
            for (int i = 0; i < numDir; i++) {
                (*theVector)((*dir)(i))         = -q(i);
                (*theVector)((*dir)(i)+numDOF2) =  q(i);
            }
            
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
        
    case 6:  // basic deformations and basic forces
        if (eleInformation.theVector != 0) {
            int i;
            for(i=0; i<numDir; i++) {
                (*(eleInformation.theVector))(i) = targDisp(i);
            }
            for(i=0; i<numDir; i++) {
                (*(eleInformation.theVector))(i+numDir) = q(i);
            }
        }
        return 0;
        
    default:
        return 0;
    }
}


// Establish the external nodes and set up the transformation matrix for orientation
void EEZeroLength::setUp(int Nd1, int Nd2, const Vector &x, const Vector &yp)
{ 
    // ensure the connectedExternalNode ID is of correct size & set values
    if (connectedExternalNodes.Size() != 2)  {
        opserr << "EEZeroLength::setUp() - element: "
            << this->getTag() << " failed to create an ID of size 2\n";
    }
    
    connectedExternalNodes(0) = Nd1;
    connectedExternalNodes(1) = Nd2;
    
    // set node pointers to NULL
    int i;
    for (i=0; i<2; i++)
        theNodes[i] = 0;
    
    // check that vectors for orientation are of correct size
    if ( x.Size() != 3 || yp.Size() != 3 )  {
        opserr << "EEZeroLength::setUp() - element: "
            << this->getTag() << " incorrect dimension of orientation vectors\n";
    }
    
    // establish orientation of element for the tranformation matrix
    // z = x cross yp
    Vector z(3);
    z(0) = x(1)*yp(2) - x(2)*yp(1);
    z(1) = x(2)*yp(0) - x(0)*yp(2);
    z(2) = x(0)*yp(1) - x(1)*yp(0);
    
    // y = z cross x
    Vector y(3);
    y(0) = z(1)*x(2) - z(2)*x(1);
    y(1) = z(2)*x(0) - z(0)*x(2);
    y(2) = z(0)*x(1) - z(1)*x(0);
    
    // compute length(norm) of vectors
    double xn = x.Norm();
    double yn = y.Norm();
    double zn = z.Norm();
    
    // check valid x and y vectors, i.e. not parallel and of zero length
    if (xn == 0 || yn == 0 || zn == 0)  {
        opserr << "EEZeroLength::setUp() - element: "
            << this->getTag() << " invalid orientation vectors\n";
    }
    
    // create transformation matrix of direction cosines
    for (i=0; i<3; i++)  {
        transformation(0,i) = x(i)/xn;
        transformation(1,i) = y(i)/yn;
        transformation(2,i) = z(i)/zn;
    }    
}


// set basic deformation-displacement transformation matrix
void EEZeroLength::setTranGlobalBasic(Etype elemType, int numDir)
{
    enum Dtype { TRANS, ROTATE };
    
    int axisID, dirID;
    Dtype dirType;
    
    // resize transformation matrix and zero it
    T.resize(numDir,numDOF);
    T.Zero();
    
    // loop over directions, setting row in tran for each direction
    // depending on dimensionality of element
    for (int i=0; i<numDir; i++)  {
        
        dirID  = (*dir)(i);     // direction 0 to 5;
        axisID = dirID % 3;     // 0, 1, 2 for axis of translation or rotation
        
        // set direction type to translation or rotation
        dirType = (dirID<3) ? TRANS : ROTATE;
        
        // now switch on dimensionality of element
        switch (elemType)  {
            
        case D1N2:
            if (dirType == TRANS)
                T(i,1) = transformation(axisID,0);
            break;
            
        case D2N4:
            if (dirType == TRANS)  {
                T(i,2) = transformation(axisID,0);  
                T(i,3) = transformation(axisID,1);
            }
            break;
            
        case D2N6: 
            if (dirType == TRANS)  {
                T(i,3) = transformation(axisID,0);  
                T(i,4) = transformation(axisID,1);
                T(i,5) = 0.0;
            } else if (dirType == ROTATE)  {
                T(i,3) = 0.0;
                T(i,4) = 0.0;
                T(i,5) = transformation(axisID,2);
            }
            break;
            
        case D3N6:
            if (dirType == TRANS)  {
                T(i,3) = transformation(axisID,0);  
                T(i,4) = transformation(axisID,1);
                T(i,5) = transformation(axisID,2);
            }
            break;
            
        case D3N12:
            if (dirType == TRANS)  {
                T(i,6)  = transformation(axisID,0);  
                T(i,7)  = transformation(axisID,1);
                T(i,8)  = transformation(axisID,2);
                T(i,9)  = 0.0;
                T(i,10) = 0.0;
                T(i,11) = 0.0;
            } else if (dirType == ROTATE)  {
                T(i,6)  = 0.0;
                T(i,7)  = 0.0;
                T(i,8)  = 0.0;
                T(i,9)  = transformation(axisID,0);
                T(i,10) = transformation(axisID,1);
                T(i,11) = transformation(axisID,2);
            }
            break;
            
        }  // end switch
        
        // fill in first half of transformation matrix with negative sign
        for (int j=0; j < numDOF/2; j++ )
            T(i,j) = -T(i,j+numDOF/2);   
    }
}
