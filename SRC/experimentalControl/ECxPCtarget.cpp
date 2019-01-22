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
// Created: 09/06
// Revision: A
//
// Description: This file contains the implementation of the ECxPCtarget class.

#include "ECxPCtarget.h"
#include <ExperimentalCP.h>

#include <windows.h>
#include <xpcapi.h>
#include <xpcapiconst.h>


ECxPCtarget::ECxPCtarget(int tag, int nTrialCPs, ExperimentalCP **trialcps,
    int nOutCPs, ExperimentalCP **outcps, char *ipaddress, char *ipport,
    char *appFile, int _timeout, int reltrial)
    : ExperimentalControl(tag), numTrialCPs(nTrialCPs), numOutCPs(nOutCPs),
    ipAddress(ipaddress), ipPort(ipport), timeout(_timeout),
    numCtrlSignals(0), numDaqSignals(0), ctrlSignal(0), daqSignal(0),
    ctrlSigOffset(0), daqSigOffset(0), trialSigOffset(0),
    useRelativeTrial(reltrial), gotRelativeTrial(!reltrial),
    newTargetId(0), switchPCId(0), atTargetId(0),
    ctrlSignalId(0), daqSignalId(0)
{
    // get trial and output control points
    if (trialcps == 0 || outcps == 0)  {
        opserr << "ECxPCtarget::ECxPCtarget() - "
            << "null trialCPs or outCPs array passed.\n";
        exit(OF_ReturnType_failed);
    }
    trialCPs = trialcps;
    outCPs = outcps;
    
    // get total number of control and daq signals and
    // check if any signals use relative reference
    int ctrlIsRelative = 0;
    for (int i=0; i<numTrialCPs; i++)  {
        int numSignals = trialCPs[i]->getNumSignal();
        numCtrlSignals += numSignals;
        
        int j = 0;
        ID isRel = trialCPs[i]->getSigRefType();
        while (!ctrlIsRelative && j < numSignals)
            ctrlIsRelative = isRel(j++);
    }
    int daqIsRelative = 0;
    for (int i=0; i<numOutCPs; i++)  {
        int numSignals = outCPs[i]->getNumSignal();
        numDaqSignals += numSignals;
        
        int j = 0;
        ID isRel = outCPs[i]->getSigRefType();
        while (!daqIsRelative && j < numSignals)
            daqIsRelative = isRel(j++);
    }
    
    // resize offsets if any signals use relative reference
    if (ctrlIsRelative)
        ctrlSigOffset.resize(numCtrlSignals);
    if (daqIsRelative)
        daqSigOffset.resize(numDaqSignals);
    
    // split appFile into file name and path
    char *pos = strrchr(appFile, '/');
    if (pos != NULL)  {
        strcpy(appName, pos+1);
        *pos = '\0';
        strcpy(appPath, appFile);
    } else  {
        strcpy(appName, appFile);
        strcpy(appPath, "nopath");
    }
    delete [] appFile;
    appFile = 0;
    
    // initialize the xPC Target dynamic link library
    if (xPCInitAPI())  {
        opserr << "ECxPCtarget::ECxPCtarget()"
            << " - xPCInitAPI: unable to load xPC Target API.\n";
        exit(OF_ReturnType_failed);
    }
    
    // the host application OpenFresco needs to access the xPC Target;
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
    xPCSetLoadTimeOut(port, timeout);
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
    char currentAppName[256];
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
    this->ExperimentalControl::sleep(1000);
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
    numCtrlSignals(0), numDaqSignals(0), ctrlSignal(0), daqSignal(0),
    ctrlSigOffset(0), daqSigOffset(0), trialSigOffset(0),
    useRelativeTrial(0), gotRelativeTrial(1),
    newTargetId(0), switchPCId(0), atTargetId(0),
    ctrlSignalId(0), daqSignalId(0)
{
    numTrialCPs = ec.numTrialCPs;
    trialCPs    = ec.trialCPs;
    numOutCPs   = ec.numOutCPs;
    outCPs      = ec.outCPs;
    ipAddress   = ec.ipAddress;
    ipPort      = ec.ipPort;
    timeout     = ec.timeout;
    
    strcpy(appName,ec.appName);
    strcpy(appPath,ec.appPath);
    port = ec.port;
    
    numCtrlSignals = ec.numCtrlSignals;
    numDaqSignals  = ec.numDaqSignals;
    useRelativeTrial = ec.useRelativeTrial;
    gotRelativeTrial = ec.gotRelativeTrial;
    
    ctrlSigOffset  = ec.ctrlSigOffset;
    daqSigOffset   = ec.daqSigOffset;
}


ECxPCtarget::~ECxPCtarget()
{
    // stop the target application on the xPC Target
    xPCStopApp(port);
    
    // each application has to close the channel and unregister itself before exiting
    xPCClosePort(port);
    xPCFreeAPI();
    
    // delete memory of signal vectors
    if (ctrlSignal != 0)
        delete [] ctrlSignal;
    if (daqSignal != 0)
        delete [] daqSignal;
    
    // delete memory of strings
    if (ipAddress != 0)
        delete [] ipAddress;
    if (ipPort != 0)
        delete [] ipPort;
    
    // control points are not copies, so do not clean them up here
    //int i;
    //if (trialCPs != 0)  {
    //    for (i=0; i<numTrialCPs; i++)  {
    //        if (trialCPs[i] != 0)
    //            delete trialCPs[i];
    //    }
    //    delete [] trialCPs;
    //}
    //if (outCPs != 0)  {
    //    for (i=0; i<numOutCPs; i++)  {
    //        if (outCPs[i] != 0)
    //            delete outCPs[i];
    //    }
    //    delete [] outCPs;
    //}
    
    opserr << endln;
    opserr << "****************************************\n";
    opserr << "* The rtp application has been stopped *\n";
    opserr << "****************************************\n";
    opserr << endln;
}


int ECxPCtarget::setup()
{
    int rValue = 0;
    
    if (ctrlSignal != 0)
        delete [] ctrlSignal;
    if (daqSignal != 0)
        delete [] daqSignal;
    if (daqSignalId != 0)
        delete [] daqSignalId;
    
    // create control signal array
    ctrlSignal = new double [numCtrlSignals];
    if (ctrlSignal == 0)  {
        opserr << "ECxPCtarget::setup() - failed to create ctrlSignal array.\n";
        xPCClosePort(port);
        xPCFreeAPI();
        exit(OF_ReturnType_failed);
    }
    for (int i=0; i<numCtrlSignals; i++)
        ctrlSignal[i] = 0.0;
    
    // create daq signal array
    daqSignal = new double [numDaqSignals];
    daqSignalId = new int [numDaqSignals];
    if (daqSignal == 0 || daqSignalId == 0)  {
        opserr << "ECxPCtarget::setup() - failed to create daqSignal array.\n";
        xPCClosePort(port);
        xPCFreeAPI();
        exit(OF_ReturnType_failed);
    }
    for (int i=0; i<numDaqSignals; i++)  {
        daqSignal[i] = 0.0;
        daqSignalId[i] = 0;
    }
    
    // resize ctrl and daq signal offset vectors
    ctrlSigOffset.resize(numCtrlSignals);
    ctrlSigOffset.Zero();
    daqSigOffset.resize(numDaqSignals);
    daqSigOffset.Zero();
    
    // resize trial signal offset vector
    trialSigOffset.resize(numCtrlSignals);
    trialSigOffset.Zero();
    
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
    ctrlSignalId = xPCGetParamIdx(port, "xPC HC/targSignal", "Value");
    if (xPCGetLastError())  {
        xPCErrorMsg(xPCGetLastError(), errMsg);
        opserr << "ECxPCtarget::setup() - "
            << "xPCGetParamIdx - targSignal: error = " << errMsg << endln;
        xPCClosePort(port);
        xPCFreeAPI();
        exit(OF_ReturnType_failed);
    }
    if (numDaqSignals==1)  {
        daqSignalId[0] = xPCGetSignalIdx(port, "xPC HC/measSignal");
        if (xPCGetLastError())  {
            xPCErrorMsg(xPCGetLastError(), errMsg);
            opserr << "ECxPCtarget::setup() - "
                << "xPCGetSignalIdx - measSignal: error = " << errMsg << endln;
            xPCClosePort(port);
            xPCFreeAPI();
            exit(OF_ReturnType_failed);
        }
    } else  {
        char sigStr[25], numStr[3];
        for (int i=0; i<numDaqSignals; i++)  {
            strcpy(sigStr, "xPC HC/measSignal/s");
            sprintf(numStr, "%d", i+1);
            strcat(sigStr, numStr);
            daqSignalId[i] = xPCGetSignalIdx(port, sigStr);
            if (xPCGetLastError())  {
                xPCErrorMsg(xPCGetLastError(), errMsg);
                opserr << "ECxPCtarget::setup() - "
                    << "xPCGetSignalIdx - measSignal: error = " << errMsg << endln;
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
    this->ExperimentalControl::sleep(1000);
    
    do  {
        rValue += this->control();
        rValue += this->acquire();
        
        int i;
        opserr << "****************************************************************\n";
        opserr << "* Initial signal values of DAQ are:\n";
        opserr << "*\n";
        for (i=0; i<numDaqSignals; i++)
            opserr << "*   s" << i << " = " << daqSignal[i] << endln;
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
    // check sizeTrial and sizeOut against sizes
    // specified in the control points
    // ECxPCtarget objects can use:
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
            opserr << "ECxPCtarget::setSize() - wrong sizeTrial/Out\n"; 
            opserr << "see User Manual.\n";
            xPCClosePort(port);
            xPCFreeAPI();
            exit(OF_ReturnType_failed);
        }
    }
    // finally assign sizes
    (*sizeCtrl) = maxdofT;
    (*sizeDaq)  = maxdofO;
    
    return OF_ReturnType_completed;
}


int ECxPCtarget::setTrialResponse(
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
                ctrlSignal[k] = (*disp)(dof(j));
            }
            else if (rsp(j) == OF_Resp_Force  &&  force != 0)  {
                ctrlSignal[k] = (*force)(dof(j));
            }
            else if (rsp(j) == OF_Resp_Time  &&  time != 0)  {
                ctrlSignal[k] = (*time)(dof(j));
            }
            else if (rsp(j) == OF_Resp_Vel  &&  vel != 0)  {
                ctrlSignal[k] = (*vel)(dof(j));
            }
            else if (rsp(j) == OF_Resp_Accel  &&  accel != 0)  {
                ctrlSignal[k] = (*accel)(dof(j));
            }
            // filter control signal if the filter exists
            if (theCtrlFilters[rsp(j)] != 0)
                ctrlSignal[k] = theCtrlFilters[rsp(j)]->filtering(ctrlSignal[k]);
            k++;
        }
    }
    
    // send control signal array to controller
    k += this->control();
    
    return (k - numCtrlSignals);
}


