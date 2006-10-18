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
// Description: This file contains the implementation of the ECdSpace class.

#include "ECdSpace.h"

#include <clib32.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


ECdSpace::ECdSpace(int tag, int pctype, char *boardname)
    : ExperimentalControl(tag),
    pcType(pctype), boardName(boardname),
    targDisp(0), targVel(0), targAccel(0), measDisp(0), measForce(0)
{	
    // open output file
    //outFile = fopen("ECdSpace.out","w");
    //if (outFile==NULL)  {
    //	opserr << "ECdSpace::ECdSpace()"
    //		<< " - fopen: could not open output file" << endln;
    //  exit(OF_ReturnType_failed);
    //}
    
    // the host application OpenSees needs to access the dSpace board
    // OpenSees is therefore registred with the DSP device driver
    error = DS_register_host_app("OpenSees");
    if (error != DS_NO_ERROR)  {
        opserr << "ECdSpace::ECdSpace()"
            << " - DS_register_host_app: error = " << error << endln;
        exit(OF_ReturnType_failed);
    }
    
    // before accessing the processor board, the board index of the
    // board is obtained from the DSP device driver
    error = DS_board_index(boardName, &board_index);	
    if (error != DS_NO_ERROR)  {
        opserr << "ECdSpace::ECdSpace() - a board named " << boardName
            << " is not registered with the DSP device driver" << endln;
        DS_unregister_host_app();
        exit(OF_ReturnType_failed);
    }
    
    // in order to get more information about the selected board the CLIB
    // function DS_board_spec() is called
    error = DS_board_spec(board_index, &board_spec);	
    if (error != DS_NO_ERROR)  {
        opserr << "ECdSpace::ECdSpace()"
            << " - DS_board_spec: error = " << error << endln;
        DS_unregister_host_app();
        exit(OF_ReturnType_failed);
    }
    
    opserr << "*****************************************\n";
    opserr << "* The board " << boardName << " has been initialized *\n";
    opserr << "*****************************************\n";
    opserr << endln;
    
    // before accessing the board it is required to check if an application is loaded
    // as well as its current status on the board
    error = DS_is_reset(board_index, &boardState);
    if (error != DS_NO_ERROR)  {
        opserr << "ECdSpace::ECdSpace() - DS_is_reset: error = " << error << endln;
        DS_unregister_host_app();
        exit(OF_ReturnType_failed);
    }
    if (boardState == DS_TRUE)  {
        opserr << "ECdSpace::ECdSpace() - No rtp application is loaded on the board - error" << endln;
        DS_unregister_host_app();
        exit(OF_ReturnType_failed);
    } else  {
        error = DS_get_var_addr(board_index, "simState", &simStateId);
        if (error != DS_NO_ERROR)  {
            opserr << "ECdSpace::ECdSpace() - DS_get_var_addr: error = " << error << endln;
            DS_unregister_host_app();
            exit(OF_ReturnType_failed);
        } else  {
            error = DS_read_32(board_index, simStateId, 1, (UInt32 *)&simState);
            if (error != DS_NO_ERROR)  {
                opserr << "ECdSpace::ECdSpace() - DS_read_32: error = " << error << endln;
                DS_unregister_host_app();
                exit(OF_ReturnType_failed);
            }
        }
        if (simState != 0)  {
            simState = 0;
            error = DS_write_32(board_index, simStateId, 1, (UInt32 *)&simState);
            if (error != DS_NO_ERROR)  {
                opserr << "ECdSpace::ECdSpace() - DS_write_32: error = " << error << endln;
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
    : ExperimentalControl(ec)
{	
    pcType = ec.pcType;
    boardName = ec.boardName;
    
    targDisp = new double[(*sizeCtrl)[OF_Resp_Disp]];
    targVel = new double[(*sizeCtrl)[OF_Resp_Vel]];
    targAccel = new double[(*sizeCtrl)[OF_Resp_Accel]];
    
    measDisp = new double[(*sizeDaq)[OF_Resp_Disp]];
    measForce = new double[(*sizeDaq)[OF_Resp_Force]];
}


ECdSpace::~ECdSpace()
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
    
    // stop the rtp application
    simState = 0;
    error = DS_write_32(board_index, simStateId, 1, (UInt32 *)&simState);
    if (error != DS_NO_ERROR)  {
        opserr << "ECdSpace::~ECdSpace() - DS_write_32: error = " << error << endln;
        DS_unregister_host_app();
        exit(OF_ReturnType_failed);
    }
    
    // each application has to unregister itself before exiting
    DS_unregister_host_app();
    
    opserr << "****************************************\n";
    opserr << "* The rtp application has been stopped *\n";
    opserr << "****************************************\n";
    opserr << endln;
}


int ECdSpace::setSize(ID sizeT, ID sizeO)
{
    // check sizeTrial and sizeOut
    // for ECdSpace object
    
    // ECdSpace object only use 
    // disp or disp and vel or disp, vel and accel for trial
    // disp and force for output
    // check these are available in sizeT/sizeO.
    
    if((pcType == 0 && sizeT[OF_Resp_Disp] == 0) || 
        (pcType == 1 && sizeT[OF_Resp_Disp] == 0 && sizeT[OF_Resp_Vel] == 0) ||
        (pcType == 2 && sizeT[OF_Resp_Disp] == 0 && sizeT[OF_Resp_Vel] == 0 && sizeT[OF_Resp_Accel] == 0) ||
        sizeO[OF_Resp_Disp] == 0 ||
        sizeO[OF_Resp_Force] == 0) {
        opserr << "ECdSpace::setSize - wrong sizeTrial/Out\n"; 
        opserr << "see User Manual.\n";
        return OF_ReturnType_failed;
    }
    
    *sizeCtrl = sizeT;
    *sizeDaq = sizeO;
    
    return OF_ReturnType_completed;
}


int ECdSpace::setup()
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
    if ((*sizeCtrl)[OF_Resp_Disp]=!0  && pcType==2)  {
        targVel = new double[(*sizeCtrl)[OF_Resp_Vel]];
        for (int i=0; i<(*sizeCtrl)[OF_Resp_Vel]; i++)
            targVel[i] = 0.0;
    }
    if ((*sizeCtrl)[OF_Resp_Disp]=!0 && pcType==3)  {
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
        for (int i=0; i<(*sizeDaq)[OF_Resp_Disp]; i++)
            measDisp[i] = 0.0;
    }
    if ((*sizeDaq)[OF_Resp_Force] =! 0)  {
        measForce = new double[(*sizeDaq)[OF_Resp_Force]];
        for (int i=0; i<(*sizeDaq)[OF_Resp_Force]; i++)
            measForce[i] = 0.0;
    }
    
    // get addresses of the controlled variables on the DSP board		  
    error = DS_get_var_addr(board_index, "updateFlag", &updateFlagId);
    if (error != DS_NO_ERROR)  {
        opserr << "ECdSpace::ECdSpace() - DS_get_var_addr - updateFlag: error = " << error << endln;
        DS_unregister_host_app();
        return OF_ReturnType_failed;
    }
    error = DS_get_var_addr(board_index, "targetFlag", &targetFlagId);
    if (error != DS_NO_ERROR)  {
        opserr << "ECdSpace::ECdSpace() - DS_get_var_addr - targetFlag: error = " << error << endln;
        DS_unregister_host_app();
        return OF_ReturnType_failed;
    }
    error = DS_get_var_addr(board_index, "targDsp", &targDispId);
    if (error != DS_NO_ERROR)  {
        opserr << "ECdSpace::ECdSpace() - DS_get_var_addr - targDsp: error = " << error << endln;
        DS_unregister_host_app();
        return OF_ReturnType_failed;
    }
    if (pcType==2 || pcType==3)  {
        error = DS_get_var_addr(board_index, "targVel", &targVelId);
        if (error != DS_NO_ERROR)  {
            opserr << "ECdSpace::ECdSpace() - DS_get_var_addr - targVel: error = " << error << endln;
            DS_unregister_host_app();
            return OF_ReturnType_failed;
        }
    }
    if (pcType==3)  {
        error = DS_get_var_addr(board_index, "targAcc", &targAccelId);
        if (error != DS_NO_ERROR)  {
            opserr << "ECdSpace::ECdSpace() - DS_get_var_addr - targAcc: error = " << error << endln;
            DS_unregister_host_app();
            return OF_ReturnType_failed;
        }
    }
    error = DS_get_var_addr(board_index, "measDsp", &measDispId);
    if (error != DS_NO_ERROR)  {
        opserr << "ECdSpace::ECdSpace() - DS_get_var_addr - measDsp: error = " << error << endln;
        DS_unregister_host_app();
        return OF_ReturnType_failed;
    }
    error = DS_get_var_addr(board_index, "measFrc", &measForceId);
    if (error != DS_NO_ERROR)  {
        opserr << "ECdSpace::ECdSpace() - DS_get_var_addr - measFrc: error = " << error << endln;
        DS_unregister_host_app();
        return OF_ReturnType_failed;
    }
    
    // print experimental control information
    this->Print(opserr);
    
    opserr << "**************************************************************\n";
    opserr << "* Make sure that offset values of controller are set to ZERO *\n";
    opserr << "*                                                            *\n";
    opserr << "* Hit 'Enter' to proceed the initialization                  *\n";
    opserr << "**************************************************************\n";
    opserr << endln;
    char c = getchar();
    if (c == 'c')  {
        DS_unregister_host_app();
        return OF_ReturnType_failed;
    }
    
    do  {
        // start the rtp application
        simState = 2;
        error = DS_write_32(board_index, simStateId, 1, (UInt32 *)&simState);
        if (error != DS_NO_ERROR)  {
            opserr << "ECdSpace::ECdSpace() - DS_write_32: error = " << error << endln;
            DS_unregister_host_app();
            return OF_ReturnType_failed;
        }
        
        // reset the updateFlag and targDisp, targVel and targAccel
        updateFlag = 0;
        error = DS_write_64(board_index, updateFlagId, 1, (UInt64 *)&updateFlag);
        if (error != DS_NO_ERROR)  {
            opserr << "ECdSpace::ECdSpace() - DS_write_64: error = " << error << endln;
            DS_unregister_host_app();
            return OF_ReturnType_failed;
        }
        error = DS_write_64(board_index, targDispId, (*sizeCtrl)[OF_Resp_Disp], (UInt64 *)targDisp);
        if (error != DS_NO_ERROR)  {
            opserr << "ECdSpace::ECdSpace() - DS_write_64: error = " << error << endln;
            DS_unregister_host_app();
            return OF_ReturnType_failed;
        }
        if (pcType==2 || pcType==3)  {
            error = DS_write_64(board_index, targVelId, (*sizeCtrl)[OF_Resp_Vel], (UInt64 *)targVel);
            if (error != DS_NO_ERROR)  {
                opserr << "ECdSpace::ECdSpace() - DS_write_64: error = " << error << endln;
                DS_unregister_host_app();
                return OF_ReturnType_failed;
            }
        }
        if (pcType==3)  {
            error = DS_write_64(board_index, targAccelId, (*sizeCtrl)[OF_Resp_Accel], (UInt64 *)targAccel);
            if (error != DS_NO_ERROR)  {
                opserr << "ECdSpace::ECdSpace() - DS_write_64: error = " << error << endln;
                DS_unregister_host_app();
                return OF_ReturnType_failed;
            }
        }
        updateFlag = 1;
        error = DS_write_64(board_index, updateFlagId, 1, (UInt64 *)&updateFlag);
        if (error != DS_NO_ERROR)  {
            opserr << "ECdSpace::ECdSpace() - DS_write_64: error = " << error << endln;
            DS_unregister_host_app();
            return OF_ReturnType_failed;
        }
        
        // wait until target is reached
        targetFlag = 1;
        while (targetFlag)  {
            // pause for 1msec
            this->sleep(1);
            
            // get target flag
            error = DS_read_64(board_index, targetFlagId, 1, (UInt64 *)&targetFlag);
            if (error != DS_NO_ERROR)  {
                opserr << "ECdSpace::ECdSpace() - DS_read_64: error = " << error << endln;
                DS_unregister_host_app();
                return OF_ReturnType_failed;
            }
        }
        
        // read displacements and resisting forces from board
        error = DS_read_64(board_index, measDispId, (*sizeDaq)[OF_Resp_Disp], (UInt64 *)measDisp);
        if (error != DS_NO_ERROR)  {
            opserr << "ECdSpace::ECdSpace() - DS_read_64: error = " << error << endln;
            DS_unregister_host_app();
            return OF_ReturnType_failed;
        }
        error = DS_read_64(board_index, measForceId, (*sizeDaq)[OF_Resp_Force], (UInt64 *)measForce);
        if (error != DS_NO_ERROR)  {
            opserr << "ECdSpace::ECdSpace() - DS_read_64: error = " << error << endln;
            DS_unregister_host_app();
            return OF_ReturnType_failed;
        }
        
        // reset updateFlag
        updateFlag = 0;
        error = DS_write_64(board_index, updateFlagId, 1, (UInt64 *)&updateFlag);
        if (error != DS_NO_ERROR)  {
            opserr << "ECdSpace::ECdSpace() - DS_write_64: error = " << error << endln;
            DS_unregister_host_app();
            return OF_ReturnType_failed;
        }
        // pause for 1msec
        this->sleep(1);
        
        // stop the rtp application
        simState = 0;
        error = DS_write_32(board_index, simStateId, 1, (UInt32 *)&simState);
        if (error != DS_NO_ERROR)  {
            opserr << "ECdSpace::ECdSpace() - DS_write_32: error = " << error << endln;
            DS_unregister_host_app();
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
            DS_unregister_host_app();
            return OF_ReturnType_failed;
        } else if (c == 'r')  {
            getchar();
        }
    } while (c == 'r');
    
    // start the rtp application
    simState = 2;
    error = DS_write_32(board_index, simStateId, 1, (UInt32 *)&simState);
    if (error != DS_NO_ERROR)  {
        opserr << "ECdSpace::ECdSpace() - DS_write_32: error = " << error << endln;
        DS_unregister_host_app();
        return OF_ReturnType_failed;
    }
    
    opserr << "******************\n";
    opserr << "* Running......  *\n";
    opserr << "******************\n";
    opserr << endln;
    
    return OF_ReturnType_completed;
}


int ECdSpace::setTrialResponse(const Vector* disp,
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


int ECdSpace::getDaqResponse(Vector* disp,
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


int ECdSpace::commitState()
{	
    return OF_ReturnType_completed;
}


ExperimentalControl *ECdSpace::getCopy()
{
    return new ECdSpace(*this);
}


void ECdSpace::Print(OPS_Stream &s, int flag)
{
    s << "****************************************************************\n";
    s << "* ExperimentalControl: " << this->getTag() << endln; 
    s << "* type: ECdSpace\n";
    s << "*   boardName: " << boardName << endln;
    s << "*   pcType: " << pcType << endln;
    if (theFilter != 0) {
        s << "*\tFilter: " << *theFilter << endln;
    }
    s << "****************************************************************\n";
    s << endln;
}


int ECdSpace::control()
{
    // send targDisp, targVel and targAccel and set updateFlag
    error = DS_write_64(board_index, targDispId, (*sizeCtrl)[OF_Resp_Disp], (UInt64 *)targDisp);
    if (error != DS_NO_ERROR)  {
        opserr << "ECdSpace::execute() - DS_write_64: error = " << error << endln;
        DS_unregister_host_app();
        return OF_ReturnType_failed;
    }
    if (pcType==2 || pcType==3)  {
        error = DS_write_64(board_index, targVelId, (*sizeCtrl)[OF_Resp_Vel], (UInt64 *)targVel);
        if (error != DS_NO_ERROR)  {
            opserr << "ECdSpace::execute() - DS_write_64: error = " << error << endln;
            DS_unregister_host_app();
            return OF_ReturnType_failed;
        }
    }
    if (pcType==3)  {
        error = DS_write_64(board_index, targAccelId, (*sizeCtrl)[OF_Resp_Accel], (UInt64 *)targAccel);
        if (error != DS_NO_ERROR)  {
            opserr << "ECdSpace::execute() - DS_write_64: error = " << error << endln;
            DS_unregister_host_app();
            return OF_ReturnType_failed;
        }
    }
    updateFlag = 1;
    error = DS_write_64(board_index, updateFlagId, 1, (UInt64 *)&updateFlag);
    if (error != DS_NO_ERROR)  {
        opserr << "ECdSpace::execute() - DS_write_64: error = " << error << endln;
        DS_unregister_host_app();
        return OF_ReturnType_failed;
    }
    
    return OF_ReturnType_completed;
}


int ECdSpace::acquire()
{
    targetFlag = 1;
    while (targetFlag)  {
        // pause for 1msec
        this->sleep(1);
        
        // get target flag
        error = DS_read_64(board_index, targetFlagId, 1, (UInt64 *)&targetFlag);
        if (error != DS_NO_ERROR)  {
            opserr << "ECdSpace::acquire() - DS_read_64: error = " << error << endln;
            DS_unregister_host_app();
            return OF_ReturnType_failed;
        }
    }
    
    // read displacements and resisting forces at target
    error = DS_read_64(board_index, measDispId, (*sizeDaq)[OF_Resp_Disp], (UInt64 *)measDisp);
    if (error != DS_NO_ERROR)	{
        opserr << "ECdSpace::acquire() - DS_read_64: error = " << error << endln;
        DS_unregister_host_app();
        return OF_ReturnType_failed;
    }
    error = DS_read_64(board_index, measForceId, (*sizeDaq)[OF_Resp_Force], (UInt64 *)measForce);
    if (error != DS_NO_ERROR)	{
        opserr << "ECdSpace::acquire() - DS_read_64: error = " << error << endln;
        DS_unregister_host_app();
        return OF_ReturnType_failed;
    }
    
    // reset updateFlag
    updateFlag = 0;
    error = DS_write_64(board_index, updateFlagId, 1, (UInt64 *)&updateFlag);
    if (error != DS_NO_ERROR)  {
        opserr << "ECdSpace::acquire() - DS_write_64: error = " << error << endln;
        DS_unregister_host_app();
        return OF_ReturnType_failed;
    }
    // pause for 1msec
    this->sleep(1);
    
    return OF_ReturnType_completed;
}


void ECdSpace::sleep(const clock_t wait)
{
    clock_t goal;
    goal = wait + clock();
    while (goal>clock());
}
