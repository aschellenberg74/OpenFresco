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

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 09/07
// Revision: A
//
// Description: This file contains the implementation of the
// ECSimFEAdapter class.

#include "ECSimFEAdapter.h"

#include <Channel.h>
#include <TCP_Socket.h>


ECSimFEAdapter::ECSimFEAdapter(int tag,    
    char *ipaddress, int ipport)
    : ECSimulation(tag),
    ipAddress(ipaddress), ipPort(ipport),
    dataSize(OF_Network_dataSize), theChannel(0),
    sData(0), sendData(0), rData(0), recvData(0),
    targDisp(0), targForce(0), measDisp(0), measForce(0)
{   
    // setup the connection
    theChannel = new TCP_Socket(ipPort, ipAddress);
    if (theChannel->setUpConnection() != 0)  {
        opserr << "ECSimFEAdapter::ECSimFEAdapter() - "
            << "failed to setup TCP connection to adapter element\n";
        delete theChannel;
        exit(OF_ReturnType_failed);
    }
    
    opserr << "*************************************************\n";
    opserr << "* The channel with address: " << ipAddress << "        *\n";
    opserr << "* and port: " << ipPort << " has been opened:              *\n";
    opserr << "*************************************************\n";
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


ECSimFEAdapter::ECSimFEAdapter(const ECSimFEAdapter &ec)
    : ECSimulation(ec),
    dataSize(OF_Network_dataSize)
{
    // temporarily does nothing
}


ECSimFEAdapter::~ECSimFEAdapter()
{
    // send termination to adapter element
    sData[0] = OF_RemoteTest_DIE;
    theChannel->sendVector(0, 0, *sendData, 0);

    // delete memory of target vectors
    if (targDisp != 0)
        delete targDisp;
    if (targForce != 0)
        delete targForce;
    
    // delete memory of measured vectors
    if (measDisp != 0)
        delete measDisp;
    if (measForce != 0)
        delete measForce;

    // close connection by destroying theChannel
    if (theChannel != 0)
        delete theChannel;

    opserr << "***********************************************************\n";
    opserr << "* The connection with the adapter element has been closed *\n";
    opserr << "***********************************************************\n";
    opserr << endln;
}


int ECSimFEAdapter::setSize(ID sizeT, ID sizeO)
{
    // check sizeTrial and sizeOut
    // for ECSimFEAdapter object
    
    // ECSimFEAdapter objects can only use 
    // disp and force for trial and disp and force for output
    // check these are available in sizeT/sizeO.
    if ((sizeT[OF_Resp_Disp] == 0 && sizeT[OF_Resp_Force] == 0) ||
        (sizeO[OF_Resp_Disp] == 0) || (sizeO[OF_Resp_Force] == 0))  {
        opserr << "ECSimFEAdapter::setSize() - wrong sizeTrial/Out\n"; 
        opserr << "see User Manual.\n";
        delete theChannel;
        exit(OF_ReturnType_failed);
    }
    
    *sizeCtrl = sizeT;
    *sizeDaq  = sizeO;
    
    return OF_ReturnType_completed;
}


int ECSimFEAdapter::setup()
{
    if (targDisp != 0)
        delete targDisp;
    if (targForce != 0)
        delete targForce;
    
    int id = 1;
    if ((*sizeCtrl)[OF_Resp_Disp] != 0)  {
        targDisp = new Vector(&sData[id],(*sizeCtrl)[OF_Resp_Disp]);
        id += (*sizeCtrl)[OF_Resp_Disp];
    }
    if ((*sizeCtrl)[OF_Resp_Force] != 0)  {
        targForce = new Vector(&sData[id],(*sizeCtrl)[OF_Resp_Force]);
        id += (*sizeCtrl)[OF_Resp_Force];
    }
    
    if (measDisp != 0)
        delete measDisp;
    if (measForce != 0)
        delete measForce;
    
    id = 0;
    if ((*sizeDaq)[OF_Resp_Disp] != 0)  {
        measDisp = new Vector(&rData[id],(*sizeDaq)[OF_Resp_Disp]);
        id += (*sizeDaq)[OF_Resp_Disp];
    }
    if ((*sizeDaq)[OF_Resp_Force] != 0)  {
        measForce = new Vector(&rData[id],(*sizeDaq)[OF_Resp_Force]);
        id += (*sizeDaq)[OF_Resp_Force];
    }
    
    // send the data size to the adapter element
    ID idData(2*OF_Resp_All+1);
    for (int i=0; i<OF_Resp_All; i++)  {
        idData(i) = (*sizeCtrl)[i];
        idData(OF_Resp_All+i) = (*sizeDaq)[i];
    }
    idData(2*OF_Resp_All) = dataSize;
    theChannel->sendID(0, 0, idData, 0);

    // print experimental control information
    this->Print(opserr);
    
    opserr << "******************\n";
    opserr << "* Running......  *\n";
    opserr << "******************\n";
    opserr << endln;
    
    return OF_ReturnType_completed;
}


int ECSimFEAdapter::setTrialResponse(const Vector* disp,
    const Vector* vel,
    const Vector* accel,
    const Vector* force,
    const Vector* time)
{
    *targDisp = *disp;
    if (theFilter != 0)  {
        for (int i=0; i<(*sizeCtrl)[OF_Resp_Disp]; i++)
            (*targDisp)(i) = theFilter->filtering((*targDisp)(i));
    }
    *targForce = *force;

    this->control();
    
    return OF_ReturnType_completed;
}


int ECSimFEAdapter::getDaqResponse(Vector* disp,
    Vector* vel,
    Vector* accel,
    Vector* force,
    Vector* time)
{
    this->acquire();
    
    *disp = *measDisp;
    *force = *measForce;
        
    return OF_ReturnType_completed;
}


int ECSimFEAdapter::commitState()
{	
    return OF_ReturnType_completed;
}


ExperimentalControl *ECSimFEAdapter::getCopy()
{
    return new ECSimFEAdapter(*this);
}


void ECSimFEAdapter::Print(OPS_Stream &s, int flag)
{
    s << "****************************************************************\n";
    s << "* ExperimentalControl: " << this->getTag() << endln; 
    s << "* type: ECSimFEAdapter\n";
    s << "*   ipAddress: " << ipAddress << ", ipPort: " << ipPort << endln;
    if (theFilter != 0) {
        s << "*\tFilter: " << *theFilter << endln;
    }
    s << "****************************************************************\n";
    s << endln;
}


int ECSimFEAdapter::control()
{
    sData[0] = OF_RemoteTest_setTrialResponse;
    theChannel->sendVector(0, 0, *sendData, 0);

    return OF_ReturnType_completed;
}


int ECSimFEAdapter::acquire()
{
    sData[0] = OF_RemoteTest_getForce;
    theChannel->sendVector(0, 0, *sendData, 0);
    theChannel->recvVector(0, 0, *recvData, 0);

    return OF_ReturnType_completed;
}


void ECSimFEAdapter::sleep(const clock_t wait)
{
    clock_t goal;
    goal = wait + clock();
    while (goal>clock());
}
