/* 
* tclAppInit.cpp --
*
*	Provides a default version of the main program and Tcl_AppInit
*	procedure for Tcl applications (without Tk).
*
* Copyright (c) 1994-1997 Sun Microsystems, Inc.
* Copyright (c) 1998-1999 by Scriptics Corporation.
*
* See the file "license.terms" for information on usage and
* redistribution of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

/*                            MODIFIED FOR                            */

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

// Modified: Frank McKenna & Andreas Schellenberg
// Created: 10/06
// Revision: A

extern "C" {
#include <tcl.h>
//#include <tk.h>
}

/*
* The following variable is a special hack that is needed in order for
* Sun shared libraries to be used for Tcl.
*/

#ifdef _UNIX
//extern "C" int matherr();
//int *tclDummyMathPtr = (int *) matherr;
#endif /* UNIX */

#ifdef TCL_TEST
extern "C" {
#include "tclInt.h"
}

extern int		Procbodytest_Init _ANSI_ARGS_((Tcl_Interp *interp));
extern int		Procbodytest_SafeInit _ANSI_ARGS_((Tcl_Interp *interp));
extern int		TclObjTest_Init _ANSI_ARGS_((Tcl_Interp *interp));
extern int		Tcltest_Init _ANSI_ARGS_((Tcl_Interp *interp));
#ifdef TCL_THREADS
extern int		TclThread_Init _ANSI_ARGS_((Tcl_Interp *interp));
#endif
#endif /* TCL_TEST */

#ifdef TCL_XT_TEST
extern void		XtToolkitInitialize _ANSI_ARGS_((void));
extern int		Tclxttest_Init _ANSI_ARGS_((Tcl_Interp *interp));
#endif /* TCL_XT_TEST */

#include <Domain.h>
#include <TclModelBuilder.h>
#include <FileStream.h>
#include <SimulationInformation.h>
#include <StandardStream.h>

#include <Node.h>
#include <ExperimentalElement.h>
#include <ExperimentalSite.h>
#include <ActorExpSite.h>

#include <TCP_Socket.h>
#include <TCP_SocketSSL.h>
#include <Matrix.h>

Domain *theDomain = 0;
TclModelBuilder *theTclBuilder = 0;

SimulationInformation simulationInfo;
char *simulationInfoOutputFilename;

/*
*----------------------------------------------------------------------
*
* main --
*
*	This is the main program for the application.
*
* Results:
*	None: Tcl_Main never returns here, so this procedure never
*	returns either.
*
* Side effects:
*	Whatever the application does.
*
*----------------------------------------------------------------------
*/
extern void tclMain(int argc, char **argv, Tcl_AppInitProc *appInitProc);

int main(int argc, char **argv)
{
    /*
    * The following #if block allows you to change the AppInit
    * function by using a #define of TCL_LOCAL_APPINIT instead
    * of rewriting this entire file.  The #if checks for that
    * #define and uses Tcl_AppInit if it doesn't exist.
    */

#ifndef TCL_LOCAL_APPINIT
#define TCL_LOCAL_APPINIT Tcl_AppInit    
#endif

    /* fmk - comment out the following block to get to compile 
    extern "C" int TCL_LOCAL_APPINIT _ANSI_ARGS_((Tcl_Interp *interp));
    fmk - end commented block */

    /*
    * The following #if block allows you to change how Tcl finds the startup
    * script, prime the library or encoding paths, fiddle with the argv,
    * etc., without needing to rewrite Tcl_Main()
    */

#ifdef TCL_LOCAL_MAIN_HOOK
    extern int TCL_LOCAL_MAIN_HOOK _ANSI_ARGS_((int *argc, char ***argv));
#endif

#ifdef TCL_XT_TEST
    XtToolkitInitialize();
#endif

#ifdef TCL_LOCAL_MAIN_HOOK
    TCL_LOCAL_MAIN_HOOK(&argc, &argv);
#endif

    tclMain(argc, argv, TCL_LOCAL_APPINIT);

    return 0;   /* Needed only to prevent compiler warning. */
}


extern ExperimentalSite *getExperimentalSite(int tag);

// experimental control point commands
extern int TclExpCPCommand(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, Domain *theDomain, TclModelBuilder *theTclBuilder);

