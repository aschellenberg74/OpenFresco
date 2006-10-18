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
// $Source$

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 09/06
// Revision: A
//
// Description: This file contains the implementation of the ECxPCtarget class.

#include "ECxPCtarget.h"

#include <xpcapi.h>
#include <xpcapiconst.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


ECxPCtarget::ECxPCtarget(int tag, int pctype, char *ipaddress,
    char *ipport, char *appname, char *apppath)
    : ExperimentalControl(tag),
    pcType(pctype), ipAddress(ipaddress), ipPort(ipport), appName(appname), appPath(apppath),
    targDisp(0), targVel(0), targAccel(0), measDisp(0), measForce(0),
    measDispId(0), measForceId(0)
{
    // open output file
    //outFile = fopen("ECxPCtarget.out","w");
    //if (outFile==NULL)  {
    //	opserr << "ECxPCtarget::ECxPCtarget()"
    //		<< " - fopen: could not open output file" << endln;
    //  exit(OF_ReturnType_failed);
    //}
    
    // initialize the xPC Target dynamic link library
    if (xPCInitAPI())  {
        opserr << "ECxPCtarget::ECxPCtarget()"
            << " - xPCInitAPI: unable to load xPC Target API" << endln;
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
    
    opserr << "*************************************************\n";
    opserr << "* The TCP/IP channel with address: " << ipAddress << " *\n";
    opserr << "* and port: " << ipPort << " has been opened:              *\n";
    opserr << "*************************************************\n";
    opserr << endln;
    
    // load the compiled target application to the target
    // if an application is already on the target it is unloaded first
    xPCLoadApp(port, appPath, appName);
    if (xPCGetLastError())  {
        xPCErrorMsg(xPCGetLastError(), errMsg);
        opserr << "ECxPCtarget::ECxPCtarget() - xPCLoadApp: error = " << errMsg << endln;
        xPCClosePort(port);
        xPCFreeAPI();
        exit(OF_ReturnType_failed);
    }
    
    // stop the target application on the xPC Target
    xPCStartApp(port);
    if (xPCGetLastError())  {
        xPCErrorMsg(xPCGetLastError(), errMsg);
        opserr << "ECxPCtarget::ECxPCtarget() - xPCStartApp: error = " << errMsg << endln;
        xPCClosePort(port);
        xPCFreeAPI();
        exit(OF_ReturnType_failed);
    }
    this->sleep(1000);
    xPCStopApp(port);
    if (xPCGetLastError())  {
        xPCErrorMsg(xPCGetLastError(), errMsg);
        opserr << "ECxPCtarget::ECxPCtarget() - xPCStopApp: error = " << errMsg << endln;
        xPCClosePort(port);
        xPCFreeAPI();
        exit(OF_ReturnType_failed);
    }
    
    opserr << "************************************************************************\n";
    opserr << "* The application '" << appName << "' has been loaded and is stopped  \n";
    opserr << "* sample time = " << xPCGetSampleTime(port) << ", stop time = " << xPCGetStopTime(port) << "\n";
    opserr << "************************************************************************\n";
    opserr << endln;
    
}


ECxPCtarget::ECxPCtarget(const ECxPCtarget &ec)
    : ExperimentalControl(ec)
{
    pcType = ec.pcType;
    ipAddress = ec.ipAddress;
    ipPort = ec.ipPort;
    appName = ec.appName;
    appPath = ec.appPath;
}


ECxPCtarget::~ECxPCtarget()
{
    // close output file
    //fclose(outFile);
    
    // delete memory of target vectors
    if (targDisp != 0)
        delete targDisp;
    if (targVel != 0)
        delete targVel;
    if (targAccel != 0)
        delete targAccel;
    
    // delete memory of measured vectors
    if (measDisp != 0)
        delete measDisp;
    if (measForce != 0)
        delete measForce;
    if (measDispId != 0)
        delete measDispId;
    if (measForceId != 0)
        delete measForceId;
    
    // stop the target application on the xPC Target
    xPCStopApp(port);
    
    // each application has to close the channel and unregister itself before exiting
    xPCClosePort(port);
    xPCFreeAPI();
    
    opserr << "****************************************\n";
    opserr << "* The rtp application has been stopped *\n";
    opserr << "****************************************\n";
    opserr << endln;
}


int ECxPCtarget::setSize(ID sizeT, ID sizeO)
{
    // check sizeTrial and sizeOut
    // for ECxPCtarget object
    
    // ECxPCtarget object only use 
    // disp or disp and vel or disp, vel and accel for trial
    // disp and force for output
    // check these are available in sizeT/sizeO.
    
    if((pcType == 0 && sizeT[OF_Resp_Disp] == 0) || 
        (pcType == 1 && sizeT[OF_Resp_Disp] == 0 && sizeT[OF_Resp_Vel] == 0) ||
        (pcType == 2 && sizeT[OF_Resp_Disp] == 0 && sizeT[OF_Resp_Vel] == 0 && sizeT[OF_Resp_Accel] == 0) ||
        sizeO[OF_Resp_Disp] == 0 ||
        sizeO[OF_Resp_Force] == 0) {
        opserr << "ECxPCtarget::setSize - wrong sizeTrial/Out\n"; 
        opserr << "see User Manual.\n";
        exit(OF_ReturnType_failed);
    }
    
    *sizeCtrl = sizeT;
    *sizeDaq = sizeO;
    
    return OF_ReturnType_completed;
}


int ECxPCtarget::setup()
{
    if (targDisp != 0)
        delete targDisp;
    if (targVel != 0)
        delete targVel;
    if (targAccel != 0)
        delete targAccel;
    
    if ((*sizeCtrl)[OF_Resp_Disp] =! 0)  {
        targDisp = new double[(*sizeCtrl)[OF_Resp_Disp]];
        for (int i=0; i<(*sizeCtrl)[OF_Resp_Disp]; i++)
            targDisp[i] = 0.0;
    }
    if ((*sizeCtrl)[OF_Resp_Vel] =! 0  && pcType == 2)  {
        targVel = new double[(*sizeCtrl)[OF_Resp_Vel]];
        for (int i=0; i<(*sizeCtrl)[OF_Resp_Vel]; i++)
            targVel[i] = 0.0;
    }
    if ((*sizeCtrl)[OF_Resp_Accel] =! 0 && pcType == 3)  {
        targAccel = new double[(*sizeCtrl)[OF_Resp_Accel]];
        for (int i=0; i<(*sizeCtrl)[OF_Resp_Accel]; i++)
            targAccel[i] = 0.0;
    }
    
    if (measDisp != 0)
        delete measDisp;
    if (measForce != 0)
        delete measForce;
    
    if ((*sizeDaq)[OF_Resp_Disp] =! 0)  {
        measDisp = new double[(*sizeDaq)[OF_Resp_Disp]];
        measDispId = new int[(*sizeDaq)[OF_Resp_Disp]];
        for (int i=0; i<(*sizeDaq)[OF_Resp_Disp]; i++)
            measDisp[i] = 0.0;
    }
    if ((*sizeDaq)[OF_Resp_Force] =! 0)  {
        measForce = new double[(*sizeDaq)[OF_Resp_Force]];
        measForceId = new int[(*sizeDaq)[OF_Resp_Force]];
        for (int i=0; i<(*sizeDaq)[OF_Resp_Force]; i++)
            measForce[i] = 0.0;
    }
    
    // get addresses of the controlled variables on the xPC Target
    updateFlagId = xPCGetParamIdx(port, "xPC HC/updateFlag", "Value");
    if (xPCGetLastError())  {
        xPCErrorMsg(xPCGetLastError(), errMsg);
        opserr << "ECxPCtarget::ECxPCtarget() - xPCGetParamIdx - updateFlag: error = " << errMsg << endln;
        xPCClosePort(port);
        xPCFreeAPI();
        return OF_ReturnType_failed;
    }
    targetFlagId = xPCGetSignalIdx(port, "xPC HC/targetFlag");
    if (xPCGetLastError())  {
        xPCErrorMsg(xPCGetLastError(), errMsg);
        opserr << "ECxPCtarget::ECxPCtarget() - xPCGetSignalIdx - targetFlag: error = " << errMsg << endln;
        xPCClosePort(port);
        xPCFreeAPI();
        return OF_ReturnType_failed;
    }
    targDispId = xPCGetParamIdx(port, "xPC HC/targDsp", "Value");
    if (xPCGetLastError())  {
        xPCErrorMsg(xPCGetLastError(), errMsg);
        opserr << "ECxPCtarget::ECxPCtarget() - xPCGetParamIdx - targDsp: error = " << errMsg << endln;
        xPCClosePort(port);
        xPCFreeAPI();
        return OF_ReturnType_failed;
    }
    if (pcType==2 || pcType==3)  {
        targVelId = xPCGetParamIdx(port, "xPC HC/targVel", "Value");
        if (xPCGetLastError())  {
            xPCErrorMsg(xPCGetLastError(), errMsg);
            opserr << "ECxPCtarget::ECxPCtarget() - xPCGetParamIdx - targVel: error = " << errMsg << endln;
            xPCClosePort(port);
            xPCFreeAPI();
            return OF_ReturnType_failed;
        }
    }
    if (pcType==3)  {
        targAccelId = xPCGetParamIdx(port, "xPC HC/targAcc", "Value");
        if (xPCGetLastError())  {
            xPCErrorMsg(xPCGetLastError(), errMsg);
            opserr << "ECxPCtarget::ECxPCtarget() - xPCGetParamIdx - targAcc: error = " << errMsg << endln;
            xPCClosePort(port);
            xPCFreeAPI();
            return OF_ReturnType_failed;
        }
    }
    if ((*sizeDaq)[OF_Resp_Disp]==1)  {
        measDispId[0] = xPCGetSignalIdx(port, "xPC HC/measDsp");
        if (xPCGetLastError())  {
            xPCErrorMsg(xPCGetLastError(), errMsg);
            opserr << "ECxPCtarget::ECxPCtarget() - xPCGetSignalIdx - measDsp: error = " << errMsg << endln;
            xPCClosePort(port);
            xPCFreeAPI();
            return OF_ReturnType_failed;
        }
    } else  {
        char dspStr[20], sigStr[3];
        for (int i=0; i<(*sizeDaq)[OF_Resp_Disp]; i++)  {
            strcpy(dspStr,"xPC HC/measDsp/s");
            sprintf(sigStr, "%d", i+1);
            strcat(dspStr, sigStr);
            measDispId[i] = xPCGetSignalIdx(port, dspStr);
            if (xPCGetLastError())  {
                xPCErrorMsg(xPCGetLastError(), errMsg);
                opserr << "ECxPCtarget::ECxPCtarget() - xPCGetSignalIdx - measDsp: error = " << errMsg << endln;
                xPCClosePort(port);
                xPCFreeAPI();
                return OF_ReturnType_failed;
            }
        }
    }
    if ((*sizeDaq)[OF_Resp_Force]==1)  {
        measForceId[0] = xPCGetSignalIdx(port, "xPC HC/measFrc");
        if (xPCGetLastError())  {
            xPCErrorMsg(xPCGetLastError(), errMsg);
            opserr << "ECxPCtarget::ECxPCtarget() - xPCGetSignalIdx - measFrc: error = " << errMsg << endln;
            xPCClosePort(port);
            xPCFreeAPI();
            return OF_ReturnType_failed;
        }
    } else  {
        char frcStr[20], sigStr[3];
        for (int i=0; i<(*sizeDaq)[OF_Resp_Force]; i++)  {
            strcpy(frcStr,"xPC HC/measFrc/s");
            sprintf(sigStr, "%d", i+1);
            strcat(frcStr, sigStr);
            measForceId[i] = xPCGetSignalIdx(port, frcStr);
            if (xPCGetLastError())  {
                xPCErrorMsg(xPCGetLastError(), errMsg);
                opserr << "ECxPCtarget::ECxPCtarget() - xPCGetSignalIdx - measFrc: error = " << errMsg << endln;
                xPCClosePort(port);
                xPCFreeAPI();
                return OF_ReturnType_failed;
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
    char c = getchar();
    if (c == 'c')  {
        xPCClosePort(port);
        xPCFreeAPI();
        return OF_ReturnType_failed;
    }
    
    do  {
        // start the target application on the xPC Target
        xPCStartApp(port);
        if (xPCGetLastError())  {
            xPCErrorMsg(xPCGetLastError(), errMsg);
            opserr << "ECxPCtarget::ECxPCtarget() - xPCStartApp: error = " << errMsg << endln;
            xPCClosePort(port);
            xPCFreeAPI();
            return OF_ReturnType_failed;
        }
        
        // reset the updateFlag and targDisp, targVel and targAccel
        updateFlag = 0;
        xPCSetParam(port, updateFlagId, &updateFlag);
        if (xPCGetLastError())  {
            xPCErrorMsg(xPCGetLastError(), errMsg);
            opserr << "ECxPCtarget::ECxPCtarget() - xPCSetParam: error = " << errMsg << endln;
            xPCClosePort(port);
            xPCFreeAPI();
            return OF_ReturnType_failed;
        }
        xPCSetParam(port, targDispId, targDisp);
        if (xPCGetLastError())  {
            xPCErrorMsg(xPCGetLastError(), errMsg);
            opserr << "ECxPCtarget::ECxPCtarget() - xPCSetParam: error = " << errMsg << endln;
            xPCClosePort(port);
            xPCFreeAPI();
            return OF_ReturnType_failed;
        }
        if (pcType==2 || pcType==3)  {
            xPCSetParam(port, targVelId, targVel);
            if (xPCGetLastError())  {
                xPCErrorMsg(xPCGetLastError(), errMsg);
                opserr << "ECxPCtarget::ECxPCtarget() - xPCSetParam: error = " << errMsg << endln;
                xPCClosePort(port);
                xPCFreeAPI();
                return OF_ReturnType_failed;
            }
        }
        if (pcType==3)  {
            xPCSetParam(port, targAccelId, targAccel);
            if (xPCGetLastError())  {
                xPCErrorMsg(xPCGetLastError(), errMsg);
                opserr << "ECxPCtarget::ECxPCtarget() - xPCSetParam: error = " << errMsg << endln;
                xPCClosePort(port);
                xPCFreeAPI();
                return OF_ReturnType_failed;
            }
        }
        updateFlag = 1;
        xPCSetParam(port, updateFlagId, &updateFlag);
        if (xPCGetLastError())  {
            xPCErrorMsg(xPCGetLastError(), errMsg);
            opserr << "ECxPCtarget::ECxPCtarget() - xPCSetParam: error = " << errMsg << endln;
            xPCClosePort(port);
            xPCFreeAPI();
            return OF_ReturnType_failed;
        }
        
        // wait until target is reached
        targetFlag = 1;
        while (targetFlag)  {
            // pause for 1msec
            this->sleep(1);
            
            // get target flag		
            targetFlag = xPCGetSignal(port, targetFlagId);
            if (xPCGetLastError())  {
                xPCErrorMsg(xPCGetLastError(), errMsg);
                opserr << "ECxPCtarget::ECxPCtarget() - xPCGetSignal: error = " << errMsg << endln;
                xPCClosePort(port);
                xPCFreeAPI();
                return OF_ReturnType_failed;
            }
        }
        
        // read displacements and resisting forces at target
        xPCGetSignals(port, (*sizeDaq)[OF_Resp_Disp], measDispId, measDisp);
        if (xPCGetLastError())  {
            xPCErrorMsg(xPCGetLastError(), errMsg);
            opserr << "ECxPCtarget::ECxPCtarget() - xPCGetSignal: error = " << errMsg << endln;
            xPCClosePort(port);
            xPCFreeAPI();
            return OF_ReturnType_failed;
        }
        xPCGetSignals(port, (*sizeDaq)[OF_Resp_Force], measForceId, measForce);
        if (xPCGetLastError())  {
            xPCErrorMsg(xPCGetLastError(), errMsg);
            opserr << "ECxPCtarget::ECxPCtarget() - xPCGetSignal: error = " << errMsg << endln;
            xPCClosePort(port);
            xPCFreeAPI();
            return OF_ReturnType_failed;
        }
        
        // reset updateFlag
        updateFlag = 0;
        xPCSetParam(port, updateFlagId, &updateFlag);
        if (xPCGetLastError())  {
            xPCErrorMsg(xPCGetLastError(), errMsg);
            opserr << "ECxPCtarget::ECxPCtarget() - xPCSetParam: error = " << errMsg << endln;
            xPCClosePort(port);
            xPCFreeAPI();
            return OF_ReturnType_failed;
        }
        // pause for 1msec
        this->sleep(1);
        
        // stop the target application on the xPC Target
        xPCStopApp(port);
        if (xPCGetLastError())  {
            xPCErrorMsg(xPCGetLastError(), errMsg);
            opserr << "ECxPCtarget::ECxPCtarget() - xPCStopApp: error = " << errMsg << endln;
            xPCClosePort(port);
            xPCFreeAPI();
            return OF_ReturnType_failed;
        }
        
        int i;
        opserr << "**************************************\n";
        opserr << "* Initial values of DAQ are:         *\n";
        opserr << "*                                    *\n";
        opserr << "* dspDaq = [";
        for (i=0; i<(*sizeDaq)[OF_Resp_Disp]; i++)
            opserr << " " << measDisp[i];
        opserr << " ]\n";
        opserr << "* frcDaq = [";
        for (i=0; i<(*sizeDaq)[OF_Resp_Force]; i++)
            opserr << " " << measForce[i];
        opserr << " ]\n";
        opserr << "*                                    *\n";
        opserr << "* Press 'Enter' to start the test or *\n";
        opserr << "* 'r' to repeat the measurement or   *\n";
        opserr << "* 'c' to cancel the initialization   *\n";
        opserr << "**************************************\n";
        opserr << endln;
        c = getchar();
        if (c == 'c')  {
            xPCClosePort(port);
            xPCFreeAPI();
            return OF_ReturnType_failed;
        } else if (c == 'r')  {
            getchar();
        }
        } while (c == 'r');
        // start the target application on the xPC Target
        xPCStartApp(port);
        if (xPCGetLastError())  {
            xPCErrorMsg(xPCGetLastError(), errMsg);
            opserr << "ECxPCtarget::ECxPCtarget() - xPCStartApp: error = " << errMsg << endln;
            xPCClosePort(port);
            xPCFreeAPI();
            return OF_ReturnType_failed;
        }
        
        opserr << "******************\n";
        opserr << "* Running......  *\n";
        opserr << "******************\n";
        opserr << endln;
        
        return OF_ReturnType_completed;
}


int ECxPCtarget::setTrialResponse(const Vector* disp,
    const Vector* vel,
    const Vector* accel,
    const Vector* force,
    const Vector* time)
{
    int i;
    for (i=0; i<(*sizeCtrl)[OF_Resp_Disp]; i++)  {
        targDisp[i] = (*disp)(i);
        if (theFilter != 0)  {
            targDisp[i] = theFilter->filtering(targDisp[i]);
        }
    }
    for (i=0; i<(*sizeCtrl)[OF_Resp_Vel]; i++)
        targVel[i] = (*vel)(i);
    for (i=0; i<(*sizeCtrl)[OF_Resp_Accel]; i++)
        targAccel[i] = (*accel)(i);
    
    //printf("d0 = %+8.4f, d1 = %+8.4f, d2 = %+8.4f\n",targDsp[0],targDsp[1],targDsp[2]);
    //printf("d = %+8.4f, v = %+10.4f, a = %+12.4f\n",targDsp[0],targVel[0],targAcc[0]);
    //fprintf(outFile,"%+1.16e  %+1.16e  %+1.16e\n",targDsp[0],targVel[0],targAcc[0]);
    
    this->control();
    
    return OF_ReturnType_completed;
}


int ECxPCtarget::getDaqResponse(Vector* disp,
                                Vector* vel,
                                Vector* accel,
                                Vector* force,
                                Vector* time)
{
    this->acquire();
    
    int i;
    for (i=0; i<(*sizeDaq)[OF_Resp_Disp]; i++)
        (*disp)(i) = measDisp[i];
    for (i=0; i<(*sizeDaq)[OF_Resp_Force]; i++)
        (*force)(i) = measForce[i];
    
    //printf("dspDaq%d = [ %+6.3f ]  frcDaq%d = [ %+6.3f ]\n",theTag,(*disp),theTag,(*force));
    
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


void ECxPCtarget::Print(OPS_Stream &s, int flag)
{
    s << "****************************************************************\n";
    s << "* ExperimentalControl: " << this->getTag() << endln; 
    s << "* type: ECxPCtarget\n";
    s << "*   ipAddress: " << ipAddress << ", ipPort: " << ipPort << endln;
    s << "*   appName: " << appName << endln;
    s << "*   appPath: " << appPath << endln;
    s << "*   pcType: " << pcType << endln;
    if (theFilter != 0) {
        s << "*\tFilter: " << *theFilter << endln;
    }
    s << "****************************************************************\n";
    s << endln;
}


int ECxPCtarget::control()
{
    // send targDisp, targVel and targAccel and set updateFlag
    xPCSetParam(port, targDispId, targDisp);
    if (xPCGetLastError())  {
        xPCErrorMsg(xPCGetLastError(), errMsg);
        opserr << "ECxPCtarget::execute() - xPCSetParam: error = " << errMsg << endln;
        xPCClosePort(port);
        xPCFreeAPI();
        return xPCGetLastError();
    }
    if (pcType==2 || pcType==3)  {
        xPCSetParam(port, targVelId, targVel);
        if (xPCGetLastError())  {
            xPCErrorMsg(xPCGetLastError(), errMsg);
            opserr << "ECxPCtarget::execute() - xPCSetParam: error = " << errMsg << endln;
            xPCClosePort(port);
            xPCFreeAPI();
            return xPCGetLastError();
        }
    }
    if (pcType==3)  {
        xPCSetParam(port, targAccelId, targAccel);
        if (xPCGetLastError())  {
            xPCErrorMsg(xPCGetLastError(), errMsg);
            opserr << "ECxPCtarget::execute() - xPCSetParam: error = " << errMsg << endln;
            xPCClosePort(port);
            xPCFreeAPI();
            return xPCGetLastError();
        }
    }
    updateFlag = 1;
    xPCSetParam(port, updateFlagId, &updateFlag);
    if (xPCGetLastError())  {
        xPCErrorMsg(xPCGetLastError(), errMsg);
        opserr << "ECxPCtarget::execute() - xPCSetParam: error = " << errMsg << endln;
        xPCClosePort(port);
        xPCFreeAPI();
        return xPCGetLastError();
    }
    
    return OF_ReturnType_completed;
}


int ECxPCtarget::acquire()
{
    targetFlag = 1;
    while (targetFlag)  {
        // pause for 1msec
        this->sleep(1);
        
        // get target flag		
        targetFlag = xPCGetSignal(port, targetFlagId);
        if (xPCGetLastError())  {
            xPCErrorMsg(xPCGetLastError(), errMsg);
            opserr << "ECxPCtarget::acquire() - xPCGetSignal: error = " << errMsg << endln;
            xPCClosePort(port);
            xPCFreeAPI();
            return xPCGetLastError();
        }
    }
    
    // read displacements and resisting forces at target
    xPCGetSignals(port, (*sizeDaq)[OF_Resp_Disp], measDispId, measDisp);
    if (xPCGetLastError())  {
        xPCErrorMsg(xPCGetLastError(), errMsg);
        opserr << "ECxPCtarget::acquire() - xPCGetSignal: error = " << errMsg << endln;
        xPCClosePort(port);
        xPCFreeAPI();
        return xPCGetLastError();
    }
    xPCGetSignals(port, (*sizeDaq)[OF_Resp_Force], measForceId, measForce);
    if (xPCGetLastError())  {
        xPCErrorMsg(xPCGetLastError(), errMsg);
        opserr << "ECxPCtarget::acquire() - xPCGetSignal: error = " << errMsg << endln;
        xPCClosePort(port);
        xPCFreeAPI();
        return xPCGetLastError();
    }
    
    // reset updateFlag
    updateFlag = 0;
    xPCSetParam(port, updateFlagId, &updateFlag);
    if (xPCGetLastError())  {
        xPCErrorMsg(xPCGetLastError(), errMsg);
        opserr << "ECxPCtarget::acquire() - xPCSetParam: error = " << errMsg << endln;
        xPCClosePort(port);
        xPCFreeAPI();
        return xPCGetLastError();
    }
    // pause for 1msec
    this->sleep(1);
    
    return OF_ReturnType_completed;
}


void ECxPCtarget::sleep(const clock_t wait)
{
    clock_t goal;
    goal = wait + clock();
    while (goal>clock());
}
