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
    // open log file
    logFile = fopen("ECLabVIEW.log","w");
    if (logFile == 0)  {
        opserr << "ECLabVIEW::ECLabVIEW() - "
            << "fopen: could not open log file.\n";
        exit(OF_ReturnType_failed);
    }

    if (trialcps == 0 || outcps == 0)  {
      opserr << "ECLabVIEW::ECLabVIEW() - "
          << "null trialCPs or outCPs array passed.\n";
      exit(OF_ReturnType_failed);
    }
    trialCPs = trialcps;
    outCPs = outcps;

    // setup the connection
    theSocket = new TCP_Socket(ipPort, ipAddress);
    if (theSocket->setUpConnection() != 0)  {
        opserr << "ECLabVIEW::ECLabVIEW() - "
            << "failed to setup TCP connection to LabVIEW.\n";
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

    // allocate memory for the receive messages
    rData = new char [dataSize];
    recvData = new Message(rData, dataSize);

    // open a session with LabVIEW
    sprintf(sData,"open-session\tOpenFresco\n");
        fprintf(logFile,"%s",sData);
    sendData = new Message(sData,(int)strlen(sData));  // needed because of bug in LabVIEW-plugin
    theSocket->sendMsg(0, 0, *sendData, 0);
    theSocket->recvMsgUnknownSize(0, 0, *recvData, 0);
        fprintf(logFile,"%s",rData);

    if (strcmp(strtok(rData,"\t"),"OK") != 0)  {
        opserr << "ECLabVIEW::ECLabVIEW() - "
            << "failed to open a session with LabVIEW.\n";
        opserr << rData << endln;
        delete theSocket;
        exit(OF_ReturnType_failed);
    }

    // send parameters (needed for NEES-SAM & MiniMost -> remove later)
    sprintf(sData,"set-parameter\tOPFTransaction\tnstep\t1\n");
        fprintf(logFile,"%s",sData);
    sendData = new Message(sData,(int)strlen(sData));  // needed because of bug in LabVIEW-plugin
    theSocket->sendMsg(0, 0, *sendData, 0);
    theSocket->recvMsgUnknownSize(0, 0, *recvData, 0);
        fprintf(logFile,"%s",rData);

    if (strcmp(strtok(rData,"\t"),"OK") != 0)  {
        opserr << "ECLabVIEW::ECLabVIEW() - "
            << "failed to set parameter with LabVIEW.\n";
        opserr << rData << endln;
        delete theSocket;
        exit(OF_ReturnType_failed);
    }
}


ECLabVIEW::ECLabVIEW(const ECLabVIEW &ec)
    : ExperimentalControl(ec)
{
    // temporarily does nothing
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
        fprintf(logFile,"%s",sData);
    sendData = new Message(sData,(int)strlen(sData));  // needed because of bug in LabVIEW-plugin
    theSocket->sendMsg(0, 0, *sendData, 0);
    theSocket->recvMsgUnknownSize(0, 0, *recvData, 0);
        fprintf(logFile,"%s",rData);

    if (strcmp(strtok(rData,"\t"),"OK") != 0)  {
        opserr << "ECLabVIEW::~ECLabVIEW() - "
            << "failed to close the current session with LabVIEW.\n";
        opserr << rData << endln;
    }

    // close connection by destroying theSocket
    if (theSocket != 0)
        delete theSocket;

    // close output file
    fclose(logFile);

    opserr << endln;
    opserr << "********************************************\n";
    opserr << "* The session with LabVIEW has been closed *\n";
    opserr << "********************************************\n";
    opserr << endln;
}


