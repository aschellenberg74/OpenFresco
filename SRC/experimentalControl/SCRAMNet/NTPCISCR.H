/******************************************************************************/
/*                                                                            */
/*   SSSSSSS  CCCCCCC  RRRRRR      A     M     M  N     N                     */
/*   S        C        R     R    A A    MM   MM  NN    N   eeeee      t      */
/*   S        C        R     R   A   A   M M M M  N N   N  e     e  ttttttt   */
/*   SSSSSSS  C        RRRRRR   AAAAAAA  M  M  M  N  N  N  eeeeeee     t      */
/*         S  C        R     R  A     A  M     M  N   N N  e           t      */
/*         S  C        R     R  A     A  M     M  N    NN  e     e     t  t   */
/*   SSSSSSS  CCCCCCC  R     R  A     A  M     M  N     N   eeeee      ttt    */
/*                                                                            */
/******************************************************************************/
/*       COPYRIGHT (c) 1989-2007 BY                                           */
/*       CURTISS WRIGHT CONTROLS INC.                                         */
/*       DAYTON, OHIO                                                         */
/*       ALL RIGHTS RESERVED.                                                 */
/*                                                                            */
/*       YOU ARE LEGALLY ACCOUNTABLE FOR ANY VIOLATION OF THE                 */
/*       LICENSE AGREEMENT OR APPLICABLE COPYRIGHT, TRADEMARK,                */
/*       OR TRADE SECRET LAWS.                                                */
/*	 							                                              */
/*       THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY BE                */
/*       USED AND COPIED ONLY IN ACCORDANCE WITH THE TERMS OF                 */
/*       SUCH LICENSE AND WITH THE INCLUSION OF THE ABOVE                     */
/*       COPYRIGHT NOTICE. THIS SOFTWARE OR ANY OTHER COPIES                  */
/*       THEREOF MAY NOT BE PROVIDED OR OTHERWISE MADE                        */
/*       AVAILABLE TO ANY OTHER PERSON. NO TITLE TO AND                       */
/*       OWNERSHIP OF THE SOFTWARE IS HEREBY TRANSFERRED.                     */
/*                                                                            */
/*       THE INFORMATION IN THIS SOFTWARE IS SUBJECT TO CHANGE                */
/*       WITHOUT NOTICE AND SHOULD NOT BE CONSTRUED AS A                      */
/*       COMMITMENT BY CURTISS WRIGHT CONTROLS INC.                           */
/*                                                                            */
/*       CURTISS WRIGHT ASSUMES NO RESPONSIBILITY FOR THE USE OR              */
/*       RELIABILITY OF ITS SOFTWARE ON EQUIPMENT WHICH IS NOT                */
/*       SUPPLIED BY CURTISS WRIGHT CONTROLS INC.                             */
/*                                                                            */
/*       ANY QUESTIONS REGARDING THIS SOFTWARE SHOULD BE                      */
/*       DIRECTED TO:   SCRAMNET CUSTOMER SERVICE DEPARTMENT                  */
/*                      SOFTWARE DIVISION                                     */
/*                      937-252-5601                                          */
/*                                                                            */
/******************************************************************************/
    
/*********************************************************************
*                                                                    *
* File   : ntscrdll.h                                                *
* Author : Steve Thompson                                            *
* Date   : Apr 12, 1996                                              *
* Purpose: Provide general-purpose routines for mapping SCRAMNet to  *
*          linear memory from within Windows (32-bit)                *
*                                                                    *
* Modified: Sep 9, 1996												 *
* Author  : Kenn Scribner											 *
* Purpose : Insert additional IOCTL calls.							 *
*                                                                    *
* Revised : Oct 18, 2005 DLC                                         *
*                                                                    *
* Revised : Mar 11, 2008 DLC  Added New ScrDebug Flag definitions    *
*
**********************************************************************/
    
#if !defined(__NTPCISCR)
    
#define __NTPCISCR
    
#define DRIVER_NAME				"NtPciScram"
#define DRIVER_VERSION			"2.11"

/* 
   Macros used to use for casting pointers to 64-bit ints
   and 64-bit ints to pointers (without compiler warnings).  
   example:
       uint64 val = 0x12345678;
       uint32 *ptr = UINT64_TO_PTR(uint32, val);
   Use with caution. 
*/
typedef unsigned __int64    uintpsize;   
//typedef DWORD64    uintpsize;   
#define PTR_TO_UINT64(ptr)              ((DWORD64)((uintpsize)ptr))
#define UINT64_TO_PTR(ptr_type, val64)  ((ptr_type*)((uintpsize)val64))

// DLC convert exPagePool calls to exPagePoolWithTag calls
// using the foloing contant as the tag
#define SCR_POOL_TAG 'MRCS'

// DLC 8-9-05 Added new scrDbgPrint and control codes to
// allow more selective display of debugging information

