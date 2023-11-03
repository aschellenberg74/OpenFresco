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

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 08/11
// Revision: A
//
// Description: This file contains the implementation of the
// ECGenericTCP class.

#include "ECGenericTCP.h"

#include <fstream>
using std::ifstream;
#include <iomanip>
using std::ios;

#include <Message.h>
#include <Channel.h>
#include <TCP_Socket.h>
#include <TCP_SocketSSL.h>
#include <UDP_Socket.h>
#include <elementAPI.h>


void* OPF_ECGenericTCP()
{
    // pointer to experimental control that will be returned
    ExperimentalControl* theControl = 0;

    if (OPS_GetNumRemainingInputArgs() < 15) {
        opserr << "WARNING invalid number of arguments\n";
        opserr << "Want: expControl GenericTCP tag ipAddr ipPort -ctrlModes (5 mode) -daqModes (5 mode) "
            << "<-initFile fileName> <-ssl> <-udp> <-ctrlFilters (5 filterTag)> <-daqFilters (5 filterTag)>\n";
        return 0;
    }
    
    // control tag
    int tag;
    int numdata = 1;
    if (OPS_GetIntInput(&numdata, &tag) != 0) {
        opserr << "WARNING invalid expControl GenericTCP tag\n";
        return 0;
    }
    
    // ip address
    char* ipAddr;
    const char* type = OPS_GetString();
    ipAddr = new char[strlen(type) + 1];
    strcpy(ipAddr, type);
    
    // ip port
    int ipPort;
    numdata = 1;
    if (OPS_GetIntInput(&numdata, &ipPort) < 0) {
        opserr << "WARNING invalid ipPort\n";
        opserr << "expControl GenericTCP " << tag << endln;
        return 0;
    }
    
    // ctrlModes
    ID ctrlModes(5);
    type = OPS_GetString();
    if (strcmp(type, "-ctrlModes") != 0) {
        opserr << "WARNING expecting -ctrlModes (5 mode)\n";
        opserr << "expControl GenericTCP " << tag << endln;
        return 0;
    }
    for (int i = 0; i < 5; i++) {
        int mode;
        numdata = 1;
        if (OPS_GetIntInput(&numdata, &mode) < 0) {
            opserr << "WARNING invalid control mode\n";
            opserr << "expControl GenericTCP " << tag << endln;
            return 0;
        }
        ctrlModes(i) = mode;
    }
    
    // daqModes
    ID daqModes(5);
    type = OPS_GetString();
    if (strcmp(type, "-daqModes") != 0) {
        opserr << "WARNING expecting -daqModes (5 mode)\n";
        opserr << "expControl GenericTCP " << tag << endln;
        return 0;
    }
    for (int i = 0; i < 5; i++) {
        int mode;
        numdata = 1;
        if (OPS_GetIntInput(&numdata, &mode) < 0) {
            opserr << "WARNING invalid daq mode\n";
            opserr << "expControl GenericTCP " << tag << endln;
            return 0;
        }
        daqModes(i) = mode;
    }
    
    // optional parameters
    char* initFileName = 0;
    int ssl = 0, udp = 0;
    while (OPS_GetNumRemainingInputArgs() > 0) {
        type = OPS_GetString();
        if (strcmp(type, "-initFile") == 0) {
            type = OPS_GetString();
            initFileName = new char[strlen(type) + 1];
            strcpy(initFileName, type);
        }
        else if (strcmp(type, "-ssl") == 0) {
            ssl = 1; udp = 0;
        }
        else if (strcmp(type, "-udp") == 0) {
            udp = 1; ssl = 0;
        }
    }
    
    // parsing was successful, allocate the control
    theControl = new ECGenericTCP(tag, ipAddr, ipPort,
        ctrlModes, daqModes, initFileName, ssl, udp);
    if (theControl == 0) {
        opserr << "WARNING could not create experimental control of type GenericTCP\n";
        return 0;
    }
    
    return theControl;
}