int openFresco_addExperimentalCP(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{ 
    return TclExpCPCommand(clientData, interp, argc, argv,
        theDomain, theTclBuilder);
}

// experimental signal filter commands
extern int TclExpSignalFilterCommand(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, Domain *theDomain, TclModelBuilder *theTclBuilder);

int openFresco_addExperimentalSignalFilter(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{ 
    return TclExpSignalFilterCommand(clientData, interp, argc, argv,
        theDomain, theTclBuilder);
}

// experimental control commands
extern int TclExpControlCommand(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, Domain *theDomain, TclModelBuilder *theTclBuilder);

int openFresco_addExperimentalControl(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{ 
    return TclExpControlCommand(clientData, interp, argc, argv,
        theDomain, theTclBuilder);
}

// experimental setup commands
extern int TclExpSetupCommand(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, Domain *theDomain, TclModelBuilder *theTclBuilder);

int openFresco_addExperimentalSetup(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{ 
    return TclExpSetupCommand(clientData, interp, argc, argv,
        theDomain, theTclBuilder);
}

// experimental site commands
extern int TclExpSiteCommand(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, Domain *theDomain, TclModelBuilder *theTclBuilder);

int openFresco_addExperimentalSite(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{ 
    return TclExpSiteCommand(clientData, interp, argc, argv,
        theDomain, theTclBuilder);
}

// experimental element commands
extern int TclExpElementCommand(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, Domain *theDomain, TclModelBuilder *theTclBuilder);

int openFresco_addExperimentalElement(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{ 
    return TclExpElementCommand(clientData, interp, argc, argv,
        theDomain, theTclBuilder);
}


// start laboratory server command
int openFresco_startLabServer(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{ 
    if (argc != 2)  {
        opserr << "WARNING insufficient arguments\n"
            << "Want: startLabServer siteTag\n";
        return TCL_ERROR;
    }

    int siteTag;

    if (Tcl_GetInt(interp, argv[1], &siteTag) != TCL_OK)  {
        opserr << "WARNING invalid startLabServer siteTag\n";
        return TCL_ERROR;
    }
    ActorExpSite *theExperimentalSite =
        dynamic_cast <ActorExpSite*> (getExperimentalSite(siteTag));
    if (theExperimentalSite != 0)  {
        // start server process
        opserr << "\nActorExpSite " << siteTag
            << " now running..." << endln;
        theExperimentalSite->run();
    } else  {
        opserr << "WARNING actor experimental site not found\n";
        opserr << "unable to start expSite: " << siteTag << endln;
        return TCL_ERROR;
    }
    delete theExperimentalSite;

    return TCL_OK;
}


// start simulation application site server command
int openFresco_startSimAppSiteServer(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{ 
    if (argc < 3)  {
        opserr << "WARNING insufficient arguments\n"
            << "Want: startSimAppSiteServer siteTag ipPort <-ssl>\n";
        return TCL_ERROR;
    }

    int siteTag, ipPort;
    int ssl = 0;
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
    }

    // setup the connection
    if (!ssl)  {
        theChannel = new TCP_Socket(ipPort);
        if (theChannel != 0) {
            opserr << "\nChannel successfully created: "
                << "Waiting for Simulation Application Client...\n";
        } else {
            opserr << "WARNING could not create channel\n";
            return TCL_ERROR;
        }
    }
    else  {
        theChannel = new TCP_SocketSSL(ipPort);
        if (theChannel != 0) {
            opserr << "\nSSL Channel successfully created: "
                << "Waiting for Simulation Application Client...\n";
        } else {
            opserr << "WARNING could not create SSL channel\n";
            return TCL_ERROR;
        }
    }
    theChannel->setUpConnection();

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
        << " now running..." << endln;
    bool exitYet = false;
    while (!exitYet) {
        theChannel->recvVector(0, 0, *recvData, 0);
        int action = (int)rData[0];
        //opserr << "\nLOOP action: " << *recvData << endln;

        switch(action) {
        case OF_RemoteTest_setTrialResponse:
            theExperimentalSite->setTrialResponse(rDisp, rVel, rAccel, rForce, rTime);
            break;
        case OF_RemoteTest_getDaqResponse:
            theExperimentalSite->getDaqResponse(sDisp, sVel, sAccel, sForce, sTime);
            theChannel->sendVector(0, 0, *sendData, 0);
            break;
        case OF_RemoteTest_commitState:
            theExperimentalSite->commitState();
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
                << action << " received" << endln;
            break;
        }
    }
    opserr << "\nSimAppSiteServer with ExpSite " << siteTag
        << " shutdown\n" << endln;

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


// start simulation application element server command
int openFresco_startSimAppElemServer(ClientData clientData,
    Tcl_Interp *interp, int argc, TCL_Char **argv)
{ 
    if (argc < 3)  {
        opserr << "WARNING insufficient arguments\n"
            << "Want: startSimAppElemServer eleTag ipPort <-ssl>\n";
        return TCL_ERROR;
    }

    int eleTag, ipPort;
    int ssl = 0;
    Channel *theChannel = 0;

    if (Tcl_GetInt(interp, argv[1], &eleTag) != TCL_OK)  {
        opserr << "WARNING invalid startSimAppElemServer eleTag\n";
        return TCL_ERROR;
    }
    ExperimentalElement *theExperimentalElement =
        dynamic_cast <ExperimentalElement*> (theDomain->getElement(eleTag));
    if (theExperimentalElement == 0)  {
        opserr << "WARNING experimental element not found\n";
        opserr << "startSimAppElemServer expElement: " << eleTag << endln;
        return TCL_ERROR;
    }
    if (Tcl_GetInt(interp, argv[2], &ipPort) != TCL_OK)  {
        opserr << "WARNING invalid startSimAppElemServer ipPort\n";
        return TCL_ERROR;
    }
    if (argc == 4)  {
        if (strcmp(argv[3], "-ssl") == 0)
            ssl = 1;
    }

    // setup the connection
    if (!ssl)  {
        theChannel = new TCP_Socket(ipPort);
        if (theChannel != 0) {
            opserr << "\nChannel successfully created: "
                << "Waiting for Simulation Application Client...\n";
        } else {
            opserr << "WARNING could not create channel\n";
            return TCL_ERROR;
        }
    }
    else  {
        theChannel = new TCP_SocketSSL(ipPort);
        if (theChannel != 0) {
            opserr << "\nSSL Channel successfully created: "
                << "Waiting for Simulation Application Client...\n";
        } else {
            opserr << "WARNING could not create SSL channel\n";
            return TCL_ERROR;
        }
    }
    theChannel->setUpConnection();

    // get the data size for the experimental element
    int intData[2*OF_Resp_All+1];
    ID idData(intData, 2*OF_Resp_All+1);
    ID sizeCtrl(intData, OF_Resp_All);
    ID sizeDaq(&intData[OF_Resp_All], OF_Resp_All);
    int *dataSize = &intData[2*OF_Resp_All];
    idData.Zero();

    theChannel->recvID(0, 0, idData, 0);

    // check data size of experimental element
    int i, ndf = 0;
    const ID eleNodes = theExperimentalElement->getExternalNodes();
    int numNodes = eleNodes.Size();
    Node **theNodes = theExperimentalElement->getNodePtrs();

    for (i=0; i<numNodes; i++) {
        ndf += theNodes[i]->getNumberDOF();
    }

    if ((sizeCtrl(OF_Resp_Disp) != 0 && sizeCtrl(OF_Resp_Disp) != ndf) ||
        (sizeCtrl(OF_Resp_Vel) != 0 && sizeCtrl(OF_Resp_Vel) != ndf) ||
        (sizeCtrl(OF_Resp_Accel) != 0 && sizeCtrl(OF_Resp_Accel) != ndf) ||
        (sizeCtrl(OF_Resp_Force) != 0 && sizeCtrl(OF_Resp_Force) != ndf) ||
        (sizeCtrl(OF_Resp_Time) != 0 && sizeCtrl(OF_Resp_Time) != 1)) {
        opserr << "WARNING incorrect number of control degrees of freedom (ndf)\n";
        opserr << "want: " << ndf << " but got: " << sizeCtrl << endln;
        return TCL_ERROR;
    }
    if ((sizeDaq(OF_Resp_Disp) != 0 && sizeDaq(OF_Resp_Disp) != ndf) ||
        (sizeDaq(OF_Resp_Vel) != 0 && sizeDaq(OF_Resp_Vel) != ndf) ||
        (sizeDaq(OF_Resp_Accel) != 0 && sizeDaq(OF_Resp_Accel) != ndf) ||
        (sizeDaq(OF_Resp_Force) != 0 && sizeDaq(OF_Resp_Force) != ndf) ||
        (sizeDaq(OF_Resp_Time) != 0 && sizeDaq(OF_Resp_Time) != 1)) {
        opserr << "WARNING incorrect number of daq degrees of freedom (ndf)\n";
        opserr << "want: " << ndf << " but got: " << sizeDaq << endln;
        return TCL_ERROR;
    }
    
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
    Matrix *sMatrix = new Matrix(sData, ndf, ndf);
    sMatrix->Zero();
    
    // start server loop
    opserr << "\nSimAppElemServer with ExpElement " << eleTag
        << " now running..." << endln;
    Vector nodeData(1);
    bool exitYet = false;
    while (!exitYet) {
        theChannel->recvVector(0, 0, *recvData, 0);
        int action = (int)rData[0];
        //opserr << "\nLOOP action: " << *recvData << endln;

        switch(action) {
        case OF_RemoteTest_setTrialResponse:
            id = 0;
            for (i=0; i<numNodes; i++) {
                ndf = theNodes[i]->getNumberDOF();
                nodeData.resize(ndf);
                if (rDisp != 0) {
                    nodeData.Extract(*rDisp,id);
                    theNodes[i]->setTrialDisp(nodeData);
                }
                if (rVel != 0) {
                    nodeData.Extract(*rVel,id);
                    theNodes[i]->setTrialVel(nodeData);
                }
                if (rAccel != 0) {
                    nodeData.Extract(*rAccel,id);
                    theNodes[i]->setTrialAccel(nodeData);
                }
                id += ndf;
            }
            if (rTime != 0)
                theDomain->setCurrentTime((*rTime)(0));
            theDomain->update();
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
        case OF_RemoteTest_commitState:
            theDomain->commit();
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
        case OF_RemoteTest_getTangentStiff:
            (*sMatrix) = theExperimentalElement->getTangentStiff();
            theChannel->sendVector(0, 0, *sendData, 0);
            break;
        case OF_RemoteTest_getInitialStiff:
            (*sMatrix) = theExperimentalElement->getInitialStiff();
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
            delete theExperimentalElement;
            break;
        default:
            opserr << "WARNING SimAppElemServer invalid action "
                << action << " received" << endln;
            break;
        }
    }
    opserr << "\nSimAppElemServer with ExpElement " << eleTag
        << " shutdown\n" << endln;

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
    delete sMatrix;
    delete sendData;
    delete [] sData;

    return TCL_OK;
}


// model builder command
int specifyModelBuilder(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv)
{
    // make sure at least one other argument contains model builder type
    if (argc < 2) {
        opserr << "WARNING need to specify a model type, valid types:\n";
        opserr << "\tBasicBuilder\n";
        return TCL_ERROR;
    }    

    // invoke the destructor on the old builder
    if (theTclBuilder != 0) {
        delete theTclBuilder;
        theTclBuilder = 0;
    }

    // check argv[1] for type of ModelBuilder and create the object 
    if (strcmp(argv[1],"basicBuilder") == 0 ||
        strcmp(argv[1],"BasicBuilder") == 0) {
        int ndm = 0;
        int ndf = 0;

        if (argc < 4) {
            opserr << "WARNING incorrect number of command arguments\n";
            opserr << "model modelBuilderType -ndm ndm? <-ndf ndf?>\n";
            return TCL_ERROR;
        }

        int argPos = 2;
        while (argPos < argc) {
            if (strcmp(argv[argPos],"-ndm") == 0 ||
                strcmp(argv[argPos],"-NDM") == 0) {	
                argPos++;
                if (argPos < argc) {
                    if (Tcl_GetInt(interp, argv[argPos], &ndm) != TCL_OK) {
                        opserr << "WARNING error reading ndm: " << argv[argPos];
                        opserr << "\nmodel modelBuilderType -ndm ndm? <-ndf ndf?>\n";
                        return TCL_ERROR;
                    }	  
                    argPos++;
                }
            }
            else if (strcmp(argv[argPos],"-ndf") == 0 ||
                strcmp(argv[argPos],"-NDF") == 0) {	
                argPos++;
                if (argPos < argc) {
                    if (Tcl_GetInt(interp, argv[argPos], &ndf) != TCL_OK) {
                        opserr << "WARNING error reading ndf: " << argv[argPos];
                        opserr << "\nmodel modelBuilderType -ndm ndm? <-ndf ndf?>\n";
                        return TCL_ERROR;
                    }	  
                    argPos++;
                }
            }
            else
                argPos++;
        }

        // check that ndm was specified
        if (ndm == 0) {
            opserr << "WARNING need to specify ndm\n";
            opserr << "model modelBuilderType -ndm ndm? <-ndf ndf?>\n";
            return TCL_ERROR;
        }

        // check for ndf, if not assume one
        if (ndf == 0) {
            if (ndm == 1) 
                ndf = 1;
            else if (ndm == 2)
                ndf = 3;
            else if (ndm == 3)
                ndf = 6;
            else {
                opserr << "WARNING specified ndm, " << ndm << ", will not work\n";
                opserr << "with any elements in BasicBuilder\n";
                return TCL_ERROR;
            }
        }

        // create the model builder
        theTclBuilder = new TclModelBuilder(*theDomain, interp, ndm, ndf);
        if (theTclBuilder == 0) {
            opserr << "WARNING ran out of memory in creating BasicBuilder model\n";
            return TCL_ERROR;
        }
    }
    else {
        Tcl_SetResult(interp, "WARNING unknown model builder type", TCL_STATIC);

        opserr << "WARNING model builder type " << argv[1]
            << " not supported\n";
        return TCL_ERROR;
    }

    return TCL_OK;
}


/*
*----------------------------------------------------------------------
*
* Tcl_AppInit --
*
*	This procedure performs application-specific initialization.
*	Most applications, especially those that incorporate additional
*	packages, will have their own version of this procedure.
*
* Results:
*	Returns a standard Tcl completion code, and leaves an error
*	message in the interp's result if an error occurs.
*
* Side effects:
*	Depends on the startup script.
*
*----------------------------------------------------------------------
*/
int Tcl_AppInit(Tcl_Interp *interp)
{
    if (Tcl_Init(interp) == TCL_ERROR) {
        return TCL_ERROR;
    }

#ifdef TCL_TEST
#ifdef TCL_XT_TEST
    if (Tclxttest_Init(interp) == TCL_ERROR) {
        return TCL_ERROR;
    }
#endif
    if (Tcltest_Init(interp) == TCL_ERROR) {
        return TCL_ERROR;
    }
    Tcl_StaticPackage(interp, "Tcltest", Tcltest_Init,
        (Tcl_PackageInitProc *) NULL);
    if (TclObjTest_Init(interp) == TCL_ERROR) {
        return TCL_ERROR;
    }
#ifdef TCL_THREADS
    if (TclThread_Init(interp) == TCL_ERROR) {
        return TCL_ERROR;
    }
#endif
    if (Procbodytest_Init(interp) == TCL_ERROR) {
        return TCL_ERROR;
    }
    Tcl_StaticPackage(interp, "procbodytest", Procbodytest_Init,
        Procbodytest_SafeInit);
#endif /* TCL_TEST */

    /*
    * Call the init procedures for included packages.  Each call should
    * look like this:
    *
    * if (Mod_Init(interp) == TCL_ERROR) {
    *     return TCL_ERROR;
    * }
    *
    * where "Mod" is the name of the module.
    */

    /*
    * Call Tcl_CreateCommand for application-specific commands, if
    * they weren't already created by the init procedures called above.
    */

    // OpenFresco commands
    theDomain = new Domain();

    Tcl_CreateCommand(interp, "model", specifyModelBuilder,
        (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);    

    Tcl_CreateCommand(interp, "expControlPoint", openFresco_addExperimentalCP,
        (ClientData)NULL, NULL);

    Tcl_CreateCommand(interp, "expSignalFilter", openFresco_addExperimentalSignalFilter,
        (ClientData)NULL, NULL);

    Tcl_CreateCommand(interp, "expControl", openFresco_addExperimentalControl,
        (ClientData)NULL, NULL);

    Tcl_CreateCommand(interp, "expSetup", openFresco_addExperimentalSetup,
        (ClientData)NULL, NULL);

    Tcl_CreateCommand(interp, "expSite", openFresco_addExperimentalSite,
        (ClientData)NULL, NULL);

    Tcl_CreateCommand(interp, "expElement", openFresco_addExperimentalElement,
        (ClientData)NULL, NULL);

    Tcl_CreateCommand(interp, "startLabServer", openFresco_startLabServer,
        (ClientData)NULL, NULL);

    Tcl_CreateCommand(interp, "startSimAppSiteServer", openFresco_startSimAppSiteServer,
        (ClientData)NULL, NULL);

    Tcl_CreateCommand(interp, "startSimAppElemServer", openFresco_startSimAppElemServer,
        (ClientData)NULL, NULL);
    
    /*
    * Specify a user-specific startup file to invoke if the application
    * is run interactively.  Typically the startup file is "~/.apprc"
    * where "app" is the name of the application.  If this line is deleted
    * then no user-specific startup file will be run under any conditions.
    */
    Tcl_SetVar(interp, "tcl_rcFileName", "~/.tclshrc", TCL_GLOBAL_ONLY);
    return TCL_OK;
}


int OpenSeesExit(ClientData clientData, Tcl_Interp *interp, int argc, TCL_Char **argv)
{
    if (simulationInfoOutputFilename != 0) {
        simulationInfo.end();
        FileStream simulationInfoOutputFile;
        simulationInfoOutputFile.setFile(simulationInfoOutputFilename);
        simulationInfoOutputFile.open();
        simulationInfoOutputFile << simulationInfo;
        simulationInfoOutputFile.close();
    }

    Tcl_Exit(0);
    return 0;
}
