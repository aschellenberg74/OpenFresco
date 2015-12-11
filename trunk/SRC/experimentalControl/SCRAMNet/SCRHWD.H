#ifndef __SCRPCI__
#define __SCRPCI__

/*
  ********************************************************************
  **
  **       Author:  Steve Johnson
  **
  **       Date:    January 1996
  **
  **     Purpose: PCI control and status register definition file
  **
  **     Modified 16-Feb-96 DEM
  **			  01-Oct-96 KWS (WinNT Mods)
  **              03-Mar-08 DLC (chend MAX)DMA_CHAUNK SIZE to 1MB)
  **
  ********************************************************************
*/

#define HWD_VERSION "PCI-2.02"

#ifdef _WINNT /* Windows NT(tm) prototypes in scrproto.h */
#include <scrwin.h>
#include <scrproto.h>
#endif

#define ACR_MASK     0x1f


#ifndef _WINNT 
/* DLC */
#ifdef DRIVER   
   typedef volatile unsigned long * DWORDPTR;
   typedef volatile unsigned short * WORDPTR;
   typedef volatile unsigned char * BYTEPTR;   
#endif
#define ACR_SHIFT     0

typedef unsigned long int volatile * SCR_LONG_PTR;
typedef unsigned short int volatile * SCR_SHORT_PTR;
typedef unsigned char volatile * SCR_BYTE_PTR;
//DLC 8-5-05 added new data type to use for holding address values
typedef DWORD64 SCR_ADDR_TYPE;

#else /* _WINNT */

#define ACR_SHIFT     0
typedef unsigned long SCR_LONG_DATA;


#endif /* _WINNT */


typedef struct
{
    ULONG offset;
    ULONG bytes;
    LONGLONG user_address;
} TScramnetDMAControl;
typedef TScramnetDMAControl* PScramnetDMAControl; 

/* SCRAMNet control and status register definition file */
struct scr_device
 {
  unsigned short int scr_csr0;
  unsigned short int scr_csr1;
  unsigned short int scr_csr2;
  unsigned short int scr_csr3;
  unsigned short int scr_csr4;
  unsigned short int scr_csr5;
  unsigned short int scr_csr6;
  /* LX fields */
  unsigned short int scr_csr7;
 };

typedef struct _scram_config
  {
   unsigned long int scram_reg_addr;  /* csr register vme physical address */
   unsigned long int scram_mem_addr;  /* starting physical memory address  */
   unsigned long int scram_mem_size;  /* memory size in bytes              */
   unsigned short int scram_mem_vect; /* SCRAMNet vme bus interrupt vector */
   unsigned short int scram_err_vect; /* SCRAMNet vme bus interrupt vector */
   unsigned short int scram_node_id;  /* Node Id for SCRAMNet ring         */
   unsigned short int scram_net_to;   /* Network timeout for the ring      */
   char *scram_mem_dev;               /* Device name to mmap memory.       */
   char *scram_reg_dev;               /* Device name to mmap registers.    */
   char *scram_int_dev;               /* Device name to set interrupts.    */
  } SCRAM_CFG;


/* SCRAMNet installation dependent settings. (For driver and application
   proccesses.) */

#define MAX_PCI_CARDS	10

#define SCR_REG_BASE_DEF 0x2000
#define SCR_MEM_BASE_DEF 0xA0C00000
#define SCR_MEM_LEN_DEF  0x200000
#define SCR_VECTOR_DEF   0x0F
#define SCR_PAGE_LEN_DEF 0x200000
#define SCR_NODE_ID_DEF  0xf2
#define SCR_NET_TO_DEF   0xff
#define SCR_CSR_DEV_DEF  "/dev/eisa/eisa0io"
#define SCR_MEM_DEV_DEF  "/dev/eisa/eisa0mem"
#define SCR_INT_DEV_DEF  "/dev/scr"


#ifndef NO_CONFIG

extern SCRAM_CFG Scramnet_config;