ECGenericTCP::ECGenericTCP(int tag,
    char *ipaddress, int ipport,
    ID ctrlmodes, ID daqmodes,
    char *initfilename, int ssl, int udp)
    : ExperimentalControl(tag),
    ipAddress(ipaddress), ipPort(ipport),
    dataSize(OF_Network_dataSize), theChannel(0),
    sData(0), sendData(0), rData(0), recvData(0),
    ctrlModes(ctrlmodes), daqModes(daqmodes), initFileName(initfilename),
    ctrlDisp(0), ctrlVel(0), ctrlAccel(0), ctrlForce(0), ctrlTime(0),
    daqDisp(0), daqVel(0), daqAccel(0), daqForce(0), daqTime(0)
{   
    // check size of control and daq mode IDs
    if (ctrlModes.Size() != 5 || daqModes.Size() != 5)  {
        opserr << "ECGenericTCP::ECGenericTCP() - "
            << "ctrlMode and/or daqMode IDs have wrong size.\n";
        exit(OF_ReturnType_failed);
    }
    
    // setup the connection
    if (ssl)
        theChannel = new TCP_SocketSSL(ipPort, ipAddress);
    else if (udp)
        theChannel = new UDP_Socket(ipPort, ipAddress);
    else
        theChannel = new TCP_Socket(ipPort, ipAddress);
    
    if (!theChannel)  {
        opserr << "ECGenericTCP::ECGenericTCP() - "
            << "failed to create channel.\n";
        exit(OF_ReturnType_failed);
    }
    if (theChannel->setUpConnection() != 0)  {
        opserr << "ECGenericTCP::ECGenericTCP() - "
            << "failed to setup TCP connection to generic controller.\n";
        delete theChannel;
        exit(OF_ReturnType_failed);
    }
    
    opserr << "****************************************************************\n";
    opserr << "* The channel with address: " << ipAddress << endln;
    opserr << "* and port: " << ipPort << " has been opened\n";
    opserr << "****************************************************************\n";
    opserr << endln;
    
    // allocate memory for the send vectors
    sData = new double [dataSize];
    sendData = new Vector(sData, dataSize);
    sendData->Zero();
    
    // allocate memory for the receive vectors
    rData = new double [dataSize];
    recvData = new Vector(rData, dataSize);
    recvData->Zero();
}


ECGenericTCP::ECGenericTCP(const ECGenericTCP &ec)
    : ExperimentalControl(ec),
    dataSize(OF_Network_dataSize), theChannel(0),
    sData(0), sendData(0), rData(0), recvData(0),
    ctrlDisp(0), ctrlVel(0), ctrlAccel(0), ctrlForce(0), ctrlTime(0),
    daqDisp(0), daqVel(0), daqAccel(0), daqForce(0), daqTime(0)
{
    // use the existing channel which is set up
    ipAddress = ec.ipAddress;
    ipPort = ec.ipPort;
    theChannel = ec.theChannel;
    ctrlModes = ec.ctrlModes;
    daqModes = ec.daqModes;
    initFileName = ec.initFileName;
    
    // allocate memory for the send vectors
    sData = new double [dataSize];
    sendData = new Vector(sData, dataSize);
    sendData->Zero();
    
    // allocate memory for the receive vectors
    rData = new double [dataSize];
    recvData = new Vector(rData, dataSize);
    recvData->Zero();
}


ECGenericTCP::~ECGenericTCP()
{
    // send termination to generic controller
    sData[0] = OF_RemoteTest_DIE;
    theChannel->sendVector(0, 0, *sendData, 0);
    
    // delete memory of ctrl vectors
    if (ctrlDisp != 0)
        delete ctrlDisp;
    if (ctrlVel != 0)
        delete ctrlVel;
    if (ctrlAccel != 0)
        delete ctrlAccel;
    if (ctrlForce != 0)
        delete ctrlForce;
    if (ctrlTime != 0)
        delete ctrlTime;
    
    // delete memory of daq vectors
    if (daqDisp != 0)
        delete daqDisp;
    if (daqVel != 0)
        delete daqVel;
    if (daqAccel != 0)
        delete daqAccel;
    if (daqForce != 0)
        delete daqForce;
    if (daqTime != 0)
        delete daqTime;
    
    // delete memory of strings
    if (ipAddress != 0)
        delete [] ipAddress;
    if (initFileName != 0)
        delete [] initFileName;
    
    // close connection by destroying theChannel
    if (sendData != 0)
        delete sendData;
    if (sData != 0)
        delete [] sData;
    if (recvData != 0)
        delete recvData;
    if (rData != 0)
        delete [] rData;
    if (theChannel != 0)
        delete theChannel;
    
    opserr << endln;
    opserr << "**************************************************************\n";
    opserr << "* The connection with the generic controller has been closed *\n";
    opserr << "**************************************************************\n";
    opserr << endln;
}


