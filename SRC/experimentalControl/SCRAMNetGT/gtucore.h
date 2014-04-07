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

/*
 * @file gtucore.h
 * @brief    core portions available to user level API
 */

#ifndef __GT_UCORE_H__
#define __GT_UCORE_H__

#include "systypes.h"

#define FILE_REV_GTUCORE_H  "6"     /* 08/30/2011 */

/******************************************************************/
/************************ ERROR CODES *****************************/
/******************************************************************/

#define SCGT_SUCCESS                   0
#define SCGT_SYSTEM_ERROR              1
#define SCGT_BAD_PARAMETER             2
#define SCGT_DRIVER_ERROR              3
#define SCGT_TIMEOUT                   4
#define SCGT_CALL_UNSUPPORTED          5
#define SCGT_INSUFFICIENT_RESOURCES    6
#define SCGT_LINK_ERROR                7
#define SCGT_MISSED_INTERRUPTS         8
#define SCGT_DRIVER_MISSED_INTERRUPTS  9
#define SCGT_DMA_UNSUPPORTED          10
#define SCGT_HARDWARE_ERROR           11

/******************************************************************/
/*********************** STATE IDs ********************************/
/******************************************************************/

#define SCGT_NODE_ID                0
#define SCGT_ACTIVE_LINK            1
#define SCGT_NUM_LINK_ERRS          2
#define SCGT_EWRAP                  3
#define SCGT_WRITE_ME_LAST          4
#define SCGT_UNICAST_INT_MASK       5
#define SCGT_BROADCAST_INT_MASK     6
#define SCGT_INT_SELF_ENABLE        7
#define SCGT_RING_SIZE              8
#define SCGT_UPSTREAM_NODE_ID       9
#define SCGT_NET_TIMER_VAL         10
#define SCGT_LATENCY_TIMER_VAL     11
#define SCGT_SM_TRAFFIC_CNT        12
#define SCGT_SPY_SM_TRAFFIC_CNT    13
#define SCGT_SPY_NODE_ID           14
#define SCGT_TRANSMIT_ENABLE       15
#define SCGT_RECEIVE_ENABLE        16
#define SCGT_RETRANSMIT_ENABLE     17
#define SCGT_LASER_0_ENABLE        18
#define SCGT_LASER_1_ENABLE        19
#define SCGT_LINK_UP               20
#define SCGT_LASER_0_SIGNAL_DET    21
#define SCGT_LASER_1_SIGNAL_DET    22
#define SCGT_D64_ENABLE            23
#define SCGT_BYTE_SWAP_ENABLE      24
#define SCGT_WORD_SWAP_ENABLE      25
#define SCGT_LINK_ERR_INT_ENABLE   26
#define SCGT_READ_BYPASS_ENABLE    27


/*****************************************************************/
/********************* INTERRUPT TYPES ***************************/
/*****************************************************************/

#define SCGT_UNICAST_INTR     0
#define SCGT_BROADCAST_INTR   1
#define SCGT_ERROR_INTR       2
#define SCGT_NO_INTR          3


/*****************************************************************/
/********************* READ/WRITE FLAGS **************************/
/*****************************************************************/

#define SCGT_RW_PIO               0x1
#define SCGT_RW_DMA_BYTE_SWAP     0x2
#define SCGT_RW_DMA_WORD_SWAP     0x4
#define SCGT_RW_DMA_PHYS_ADDR     0x8
#define SCGR_RW_PIO_8_BIT         0x10
#define SCGR_RW_PIO_16_BIT        0x20


/**********************************************************/
/****************** CORE DATA TYPES ***********************/
/**********************************************************/

/**
 * device info struct
 */

typedef struct _scgtDeviceInfo
{
    uint32 reserved;
    char driverRevisionStr[128];  /**< string containing driver revision info */
    char boardLocationStr[128];   /**< string describing board location */
    uint32 unitNum;               /**< unit number */
    uint32 popMemSize;            /**< populated memory size in bytes */
    uint32 mappedMemSize;         /**< mapped memory size in bytes */
    uint32 numLinks;              /**< number of supported links */
    uint32 revisionID;            /**< firmware revision ID */
} scgtDeviceInfo;


/**
 * interrupt struct
 */
 
typedef struct _scgtInterrupt
{
    uint32 type;          /**< SCGT_UNICAST_INTR, SCGT_BROADCAST_INTR, or 
                             SCGT_ERROR_INTR */
    uint32 sourceNodeID;  /**< source node ID */
    uint32 id;            /**< 0 to 31 designating the interrupt number for 
                             broadcast interrupts, destination node ID for 
                             sending unicast interrupts. */ 
    uint32 val;           /**< user defined data sent with broadcast or unicast 
                             interrupt or error code if error interrupt */
    uint32 seqNum;        /**< Sequence number */
} scgtInterrupt;



#endif /* __GT_UCORE_H__ */
