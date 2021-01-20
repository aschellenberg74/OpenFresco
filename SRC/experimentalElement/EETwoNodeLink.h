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

#ifndef EETwoNodeLink_h
#define EETwoNodeLink_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 09/06
// Revision: A
//
// Description: This file contains the class definition for EETwoNodeLink.
// EETwoNodeLink is an experimental element defined by two nodes.
// Experimental element objects are associated with experimental setup objects.
// This EETwoNodeLink element will work in 1d, 2d or 3d problems.

#include "ExperimentalElement.h"

class Channel;

// Type of dimension of element NxDy has dimension x=1,2,3 and
// y=2,4,6,12 degrees-of-freedom for the element
#ifndef ZeroLength_h
    enum Etype { D1N2, D2N4, D2N6, D3N6, D3N12 };
#endif

#define ELE_TAG_EETwoNodeLink 9953


class EETwoNodeLink : public ExperimentalElement
{
public:
    // constructors
    EETwoNodeLink(int tag, int dimension, int Nd1, int Nd2,
        const ID &direction, ExperimentalSite *site,
        ExperimentalTangentStiff *tangStiff = 0,
        const Vector y = 0, const Vector x = 0, const Vector Mratio = 0,
        const Vector shearDistI = 0, bool iMod = false,
        int addRayleigh = 1, double mass = 0.0);
    EETwoNodeLink(int tag, int dimension, int Nd1, int Nd2,
        const ID &direction, int port, char *machineInetAddress = 0,
        int ssl = 0, int udp = 0, int dataSize = OF_Network_dataSize,
        ExperimentalTangentStiff *tangStiff = 0,
        const Vector y = 0, const Vector x = 0, const Vector Mratio = 0,
        const Vector shearDistI = 0, bool iMod = false,
        int addRayleigh = 1, double mass = 0.0);
    
    // destructor
    ~EETwoNodeLink();
    
    // method to get class type
    const char *getClassType() const {return "EETwoNodeLink";};
    
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
    const Matrix &getTangentStiff();
    const Matrix &getDamp();
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
    const Vector &getBasicForce();
    
    // public methods for element output
    int sendSelf(int commitTag, Channel &theChannel);
    int recvSelf(int commitTag, Channel &theChannel,
        FEM_ObjectBroker &theBroker);
    int displaySelf(Renderer &theViewer, int displayMode,
        float fact, const char **modes = 0, int numModes = 0);
    void Print(OPS_Stream &s, int flag = 0);
    
    // public methods for element recorder
    Response *setResponse(const char **argv, int argc, OPS_Stream &s);
    int getResponse(int responseID, Information &eleInfo);
    
private:
    Etype elemType;
    
    // private methods
    void setUp();
    void setTranGlobalLocal();
    void setTranLocalBasic();
    void addPDeltaForces(Vector &pLocal);
    void addPDeltaStiff(Matrix &kLocal);
    
    // private attributes - a copy for each object of the class
    int dimension;                  // 1, 2, or 3 dimensions
    int numDOF;                     // number of dof for EETwoNodeLink
    ID connectedExternalNodes;      // contains the tags of the end nodes
    Node *theNodes[2];
    
    // parameters
    int numDir;         // number of directions
    ID *dir;            // array of directions 0-5
    Matrix trans;       // transformation matrix for orientation
    Vector x;           // local x direction
    Vector y;           // local y direction
    Vector Mratio;      // p-delta moment distribution ratios
    Vector shearDistI;  // shear distance from node I as fraction of length
    bool iMod;          // I-Modification flag
    int addRayleigh;    // flag to add Rayleigh damping
    double mass;        // total mass
    double L;           // element length
    
    Channel *theChannel;    // channel
    double *sData;          // send data array
    Vector *sendData;       // send vector
    double *rData;          // receive data array
    Vector *recvData;       // receive vector
    
    Vector *db;         // trial displacements in basic system
    Vector *vb;         // trial velocities in basic system
    Vector *ab;         // trial accelerations in basic system
    Vector *t;          // trial time
    
    Vector *dbDaq;      // daq displacements in basic system
    Vector *vbDaq;      // daq velocities in basic system
    Vector *abDaq;      // daq accelerations in basic system
    Vector *qbDaq;      // daq forces in basic system
    Vector *tDaq;       // daq time
    
    Vector dbCtrl;      // ctrl displacements in basic system
    Vector vbCtrl;      // ctrl velocities in basic system
    Vector abCtrl;      // ctrl accelerations in basic system
    
    Vector dl;          // displacements in local system
    Matrix Tgl;         // transformation matrix from global to local system
    Matrix Tlb;         // transformation matrix from local to basic system
    
    Matrix kb;          // tangent stiffness matrix in basic system
    Matrix kbInit;      // initial stiffness matrix in basic system
    Matrix kbLast;      // tangent stiffness matrix in basic system at last update
    
    Vector dbLast;      // displacements in basic system at last update
    Vector dbDaqLast;   // daq disp in basic system at last update
    Vector qbDaqLast;   // daq force in basic system at last update
    double tLast;       // time at last update
    
    Matrix *theMatrix;  // pointer to objects matrix (a class wide Matrix)
    Vector *theVector;  // pointer to objects vector (a class wide Vector)
    Vector *theLoad;    // pointer to the load vector
    
    // static data - single copy for all objects of the class
    static Matrix EETwoNodeLinkM2;   // class wide matrix for 2*2
    static Matrix EETwoNodeLinkM4;   // class wide matrix for 4*4
    static Matrix EETwoNodeLinkM6;   // class wide matrix for 6*6
    static Matrix EETwoNodeLinkM12;  // class wide matrix for 12*12
    static Vector EETwoNodeLinkV2;   // class wide Vector for size 2
    static Vector EETwoNodeLinkV4;   // class wide Vector for size 4
    static Vector EETwoNodeLinkV6;   // class wide Vector for size 6
    static Vector EETwoNodeLinkV12;  // class wide Vector for size 12
    
    bool firstWarning;
};

#endif
