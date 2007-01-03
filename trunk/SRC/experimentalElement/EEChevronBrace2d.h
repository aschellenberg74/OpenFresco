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
    // constructors
    EEChevronBrace2d(int tag, int Nd1, int Nd2, int Nd3,
        ExperimentalSite *site,
        bool iMod = false, bool nlGeomFlag = false,
        double rho1 = 0.0, double rho2 = 0.0);
    EEChevronBrace2d(int tag, int Nd1, int Nd2, int Nd3,
        int port, char *machineInetAddress = 0,
        int dataSize = OF_Network_dataSize,
        bool iMod = false, bool nlGeomFlag = false,
        double rho1 = 0.0, double rho2 = 0.0);
    
    // destructor
    ~EEChevronBrace2d();
    
    // method to get class type
    const char *getClassType() const {return "EEChevronBrace2d";};

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
    
    Response *setResponse(const char **argv, int argc,
        Information &eleInformation, OPS_Stream &s);
    int getResponse(int responseID, Information &eleInformation);
    
protected:
    
private:
    // private attributes - a copy for each object of the class
    ID  connectedExternalNodes;   // contains the tags of the end nodes
    
    bool iMod;          // I-Modification flag
    bool nlFlag;        // non-linear geometry flag
    double rho1, rho2;  // masses per unit length
    double L1, L2;      // undeformed length of trusses
    
    static Matrix theMatrix;
    static Vector theVector;
    static Vector theLoad;
    
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
    Vector *qMeas;      // measured forces in local system
    Vector *tMeas;      // measured time

    Vector dbTarg;      // target displacements in basic system
    Vector dbPast;      // past displacements in basic system
    Matrix kbInit;      // stiffness matrix in basic system
    
    double dx1[2], dx2[2];  // element projections
    Matrix T;               // transformation matrix
    
    Node *theNodes[3];
};

#endif