#define SCR_DBG_ERROR        0x00000100
#define SCR_DBG_WARN         0x00000200
#define SCR_DBG_PNP          0x00000400
#define SCR_DBG_MAP          0x00000800
// -----------------------------------------
#define SCR_DBG_CLOSE        0x00001000
#define SCR_DBG_MISC         0x00002000
#define SCR_DBG_IOCTL        0x00004000
#define SCR_DBG_IOCTL_DISP   0x00008000
// -----------------------------------------
#define SCR_DBG_PCI_WRITE    0x00010000
#define SCR_DBG_PCI_READ     0x00020000
#define SCR_DBG_DEF_IRP_COMP 0x00040000
#define SCR_DBG_FLOW         0x00080000
// -----------------------------------------
#define SCR_DBG_INIT         0x00100000
#define SCR_DBG_UTIL         0x00200000
#define SCR_DBG_CNTR         0x00400000
#define SCR_DBG_DEV_CTRL     0x00800000
// -----------------------------------------
#define SCR_DBG_IRQ_SETUP    0x01000000
#define SCR_DBG_IRQ_DPC      0x02000000
#define SCR_DBG_IRQ_ISR      0x04000000

// -----------------------------------------
#define SCR_DBG_DMA_SETUP    0x10000000
#define SCR_DGB_DMA_DPC      0x20000000
#define SCR_DBG_DMA_FUNCT    0x40000000
#define SCR_DGB_DMA_ADPT_CTL 0x80000000

// -----------------------------------------
#define SCR_DBG_DEFAULT  SCR_DBG_ERROR        | SCR_DBG_WARN       | \
	                     SCR_DBG_PNP          | SCR_DBG_MAP        | \
						 SCR_DBG_CLOSE        | SCR_DBG_MISC       | \
					     SCR_DBG_IOCTL        | SCR_DBG_IOCTL_DISP | \
						 SCR_DBG_PCI_WRITE    | SCR_DBG_PCI_READ   | \
						 SCR_DBG_DEF_IRP_COMP | SCR_DBG_FLOW       | \
                         SCR_DBG_INIT         | SCR_DBG_UTIL       | \
						 SCR_DBG_CNTR         | SCR_DBG_DEV_CTRL   | \
                         SCR_DBG_IRQ_SETUP    | /* SCR_DBG_IRQ_DPC | */ \
						 /*SCR_DBG_IRQ_ISR    |*/  \
                         SCR_DBG_DMA_SETUP    | /* SCR_DGB_DMA_DPC |*/ \
						 SCR_DBG_DMA_FUNCT    | SCR_DGB_DMA_ADPT_CTL ;
			  

   // note this must be maintained in sync with regedit.cpp....
      
#define IoCtlType			40000
    
// The IOCTL function codes from 0x800 to 0xFFF are for customer use.
#define IoCtlFindCard \
    		CTL_CODE( IoCtlType, 0x925, METHOD_BUFFERED, FILE_READ_ACCESS )
#define IoCtlReadSCRAMNet \
    		CTL_CODE( IoCtlType, 0x927, METHOD_BUFFERED, FILE_READ_ACCESS )
#define IoCtlWriteSCRAMNet \
    		CTL_CODE( IoCtlType, 0x928, METHOD_BUFFERED, FILE_READ_ACCESS )
#define IoCtlGetApp0MemPtr \
    		CTL_CODE( IoCtlType, 0x929, METHOD_BUFFERED, FILE_READ_ACCESS )
#define IoCtlGetApp1MemPtr \
    		CTL_CODE( IoCtlType, 0x930, METHOD_BUFFERED, FILE_READ_ACCESS )
#define IoCtlGetCSRMemPtr \
    		CTL_CODE( IoCtlType, 0x931, METHOD_BUFFERED, FILE_READ_ACCESS )
//#define IoCtlGetBaseMemPtr \
//    		CTL_CODE( IoCtlType, 0x932, METHOD_BUFFERED, FILE_READ_ACCESS )
#define IoCtlWritePciConfigByte \
    		CTL_CODE( IoCtlType, 0x933, METHOD_BUFFERED, FILE_READ_ACCESS )
#define IoCtlReadPciConfigLong \
    		CTL_CODE( IoCtlType, 0x934, METHOD_BUFFERED, FILE_READ_ACCESS )
#define IoCtlMapUserPhysicalMemory \
    		CTL_CODE( IoCtlType, 0x935, METHOD_BUFFERED, FILE_READ_ACCESS )
#define IoCtlUnmapUserPhysicalMemory \
    		CTL_CODE( IoCtlType, 0x936, METHOD_BUFFERED, FILE_READ_ACCESS )
/*
#define IoCtlMapInterrupt \
    		CTL_CODE( IoCtlType, 0x937, METHOD_BUFFERED, FILE_READ_ACCESS )
#define IoCtlUnmapInterrupt \
    		CTL_CODE( IoCtlType, 0x938, METHOD_BUFFERED, FILE_READ_ACCESS )
*/
#define IoCtlMapInterrupt \
    		CTL_CODE( IoCtlType, 0x937, METHOD_BUFFERED, FILE_ANY_ACCESS )