#define SCR_REG_BASE (Scramnet_config.scram_reg_addr)
#define SCR_MEM_BASE (Scramnet_config.scram_mem_addr)
#define SCR_MEM_LEN  (Scramnet_config.scram_mem_size)
#define SCR_VECTOR   (Scramnet_config.scram_mem_vect)
#define SCR_NODE_ID  (Scramnet_config.scram_node_id)
#define SCR_NET_TO   (Scramnet_config.scram_net_to)
#define SCR_CSR_DEV  (Scramnet_config.scram_reg_dev)
#define SCR_MEM_DEV  (Scramnet_config.scram_mem_dev)
#define SCR_INT_DEV  (Scramnet_config.scram_int_dev)

#else

#define SCR_REG_BASE SCR_REG_BASE_DEF /* csr register vme physical address */
#define SCR_MEM_BASE SCR_MEM_BASE_DEF /* starting physical memory address. */
#define SCR_MEM_LEN  SCR_MEM_LEN_DEF  /* memory size in bytes for 128k */
#define SCR_VECTOR   SCR_VECTOR_DEF   /* SCRAMNet vme bus interrupt vector. */
#define SCR_PAGE_LEN SCR_PAGE_LEN_DEF /* For the SCRAMNet library routines */
				  /*  is the shared memory page size. If */
				  /*  the system will not support this size */
				  /*  change the system tunable parameter. */
#define SCR_NODE_ID  SCR_NODE_ID_DEF
#define SCR_NET_TO   SCR_NET_TO_DEF
#define SCR_CSR_DEV  SCR_CSR_DEV_DEF
#define SCR_MEM_DEV  SCR_MEM_DEV_DEF
#define SCR_INT_DEV  SCR_INT_DEV_DEF

#endif

/* Bit defintions for scr_csr0 */

#define CSR0_RXEN  0x0001   /* Rec En (RXEN) */
#define CSR0_TXEN  0x0002   /* Tx En (TXEN) */
#define CSR0_TWO   0x0004   /* Not used on Vme bus */
#define CSR0_HIE   0x0008   /* Host Interrupt Enable (HIE) */
#define CSR0_IME   0x0010   /* Interrupt Map Enable (IME) */
#define CSR0_MME   0x0020   /* Interrupt on memory Mask Match Enable (IMME) */
#define CSR0_ORF   0x0040   /* Override RIE Flag (ORF)(Bit 3 (HIE) must be 1 */
#define CSR0_IOE   0x0080   /* Interrupt on errors (IOE) */
#define CSR0_NIE   0x0100   /* Network Interrupt Enable (NIE) */
#define CSR0_OTF   0x0200   /* Override TIE flag (OTF)(Bit 8 (NIE) must be 1 */
#define CSR0_DDE   0x0400   /* Enable Tx data filter (DDEN) */
#define CSR0_EN4K  0x0800   /* Enable lower 4k bytes for data driven. */
			    /* Valid only when bit 10 (DDEN) is set. (EN4K) */
#define CSR0_RTRF  0x1000   /* Reset Rx Tx Fifo (RTRF) */
#define CSR0_RIF   0x2000   /* Reset Interrupt Fifo (RSTIF) */
#define CSR0_RSMF  0x4000   /* Reset Shared Memory Fifo (RSMF) */
#define CSR0_INSRT 0x8000   /* Insert node, by default the node does not xmit */

/* Bit definitions for scr_csr1 */

#define CSR1_SMFF  0x0001   /* Shared memory fifo full signal, reset on read */
#define CSR1_SMFNE 0x0002   /* Shared memory fifo not empty, 0 = empty */
#define CSR1_SMHF  0x0004   /* Shared memory fifo half full, reset on read */
#define CSR1_RTFF  0x0008   /* Rx Tx fifo full signal, reset on read */
#define CSR1_IFF   0x0010   /* Interrupt fifo full (IFF), reset on read */
#define CSR1_SMFS  0x0020   /* Shared memory fifo skew (SMFS),reset on read */
#define CSR1_CD    0x0040   /* Carrier detect (CD) signal, reset on read */
#define CSR1_BB    0x0080   /* Bad Byte, parity detected, reset on read */
#define CSR1_RXO   0x0100   /* Receiver Overflow (RXO), reset on read */
#define CSR1_RTRY  0x0200   /* Transmit Retry (RTRY), reset on read */
#define CSR1_IARM  0x4000   /* Interrupts Armed (IARM), rearmed on write */
#define CSR1_FOBN  0x8000   /* Fiber Optic Bypass Not Connected */

