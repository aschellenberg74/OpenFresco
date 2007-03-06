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
// Created: 01/07
// Revision: A
//
// Description: This file contains the implementation of the ECLabVIEW class.

#include "ECLabVIEW.h"


#include <ExperimentalCP.h>
#include <Message.h>
#include <TCP_Socket.h>
#include <time.h>


ECLabVIEW::ECLabVIEW(int tag,
    int nTrialCPs, ExperimentalCP **trialcps,
    int nOutCPs, ExperimentalCP **outcps,
    char *ipaddress, int ipport)
    : ExperimentalControl(tag),
    numTrialCPs(nTrialCPs), numOutCPs(nOutCPs),
    ipAddress(ipaddress), ipPort(ipport),
    theSocket(0), sData(0), sendData(0), rData(0), recvData(0),
    targDisp(0), targForce(0), measDisp(0), measForce(0)

{   
    if (trialcps == 0 || outcps == 0)  {
      opserr << "ECLabVIEW::ECLabVIEW() - "
          << "null trialCPs or outCPs array passed\n";
      exit(OF_ReturnType_failed);
    }
    trialCPs = trialcps;
    outCPs = outcps;

    // setup the connection
    theSocket = new TCP_Socket(ipPort, ipAddress);
    if (theSocket->setUpConnection() != 0)  {
        opserr << "ECLabVIEW::ECLabVIEW() - "
            << "failed to setup TCP connection to LabVIEW\n";
        delete theSocket;
        exit(OF_ReturnType_failed);
    }
    
    opserr << "*************************************************\n";
    opserr << "* The TCP/IP socket with address: " << ipAddress << "  *\n";
    opserr << "* and port: " << ipPort << " has been opened:              *\n";
    opserr << "*************************************************\n";
    opserr << endln;

    // allocate memory for the send messages
    const int dataSize = 512;
    sData = new char [dataSize];
    sendData = new Message(sData, dataSize);

    // allocate memory for the receive vectors
    rData = new char [dataSize];
    recvData = new Message(rData, dataSize);

    // open a session with LabVIEW
    sprintf(sData,"open-session\tOpenFresco\n");
    theSocket->sendMsg(0, 0, *sendData, 0);
    theSocket->recvMsg(0, 0, *recvData, 0);

    if (strtok(rData,"\t") != "OK")  {
        opserr << "ECLabVIEW::ECLabVIEW() - "
            << "failed to open a session with LabVIEW\n";
        delete theSocket;
        exit(OF_ReturnType_failed);
    }
}


ECLabVIEW::ECLabVIEW(const ECLabVIEW &ec)
    : ExperimentalControl(ec)
{
    
}


ECLabVIEW::~ECLabVIEW()
{
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
    
    // close the session with LabVIEW
    sprintf(sData,"close-session\tOpenFresco\n");
    theSocket->sendMsg(0, 0, *sendData, 0);
    theSocket->recvMsg(0, 0, *recvData, 0);

    if (strtok(rData,"\t") != "OK")  {
        opserr << "ECLabVIEW::~ECLabVIEW() - "
            << "failed to close the current session with LabVIEW\n";
    }

    // close connection by destroying theSocket
    if (theSocket != 0)
        delete theSocket;
}


int ECLabVIEW::setSize(ID sizeT, ID sizeO)
{
    // check sizeTrial and sizeOut
    // for ECLabVIEW object
    
    // ECLabVIEW objects only use 
    // disp for trial and disp and force for output
    // check these are available in sizeT/sizeO.
    
    if (sizeT[OF_Resp_Disp] == 0 ||
        sizeO[OF_Resp_Disp] == 0 || sizeO[OF_Resp_Force] == 0) {
        opserr << "ECLabVIEW::setSize - wrong sizeTrial/Out\n"; 
        opserr << "see User Manual.\n";
        sprintf(sData,"close-session\tOpenFresco\n");
        theSocket->sendMsg(0, 0, *sendData, 0);
        delete theSocket;
        return OF_ReturnType_failed;
    }
    
    *sizeCtrl = sizeT;
    *sizeDaq  = sizeO;
    
    return OF_ReturnType_completed;
}