#define IoCtlUnmapInterrupt \
    		CTL_CODE( IoCtlType, 0x938, METHOD_BUFFERED, FILE_ANY_ACCESS )

#define IoCtlCountCards \
    		CTL_CODE( IoCtlType, 0x939, METHOD_BUFFERED, FILE_READ_ACCESS )
    
#define IoCtlWritePciConfigShort \
  		    CTL_CODE( IoCtlType, 0x940, METHOD_BUFFERED, FILE_READ_ACCESS )
#define IoCtlWritePciConfigLong \
    		 CTL_CODE( IoCtlType, 0x941, METHOD_BUFFERED, FILE_READ_ACCESS )

// DLC 10-16-05 added new code to flush disconnect any register interrupt info.
#define IoCtlFlushInterruptHandler \
    		 CTL_CODE( IoCtlType, 0x942, METHOD_BUFFERED, FILE_READ_ACCESS )
//DLC 8-10-05 added new code to alter scrdebug mode flag paramater controling
// what type of debug message to display

#define IoCtlSetDebugLevel \
    		 CTL_CODE( IoCtlType, 0x943, METHOD_BUFFERED, FILE_READ_ACCESS )
#define IoCtlGetDebugLevel \
    		 CTL_CODE( IoCtlType, 0x944, METHOD_BUFFERED, FILE_READ_ACCESS )

   
#define PCI_IO_BASE		0x10
#define PCI_BASE_0		0x14
#define PCI_BASE_1		0x18
#define PCI_MAP0		   0x40
#define PCI_MAP1		   0x44
#define PCI_INT_CFG		0x4C
#define FIFO_CFG		   0x70
#define V3_DEVICE_ID	   0x5300
#define V3_VENDOR_ID	   0x1387
#define OLD_DEVICE_ID   0x4750
#define OLD_VENDOR_ID   0x11b0
    
#define IO_BASE			0
#define APERTURE_0		1
#define APERTURE_1		2
#define CSR				   3
    
#define CSR_SIZE		   0x1000
#define PCI_CONFIG_SIZE 0x100 //CJF
#define IO_BASE_ADDR	   0xff000000
#define BASE0_ADDR		0xfd000000
#define BASE1_ADDR		0xfe000000
#define CSR_ADDR		   BASE0_ADDR + 0x800000
    
/* DLC 8-5-05 renamed data structure memorys to support 64-bit pointers */
/*
typedef struct _MEM_MAP 
{
  	ULONG MemAddr;		// was PhysMemAddr
	ULONG MemSize;		//		PhysMemSize
} MEM_MAP, *PMEM_MAP;
*/
typedef struct _MEM_MAP 
{
 	LARGE_INTEGER MemPhysAddr;  // the Physical PCI bus address of the card
    //PUINT32          MemPhysAddrPtr;  // the Physical PCI bus address of the card
	PVOID            MemVoidPtr;   // A virtual pointer to the PCI card memory
	ULONG64          MemVirtAddr;
	ULONG            MemSize;		// The size of the memory block.
} MEM_MAP, *PMEM_MAP;
    
typedef struct _PCI_ID 
{
  	USHORT					Vendor;
  	USHORT					Device;
  	ULONG						Bus;
  	ULONG						Slot;
} PCI_ID, *PPCI_ID;
    
typedef struct _CONF_PACKET 
{
  	ULONG						Bus;
  	ULONG						Slot;
  	ULONG						Offset;
  	UCHAR						Data;
	USHORT					SData;
   ULONG						LData;
} CONF_PACKET, *PCONF_PACKET;
    
typedef struct _WRITE_CHAR 
{
  	UCHAR						Size;
  	UCHAR						Area;
  	ULONG						Offset;
  	UCHAR						Data;
} WRITE_CHAR, *PWRITE_CHAR;
    
typedef struct _WRITE_SHORT 
{
  	UCHAR						Size;
  	UCHAR						Area;
  	ULONG						Offset;
  	USHORT					Data;
} WRITE_SHORT, *PWRITE_SHORT;
    
typedef struct _WRITE_LONG 
{
  	UCHAR						Size;
  	UCHAR						Area;
  	ULONG						Offset;
  	ULONG						Data;
} WRITE_LONG, *PWRITE_LONG;
    
typedef struct _READ_DATA 
{
  	UCHAR						Size;
  	UCHAR						Area;
  	ULONG						Offset;
} READ_DATA, *PREAD_DATA;


typedef struct InterruptData
{
	HANDLE  *hInterrupt;
	DWORD64 intHandle;
	int	    pid;
	int     nBrdNum;
} INT_DATA, PINT_DATA;

#define EVENT_NAME_UNICODE_STR_SIZE 80


#endif
    