int ECLabVIEW::setup()
{
    int rValue = 0;
    
    if (targDisp != 0)
        delete targDisp;
    if (targForce != 0)
        delete targForce;
    
    if ((*sizeCtrl)(OF_Resp_Disp) != 0)  {
        targDisp = new Vector((*sizeCtrl)(OF_Resp_Disp));
        targDisp->Zero();
    }
    if ((*sizeCtrl)(OF_Resp_Force) != 0)  {
        targForce = new Vector((*sizeCtrl)(OF_Resp_Force));
        targForce->Zero();
    }
    
    if (measDisp != 0)
        delete measDisp;
    if (measForce != 0)
        delete measForce;
    
    if ((*sizeDaq)(OF_Resp_Disp) != 0)  {
        measDisp = new Vector((*sizeDaq)(OF_Resp_Disp));
        measDisp->Zero();
    }
    if ((*sizeDaq)(OF_Resp_Force) != 0)  {
        measForce = new Vector((*sizeDaq)(OF_Resp_Force));
        measForce->Zero();
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
        sprintf(sData,"close-session\tOpenFresco\n");
            fprintf(logFile,"%s",sData);
        sendData = new Message(sData,(int)strlen(sData));  // needed because of bug in LabVIEW-plugin
        theSocket->sendMsg(0, 0, *sendData, 0);
        delete theSocket;
        exit(OF_ReturnType_failed);
    }
    
	do  {
        rValue += this->control();
        rValue += this->acquire();
        
        int i;
        opserr << "**************************************\n";
        opserr << "* Initial values of DAQ are:         *\n";
        opserr << "*                                    *\n";
        opserr << "* dspDaq = [";
        for (i=0; i<(*sizeDaq)(OF_Resp_Disp); i++)
            opserr << " " << (*measDisp)(i);
        opserr << " ]\n";
        opserr << "* frcDaq = [";
        for (i=0; i<(*sizeDaq)(OF_Resp_Force); i++)
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
            getchar();
            sprintf(sData,"close-session\tOpenFresco\n");
                fprintf(logFile,"%s",sData);
            sendData = new Message(sData,(int)strlen(sData));  // needed because of bug in LabVIEW-plugin
            theSocket->sendMsg(0, 0, *sendData, 0);
            delete theSocket;
            exit(OF_ReturnType_failed);
        } else if (c == 'r')  {
            getchar();
        }
    } while (c == 'r');
    
    opserr << "******************\n";
    opserr << "* Running......  *\n";
    opserr << "******************\n";
    opserr << endln;
    
    return rValue;
}


int ECLabVIEW::setSize(ID sizeT, ID sizeO)
{
    // check sizeTrial and sizeOut
    // for ECLabVIEW object
    
    // ECLabVIEW objects can only use 
    // disp and force for trial and disp and force for output
    // check these are available in sizeT/sizeO.
    int sizeTDisp = 0, sizeTForce = 0;
    int sizeODisp = 0, sizeOForce = 0;
    for (int i=0; i<numTrialCPs; i++)  {
        sizeTDisp += (trialCPs[i]->getSizeRespType())(OF_Resp_Disp);
        sizeTForce += (trialCPs[i]->getSizeRespType())(OF_Resp_Force);
    }
    for (int i=0; i<numOutCPs; i++)  {
        sizeODisp += (outCPs[i]->getSizeRespType())(OF_Resp_Disp);
        sizeOForce += (outCPs[i]->getSizeRespType())(OF_Resp_Force);
    }
    if ((sizeTDisp != 0 && sizeTDisp != sizeT(OF_Resp_Disp)) ||
        (sizeTForce != 0 && sizeTForce != sizeT(OF_Resp_Force)) ||
        (sizeODisp != 0 && sizeODisp != sizeO(OF_Resp_Disp)) ||
        (sizeOForce != 0 && sizeOForce != sizeO(OF_Resp_Force)))  {
        opserr << "ECLabVIEW::setSize() - wrong sizeTrial/Out\n"; 
        opserr << "see User Manual.\n";
        sprintf(sData,"close-session\tOpenFresco\n");
        sendData = new Message(sData,(int)strlen(sData));  // needed because of bug in LabVIEW-plugin
        theSocket->sendMsg(0, 0, *sendData, 0);
        delete theSocket;
        exit(OF_ReturnType_failed);
    }
    
    *sizeCtrl = sizeT;
    *sizeDaq  = sizeO;
    
    return OF_ReturnType_completed;
}


int ECLabVIEW::setTrialResponse(const Vector* disp,
    const Vector* vel,
    const Vector* accel,
    const Vector* force,
    const Vector* time)
{
    int i, rValue = 0;
    if (disp != 0)  {
        *targDisp = *disp;
        if (theCtrlFilters[OF_Resp_Disp] != 0)  {
            for (i=0; i<(*sizeCtrl)(OF_Resp_Disp); i++)
                (*targDisp)(i) = theCtrlFilters[OF_Resp_Disp]->filtering((*targDisp)(i));
        }
    }
    if (force != 0)  {
        *targForce = *force;
        if (theCtrlFilters[OF_Resp_Force] != 0)  {
            for (i=0; i<(*sizeCtrl)(OF_Resp_Force); i++)
                (*targForce)(i) = theCtrlFilters[OF_Resp_Force]->filtering((*targForce)(i));
        }
    }

    rValue = this->control();
    
    return rValue;
}