int ECGenericTCP::setup()
{
    int rValue = 0;
    
    if (ctrlDisp != 0)
        delete ctrlDisp;
    if (ctrlVel != 0)
        delete ctrlVel;
    if (ctrlAccel != 0)
        delete ctrlAccel;
    if (ctrlForce != 0)
        delete ctrlForce;
    if (ctrlTime != 0)
        delete ctrlTime;
    
    int id = 1;
    if ((*sizeCtrl)(OF_Resp_Disp) != 0)  {
        ctrlDisp = new Vector(&sData[id],(*sizeCtrl)(OF_Resp_Disp));
        id += (*sizeCtrl)(OF_Resp_Disp);
    }
    if ((*sizeCtrl)(OF_Resp_Vel) != 0)  {
        ctrlVel = new Vector(&sData[id],(*sizeCtrl)(OF_Resp_Vel));
        id += (*sizeCtrl)(OF_Resp_Vel);
    }
    if ((*sizeCtrl)(OF_Resp_Accel) != 0)  {
        ctrlAccel = new Vector(&sData[id],(*sizeCtrl)(OF_Resp_Accel));
        id += (*sizeCtrl)(OF_Resp_Accel);
    }
    if ((*sizeCtrl)(OF_Resp_Force) != 0)  {
        ctrlForce = new Vector(&sData[id],(*sizeCtrl)(OF_Resp_Force));
        id += (*sizeCtrl)(OF_Resp_Force);
    }
    if ((*sizeCtrl)(OF_Resp_Time) != 0)  {
        ctrlTime = new Vector(&sData[id],(*sizeCtrl)(OF_Resp_Time));
        id += (*sizeCtrl)(OF_Resp_Time);
    }
    
    if (daqDisp != 0)
        delete daqDisp;
    if (daqVel != 0)
        delete daqVel;
    if (daqAccel != 0)
        delete daqAccel;
    if (daqForce != 0)
        delete daqForce;
    if (daqTime != 0)
        delete daqTime;
    
    id = 0;
    if ((*sizeDaq)(OF_Resp_Disp) != 0)  {
        daqDisp = new Vector(&rData[id],(*sizeDaq)(OF_Resp_Disp));
        id += (*sizeDaq)(OF_Resp_Disp);
    }
    if ((*sizeDaq)(OF_Resp_Vel) != 0)  {
        daqVel = new Vector(&rData[id],(*sizeDaq)(OF_Resp_Vel));
        id += (*sizeDaq)(OF_Resp_Vel);
    }
    if ((*sizeDaq)(OF_Resp_Accel) != 0)  {
        daqAccel = new Vector(&rData[id],(*sizeDaq)(OF_Resp_Accel));
        id += (*sizeDaq)(OF_Resp_Accel);
    }
    if ((*sizeDaq)(OF_Resp_Force) != 0)  {
        daqForce = new Vector(&rData[id],(*sizeDaq)(OF_Resp_Force));
        id += (*sizeDaq)(OF_Resp_Force);
    }
    if ((*sizeDaq)(OF_Resp_Time) != 0)  {
        daqTime = new Vector(&rData[id],(*sizeDaq)(OF_Resp_Time));
        id += (*sizeDaq)(OF_Resp_Time);
    }
    
    // send the data size to the generic controller
    ID idData(2*OF_Resp_All+1);
    for (int i=0; i<OF_Resp_All; i++)  {
        idData(i) = (*sizeCtrl)(i);
        idData(OF_Resp_All+i) = (*sizeDaq)(i);
    }
    idData(2*OF_Resp_All) = dataSize;
    theChannel->sendID(0, 0, idData, 0);
    
    // read the parameter initialization file and send information
    if (initFileName != 0)  {
        
        // determine the number of lines in the parameter file
        ifstream initFile;
        const int maxLineLength = 16384;
        char mData[maxLineLength+1];
        char *line = &mData[1];
        Message *msgData = new Message(mData, maxLineLength+1);
        
        // open the parameter initialization file
        initFile.open(initFileName, ios::in);
        if (initFile.bad() || !initFile.is_open())  {
            opserr << "ECGenericTCP::setup() - "
                << "could not open parameter initialization file.\n";
            return(OF_ReturnType_failed);
        } else  {
            // loop through the lines in the file
            mData[0] = OF_RemoteTest_setup;
            while (initFile.getline(line,maxLineLength))  {
                // send each line as a char array to the controller
                theChannel->sendMsg(0, 0, *msgData, 0);
            }
        }
        initFile.close();
        delete msgData;
    }
    
    // print experimental control information
    this->Print(opserr);
    
    opserr << "****************************************************************\n";
    opserr << "* Make sure that offset values of controller are set to ZERO   *\n";
    opserr << "*                                                              *\n";
    opserr << "* Press 'Enter' to proceed or 'c' to cancel the initialization *\n";
    opserr << "****************************************************************\n";
    opserr << endln;
    int c = getchar();
    if (c == 'c')  {
        getchar();
        sData[0] = OF_RemoteTest_DIE;
        theChannel->sendVector(0, 0, *sendData, 0);
        delete theChannel;
        exit(OF_ReturnType_failed);
    }
    
    do  {
        rValue += this->control();
        rValue += this->acquire();
        
        int i;
        opserr << "****************************************************************\n";
        opserr << "* Initial values of DAQ are:\n";
        opserr << "*\n";
        opserr << "* dspDaq = [";
        for (i=0; i<(*sizeDaq)(OF_Resp_Disp); i++)
            opserr << " " << (*daqDisp)(i);
        opserr << " ]\n";
        opserr << "* frcDaq = [";
        for (i=0; i<(*sizeDaq)(OF_Resp_Force); i++)
            opserr << " " << (*daqForce)(i);
        opserr << " ]\n";
        opserr << "*\n";
        opserr << "* Press 'Enter' to start the test or\n";
        opserr << "* 'r' to repeat the measurement or\n";
        opserr << "* 'c' to cancel the initialization\n";
        opserr << "****************************************************************\n";
        opserr << endln;
        c = getchar();
        if (c == 'c')  {
            getchar();
            sData[0] = OF_RemoteTest_DIE;
            theChannel->sendVector(0, 0, *sendData, 0);
            delete theChannel;
            exit(OF_ReturnType_failed);
        } else if (c == 'r')  {
            getchar();
        }
    } while (c == 'r');
    
    opserr << "*****************\n";
    opserr << "* Running...... *\n";
    opserr << "*****************\n";
    opserr << endln;
    
    return OF_ReturnType_completed;
}