int ECxPCtarget::getDaqResponse(
    Vector* disp,
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
                daqSignal[k] = theDaqFilters[rsp(j)]->filtering(daqSignal[k]);
            // populate the daq response vectors
            if (rsp(j) == OF_Resp_Disp  &&  disp != 0)  {
                (*disp)(dof(j)) = daqSignal[k];
            }
            else if (rsp(j) == OF_Resp_Force  &&  force != 0)  {
                (*force)(dof(j)) = daqSignal[k];
            }
            else if (rsp(j) == OF_Resp_Time  &&  time != 0)  {
                (*time)(dof(j)) = daqSignal[k];
            }
            else if (rsp(j) == OF_Resp_Vel  &&  vel != 0)  {
                (*vel)(dof(j)) = daqSignal[k];
            }
            else if (rsp(j) == OF_Resp_Accel  &&  accel != 0)  {
                (*accel)(dof(j)) = daqSignal[k];
            }
            k++;
        }
    }
    
    return (k + rValue - numDaqSignals);
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
        theResponse = new ExpControlResponse(this, 1,
            Vector(numCtrlSignals));
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
        theResponse = new ExpControlResponse(this, 2,
            Vector(numDaqSignals));
    }
    output.endTag();
    
    return theResponse;
}


int ECxPCtarget::getResponse(int responseID, Information &info)
{
    Vector resp(0);
    
    switch (responseID)  {
    case 1:  // ctrl signals
        resp.setData(ctrlSignal,numCtrlSignals);
        return info.setVector(resp);
        
    case 2:  // daq signals
        resp.setData(daqSignal,numDaqSignals);
        return info.setVector(resp);
        
    default:
        return OF_ReturnType_failed;
    }
}


