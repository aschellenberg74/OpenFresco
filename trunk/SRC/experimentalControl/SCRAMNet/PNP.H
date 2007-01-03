/*****************************************************************************
*       PNP.H -  INCLUDE FILE
*****************************************************************************/
#ifndef _PNP_
#define _PNP_   
#include <windows.h>

#define MAX_MEM_REGISTERS       9
#define MAX_IO_PORTS            20
#define MAX_IRQS                7
#define MAX_DMA_CHANNELS        7

/*********************************************************************/

struct Device_ID
{
  DWORD dBusID;         /* Bus type 0 undefined */
  DWORD dDevID;         /* Physical device ID, 0xFFFFFFFF is undefined */
  DWORD dSerialNum;     /* Serial/Instance number, 0 is undefined */
  DWORD dLogicalID;     /* Logical device ID(PnP), Class code(PCI) */
  DWORD dFlags;
};
  

typedef struct PnPAccess
    {
      BYTE bCSN;
      BYTE bLogicalDevNumber;
      WORD wReadDataPort;
    } PnPAccessType;


typedef struct 
{
  struct Device_ID sDeviceId;
  WORD  wNumMemWindows;
  DWORD dMemBase[MAX_MEM_REGISTERS];
  DWORD dMemLength[MAX_MEM_REGISTERS];
  WORD  wMemAttrib[MAX_MEM_REGISTERS];
  WORD  wNumIOPorts;
  WORD  wIOPort_Base[MAX_IO_PORTS];
  WORD  wIOPort_Length[MAX_IO_PORTS];
  WORD  wNumIRQs;
  BYTE  bIRQRegisters[MAX_IRQS];
  BYTE  bIRQAttrib[MAX_IRQS];
  WORD  wNumDMAs;
  BYTE  bDMALst[MAX_DMA_CHANNELS];
  WORD  wDMAAttrib[MAX_DMA_CHANNELS];
  BYTE  bReserved1[3];
} Config_Info;
      

/* --------------------------------------------------------------------------- */

extern unsigned int PNP_ADDR;        /* set in pnp_init.c */
extern unsigned int PNP_WR_DATA;

#define DEFAULT_PNP_ADDR                0x279
#define DEFAULT_PNP_WR_DATA             0xA79
#define DEFAULT_READ_DATA_PORT          0x20b

/* THESE ARE VERSION 1.0 ADDRESS PORT VALUES */
#define PNP_RD_DATA_PORT                0x00
#define PNP_SERIAL_ISOL                 0x01
#define PNP_CONFIG_CONTROL              0x02
#define PNP_WAKE                        0x03
#define PNP_RESOURCE_DATA               0x04
#define PNP_STATUS                      0x05
#define PNP_CARD_SELECT_NUMBER          0x06
#define PNP_LOGICAL_DEVICE_NUMBER       0x07
/* CARD LEVEL RESERVED 0x08..0x1F */

/* CARD LEVEL VENDOR DEFINED 0x20..0x2F */

/* non standard regigters */
#define PNP_PIO_DATA                    0x20
#define PNP_PIO_DATA_DIRECTION          0x21

#define PRM_WR_EN_PORT                  0x22
#define LOW_DATA_PORT                   0x23
#define HIGH_DATA_PORT                  0x24
#define STATUS_PORT                     0x25
#define PRM_COMMAND                     0x26

/* return to standard registers list */
#define PNP_ACTIVATE                    0x30
#define PNP_IO_RANGE_CHECK              0x31
/* LOGICAL DEVICE CONTROL RESERVED 0x32..0x37 */
/* LOGICAL DEVICE CONTROL VENDOR DEFINED 0x38..0x3F */

#define PNP_MEM_BASE_HIGH_DESC_0        0x40
#define PNP_MEM_BASE_LOW_DESC_0         0x41
#define PNP_MEM_CONTROL_DESC_0          0x42
#define PNP_MEM_ULIMIT_HIGH_DESC_0      0x43
#define PNP_MEM_ULIMIT_LOW_DESC_0       0x44
/* FILLER 0x45..0x47 */
#define PNP_MEM_BASE_HIGH_DESC_1        0x48
#define PNP_MEM_BASE_LOW_DESC_1         0x49
#define PNP_MEM_CONTROL_DESC_1          0x4A
#define PNP_MEM_ULIMIT_HIGH_DESC_1      0x4B
#define PNP_MEM_ULIMIT_LOW_DESC_1       0x4C
/* FILLER 0x4D..0x4F */
#define PNP_MEM_BASE_HIGH_DESC_2        0x50
#define PNP_MEM_BASE_LOW_DESC_2         0x51
#define PNP_MEM_CONTROL_DESC_2          0x52
#define PNP_MEM_ULIMIT_HIGH_DESC_2      0x53
#define PNP_MEM_ULIMIT_LOW_DESC_2       0x54
/* FILLER 0x55..0x57 */
#define PNP_MEM_BASE_HIGH_DESC_3        0x58
#define PNP_MEM_BASE_LOW_DESC_3         0x59
#define PNP_MEM_CONTROL_DESC_3          0x5A
#define PNP_MEM_ULIMIT_HIGH_DESC_3      0x5B
#define PNP_MEM_ULIMIT_LOW_DESC_3       0x5C
/* FILLER 0x5D..0x5 F */