int ECGenericTCP::setSize(ID sizeT, ID sizeO)
{
    // check sizeTrial and sizeOut
    // for ECGenericTCP object
    
    // ECGenericTCP objects can only use 
    // responses specified in ctrlModes and daqModes
    // check that they are available in sizeT/sizeO.
    if (ctrlModes(OF_Resp_Disp))  { 
        if (sizeT(OF_Resp_Disp) == 0)  {
            opserr << "ECGenericTCP::setSize() - disp not available for control\n";
            opserr << "see User Manual.\n";
            delete theChannel;
            exit(OF_ReturnType_failed);
        }
        (*sizeCtrl)(OF_Resp_Disp) = sizeT(OF_Resp_Disp);
    } else  {
        (*sizeCtrl)(OF_Resp_Disp) = 0;
    }
    if (ctrlModes(OF_Resp_Vel))  { 
        if (sizeT(OF_Resp_Vel) == 0)  {
            opserr << "ECGenericTCP::setSize() - vel not available for control\n";
            opserr << "see User Manual.\n";
            delete theChannel;
            exit(OF_ReturnType_failed);
        }
        (*sizeCtrl)(OF_Resp_Vel) = sizeT(OF_Resp_Vel);
    } else  {
        (*sizeCtrl)(OF_Resp_Vel) = 0;
    }
    if (ctrlModes(OF_Resp_Accel))  { 
        if (sizeT(OF_Resp_Accel) == 0)  {
            opserr << "ECGenericTCP::setSize() - accel not available for control\n";
            opserr << "see User Manual.\n";
            delete theChannel;
            exit(OF_ReturnType_failed);
        }
        (*sizeCtrl)(OF_Resp_Accel) = sizeT(OF_Resp_Accel);
    } else  {
        (*sizeCtrl)(OF_Resp_Accel) = 0;
    }
    if (ctrlModes(OF_Resp_Force))  { 
        if (sizeT(OF_Resp_Force) == 0)  {
            opserr << "ECGenericTCP::setSize() - force not available for control\n";
            opserr << "see User Manual.\n";
            delete theChannel;
            exit(OF_ReturnType_failed);
        }
        (*sizeCtrl)(OF_Resp_Force) = sizeT(OF_Resp_Force);
    } else  {
        (*sizeCtrl)(OF_Resp_Force) = 0;
    }
    if (ctrlModes(OF_Resp_Time))  { 
        if (sizeT(OF_Resp_Time) == 0)  {
            opserr << "ECGenericTCP::setSize() - time not available for control\n";
            opserr << "see User Manual.\n";
            delete theChannel;
            exit(OF_ReturnType_failed);
        }
        (*sizeCtrl)(OF_Resp_Time) = sizeT(OF_Resp_Time);
    } else  {
        (*sizeCtrl)(OF_Resp_Time) = 0;
    }
    
    if (daqModes(OF_Resp_Disp))  { 
        if (sizeO(OF_Resp_Disp) == 0)  {
            opserr << "ECGenericTCP::setSize() - disp not accepted from daq\n";
            opserr << "see User Manual.\n";
            delete theChannel;
            exit(OF_ReturnType_failed);
        }
        (*sizeDaq)(OF_Resp_Disp) = sizeO(OF_Resp_Disp);
    } else  {
        (*sizeDaq)(OF_Resp_Disp) = 0;
    }
    if (daqModes(OF_Resp_Vel))  { 
        if (sizeO(OF_Resp_Vel) == 0)  {
            opserr << "ECGenericTCP::setSize() - vel not accepted from daq\n";
            opserr << "see User Manual.\n";
            delete theChannel;
            exit(OF_ReturnType_failed);
        }
        (*sizeDaq)(OF_Resp_Vel) = sizeO(OF_Resp_Vel);
    } else  {
        (*sizeDaq)(OF_Resp_Vel) = 0;
    }
    if (daqModes(OF_Resp_Accel))  { 
        if (sizeO(OF_Resp_Accel) == 0)  {
            opserr << "ECGenericTCP::setSize() - accel not accepted from daq\n";
            opserr << "see User Manual.\n";
            delete theChannel;
            exit(OF_ReturnType_failed);
        }
        (*sizeDaq)(OF_Resp_Accel) = sizeO(OF_Resp_Accel);
    } else  {
        (*sizeDaq)(OF_Resp_Accel) = 0;
    }
    if (daqModes(OF_Resp_Force))  { 
        if (sizeO(OF_Resp_Force) == 0)  {
            opserr << "ECGenericTCP::setSize() - force not accepted from daq\n";
            opserr << "see User Manual.\n";
            delete theChannel;
            exit(OF_ReturnType_failed);
        }
        (*sizeDaq)(OF_Resp_Force) = sizeO(OF_Resp_Force);
    } else  {
        (*sizeDaq)(OF_Resp_Force) = 0;
    }
    if (daqModes(OF_Resp_Time))  { 
        if (sizeO(OF_Resp_Time) == 0)  {
            opserr << "ECGenericTCP::setSize() - time not accepted from daq\n";
            opserr << "see User Manual.\n";
            delete theChannel;
            exit(OF_ReturnType_failed);
        }
        (*sizeDaq)(OF_Resp_Time) = sizeO(OF_Resp_Time);
    } else  {
        (*sizeDaq)(OF_Resp_Time) = 0;
    }
    
    return OF_ReturnType_completed;
}


