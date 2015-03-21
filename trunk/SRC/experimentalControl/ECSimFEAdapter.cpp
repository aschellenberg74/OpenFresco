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

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 09/07
// Revision: A
//
// Description: This file contains the implementation of the
// ECSimFEAdapter class.

#include "ECSimFEAdapter.h"
#include <ExperimentalCP.h>

#include <Channel.h>
#include <TCP_Socket.h>


ECSimFEAdapter::ECSimFEAdapter(int tag, int nTrialCPs, ExperimentalCP **trialcps,
    int nOutCPs, ExperimentalCP **outcps, char *ipaddress, int ipport)
    : ECSimulation(tag), numTrialCPs(nTrialCPs), numOutCPs(nOutCPs),
    ipAddress(ipaddress), ipPort(ipport), dataSize(OF_Network_dataSize),
    theChannel(0), sData(0), sendData(0), rData(0), recvData(0),
    numCtrlSignals(0), numDaqSignals(0), ctrlSignal(0), daqSignal(0)
{
    // get trial and output control points
    if (trialcps == 0 || outcps == 0)  {
        opserr << "ECSimFEAdapter::ECSimFEAdapter() - "
            << "null trialCPs or outCPs array passed.\n";
        exit(OF_ReturnType_failed);
    }
    trialCPs = trialcps;
    outCPs = outcps;
    
    // get total number of control and daq signals
    for (int i=0; i<numTrialCPs; i++)
        numCtrlSignals += trialCPs[i]->getNumSignal();
    for (int i=0; i<numOutCPs; i++)
        numDaqSignals += outCPs[i]->getNumSignal();
    
    // modify data size of network transactions
    if (dataSize < numCtrlSignals)
        dataSize = numCtrlSignals;
    if (dataSize < numDaqSignals)
        dataSize = numDaqSignals;
    
    // setup the connection
    theChannel = new TCP_Socket(ipPort, ipAddress);
    if (theChannel->setUpConnection() != 0)  {
        opserr << "ECSimFEAdapter::ECSimFEAdapter() - "
            << "failed to setup TCP connection to adapter element.\n";
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


ECSimFEAdapter::ECSimFEAdapter(const ECSimFEAdapter &ec)
    : ECSimulation(ec), dataSize(OF_Network_dataSize),
    theChannel(0), sData(0), sendData(0), rData(0), recvData(0),
    numCtrlSignals(0), numDaqSignals(0), ctrlSignal(0), daqSignal(0)
{
    // use the existing channel which is set up
    numTrialCPs = ec.numTrialCPs;
    trialCPs    = ec.trialCPs;
    numOutCPs   = ec.numOutCPs;
    outCPs      = ec.outCPs;
    ipAddress   = ec.ipAddress;
    ipPort      = ec.ipPort;
    
    numCtrlSignals = ec.numCtrlSignals;
    numDaqSignals  = ec.numDaqSignals;
    
    dataSize   = ec.dataSize;
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


ECSimFEAdapter::~ECSimFEAdapter()
{
    // send termination to adapter element (twice for reliability)
    sData[0] = OF_RemoteTest_DIE;
    theChannel->sendVector(0, 0, *sendData, 0);
    theChannel->sendVector(0, 0, *sendData, 0);
    
    // delete memory of signal vectors
    if (ctrlSignal != 0)
        delete ctrlSignal;
    if (daqSignal != 0)
        delete daqSignal;
    
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
    
    // delete memory of control points
    int i;
    if (trialCPs != 0)  {
        for (i=0; i<numTrialCPs; i++)  {
            if (trialCPs[i] != 0)
                delete trialCPs[i];
        }
        delete [] trialCPs;
    }
    if (outCPs != 0)  {
        for (i=0; i<numOutCPs; i++)  {
            if (outCPs[i] != 0)
                delete outCPs[i];
        }
        delete [] outCPs;
    }
    
    opserr << endln;
    opserr << "***********************************************************\n";
    opserr << "* The connection with the adapter element has been closed *\n";
    opserr << "***********************************************************\n";
    opserr << endln;
}


int ECSimFEAdapter::setup()
{
    if (ctrlSignal != 0)
        delete ctrlSignal;
    if (daqSignal != 0)
        delete daqSignal;
    
    // create control signal vector (start at location 1, location 0 is for flag)
    ctrlSignal = new Vector(&sData[1], numCtrlSignals);
    if (ctrlSignal == 0)  {
        opserr << "ECSimFEAdapter::setup() - failed to create ctrlSignal vector.\n";
        delete theChannel;
        exit(OF_ReturnType_failed);
    }
    
    // create daq signal vector (start at location 0)
    daqSignal = new Vector(&rData[0], numDaqSignals);
    if (daqSignal == 0)  {
        opserr << "ECSimFEAdapter::setup() - failed to create daqSignal vector.\n";
        delete theChannel;
        exit(OF_ReturnType_failed);
    }
    
    // send the data sizes to the adapter element
    ID idData(2*OF_Resp_All+1);
    for (int i=0; i<OF_Resp_All; i++)  {
        idData(i) = (*sizeCtrl)(i);
        idData(OF_Resp_All+i) = (*sizeDaq)(i);
    }
    idData(2*OF_Resp_All) = dataSize;
    theChannel->sendID(0, 0, idData, 0);
    
    // print experimental control information
    this->Print(opserr);
    
    opserr << "*****************\n";
    opserr << "* Running...... *\n";
    opserr << "*****************\n";
    opserr << endln;
    
    return OF_ReturnType_completed;
}


int ECSimFEAdapter::setSize(ID sizeT, ID sizeO)
{
    // check sizeTrial and sizeOut against sizes
    // specified in the control points
    // ECSimFEAdapter objects can use:
    //     disp, vel, accel, force and time for trial and
    //     disp, vel, accel, force and time for output
    
    // get maximum dof IDs for each trial response quantity
    ID maxdofT(OF_Resp_All);
    for (int i=0; i<numTrialCPs; i++)  {
        // get trial control point parameters
        int numSignals = trialCPs[i]->getNumSignal();
        ID dof = trialCPs[i]->getDOF();
        ID rsp = trialCPs[i]->getRspType();
        
        // loop through all the trial control point signals
        for (int j=0; j<numSignals; j++)  {
            dof(j)++;  // switch to 1-based indexing
            maxdofT(rsp(j)) = dof(j) > maxdofT(rsp(j)) ? dof(j) : maxdofT(rsp(j));
        }
    }
    // get maximum dof IDs for each output response quantity
    ID maxdofO(OF_Resp_All);
    for (int i=0; i<numOutCPs; i++)  {
        // get output control point parameters
        int numSignals = outCPs[i]->getNumSignal();
        ID dof = outCPs[i]->getDOF();
        ID rsp = outCPs[i]->getRspType();
        
        // loop through all the output control point signals
        for (int j=0; j<numSignals; j++)  {
            dof(j)++;  // switch to 1-based indexing
            maxdofO(rsp(j)) = dof(j) > maxdofO(rsp(j)) ? dof(j) : maxdofO(rsp(j));
        }
    }
    // now check if dof IDs are within limits
    for (int i=0; i<OF_Resp_All; i++)  {
        if ((maxdofT(i) != 0  &&  maxdofT(i) > sizeT(i)) || 
            (maxdofO(i) != 0  &&  maxdofO(i) > sizeO(i)))  {
            opserr << "ECSimFEAdapter::setSize() - wrong sizeTrial/Out\n"; 
            opserr << "see User Manual.\n";
            delete theChannel;
            exit(OF_ReturnType_failed);
        }
    }
    // finally assign sizes
    (*sizeCtrl) = maxdofT;
    (*sizeDaq)  = maxdofO;
    
    return OF_ReturnType_completed;
}


int ECSimFEAdapter::setTrialResponse(
    const Vector* disp,
    const Vector* vel,
    const Vector* accel,
    const Vector* force,
    const Vector* time)
{
    // loop through all the trial control points
    int k = 0;
    for (int i=0; i<numTrialCPs; i++)  {
        // get trial control point parameters
        int numSignals = trialCPs[i]->getNumSignal();
        ID dof = trialCPs[i]->getDOF();
        ID rsp = trialCPs[i]->getRspType();
        
        // loop through all the trial control point dofs
        for (int j=0; j<numSignals; j++)  {
            // assemble the control signal array
            if (rsp(j) == OF_Resp_Disp  &&  disp != 0)  {
                (*ctrlSignal)(k) = (*disp)(dof(j));
            }
            else if (rsp(j) == OF_Resp_Force  &&  force != 0)  {
                (*ctrlSignal)(k) = (*force)(dof(j));
            }
            else if (rsp(j) == OF_Resp_Time  &&  time != 0)  {
                (*ctrlSignal)(k) = (*time)(dof(j));
            }
            else if (rsp(j) == OF_Resp_Vel  &&  vel != 0)  {
                (*ctrlSignal)(k) = (*vel)(dof(j));
            }
            else if (rsp(j) == OF_Resp_Accel  &&  accel != 0)  {
                (*ctrlSignal)(k) = (*accel)(dof(j));
            }
            // filter control signal if the filter exists
            if (theCtrlFilters[rsp(j)] != 0)
                (*ctrlSignal)(k) = theCtrlFilters[rsp(j)]->filtering((*ctrlSignal)(k));
            k++;
        }
    }
    
    // send control signal array to controller
    k += this->control();
    
    return (k - numCtrlSignals);
}


int ECSimFEAdapter::getDaqResponse(Vector* disp,
    Vector* vel,
    Vector* accel,
    Vector* force,
    Vector* time)
{
    // get daq signal array from controller/daq
    int rValue = this->acquire();
    
    // loop through all the output control points
    int k = 0;
    for (int i=0; i<numOutCPs; i++)  {
        // get output control point parameters
        int numSignals = outCPs[i]->getNumSignal();
        ID dof = outCPs[i]->getDOF();
        ID rsp = outCPs[i]->getRspType();
        
        // loop through all the output control point dofs
        for (int j=0; j<numSignals; j++)  {
            // filter daq signal if the filter exists
            if (theDaqFilters[rsp(j)] != 0)
                (*daqSignal)(k) = theDaqFilters[rsp(j)]->filtering((*daqSignal)(k));
            // populate the daq response vectors
            if (rsp(j) == OF_Resp_Disp  &&  disp != 0)  {
                (*disp)(dof(j)) = (*daqSignal)(k);
            }
            else if (rsp(j) == OF_Resp_Force  &&  force != 0)  {
                (*force)(dof(j)) = (*daqSignal)(k);
            }
            else if (rsp(j) == OF_Resp_Time  &&  time != 0)  {
                (*time)(dof(j)) = (*daqSignal)(k);
            }
            else if (rsp(j) == OF_Resp_Vel  &&  vel != 0)  {
                (*vel)(dof(j)) = (*daqSignal)(k);
            }
            else if (rsp(j) == OF_Resp_Accel  &&  accel != 0)  {
                (*accel)(dof(j)) = (*daqSignal)(k);
            }
            k++;
        }
    }
    
    return (k + rValue - numDaqSignals);
}


int ECSimFEAdapter::commitState()
{
    return OF_ReturnType_completed;
}


ExperimentalControl *ECSimFEAdapter::getCopy()
{
    return new ECSimFEAdapter(*this);
}


Response* ECSimFEAdapter::setResponse(const char **argv, int argc,
    OPS_Stream &output)
{
    int i;
    char outputData[15];
    Response *theResponse = 0;
    
    output.tag("ExpControlOutput");
    output.attr("ctrlType",this->getClassType());
    output.attr("ctrlTag",this->getTag());
    
    // ctrl signals
    if (ctrlSignal != 0 && (
        strcmp(argv[0],"ctrlSig") == 0 ||
        strcmp(argv[0],"ctrlSignal") == 0 ||
        strcmp(argv[0],"ctrlSignals") == 0))
    {
        for (i=0; i<numCtrlSignals; i++)  {
            sprintf(outputData,"ctrlSignal%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 1, *ctrlSignal);
    }
    
    // daq signals
    if (daqSignal != 0 && (
        strcmp(argv[0],"daqSig") == 0 ||
        strcmp(argv[0],"daqSignal") == 0 ||
        strcmp(argv[0],"daqSignals") == 0))
    {
        for (i=0; i<numDaqSignals; i++)  {
            sprintf(outputData,"daqSignal%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 2, *daqSignal);
    }
    output.endTag();
    
    return theResponse;
}


int ECSimFEAdapter::getResponse(int responseID, Information &info)
{
    switch (responseID)  {
    case 1:  // ctrl signals
        return info.setVector(*ctrlSignal);
        
    case 2:  // daq signals
        return info.setVector(*daqSignal);
        
    default:
        return -1;
    }
}


void ECSimFEAdapter::Print(OPS_Stream &s, int flag)
{
    s << "****************************************************************\n";
    s << "* ExperimentalControl: " << this->getTag() << endln; 
    s << "*   type: ECSimFEAdapter\n";
    s << "*   ipAddress: " << ipAddress << ", ipPort: " << ipPort << endln;
    s << "*   trialCPs:";
    for (int i=0; i<numTrialCPs; i++)
        s << " " << trialCPs[i]->getTag();
    s << "\n*   outCPs:";
    for (int i=0; i<numOutCPs; i++)
        s << " " << outCPs[i]->getTag();
    s << "\n*   ctrlFilters:";
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
    s << "\n****************************************************************\n\n";
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
