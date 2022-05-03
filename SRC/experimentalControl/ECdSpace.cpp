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
// Description: This file contains the implementation of the ECdSpace class.

#include "ECdSpace.h"
#include <ExperimentalCP.h>

#include <elementAPI.h>


void* OPF_ECdSpace()
{
    // pointer to experimental control that will be returned
    ExperimentalControl* theControl = 0;
    
    if (OPS_GetNumRemainingInputArgs() < 6) {
        opserr << "WARNING invalid number of arguments\n";
        opserr << "Want: expControl dSpace tag boardName -trialCP cpTags -outCP cpTags "
            << "<-ctrlFilters (5 filterTag)> <-daqFilters (5 filterTag)>\n";
        return 0;
    }
    
    // control tag
    int tag;
    int numdata = 1;
    if (OPS_GetIntInput(&numdata, &tag) != 0) {
        opserr << "WARNING invalid expControl dSpace tag\n";
        return 0;
    }
    
    // boardName
    char* boardName;
    const char* type = OPS_GetString();
    boardName = new char[strlen(type) + 1];
    strcpy(boardName, type);
    
    // trialCPs
    type = OPS_GetString();
    if (strcmp(type, "-trialCP") != 0) {
        opserr << "WARNING expecting -trialCP cpTags\n";
        opserr << "expControl dSpace " << tag << endln;
        return 0;
    }
    ID cpTags(32);
    int numTrialCPs = 0;
    while (OPS_GetNumRemainingInputArgs() > 0) {
        int cpTag;
        numdata = 1;
        int numArgs = OPS_GetNumRemainingInputArgs();
        if (OPS_GetIntInput(&numdata, &cpTag) < 0) {
            if (numArgs > OPS_GetNumRemainingInputArgs()) {
                // move current arg back by one
                OPS_ResetCurrentInputArg(-1);
            }
            break;
        }
        cpTags(numTrialCPs++) = cpTag;
    }
    if (numTrialCPs == 0) {
        opserr << "WARNING no trialCPTags specified\n";
        opserr << "expControl dSpace " << tag << endln;
        return 0;
    }
    cpTags.resize(numTrialCPs);
    // create the array to hold the trial control points
    ExperimentalCP** trialCPs = new ExperimentalCP * [numTrialCPs];
    if (trialCPs == 0) {
        opserr << "WARNING out of memory\n";
        opserr << "expControl dSpace " << tag << endln;
        return 0;
    }
    // populate array with trial control points
    for (int i = 0; i < numTrialCPs; i++) {
        trialCPs[i] = 0;
        trialCPs[i] = OPF_getExperimentalCP(cpTags(i));
        if (trialCPs[i] == 0) {
            opserr << "WARNING experimental control point not found\n";
            opserr << "expControlPoint " << cpTags(i) << endln;
            opserr << "expControl dSpace " << tag << endln;
            return 0;
        }
    }
    
    // outCPs
    type = OPS_GetString();
    if (strcmp(type, "-outCP") != 0) {
        opserr << "WARNING expecting -outCP cpTags\n";
        opserr << "expControl dSpace " << tag << endln;
        return 0;
    }
    cpTags.resize(32); cpTags.Zero();
    int numOutCPs = 0;
    while (OPS_GetNumRemainingInputArgs() > 0) {
        int cpTag;
        numdata = 1;
        int numArgs = OPS_GetNumRemainingInputArgs();
        if (OPS_GetIntInput(&numdata, &cpTag) < 0) {
            if (numArgs > OPS_GetNumRemainingInputArgs()) {
                // move current arg back by one
                OPS_ResetCurrentInputArg(-1);
            }
            break;
        }
        cpTags(numOutCPs++) = cpTag;
    }
    if (numOutCPs == 0) {
        opserr << "WARNING no outCPTags specified\n";
        opserr << "expControl dSpace " << tag << endln;
        return 0;
    }
    cpTags.resize(numOutCPs);
    // create the array to hold the output control points
    ExperimentalCP** outCPs = new ExperimentalCP * [numOutCPs];
    if (outCPs == 0) {
        opserr << "WARNING out of memory\n";
        opserr << "expControl dSpace " << tag << endln;
        return 0;
    }
    // populate array with output control points
    for (int i = 0; i < numOutCPs; i++) {
        outCPs[i] = 0;
        outCPs[i] = OPF_getExperimentalCP(cpTags(i));
        if (outCPs[i] == 0) {
            opserr << "WARNING experimental control point not found\n";
            opserr << "expControlPoint " << cpTags(i) << endln;
            opserr << "expControl dSpace " << tag << endln;
            return 0;
        }
    }
    
    // parsing was successful, allocate the control
    theControl = new ECdSpace(tag, numTrialCPs, trialCPs,
        numOutCPs, outCPs, boardName);
    if (theControl == 0) {
        opserr << "WARNING could not create experimental control of type dSpace\n";
        return 0;
    }
    
    // cleanup dynamic memory
    //if (trialCPs != 0)
    //    delete[] trialCPs;
    //if (outCPs != 0)
    //    delete[] outCPs;
    
    return theControl;
}