int ECGenericTCP::setTrialResponse(const Vector* disp,
    const Vector* vel,
    const Vector* accel,
    const Vector* force,
    const Vector* time)
{
    int i, rValue = 0;
    if (disp != 0 && (*sizeCtrl)(OF_Resp_Disp) != 0)  {
        *ctrlDisp = *disp;
        if (theCtrlFilters[OF_Resp_Disp] != 0)  {
            for (i=0; i<(*sizeCtrl)(OF_Resp_Disp); i++)
                (*ctrlDisp)(i) = theCtrlFilters[OF_Resp_Disp]->filtering((*ctrlDisp)(i));
        }
    }
    if (vel != 0 && (*sizeCtrl)(OF_Resp_Vel) != 0)  {
        *ctrlVel = *vel;
        if (theCtrlFilters[OF_Resp_Vel] != 0)  {
            for (i=0; i<(*sizeCtrl)(OF_Resp_Vel); i++)
                (*ctrlVel)(i) = theCtrlFilters[OF_Resp_Vel]->filtering((*ctrlVel)(i));
        }
    }
    if (accel != 0 && (*sizeCtrl)(OF_Resp_Accel) != 0)  {
        *ctrlAccel = *accel;
        if (theCtrlFilters[OF_Resp_Accel] != 0)  {
            for (i=0; i<(*sizeCtrl)(OF_Resp_Accel); i++)
                (*ctrlAccel)(i) = theCtrlFilters[OF_Resp_Accel]->filtering((*ctrlAccel)(i));
        }
    }
    if (force != 0 && (*sizeCtrl)(OF_Resp_Force) != 0)  {
        *ctrlForce = *force;
        if (theCtrlFilters[OF_Resp_Force] != 0)  {
            for (i=0; i<(*sizeCtrl)(OF_Resp_Force); i++)
                (*ctrlForce)(i) = theCtrlFilters[OF_Resp_Force]->filtering((*ctrlForce)(i));
        }
    }
    if (time != 0 && (*sizeCtrl)(OF_Resp_Time) != 0)  {
        *ctrlTime = *time;
        if (theCtrlFilters[OF_Resp_Time] != 0)  {
            for (i=0; i<(*sizeCtrl)(OF_Resp_Time); i++)
                (*ctrlTime)(i) = theCtrlFilters[OF_Resp_Time]->filtering((*ctrlTime)(i));
        }
    }
    
    rValue = this->control();
    
    return rValue;
}


