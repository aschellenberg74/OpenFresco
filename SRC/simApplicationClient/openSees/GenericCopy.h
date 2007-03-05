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

// $Revision: $
// $Date: $
// $URL: $

#ifndef GenericCopy_h
#define GenericCopy_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 11/06
// Revision: A
//
// Description: This file contains the class definition for GenericCopy.
// GenericCopy is a generic element defined by any number of nodes and 
// the degrees of freedom at those nodes. The element communicates with 
// OpenFresco trough a tcp/ip connection.

#include <Element.h>
#include <Matrix.h>
#include <FrescoGlobals.h>

#define ELE_TAG_GenericCopy 9956


class GenericCopy : public Element
{
public:
    // constructors
    GenericCopy(int tag, ID nodes, int srcTag);
    
    // destructor
    ~GenericCopy();
    
    // method to get class type
    const char *getClassType(void) const {return "GenericCopy";};

    // public methods to obtain information about dof & connectivity    
    int getNumExternalNodes(void) const;
    const ID &getExternalNodes(void);
    Node **getNodePtrs(void);
    int getNumDOF(void);
    void setDomain(Domain *theDomain);
    
    // public methods to set the state of the element    
    int commitState(void);
    int revertToLastCommit(void);        
    int revertToStart(void);
    int update(void);
    
    // public methods to obtain stiffness, mass, damping and residual information    
    const Matrix &getTangentStiff(void);
    const Matrix &getInitialStiff(void);
    //const Matrix &getDamp(void);
    const Matrix &getMass(void);
    
    void zeroLoad(void);
    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);
    const Vector &getResistingForce(void);
    const Vector &getResistingForceIncInertia(void);
    
    // public methods for element output
    int sendSelf(int commitTag, Channel &theChannel);
    int recvSelf(int commitTag, Channel &theChannel, FEM_ObjectBroker &theBroker);
    int displaySelf(Renderer &theViewer, int displayMode, float fact);    
    void Print(OPS_Stream &s, int flag = 0);    
    
    Response *setResponse(const char **argv, int argc, OPS_Stream &s);
    int getResponse(int responseID, Information &eleInformation);
    
protected:
    
private:
    // private attributes - a copy for each object of the class
    ID connectedExternalNodes;      // contains the tags of the end nodes

    int numExternalNodes;
    int numDOF;

    int srcTag;
    Element *theSource;
        
    static Matrix theMatrix;
    static Matrix theInitStiff;
    static Matrix theMass;
    static Vector theVector;
    static Vector theLoad;

    bool initStiffFlag;
    bool massFlag;
        
    Node **theNodes;
};

#endif