ECdSpace::ECdSpace(int tag, int nTrialCPs, ExperimentalCP **trialcps,
    int nOutCPs, ExperimentalCP **outcps, char *boardname)
    : ExperimentalControl(tag),
    numTrialCPs(nTrialCPs), numOutCPs(nOutCPs), boardName(boardname),
    numCtrlSignals(0), numDaqSignals(0), ctrlSignal(0), daqSignal(0),
    simStateId(0), newTargetId(0), switchPCId(0), atTargetId(0),
    ctrlSignalId(0), daqSignalId(0)
{
    // get trial and output control points
    if (trialcps == 0 || outcps == 0)  {
        opserr << "ECdSpace::ECdSpace() - "
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
    
    // the host application OpenFresco needs to access the dSpace board
    // OpenFresco is therefore registred with the DSP device driver
    error = DS_register_host_app("OpenFresco");
    if (error != DS_NO_ERROR)  {
        opserr << "ECdSpace::ECdSpace() - "
            << "DS_register_host_app: error = " << error << endln;
        exit(OF_ReturnType_failed);
    }
    
    // before accessing the processor board, the board index of the
    // board is obtained from the DSP device driver
    error = DS_board_index(boardName, &board_index);
    if (error != DS_NO_ERROR)  {
        opserr << "ECdSpace::ECdSpace() - a board named " << boardName
            << " is not registered with the DSP device driver.\n";
        DS_unregister_host_app();
        exit(OF_ReturnType_failed);
    }
    
    // in order to get more information about the selected board the CLIB
    // function DS_board_spec() is called
    error = DS_board_spec(board_index, &board_spec);
    if (error != DS_NO_ERROR)  {
        opserr << "ECdSpace::ECdSpace() - "
            << "DS_board_spec: error = " << error << endln;
        DS_unregister_host_app();
        exit(OF_ReturnType_failed);
    }
    
    opserr << "************************************************\n";
    opserr << "* The dSpace board " << boardName << " has been initialized *\n";
    opserr << "************************************************\n";
    opserr << endln;
    
    // before accessing the board it is required to check if an application is loaded
    // as well as its current status on the board
    error = DS_is_reset(board_index, &boardState);
    if (error != DS_NO_ERROR)  {
        opserr << "ECdSpace::ECdSpace() - "
            << "DS_is_reset: error = " << error << endln;
        DS_unregister_host_app();
        exit(OF_ReturnType_failed);
    }
    if (boardState == DS_TRUE)  {
        opserr << "ECdSpace::ECdSpace() - "
            << "No rtp application is loaded on the board - error" << endln;
        DS_unregister_host_app();
        exit(OF_ReturnType_failed);
    } else  {
        error = DS_get_var_addr(board_index, "simState", &simStateId);
        if (error != DS_NO_ERROR)  {
            opserr << "ECdSpace::ECdSpace() - "
                << "DS_get_var_addr: error = " << error << endln;
            DS_unregister_host_app();
            exit(OF_ReturnType_failed);
        } else  {
            error = DS_read_32(board_index, simStateId, 1, (UInt32 *)&simState);
            if (error != DS_NO_ERROR)  {
                opserr << "ECdSpace::ECdSpace() - "
                    << "DS_read_32: error = " << error << endln;
                DS_unregister_host_app();
                exit(OF_ReturnType_failed);
            }
        }
        if (simState != 0)  {
            simState = 0;
            error = DS_write_32(board_index, simStateId, 1, (UInt32 *)&simState);
            if (error != DS_NO_ERROR)  {
                opserr << "ECdSpace::ECdSpace() - "
                    << "DS_write_32: error = " << error << endln;
                DS_unregister_host_app();
                exit(OF_ReturnType_failed);
            }
        }
        opserr << "******************************************************\n";
        opserr << "* The rtp application has been loaded and is stopped *\n";
        opserr << "******************************************************\n";
        opserr << endln;
    }
}


ECdSpace::ECdSpace(const ECdSpace &ec)
    : ExperimentalControl(ec),
    numCtrlSignals(0), numDaqSignals(0), ctrlSignal(0), daqSignal(0),
    simStateId(0), newTargetId(0), switchPCId(0), atTargetId(0),
    ctrlSignalId(0), daqSignalId(0)
{
    numTrialCPs = ec.numTrialCPs;
    trialCPs    = ec.trialCPs;
    numOutCPs   = ec.numOutCPs;
    outCPs      = ec.outCPs;
    boardName   = ec.boardName;
    
    boardState  = ec.boardState;
    simState    = ec.simState;
    board_index = ec.board_index;
    board_spec  = ec.board_spec;
    simStateId  = ec.simStateId;
    
    numCtrlSignals = ec.numCtrlSignals;
    numDaqSignals  = ec.numDaqSignals;
}


ECdSpace::~ECdSpace()
{
    // stop the rtp application
    simState = 0;
    error = DS_write_32(board_index, simStateId, 1, (UInt32 *)&simState);
    if (error != DS_NO_ERROR)  {
        opserr << "ECdSpace::~ECdSpace() - "
            << "DS_write_32: error = " << error << endln;
    }
    
    // each application has to unregister itself before exiting
    DS_unregister_host_app();
    
    // delete memory of signal vectors
    if (ctrlSignal != 0)
        delete [] ctrlSignal;
    if (daqSignal != 0)
        delete [] daqSignal;
    
    // delete memory of string
    if (boardName != 0)
        delete [] boardName;
    
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


int ECdSpace::setup()
{
    int rValue = 0;
    
    if (ctrlSignal != 0)
        delete [] ctrlSignal;
    if (daqSignal != 0)
        delete [] daqSignal;
    
    // create control signal array
    ctrlSignal = new double [numCtrlSignals];
    if (ctrlSignal == 0)  {
        opserr << "ECdSpace::setup() - failed to create ctrlSignal array.\n";
        DS_unregister_host_app();
        exit(OF_ReturnType_failed);
    }
    for (int i=0; i<numCtrlSignals; i++)
        ctrlSignal[i] = 0.0;
    
    // create daq signal array
    daqSignal = new double [numDaqSignals];
    if (daqSignal == 0)  {
        opserr << "ECdSpace::setup() - failed to create daqSignal array.\n";
        DS_unregister_host_app();
        exit(OF_ReturnType_failed);
    }
    for (int i=0; i<numDaqSignals; i++)
        daqSignal[i] = 0.0;
    
    // get addresses of the controlled variables on the DSP board
    error = DS_get_var_addr(board_index, "newTarget", &newTargetId);
    if (error != DS_NO_ERROR)  {
        opserr << "ECdSpace::setup() - DS_get_var_addr - "
            << "newTarget: error = " << error << endln;
        DS_unregister_host_app();
        exit(OF_ReturnType_failed);
    }
    error = DS_get_var_addr(board_index, "switchPC", &switchPCId);
    if (error != DS_NO_ERROR)  {
        opserr << "ECdSpace::setup() - DS_get_var_addr - "
            << "switchPC: error = " << error << endln;
        DS_unregister_host_app();
        exit(OF_ReturnType_failed);
    }
    error = DS_get_var_addr(board_index, "atTarget", &atTargetId);
    if (error != DS_NO_ERROR)  {
        opserr << "ECdSpace::setup() - DS_get_var_addr - "
            << "atTarget: error = " << error << endln;
        DS_unregister_host_app();
        exit(OF_ReturnType_failed);
    }
    error = DS_get_var_addr(board_index, "targSignal", &ctrlSignalId);
    if (error != DS_NO_ERROR)  {
        opserr << "ECdSpace::setup() - DS_get_var_addr - "
            << "targSignal: error = " << error << endln;
        DS_unregister_host_app();
        exit(OF_ReturnType_failed);
    }
    error = DS_get_var_addr(board_index, "measSignal", &daqSignalId);
    if (error != DS_NO_ERROR)  {
        opserr << "ECdSpace::setup() - DS_get_var_addr - "
            << "measSignal: error = " << error << endln;
        DS_unregister_host_app();
        exit(OF_ReturnType_failed);
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
        DS_unregister_host_app();
        exit(OF_ReturnType_failed);
    }
    
    // start the rtp application
    simState = 2;
    error = DS_write_32(board_index, simStateId, 1, (UInt32 *)&simState);
    if (error != DS_NO_ERROR)  {
        opserr << "ECdSpace::setup() - "
            << "DS_write_32(simState): error = " << error << endln;
        DS_unregister_host_app();
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
            simState = 0;
            DS_write_32(board_index, simStateId, 1, (UInt32 *)&simState);
            DS_unregister_host_app();
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


int ECdSpace::setSize(ID sizeT, ID sizeO)
{
    // check sizeTrial and sizeOut against sizes
    // specified in the control points
    // ECdSpace objects can use:
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
            opserr << "ECdSpace::setSize() - wrong sizeTrial/Out\n"; 
            opserr << "see User Manual.\n";
            DS_unregister_host_app();
            exit(OF_ReturnType_failed);
        }
    }
    // finally assign sizes
    (*sizeCtrl) = maxdofT;
    (*sizeDaq)  = maxdofO;
    
    return OF_ReturnType_completed;
}


int ECdSpace::setTrialResponse(
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


int ECdSpace::getDaqResponse(
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


int ECdSpace::commitState()
{
    return OF_ReturnType_completed;
}


ExperimentalControl *ECdSpace::getCopy()
{
    return new ECdSpace(*this);
}


Response* ECdSpace::setResponse(const char **argv, int argc,
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


int ECdSpace::getResponse(int responseID, Information &info)
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


void ECdSpace::Print(OPS_Stream &s, int flag)
{
    s << "****************************************************************\n";
    s << "* ExperimentalControl: " << this->getTag() << endln; 
    s << "* type: ECdSpace\n";
    s << "*   boardName: " << boardName;
    s << "\n*   trialCPs:";
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


int ECdSpace::control()
{
    // send ctrlSignal
    error = DS_write_64(board_index, ctrlSignalId, numCtrlSignals, (UInt64 *)ctrlSignal);
    if (error != DS_NO_ERROR)  {
        opserr << "ECdSpace::control() - "
            << "DS_write_64(ctrlSignal): error = " << error << endln;
        DS_unregister_host_app();
        return -error;
    }
    
    // set newTarget flag
    newTarget = 1;
    error = DS_write_32(board_index, newTargetId, 1, (UInt32 *)&newTarget);
    if (error != DS_NO_ERROR)  {
        opserr << "ECdSpace::control() - "
            << "DS_write_32(newTarget): error = " << error << endln;
        DS_unregister_host_app();
        return -error;
    }
    
    // wait until switchPC flag has changed as well
    switchPC = 0;
    while (switchPC != 1)  {
        error = DS_read_32(board_index, switchPCId, 1, (UInt32 *)&switchPC);
        if (error != DS_NO_ERROR)  {
            opserr << "ECdSpace::control() - "
                << "DS_read_32(switchPC): error = " << error << endln;
            DS_unregister_host_app();
            return -error;
        }
    }
    
    // reset newTarget flag
    newTarget = 0;
    error = DS_write_32(board_index, newTargetId, 1, (UInt32 *)&newTarget);
    if (error != DS_NO_ERROR)  {
        opserr << "ECdSpace::control() - "
            << "DS_write_32(newTarget): error = " << error << endln;
        DS_unregister_host_app();
        return -error;
    }
    
    // wait until switchPC flag has changed as well
    switchPC = 1;
    while (switchPC != 0)  {
        error = DS_read_32(board_index, switchPCId, 1, (UInt32 *)&switchPC);
        if (error != DS_NO_ERROR)  {
            opserr << "ECdSpace::control() - "
                << "DS_read_32(switchPC): error = " << error << endln;
            DS_unregister_host_app();
            return -error;
        }
    }
    
    return OF_ReturnType_completed;
}


int ECdSpace::acquire()
{
    // wait until target is reached
    atTarget = 0;
    while (atTarget != 1)  {
        error = DS_read_32(board_index, atTargetId, 1, (UInt32 *)&atTarget);
        if (error != DS_NO_ERROR)  {
            opserr << "ECdSpace::acquire() - "
                << "DS_read_32(atTarget): error = " << error << endln;
            DS_unregister_host_app();
            return -error;
        }
    }
    
    // read measured signals at target
    error = DS_read_64(board_index, daqSignalId, numDaqSignals, (UInt64 *)daqSignal);
    if (error != DS_NO_ERROR)  {
        opserr << "ECdSpace::acquire() - "
            << "DS_read_64(daqSignal): error = " << error << endln;
        DS_unregister_host_app();
        return -error;
    }
    
    return OF_ReturnType_completed;
}
