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

// $Revision: $
// $Date: $
// $URL: $

#ifndef ECSimFEAdapter_h
#define ECSimFEAdapter_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 09/07
// Revision: A
//
// Description: This file contains the class definition for ECSimFEAdapter.
// ECSimFEAdapter is a controller class for communicating with an adapter
// element of an other FE-software over a single TCP/IP connection.

#include "ECSimulation.h"

class Channel;

class ECSimFEAdapter : public ECSimulation
{
public:
    // constructors
    ECSimFEAdapter(int tag, char *ipAddress, int ipPort = 44000);
    ECSimFEAdapter(const ECSimFEAdapter &ec);
    
    // destructor
    virtual ~ECSimFEAdapter();
    
    // public methods to set and to get response
    virtual int setSize(ID sizeT, ID sizeO);
    virtual int setup();
    
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

    Vector *targDisp, *targForce;
    Vector *measDisp, *measForce;
};

#endif
