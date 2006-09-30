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
// Description: This file contains the implementation of the EEChevronBrace2d class.

#include "EEChevronBrace2d.h"

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
Matrix EEChevronBrace2d::theMatrix(9,9);
Vector EEChevronBrace2d::theVector(9);
Vector EEChevronBrace2d::theLoad(9);


// responsible for allocating the necessary space needed by each object
// and storing the tags of the ExperimentalBeamColumn end nodes.
EEChevronBrace2d::EEChevronBrace2d(int tag, int Nd1, int Nd2, int Nd3,
    ExperimentalSite *site,
    bool iM, bool copy, bool nlGeomFlag, double r1, double r2)
    : ExperimentalElement(tag, ELE_TAG_EEChevronBrace2d, site),
    connectedExternalNodes(3),
    iMod(iM), isCopy(copy), nlFlag(nlGeomFlag), rho1(r1), rho2(r2),
    L1(0.0), L2(0.0), q(6), targDisp(3), kbInit(3,3), T(3,9)
{
    // ensure the connectedExternalNode ID is of correct size & set values
    if (connectedExternalNodes.Size() != 3)  {
        opserr << "EEChevronBrace2d::EEChevronBrace2d() - element: "
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
    
    (*sizeCtrl)[OF_Resp_Disp] = 3;
    (*sizeCtrl)[OF_Resp_Vel] = 3;
    (*sizeCtrl)[OF_Resp_Accel] = 3;
    (*sizeCtrl)[OF_Resp_Time] = 1;
    
    (*sizeDaq)[OF_Resp_Disp] = 3;
    (*sizeDaq)[OF_Resp_Force] = 6;
    
    if (isCopy == false)
        theSite->setSize(*sizeCtrl, *sizeDaq);
    
    // set the initial stiffness matrix size
    theInitStif.resize(9,9);
    
    // initialize vectors
    q.Zero();
    targDisp.Zero();
    
    // open output file
    //outFile = fopen("elemDisp.out","w");
    //if (outFile==NULL)  {
    //	opserr << "EEChevronBrace2d::EEChevronBrace2d() - "
    //		<< "fopen: could not open output file\n";
    //  exit(-1);
    //}
}


// delete must be invoked on any objects created by the object
// and on the experimental object.
EEChevronBrace2d::~EEChevronBrace2d()
{
    // close output file
    //fclose(outFile);
    
    // invoke the destructor on any objects created by the object
    // that the object still holds a pointer to
}


int EEChevronBrace2d::getNumExternalNodes(void) const
{
    return 3;
}


const ID& EEChevronBrace2d::getExternalNodes(void) 
{
    return connectedExternalNodes;
}


Node** EEChevronBrace2d::getNodePtrs(void) 
{
    return theNodes;
}


int EEChevronBrace2d::getNumDOF(void) 
{
    return 9;
}


int EEChevronBrace2d::getNumBasicDOF(void) 
{
    return 3;
}


// to set a link to the enclosing Domain and to set the node pointers.
// also determines the number of dof associated
// with the ExperimentalBeamColumn element, we set matrix and 
// vector pointers, allocate space for t matrix and define it 
// as the basic deformation-displacement transformation matrix.
void EEChevronBrace2d::setDomain(Domain *theDomain)
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
            opserr << "WARNING EEChevronBrace2d::setDomain() - Nd1: " 
                << Nd1 << " does not exist in the model for ";
        } else if (!theNodes[1])  {
            opserr << "WARNING EEChevronBrace2d::setDomain() - Nd2: " 
                << Nd2 << " does not exist in the model for ";
        } else  {
            opserr << "WARNING EEChevronBrace2d::setDomain() - Nd3: " 
                << Nd3 << " does not exist in the model for ";
        }
        opserr << "EEChevronBrace2d ele: " << this->getTag() << endln;
        
        return;
    }
    
    // now determine the number of dof and the dimension    
    int dofNd1 = theNodes[0]->getNumberDOF();
    int dofNd2 = theNodes[1]->getNumberDOF();	
    int dofNd3 = theNodes[2]->getNumberDOF();	
    
    // if differing dof at the ends - print a warning message
    if (dofNd1 != 3)  {
        opserr << "EEChevronBrace2d::setDomain() - node 1: "
            << connectedExternalNodes(0) << " has incorrect number of DOF (not 3)\n";
        return;
    }
    if (dofNd2 != 3)  {
        opserr << "EEChevronBrace2d::setDomain() - node 2: "
            << connectedExternalNodes(1) << " has incorrect number of DOF (not 3)\n";
        return;
    }
    if (dofNd3 != 3)  {
        opserr << "EEChevronBrace2d::setDomain() - node 3: "
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
        opserr <<"EEChevronBrace2d::setDomain() - element: "
            << this->getTag() << " has diagonal 1 with zero length\n";
        return;
    }
    if (L2 == 0.0)  {
        opserr <<"EEChevronBrace2d::setDomain() - element: "
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


int EEChevronBrace2d::commitState()
{
    int rValue = 0;
    
    if (isCopy == false)
        rValue += theSite->commitState();
    
    return rValue;
}


int EEChevronBrace2d::update(void)
{
    // get current time
    Vector time(1);
    Domain *theDomain = this->getDomain();
    time(0) = theDomain->getCurrentTime();
    
    // linear geometry
    if (nlFlag == false)  {
        // determine global displacements
        const Vector &dsp1 = theNodes[0]->getTrialDisp();
        const Vector &dsp2 = theNodes[1]->getTrialDisp();
        const Vector &dsp3 = theNodes[2]->getTrialDisp();
        
        const Vector &vel1 = theNodes[0]->getTrialVel();
        const Vector &vel2 = theNodes[1]->getTrialVel();
        const Vector &vel3 = theNodes[2]->getTrialVel();
        
        const Vector &acc1 = theNodes[0]->getTrialAccel();
        const Vector &acc2 = theNodes[1]->getTrialAccel();
        const Vector &acc3 = theNodes[2]->getTrialAccel();
        
        static Vector dg(9), vg(9), ag(9);
        for (int i = 0; i < 3; i++)  {
            dg(i)   = dsp1(i);  vg(i)   = vel1(i);  ag(i)   = acc1(i);
            dg(i+3) = dsp2(i);  vg(i+3) = vel2(i);  ag(i+3) = acc2(i);
            dg(i+6) = dsp3(i);  vg(i+6) = vel3(i);  ag(i+6) = acc3(i);
        }
        
        // transform displacements from the global to the basic system
        static Vector db(3), vb(3), ab(3);
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
        
        // nonlinear geometry
    } else  {
        // not implemented yet
    }
    
    return 0;
}


int EEChevronBrace2d::setInitialStiff(const Matrix& kbinit)
{
    kbInit = kbinit;
    
    if (kbInit.noRows() != 3 || kbInit.noCols() != 3)  {
        opserr << "EEChevronBrace2d::setInitialStiff() - " 
            << "matrix size is incorrect for element: "
            << this->getTag() << endln;
        return -1;
    }
    
    // transform stiffness matrix from the basic to the global system
    theInitStif.Zero();
    theInitStif.addMatrixTripleProduct(0.0, T, kbInit, 1.0);
    
    return 0;
}


const Matrix& EEChevronBrace2d::getMass(void)
{
    // zero the matrix
    theMatrix.Zero();
    
    // check for quick return
    if ((L1 == 0.0 && rho2 == 0) || (L2 == 0 && rho1 == 0.0))  {
        return theMatrix;
    }    
    
    double m1 = 0.5*rho1*L1;
    double m2 = 0.5*rho2*L2;
    theMatrix(0,0) = m1;
    theMatrix(1,1) = m1;	
    theMatrix(3,3) = m2;
    theMatrix(4,4) = m2;
    theMatrix(6,6) = m1+m2;
    theMatrix(7,7) = m1+m2;
    
    return theMatrix; 
}


void EEChevronBrace2d::zeroLoad(void)
{
    theLoad.Zero();
}


int EEChevronBrace2d::addLoad(ElementalLoad *theLoad, double loadFactor)
{  
    opserr <<"EEChevronBrace2d::addLoad() - "
        << "load type unknown for element: "
        << this->getTag() << endln;
    
    return -1;
}


int EEChevronBrace2d::addInertiaLoadToUnbalance(const Vector &accel)
{
    // check for quick return
    if ((L1 == 0.0 && rho2 == 0) || (L2 == 0 && rho1 == 0.0))  {
        return 0;
    }    
    
    // get R * accel from the nodes
    const Vector &Raccel1 = theNodes[0]->getRV(accel);
    const Vector &Raccel2 = theNodes[1]->getRV(accel);
    const Vector &Raccel3 = theNodes[2]->getRV(accel);
    
    if (3 != Raccel1.Size() || 3 != Raccel2.Size() || 3 != Raccel3.Size())  {
        opserr << "EEChevronBrace2d::addInertiaLoadToUnbalance() - "
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


const Vector& EEChevronBrace2d::getResistingForce()
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
    
    // zero the residual
    theVector.Zero();
    
    // determine resisting forces in basic system
    q = theSite->getForce();
    
    if (iMod == true)  {
        // get measured displacements
        static Vector measDisp(3);
        measDisp = theSite->getDisp();
                
        // correct for displacement control errors using I-Modification
        static Vector qb;
        qb = kbInit*(measDisp - targDisp);
        
        //double ratioX1 = (q(0)+q(3)!=0) ? q(0)/(q(0) + q(3)) : 0.5;
        //double ratioX2 = (q(0)+q(3)!=0) ? q(3)/(q(0) + q(3)) : 0.5;
        //double ratioY1 = (q(1)+q(4)!=0) ? q(1)/(q(1) + q(4)) : 0.5;
        //double ratioY2 = (q(1)+q(4)!=0) ? q(4)/(q(1) + q(4)) : 0.5;        
        //q(0) += qb(0)*ratioX1;
        //q(1) += qb(1)*ratioY1;
        //q(3) += qb(0)*ratioX2;
        //q(4) += qb(1)*ratioY2;
        
        q(0) += qb(0)/2 + qb(1)/2*dx1[0]/dx1[1];
        q(1) += qb(0)/2*dx1[1]/dx1[0] + qb(1)/2;
        q(3) += qb(0)/2 + qb(1)/2*dx2[0]/dx2[1];
        q(4) += qb(0)/2*dx2[1]/dx2[0] + qb(1)/2;
    }
    
    // determine resisting forces in global system and account for load-cell
    // cross-talk by averaging between shear and axial loads
    theVector(0) = 0.5*(q(0) + dx1[0]/dx1[1]*q(1));
    theVector(1) = 0.5*(dx1[1]/dx1[0]*q(0) + q(1));
    //theVector(2) = q(2);    // with moments
    theVector(2) = 0;       // w/o moments
    theVector(3) = 0.5*(q(3) + dx2[0]/dx2[1]*q(4));
    theVector(4) = 0.5*(dx2[1]/dx2[0]*q(3) + q(4));
    //theVector(5) = q(5);    // with moments
    theVector(5) = 0;       // w/o moments
    theVector(6) = -theVector(0) - theVector(3);
    theVector(7) = -theVector(1) - theVector(4);
    //theVector(8) = -theVector(0)*dx1[1] + theVector(1)*dx1[0] - theVector(2) - theVector(3)*dx2[1] + theVector(4)*dx2[0] - theVector(5);    // with moments
    theVector(8) = 0;       // w/o moments
    
    // subtract external load
    theVector.addVector(1.0, theLoad, -1.0);
    
    return theVector;
}


const Vector& EEChevronBrace2d::getResistingForceIncInertia()
{	
    theVector = this->getResistingForce();
    
    // add the damping forces if rayleigh damping
    if (alphaM != 0.0 || betaK != 0.0 || betaK0 != 0.0 || betaKc != 0.0)
        theVector += this->getRayleighDampingForces();
    
    // now include the mass portion
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
        theVector(6) += (m1+m2) * accel2(0);    
        theVector(7) += (m1+m2) * accel2(1);
    }
    
    return theVector;
}


int EEChevronBrace2d::sendSelf(int commitTag, Channel &theChannel)
{
    // has not been implemented yet.....
    return 0;
}


int EEChevronBrace2d::recvSelf(int commitTag, Channel &theChannel,
                               FEM_ObjectBroker &theBroker)
{
    // has not been implemented yet.....
    return 0;
}


int EEChevronBrace2d::displaySelf(Renderer &theViewer,
                                  int displayMode, float fact)
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
    
    const Vector &end1Disp = theNodes[0]->getDisp();
    const Vector &end2Disp = theNodes[1]->getDisp();
    const Vector &end3Disp = theNodes[2]->getDisp();
    
    static Matrix coords(3,3);
    
    for (int i = 0; i < 2; i++)  {
        coords(0,i) = end1Crd(i) + end1Disp(i)*fact;
        coords(1,i) = end2Crd(i) + end2Disp(i)*fact;    
        coords(2,i) = end3Crd(i) + end3Disp(i)*fact;    
    }
    
    return theViewer.drawPolygon (coords, values);
}