/* Bit definitions for scr_csr2 */

#define CSR2_BTBS  0x0001   /* Byte transactions byte swap. */
#define CSR2_BTWS  0x0002   /* Byte transactions word swap. */
#define CSR2_WTBS  0x0004   /* Word transactions byte swap. */
#define CSR2_WTWS  0x0008   /* Word transactions word swap. */
#define CSR2_LTBS  0x0010   /* Long word transactions byte swap. */
#define CSR2_LTWS  0x0020   /* Long word transactions word swap. */
#define CSR2_DFOLB 0x0040   /* Enable fiber optic loopback (DFOLB) */
#define CSR2_EWLB  0x0080   /* Enable wire loopback (EWLB) */
#define CSR2_DHMW  0x0100   /* Disable host to shared memory write (NOWRT) */
#define CSR2_WOS   0x0200   /* Write of own slot to shared memory (WOSEN) */
#define CSR2_ISEN  0x0400   /* Enable interrupt on receipt of own interrupt */
			    /* slot (ISEN). */
#define CSR2_ETCS  0x0800   /* Enable transmission of control slot (TxCrtl) */
#define CSR2_HIPRO 0x2000   /* Enable HIPRO transfer mode  */
#define CSR2_BRST  0x8000   /* Set burst mode operation */

/* Bit definitions for scr_csr3 */
/*      Low order byte is the Node number count on the ring.
	It is valid after the first transmission from your node.
	High order byte is the node id number of the current message.  */

/* Bit definitions for scr_csr4 */


/* Bit definitions for scr_csr5 */

/* Bits 0 thru 6 are the high order 7 bits of the interrupt address */

#define CSR5_RB    0x4000  /* Retry bit */
#define CSR5_IFE   0x8000  /* Interrupt fifo empty */

//#define MAX_SCRAM_MEM    0x800000 /* 8 MB Max. SCRAMNet Memory Size */
#define SCR_MAX_DMA_LEN    0x100000 /* 1 MB Max DMA transfer lenght*/

#define SCR_IOCTL_MAX  15      /* Maximum ioctl command value. */

#define SCR_IOCTL_MIN  0      /* Minimum ioctl command value. */

#define SCR_MAX_PROC   0x20  /* Maximum number of processes allowed */
                                     /* to attach to this driver. */

/*
**************************************************************************
**
**    List of ioctl command definitions.
**
**************************************************************************
*/

#define INTSIG        1      /* Notify driver this process wants */
			     /*  signaled when an interrupt occurrs. */

#define SCR_LNK    2      /* This command does a simple link of the */
                              /*  host network node to the network. This */
                              /*  means that the node will both transmit */
                                /*  and receive messages to and from the */
                              /*  network. The agrument to the command */
                              /*  may specify data filtering options. */

#define TX_INT        3      /* This ioctl command sets the host network */
                              /*  up for transmitting network messages */
                              /*  with the interrupt bit set in the */
                               /*  protocol. */
                               /*  There are two methods for determining */
                              /*  whether a transmit message should go out */
                              /*  as an interrupt or not. A flag 'INTALL' */
                              /*  determines which mode you are setting. */
                              /*  If INTALL is passed as the agrument to */
                              /*  the ioctl command then you are put into */
                              /*  a mode where every host write to shared */
                              /*  memory will generate a network interrupt */
                              /*  message. If INTACR is passed as the ioctl *
                              /*  command agrument then the node is set up */
                              /*  to send network interrupt messages when */
                              /*  the host writes to a shared memory */
                              /*  location that has the TIE (transmit */
                              /*  interrupt enable) flag set in the */
                              /*  auxiliary control ram address */
                              /*  corresponding to that shared memory */
                              /*  location. */

