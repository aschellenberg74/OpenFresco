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

#ifndef EEBeamColumn2d_h
#define EEBeamColumn2d_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 09/06
// Revision: A
//
// Description: This file contains the class definition for EEBeamColumn2d.
// EEBeamColumn2d is an experimental beam-column element defined by two nodes.
// Experimental element objects are associated with experimental setup objects.

#include <ExperimentalElement.h>

class CrdTransf2d;

#define ELE_TAG_EEBeamColumn2d 9951


class EEBeamColumn2d : public ExperimentalElement
{
public:
    // constructor
    EEBeamColumn2d(int tag, int Nd1, int Nd2,
        ExperimentalSite *site,
        CrdTransf2d &coordTransf,
        bool iMod = false, bool isCopy = false,
        double rho = 0.0);
    
    // destructor
    ~EEBeamColumn2d();
    
    // method to get class type
    const char *getClassType(void) const {return "EEBeamColumn2d";};

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
    ID connectedExternalNodes;      // contains the tags of the end nodes
    CrdTransf2d *theCoordTransf;
    
    bool iMod;      // I-Modification flag
    bool isCopy;    // element copy flag
    double rho;     // rho: mass per unit length
    double L;       // undeformed element length
    
    static Matrix theMatrix;
    static Vector theVector;
    static Vector theLoad;
    
    Vector q;           // forces in basic system
    Vector targDisp;    // displacements in basic system
    Matrix kbInit;      // stiffness matrix in basic system
    double q0[3];       // fixed end forces in basic system
    double p0[3];       // reactions in basic system
    
    Matrix T;           // transformation matrix
    Matrix Tinv;        // inverse of transformation matrix
    
    Node *theNodes[2];
    
    FILE *outFile;
};

#endif
