/************************************************************************/
/*                                                                      */
/*  File   : SCRPLUS.H                                                  */
/*  Date   : Nov. 18, 1993                                              */
/*  Author : D.J. Sisolak                                               */
/*  Purpose: This header file includes definitions of the CSR bits only */
/*           found on the SCRAMNet+ boards. It also declares types and  */
/*           macros used in the SCRAMNet+ library.                      */
/*                                                                      */
/************************************************************************/
#define _WINNT 1

#ifndef __SCRPLUS__
#define __SCRPLUS__

#ifndef __SCR__
#include <scr.h>
#endif

#ifndef __SCRHWD__
#include <scrhwd.h>
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifdef MSC7
#define SIGUSR1      SIGILL
#endif

#define SCR_CSR7   7          /* Identifying a particular csr register. */
#define SCR_CSR8   8          /* Identifying a particular csr register. */
#define SCR_CSR9   9          /* Identifying a particular csr register. */
#define SCR_CSR10  10         /* Identifying a particular csr register. */
#define SCR_CSR11  11         /* Identifying a particular csr register. */
#define SCR_CSR12  12         /* Identifying a particular csr register. */
#define SCR_CSR13  13         /* Identifying a particular csr register. */
#define SCR_CSR14  14         /* Identifying a particular csr register. */
#define SCR_CSR15  15         /* Identifying a particular csr register. */
#define SCR_CSR16  16         /* Identifying a particular csr register. */

/************************************************************************/
/* Identify the bits of the SCRAMNet+ CSRs which are not the same as    */
/* the old SCRAMNet Classic.                                            */
/************************************************************************/
#define CSR1_TXFF  0x0001   /* Transmit fifo full signal, reset on read */
#define CSR1_TXFNE 0x0002   /* Transmit fifo not empty, 0 = empty */
#define CSR1_TXF78 0x0004   /* Transmit fifo 7/8 full, reset on read */
#define CSR1_INTFF 0x0010   /* Interrupt fifo full (IFF), reset on read */
#define CSR1_PV    0x0020   /* Protocol violation error detected. */
#define CSR1_BM    0x0080   /* Bad Message, parity detected, reset on read */
#define CSR1_RXO   0x0100   /* Receiver Overflow (RXO), reset on read */
#define CSR1_TXRTY 0x0200   /* Transmit Retry (RTRY), reset on read */
#define CSR1_TXRTO 0x0400   /* Transmit Retry on timeout, reset on read */
#define CSR1_GPCO  0x1000   /* General Purpose Counter overflow, read resets.*/
#define CSR1_IARM  0x4000   /* Interrupts Armed (IARM), rearmed on write */
#define CSR1_FOBN  0x8000   /* Fiber Optic Bypass Not Connected */

#define CSR2_LLIM       0x0800
#define CSR2_VLEN       0x1000
#define CSR2_MMSG       0x4000

#define CSR8_IDMX       0x0001
#define CSR8_DHLD       0x0002
#define CSR8_CSEL       0x0004
#define CSR8_EXCS       0x0008
#define CSR8_MWDO       0x0010
#define CSR8_EPRE       0x0020
#define CSR8_MCLK       0x0040
#define CSR8_EDIN       0x0080
#define CSR8_ARST       0x0100
#define CSR8_FREE       0x0200
#define CSR8_IOVR       0x0400
#define CSR8_MESW       0x0800
#define CSR8_MSIZMSK    0x7000
#define CSR8_PHNT       0x8000

#define CSR9_TXFF       0x0001
#define CSR9_TXFNE      0x0002
#define CSR9_TXF78      0x0004
#define CSR9_BIST       0x0008
#define CSR9_INTFF      0x0010
#define CSR9_PV         0x0020
#define CSR9_CD         0x0040
#define CSR9_BM         0x0080
#define CSR9_RXO        0x0100
#define CSR9_TXRTY      0x0200
#define CSR9_TXRTO      0x0400
#define CSR9_RTRF       0x0800
#define CSR9_IUCO       0x1000
#define CSR9_TRGC       0x2000
#define CSR9_ERRC       0x4000
#define CSR9_FOBN       0x8000

#define CSR10_SMAEN     0x0001

#define CSR12_VPEN      0x0001

#define ACR_RIE        0x01
#define ACR_TIE        0x02
#define ACR_TRIG1    0x04
#define ACR_TRIG2    0x08
#define ACR_HIPRO    0x10
#define ACR_TRIG3    0x20

/************************************************************************/
/* Define macros used for library functions to be used as parameters.   */
/************************************************************************/
#define VP_SET          1
#define VP_RESET        2
#define VP_READ         3

#define CNTR_ERRORS     1
#define CNTR_TRIGS      2
#define CNTR_TRANSIT    3
#define CNTR_NET_EVENTS 4
#define CNTR_FREERUN    5
#define CNTR_FREERUN_T2 6

#define GOLD            1
#define BURST           2
#define PLATINUM        3
#define GOLD_PLUS       4
#define BURST_PLUS      5
#define PLATINUM_PLUS   6

/************************************************************************/
/* Define types and structures used by the new SCRAMNet+ libraries and  */
/* hardware confidence tests.                                           */
/************************************************************************/

#define TAGE_MASK    0x00ff
#define TXRX_MASK    0xff00

#define MAX_ID        0xff
#define MAX_AGE        0Xff

#ifndef _WINNT /* Windows NT(tm) prototypes in scrproto.h */

#ifndef NO_PROTO

int sp_cfg_read( int );
int sp_plus_find( void );
int sp_scram_init( int );
int sp_msg_life( unsigned );
int sp_set_cntr( int );
int sp_txrx_id( unsigned char );
int sp_bist_rd( int* );
unsigned long sp_mem_size( void );
int sp_net_to( unsigned short );
int sp_protocol( unsigned );
int sp_rx_id( unsigned );
int sp_set_vp( int, unsigned );
int sp_set_sm_addr( unsigned long );
void sp_config_file( char* );
int is_boardline( char* );

#else /* NO_PROTO */

int sp_cfg_read();
int sp_plus_find();
int sp_scram_init();
int sp_msg_life();
int sp_set_cntr();
int sp_txrx_id();
int sp_bist_rd();
unsigned long sp_mem_size();
int sp_net_to();
int sp_protocol();
int sp_rx_id();
int sp_set_vp();
int sp_set_sm_addr();
void sp_config_file();
int is_boardline();

#endif /* NO_PROTO */

#endif /* _WINNT */

#endif /* __SCRPLUS__ */
