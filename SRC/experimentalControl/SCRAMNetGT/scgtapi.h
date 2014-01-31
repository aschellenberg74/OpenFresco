/******************************************************************************/
/*                              SCRAMNet GT                                   */
/******************************************************************************/
/*                                                                            */
/* Copyright (c) 2002-2005 Curtiss-Wright Controls.                           */
/*               support@systran.com 800-252-5601 (U.S. only) 937-252-5601    */
/*                                                                            */
/* This library is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU Lesser General Public                 */
/* License as published by the Free Software Foundation; either               */
/* version 2.1 of the License, or (at your option) any later version.         */
/*                                                                            */
/* See the GNU Lesser General Public License for more details.                */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/*    Module      : scgtapi.h                                                 */
/*                                                                            */
/*    Description : This file defines the API function call                   */
/*                  definitions and data structures for                       */
/*                  interfacing to the SCRAMNet GT driver.                    */
/*                                                                            */
/*    API Rev     : 1.0                                                       */
/*                                                                            */
/******************************************************************************/

#ifndef __SCGT_API_H__
#define __SCGT_API_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "systypes.h"
#include "gtucore.h"

/**************************************************************************/
/************************** CONSTANTS *************************************/
/**************************************************************************/
extern const char * scgtapiRevisionStr; /* the compiled api revision */

/* FILE_REV_SCGTAPI_H is not the api revision. See scgtapiRevisionStr */
#define FILE_REV_SCGTAPI_H   "2"     /* 2007/9/14 */

/**************************************************************************/
/**************************  D A T A  T Y P E S  **************************/
/**************************************************************************/

/*
 * Define HANDLE based on platform.
 * All systems use integer file handles except for Windows.
 */

#ifdef PLATFORM_WIN

/* HANDLE is declared in <windows.h> */
#include <windows.h>

typedef HANDLE scgtOSHandle;

#elif defined PLATFORM_VISA
typedef scgtViData * scgtOSHandle;

#elif defined PLATFORM_UNIX || defined PLATFORM_VXWORKS || defined PLATFORM_RTX

typedef int scgtOSHandle;

#else

#error "Must define one of PLATFORM_WIN, PLATFORM_UNIX, PLATFORM_VXWORKS, PLATFORM_RTX or PLATFORM_VISA"

#endif

typedef struct _scgtHandle
{
    scgtOSHandle osHandle;
    uint32 unitNum;
    uint32 *memMapPtr;    /* ptr to GT memory */
    uint32 memMapSize;    /* size in bytes of the map */
} scgtHandle;

typedef uint32 scgtIntrHandle;


#ifndef DLLEXPORT
#define DLLEXPORT
#endif


/**************************************************************************/
/******************  F U N C T I O N  P R O T O T Y P E S  ****************/
/**************************************************************************/

/*  Consult the SCGT API Guide for more details. */


/*
 * scgtOpen()
 */
 
DLLEXPORT uint32 scgtOpen(uint32 unitNum, scgtHandle *pHandle);

/*
 * scgtClose()
 */

DLLEXPORT uint32 scgtClose(scgtHandle *pHandle);

/*
 * scgtWrite()
 */
 
DLLEXPORT uint32 scgtWrite(scgtHandle *pHandle,
                 uint32 gtMemoryOffset,
                 void   *pDataBuffer,
                 uint32 bytesToTransfer,
                 uint32 flags,
                 uint32 *pBytesTransferred,
                 scgtInterrupt *pInterrupt);
 

/*
 * scgtRead()
 */
                   
DLLEXPORT uint32 scgtRead(scgtHandle *pHandle,
                uint32 gtMemoryOffset,
                void   *pDataBuffer,
                uint32 bytesToTransfer,
                uint32 flags,
                uint32 *pBytesTransferred); 

/*
 * scgtMemMap()
 */
 
DLLEXPORT void *scgtMapMem(scgtHandle *pHandle);

/*
 * scgtMemUnmap()
 */
 
DLLEXPORT void scgtUnmapMem(scgtHandle *pHandle);

/*
 * scgtGetInterrupt()
 */
 
DLLEXPORT uint32 scgtGetInterrupt(scgtHandle *pHandle,
                        scgtIntrHandle *intrHandle,
                        scgtInterrupt *interruptBuffer,
                        uint32 numInterrupts,
                        uint32 timeout,
                        uint32 *numInterruptsRet);

/*
 * scgtGetDeviceInfo()
 */
                      
DLLEXPORT uint32 scgtGetDeviceInfo(scgtHandle *pHandle,
                         scgtDeviceInfo *pDeviceInfo);

/*
 * scgtGetState()
 */
 
DLLEXPORT uint32 scgtGetState(scgtHandle *pHandle,
                    uint32 stateID);
                      
/*
 * scgtSetState()
 */
 
DLLEXPORT uint32 scgtSetState(scgtHandle *pHandle,
                    uint32 stateID,
                    uint32 val);

/*
 * scgtReadCR()
 */
                         
DLLEXPORT uint32 scgtReadCR(scgtHandle *pHandle, uint32 offset);

/*
 * scgtWriteCR()
 */
                     
DLLEXPORT uint32 scgtWriteCR(scgtHandle *pHandle, uint32 offset, uint32 val);

/*
 * scgtReadNMR()
 *     read net-management register
 */
 
DLLEXPORT uint32 scgtReadNMR(scgtHandle *pHandle, uint32 offset);

/*
 * scgtWriteNMR()
 *     write net-management register
 */
 
DLLEXPORT uint32 scgtWriteNMR(scgtHandle *pHandle,
                    uint32 offset,
                    uint32 val);


/*
 * scgtGetErrStr()
 */
                      
DLLEXPORT char *scgtGetErrStr(uint32 retcode);

/*
 * scgtGetStats()
 */
 
DLLEXPORT uint32 scgtGetStats(scgtHandle *pHandle,
                    uint32 *statsArray,
                    char *statNames,
                    uint32 firstStatIndex,
                    uint32 numStats);

/*
 * scgtGetApiRevStr
 */
DLLEXPORT const char * scgtGetApiRevStr( void );

#ifdef __cplusplus
}
#endif

#endif /* __SCGT_API_H__ */