#define RX_INT        4      /* This ioctl command sets the host network */
                              /*  node to generate an interrupt to the host *
                              /*  processor upon qualified network writes */
                              /*  to the shared memory. */
                              /*  There are two methods for determining */
                              /*  whether a network write to shared memory */
                              /*  should cause a host interrupt or not. */
                              /*  A flag 'INTALL' */
                              /*  determines which mode you are setting. */
                              /*  If INTALL is passed is the agrument to */
                              /*  the ioctl command then you are put into */
                              /* a mode where every network write to shared *
                              /*  memory will generate a host interrupt. */
                              /*  If INTACR is passed as the ioctl command */
                              /*  agrument then the node is set up to */
                              /*  receive network interrupt messages when */
                              /*  the network writes to a shared memory */
                              /*  location that has the TIE (transmit */
                              /*  interrupt enable) flag set in the */
                              /*  auxcillary control ram address */
                              /*  corresponding to that shared memory */
                              /*  location. */
                              /*  Additionally you may or in the INTOWN */
                              /*  flag to allow the host to be interrupted */
                              /*  on reception of it's own network */
                               /*  messages. */

#define ERR_INT     0x200

#define WRITE_ME_LAST 6 /* Used  in scr_setint_mm for access mode */
                        /* selection by user.   slj042893         */
#define WRITE_DIRECT  7 /* Used  in scr_setint_mm for access mode */
                        /* selection by user.   slj042893         */


#define INT_ERR     5  /* Interrupt on network error. */

#define NTW_WRT_HST 6  /* This ioctl command allows the host network *
                             /*  messages to be written into it's own */
                              /*  shared memory as a network write. This */
                              /*  is in place of normal */
                              /* host writes to the shared memory. */ 

#define DATA_FLT  7    /* This ioctl command allows three data */
                              /*  filtering options determined by the use */
                              /*  of the agruments, FLT_ALL, FLT_OFF, and */
                              /* FLT_HGH. */

#define BYTE_SWP  8    /* This ioctl command will set up the host */
                              /*  for byte swapping based on byte, word, */
                              /*  and longword accesses for conversion to */
                               /*  or from littel endian and big endian */
                              /* integer data formats. */ 

#define ACT_ND         9    /* This ioctl command excepts the agrument */
                              /*  as a three bit indicator of the active */
                              /*  node. */

#define RW_CSR      10    /* This ioctl command let's the power user */
                              /*  read or write any SMARTNet register */
                              /*  for any reason. To do this the agrument */
                              /*  is interpreted by the device driver as */
                              /*  a pointer to a data structure that looks */

#define RST_FIFO 11      /* This ioctl command will reset the three  */
                              /*  SCRAMNet fifos. */

#define AUX_RAM_LNK 12 /* This ioctl command will put the node into */
                              /*  a mode where shared memory accesses reach*/
                              /*  an auxillary control ram instead of */
                              /*  the normal shared memory. The auxillary */
                              /*  ram is used to set the node up to receive *
                              /*  and/or transmit interrupt messages. The */
                              /*  agrument RST_LNK will return the node to */
                              /* normal memory mode. */ 

#define INT_DIAG1    13      /* This ioctl command will initialize the */
                              /*  node for interrupt diagnostics. This */
                              /*  command sets hex code 0x12b in csr0 and */
                              /*  0x100 in csr2. Agrument RST_LNK will */
                              /*  undo the effects of this command. */ 

#define RD_FIFO   14   /* This command will read the fifo and pass */
                              /*  the value back to the calling process. */

#define FLUSHSIG     15      /* Notify driver to flush all registered */
                             /*  interrupt processes */

/* 
** V3 Bit definitions 
*/
/* in PCI_INT_STAT */
#define DMA1_INT           0x2000000
#define DMA0_INT           0x1000000
#define INTB_TO_A          0x2
/* in PCI_INT_CFG */
#define DMA1_ENABLE        0x2000000
#define DMA0_ENABLE        0x1000000
#define MODE_B             0x40000
#define MODE_A             0x20000
/* in DMA_LENGTH0 */
#define DMA_START 		   0x1000000
#define DMA_PCI2LOCAL	   0x10000000