int ECLabVIEW::getDaqResponse(Vector* disp,
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
                (*measDisp)(i) = theDaqFilters[OF_Resp_Disp]->filtering((*measDisp)(i));
        }
        *disp = *measDisp;
    }
    if (force != 0)  {
        if (theDaqFilters[OF_Resp_Force] != 0)  {
            for (i=0; i<(*sizeDaq)(OF_Resp_Force); i++)
                (*measForce)(i) = theDaqFilters[OF_Resp_Force]->filtering((*measForce)(i));
        }
        *force = *measForce;
    }
        
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
    if (theCtrlFilters != 0)
        s << "*   ctrlFilter: " << *theCtrlFilters << endln;
    if (theDaqFilters != 0)
        s << "*   daqFilter: " << *theDaqFilters << endln;
    s << "****************************************************************\n";
    s << endln;
}


int ECLabVIEW::control()
{
    // get current local time to setup transaction ID
    time(&rawtime);
    ptm = localtime(&rawtime);
    sprintf(OPFTransactionID,"OPFTransaction%4d%02d%02d%02d%02d%02d",
        1900+ptm->tm_year,ptm->tm_mon,ptm->tm_mday,ptm->tm_hour,ptm->tm_min,ptm->tm_sec);

    // propose target values
    int dID = 0, fID = 0;
    sprintf(sData,"propose\t%s",OPFTransactionID);

    // loop through all the trial control points
    for (int i=0; i<numTrialCPs; i++)  {
        // append trial control point name
        if (i==0)
            sprintf(sData,"%s\tMDL-00-01",sData);
            //sprintf(sData,"%s\tCPNode%02d",sData,trialCPs[i]->getNodeTag());
        else
            sprintf(sData,"%s\tcontrol-point\tCPNode%02d",sData,trialCPs[i]->getNodeTag());

        // get trial control point parameters
        int ndm = trialCPs[i]->getNDM();
        int numDir = trialCPs[i]->getNumDirection();
        ID dir = trialCPs[i]->getDirection();
        ID resp = trialCPs[i]->getResponseType();
        Vector fact = trialCPs[i]->getFactor();
        const Vector *lowerLim = trialCPs[i]->getLowerLimit();
        const Vector *upperLim = trialCPs[i]->getUpperLimit();

        if (lowerLim == 0 || upperLim == 0)  {
            // loop through all the trial control point parameters
            // without checking if commands are within limits
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
                        << "requested direction is not supported.\n";
                    exit(OF_ReturnType_failed);
                }
                // append ParameterType and Parameter
                if (dir(j) < ndm && resp(j) == OF_Resp_Disp)  {
                    sprintf(sData,"%s\tdisplacement\t%.10E",sData,fact(j)*(*targDisp)(dID));
                    dID++;
                }
                else if (dir(j) < ndm && resp(j) == OF_Resp_Force)  {
                    sprintf(sData,"%s\tforce\t%.10E",sData,fact(j)*(*targForce)(fID));
                    fID++;
                }
                else if (dir(j) >= ndm && resp(j) == OF_Resp_Disp)  {
                    sprintf(sData,"%s\trotation\t%.10E",sData,fact(j)*(*targDisp)(dID));
                    dID++;
                }
                else if (dir(j) >= ndm && resp(j) == OF_Resp_Force)  {
                    sprintf(sData,"%s\tmoment\t%.10E",sData,fact(j)*(*targForce)(fID));
                    fID++;
                }
                else {
                    opserr << "ECLabVIEW::control() - "
                        << "requested response type is not supported.\n";
                    exit(OF_ReturnType_failed);
                }
            }
        }
        else  {
            // loop through all the trial control point parameters
            // and check if commands are within given limits
            double parameter; int c;
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
                        << "requested direction is not supported.\n";
                    exit(OF_ReturnType_failed);
                }
                // append ParameterType
                if (dir(j) < ndm && resp(j) == OF_Resp_Disp)  {
                    parameter = fact(j)*(*targDisp)(dID);
                    sprintf(sData,"%s\tdisplacement",sData);
                    dID++;
                }
                else if (dir(j) < ndm && resp(j) == OF_Resp_Force)  {
                    parameter = fact(j)*(*targForce)(fID);
                    sprintf(sData,"%s\tforce",sData);
                    fID++;
                }
                else if (dir(j) >= ndm && resp(j) == OF_Resp_Disp)  {
                    parameter = fact(j)*(*targDisp)(dID);
                    sprintf(sData,"%s\trotation",sData);
                    dID++;
                }
                else if (dir(j) >= ndm && resp(j) == OF_Resp_Force)  {
                    parameter = fact(j)*(*targForce)(fID);
                    sprintf(sData,"%s\tmoment",sData);
                    fID++;
                }
                else {
                    opserr << "ECLabVIEW::control() - "
                        << "requested response type is not supported.\n";
                    exit(OF_ReturnType_failed);
                }
                // check if parameter is within limits and append
                if (parameter < (*lowerLim)(j) || parameter > (*upperLim)(j))  {
                    opserr << "*************************************************\n";
                    opserr << "* WARNING - Control command exceeds the limits: *\n";
                    opserr << "*                                               *\n";
                    opserr << "* Limits = [" << (*lowerLim)(j) << "," << (*upperLim)(j) << "]";
                    opserr << " -> Command = " << parameter << endln;
                    opserr << "*                                               *\n";
                    opserr << "* Press 'Enter' to continue the test or         *\n";
                    opserr << "* 's' to saturate the command at the limits or  *\n";
                    opserr << "* 'c' to cancel the test                        *\n";
                    opserr << "*************************************************\n";
                    opserr << endln;
                    c = getchar();
                    if (c == 'c')  {
                        getchar();
                        sprintf(sData,"close-session\tOpenFresco\n");
                            fprintf(logFile,"%s",sData);
                        sendData = new Message(sData,(int)strlen(sData));  // needed because of bug in LabVIEW-plugin
                        theSocket->sendMsg(0, 0, *sendData, 0);
                        delete theSocket;
                        exit(OF_ReturnType_failed);
                    } else if (c == 's')  {
                        getchar();
                        parameter = (parameter < (*lowerLim)(j)) ? (*lowerLim)(j) : (*upperLim)(j);
                    }
                }
                sprintf(sData,"%s\t%.10E",sData,parameter);
            }
        }
    }
    sprintf(sData,"%s\n",sData);
        fprintf(logFile,"%s",sData);
    sendData = new Message(sData,(int)strlen(sData));  // needed because of bug in LabVIEW-plugin
    theSocket->sendMsg(0, 0, *sendData, 0);
    theSocket->recvMsgUnknownSize(0, 0, *recvData, 0);
        fprintf(logFile,"%s",rData);
    if (strcmp(strtok(rData,"\t"),"OK") != 0)  {
        opserr << "ECLabVIEW::control() - "
            << "proposed control values were not accepted.\n";
        opserr << rData << endln;
        exit(OF_ReturnType_failed);
    }

    // execute target values
    sprintf(sData,"execute\t%s\n",OPFTransactionID);
        fprintf(logFile,"%s",sData);
    sendData = new Message(sData,(int)strlen(sData));  // needed because of bug in LabVIEW-plugin
    theSocket->sendMsg(0, 0, *sendData, 0);
    theSocket->recvMsgUnknownSize(0, 0, *recvData, 0);
        fprintf(logFile,"%s",rData);
    if (strcmp(strtok(rData,"\t"),"OK") != 0)  {
        opserr << "ECLabVIEW::control() - "
            << "failed to execute proposed control values.\n";
        opserr << rData << endln;
        exit(OF_ReturnType_failed);
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
        sprintf(sData,"%s\tMDL-00-01",sData);
        //sprintf(sData,"%s\tCPNode%02d",sData,outCPs[i]->getNodeTag());
    }
    sprintf(sData,"%s\n",sData);
        fprintf(logFile,"%s",sData);
    sendData = new Message(sData,(int)strlen(sData));  // needed because of bug in LabVIEW-plugin
    theSocket->sendMsg(0, 0, *sendData, 0);

    // receive output control point daq values
    int direction, response;
    int dID = 0, fID = 0;
    for (int i=0; i<numOutCPs; i++)  {
        // disaggregate received data
        theSocket->recvMsgUnknownSize(0, 0, *recvData, 0);
            fprintf(logFile,"%s",rData);
        if (strcmp(strtok(rData,"\t"),"OK") != 0)  {
            opserr << "ECLabVIEW::acquire() - "
                << "failed to acquire control-point "
                << outCPs[i]->getTag() << " values.\n";
            opserr << rData << endln;
            exit(OF_ReturnType_failed);
        }
        tokenPtr = strtok(NULL,"\t");
        tokenPtr = strtok(NULL,"\t");
        if (strcmp(tokenPtr,OPFTransactionID) != 0)  {
            opserr << "ECLabVIEW::acquire() - "
                << "received wrong OPFTransactionID\n"
                << " want: " << OPFTransactionID
                << " but got: " << tokenPtr << endln;
            exit(OF_ReturnType_failed);
        }
        tokenPtr = strtok(NULL,"\t");
        sprintf(cpName,"MDL-00-01");
        //sprintf(cpName,"CPNode%02d",outCPs[i]->getNodeTag());
        if (strcmp(tokenPtr,cpName) != 0)  {
            opserr << "ECLabVIEW::acquire() - "
                << "received wrong control-point\n"
                << " want: " << cpName
                << " but got: " << tokenPtr << endln;
            exit(OF_ReturnType_failed);
        }
        tokenPtr = strtok(NULL,"\t");

        // get output control point parameters
        int ndf = trialCPs[i]->getNDF();
        int numDir = outCPs[i]->getNumDirection();
        ID dir = outCPs[i]->getDirection();
        ID resp = outCPs[i]->getResponseType();
        Vector fact = outCPs[i]->getFactor();
        ID sizeRespType = outCPs[i]->getSizeRespType();
        int sizeDisp = 0, sizeForce = 0;

        while (tokenPtr != NULL)  {
            strcpy(GeomType,tokenPtr);
            strcpy(ParamType,strtok(NULL,"\t"));
            Parameter = atof(strtok(NULL,"\t"));
            if (strcmp(GeomType,"x") == 0)  {
                if (strcmp(ParamType,"displacement") == 0)  {
                    direction = 0;
                    response  = OF_Resp_Disp;
                }
                else if (strcmp(ParamType,"force") == 0)  {
                    direction = 0;
                    response  = OF_Resp_Force;
                }
                else if (strcmp(ParamType,"rotation") == 0)  {
                    direction = 3;
                    response  = OF_Resp_Disp;
                }
                else if (strcmp(ParamType,"moment") == 0)  {
                    direction = 3;
                    response  = OF_Resp_Force;
                }
            }
            else if (strcmp(GeomType,"y") == 0)  {
                if (strcmp(ParamType,"displacement") == 0)  {
                    direction = 1;
                    response  = OF_Resp_Disp;
                }
                else if (strcmp(ParamType,"force") == 0)  {
                    direction = 1;
                    response  = OF_Resp_Force;
                }
                else if (strcmp(ParamType,"rotation") == 0)  {
                    direction = 4;
                    response  = OF_Resp_Disp;
                }
                else if (strcmp(ParamType,"moment") == 0)  {
                    direction = 4;
                    response  = OF_Resp_Force;
                }
            }
            else if (strcmp(GeomType,"z") == 0)  {
                if (strcmp(ParamType,"displacement") == 0)  {
                    direction = 2;
                    response  = OF_Resp_Disp;
                }
                else if (strcmp(ParamType,"force") == 0)  {
                    direction = 2;
                    response  = OF_Resp_Force;
                }
                else if (strcmp(ParamType,"rotation") == 0)  {
                    direction = ndf-1;
                    response  = OF_Resp_Disp;
                }
                else if (strcmp(ParamType,"moment") == 0)  {
                    direction = ndf-1;
                    response  = OF_Resp_Force;
                }
            }
            if (direction > ndf)  {
                opserr << "ECLabVIEW::acquire() - "
                    << "received wrong direction\n"
                    << " direction <= " << ndf
                    << " but got: " << direction << endln;
                exit(OF_ReturnType_failed);
            }

            // assemble displacement and force daq vectors
            if (response == 0)  {
                int id = dID;
                for (int j=0; j<numDir; j++)  {
                    if (resp(j) == response)  {
                        if (dir(j) == direction)  {
                            (*measDisp)(id) = fact(j)*Parameter;
                            sizeDisp++;
                        }
                        id++;
                    }
                }
            }
            else if (response == 3)  {
                int id = fID;
                for (int j=0; j<numDir; j++)  {
                    if (resp(j) == response)  {
                        if (dir(j) == direction)  {
                            (*measForce)(id) = fact(j)*Parameter;
                            sizeForce++;
                        }
                        id++;
                    }
                }
            }
            tokenPtr = strtok(NULL,"\t");
        }

        // check if received number of parameters is correct
        if (sizeDisp != sizeRespType(0) || sizeForce != sizeRespType(3))  {
            opserr << "ECLabVIEW::acquire() - "
                << "received wrong number of displacement or force parameters\n"
                << " want: " << sizeRespType(0) << " displacements" 
                << "and " << sizeRespType(3) << " forces.\n";
             exit(OF_ReturnType_failed);
        }
        dID += sizeRespType(0);
        fID += sizeRespType(3);
    }

    return OF_ReturnType_completed;
}


void ECLabVIEW::sleep(const clock_t wait)
{
    clock_t goal;
    goal = wait + clock();
    while (goal>clock());
}