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
// $URL: $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 09/06
// Revision: A
//
// Description: This file contains the implementation of the ECxPCtarget class.

#include "ECxPCtarget.h"

#include <windows.h>
#include <xpcapi.h>
#include <xpcapiconst.h>


ECxPCtarget::ECxPCtarget(int tag, int pctype, char *ipaddress,
    char *ipport, char *appname, char *apppath, int timeout)
    : ExperimentalControl(tag),
    pcType(pctype), ipAddress(ipaddress), ipPort(ipport),
    appName(appname), appPath(apppath), timeOut(timeout),
    ctrlDisp(0), ctrlVel(0), ctrlAccel(0), daqDisp(0), daqForce(0),
    daqDispId(0), daqForceId(0)
{    
    // initialize the xPC Target dynamic link library
    if (xPCInitAPI())  {
        opserr << "ECxPCtarget::ECxPCtarget()"
            << " - xPCInitAPI: unable to load xPC Target API.\n";
        exit(OF_ReturnType_failed);
    }
    
    // the host application OpenSees needs to access the xPC Target;
    // a TCP/IP channel is therefore opened to the xPC Target
    port = xPCOpenTcpIpPort(ipAddress, ipPort);
    if (xPCGetLastError())  {
        xPCErrorMsg(xPCGetLastError(), errMsg);
        opserr << "ECxPCtarget::ECxPCtarget()"
            << " - xPCOpenTcpIpPort: error = " << errMsg << endln;
        xPCFreeAPI();
        exit(OF_ReturnType_failed);
    }

    // set timeout value between host and target PC
    xPCSetLoadTimeOut(port, timeOut);
    if (xPCGetLastError())  {
        xPCErrorMsg(xPCGetLastError(), errMsg);
        opserr << "ECxPCtarget::ECxPCtarget()"
            << " - xPCSetLoadTimeOut: error = " << errMsg << endln;
        xPCFreeAPI();
        exit(OF_ReturnType_failed);
    }
    
    opserr << "****************************************************************\n";
    opserr << "* The TCP/IP channel with address: " << ipAddress << endln;
    opserr << "* and port: " << ipPort << " has been opened\n";
    opserr << "****************************************************************\n";
    opserr << endln;

    // check if target application is already loaded
    // otherwise load the desired application
    char *currentAppName = new char [256];
    xPCGetAppName(port, currentAppName);
    if (xPCGetLastError())  {
        xPCErrorMsg(xPCGetLastError(), errMsg);
        opserr << "ECxPCtarget::ECxPCtarget() - "
            << "xPCGetAppName: error = " << errMsg << endln;
        xPCClosePort(port);
        xPCFreeAPI();
        exit(OF_ReturnType_failed);
    }
    if (strcmp(currentAppName,appName) != 0)  {
        // unload the current application
        xPCUnloadApp(port);
        if (xPCGetLastError())  {
            xPCErrorMsg(xPCGetLastError(), errMsg);
            opserr << "ECxPCtarget::ECxPCtarget() - "
                << "xPCUnloadApp: error = " << errMsg << endln;
            xPCClosePort(port);
            xPCFreeAPI();
            exit(OF_ReturnType_failed);
        }
        // load the new target application
        xPCLoadApp(port, appPath, appName);
        if (xPCGetLastError())  {
            xPCErrorMsg(xPCGetLastError(), errMsg);
            opserr << "ECxPCtarget::ECxPCtarget() - "
                << "xPCLoadApp: error = " << errMsg << endln;
            xPCClosePort(port);
            xPCFreeAPI();
            exit(OF_ReturnType_failed);
        }
    }
    delete [] currentAppName;
    
    // stop the target application on the xPC Target
    xPCStartApp(port);
    if (xPCGetLastError())  {
        xPCErrorMsg(xPCGetLastError(), errMsg);
        opserr << "ECxPCtarget::ECxPCtarget() - "
            << "xPCStartApp: error = " << errMsg << endln;
        xPCClosePort(port);
        xPCFreeAPI();
        exit(OF_ReturnType_failed);
    }
    this->sleep(1000);
    xPCStopApp(port);
    if (xPCGetLastError())  {
        xPCErrorMsg(xPCGetLastError(), errMsg);
        opserr << "ECxPCtarget::ECxPCtarget() - "
            << "xPCStopApp: error = " << errMsg << endln;
        xPCClosePort(port);
        xPCFreeAPI();
        exit(OF_ReturnType_failed);
    }
    
    opserr << "****************************************************************\n";
    opserr << "* The application '" << appName << "' has been loaded and is stopped\n";
    opserr << "* sample time = " << xPCGetSampleTime(port) << ", stop time = ";
    opserr << xPCGetStopTime(port) << ", timeout time = " << xPCGetLoadTimeOut(port) << endln;
    opserr << "****************************************************************\n";
    opserr << endln;
}