/* DMA definitions */
#define SCRAM_IOCTL        0x900
#define SCRAM_DMA_WRITE    CTL_CODE( FILE_DEVICE_UNKNOWN, SCRAM_IOCTL + 8, METHOD_OUT_DIRECT, FILE_ANY_ACCESS )
#define SCRAM_DMA_READ     CTL_CODE( FILE_DEVICE_UNKNOWN, SCRAM_IOCTL + 9, METHOD_IN_DIRECT, FILE_ANY_ACCESS )

/* Define V3's device and vendor ID's */
#define DEVICEID 0x4750
#define VENDORID 0x11B0

/****************************************************************************/
/*   PCI Functions                                                          */
/****************************************************************************/
#define PCI_FUNCTION_ID                 0xB1
#define FIND_PCI_DEVICE                 0x02
#define FIND_PCI_CLASS_CODE             0x03
#define GENERATE_SPECIAL_CYCLE          0x06
#define READ_CONFIG_BYTE                0x08
#define READ_CONFIG_WORD                0x09
#define READ_CONFIG_DWORD               0x0A
#define WRITE_CONFIG_BYTE               0x0B
#define WRITE_CONFIG_WORD               0x0C
#define WRITE_CONFIG_DWORD              0x0D

/****************************************************************************/
/*   PCI Return Code List                                                   */
/****************************************************************************/
 
#define SUCCESSFUL                      0x00

/****************************************************************************/
/*   V3 Controller internal registers                                       */
/****************************************************************************/
 
#define PCI_VENDOR                      0x00
#define PCI_DEVICE                      0x02
#define PCI_CMD                         0x04
#define PCI_STAT                        0x06
#define PCI_CC_REV                      0x08
#define PCI_HDR_CFG                     0x0C
#define PCI_IO_BASE                     0x10
#define PCI_BASE0                       0x14
#define PCI_BASE1                       0x18
#define PCI_SUBSYS                      0x2C
#define PCI_ROM                         0x30
#define PCI_BPARAM                      0x3C
#define PCI_MAP0                        0x40 
#define PCI_MAP1                        0x44
#define PCI_INT_STAT                    0x48
#define PCI_INT_CFG                     0x4C
#define LB_BASE0                        0x54
#define LB_BASE1                        0x58
#define LB_MAP0                         0x5E
#define LB_MAP1                         0x62
#define LB_IO_BASE                      0x6E
#define LB_ISTAT                        0x76
#define LB_IMASK                        0x77
#define FIFO_CFG                        0x70
#define FIFO_PRIORITY                   0x6E
#define FIFO_STAT                       0x74
#define DMA_PCI_ADDR0                   0x80
#define DMA_PCI_ADDR1                   0x90
#define DMA_LOCAL_ADDR0                 0x84
#define DMA_LOCAL_ADDR1                 0x94
#define DMA_LENGTH0                     0x88
#define DMA_LENGTH1                     0x98
#define DMA_CSR0                        0x8B
#define DMA_CSR1                        0x9B
#define DMA_CTLB_ADDR0                  0x8C
#define DMA_CTLB_ADDR1                  0x9C
#define MAIL_DATA0                      0xC0
#define MAIL_DATA1                      0xC1
#define MAIL_DATA2                      0xC2
#define MAIL_DATA3                      0xC3
#define MAIL_DATA4                      0xC4
#define MAIL_DATA5                      0xC5
#define MAIL_DATA6                      0xC6
#define MAIL_DATA7                      0xC7
#define MAIL_DATA8                      0xC8
#define MAIL_DATA9                      0xC9
#define MAIL_DATA10                     0xCA
#define MAIL_DATA11                     0xCB
#define MAIL_DATA12                     0xCC
#define MAIL_DATA13                     0xCD
#define MAIL_DATA14                     0xCE
#define MAIL_DATA15                     0xCF
#define LB_MAIL_IEWR                    0xD0
#define LB_MAIL_IERD                    0xD2
#define PCI_MAIL_IEWR                   0xD4
#define PCI_MAIL_IERD                   0xD6
#define MAIL_WR_STAT                    0xD8 
#define MAIL_RD_STAT                    0xDA
#define SYSTEM                          0x78

#endif /* __SCRPCI__*/