int ECGenericTCP::getDaqResponse(Vector* disp,
    Vector* vel,
    Vector* accel,
    Vector* force,
    Vector* time)
{
    this->acquire();
    
    int i;
    if (disp != 0 && (*sizeDaq)(OF_Resp_Disp) != 0)  {
        if (theDaqFilters[OF_Resp_Disp] != 0)  {
            for (i=0; i<(*sizeDaq)(OF_Resp_Disp); i++)
                (*daqDisp)(i) = theDaqFilters[OF_Resp_Disp]->filtering((*daqDisp)(i));
        }
        *disp = *daqDisp;
    }
    if (vel != 0 && (*sizeDaq)(OF_Resp_Vel) != 0)  {
        if (theDaqFilters[OF_Resp_Vel] != 0)  {
            for (i=0; i<(*sizeDaq)(OF_Resp_Vel); i++)
                (*daqVel)(i) = theDaqFilters[OF_Resp_Vel]->filtering((*daqVel)(i));
        }
        *vel = *daqVel;
    }
    if (accel != 0 && (*sizeDaq)(OF_Resp_Accel) != 0)  {
        if (theDaqFilters[OF_Resp_Accel] != 0)  {
            for (i=0; i<(*sizeDaq)(OF_Resp_Accel); i++)
                (*daqAccel)(i) = theDaqFilters[OF_Resp_Accel]->filtering((*daqAccel)(i));
        }
        *accel = *daqAccel;
    }
    if (force != 0 && (*sizeDaq)(OF_Resp_Force) != 0)  {
        if (theDaqFilters[OF_Resp_Force] != 0)  {
            for (i=0; i<(*sizeDaq)(OF_Resp_Force); i++)
                (*daqForce)(i) = theDaqFilters[OF_Resp_Force]->filtering((*daqForce)(i));
        }
        *force = *daqForce;
    }
    if (time != 0 && (*sizeDaq)(OF_Resp_Time) != 0)  {
        if (theDaqFilters[OF_Resp_Time] != 0)  {
            for (i=0; i<(*sizeDaq)(OF_Resp_Time); i++)
                (*daqTime)(i) = theDaqFilters[OF_Resp_Time]->filtering((*daqTime)(i));
        }
        *time = *daqTime;
    }
    
    return OF_ReturnType_completed;
}


