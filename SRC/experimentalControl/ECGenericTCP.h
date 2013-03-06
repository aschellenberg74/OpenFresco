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
// $URL$

#ifndef ECGenericTCP_h
#define ECGenericTCP_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 08/11
// Revision: A
//
// Description: This file contains the class definition for ECGenericTCP.
// ECGenericTCP is a controller class for communicating with a generic
// controller over a single TCP/IP connection.

#include "ExperimentalControl.h"

class Channel;

class ECGenericTCP : public ExperimentalControl
{
public:
    // constructors
    ECGenericTCP(int tag, char *ipAddress, int ipPort,
        ID ctrlModes, ID daqModes, char *initFileName = 0,
        int ssl = 0, int udp = 0);
    ECGenericTCP(const ECGenericTCP &ec);
    
    // destructor
    virtual ~ECGenericTCP();
    
    // method to get class type
    const char *getClassType() const {return "ECGenericTCP";};
    
    // public methods to set and to get response
    virtual int setup();
    virtual int setSize(ID sizeT, ID sizeO);
    
    virtual int setTrialResponse(const Vector* disp, 
        const Vector* vel,
        const Vector* accel,
        const Vector* force,
        const Vector* time);
    virtual int getDaqResponse(Vector* disp,
        Vector* vel,
        Vector* accel,
        Vector* force,
        Vector* time);
    
    virtual int commitState();
    
    virtual ExperimentalControl *getCopy();
    
    // public methods for experimental control recorder
    virtual Response *setResponse(const char **argv, int argc,
        OPS_Stream &output);
    virtual int getResponse(int responseID, Information &info);
    
    // public methods for output
    void Print(OPS_Stream &s, int flag = 0);    
    
protected:
    // protected methods to set and to get response
    virtual int control();
    virtual int acquire();
    
private:
    void sleep(const clock_t wait);
    
    char *ipAddress;            // ip address
    int ipPort;                 // ip port
    const int dataSize;         // data size of network transactions
    
    Channel *theChannel;        // channel
    double *sData;              // send data array
    Vector *sendData;           // send vector
    double *rData;              // receive data array
    Vector *recvData;           // receive vector

    ID ctrlModes;               // id with control modes (size = 5)
    ID daqModes;                // id with daq modes (size = 5)

    char *initFileName;         // parameter initialization file
    
    Vector *ctrlDisp, *ctrlVel, *ctrlAccel, *ctrlForce, *ctrlTime;
    Vector *daqDisp, *daqVel, *daqAccel, *daqForce, *daqTime;
};

#endif
