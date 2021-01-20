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

#ifndef EEGeneric_h
#define EEGeneric_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 10/06
// Revision: A
//
// Description: This file contains the class definition for EEGeneric.
// EEGeneric is a generic experimental element defined by any number
// of nodes and the degrees of freedom at those nodes. Experimental 
// element objects are associated with experimental setup objects.

#include "ExperimentalElement.h"

class Channel;

#define ELE_TAG_EEGeneric 9955


class EEGeneric : public ExperimentalElement
{
public:
    // constructors
    EEGeneric(int tag, ID nodes, ID *dof,
        ExperimentalSite *site,
        bool iMod = false, int addRayleigh = 1, const Matrix *mass = 0,
        int checkTime = 0);
    EEGeneric(int tag, ID nodes, ID *dof,
        int port, char *machineInetAddress = 0,
        int ssl = 0, int udp = 0, int dataSize = OF_Network_dataSize,
        bool iMod = false, int addRayleigh = 1, const Matrix *mass = 0,
        int checkTime = 0);
    
    // destructor
    ~EEGeneric();
    
    // method to get class type
    const char *getClassType() const {return "EEGeneric";};
    
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
    ID connectedExternalNodes;  // contains the tags of the end nodes
    ID *theDOF;                 // array with the dof of the end nodes
    ID basicDOF;                // contains the basic dof
    
    int numExternalNodes;       // number of external nodes
    int numDOF;                 // number of total DOF
    int numBasicDOF;            // number of used DOF
    
    bool iMod;                  // I-Modification flag
    int addRayleigh;            // flag to add Rayleigh damping
    Matrix *mass;               // mass matrix
    int checkTime;              // flag to check time when setting trial
    
    Matrix theMatrix;           // objects matrix
    Vector theVector;           // objects vector
    Vector theLoad;             // load vector
    
    Channel *theChannel;        // channel
    double *sData;              // send data array
    Vector *sendData;           // send vector
    double *rData;              // receive data array
    Vector *recvData;           // receive vector
    
    Vector *db;         // trial displacements in basic system
    Vector *vb;         // trial velocities in basic system
    Vector *ab;         // trial accelerations in basic system
    Vector *t;          // trial time
    
    Vector *dbDaq;      // daq displacements in basic system
    Vector *vbDaq;      // daq velocities in basic system
    Vector *abDaq;      // daq accelerations in basic system
    Vector *qDaq;       // daq forces in basic system
    Vector *tDaq;       // daq time
    
    Vector dbCtrl;      // ctrl displacements in basic system
    Vector vbCtrl;      // ctrl velocities in basic system
    Vector abCtrl;      // ctrl accelerations in basic system
    
    Matrix kbInit;      // stiffness matrix in basic system
    Vector dbLast;      // displacements in basic system at last update
    double tLast;       // time at last update
    
    Node **theNodes;
};

#endif
