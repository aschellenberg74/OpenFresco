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
// Created: 01/21
// Revision: A
//
// Description: This file contains the functions invoked when the user
// invokes the different OpenFresco server commands in the interpreter. 

#include <elementAPI.h>
#include <Domain.h>
#include <Node.h>
#include <TCP_Socket.h>
#include <TCP_SocketSSL.h>
#include <UDP_Socket.h>

#include <ExperimentalElement.h>
#include <ExperimentalSite.h>
#include <ActorExpSite.h>

#include <conio.h>


int OPF_startLabServer()
{
    if (OPS_GetNumRemainingInputArgs() != 1)  {
        opserr << "WARNING insufficient arguments\n"
            << "Want: startLabServer siteTag\n";
        return -1;
    }
    
    int siteTag;
    int numdata = 1;
    if (OPS_GetIntInput(&numdata, &siteTag) < 0) {
        opserr << "WARNING invalid startLabServer siteTag\n";
        return -1;
    }
    ActorExpSite *theExperimentalSite =
        dynamic_cast <ActorExpSite*> (OPF_getExperimentalSite(siteTag));
    if (theExperimentalSite != 0)  {
        // start server process
        opserr << "\nActorExpSite " << siteTag
            << " - now running..." << endln;
        theExperimentalSite->run();
    } else  {
        opserr << "WARNING actor experimental site not found\n";
        opserr << "unable to start expSite: " << siteTag << endln;
        return -1;
    }
    
    return 0;
}


int OPF_startLabServerInteractive()
{
    if (OPS_GetNumRemainingInputArgs() != 2) {
        opserr << "WARNING insufficient arguments\n"
            << "Want: startLabServerInteractive siteTag numSteps\n";
        return -1;
    }

    int siteTag, numSteps;
    bool exitYet = false;
    bool showMsg = true;
    int action = 'r';
    int step = 1;
    int numdata = 1;
    if (OPS_GetIntInput(&numdata, &siteTag) < 0) {
        opserr << "WARNING invalid startLabServerInteractive siteTag\n";
        return -1;
    }
    numdata = 1;
    if (OPS_GetIntInput(&numdata, &numSteps) < 0) {
        opserr << "WARNING invalid startLabServerInteractive numSteps\n";
        return -1;
    }
    ActorExpSite* theExperimentalSite =
        dynamic_cast <ActorExpSite*> (OPF_getExperimentalSite(siteTag));
    if (theExperimentalSite != 0) {
        // start server process and run for numSteps
        opserr << "\nActorExpSite " << siteTag
            << " - now interactively running for " << numSteps << " steps..." << endln;
        opserr << "Press 'p' to pause, 'r' to resume, 's' to step, or 'e' to end the test." << endln;
        while (step <= numSteps && exitYet == false) {
            // check if we got a keyboard input
            if (_kbhit())
                action = _getch();
            
            switch (action) {
            case 'r':
                // run or resume test
                if (!showMsg) {
                    opserr << "\nActorExpSite " << siteTag
                        << " - now resuming the test..." << endln;
                    showMsg = true;
                }
                theExperimentalSite->runTill(OF_RemoteTest_commitState);
                step++;
                break;
            case 'p':
                // pause test
                if (showMsg) {
                    opserr << "\nActorExpSite " << siteTag << " - at step "
                        << step << " - now pausing the test..." << endln;
                    showMsg = false;
                }
                break;
            case 's':
            case '0':
                // 10^0 times step test
                if (!showMsg) {
                    opserr << "\nActorExpSite " << siteTag << " - at step "
                        << step << " - now stepping the test..." << endln;
                    showMsg = false;
                }
                theExperimentalSite->runTill(OF_RemoteTest_commitState);
                action = 'p';
                step++;
                break;
            case '1':
                // 10^1 times step test
                if (!showMsg) {
                    opserr << "\nActorExpSite " << siteTag << " - at step "
                        << step << " - now 10x stepping the test..." << endln;
                    showMsg = false;
                }
                for (int s=0; s<10; s++) {
                    theExperimentalSite->runTill(OF_RemoteTest_commitState);
                    step++;
                }
                action = 'p';
                break;
            case '2':
                // 10^2 times step test
                if (!showMsg) {
                    opserr << "\nActorExpSite " << siteTag << " - at step "
                        << step << " - now 100x stepping the test..." << endln;
                    showMsg = false;
                }
                for (int s=0; s<100; s++) {
                    theExperimentalSite->runTill(OF_RemoteTest_commitState);
                    step++;
                }
                action = 'p';
                break;
            case '3':
                // 10^3 times step test
                if (!showMsg) {
                    opserr << "\nActorExpSite " << siteTag << " - at step "
                        << step << " - now 1000x stepping the test..." << endln;
                    showMsg = false;
                }
                for (int s=0; s<1000; s++) {
                    theExperimentalSite->runTill(OF_RemoteTest_commitState);
                    step++;
                }
                action = 'p';
                break;
            case '4':
                // 10^4 times step test
                if (!showMsg) {
                    opserr << "\nActorExpSite " << siteTag << " - at step "
                        << step << " - now 10000x stepping the test..." << endln;
                    showMsg = false;
                }
                for (int s = 0; s < 10000; s++) {
                    theExperimentalSite->runTill(OF_RemoteTest_commitState);
                    step++;
                }
                action = 'p';
                break;
            case 'e':
                // end test and stop server process
                opserr << "\nActorExpSite " << siteTag << " - at step "
                    << step << " - now shut down." << endln;
                theExperimentalSite->runTill(OF_RemoteTest_shutdown);
                exitYet = true;
                break;
            }
        }
    }
    else {
        opserr << "WARNING actor experimental site not found\n";
        opserr << "unable to interactively start expSite: " << siteTag << endln;
        return -1;
    }

    return 0;
}


