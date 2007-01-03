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

#ifndef EEZeroLength_h
#define EEZeroLength_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 09/06
// Revision: A
//
// Description: This file contains the class definition for EEZeroLength.
// EEZeroLength is an experimental element defined by two nodes with the same coordinates.
// Experimental element objects are associated with experimental setup objects.
// This EEZeroLength element will work in 1d, 2d or 3d problems.

#include <ExperimentalElement.h>

// Type of dimension of element NxDy has dimension x=1,2,3 and
// y=2,4,6,12 degrees-of-freedom for the element
#ifndef ZeroLength_h
    enum Etype { D1N2, D2N4, D2N6, D3N6, D3N12 };
#endif

#define ELE_TAG_EEZeroLength 9953


class EEZeroLength : public ExperimentalElement
{
public:
    // constructors
    EEZeroLength(int tag, int dimension, int Nd1, int Nd2,
        const ID &direction,
        const Vector &x, const Vector &yprime,
        ExperimentalSite *site,
        bool iMod = false, double mass = 0.0);
    EEZeroLength(int tag, int dimension, int Nd1, int Nd2,
        const ID &direction,
        const Vector &x, const Vector &yprime,
        int port, char *machineInetAddress = 0,
        int dataSize = OF_Network_dataSize,
        bool iMod = false, double mass = 0.0);

    // destructor
    ~EEZeroLength();
    
    // method to get class type
    const char *getClassType() const {return "EEZeroLength";};

    // public methods to obtain information about dof & connectivity    
    int getNumExternalNodes() const;
    const ID &getExternalNodes();
    Node **getNodePtrs();
    int getNumDOF();
    int getNumBasicDOF();
    void setDomain(Domain *theDomain);
    
    // public methods to set the state of the element
    int commitState();
    int update();
    
    // public methods to set and to obtain stiffness,
    // and to obtain mass, damping and residual information
    int setInitialStiff(const Matrix& kbInit);
    const Matrix &getMass();
    
    void zeroLoad();
    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);
    const Vector &getResistingForce();
    const Vector &getResistingForceIncInertia();
    
    // public methods to obtain the daq response in global system
    const Vector &getTime();

    // public methods to obtain the daq response in basic system
    const Vector &getBasicDisp();
    const Vector &getBasicVel();
    const Vector &getBasicAccel();

    // public methods for element output
    int sendSelf(int commitTag, Channel &theChannel);
    int recvSelf(int commitTag, Channel &theChannel, FEM_ObjectBroker &theBroker);
    int displaySelf(Renderer &theViewer, int displayMode, float fact);    
    void Print(OPS_Stream &s, int flag = 0);    
    
    Response *setResponse(const char **argv, int argc,
        Information &eleInformation,  OPS_Stream &s);
    int getResponse(int responseID, Information &eleInformation);
    
private:
    Etype elemType;
    
    // private methods
    void setUp(int Nd1, int Nd2, const Vector& x, const Vector& y);    
    void setTranGlobalBasic(Etype e, int n);
    
    // private attributes - a copy for each object of the class
    int dimension;					// 1, 2, or 3 dimensions
    int numDOF;						// number of dof for EEZeroLength
    ID connectedExternalNodes;      // contains the tags of the end nodes
    
    int numDir;             // number of directions
    ID *dir;                // array of directions 0-5
    Matrix transformation;  // transformation matrix for orientation
    
    bool iMod;          // I-Modification flag
    double mass;        // total mass
    
    Matrix *theMatrix;  // pointer to objects matrix (a class wide Matrix)
    Vector *theVector;  // pointer to objects vector (a class wide Vector)
    Vector *theLoad;    // pointer to the load vector
    
    TCP_Socket *theSocket;      // tcp/ip socket
    double *sData;              // send data array
    Vector *sendData;           // send vector
    double *rData;              // receive data array
    Vector *recvData;           // receive vector

    Vector *db;         // trial displacements in basic system
    Vector *vb;         // trial velocities in basic system
    Vector *ab;         // trial accelerations in basic system
    Vector *t;          // trial time

    Vector *dbMeas;     // measured displacements in basic system
    Vector *vbMeas;     // measured velocities in basic system
    Vector *abMeas;     // measured accelerations in basic system
    Vector *qMeas;      // measured forces in basic system
    Vector *tMeas;      // measured time

    Vector dbTarg;      // target displacements in basic system
    Vector dbPast;      // past displacements in basic system
    Matrix kbInit;      // stiffness matrix in basic system
    
    Matrix T;           // transformation matrix
    
    Node *theNodes[2];
    
    // static data - single copy for all objects of the class	
    static Matrix EEZeroLengthM2;   // class wide matrix for 2*2
    static Matrix EEZeroLengthM4;   // class wide matrix for 4*4
    static Matrix EEZeroLengthM6;   // class wide matrix for 6*6
    static Matrix EEZeroLengthM12;  // class wide matrix for 12*12
    static Vector EEZeroLengthV2;   // class wide Vector for size 2
    static Vector EEZeroLengthV4;   // class wide Vector for size 4
    static Vector EEZeroLengthV6;   // class wide Vector for size 6
    static Vector EEZeroLengthV12;  // class wide Vector for size 12
};

#endif




