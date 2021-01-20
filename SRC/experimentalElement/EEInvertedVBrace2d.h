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

#ifndef EEInvertedVBrace2d_h
#define EEInvertedVBrace2d_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 09/06
// Revision: A
//
// Description: This file contains the class definition for EEInvertedVBrace2d.
// EEInvertedVBrace2d is an experimental inverted-V brace element defined by three nodes.
// Experimental element objects are associated with experimental setup objects.

#include "ExperimentalElement.h"

class Channel;

#define ELE_TAG_EEInvertedVBrace2d 9954


class EEInvertedVBrace2d : public ExperimentalElement
{
public:
    // constructors
    EEInvertedVBrace2d(int tag, int Nd1, int Nd2, int Nd3,
        ExperimentalSite *site,
        bool iMod = false, bool nlGeom = false,
        int addRayleigh = 1, double rho1 = 0.0, double rho2 = 0.0);
    EEInvertedVBrace2d(int tag, int Nd1, int Nd2, int Nd3,
        int port, char *machineInetAddress = 0,
        int ssl = 0, int udp = 0, int dataSize = OF_Network_dataSize,
        bool iMod = false, bool nlGeom = false, int addRayleigh = 1,
        double rho1 = 0.0, double rho2 = 0.0);
    
    // destructor
    ~EEInvertedVBrace2d();
    
    // method to get class type
    const char *getClassType() const {return "EEInvertedVBrace2d";};
    
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
    
protected:
    
private:
    // private attributes - a copy for each object of the class
    ID  connectedExternalNodes;   // contains the tags of the end nodes
    
    bool iMod;          // I-Modification flag
    bool nlGeom;        // non-linear geometry flag
    int addRayleigh;    // flag to add Rayleigh damping
    double rho1, rho2;  // masses per unit length
    double L1, L2;      // undeformed length of trusses
    
    static Matrix theMatrix;  // a class wide Matrix
    static Vector theVector;  // a class wide Vector
    Vector theLoad;
    
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
    Vector *qDaq;       // daq forces in local system
    Vector *tDaq;       // daq time
    
    Vector dbCtrl;      // ctrl displacements in basic system
    Vector vbCtrl;      // ctrl velocities in basic system
    Vector abCtrl;      // ctrl accelerations in basic system
    
    double dx1[2], dx2[2];  // element projections
    Matrix T;               // transformation matrix
    
    Matrix kbInit;      // stiffness matrix in basic system
    Vector dbLast;      // displacements in basic system at last update
    double tLast;       // time at last update
    
    Node *theNodes[3];
};

#endif
