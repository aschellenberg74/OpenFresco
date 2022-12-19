/* ****************************************************************** **
**    OpenFRESCO - Open Framework                                     **
**                 for Experimental Setup and Control                 **
**                                                                    **
**                                                                    **
** Copyright (c) 2006, Yoshikazu Takahashi, Kyoto University          **
** All rights reserved.                                               **
**                                                                    **
** Licensed under the modified BSD License (the "License");           **
** you may not use this file except in compliance with the License.   **
** You may obtain a copy of the License in main directory.            **
** Unless required by applicable law or agreed to in writing,         **
** software distributed under the License is distributed on an        **
** "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,       **
** either express or implied. See the License for the specific        **
** language governing permissions and limitations under the License.  **
**                                                                    **
** Developed by:                                                      **
**   Yoshikazu Takahashi (yos@catfish.dpri.kyoto-u.ac.jp)             **
**   Andreas Schellenberg (andreas.schellenberg@gmx.net)              **
**   Gregory L. Fenves (fenves@berkeley.edu)                          **
**                                                                    **
** ****************************************************************** */

#ifndef FrescoGlobals_h
#define FrescoGlobals_h

// Written: Yoshi (yos@catfish.dpri.kyoto-u.ac.jp)
// Created: 09/06
// Revision: A
//
// Description: This file contains the global values in OpenFresco

#define OPF_VERSION "2.7.2 (Build 221218)"

#ifndef _WIN64
#define WIN_ARCH "32-Bit"
#else
#define WIN_ARCH "64-Bit"
#endif

//#ifndef _bool_h
//#include "bool.h"
//#endif

// Return Type
static const int OF_ReturnType_completed =  0;
static const int OF_ReturnType_failed    = -1;

// tentative return type
static const int OF_ReturnType_ready     = 2;
static const int OF_ReturnType_accepted  = 3;
static const int OF_ReturnType_received  = 4;
static const int OF_ReturnType_started   = 5;

// Response Type
static const int OF_Resp_Disp  = 0;
static const int OF_Resp_Vel   = 1;
static const int OF_Resp_Accel = 2;
static const int OF_Resp_Force = 3;
static const int OF_Resp_Time  = 4;
static const int OF_Resp_All   = 5; 

// Remote Test
static const int OF_RemoteTest_open             = 1;
static const int OF_RemoteTest_setup            = 2;
static const int OF_RemoteTest_setTrialResponse = 3;
static const int OF_RemoteTest_execute          = 4;
static const int OF_RemoteTest_commitState      = 5;
static const int OF_RemoteTest_getDaqResponse   = 6;
static const int OF_RemoteTest_getDisp          = 7;
static const int OF_RemoteTest_getVel           = 8;
static const int OF_RemoteTest_getAccel         = 9;
static const int OF_RemoteTest_getForce         = 10;
static const int OF_RemoteTest_getTime          = 11;
static const int OF_RemoteTest_getInitialStiff  = 12;
static const int OF_RemoteTest_getTangentStiff  = 13;
static const int OF_RemoteTest_getDamp          = 14;
static const int OF_RemoteTest_getMass          = 15;
static const int OF_RemoteTest_shutdown         = 98;
static const int OF_RemoteTest_DIE              = 99;

// tentative remote test
static const int OF_RemoteTest_acquire          = 50;

// Network Transmition Data Size
static const int OF_Network_dataSize = 256; //536; 

#endif