ECxPCtarget::ECxPCtarget(const ECxPCtarget &ec)
    : ExperimentalControl(ec),
    ctrlDisp(0), ctrlVel(0), ctrlAccel(0), daqDisp(0), daqForce(0),
    daqDispId(0), daqForceId(0)
{
    pcType = ec.pcType;
    port = ec.port;
    ipAddress = ec.ipAddress;
    ipPort = ec.ipPort;
    appName = ec.appName;
    appPath = ec.appPath;
    timeOut = ec.timeOut;
}


ECxPCtarget::~ECxPCtarget()
{
    // delete memory of ctrl vectors
    if (ctrlDisp != 0)
        delete ctrlDisp;
    if (ctrlVel != 0)
        delete ctrlVel;
    if (ctrlAccel != 0)
        delete ctrlAccel;
    
    // delete memory of daq vectors
    if (daqDisp != 0)
        delete daqDisp;
    if (daqForce != 0)
        delete daqForce;
    if (daqDispId != 0)
        delete daqDispId;
    if (daqForceId != 0)
        delete daqForceId;
    
    // delete memory of strings
    if (ipAddress != 0)
        delete [] ipAddress;
    if (ipPort != 0)
        delete [] ipPort;
    if (appName != 0)
        delete [] appName;
    if (appPath != 0)
        delete [] appPath;
    
    // stop the target application on the xPC Target
    xPCStopApp(port);
    
    // each application has to close the channel and unregister itself before exiting
    xPCClosePort(port);
    xPCFreeAPI();
    
    opserr << endln;
    opserr << "****************************************\n";
    opserr << "* The rtp application has been stopped *\n";
    opserr << "****************************************\n";
    opserr << endln;
}


