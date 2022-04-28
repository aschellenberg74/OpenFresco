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

#ifndef ECLabVIEW_h
#define ECLabVIEW_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 01/07
// Revision: A
//
// Description: This file contains the class definition for ECLabVIEW.
// ECLabVIEW is a controller class for communicating with LabVIEW
// using ASCII over a single TCP/IP connection.

#include "ExperimentalControl.h"

#include <time.h>

class ExperimentalCP;
class TCP_Socket;
class Message;

class ECLabVIEW : public ExperimentalControl
{
public:
    // constructors
    ECLabVIEW(int tag,
        int nTrialCPs, ExperimentalCP** trialCPs,
        int nOutCPs, ExperimentalCP** outCPs,
        char* ipAddress, int ipPort = 44000);
    ECLabVIEW(const ECLabVIEW& ec);
    
    // destructor
    virtual ~ECLabVIEW();
    
    // method to get class type
    const char* getClassType() const { return "ECLabVIEW"; };
    
    // public methods to set and to get response
    virtual int setup();
    virtual int setSize(ID sizeT, ID sizeO);
    
    virtual int setTrialResponse(
        const Vector* disp,
        const Vector* vel,
        const Vector* accel,
        const Vector* force,
        const Vector* time);
    virtual int getDaqResponse(
        Vector* disp,
        Vector* vel,
        Vector* accel,
        Vector* force,
        Vector* time);
    
    virtual int commitState();
    
    virtual ExperimentalControl* getCopy();
    
    // public methods for experimental control recorder
    virtual Response* setResponse(const char** argv, int argc,
        OPS_Stream& output);
    virtual int getResponse(int responseID, Information& info);
    
    // public methods for output
    void Print(OPS_Stream& s, int flag = 0);

protected:
    // protected methods to set and to get response
    virtual int control();
    virtual int acquire();

private:
    int numTrialCPs;            // number of trial control points
    ExperimentalCP** trialCPs;  // trial control points
    int numOutCPs;              // number of output control points
    ExperimentalCP** outCPs;    // output control points
    char* ipAddress;            // ip address
    int ipPort;                 // ip port
    
    TCP_Socket* theSocket;  // tcp/ip socket
    char* sData;            // send data array
    Message* sendData;      // send vector
    char* rData;            // receive data array
    Message* recvData;      // receive vector
    
    int numCtrlSignals, numDaqSignals;   // number of signals
    Vector* ctrlDisp, * ctrlForce;       // control vectors
    Vector* daqDisp, * daqForce;         // daq vectors
    Vector trialSigOffset;               // trial signal offsets
    Vector ctrlSigOffset, daqSigOffset;  // ctrl and daq signal offsets (i.e. setpoints)
    int gotRelativeTrial;                // relative trial signal flags
    
    time_t rawtime;
    struct tm* ptm;
    char OPFTransactionID[30];
    char errMsg[80];
    
    FILE* logFile;
};

#endif