int ECLabVIEW::setup()
{
    if (targDisp != 0)
        delete targDisp;
    if (targForce != 0)
        delete targForce;
    
    if ((*sizeCtrl)[OF_Resp_Disp] != 0)
        targDisp = new Vector((*sizeCtrl)[OF_Resp_Disp]);
    if ((*sizeCtrl)[OF_Resp_Force] != 0)
        targForce = new Vector((*sizeCtrl)[OF_Resp_Force]);
    
    if (measDisp != 0)
        delete measDisp;
    if (measForce != 0)
        delete measForce;
    
    if ((*sizeDaq)[OF_Resp_Disp] != 0)
        measDisp = new Vector((*sizeDaq)[OF_Resp_Disp]);
    if ((*sizeDaq)[OF_Resp_Force] != 0)
        measForce = new Vector((*sizeDaq)[OF_Resp_Force]);
    
    // print experimental control information
    this->Print(opserr);
    
    opserr << "****************************************************************\n";
    opserr << "* Make sure that offset values of controller are set to ZERO   *\n";
    opserr << "*                                                              *\n";
    opserr << "* Press 'Enter' to proceed or 'c' to cancel the initialization *\n";
    opserr << "****************************************************************\n";
    opserr << endln;
    char c = getchar();
    if (c == 'c')  {
        sprintf(sData,"close-session\tOpenFresco\n");
        theSocket->sendMsg(0, 0, *sendData, 0);
        delete theSocket;
        return OF_ReturnType_failed;
    }
    		
	do  {
        this->control();
        this->acquire();
        
        int i;
        opserr << "**************************************\n";
        opserr << "* Initial values of DAQ are:         *\n";
        opserr << "*                                    *\n";
        opserr << "* dspDaq = [";
        for (i=0; i<(*sizeDaq)[OF_Resp_Disp]; i++)
            opserr << " " << (*measDisp)(i);
        opserr << " ]\n";
        opserr << "* frcDaq = [";
        for (i=0; i<(*sizeDaq)[OF_Resp_Force]; i++)
            opserr << " " << (*measForce)(i);
        opserr << " ]\n";
        opserr << "*                                    *\n";
        opserr << "* Press 'Enter' to start the test or *\n";
        opserr << "* 'r' to repeat the measurement or   *\n";
        opserr << "* 'c' to cancel the initialization   *\n";
        opserr << "**************************************\n";
        opserr << endln;
        c = getchar();
        if (c == 'c')  {
            sprintf(sData,"close-session\tOpenFresco\n");
            theSocket->sendMsg(0, 0, *sendData, 0);
            delete theSocket;
            return OF_ReturnType_failed;
        } else if (c == 'r')  {
            getchar();
        }
    } while (c == 'r');
    
    opserr << "******************\n";
    opserr << "* Running......  *\n";
    opserr << "******************\n";
    opserr << endln;
    
    return OF_ReturnType_completed;
}