int OPF_setupLabServer()
{
    if (OPS_GetNumRemainingInputArgs() != 1)  {
        opserr << "WARNING insufficient arguments\n"
            << "Want: setupLabServer siteTag\n";
        return -1;
    }
    
    int siteTag;
    int numdata = 1;
    if (OPS_GetIntInput(&numdata, &siteTag) < 0) {
        opserr << "WARNING invalid setupLabServer siteTag\n";
        return -1;
    }
    ActorExpSite *theExperimentalSite =
        dynamic_cast <ActorExpSite*> (OPF_getExperimentalSite(siteTag));
    if (theExperimentalSite != 0)  {
        // start server process and run till setup is done
        opserr << "\nActorExpSite " << siteTag
            << " - now being setup..." << endln;
        theExperimentalSite->runTill(OF_RemoteTest_setup);
    } else  {
        opserr << "WARNING actor experimental site not found\n";
        opserr << "unable to setup expSite: " << siteTag << endln;
        return -1;
    }
    
    return 0;
}


int OPF_stepLabServer()
{
    if (OPS_GetNumRemainingInputArgs() != 2)  {
        opserr << "WARNING insufficient arguments\n"
            << "Want: stepLabServer siteTag numSteps\n";
        return -1;
    }
    
    int siteTag, numSteps;
    int step = 1;
    int numdata = 1;
    if (OPS_GetIntInput(&numdata, &siteTag) < 0) {
        opserr << "WARNING invalid stepLabServer siteTag\n";
        return -1;
    }
    numdata = 1;
    if (OPS_GetIntInput(&numdata, &numSteps) < 0) {
        opserr << "WARNING invalid stepLabServer numSteps\n";
        return -1;
    }
    ActorExpSite *theExperimentalSite =
        dynamic_cast <ActorExpSite*> (OPF_getExperimentalSite(siteTag));
    if (theExperimentalSite != 0)  {
        // start server process and run for numSteps
        opserr << "\nActorExpSite " << siteTag
            << " - now running for " << numSteps << " steps..." << endln;
        while (step <= numSteps)  {
            theExperimentalSite->runTill(OF_RemoteTest_commitState);
            step++;
        }
    } else  {
        opserr << "WARNING actor experimental site not found\n";
        opserr << "unable to step expSite: " << siteTag << endln;
        return -1;
    }
    
    return 0;
}