int ECxPCtarget::setup()
{
    int rValue = 0;
    
    if (ctrlDisp != 0)
        delete ctrlDisp;
    if (ctrlVel != 0)
        delete ctrlVel;
    if (ctrlAccel != 0)
        delete ctrlAccel;
    
    if ((*sizeCtrl)(OF_Resp_Disp) != 0)  {
        ctrlDisp = new double [(*sizeCtrl)(OF_Resp_Disp)];
        for (int i=0; i<(*sizeCtrl)(OF_Resp_Disp); i++)
            ctrlDisp[i] = 0.0;
    }
    if ((*sizeCtrl)(OF_Resp_Vel) != 0)  {
        ctrlVel = new double [(*sizeCtrl)(OF_Resp_Vel)];
        for (int i=0; i<(*sizeCtrl)(OF_Resp_Vel); i++)
            ctrlVel[i] = 0.0;
    }
    if ((*sizeCtrl)(OF_Resp_Accel) != 0)  {
        ctrlAccel = new double [(*sizeCtrl)(OF_Resp_Accel)];
        for (int i=0; i<(*sizeCtrl)(OF_Resp_Accel); i++)
            ctrlAccel[i] = 0.0;
    }
    
    if (daqDisp != 0)
        delete daqDisp;
    if (daqForce != 0)
        delete daqForce;
    
    if ((*sizeDaq)(OF_Resp_Disp) != 0)  {
        daqDisp = new double [(*sizeDaq)(OF_Resp_Disp)];
        daqDispId = new int [(*sizeDaq)(OF_Resp_Disp)];
        for (int i=0; i<(*sizeDaq)(OF_Resp_Disp); i++)  {
            daqDisp[i] = 0.0;
            daqDispId[i] = 0;
        }
    }
    if ((*sizeDaq)(OF_Resp_Force) != 0)  {
        daqForce = new double [(*sizeDaq)(OF_Resp_Force)];
        daqForceId = new int [(*sizeDaq)(OF_Resp_Force)];
        for (int i=0; i<(*sizeDaq)(OF_Resp_Force); i++)  {
            daqForce[i] = 0.0;
            daqForceId[i] = 0;
        }
    }
    
    // get addresses of the controlled variables on the xPC Target
    newTargetId = xPCGetParamIdx(port, "xPC HC/newTarget", "Value");
    if (xPCGetLastError())  {
        xPCErrorMsg(xPCGetLastError(), errMsg);
        opserr << "ECxPCtarget::setup() - "
            << "xPCGetParamIdx - newTarget: error = " << errMsg << endln;
        xPCClosePort(port);
        xPCFreeAPI();
        exit(OF_ReturnType_failed);
    }
    switchPCId = xPCGetSignalIdx(port, "xPC HC/switchPC");
    if (xPCGetLastError())  {
        xPCErrorMsg(xPCGetLastError(), errMsg);
        opserr << "ECxPCtarget::setup() - "
            << "xPCGetSignalIdx - switchPC: error = " << errMsg << endln;
        xPCClosePort(port);
        xPCFreeAPI();
        exit(OF_ReturnType_failed);
    }
    atTargetId = xPCGetSignalIdx(port, "xPC HC/atTarget");
    if (xPCGetLastError())  {
        xPCErrorMsg(xPCGetLastError(), errMsg);
        opserr << "ECxPCtarget::setup() - "
            << "xPCGetSignalIdx - atTarget: error = " << errMsg << endln;
        xPCClosePort(port);
        xPCFreeAPI();
        exit(OF_ReturnType_failed);
    }
    ctrlDispId = xPCGetParamIdx(port, "xPC HC/targDsp", "Value");
    if (xPCGetLastError())  {
        xPCErrorMsg(xPCGetLastError(), errMsg);
        opserr << "ECxPCtarget::setup() - "
            << "xPCGetParamIdx - targDsp: error = " << errMsg << endln;
        xPCClosePort(port);
        xPCFreeAPI();
        exit(OF_ReturnType_failed);
    }
    if (pcType==2 || pcType==3)  {
        ctrlVelId = xPCGetParamIdx(port, "xPC HC/targVel", "Value");
        if (xPCGetLastError())  {
            xPCErrorMsg(xPCGetLastError(), errMsg);
            opserr << "ECxPCtarget::setup() - "
                << "xPCGetParamIdx - targVel: error = " << errMsg << endln;
            xPCClosePort(port);
            xPCFreeAPI();
            exit(OF_ReturnType_failed);
        }
    }
    if (pcType==3)  {
        ctrlAccelId = xPCGetParamIdx(port, "xPC HC/targAcc", "Value");
        if (xPCGetLastError())  {
            xPCErrorMsg(xPCGetLastError(), errMsg);
            opserr << "ECxPCtarget::setup() - "
                << "xPCGetParamIdx - targAcc: error = " << errMsg << endln;
            xPCClosePort(port);
            xPCFreeAPI();
            exit(OF_ReturnType_failed);
        }
    }
    if ((*sizeDaq)(OF_Resp_Disp)==1)  {
        daqDispId[0] = xPCGetSignalIdx(port, "xPC HC/measDsp");
        if (xPCGetLastError())  {
            xPCErrorMsg(xPCGetLastError(), errMsg);
            opserr << "ECxPCtarget::setup() - "
                << "xPCGetSignalIdx - measDsp: error = " << errMsg << endln;
            xPCClosePort(port);
            xPCFreeAPI();
            exit(OF_ReturnType_failed);
        }
    } else  {
        char dspStr[20], sigStr[3];
        for (int i=0; i<(*sizeDaq)(OF_Resp_Disp); i++)  {
            strcpy(dspStr,"xPC HC/measDsp/s");
            sprintf(sigStr, "%d", i+1);
            strcat(dspStr, sigStr);
            daqDispId[i] = xPCGetSignalIdx(port, dspStr);
            if (xPCGetLastError())  {
                xPCErrorMsg(xPCGetLastError(), errMsg);
                opserr << "ECxPCtarget::setup() - "
                    << "xPCGetSignalIdx - measDsp: error = " << errMsg << endln;
                xPCClosePort(port);
                xPCFreeAPI();
                exit(OF_ReturnType_failed);
            }
        }
    }
    if ((*sizeDaq)(OF_Resp_Force)==1)  {
        daqForceId[0] = xPCGetSignalIdx(port, "xPC HC/measFrc");
        if (xPCGetLastError())  {
            xPCErrorMsg(xPCGetLastError(), errMsg);
            opserr << "ECxPCtarget::setup() - xPCGetSignalIdx - measFrc: error = " << errMsg << endln;
            xPCClosePort(port);
            xPCFreeAPI();
            exit(OF_ReturnType_failed);
        }
    } else  {
        char frcStr[20], sigStr[3];
        for (int i=0; i<(*sizeDaq)(OF_Resp_Force); i++)  {
            strcpy(frcStr,"xPC HC/measFrc/s");
            sprintf(sigStr, "%d", i+1);
            strcat(frcStr, sigStr);
            daqForceId[i] = xPCGetSignalIdx(port, frcStr);
            if (xPCGetLastError())  {
                xPCErrorMsg(xPCGetLastError(), errMsg);
                opserr << "ECxPCtarget::setup() - "
                    << "xPCGetSignalIdx - measFrc: error = " << errMsg << endln;
                xPCClosePort(port);
                xPCFreeAPI();
                exit(OF_ReturnType_failed);
            }
        }
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
        xPCClosePort(port);
        xPCFreeAPI();
        exit(OF_ReturnType_failed);
    }
    
    // start the target application on the xPC Target
    xPCStartApp(port);
    if (xPCGetLastError())  {
        xPCErrorMsg(xPCGetLastError(), errMsg);
        opserr << "ECxPCtarget::setup() - "
            << "xPCStartApp: error = " << errMsg << endln;
        xPCClosePort(port);
        xPCFreeAPI();
        exit(OF_ReturnType_failed);
    }
    this->sleep(1000);
		
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
            xPCStopApp(port);
            xPCClosePort(port);
            xPCFreeAPI();
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


int ECxPCtarget::setSize(ID sizeT, ID sizeO)
{
    // check sizeTrial and sizeOut
    // for ECxPCtarget object
    
    // ECxPCtarget objects only use 
    // disp or disp and vel or disp, vel and accel for trial and
    // disp and force for output
    // check these are available in sizeT/sizeO.
    if ((pcType == 0 && sizeT(OF_Resp_Disp) == 0) || 
        (pcType == 1 && sizeT(OF_Resp_Disp) == 0 && sizeT(OF_Resp_Vel) == 0) ||
        (pcType == 2 && sizeT(OF_Resp_Disp) == 0 && sizeT(OF_Resp_Vel) == 0 && sizeT(OF_Resp_Accel) == 0) ||
        (sizeO(OF_Resp_Disp) == 0 && sizeO(OF_Resp_Force) == 0))  {
        opserr << "ECxPCtarget::setSize() - wrong sizeTrial/Out\n"; 
        opserr << "see User Manual.\n";
        xPCClosePort(port);
        xPCFreeAPI();
        exit(OF_ReturnType_failed);
    }
    
    *sizeCtrl = sizeT;
    *sizeDaq  = sizeO;

    return OF_ReturnType_completed;
}


int ECxPCtarget::setTrialResponse(const Vector* disp,
    const Vector* vel,
    const Vector* accel,
    const Vector* force,
    const Vector* time)
{
    int i, rValue = 0;
    if (disp != 0)  {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Disp); i++)  {
            ctrlDisp[i] = (*disp)(i);
            if (theCtrlFilters[OF_Resp_Disp] != 0)
                ctrlDisp[i] = theCtrlFilters[OF_Resp_Disp]->filtering(ctrlDisp[i]);
        }
    }
    if (vel != 0)  {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Vel); i++)  {
            ctrlVel[i] = (*vel)(i);
            if (theCtrlFilters[OF_Resp_Vel] != 0)
                ctrlVel[i] = theCtrlFilters[OF_Resp_Vel]->filtering(ctrlVel[i]);
        }
    }
    if (accel != 0)  {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Accel); i++)  {
            ctrlAccel[i] = (*accel)(i);
            if (theCtrlFilters[OF_Resp_Accel] != 0)
                ctrlAccel[i] = theCtrlFilters[OF_Resp_Accel]->filtering(ctrlAccel[i]);
        }
    }
    
    rValue = this->control();
    
    return rValue;
}


