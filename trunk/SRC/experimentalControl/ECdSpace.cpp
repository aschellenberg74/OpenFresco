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
// Created: 09/06
// Revision: A
//
// Description: This file contains the implementation of the ECdSpace class.

#include "ECdSpace.h"


ECdSpace::ECdSpace(int tag, int pctype, char *boardname)
    : ExperimentalControl(tag),
    pcType(pctype), boardName(boardname),
    ctrlDisp(0), ctrlVel(0), ctrlAccel(0), daqDisp(0), daqForce(0)
{
    // the host application OpenFresco needs to access the dSpace board
    // OpenSees is therefore registred with the DSP device driver
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
    
    opserr << "****************************************************************\n";
    opserr << "* The dSpace board " << boardName << " has been initialized\n";
    opserr << "****************************************************************\n";
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
    ctrlDisp(0), ctrlVel(0), ctrlAccel(0), daqDisp(0), daqForce(0)
{
    boardState = ec.boardState;
    simState = ec.simState;
    pcType = ec.pcType;
    boardName = ec.boardName;
    board_index = ec.board_index;
    simStateId = ec.simStateId;
}


ECdSpace::~ECdSpace()
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
    
    // delete memory of string
    if (boardName != 0)
        delete [] boardName;
    
    // stop the rtp application
    simState = 0;
    error = DS_write_32(board_index, simStateId, 1, (UInt32 *)&simState);
    if (error != DS_NO_ERROR)  {
        opserr << "ECdSpace::~ECdSpace() - "
            << "DS_write_32: error = " << error << endln;
        DS_unregister_host_app();
        exit(OF_ReturnType_failed);
    }
    
    // each application has to unregister itself before exiting
    DS_unregister_host_app();
    
    opserr << endln;
    opserr << "****************************************\n";
    opserr << "* The rtp application has been stopped *\n";
    opserr << "****************************************\n";
    opserr << endln;
}


int ECdSpace::setup()
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
        for (int i=0; i<(*sizeDaq)(OF_Resp_Disp); i++)
            daqDisp[i] = 0.0;
    }
    if ((*sizeDaq)(OF_Resp_Force) != 0)  {
        daqForce = new double [(*sizeDaq)(OF_Resp_Force)];
        for (int i=0; i<(*sizeDaq)(OF_Resp_Force); i++)
            daqForce[i] = 0.0;
    }
    
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
    error = DS_get_var_addr(board_index, "targDsp", &ctrlDispId);
    if (error != DS_NO_ERROR)  {
        opserr << "ECdSpace::setup() - DS_get_var_addr - "
            << "targDsp: error = " << error << endln;
        DS_unregister_host_app();
        exit(OF_ReturnType_failed);
    }
    if (pcType==2 || pcType==3)  {
        error = DS_get_var_addr(board_index, "targVel", &ctrlVelId);
        if (error != DS_NO_ERROR)  {
            opserr << "ECdSpace::setup() - DS_get_var_addr - "
                << "targVel: error = " << error << endln;
            DS_unregister_host_app();
            exit(OF_ReturnType_failed);
        }
    }
    if (pcType==3)  {
        error = DS_get_var_addr(board_index, "targAcc", &ctrlAccelId);
        if (error != DS_NO_ERROR)  {
            opserr << "ECdSpace::setup() - DS_get_var_addr - "
                << "targAcc: error = " << error << endln;
            DS_unregister_host_app();
            exit(OF_ReturnType_failed);
        }
    }
    error = DS_get_var_addr(board_index, "measDsp", &daqDispId);
    if (error != DS_NO_ERROR)  {
        opserr << "ECdSpace::setup() - DS_get_var_addr - "
            << "measDsp: error = " << error << endln;
        DS_unregister_host_app();
        exit(OF_ReturnType_failed);
    }
    error = DS_get_var_addr(board_index, "measFrc", &daqForceId);
    if (error != DS_NO_ERROR)  {
        opserr << "ECdSpace::setup() - DS_get_var_addr - "
            << "measFrc: error = " << error << endln;
        DS_unregister_host_app();
        exit(OF_ReturnType_failed);
    }
    
    // print experimental control information
    this->Print(opserr);
    
    opserr << "**************************************************************\n";
    opserr << "* Make sure that offset values of controller are set to ZERO *\n";
    opserr << "*                                                            *\n";
    opserr << "* Hit 'Enter' to proceed the initialization                  *\n";
    opserr << "**************************************************************\n";
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
    // check sizeTrial and sizeOut
    // for ECdSpace object
    
    // ECdSpace objects only use 
    // disp or disp and vel or disp, vel and accel for trial and
    // disp and force for output
    // check these are available in sizeT/sizeO.
    if ((pcType == 0 && sizeT(OF_Resp_Disp) == 0) || 
        (pcType == 1 && sizeT(OF_Resp_Disp) == 0 && sizeT(OF_Resp_Vel) == 0) ||
        (pcType == 2 && sizeT(OF_Resp_Disp) == 0 && sizeT(OF_Resp_Vel) == 0 && sizeT(OF_Resp_Accel) == 0) ||
        sizeO(OF_Resp_Disp) == 0 ||
        sizeO(OF_Resp_Force) == 0) {
        opserr << "ECdSpace::setSize() - wrong sizeTrial/Out\n"; 
        opserr << "see User Manual.\n";
        DS_unregister_host_app();
        exit(OF_ReturnType_failed);
    }
    
    *sizeCtrl = sizeT;
    *sizeDaq  = sizeO;
    
    return OF_ReturnType_completed;
}