int OPF_stopLabServer()
{
    if (OPS_GetNumRemainingInputArgs() != 1)  {
        opserr << "WARNING insufficient arguments\n"
            << "Want: stopLabServer siteTag\n";
        return -1;
    }
    
    int siteTag;
    int numdata = 1;
    if (OPS_GetIntInput(&numdata, &siteTag) < 0) {
        opserr << "WARNING invalid stopLabServer siteTag\n";
        return -1;
    }
    ActorExpSite *theExperimentalSite =
        dynamic_cast <ActorExpSite*> (OPF_getExperimentalSite(siteTag));
    if (theExperimentalSite != 0)  {
        // stop server process
        opserr << "\nActorExpSite " << siteTag
            << " - now shut down." << endln;
        theExperimentalSite->runTill(OF_RemoteTest_shutdown);
    } else  {
        opserr << "WARNING actor experimental site not found\n";
        opserr << "unable to stop expSite: " << siteTag << endln;
        return -1;
    }
    
    return 0;
}


int OPF_startSimAppSiteServer()
{
    if (OPS_GetNumRemainingInputArgs() < 2) {
        opserr << "WARNING insufficient arguments\n"
            << "Want: startSimAppSiteServer siteTag ipPort <-udp> <-ssl>\n";
        return -1;
    }
    
    int siteTag, ipPort;
    int ssl = 0, udp = 0;
    Channel* theChannel = 0;
    int numdata = 1;
    if (OPS_GetIntInput(&numdata, &siteTag) < 0) {
        opserr << "WARNING invalid startSimAppSiteServer siteTag\n";
        return -1;
    }
    ExperimentalSite* theExperimentalSite = OPF_getExperimentalSite(siteTag);
    if (theExperimentalSite == 0) {
        opserr << "WARNING experimental site not found\n";
        opserr << "startSimAppSiteServer expSite: " << siteTag << endln;
        return -1;
    }
    numdata = 1;
    if (OPS_GetIntInput(&numdata, &ipPort) < 0) {
        opserr << "WARNING invalid startSimAppSiteServer ipPort\n";
        return -1;
    }
    // optional parameters
    if (OPS_GetNumRemainingInputArgs() > 0) {
        const char* type = OPS_GetString();
        if (strcmp(type, "-ssl") == 0)
            ssl = 1;
        else if (strcmp(type, "-udp") == 0)
            udp = 1;
    }
    
    // setup the connection
    if (ssl) {
        theChannel = new TCP_SocketSSL(ipPort);
        if (theChannel != 0) {
            opserr << "\nSSL Channel successfully created: "
                << "Waiting for Simulation Application Client...\n";
        }
        else {
            opserr << "WARNING could not create SSL channel\n";
            return -1;
        }
    }
    else if (udp) {
        theChannel = new UDP_Socket(ipPort);
        if (theChannel != 0) {
            opserr << "\nUDP Channel successfully created: "
                << "Waiting for Simulation Application Client...\n";
        }
        else {
            opserr << "WARNING could not create UDP channel\n";
            return -1;
        }
    }
    else {
        theChannel = new TCP_Socket(ipPort);
        if (theChannel != 0) {
            opserr << "\nTCP Channel successfully created: "
                << "Waiting for Simulation Application Client...\n";
        }
        else {
            opserr << "WARNING could not create TCP channel\n";
            return -1;
        }
    }
    if (theChannel->setUpConnection() != 0) {
        opserr << "WARNING could not setup connection\n";
        return -1;
    }
    
    // get the data size for the experimental site
    int intData[2 * OF_Resp_All + 1];
    ID idData(intData, 2 * OF_Resp_All + 1);
    ID sizeCtrl(intData, OF_Resp_All);
    ID sizeDaq(&intData[OF_Resp_All], OF_Resp_All);
    int* dataSize = &intData[2 * OF_Resp_All];
    idData.Zero();
    
    theChannel->recvID(0, 0, idData, 0);
    theExperimentalSite->setSize(sizeCtrl, sizeDaq);
    opserr << "\nSimAppSiteServer with ExpSite " << siteTag
        << " has the following DOF sizes:" << endln;
    opserr << "   sizeCtrl = " << sizeCtrl << "   sizeDaq  = " << sizeDaq;
    
    // initialize the receive and send vectors
    Vector* rDisp = 0, * sDisp = 0;
    Vector* rVel = 0, * sVel = 0;
    Vector* rAccel = 0, * sAccel = 0;
    Vector* rForce = 0, * sForce = 0;
    Vector* rTime = 0, * sTime = 0;
    
    int id = 1;
    double* rData = new double[*dataSize];
    Vector* recvData = new Vector(rData, *dataSize);
    if (sizeCtrl(OF_Resp_Disp) != 0) {
        rDisp = new Vector(&rData[id], sizeCtrl(OF_Resp_Disp));
        id += sizeCtrl(OF_Resp_Disp);
    }
    if (sizeCtrl(OF_Resp_Vel) != 0) {
        rVel = new Vector(&rData[id], sizeCtrl(OF_Resp_Vel));
        id += sizeCtrl(OF_Resp_Vel);
    }
    if (sizeCtrl(OF_Resp_Accel) != 0) {
        rAccel = new Vector(&rData[id], sizeCtrl(OF_Resp_Accel));
        id += sizeCtrl(OF_Resp_Accel);
    }
    if (sizeCtrl(OF_Resp_Force) != 0) {
        rForce = new Vector(&rData[id], sizeCtrl(OF_Resp_Force));
        id += sizeCtrl(OF_Resp_Force);
    }
    if (sizeCtrl(OF_Resp_Time) != 0) {
        rTime = new Vector(&rData[id], sizeCtrl(OF_Resp_Time));
        id += sizeCtrl(OF_Resp_Time);
    }
    recvData->Zero();
    
    id = 0;
    double* sData = new double[*dataSize];
    Vector* sendData = new Vector(sData, *dataSize);
    if (sizeDaq(OF_Resp_Disp) != 0) {
        sDisp = new Vector(&sData[id], sizeDaq(OF_Resp_Disp));
        id += sizeDaq(OF_Resp_Disp);
    }
    if (sizeDaq(OF_Resp_Vel) != 0) {
        sVel = new Vector(&sData[id], sizeDaq(OF_Resp_Vel));
        id += sizeDaq(OF_Resp_Vel);
    }
    if (sizeDaq(OF_Resp_Accel) != 0) {
        sAccel = new Vector(&sData[id], sizeDaq(OF_Resp_Accel));
        id += sizeDaq(OF_Resp_Accel);
    }
    if (sizeDaq(OF_Resp_Force) != 0) {
        sForce = new Vector(&sData[id], sizeDaq(OF_Resp_Force));
        id += sizeDaq(OF_Resp_Force);
    }
    if (sizeDaq(OF_Resp_Time) != 0) {
        sTime = new Vector(&sData[id], sizeDaq(OF_Resp_Time));
        id += sizeDaq(OF_Resp_Time);
    }
    sendData->Zero();
    
    // start server loop
    opserr << "\nSimAppSiteServer with ExpSite " << siteTag
        << " - now running...\n";
    bool exitYet = false;
    while (!exitYet) {
        theChannel->recvVector(0, 0, *recvData, 0);
        int action = (int)rData[0];
        
        //opserr << "\nLOOP action: " << *recvData << endln;
        switch (action) {
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
            theExperimentalSite->commitState(rTime);
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
            //OPF_removeExperimentalSite(siteTag);
            //theExperimentalSite = 0;
            exitYet = true;
            break;
        default:
            opserr << "WARNING SimAppSiteServer invalid action "
                << action << " received\n";
            break;
        }
    }
    opserr << "\nSimAppSiteServer with ExpSite " << siteTag
        << " - shut down\n\n";
    
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
    delete[] rData;
    
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
    delete[] sData;
    
    return 0;
}


