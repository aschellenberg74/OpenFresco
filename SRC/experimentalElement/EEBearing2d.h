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
**   Andreas Schellenberg (andreas.schellenberg@gmail.com)            **
**   Yoshikazu Takahashi (yos@catfish.dpri.kyoto-u.ac.jp)             **
**   Gregory L. Fenves (fenves@berkeley.edu)                          **
**   Stephen A. Mahin (mahin@berkeley.edu)                            **
**                                                                    **
** ****************************************************************** */

#ifndef EEBearing2d_h
#define EEBearing2d_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 01/14
// Revision: A
//
// Description: This file contains the class definition for EEBearing2d.
// EEBearing2d is an experimental bearing element defined by two nodes.
// Experimental element objects are associated with experimental setup objects.

#include "ExperimentalElement.h"

class Channel;
class UniaxialMaterial;

#define ELE_TAG_EEBearing2d 9957


class EEBearing2d : public ExperimentalElement
{
public:
    // constructors
    EEBearing2d(int tag, int Nd1, int Nd2, 
        int pFrcCtrl, UniaxialMaterial **theMaterials,
        ExperimentalSite *site,
        const Vector y = 0, const Vector x = 0, const Vector Mratio = 0,
        double shearDistI = 0.5, bool iMod = false,
        int addRayleigh = 0, double mass = 0.0);
    EEBearing2d(int tag, int Nd1, int Nd2,
        int pFrcCtrl, UniaxialMaterial **theMaterials,
        int port, char *machineInetAddress = 0,
        int ssl = 0, int udp = 0, int dataSize = OF_Network_dataSize,
        const Vector y = 0, const Vector x = 0, const Vector Mratio = 0,
        double shearDistI = 0.5, bool iMod = false,
        int addRayleigh = 0, double mass = 0.0);
    
    // destructor
    ~EEBearing2d();
    
    // method to get class type
    const char *getClassType() const {return "EEBearing2d";};
    
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
    // private methods
    void setUp();
    void setTranGlobalLocal();
    void setTranLocalBasic();
    void addPDeltaForces(Vector &pLocal);
    void addPDeltaStiff(Matrix &kLocal);
    void applyIMod();
    
    // private attributes - a copy for each object of the class
    ID connectedExternalNodes;          // contains the tags of the end nodes
    Node *theNodes[2];                  // array of nodes
    UniaxialMaterial *theMaterials[2];  // array of uniaxial materials
    
    // parameters
    int pFrcCtrl;       // flag to switch to axial force control
    Vector x;           // local x direction
    Vector y;           // local y direction
    Vector Mratio;      // p-delta moment distribution ratios
    double shearDistI;  // shear distance from node I as fraction of length
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
    Vector *qb;         // trial forces in basic system
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
    
    Matrix kbInit;      // stiffness matrix in basic system
    Vector dbLast;      // displacements in basic system at last update
    double tLast;       // time at last update
    
    static Matrix theMatrix;    // a class wide Matrix
    static Vector theVector;    // a class wide Vector
    Vector theLoad;             // the load vector
    
    bool firstWarning;  // flag to warn about tangent stiffness
    bool onP0;          // flag to indicate if the element is on P0
};

#endif
