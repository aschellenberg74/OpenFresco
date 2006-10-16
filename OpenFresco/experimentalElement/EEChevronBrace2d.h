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

#ifndef EEChevronBrace2d_h
#define EEChevronBrace2d_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 09/06
// Revision: A
//
// Description: This file contains the class definition for EEChevronBrace2d.
// EEChevronBrace2d is an experimental chevron-brace element defined by three nodes.
// Experimental element objects are associated with experimental setup objects.

#include <ExperimentalElement.h>

#define ELE_TAG_EEChevronBrace2d 9954


class EEChevronBrace2d : public ExperimentalElement
{
public:
    // constructor
    EEChevronBrace2d(int tag, int Nd1, int Nd2, int Nd3,
        ExperimentalSite *site,
        bool iMod = false, bool isCopy = false,
        bool nlGeomFlag = false,
        double rho1 = 0.0, double rho2 = 0.0);
    
    // destructor
    ~EEChevronBrace2d();
    
    // method to get class type
    const char *getClassType(void) const {return "EEChevronBrace2d";};

    // public methods to obtain information about dof & connectivity    
    int getNumExternalNodes(void) const;
    const ID &getExternalNodes(void);
    Node **getNodePtrs(void);
    int getNumDOF(void);
    int getNumBasicDOF(void);
    void setDomain(Domain *theDomain);
    
    // public methods to set the state of the element    
    int commitState(void);
    int update(void);
    
    // public methods to set and to obtain stiffness, 
    // and to obtain mass, damping and residual information    
    int setInitialStiff(const Matrix& stiff);
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
    
    Response *setResponse(const char **argv, int argc,
        Information &eleInformation, OPS_Stream &s);
    int getResponse(int responseID, Information &eleInformation);
    
protected:
    
private:
    // private attributes - a copy for each object of the class
    ID  connectedExternalNodes;   // contains the tags of the end nodes
    
    bool iMod;          // I-Modification flag
    bool isCopy;        // element copy flag
    bool nlFlag;        // non-linear geometry flag
    double rho1, rho2;  // masses per unit length
    double L1, L2;      // undeformed length of trusses
    
    static Matrix theMatrix;
    static Vector theVector;
    static Vector theLoad;
    
    Vector q;           // forces in local system
    Vector targDisp;    // displacements in basic system
    Matrix kbInit;      // stiffness matrix in basic system
    
    double dx1[2], dx2[2];  // element projections
    Matrix T;               // transformation matrix
    
    Node *theNodes[3];
};

#endif
