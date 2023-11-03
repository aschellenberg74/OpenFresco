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
// Created: 09/09
// Revision: A
//
// Description: This file contains the implementation of the
// ECSimSimulink class.

#include "ECSimSimulink.h"

#include <Channel.h>
#include <TCP_Socket.h>
#include <TCP_SocketSSL.h>
#include <UDP_Socket.h>
#include <elementAPI.h>


void* OPF_ECSimSimulink()
{
    // pointer to experimental control that will be returned
    ExperimentalControl* theControl = 0;
    
    if (OPS_GetNumRemainingInputArgs() < 3) {
        opserr << "WARNING invalid number of arguments\n";
        opserr << "Want: expControl SimSimulink tag ipAddr ipPort <-ssl> <-udp> "
            << "<-ctrlFilters (5 filterTag)> <-daqFilters (5 filterTag)>\n";
        return 0;
    }
    
    // control tag
    int tag;
    int numdata = 1;
    if (OPS_GetIntInput(&numdata, &tag) != 0) {
        opserr << "WARNING invalid expControl SimSimulink tag\n";
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
        opserr << "expControl SimSimulink " << tag << endln;
        return 0;
    }
    
    // optional parameters
    int ssl = 0, udp = 0;
    int useRelativeTrial = 0;
    while (OPS_GetNumRemainingInputArgs() > 0) {
        type = OPS_GetString();
        if (strcmp(type, "-ssl") == 0) {
            ssl = 1; udp = 0;
        }
        else if (strcmp(type, "-udp") == 0) {
            udp = 1; ssl = 0;
        }
    }
    
    // parsing was successful, allocate the control
    theControl = new ECSimSimulink(tag, ipAddr, ipPort);
    if (theControl == 0) {
        opserr << "WARNING could not create experimental control of type SimSimulink\n";
        return 0;
    }
    
    return theControl;
}


