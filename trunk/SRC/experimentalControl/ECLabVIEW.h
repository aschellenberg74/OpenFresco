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

#ifndef ECLabVIEW_h
#define ECLabVIEW_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 01/07
// Revision: A
//
// Description: This file contains the class definition for ECLabVIEW.
// ECLabVIEW is a controller class for communicating with LabVIEW
// using ASCII over a single TCP/IP connection.

#include "ExperimentalControl.h"

class ECLabVIEW : public ExperimentalControl
{
public:
    // constructors
    ECLabVIEW(int tag, int nTrialCPs, ExperimentalCP **trialCPs,
        int nOutCPs, ExperimentalCP **outCPs,
        char *ipAddress, int ipPort = 44000);
    ECLabVIEW(const ECLabVIEW &ec);
    
    // destructor
    virtual ~ECLabVIEW();
    
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
    
    int numTrialCPs;            // number of trial control points
    ExperimentalCP **trialCPs;  // trial control points
    int numOutCPs;              // number of output control points
    ExperimentalCP **outCPs;    // output control points
    char *ipAddress;            // ip address
    int ipPort;                 // ip port

    TCP_Socket *theSocket;      // tcp/ip socket
    char *sData;                // send data array
    Message *sendData;          // send vector
    char *rData;                // receive data array
    Message *recvData;          // receive vector

    Vector *targDisp, *targForce;
    Vector *measDisp, *measForce;
    
    time_t rawtime;
    struct tm *ptm;
    char OPFTransactionID[30];
    char errMsg[80];

    FILE *logFile;
};

#endif
