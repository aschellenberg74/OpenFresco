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

#ifndef ECSimSimulink_h
#define ECSimSimulink_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 09/09
// Revision: A
//
// Description: This file contains the class definition for ECSimSimulink.
// ECSimSimulink is a controller class for communicating with a Simulink
// model of the entire control system over a single TCP/IP connection.

#include "ECSimulation.h"

class Channel;

class ECSimSimulink : public ECSimulation
{
public:
    // constructors
    ECSimSimulink(int tag, char *ipAddress, int ipPort = 44000);
    ECSimSimulink(const ECSimSimulink &ec);
    
    // destructor
    virtual ~ECSimSimulink();
    
    // method to get class type
    const char *getClassType() const {return "ECSimSimulink";};
    
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
    
    double newTarget, switchPC, atTarget;
    Vector *ctrlDisp, *ctrlForce, *ctrlTime;
    Vector *daqDisp, *daqForce, *daqTime;
};

#endif