ECSimSimulink::ECSimSimulink(int tag,
    char *ipaddress, int ipport, int _ssl, int _udp)
    : ECSimulation(tag), ipAddress(ipaddress),
    ipPort(ipport), ssl(_ssl), udp(_udp),
    dataSize(OF_Network_dataSize), theChannel(0),
    sData(0), sendData(0), rData(0), recvData(0),
    ctrlDisp(0), ctrlForce(0), ctrlTime(0),
    daqDisp(0), daqForce(0), daqTime(0)
{
    // setup the connection
    if (udp)
        theChannel = new UDP_Socket(ipPort, ipAddress);
    else if (ssl)
        theChannel = new TCP_SocketSSL(ipPort, ipAddress);
    else
        theChannel = new TCP_Socket(ipPort, ipAddress);
    
    if (theChannel->setUpConnection() != 0)  {
        opserr << "ECSimSimulink::ECSimSimulink() - "
            << "failed to setup connection to Simulink model.\n";
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


ECSimSimulink::ECSimSimulink(const ECSimSimulink &ec)
    : ECSimulation(ec),
    dataSize(OF_Network_dataSize), theChannel(0),
    sData(0), sendData(0), rData(0), recvData(0),
    ctrlDisp(0), ctrlForce(0), ctrlTime(0),
    daqDisp(0), daqForce(0), daqTime(0)
{
    // use the existing channel which is set up
    ipAddress = ec.ipAddress;
    ipPort    = ec.ipPort;
    ssl       = ec.ssl;
    udp       = ec.udp;
    
    theChannel = ec.theChannel;
    
    // allocate memory for the send vectors
    sData = new double [dataSize];
    sendData = new Vector(sData, dataSize);
    sendData->Zero();
    
    // allocate memory for the receive vectors
    rData = new double [dataSize];
    recvData = new Vector(rData, dataSize);
    recvData->Zero();
}


ECSimSimulink::~ECSimSimulink()
{
    // send termination to adapter element (twice for reliability)
    sData[0] = OF_RemoteTest_DIE;
    theChannel->sendVector(0, 0, *sendData, 0);
    theChannel->sendVector(0, 0, *sendData, 0);
    
    // delete memory of ctrl vectors
    if (ctrlDisp != 0)
        delete ctrlDisp;
    if (ctrlForce != 0)
        delete ctrlForce;
    if (ctrlTime != 0)
        delete ctrlTime;
    
    // delete memory of daq vectors
    if (daqDisp != 0)
        delete daqDisp;
    if (daqForce != 0)
        delete daqForce;
    if (daqTime != 0)
        delete daqTime;
    
    // delete memory of string
    if (ipAddress != 0)
        delete [] ipAddress;
    
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
    opserr << "**********************************************************\n";
    opserr << "* The connection with the Simulink model has been closed *\n";
    opserr << "**********************************************************\n";
    opserr << endln;
}


int ECSimSimulink::setup()
{
    int rValue = 0;
    
    if (ctrlDisp != 0)
        delete ctrlDisp;
    if (ctrlForce != 0)
        delete ctrlForce;
    if (ctrlTime != 0)
        delete ctrlTime;
    
    int id = 1;
    if ((*sizeCtrl)(OF_Resp_Disp) != 0)  {
        ctrlDisp = new Vector(&sData[id],(*sizeCtrl)(OF_Resp_Disp));
        id += (*sizeCtrl)(OF_Resp_Disp);
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
    if (daqForce != 0)
        delete daqForce;
    if (daqTime != 0)
        delete daqTime;
    
    id = 1;
    if ((*sizeDaq)(OF_Resp_Disp) != 0)  {
        daqDisp = new Vector(&rData[id],(*sizeDaq)(OF_Resp_Disp));
        id += (*sizeDaq)(OF_Resp_Disp);
    }
    if ((*sizeDaq)(OF_Resp_Force) != 0)  {
        daqForce = new Vector(&rData[id],(*sizeDaq)(OF_Resp_Force));
        id += (*sizeDaq)(OF_Resp_Force);
    }
    if ((*sizeDaq)(OF_Resp_Time) != 0)  {
        daqTime = new Vector(&rData[id],(*sizeDaq)(OF_Resp_Time));
        id += (*sizeDaq)(OF_Resp_Time);
    }
    
    // send the data size to the Simulink model
    ID idData(2*OF_Resp_All+1);
    for (int i=0; i<OF_Resp_All; i++)  {
        idData(i) = (*sizeCtrl)(i);
        idData(OF_Resp_All+i) = (*sizeDaq)(i);
    }
    idData(2*OF_Resp_All) = dataSize;
    theChannel->sendID(0, 0, idData, 0);
    
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
        this->~ECSimSimulink();
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
            opserr << " " << daqDisp[i];
        opserr << " ]\n";
        opserr << "* frcDaq = [";
        for (i=0; i<(*sizeDaq)(OF_Resp_Force); i++)
            opserr << " " << daqForce[i];
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
            this->~ECSimSimulink();
            exit(OF_ReturnType_failed);
        } else if (c == 'r')  {
            getchar();
        }
    } while (c == 'r');
    
    opserr << "*****************\n";
    opserr << "* Running...... *\n";
    opserr << "*****************\n";
    opserr << endln;
    
    return rValue;
}


int ECSimSimulink::setSize(ID sizeT, ID sizeO)
{
    // check sizeTrial and sizeOut
    // for ECSimSimulink object
    
    // ECSimSimulink objects can only use 
    // disp and force for trial and disp and force for output
    // check these are available in sizeT/sizeO.
    if ((sizeT(OF_Resp_Disp) == 0 && sizeT(OF_Resp_Force) == 0) ||
        (sizeO(OF_Resp_Disp) == 0) || (sizeO(OF_Resp_Force) == 0))  {
        opserr << "ECSimSimulink::setSize() - wrong sizeTrial/Out\n"; 
        opserr << "see User Manual.\n";
        delete theChannel;
        exit(OF_ReturnType_failed);
    }
    
    *sizeCtrl = sizeT;
    *sizeDaq  = sizeO;
    
    return OF_ReturnType_completed;
}


int ECSimSimulink::setTrialResponse(const Vector* disp,
    const Vector* vel,
    const Vector* accel,
    const Vector* force,
    const Vector* time)
{
    int i, rValue = 0;
    if (disp != 0)  {
        *ctrlDisp = *disp;
        if (theCtrlFilters[OF_Resp_Disp] != 0)  {
            for (i=0; i<(*sizeCtrl)(OF_Resp_Disp); i++)
                (*ctrlDisp)(i) = theCtrlFilters[OF_Resp_Disp]->filtering((*ctrlDisp)(i));
        }
    }
    if (force != 0)  {
        *ctrlForce = *force;
        if (theCtrlFilters[OF_Resp_Force] != 0)  {
            for (i=0; i<(*sizeCtrl)(OF_Resp_Force); i++)
                (*ctrlForce)(i) = theCtrlFilters[OF_Resp_Force]->filtering((*ctrlForce)(i));
        }
    }
    if (time != 0)  {
        *ctrlTime = *time;
        if (theCtrlFilters[OF_Resp_Time] != 0)  {
            for (i=0; i<(*sizeCtrl)(OF_Resp_Time); i++)
                (*ctrlTime)(i) = theCtrlFilters[OF_Resp_Time]->filtering((*ctrlTime)(i));
        }
    }
    
    rValue = this->control();
    
    return rValue;
}


int ECSimSimulink::getDaqResponse(Vector* disp,
    Vector* vel,
    Vector* accel,
    Vector* force,
    Vector* time)
{
    this->acquire();
    
    int i;
    if (disp != 0)  {
        if (theDaqFilters[OF_Resp_Disp] != 0)  {
            for (i=0; i<(*sizeDaq)(OF_Resp_Disp); i++)
                (*daqDisp)(i) = theDaqFilters[OF_Resp_Disp]->filtering((*daqDisp)(i));
        }
        *disp = *daqDisp;
    }
    if (force != 0)  {
        if (theDaqFilters[OF_Resp_Force] != 0)  {
            for (i=0; i<(*sizeDaq)(OF_Resp_Force); i++)
                (*daqForce)(i) = theDaqFilters[OF_Resp_Force]->filtering((*daqForce)(i));
        }
        *force = *daqForce;
    }
    if (time != 0)  {
        if (theDaqFilters[OF_Resp_Time] != 0)  {
            for (i=0; i<(*sizeDaq)(OF_Resp_Time); i++)
                (*daqTime)(i) = theDaqFilters[OF_Resp_Time]->filtering((*daqTime)(i));
        }
        *time = *daqTime;
    }
    
    return OF_ReturnType_completed;
}


int ECSimSimulink::commitState()
{
    sData[0] = OF_RemoteTest_commitState;
    theChannel->sendVector(0, 0, *sendData, 0);
    
    return OF_ReturnType_completed;
}


ExperimentalControl *ECSimSimulink::getCopy()
{
    return new ECSimSimulink(*this);
}


Response* ECSimSimulink::setResponse(const char **argv, int argc,
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
    
    // ctrl forces
    else if (ctrlForce != 0 && (
        strcmp(argv[0],"ctrlForce") == 0 ||
        strcmp(argv[0],"ctrlForces") == 0))
    {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Force); i++)  {
            sprintf(outputData,"ctrlForce%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 2, *ctrlForce);
    }
    
    // ctrl time
    else if (ctrlTime != 0 && (
        strcmp(argv[0],"ctrlTime") == 0 ||
        strcmp(argv[0],"ctrlTimes") == 0))
    {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Time); i++)  {
            sprintf(outputData,"ctrlTime%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 3, *ctrlTime);
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
        theResponse = new ExpControlResponse(this, 4, *daqDisp);
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
        theResponse = new ExpControlResponse(this, 5, *daqForce);
    }
    
    // daq time
    else if (daqTime != 0 && (
        strcmp(argv[0],"daqTime") == 0 ||
        strcmp(argv[0],"daqTimes") == 0))
    {
        for (i=0; i<(*sizeDaq)(OF_Resp_Time); i++)  {
            sprintf(outputData,"daqTime%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 6, *daqTime);
    }
    
    output.endTag();
    
    return theResponse;
}


int ECSimSimulink::getResponse(int responseID, Information &info)
{
    switch (responseID)  {
    case 1:  // ctrl displacements
        return info.setVector(*ctrlDisp);
        
    case 2:  // ctrl forces
        return info.setVector(*ctrlForce);
        
    case 3:  // ctrl time
        return info.setVector(*ctrlTime);
        
    case 4:  // daq displacements
        return info.setVector(*daqDisp);
        
    case 5:  // daq forces
        return info.setVector(*daqForce);
        
    case 6:  // daq time
        return info.setVector(*daqTime);
        
    default:
        return -1;
    }
}


void ECSimSimulink::Print(OPS_Stream &s, int flag)
{
    s << "****************************************************************\n";
    s << "* ExperimentalControl: " << this->getTag() << endln; 
    s << "*   type: ECSimSimulink\n";
    if (udp)
        s << "*   channel: UDP, ";
    else if (ssl)
        s << "*   channel: SSL, ";
    else
        s << "*   channel: TCP, ";
    s << "ipAddress: " << ipAddress << ", ipPort: " << ipPort << endln;
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


int ECSimSimulink::control()
{
    // send ctrlDisp
    sData[0] = OF_RemoteTest_setTrialResponse;
    theChannel->sendVector(0, 0, *sendData, 0);
    
    // send newTarget flag
    sData[0] = OF_RemoteTest_execute;
    theChannel->sendVector(0, 0, *sendData, 0);
    
    // wait until switchPC flag has changed as well
    sData[0] = 4.1;
    rData[0] = 0;  // switchPC flag
    while (rData[0] != 1)  {
        theChannel->sendVector(0, 0, *sendData, 0);
        theChannel->recvVector(0, 0, *recvData, 0);
    }
    
    // reset newTarget flag
    sData[0] = 4.2;
    theChannel->sendVector(0, 0, *sendData, 0);
    
    // wait until switchPC flag has changed back
    sData[0] = 4.3;
    rData[0] = 1;  // switchPC flag
    while (rData[0] != 0)  {
        theChannel->sendVector(0, 0, *sendData, 0);
        theChannel->recvVector(0, 0, *recvData, 0);
    }
    
    return OF_ReturnType_completed;
}


int ECSimSimulink::acquire()
{
    // read displacements and resisting forces until target is reached
    rData[0] = 0;  // atTarget flag
    while (rData[0] != 1)  {
        sData[0] = OF_RemoteTest_getDaqResponse;
        theChannel->sendVector(0, 0, *sendData, 0);
        theChannel->recvVector(0, 0, *recvData, 0);
    }
    
    return OF_ReturnType_completed;
}