void ECxPCtarget::Print(OPS_Stream &s, int flag)
{
    s << "****************************************************************\n";
    s << "* ExperimentalControl: " << this->getTag() << endln; 
    s << "*   type: ECxPCtarget\n";
    s << "*   ipAddress: " << ipAddress << "  ipPort: " << ipPort << endln;
    s << "*   appName: " << appName << endln;
    s << "*   appPath: " << appPath << endln;
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


int ECxPCtarget::control()
{
    // send ctrlSignal
    xPCSetParam(port, ctrlSignalId, ctrlSignal);
    if (xPCGetLastError())  {
        xPCErrorMsg(xPCGetLastError(), errMsg);
        opserr << "ECxPCtarget::control() - "
            << "xPCSetParam(ctrlSignal): error = " << errMsg << endln;
        xPCClosePort(port);
        xPCFreeAPI();
        return OF_ReturnType_failed;
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
        return OF_ReturnType_failed;
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
            return OF_ReturnType_failed;
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
        return OF_ReturnType_failed;
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
            return OF_ReturnType_failed;
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
            return OF_ReturnType_failed;
        }
    }
    
    // read measured signals at target
    xPCGetSignals(port, numDaqSignals, daqSignalId, daqSignal);
    if (xPCGetLastError())  {
        xPCErrorMsg(xPCGetLastError(), errMsg);
        opserr << "ECxPCtarget::acquire() - "
            << "xPCGetSignals(daqSignal): error = " << errMsg << endln;
        xPCClosePort(port);
        xPCFreeAPI();
        return OF_ReturnType_failed;
    }
    
    return OF_ReturnType_completed;
}