int ECLabVIEW::setTrialResponse(const Vector* disp,
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


int ECLabVIEW::getDaqResponse(Vector* disp,
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


int ECLabVIEW::commitState()
{	
    return OF_ReturnType_completed;
}


ExperimentalControl *ECLabVIEW::getCopy()
{
    return new ECLabVIEW(*this);
}


void ECLabVIEW::Print(OPS_Stream &s, int flag)
{
    s << "****************************************************************\n";
    s << "* ExperimentalControl: " << this->getTag() << endln; 
    s << "* type: ECLabVIEW\n";
    s << "*   ipAddress: " << ipAddress << ", ipPort: " << ipPort << endln;
    if (theFilter != 0) {
        s << "*\tFilter: " << *theFilter << endln;
    }
    s << "****************************************************************\n";
    s << endln;
}


int ECLabVIEW::control()
{
    // get current local time to setup transaction ID
    time(&rawtime);
    ptm = localtime(&rawtime);
    sprintf(OPFTransactionID,"OPFTransaction_%4d%02d%02d%02d%02d%02d",ptm->tm_year,ptm->tm_mon,ptm->tm_mday,ptm->tm_hour,ptm->tm_min,ptm->tm_sec);

    // propose target values
    int dID = 0, fID = 0;
    sprintf(sData,"propose\t%s",OPFTransactionID);
    
    // loop through all the trial control points
    for (int i=0; i<numTrialCPs; i++)  {
        // append trial control point name
        if (i==0)
            sprintf(sData,"%s\tCPNode_%02d",sData,trialCPs[i]->getNodeTag());
        else
            sprintf(sData,"%s\tcontrol-point\tCPNode_%02d",sData,trialCPs[i]->getNodeTag());

        // get trial control point parameters
        int numDir = trialCPs[i]->getNumDir();
        ID dir = trialCPs[i]->getDir();
        ID resp = trialCPs[i]->getResponseType();
        Vector fact = trialCPs[i]->getFactor();

        // loop through all the trial control point directions
        for (int j=0; j<numDir; j++)  {
            // append GeomType
            if (dir(j) == 0 || dir(j) == 3)  {
                sprintf(sData,"%s\tx",sData);
            }
            else if (dir(j) == 1 || dir(j) == 4)  {
                sprintf(sData,"%s\ty",sData);
            }
            else if (dir(j) == 2 || dir(j) == 5)  {
                sprintf(sData,"%s\tz",sData);
            }
            else {
                opserr << "ECLabVIEW::control() - "
                    << "requested direction is not supported\n";
                return OF_ReturnType_failed;
            }
            // append ParameterType and Parameter
            if (dir(j) < 3 && resp(j) == 0)  {
                sprintf(sData,"%s\tdisplacement\t%.10E",sData,fact(j)*(*targDisp)(dID));
                dID++;
            }
            else if (dir(j) < 3 && resp(j) == 3)  {
                sprintf(sData,"%s\tforce\t%.10E",sData,fact(j)*(*targForce)(fID));
                fID++;
            }
            else if (dir(j) > 2 && resp(j) == 0)  {
                sprintf(sData,"%s\trotation\t%.10E",sData,fact(j)*(*targDisp)(dID));
                dID++;
            }
            else if (dir(j) > 2 && resp(j) == 3)  {
                sprintf(sData,"%s\tmoment\t%.10E",sData,fact(j)*(*targForce)(fID));
                fID++;
            }
            else {
                opserr << "ECLabVIEW::control() - "
                    << "requested response type is not supported\n";
                return OF_ReturnType_failed;
            }
        }
    }
    sprintf(sData,"%s\n",sData);
    theSocket->sendMsg(0, 0, *sendData, 0);
    theSocket->recvMsg(0, 0, *recvData, 0);
    if (strtok(rData,"\t") != "OK")  {
        opserr << "ECLabVIEW::control() - "
            << "proposed control values were not accepted\n";
        return OF_ReturnType_failed;
    }

    // execute target values
    sprintf(sData,"execute\t%s\n",OPFTransactionID);
    theSocket->sendMsg(0, 0, *sendData, 0);
    theSocket->recvMsg(0, 0, *recvData, 0);
    if (strtok(rData,"\t") != "OK")  {
        opserr << "ECLabVIEW::control() - "
            << "failed to execute proposed control values\n";
        return OF_ReturnType_failed;
    }

    return OF_ReturnType_completed;
}


int ECLabVIEW::acquire()
{
    char *tokenPtr;
    char cpName[17];
    char GeomType[2];
    char ParamType[13];
    double Parameter;

    // send acquisition request
    sprintf(sData,"get-control-point\t%s",OPFTransactionID);
    for (int i=0; i<numOutCPs; i++)  {
        // append output control point name
        sprintf(sData,"%s\tCPNode_%02d",sData,outCPs[i]->getNodeTag());
    }
    sprintf(sData,"%s\n",sData);
    theSocket->sendMsg(0, 0, *sendData, 0);

    // receive output control point daq values
    int dID = 0, fID = 0;
    int direction, response;
    for (int i=0; i<numOutCPs; i++)  {
        // disaggregate received data
        theSocket->recvMsg(0, 0, *recvData, 0);
        if (strcmp(strtok(rData,"\t"),"OK") != 0 &&
            strcmp(strtok(NULL,"\t"),"0") != 0)  {
            opserr << "ECLabVIEW::acquire() - "
                << "failed to acquire control-point "
                << outCPs[i]->getTag() << " values\n";
            return OF_ReturnType_failed;
        }
        tokenPtr = strtok(NULL,"\t");
        if (strcmp(tokenPtr,OPFTransactionID) != 0)  {
            opserr << "ECLabVIEW::acquire() - "
                << "received wrong OPFTransactionID\n"
                << " want: " << OPFTransactionID << " but got: " << tokenPtr << endln;
            return OF_ReturnType_failed;
        }
        tokenPtr = strtok(NULL,"\t");
        sprintf(cpName,"CPNode_%02d",outCPs[i]->getNodeTag());
        if (strcmp(tokenPtr,cpName) != 0)  {
            opserr << "ECLabVIEW::acquire() - "
                << "received wrong control-point\n"
                << " want: " << cpName << " but got: " << tokenPtr << endln;
            return OF_ReturnType_failed;
        }
        tokenPtr = strtok(NULL,"\t");

        // get output control point parameters
        int numDir = outCPs[i]->getNumDir();
        ID dir = outCPs[i]->getDir();
        ID resp = outCPs[i]->getResponseType();
        Vector fact = outCPs[i]->getFactor();

        while (tokenPtr != NULL)  {
            strcpy(GeomType,tokenPtr);
            strcpy(ParamType,strtok(NULL,"\t"));
            Parameter = atof(strtok(NULL,"\t"));
            if (strcmp(GeomType,"x") == 0)  {
                if (strcmp(ParamType,"displacement") == 0)  {
                    direction = 0; response = 0;
                }
                else if (strcmp(ParamType,"force") == 0)  {
                    direction = 0; response = 3;
                }
                else if (strcmp(ParamType,"rotation") == 0)  {
                    direction = 3; response = 0;
                }
                else if (strcmp(ParamType,"moment") == 0)  {
                    direction = 3; response = 3;
                }
            }
            else if (strcmp(GeomType,"y") == 0)  {
                if (strcmp(ParamType,"displacement") == 0)  {
                    direction = 1; response = 0;
                }
                else if (strcmp(ParamType,"force") == 0)  {
                    direction = 1; response = 3;
                }
                else if (strcmp(ParamType,"rotation") == 0)  {
                    direction = 4; response = 0;
                }
                else if (strcmp(ParamType,"moment") == 0)  {
                    direction = 4; response = 3;
                }
            }
            else if (strcmp(GeomType,"z") == 0)  {
                if (strcmp(ParamType,"displacement") == 0)  {
                    direction = 2; response = 0;
                }
                else if (strcmp(ParamType,"force") == 0)  {
                    direction = 2; response = 3;
                }
                else if (strcmp(ParamType,"rotation") == 0)  {
                    direction = 5; response = 0;
                }
                else if (strcmp(ParamType,"moment") == 0)  {
                    direction = 5; response = 3;
                }
            }
            // loop through all the output control point directions
            int dNumDir = 0, fNumDir = 0;
            for (int j=0; j<numDir; j++)  {
                if (dir(j) == direction && resp(j) == response)  {
                    if (response == 0)  {
                        (*measDisp)(dID+j) = fact(j)*Parameter;
                        dNumDir++;
                    }
                    else if (response == 3)  {
                        (*measForce)(fID+j) = fact(j)*Parameter;
                        fNumDir++;
                    }
                }
            }
            tokenPtr = strtok(NULL,"\t");
            dID = dNumDir;
            fID = fNumDir;
        }
    }

    return OF_ReturnType_completed;
}


void ECLabVIEW::sleep(const clock_t wait)
{
    clock_t goal;
    goal = wait + clock();
    while (goal>clock());
}