void EEChevronBrace2d::Print(OPS_Stream &s, int flag)
{
    if (flag == 0)  {
        // print everything
        s << "Element: " << this->getTag(); 
        s << "  type: EEChevronBrace2d  iNode: " << connectedExternalNodes(0);
        s << "  jNode: " << connectedExternalNodes(1); 
        s << "  kNode: " << connectedExternalNodes(2) << endln;
        s << "  mass per unit length diagonal 1:  " << rho1 << endln;
        s << "  mass per unit length diagonal 2:  " << rho2 << endln;
        s << "  ExperimentalSite, tag: " << theSite->getTag() << endln;
    } else if (flag == 1)  {
        // does nothing
    }
}


Response* EEChevronBrace2d::setResponse(const char **argv, int argc,
    Information &eleInformation, OPS_Stream &output)
{
    Response *theResponse = 0;

    output.tag("ElementOutput");
    output.attr("eleType","EEChevronBrace2d");
    output.attr("eleTag",this->getTag());
    output.attr("node1",connectedExternalNodes[0]);
    output.attr("node2",connectedExternalNodes[1]);

    // global forces
    if (strcmp(argv[0],"force") == 0 || strcmp(argv[0],"forces") == 0 ||
        strcmp(argv[0],"globalForce") == 0 || strcmp(argv[0],"globalForces") == 0)
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

        theResponse = new ElementResponse(this, 2, theVector);
    }
    // local forces
    else if (strcmp(argv[0],"localForce") == 0 || strcmp(argv[0],"localForces") == 0)
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

        theResponse = new ElementResponse(this, 3, theVector);
    }
    // basic forces
    else if (strcmp(argv[0],"basicForce") == 0 || strcmp(argv[0],"basicForces") == 0)
    {
        output.tag("ResponseType","q1");
        output.tag("ResponseType","q2");
        output.tag("ResponseType","q3");
        output.tag("ResponseType","q4");
        output.tag("ResponseType","q5");
        output.tag("ResponseType","q6");

        theResponse = new ElementResponse(this, 4, Vector(6));
    }
    // basic deformations
    else if (strcmp(argv[0],"deformation") == 0 || strcmp(argv[0],"deformations") == 0 || 
        strcmp(argv[0],"basicDeformation") == 0 || strcmp(argv[0],"basicDeformations") == 0)
    {
        output.tag("ResponseType","v1");
        output.tag("ResponseType","v2");
        output.tag("ResponseType","v3");

        theResponse = new ElementResponse(this, 5, Vector(3));
    }

    output.endTag(); // ElementOutput

    return theResponse;
}


int EEChevronBrace2d::getResponse(int responseID, Information &eleInformation)
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
        return -1;
    }
}
