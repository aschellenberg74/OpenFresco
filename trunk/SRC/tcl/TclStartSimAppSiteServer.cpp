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

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 04/11
// Revision: A
//
// Description: This file contains the function invoked when the user
// invokes the startSimAppSiteServer command in the interpreter. 

#include <tcl.h>
#include <TCP_Socket.h>
#include <TCP_SocketSSL.h>
#include <UDP_Socket.h>

#include <ExperimentalSite.h>

extern ExperimentalSite *getExperimentalSite(int tag);


int TclStartSimAppSiteServer(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{ 
    if (argc < 3)  {
        opserr << "WARNING insufficient arguments\n"
            << "Want: startSimAppSiteServer siteTag ipPort <-ssl>\n";
        return TCL_ERROR;
    }
    
    int siteTag, ipPort;
    int ssl = 0, udp = 0;
    Channel *theChannel = 0;
    
    if (Tcl_GetInt(interp, argv[1], &siteTag) != TCL_OK)  {
        opserr << "WARNING invalid startSimAppSiteServer siteTag\n";
        return TCL_ERROR;
    }
    ExperimentalSite *theExperimentalSite = getExperimentalSite(siteTag);
    if (theExperimentalSite == 0)  {
        opserr << "WARNING experimental site not found\n";
        opserr << "startSimAppSiteServer expSite: " << siteTag << endln;
        return TCL_ERROR;
    }
    if (Tcl_GetInt(interp, argv[2], &ipPort) != TCL_OK)  {
        opserr << "WARNING invalid startSimAppSiteServer ipPort\n";
        return TCL_ERROR;
    }
    if (argc == 4)  {
        if (strcmp(argv[3], "-ssl") == 0)
            ssl = 1;
        else if (strcmp(argv[3], "-udp") == 0)
            udp = 1;
    }
    
    // setup the connection
    if (ssl)  {
        theChannel = new TCP_SocketSSL(ipPort);
        if (theChannel != 0) {
            opserr << "\nSSL Channel successfully created: "
                << "Waiting for Simulation Application Client...\n";
        } else {
            opserr << "WARNING could not create SSL channel\n";
            return TCL_ERROR;
        }
    }
    else if (udp)  {
        theChannel = new UDP_Socket(ipPort);
        if (theChannel != 0) {
            opserr << "\nUDP Channel successfully created: "
                << "Waiting for Simulation Application Client...\n";
        } else {
            opserr << "WARNING could not create UDP channel\n";
            return TCL_ERROR;
        }
    }
    else  {
        theChannel = new TCP_Socket(ipPort);
        if (theChannel != 0) {
            opserr << "\nTCP Channel successfully created: "
                << "Waiting for Simulation Application Client...\n";
        } else {
            opserr << "WARNING could not create TCP channel\n";
            return TCL_ERROR;
        }
    }
    if (theChannel->setUpConnection() != 0)  {
        opserr << "WARNING could not setup connection\n";
        return TCL_ERROR;
    }
    
    // get the data size for the experimental site
    int intData[2*OF_Resp_All+1];
    ID idData(intData, 2*OF_Resp_All+1);
    ID sizeCtrl(intData, OF_Resp_All);
    ID sizeDaq(&intData[OF_Resp_All], OF_Resp_All);
    int *dataSize = &intData[2*OF_Resp_All];
    idData.Zero();
    
    theChannel->recvID(0, 0, idData, 0);
    theExperimentalSite->setSize(sizeCtrl, sizeDaq);
    
    // initialize the receive and send vectors
    Vector *rDisp  = 0, *sDisp  = 0;
    Vector *rVel   = 0, *sVel   = 0;
    Vector *rAccel = 0, *sAccel = 0;
    Vector *rForce = 0, *sForce = 0;
    Vector *rTime  = 0, *sTime  = 0;
    
    int id = 1;
    double *rData = new double [*dataSize];
    Vector *recvData = new Vector(rData, *dataSize);
    if (sizeCtrl(OF_Resp_Disp) != 0)  {
        rDisp = new Vector(&rData[id], sizeCtrl(OF_Resp_Disp));
        id += sizeCtrl(OF_Resp_Disp);
    }
    if (sizeCtrl(OF_Resp_Vel) != 0)  {
        rVel = new Vector(&rData[id], sizeCtrl(OF_Resp_Vel));
        id += sizeCtrl(OF_Resp_Vel);
    }
    if (sizeCtrl(OF_Resp_Accel) != 0)  {
        rAccel = new Vector(&rData[id], sizeCtrl(OF_Resp_Accel));
        id += sizeCtrl(OF_Resp_Accel);
    }
    if (sizeCtrl(OF_Resp_Force) != 0)  {
        rForce = new Vector(&rData[id], sizeCtrl(OF_Resp_Force));
        id += sizeCtrl(OF_Resp_Force);
    }
    if (sizeCtrl(OF_Resp_Time) != 0)  {
        rTime = new Vector(&rData[id], sizeCtrl(OF_Resp_Time));
        id += sizeCtrl(OF_Resp_Time);
    }
    recvData->Zero();
    
    id = 0;
    double *sData = new double [*dataSize];
    Vector *sendData = new Vector(sData, *dataSize);
    if (sizeDaq(OF_Resp_Disp) != 0)  {
        sDisp = new Vector(&sData[id], sizeDaq(OF_Resp_Disp));
        id += sizeDaq(OF_Resp_Disp);
    }
    if (sizeDaq(OF_Resp_Vel) != 0)  {
        sVel = new Vector(&sData[id], sizeDaq(OF_Resp_Vel));
        id += sizeDaq(OF_Resp_Vel);
    }
    if (sizeDaq(OF_Resp_Accel) != 0)  {
        sAccel = new Vector(&sData[id], sizeDaq(OF_Resp_Accel));
        id += sizeDaq(OF_Resp_Accel);
    }
    if (sizeDaq(OF_Resp_Force) != 0)  {
        sForce = new Vector(&sData[id], sizeDaq(OF_Resp_Force));
        id += sizeDaq(OF_Resp_Force);
    }
    if (sizeDaq(OF_Resp_Time) != 0)  {
        sTime = new Vector(&sData[id], sizeDaq(OF_Resp_Time));
        id += sizeDaq(OF_Resp_Time);
    }
    sendData->Zero();
    
    // start server loop
    opserr << "\nSimAppSiteServer with ExpSite " << siteTag
        << " now running...\n";
    bool exitYet = false;
    while (!exitYet) {
        theChannel->recvVector(0, 0, *recvData, 0);
        int action = (int)rData[0];
        
        //opserr << "\nLOOP action: " << *recvData << endln;
        switch(action) {
        case OF_RemoteTest_open:
            opserr << "\nConnected to Experimental Element\n";
            break;
        case OF_RemoteTest_setup:
            opserr << "WARNING SimAppSiteServer action setup "
                << "received which does nothing, continuing execution\n";
            break;
        case OF_RemoteTest_setTrialResponse:
            theExperimentalSite->setTrialResponse(rDisp, rVel, rAccel, rForce, rTime);
            break;
        case OF_RemoteTest_commitState:
            theExperimentalSite->commitState();
            break;
        case OF_RemoteTest_getDaqResponse:
            theExperimentalSite->getDaqResponse(sDisp, sVel, sAccel, sForce, sTime);
            theChannel->sendVector(0, 0, *sendData, 0);
            break;
        case OF_RemoteTest_getDisp:
            (*sDisp) = theExperimentalSite->getDisp();
            theChannel->sendVector(0, 0, *sendData, 0);
            break;
        case OF_RemoteTest_getVel:
            (*sVel) = theExperimentalSite->getVel();
            theChannel->sendVector(0, 0, *sendData, 0);
            break;
        case OF_RemoteTest_getAccel:
            (*sAccel) = theExperimentalSite->getAccel();
            theChannel->sendVector(0, 0, *sendData, 0);
            break;
        case OF_RemoteTest_getForce:
            (*sForce) = theExperimentalSite->getForce();
            theChannel->sendVector(0, 0, *sendData, 0);
            break;
        case OF_RemoteTest_getTime:
            (*sTime) = theExperimentalSite->getTime();
            theChannel->sendVector(0, 0, *sendData, 0);
            break;
        case OF_RemoteTest_DIE:
            exitYet = true;
            delete theExperimentalSite;
            break;
        default:
            opserr << "WARNING SimAppSiteServer invalid action "
                << action << " received\n";
            break;
        }
    }
    opserr << "\nSimAppSiteServer with ExpSite " << siteTag
        << " shutdown\n\n";
    
    // delete allocated memory
    if (theChannel != 0)
        delete theChannel;
    
    if (rDisp != 0)
        delete rDisp;
    if (rVel != 0)
        delete rVel;
    if (rAccel != 0)
        delete rAccel;
    if (rForce != 0)
        delete rForce;
    if (rTime != 0)
        delete rTime;
    delete recvData;
    delete [] rData;
    
    if (sDisp != 0)
        delete sDisp;
    if (sVel != 0)
        delete sVel;
    if (sAccel != 0)
        delete sAccel;
    if (sForce != 0)
        delete sForce;
    if (sTime != 0)
        delete sTime;
    delete sendData;
    delete [] sData;
    
    return TCL_OK;
}