int ECGenericTCP::commitState()
{
    return OF_ReturnType_completed;
}


ExperimentalControl *ECGenericTCP::getCopy()
{
    return new ECGenericTCP(*this);
}


Response* ECGenericTCP::setResponse(const char **argv, int argc,
    OPS_Stream &output)
{
    int i;
    char outputData[15];
    Response *theResponse = 0;
    
    output.tag("ExpControlOutput");
    output.attr("ctrlType",this->getClassType());
    output.attr("ctrlTag",this->getTag());
    
    // ctrl displacements
    if (ctrlDisp != 0 && (
        strcmp(argv[0],"ctrlDisp") == 0 ||
        strcmp(argv[0],"ctrlDisplacement") == 0 ||
        strcmp(argv[0],"ctrlDisplacements") == 0))
    {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Disp); i++)  {
            sprintf(outputData,"ctrlDisp%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 1, *ctrlDisp);
    }
    
    // ctrl velocities
    else if (ctrlVel != 0 && (
        strcmp(argv[0],"ctrlVel") == 0 ||
        strcmp(argv[0],"ctrlVelocity") == 0 ||
        strcmp(argv[0],"ctrlVelocities") == 0))
    {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Vel); i++)  {
            sprintf(outputData,"ctrlVel%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 2, *ctrlVel);
    }
    
    // ctrl accelerations
    else if (ctrlAccel != 0 && (
        strcmp(argv[0],"ctrlAccel") == 0 ||
        strcmp(argv[0],"ctrlAcceleration") == 0 ||
        strcmp(argv[0],"ctrlAccelerations") == 0))
    {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Accel); i++)  {
            sprintf(outputData,"ctrlAccel%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 3, *ctrlAccel);
    }
    
    // ctrl forces
    else if (ctrlForce != 0 && (
        strcmp(argv[0],"ctrlForce") == 0 ||
        strcmp(argv[0],"ctrlForces") == 0))
    {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Force); i++)  {
            sprintf(outputData,"ctrlForce%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 4, *ctrlForce);
    }
    
    // ctrl times
    else if (ctrlTime != 0 && (
        strcmp(argv[0],"ctrlTime") == 0 ||
        strcmp(argv[0],"ctrlTimes") == 0))
    {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Time); i++)  {
            sprintf(outputData,"ctrlTime%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 5, *ctrlTime);
    }
    
    // daq displacements
    else if (daqDisp != 0 && (
        strcmp(argv[0],"daqDisp") == 0 ||
        strcmp(argv[0],"daqDisplacement") == 0 ||
        strcmp(argv[0],"daqDisplacements") == 0))
    {
        for (i=0; i<(*sizeDaq)(OF_Resp_Disp); i++)  {
            sprintf(outputData,"daqDisp%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 6, *daqDisp);
    }
    
    // daq velocities
    else if (daqVel != 0 && (
        strcmp(argv[0],"daqVel") == 0 ||
        strcmp(argv[0],"daqVelocity") == 0 ||
        strcmp(argv[0],"daqVelocities") == 0))
    {
        for (i=0; i<(*sizeDaq)(OF_Resp_Vel); i++)  {
            sprintf(outputData,"daqVel%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 7, *daqVel);
    }
    
    // daq accelerations
    else if (daqAccel != 0 && (
        strcmp(argv[0],"daqAccel") == 0 ||
        strcmp(argv[0],"daqAcceleration") == 0 ||
        strcmp(argv[0],"daqAccelerations") == 0))
    {
        for (i=0; i<(*sizeDaq)(OF_Resp_Accel); i++)  {
            sprintf(outputData,"daqAccel%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 8, *daqAccel);
    }
    
    // daq forces
    else if (daqForce != 0 && (
        strcmp(argv[0],"daqForce") == 0 ||
        strcmp(argv[0],"daqForces") == 0))
    {
        for (i=0; i<(*sizeDaq)(OF_Resp_Force); i++)  {
            sprintf(outputData,"daqForce%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 9, *daqForce);
    }
    
    // daq times
    else if (daqTime != 0 && (
        strcmp(argv[0],"daqTime") == 0 ||
        strcmp(argv[0],"daqTimes") == 0))
    {
        for (i=0; i<(*sizeDaq)(OF_Resp_Time); i++)  {
            sprintf(outputData,"daqTime%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 10, *daqTime);
    }
    
    output.endTag();
    
    return theResponse;
}


int ECGenericTCP::getResponse(int responseID, Information &info)
{
    switch (responseID)  {
    case 1:  // ctrl displacements
        return info.setVector(*ctrlDisp);
        
    case 2:  // ctrl velocities
        return info.setVector(*ctrlVel);
        
    case 3:  // ctrl accelerations
        return info.setVector(*ctrlAccel);
        
    case 4:  // ctrl forces
        return info.setVector(*ctrlForce);
        
    case 5:  // ctrl times
        return info.setVector(*ctrlTime);
        
    case 6:  // daq displacements
        return info.setVector(*daqDisp);
        
    case 7:  // daq velocities
        return info.setVector(*daqVel);
        
    case 8:  // daq accelerations
        return info.setVector(*daqAccel);
        
    case 9:  // daq forces
        return info.setVector(*daqForce);
        
    case 10:  // daq times
        return info.setVector(*daqTime);
        
    default:
        return -1;
    }
}


void ECGenericTCP::Print(OPS_Stream &s, int flag)
{
    s << "****************************************************************\n";
    s << "* ExperimentalControl: " << this->getTag() << endln; 
    s << "*   type: ECGenericTCP\n";
    s << "*   ipAddress: " << ipAddress << ", ipPort: " << ipPort << endln;
    s << "*   ctrlModes: " << ctrlModes;
    s << "*   daqModes: " << daqModes;
    if (initFileName != 0)
        s << "*   initFile: " << initFileName << endln;
    s << "*   ctrlFilters:";
    for (int i=0; i<OF_Resp_All; i++)  {
        if (theCtrlFilters[i] != 0)
            s << " " << theCtrlFilters[i]->getTag();
        else
            s << " 0";
    }
    s << "\n*   daqFilters:";
    for (int i=0; i<OF_Resp_All; i++)  {
        if (theDaqFilters[i] != 0)
            s << " " << theDaqFilters[i]->getTag();
        else
            s << " 0";
    }
    s << endln;
    s << "****************************************************************\n";
    s << endln;
}


int ECGenericTCP::control()
{
    sData[0] = OF_RemoteTest_setTrialResponse;
    theChannel->sendVector(0, 0, *sendData, 0);

    return OF_ReturnType_completed;
}


int ECGenericTCP::acquire()
{
    sData[0] = OF_RemoteTest_getDaqResponse;
    theChannel->sendVector(0, 0, *sendData, 0);
    theChannel->recvVector(0, 0, *recvData, 0);

    return OF_ReturnType_completed;
}
