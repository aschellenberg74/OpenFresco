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

#ifndef EEBeamColumn3d_h
#define EEBeamColumn3d_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 09/06
// Revision: A
//
// Description: This file contains the class definition for EEBeamColumn3d.
// EEBeamColumn3d is an experimental beam-column element defined by two nodes.
// Experimental element objects are associated with experimental setup objects.

#include "ExperimentalElement.h"

class CrdTransf;
class Channel;

#define ELE_TAG_EEBeamColumn3d 9952


class EEBeamColumn3d : public ExperimentalElement
{
public:
    // constructors
    EEBeamColumn3d(int tag, int Nd1, int Nd2,
        CrdTransf &coordTransf,
        ExperimentalSite *site,
        bool iMod = false, int addRayleigh = 1,
        double rho = 0.0, bool cMass = false);
    EEBeamColumn3d(int tag, int Nd1, int Nd2,
        CrdTransf &coordTransf,
        int port, char *machineInetAddress = 0,
        int ssl = 0, int udp = 0,
        int dataSize = OF_Network_dataSize,
        bool iMod = false, int addRayleigh = 1,
        double rho = 0.0, bool cMass = false);
    
    // destructor
    ~EEBeamColumn3d();
    
    // method to get class type
    const char *getClassType() const {return "EEBeamColumn3d";};
    
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
    int setInitialStiff(const Matrix& stiff);
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
    void applyIMod();
    
    // private attributes - a copy for each object of the class
    ID connectedExternalNodes;      // contains the tags of the end nodes
    CrdTransf *theCoordTransf;
    
    bool iMod;          // I-Modification flag
    int addRayleigh;    // flag to add Rayleigh damping
    double rho;         // rho: mass per unit length
    bool cMass;         // consistent mass flag
    int nlGeo;          // nonlinear geometry flag
    double L;           // undeformed element length
    
    static Matrix theMatrix;  // a class wide Matrix
    static Vector theVector;  // a class wide Vector
    Vector theLoad;
    
    Channel *theChannel;    // channel
    double *sData;          // send data array
    Vector *sendData;       // send vector
    double *rData;          // receive data array
    Vector *recvData;       // receive vector
    
    Vector *db;         // displacements in basic system B
    Vector *vb;         // velocities in basic system B
    Vector *ab;         // accelerations in basic system B
    Vector *t;          // time
    
    Vector *dbDaq;      // daq displacements in basic system B
    Vector *vbDaq;      // daq velocities in basic system B
    Vector *abDaq;      // daq accelerations in basic system B
    Vector *qbDaq;      // daq forces in basic system B
    Vector *tDaq;       // daq time
    
    Vector dbCtrl;      // ctrl displacements in basic system B
    Vector vbCtrl;      // ctrl velocities in basic system B
    Vector abCtrl;      // ctrl accelerations in basic system B
    
    Matrix kbInit;      // stiffness matrix in basic system B
    Vector dbLast;      // displacements in basic system B at last update
    double tLast;       // time at last update
    double qA0[6];      // fixed end forces in basic system A
    double pA0[6];      // reactions in basic system A
    
    Node *theNodes[2];
    
    bool firstWarning;
};

#endif