int ECdSpace::setTrialResponse(const Vector* disp,
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


int ECdSpace::getDaqResponse(Vector* disp,
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


int ECdSpace::getResponse(int responseID, Information &info)
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


void ECdSpace::Print(OPS_Stream &s, int flag)
{
    s << "****************************************************************\n";
    s << "* ExperimentalControl: " << this->getTag() << endln; 
    s << "* type: ECdSpace\n";
    s << "*   boardName: " << boardName << endln;
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


int ECdSpace::control()
{
    // send ctrlDisp, ctrlVel and ctrlAccel and set newTarget flag
    error = DS_write_64(board_index, ctrlDispId, (*sizeCtrl)(OF_Resp_Disp), (UInt64 *)ctrlDisp);
    if (error != DS_NO_ERROR)  {
        opserr << "ECdSpace::control() - "
            << "DS_write_64(ctrlDisp): error = " << error << endln;
        DS_unregister_host_app();
        exit(OF_ReturnType_failed);
    }
    if (pcType==2 || pcType==3)  {
        error = DS_write_64(board_index, ctrlVelId, (*sizeCtrl)(OF_Resp_Vel), (UInt64 *)ctrlVel);
        if (error != DS_NO_ERROR)  {
            opserr << "ECdSpace::control() - "
                << "DS_write_64(ctrlVel): error = " << error << endln;
            DS_unregister_host_app();
            exit(OF_ReturnType_failed);
        }
    }
    if (pcType==3)  {
        error = DS_write_64(board_index, ctrlAccelId, (*sizeCtrl)(OF_Resp_Accel), (UInt64 *)ctrlAccel);
        if (error != DS_NO_ERROR)  {
            opserr << "ECdSpace::control() - "
                << "DS_write_64(ctrlAccel): error = " << error << endln;
            DS_unregister_host_app();
            exit(OF_ReturnType_failed);
        }
    }
    
    // set newTarget flag
    newTarget = 1;
    error = DS_write_32(board_index, newTargetId, 1, (UInt32 *)&newTarget);
    if (error != DS_NO_ERROR)  {
        opserr << "ECdSpace::control() - "
            << "DS_write_32(newTarget): error = " << error << endln;
        DS_unregister_host_app();
        exit(OF_ReturnType_failed);
    }
    
    // wait until switchPC flag has changed as well
    switchPC = 0;
    while (switchPC != 1)  {
        error = DS_read_32(board_index, switchPCId, 1, (UInt32 *)&switchPC);
        if (error != DS_NO_ERROR)  {
            opserr << "ECdSpace::control() - "
                << "DS_read_32(switchPC): error = " << error << endln;
            DS_unregister_host_app();
            exit(OF_ReturnType_failed);
        }
    }
    
    // reset newTarget flag
    newTarget = 0;
    error = DS_write_32(board_index, newTargetId, 1, (UInt32 *)&newTarget);
    if (error != DS_NO_ERROR)  {
        opserr << "ECdSpace::control() - "
            << "DS_write_32(newTarget): error = " << error << endln;
        DS_unregister_host_app();
        exit(OF_ReturnType_failed);
    }
    
    // wait until switchPC flag has changed as well
    switchPC = 1;
    while (switchPC != 0)  {
        error = DS_read_32(board_index, switchPCId, 1, (UInt32 *)&switchPC);
        if (error != DS_NO_ERROR)  {
            opserr << "ECdSpace::control() - "
                << "DS_read_32(switchPC): error = " << error << endln;
            DS_unregister_host_app();
            exit(OF_ReturnType_failed);
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
            exit(OF_ReturnType_failed);
        }
    }
    
    // read displacements and resisting forces at target
    error = DS_read_64(board_index, daqDispId, (*sizeDaq)(OF_Resp_Disp), (UInt64 *)daqDisp);
    if (error != DS_NO_ERROR)  {
        opserr << "ECdSpace::acquire() - "
            << "DS_read_64(daqDisp): error = " << error << endln;
        DS_unregister_host_app();
        exit(OF_ReturnType_failed);
    }
    error = DS_read_64(board_index, daqForceId, (*sizeDaq)(OF_Resp_Force), (UInt64 *)daqForce);
    if (error != DS_NO_ERROR)  {
        opserr << "ECdSpace::acquire() - "
            << "DS_read_64(daqForce): error = " << error << endln;
        DS_unregister_host_app();
        exit(OF_ReturnType_failed);
    }
    
    return OF_ReturnType_completed;
}


void ECdSpace::sleep(const clock_t wait)
{
    clock_t goal;
    goal = wait + clock();
    while (goal>clock());
}
