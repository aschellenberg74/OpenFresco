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
    /*       COPYRIGHT (c) 1989-1996 BY                                           */
    /*       SYSTRAN CORPORATION                                                  */
    /*       4126 LINDEN AVE.                                                     */
    /*       DAYTON, OHIO 45432-3066                                              */
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
    /*       COMMITMENT BY SYSTRAN CORPORATION.                                   */
    /*                                                                            */
    /*       SYSTRAN ASSUMES NO RESPONSIBILITY FOR THE USE OR                     */
    /*       RELIABILITY OF ITS SOFTWARE ON EQUIPMENT WHICH IS NOT                */
    /*       SUPPLIED BY SYSTRAN.                                                 */
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
    **********************************************************************/
    
    #if !defined(__NTPCISCR)
    #define __NTPCISCR
    
  	#define DRIVER_NAME				"NtPciScram"
  	#define DRIVER_VERSION			"2.07b"


//DLC 
#if DBG

#define Debug_KdPrint(_x_)	DbgPrint("%s: ",DRIVER_NAME);	\
										 DbgPrint _x_ ;
#define DEBUG_LOG_PATH(_x_) DbgPrint("%s: ",DRIVER_NAME);	\
										 DbgPrint _x_ ;
#define DEBUG_TRAP()		DbgBreakPoint()
	

#else

#define Debug_KdPrint(_x_)

#define DEBUG_LOG_PATH(_x_)

#define DEBUG_TRAP()		DbgBreakPoint()

#endif
    // note this must be maintained in sync with regedit.cpp....
    
    // #endif
    
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
    	#define IoCtlGetBaseMemPtr \
    		CTL_CODE( IoCtlType, 0x932, METHOD_BUFFERED, FILE_READ_ACCESS )
    	#define IoCtlWritePciConfigByte \
    		CTL_CODE( IoCtlType, 0x933, METHOD_BUFFERED, FILE_READ_ACCESS )
    	#define IoCtlReadPciConfigLong \
    		CTL_CODE( IoCtlType, 0x934, METHOD_BUFFERED, FILE_READ_ACCESS )
    	#define IoCtlMapUserPhysicalMemory \
    		CTL_CODE( IoCtlType, 0x935, METHOD_BUFFERED, FILE_READ_ACCESS )
    	#define IoCtlUnmapUserPhysicalMemory \
    		CTL_CODE( IoCtlType, 0x936, METHOD_BUFFERED, FILE_READ_ACCESS )
    	#define IoCtlMapInterrupt \
    		CTL_CODE( IoCtlType, 0x937, METHOD_BUFFERED, FILE_READ_ACCESS )
    	#define IoCtlUnmapInterrupt \
    		CTL_CODE( IoCtlType, 0x938, METHOD_BUFFERED, FILE_READ_ACCESS )
    	#define IoCtlCountCards \
    		CTL_CODE( IoCtlType, 0x939, METHOD_BUFFERED, FILE_READ_ACCESS )
    
  	#define IoCtlWritePciConfigShort \
  		CTL_CODE( IoCtlType, 0x940, METHOD_BUFFERED, FILE_READ_ACCESS )
  	#define IoCtlWritePciConfigLong \
  		CTL_CODE( IoCtlType, 0x941, METHOD_BUFFERED, FILE_READ_ACCESS )
    	#define PCI_IO_BASE		0x10
    	#define PCI_BASE_0		0x14
    	#define PCI_BASE_1		0x18
    	#define PCI_MAP0		0x40
    	#define PCI_MAP1		0x44
        #define PCI_INT_CFG		0x4C
    	#define FIFO_CFG		0x70
    	#define V3_DEVICE_ID	0x5300
    	#define V3_VENDOR_ID	0x1387
        #define OLD_DEVICE_ID   0x4750
        #define OLD_VENDOR_ID   0x11b0
    
    	#define IO_BASE			0
    	#define APERTURE_0		1
    	#define APERTURE_1		2
    	#define CSR				3
    
    	#define CSR_SIZE		0x1000
		#define PCI_CONFIG_SIZE 0x100 //CJF
    	#define IO_BASE_ADDR	0xff000000
    	#define BASE0_ADDR		0xfd000000
    	#define BASE1_ADDR		0xfe000000
    	#define CSR_ADDR		BASE0_ADDR + 0x800000
    
    	typedef struct _MEM_MAP {
    		ULONG MemAddr;		// was PhysMemAddr
    		ULONG MemSize;		//		PhysMemSize
    		} MEM_MAP, *PMEM_MAP;
    
    	typedef struct _PCI_ID {
    		USHORT					Vendor;
    		USHORT					Device;
    		ULONG						Bus;
    		ULONG						Slot;
    		} PCI_ID, *PPCI_ID;
    
    	typedef struct _CONF_PACKET {
    		ULONG						Bus;
    		ULONG						Slot;
    		ULONG						Offset;
    		UCHAR						Data;
 		USHORT						SData;
		ULONG						LData;
    		} CONF_PACKET, *PCONF_PACKET;
    
    	typedef struct _WRITE_CHAR {
    		UCHAR						Size;
    		UCHAR						Area;
    		ULONG						Offset;
    		UCHAR						Data;
    		} WRITE_CHAR, *PWRITE_CHAR;
    
    	typedef struct _WRITE_SHORT {
    		UCHAR						Size;
    		UCHAR						Area;
    		ULONG						Offset;
    		USHORT					Data;
    		} WRITE_SHORT, *PWRITE_SHORT;
    
    	typedef struct _WRITE_LONG {
    		UCHAR						Size;
    		UCHAR						Area;
    		ULONG						Offset;
    		ULONG						Data;
    		} WRITE_LONG, *PWRITE_LONG;
    
    	typedef struct _READ_DATA {
    		UCHAR						Size;
    		UCHAR						Area;
    		ULONG						Offset;
    		} READ_DATA, *PREAD_DATA;
    #endif
    
