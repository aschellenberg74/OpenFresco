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

#ifndef EEBeamColumn3d_h
#define EEBeamColumn3d_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 09/06
// Revision: A
//
// Description: This file contains the class definition for EEBeamColumn3d.
// EEBeamColumn3d is an experimental beam-column element defined by two nodes.
// Experimental element objects are associated with experimental setup objects.

#include <ExperimentalElement.h>

class CrdTransf3d;

#define ELE_TAG_EEBeamColumn3d 9952


class EEBeamColumn3d : public ExperimentalElement
{
public:
    // constructors
    EEBeamColumn3d(int tag, int Nd1, int Nd2,
        CrdTransf3d &coordTransf,
        ExperimentalSite *site,
        bool iMod = false, double rho = 0.0);
    EEBeamColumn3d(int tag, int Nd1, int Nd2,
        CrdTransf3d &coordTransf,
        int port, char *machineInetAddress = 0,
        int dataSize = OF_Network_dataSize,
        bool iMod = false, double rho = 0.0);
    
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
    
    Response *setResponse(const char **argv, int argc, OPS_Stream &s);
    int getResponse(int responseID, Information &eleInformation);
    
protected:
    
private:
    // private attributes - a copy for each object of the class
    ID connectedExternalNodes;      // contains the tags of the end nodes
    CrdTransf3d *theCoordTransf;
    
    bool iMod;		// I-Modification flag
    double rho;		// rho: mass per unit length
    double L;		// undeformed element length
    
    static Matrix theMatrix;
    static Vector theVector;
    static Vector theLoad;
    
    TCP_Socket *theSocket;      // tcp/ip socket
    double *sData;              // send data array
    Vector *sendData;           // send vector
    double *rData;              // receive data array
    Vector *recvData;           // receive vector

    Vector *db;         // displacements in basic system B
    Vector *vb;         // velocities in basic system B
    Vector *ab;         // accelerations in basic system B
    Vector *t;          // time

    Vector *dbMeas;     // measured displacements in basic system B
    Vector *vbMeas;     // measured velocities in basic system B
    Vector *abMeas;     // measured accelerations in basic system B
    Vector *qMeas;      // measured forces in basic system B
    Vector *tMeas;      // measured time

    Vector dbTarg;      // target displacements in basic system B
    Vector vbTarg;      // target velocities in basic system B
    Vector abTarg;      // target accelerations in basic system B
    
    Vector dbPast;      // displacements for recorder in basic system B
    Matrix kbInit;      // stiffness matrix in basic system B
    double qA0[6];      // fixed end forces in basic system A
    double pA0[6];      // reactions in basic system A
    
    Matrix T;           // transformation matrix
    Matrix Tinv;        // inverse of transformation matrix
    
    Node *theNodes[2];
};

#endif