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

#ifndef EETruss_h
#define EETruss_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 09/06
// Revision: A
//
// Description: This file contains the class definition for EETruss.
// EETruss is an experimental truss element defined by two nodes.
// Experimental element objects are associated with experimental setup objects.
// This EETruss element will work in 1d, 2d or 3d problems.

#include "ExperimentalElement.h"

class Channel;

#define ELE_TAG_EETruss 9950


class EETruss : public ExperimentalElement
{
public:
    // constructors
    EETruss(int tag, int dimension, int Nd1, int Nd2,
        ExperimentalSite *site,
        ExperimentalTangentStiff *tangStiff = 0,
        bool iMod = false, int addRayleigh = 1,
        double rho = 0.0, bool cMass = false);
    EETruss(int tag, int dimension, int Nd1, int Nd2,
        int port, char *machineInetAddress = 0,
        int ssl = 0, int udp = 0, int dataSize = OF_Network_dataSize,
        ExperimentalTangentStiff *tangStiff = 0,
        bool iMod = false, int addRayleigh = 1,
        double rho = 0.0, bool cMass = false);
    
    // destructor
    ~EETruss();
    
    // method to get class type
    const char *getClassType() const {return "EETruss";};
    
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
    
protected:
    
private:
    // private attributes - a copy for each object of the class
    int numDIM;                     // truss in 1d, 2d or 3d domain
    int numDOF;                     // number of dof for truss
    ID  connectedExternalNodes;     // contains the tags of the end nodes
    
    bool iMod;          // I-Modification flag
    int addRayleigh;    // flag to add Rayleigh damping
    double rho;         // rho: mass per unit length
    bool cMass;         // consistent mass flag
    double L;           // undeformed element length
    double cosX[3];     // direction cosines for transformation
    
    Matrix *theMatrix;  // pointer to objects matrix (a class wide Matrix)
    Vector *theVector;  // pointer to objects vector (a class wide Vector)
    Vector *theLoad;    // pointer to the load vector
    
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
    
    Matrix kb;          // tangent stiffness matrix in basic system
    Matrix kbInit;      // initial stiffness matrix in basic system
    Matrix kbLast;      // tangent stiffness matrix in basic system at last update
    
    Vector dbLast;      // displacements in basic system at last update
    Vector dbDaqLast;   // daq disp in basic system at last update
    Vector qbDaqLast;   // daq force in basic system at last update
    double tLast;       // time at last update
    
    Node *theNodes[2];  // array of node pointers
    
    bool firstWarning;
    
    // static data - single copy for all objects of the class
    static Matrix EETrussM2;   // class wide matrix for 2*2
    static Matrix EETrussM4;   // class wide matrix for 4*4
    static Matrix EETrussM6;   // class wide matrix for 6*6
    static Matrix EETrussM12;  // class wide matrix for 12*12
    static Vector EETrussV2;   // class wide Vector for size 2
    static Vector EETrussV4;   // class wide Vector for size 4
    static Vector EETrussV6;   // class wide Vector for size 6
    static Vector EETrussV12;  // class wide Vector for size 12
};

#endif