#define PNP_IO_BASE_HIGH_DESC_0         0x60
#define PNP_IO_BASE_LOW_DESC_0          0x61
#define PNP_IO_BASE_HIGH_DESC_1         0x62
#define PNP_IO_BASE_LOW_DESC_1          0x63
#define PNP_IO_BASE_HIGH_DESC_2         0x64
#define PNP_IO_BASE_LOW_DESC_2          0x65
#define PNP_IO_BASE_HIGH_DESC_3         0x66
#define PNP_IO_BASE_LOW_DESC_3          0x67
#define PNP_IO_BASE_HIGH_DESC_4         0x68
#define PNP_IO_BASE_LOW_DESC_4          0x69
#define PNP_IO_BASE_HIGH_DESC_5         0x6A
#define PNP_IO_BASE_LOW_DESC_5          0x6B
#define PNP_IO_BASE_HIGH_DESC_6         0x6C
#define PNP_IO_BASE_LOW_DESC_6          0x6D
#define PNP_IO_BASE_HIGH_DESC_7         0x6E
#define PNP_IO_BASE_LOW_DESC_7          0x6F

#define PNP_INTERRUPT_LEVEL_0           0x70
#define PNP_INTERRUPT_TYPE_0            0X71
#define PNP_INTERRUPT_LEVEL_1           0x72
#define PNP_INTERRUPT_TYPE_1            0x73

#define PNP_DMA_CHANNEL_0               0x74
#define PNP_DMA_CHANNEL_1               0x75
/* LOGICAL DEVICE CONFIG RESERVED 0x76..0xEF */
/* LOGICAL DEVICE VEWNDOR DEFINED 0xF0..0xFE */
#define PNP_RESERVED                    0xFF

/* PnP resuorce types */
#define SML_PNP_VERSION                 0x01
#define SML_LOGICAL_DEVICE_ID           0x02
#define SML_COMP_DEVICE_ID              0x03
#define SML_IRQ_FORMAT                  0x04
#define SML_DMA_FORMAT                  0x05
#define SML_STRT_DEP_FUNCTION           0x06
#define SML_END_DEP_FUNCTION            0x07
#define SML_IO_DESCRIPTOR               0x08
#define SML_FIXED_IO                    0x09
#define SML_VENDOR_DEFINED              0x0E
#define SML_END_TAG                     0x0F
/*      SML_RESERVED           0x0A-0x0E */
#define LRG_MEM_RANGE                   0x01
#define LRG_ID_STRING                   0x02
#define LRG_ID_UNICODE                  0x03
#define LRG_VENDOR_DEFINED              0x04
/* #define LRG_RESERVED         0x05-0x7F */

#define SCRAMNET_ID                     0x4c720301l 
#define REV_SCRAMNET_ID                 0x0103724cl  

/* dialog switch bit masks */
#define SIMM_MASK       0x01
#define VLEN_MASK       0x02
#define MEM_CTRL_MASK   0x04
#define EE_RD_MASK      0x200
#define EE_WR_MASK      0x100
#define WRITE_PNP_MASK  0x400

#define MASK_4K         0xfff0
#define MASK_8K         0xffe0
#define MASK_16K        0xffc0
#define MASK_32K        0xff80
#define MASK_64K        0xff00
#define MASK_128K       0xfe00
/* 256K                 0xfc00 */
#define MASK_512K       0xf800
#define MASK_1M         0xF000
#define MASK_2M         0xE000
#define MASK_4M         0xC000
#define MASK_8M         0x8000

#define MIN_LOWER_MEM_BASE      0xd0000
#define MAX_LOWER_MEM_BASE      0xeffff
#define MIN_UPPER_MEM_BASE      0x100000
#define MAX_UPPER_MEM_BASE      0xFFFFFF
#define MIN_IO_BASE             0x200
#define MAX_IO_BASE             0x380
#define VALID_IRQS_MASK         0xDEF8

#endif /*_PNP_*/

/*****************END OF FILE *******************************************/