int ECxPCtarget::getDaqResponse(Vector* disp,
    Vector* vel,
    Vector* accel,
    Vector* force,
    Vector* time)
{
    this->acquire();
    
    int i;
    if (disp != 0)  {
        for (i=0; i<(*sizeDaq)(OF_Resp_Disp); i++)  {
            if (theDaqFilters[OF_Resp_Disp] != 0)
                daqDisp[i] = theDaqFilters[OF_Resp_Disp]->filtering(daqDisp[i]);
            (*disp)(i) = daqDisp[i];
        }
    }
    if (force != 0)  {
        for (i=0; i<(*sizeDaq)(OF_Resp_Force); i++)  {
            if (theDaqFilters[OF_Resp_Force] != 0)
                daqForce[i] = theDaqFilters[OF_Resp_Force]->filtering(daqForce[i]);
            (*force)(i) = daqForce[i];
        }
    }
        
    return OF_ReturnType_completed;
}


int ECxPCtarget::commitState()
{	
    return OF_ReturnType_completed;
}


ExperimentalControl *ECxPCtarget::getCopy()
{
    return new ECxPCtarget(*this);
}


Response* ECxPCtarget::setResponse(const char **argv, int argc,
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
        theResponse = new ExpControlResponse(this, 1,
            Vector((*sizeCtrl)(OF_Resp_Disp)));
    }
    
    // ctrl velocities
    if (ctrlVel != 0 && (
        strcmp(argv[0],"ctrlVel") == 0 ||
        strcmp(argv[0],"ctrlVelocity") == 0 ||
        strcmp(argv[0],"ctrlVelocities") == 0))
    {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Vel); i++)  {
            sprintf(outputData,"ctrlVel%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 2,
            Vector((*sizeCtrl)(OF_Resp_Vel)));
    }
    
    // ctrl accelerations
    if (ctrlAccel != 0 && (
        strcmp(argv[0],"ctrlAccel") == 0 ||
        strcmp(argv[0],"ctrlAcceleration") == 0 ||
        strcmp(argv[0],"ctrlAccelerations") == 0))
    {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Accel); i++)  {
            sprintf(outputData,"ctrlAccel%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 3,
            Vector((*sizeCtrl)(OF_Resp_Accel)));
    }
    
    // daq displacements
    if (daqDisp != 0 && (
        strcmp(argv[0],"daqDisp") == 0 ||
        strcmp(argv[0],"daqDisplacement") == 0 ||
        strcmp(argv[0],"daqDisplacements") == 0))
    {
        for (i=0; i<(*sizeDaq)(OF_Resp_Disp); i++)  {
            sprintf(outputData,"daqDisp%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 4,
            Vector((*sizeDaq)(OF_Resp_Disp)));
    }
    
    // daq forces
    if (daqForce != 0 && (
        strcmp(argv[0],"daqForce") == 0 ||
        strcmp(argv[0],"daqForces") == 0))
    {
        for (i=0; i<(*sizeDaq)(OF_Resp_Force); i++)  {
            sprintf(outputData,"daqForce%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 5,
            Vector((*sizeDaq)(OF_Resp_Force)));
    }
    
    output.endTag();
    
    return theResponse;
}


int ECxPCtarget::getResponse(int responseID, Information &info)
{
    Vector resp(0);

    switch (responseID)  {
    case 1:  // ctrl displacements
        resp.setData(ctrlDisp,(*sizeCtrl)(OF_Resp_Disp));
        return info.setVector(resp);
        
    case 2:  // ctrl velocities
        resp.setData(ctrlVel,(*sizeCtrl)(OF_Resp_Vel));
        return info.setVector(resp);
        
    case 3:  // ctrl accelerations
        resp.setData(ctrlAccel,(*sizeCtrl)(OF_Resp_Accel));
        return info.setVector(resp);
        
    case 4:  // daq displacements
        resp.setData(daqDisp,(*sizeDaq)(OF_Resp_Disp));
        return info.setVector(resp);
        
    case 5:  // daq forces
        resp.setData(daqForce,(*sizeDaq)(OF_Resp_Force));
        return info.setVector(resp);
        
    default:
        return -1;
    }
}


void ECxPCtarget::Print(OPS_Stream &s, int flag)
{
    s << "****************************************************************\n";
    s << "* ExperimentalControl: " << this->getTag() << endln; 
    s << "*   type: ECxPCtarget\n";
    s << "*   ipAddress: " << ipAddress << ", ipPort: " << ipPort << endln;
    s << "*   appName: " << appName << endln;
    s << "*   appPath: " << appPath << endln;
    s << "*   pcType: " << pcType << endln;
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


int ECxPCtarget::control()
{
    // send ctrlDisp, ctrlVel and ctrlAccel and set newTarget flag
    xPCSetParam(port, ctrlDispId, ctrlDisp);
    if (xPCGetLastError())  {
        xPCErrorMsg(xPCGetLastError(), errMsg);
        opserr << "ECxPCtarget::control() - "
            << "xPCSetParam(ctrlDisp): error = " << errMsg << endln;
        xPCClosePort(port);
        xPCFreeAPI();
        exit(OF_ReturnType_failed);
    }
    if (pcType==2 || pcType==3)  {
        xPCSetParam(port, ctrlVelId, ctrlVel);
        if (xPCGetLastError())  {
            xPCErrorMsg(xPCGetLastError(), errMsg);
            opserr << "ECxPCtarget::control() - "
                << "xPCSetParam(ctrlVel): error = " << errMsg << endln;
            xPCClosePort(port);
            xPCFreeAPI();
            exit(OF_ReturnType_failed);
        }
    }
    if (pcType==3)  {
        xPCSetParam(port, ctrlAccelId, ctrlAccel);
        if (xPCGetLastError())  {
            xPCErrorMsg(xPCGetLastError(), errMsg);
            opserr << "ECxPCtarget::control() - "
                << "xPCSetParam(ctrlAccel): error = " << errMsg << endln;
            xPCClosePort(port);
            xPCFreeAPI();
            exit(OF_ReturnType_failed);
        }
    }
    
	// set newTarget flag
	newTarget = 1;
	xPCSetParam(port, newTargetId, &newTarget);
    if (xPCGetLastError())  {
        xPCErrorMsg(xPCGetLastError(), errMsg);
        opserr << "ECxPCtarget::control() - "
            << "xPCSetParam(newTarget): error = " << errMsg << endln;
        xPCClosePort(port);
        xPCFreeAPI();
        exit(OF_ReturnType_failed);
    }
    
	// wait until switchPC flag has changed as well
    switchPC = 0;
    while (switchPC != 1)  {
        switchPC = xPCGetSignal(port, switchPCId);
        if (xPCGetLastError())  {
            xPCErrorMsg(xPCGetLastError(), errMsg);
            opserr << "ECxPCtarget::control() - "
                << "xPCGetSignal(switchPC): error = " << errMsg << endln;
            xPCClosePort(port);
            xPCFreeAPI();
            exit(OF_ReturnType_failed);
        }
    }
    
    // reset newTarget flag
    newTarget = 0;
    xPCSetParam(port, newTargetId, &newTarget);
    if (xPCGetLastError())  {
        xPCErrorMsg(xPCGetLastError(), errMsg);
        opserr << "ECxPCtarget::control() - "
            << "xPCSetParam(newTarget): error = " << errMsg << endln;
        xPCClosePort(port);
        xPCFreeAPI();
        exit(OF_ReturnType_failed);
    }
    
	// wait until switchPC flag has changed as well
    switchPC = 1;
	while (switchPC != 0)  {
        switchPC = xPCGetSignal(port, switchPCId);
        if (xPCGetLastError())  {
            xPCErrorMsg(xPCGetLastError(), errMsg);
            opserr << "ECxPCtarget::control() - "
                << "xPCGetSignal(switchPC): error = " << errMsg << endln;
            xPCClosePort(port);
            xPCFreeAPI();
            exit(OF_ReturnType_failed);
        }
    }
    
    return OF_ReturnType_completed;
}


int ECxPCtarget::acquire()
{
    // wait until target is reached
    atTarget = 0;
    while (atTarget != 1)  {
        atTarget = xPCGetSignal(port, atTargetId);
        if (xPCGetLastError())  {
            xPCErrorMsg(xPCGetLastError(), errMsg);
            opserr << "ECxPCtarget::acquire() - "
                << "xPCGetSignal(atTarget): error = " << errMsg << endln;
            xPCClosePort(port);
            xPCFreeAPI();
            exit(OF_ReturnType_failed);
        }
    }
    
    // read displacements and resisting forces at target
    if ((*sizeDaq)(OF_Resp_Disp) != 0)  {
        xPCGetSignals(port, (*sizeDaq)(OF_Resp_Disp), daqDispId, daqDisp);
        if (xPCGetLastError())  {
            xPCErrorMsg(xPCGetLastError(), errMsg);
            opserr << "ECxPCtarget::acquire() - "
                << "xPCGetSignals(daqDisp): error = " << errMsg << endln;
            xPCClosePort(port);
            xPCFreeAPI();
            exit(OF_ReturnType_failed);
        }
    }
    if ((*sizeDaq)(OF_Resp_Force) != 0)  {
        xPCGetSignals(port, (*sizeDaq)(OF_Resp_Force), daqForceId, daqForce);
        if (xPCGetLastError())  {
            xPCErrorMsg(xPCGetLastError(), errMsg);
            opserr << "ECxPCtarget::acquire() - "
                << "xPCGetSignals(daqForce): error = " << errMsg << endln;
            xPCClosePort(port);
            xPCFreeAPI();
            exit(OF_ReturnType_failed);
        }
    }
    
    return OF_ReturnType_completed;
}


void ECxPCtarget::sleep(const clock_t wait)
{
    clock_t goal;
    goal = wait + clock();
    while (goal>clock());
}
