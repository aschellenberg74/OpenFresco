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

#if !defined (___nidaqex_h___)
#define ___nidaqex_h___

/*
 * Typedefs, defines, constants, etc.
 */

typedef char           i8;
typedef unsigned char  u8;
typedef short          i16;
typedef unsigned short u16;
typedef long           i32;
typedef unsigned long  u32;
typedef float          f32;
typedef double         f64;

typedef i16 nidaqExRetType;

#include <nidaqerr.h>
#include <stdio.h>

/* internal constants
   - if using CVI, change as necessary for different waveforms...
   - other IDE users, changes here will not affect anything...
 */
#define WFM_MIN_PTS_IN_PERIOD   2



/* error return codes for NIDAQPlotWaveform and NIDAQMakeBuffer */
#ifndef NoError
   #define NoError 0
#endif
/* these error codes are consistent with CVI error codes */
#define NIDAQEX_INVALID_BUFFER         -12
#define NIDAQEX_INVALID_NUMPTS         -14
#define NIDAQEX_INVALID_TYPE           -53

/* C++ wrapper */
#define CPPHEADER    extern "C" {
#define CPPTRAILER   }


/*
 * Function prototypes
 */

//CPPHEADER

nidaqExRetType NIDAQErrorHandler(i16 iStatus, char *strFuncName, i16 iIgnoreWarning);
nidaqExRetType NIDAQWaitForKey(f64 dTimeLimit);

nidaqExRetType NIDAQYield(f64 dTimeLimit);

i16 AI_Configure (
	i16        slot,
	i16        chan,
	i16        inputMode,
	i16        inputRange,
	i16        polarity,
	i16        driveAIS) ;

i16 AI_VRead (
	i16        slot,
	i16        chan,
	i16        gain,
	f64        * volts);

i16 AO_Configure (
	i16        slot,
	i16        chan,
	i16        outputPolarity,
	i16        IntOrExtRef,
	f64        refVoltage,
	i16        updateMode); 

i16 AO_VWrite (
	i16        slot,
	i16        chan,
	f64        voltage) ;

i16 AO_Write (
	i16        slot,
	i16        chan,
	i16        value);

i16 WFM_Check (
	i16        slot,
	i16        channel,
	i16        * progress,
	u32        * itersDone,
	u32        * pointsDone);

i16 WFM_ClockRate (
	i16        slot,
	i16        group,
	i16        whickClock,
	i16        timebase,
	u32        updateInterval,
	i16        mode);

i16 WFM_Group_Control (
	i16        slot,
	i16        group,
	i16        operation);

i16 WFM_Group_Setup (
	i16        slot,
	i16        numChans,
	i16        * chanVect,
	i16        group);

i16 WFM_Load (
	i16        slot,
	i16        numChans,
	i16        * chanVect,
	i16        * buffer,
	u32        cnt,
	u32        iterations,
	i16        mode);

i16 WFM_Rate (
	f64        rate,
	i16        units,
	i16        * timebase,
	u32        * updateInterval);

i16 WFM_Scale (
	i16        slot,
	i16        chan,
	u32        cnt,
	f64        gain,
	f64        * voltArray,
	i16        * binArray);

#endif /* ___nidaqx_h___ */
