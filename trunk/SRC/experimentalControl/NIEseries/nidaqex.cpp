/* ****************************************************************** **
**              Experimental Test Subsystem for OpenSees              **
**                                                                    **
**                                                                    **
** (C) Copyright 2003, Yoshikazu Takahashi, Kyoto University, Japan   **
** All Rights Reserved.                                               **
**                                                                    **
** Developed by:                                                      **
**   Yoshikazu Takahashi (yos@catfish.kuciv.kyoto-u.ac.jp)            **
** Date: 2003/10                                                      **
**                                                                    **
** ------------------------------------------------------------------ **
**    OpenSees is Open System for Earthquake Engineering Simulation   **
**  of the project at Pacific Earthquake Engineering Research Center. **
**                                                                    **
**  Developed by:                                                     **
**    Frank McKenna (fmckenna@ce.berkeley.edu)                        **
**    Gregory L. Fenves (fenves@ce.berkeley.edu)                      **
**    Filip C. Filippou (filippou@ce.berkeley.edu)                    **
**                                                                    **
** ****************************************************************** */
                                                                       
// Written: yos
// Created: 11/03
//
// Purpose: This file is dummy include file for NI-DAQ for MS Windows,
//          and is used for debugging ECNIEseris on Linux system.
//          The information of the original NIDAQEx.h is as follows
/************************************************************************
 * TITLE:       NIDAQEx.h
 *              Header for supporting code module for NI-DAQ Examples
 *
 * DESCR:       This header file is to be used with any NI-DAQ example
 *              program that uses the supporting code module, NIDAQEx.c(pp).
 *
 * Copyright (c) 1997, National Instruments Corp.  All rights reserved.
 ************************************************************************/

#include "nidaqex.h"
#include <nidaqerr.h>
#include <stdio.h>

nidaqExRetType NIDAQErrorHandler(i16 iStatus, char *strFuncName, i16 iIgnoreWarning)
{
    char strMsg[255] = {0},
         strTitle[20] = {0};

    if (iStatus != NoError)
    {
       sprintf(strTitle, "NI-DAQ %s", (iStatus > 0)? "warning":"error");
       sprintf(strMsg,   "[%s] returned %s %d.\n\n",
                             strFuncName, strTitle, iStatus);
       if (((iStatus > 0) && !iIgnoreWarning) ||
           (iStatus < 0))
       {
	 //          return MessagePopup(strTitle, strMsg);
       }
    }
    return (NoError);
}


nidaqExRetType NIDAQWaitForKey(f64 dTimeLimit) 
{ return (NoError); }

nidaqExRetType NIDAQYield(f64 dTimeLimit)
{ return (NoError); }

i16 AI_Configure (
	i16        slot,
	i16        chan,
	i16        inputMode,
	i16        inputRange,
	i16        polarity,
	i16        driveAIS)
{ return 0; }

i16 AI_VRead (
	i16        slot,
	i16        chan,
	i16        gain,
	f64        * volts)
{ return 0; }

i16 AO_Configure (
	i16        slot,
	i16        chan,
	i16        outputPolarity,
	i16        IntOrExtRef,
	f64        refVoltage,
	i16        updateMode)
{ return 0; } 

i16 AO_VWrite (
	i16        slot,
	i16        chan,
	f64        voltage) 
{ return 0; }

i16 AO_Write (
	i16        slot,
	i16        chan,
	i16        value)
{ return 0; }

i16 WFM_Check (
	i16        slot,
	i16        channel,
	i16        * progress,
	u32        * itersDone,
	u32        * pointsDone)
{ *progress = 1; return 0; }

i16 WFM_ClockRate (
	i16        slot,
	i16        group,
	i16        whickClock,
	i16        timebase,
	u32        updateInterval,
	i16        mode)
{ return 0; }

i16 WFM_Group_Control (
	i16        slot,
	i16        group,
	i16        operation)
{ return 0; }

i16 WFM_Group_Setup (
	i16        slot,
	i16        numChans,
	i16        * chanVect,
	i16        group)
{ return 0; }

i16 WFM_Load (
	i16        slot,
	i16        numChans,
	i16        * chanVect,
	i16        * buffer,
	u32        cnt,
	u32        iterations,
	i16        mode)
{ return 0; }

i16 WFM_Rate (
	f64        rate,
	i16        units,
	i16        * timebase,
	u32        * updateInterval)
{ return 0; }

i16 WFM_Scale (
	i16        slot,
	i16        chan,
	u32        cnt,
	f64        gain,
	f64        * voltArray,
	i16        * binArray)
{ return 0; }