int OPF_startSimAppElemServer()
{
    if (OPS_GetNumRemainingInputArgs() < 2) {
        opserr << "WARNING insufficient arguments\n"
            << "Want: startSimAppElemServer eleTag ipPort <-udp> <-ssl>\n";
        return -1;
    }
    
    // get the domain
    Domain* theDomain = OPS_GetDomain();
    if (theDomain == 0) {
        opserr << "WARNING failed to get domain\n";
        return -1;
    }
    
    int eleTag, ipPort;
    int ssl = 0, udp = 0;
    Channel* theChannel = 0;
    int numdata = 1;
    if (OPS_GetIntInput(&numdata, &eleTag) < 0) {
        opserr << "WARNING invalid startSimAppElemServer eleTag\n";
        return -1;
    }
    ExperimentalElement* theExperimentalElement =
        dynamic_cast <ExperimentalElement*> (theDomain->getElement(eleTag));
    if (theExperimentalElement == 0) {
        opserr << "WARNING experimental element not found\n";
        opserr << "startSimAppElemServer expElement: " << eleTag << endln;
        return -1;
    }
    numdata = 1;
    if (OPS_GetIntInput(&numdata, &ipPort) < 0) {
        opserr << "WARNING invalid startSimAppElemServer ipPort\n";
        return -1;
    }
    // optional parameters
    if (OPS_GetNumRemainingInputArgs() > 0) {
        const char* type = OPS_GetString();
        if (strcmp(type, "-ssl") == 0)
            ssl = 1;
        else if (strcmp(type, "-udp") == 0)
            udp = 1;
    }
    
    // setup the connection
    if (ssl) {
        theChannel = new TCP_SocketSSL(ipPort);
        if (theChannel != 0) {
            opserr << "\nSSL Channel successfully created: "
                << "Waiting for Simulation Application Client...\n";
        }
        else {
            opserr << "WARNING could not create SSL channel\n";
            return -1;
        }
    }
    else if (udp) {
        theChannel = new UDP_Socket(ipPort);
        if (theChannel != 0) {
            opserr << "\nUDP Channel successfully created: "
                << "Waiting for Simulation Application Client...\n";
        }
        else {
            opserr << "WARNING could not create UDP channel\n";
            return -1;
        }
    }
    else {
        theChannel = new TCP_Socket(ipPort);
        if (theChannel != 0) {
            opserr << "\nTCP Channel successfully created: "
                << "Waiting for Simulation Application Client...\n";
        }
        else {
            opserr << "WARNING could not create TCP channel\n";
            return -1;
        }
    }
    if (theChannel->setUpConnection() != 0) {
        opserr << "WARNING could not setup connection\n";
        return -1;
    }
    
    // get the data size for the experimental element
    int intData[2 * OF_Resp_All + 1];
    ID idData(intData, 2 * OF_Resp_All + 1);
    ID sizeCtrl(intData, OF_Resp_All);
    ID sizeDaq(&intData[OF_Resp_All], OF_Resp_All);
    int* dataSize = &intData[2 * OF_Resp_All];
    idData.Zero();
    
    theChannel->recvID(0, 0, idData, 0);
    
    // check data size of experimental element
    int i, ndf = 0;
    const ID eleNodes = theExperimentalElement->getExternalNodes();
    int numNodes = eleNodes.Size();
    Node** theNodes = theExperimentalElement->getNodePtrs();
    
    for (i = 0; i < numNodes; i++) {
        ndf += theNodes[i]->getNumberDOF();
    }
    
    if ((sizeCtrl(OF_Resp_Disp) != 0 && sizeCtrl(OF_Resp_Disp) != ndf) ||
        (sizeCtrl(OF_Resp_Vel) != 0 && sizeCtrl(OF_Resp_Vel) != ndf) ||
        (sizeCtrl(OF_Resp_Accel) != 0 && sizeCtrl(OF_Resp_Accel) != ndf) ||
        (sizeCtrl(OF_Resp_Force) != 0 && sizeCtrl(OF_Resp_Force) != ndf) ||
        (sizeCtrl(OF_Resp_Time) != 0 && sizeCtrl(OF_Resp_Time) != 1)) {
        opserr << "\nSimAppElemServer with ExpElement " << eleTag
            << " - incorrect number of control DOF (ndf)\n"
            << "want: " << ndf << " but got: " << sizeCtrl << endln;
        return -1;
    }
    if ((sizeDaq(OF_Resp_Disp) != 0 && sizeDaq(OF_Resp_Disp) != ndf) ||
        (sizeDaq(OF_Resp_Vel) != 0 && sizeDaq(OF_Resp_Vel) != ndf) ||
        (sizeDaq(OF_Resp_Accel) != 0 && sizeDaq(OF_Resp_Accel) != ndf) ||
        (sizeDaq(OF_Resp_Force) != 0 && sizeDaq(OF_Resp_Force) != ndf) ||
        (sizeDaq(OF_Resp_Time) != 0 && sizeDaq(OF_Resp_Time) != 1)) {
        opserr << "\nSimAppElemServer with ExpElement " << eleTag
            << " - incorrect number of daq DOF (ndf)\n"
            << "want: " << ndf << " but got: " << sizeDaq << endln;
        return -1;
    }
    opserr << "\nSimAppElemServer with ExpElement " << eleTag
        << " has the following DOF sizes:" << endln;
    opserr << "   sizeCtrl = " << sizeCtrl << "   sizeDaq  = " << sizeDaq;

    // initialize the receive and send vectors
    Vector* rDisp = 0, * sDisp = 0;
    Vector* rVel = 0, * sVel = 0;
    Vector* rAccel = 0, * sAccel = 0;
    Vector* rForce = 0, * sForce = 0;
    Vector* rTime = 0, * sTime = 0;
    
    int id = 1;
    double* rData = new double[*dataSize];
    Vector* recvData = new Vector(rData, *dataSize);
    if (sizeCtrl(OF_Resp_Disp) != 0) {
        rDisp = new Vector(&rData[id], sizeCtrl(OF_Resp_Disp));
        id += sizeCtrl(OF_Resp_Disp);
    }
    if (sizeCtrl(OF_Resp_Vel) != 0) {
        rVel = new Vector(&rData[id], sizeCtrl(OF_Resp_Vel));
        id += sizeCtrl(OF_Resp_Vel);
    }
    if (sizeCtrl(OF_Resp_Accel) != 0) {
        rAccel = new Vector(&rData[id], sizeCtrl(OF_Resp_Accel));
        id += sizeCtrl(OF_Resp_Accel);
    }
    if (sizeCtrl(OF_Resp_Force) != 0) {
        rForce = new Vector(&rData[id], sizeCtrl(OF_Resp_Force));
        id += sizeCtrl(OF_Resp_Force);
    }
    if (sizeCtrl(OF_Resp_Time) != 0) {
        rTime = new Vector(&rData[id], sizeCtrl(OF_Resp_Time));
        id += sizeCtrl(OF_Resp_Time);
    }
    recvData->Zero();
    
    id = 0;
    double* sData = new double[*dataSize];
    Vector* sendData = new Vector(sData, *dataSize);
    if (sizeDaq(OF_Resp_Disp) != 0) {
        sDisp = new Vector(&sData[id], sizeDaq(OF_Resp_Disp));
        id += sizeDaq(OF_Resp_Disp);
    }
    if (sizeDaq(OF_Resp_Vel) != 0) {
        sVel = new Vector(&sData[id], sizeDaq(OF_Resp_Vel));
        id += sizeDaq(OF_Resp_Vel);
    }
    if (sizeDaq(OF_Resp_Accel) != 0) {
        sAccel = new Vector(&sData[id], sizeDaq(OF_Resp_Accel));
        id += sizeDaq(OF_Resp_Accel);
    }
    if (sizeDaq(OF_Resp_Force) != 0) {
        sForce = new Vector(&sData[id], sizeDaq(OF_Resp_Force));
        id += sizeDaq(OF_Resp_Force);
    }
    if (sizeDaq(OF_Resp_Time) != 0) {
        sTime = new Vector(&sData[id], sizeDaq(OF_Resp_Time));
        id += sizeDaq(OF_Resp_Time);
    }
    sendData->Zero();
    Matrix* sMatrix = new Matrix(sData, ndf, ndf);
    sMatrix->Zero();
    
    // start server loop
    opserr << "\nSimAppElemServer with ExpElement " << eleTag
        << " - now running...\n";
    Vector nodeData(1);
    bool exitYet = false;
    while (!exitYet) {
        theChannel->recvVector(0, 0, *recvData, 0);
        int action = (int)rData[0];
        
        //opserr << "\nLOOP action: " << *recvData << endln;
        switch (action) {
        case OF_RemoteTest_open:
            opserr << "\nConnected to GenericClient Element\n";
            break;
        case OF_RemoteTest_setup:
            opserr << "WARNING SimAppElemServer action setup "
                << "received which does nothing, continuing execution\n";
            break;
        case OF_RemoteTest_setTrialResponse:
            id = 0;
            for (i = 0; i < numNodes; i++) {
                ndf = theNodes[i]->getNumberDOF();
                nodeData.resize(ndf);
                if (rDisp != 0) {
                    nodeData.Extract(*rDisp, id);
                    theNodes[i]->setTrialDisp(nodeData);
                }
                if (rVel != 0) {
                    nodeData.Extract(*rVel, id);
                    theNodes[i]->setTrialVel(nodeData);
                }
                if (rAccel != 0) {
                    nodeData.Extract(*rAccel, id);
                    theNodes[i]->setTrialAccel(nodeData);
                }
                id += ndf;
            }
            if (rTime != 0)
                theDomain->setCurrentTime((*rTime)(0));
            theDomain->update();
            break;
        case OF_RemoteTest_commitState:
            theDomain->commit();
            break;
        case OF_RemoteTest_getDaqResponse:
            if (sDisp != 0)
                (*sDisp) = theExperimentalElement->getDisp();
            if (sVel != 0)
                (*sVel) = theExperimentalElement->getVel();
            if (sAccel != 0)
                (*sAccel) = theExperimentalElement->getAccel();
            if (sForce != 0)
                (*sForce) = theExperimentalElement->getResistingForce();
            if (sTime != 0)
                (*sTime) = theExperimentalElement->getTime();
            theChannel->sendVector(0, 0, *sendData, 0);
            break;
        case OF_RemoteTest_getDisp:
            (*sDisp) = theExperimentalElement->getDisp();
            theChannel->sendVector(0, 0, *sendData, 0);
            break;
        case OF_RemoteTest_getVel:
            (*sVel) = theExperimentalElement->getVel();
            theChannel->sendVector(0, 0, *sendData, 0);
            break;
        case OF_RemoteTest_getAccel:
            (*sAccel) = theExperimentalElement->getAccel();
            theChannel->sendVector(0, 0, *sendData, 0);
            break;
        case OF_RemoteTest_getForce:
            (*sForce) = theExperimentalElement->getResistingForce();
            theChannel->sendVector(0, 0, *sendData, 0);
            break;
        case OF_RemoteTest_getTime:
            (*sTime) = theExperimentalElement->getTime();
            theChannel->sendVector(0, 0, *sendData, 0);
            break;
        case OF_RemoteTest_getInitialStiff:
            (*sMatrix) = theExperimentalElement->getInitialStiff();
            theChannel->sendVector(0, 0, *sendData, 0);
            break;
        case OF_RemoteTest_getTangentStiff:
            (*sMatrix) = theExperimentalElement->getTangentStiff();
            theChannel->sendVector(0, 0, *sendData, 0);
            break;
        case OF_RemoteTest_getDamp:
            (*sMatrix) = theExperimentalElement->getDamp();
            theChannel->sendVector(0, 0, *sendData, 0);
            break;
        case OF_RemoteTest_getMass:
            (*sMatrix) = theExperimentalElement->getMass();
            theChannel->sendVector(0, 0, *sendData, 0);
            break;
        case OF_RemoteTest_DIE:
            exitYet = true;
            break;
        default:
            opserr << "WARNING SimAppElemServer invalid action "
                << action << " received\n";
            break;
        }
    }
    opserr << "\nSimAppElemServer with ExpElement " << eleTag
        << " - shut down\n\n";
    
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
    delete[] rData;
    
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
    delete sMatrix;
    delete sendData;
    delete[] sData;
    
    return 0;
}
