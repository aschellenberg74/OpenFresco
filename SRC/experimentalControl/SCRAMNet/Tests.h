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
* File    : windiags.h                                               *
*                                                                    *
* Date    : Sep 9, 1996												 *
* Author  : Kenn Scribner											 *
* Purpose : Header file for windiags.c.								 *
**********************************************************************/
#if !defined (__WINDIAGS)
#define __WINDIAGS

	#include <stdio.h> // FILE

	#define PROP_DELAY1		100
	
	/* Window color (blue-ish color...looks really good on 24-bit screens). */
	#define WINDOW_COLOR	RGB(50,240,255)

	/* Window class strings. */
	#define CANCELCLASS		"CancelWndClass"

	/* Splash bitmap parms. */
	#define BITMAPWIDTH		320
	#define BITMAPHEIGHT	240

	/* Default point size for output text. */
	#define DEFAULTPOINTSIZE		9

	/* "SCRAMnet" strings. */
	#define SCRAM1			"\n\n\n"
	#define SCRAM2			"        ===== ===== ====    =   =   = =   =  ---    -     +\n"
	#define SCRAM3			"        =     =     =   =  = =  == == ==  = -   - -----   +\n"
	#define SCRAM4			"        ===== =     ====  =   = = = = = = = -----   -   +++++\n"
	#define SCRAM5			"            = =     =   = ===== =   = =  == -       -     +\n"
	#define SCRAM6			"        ===== ===== =   = =   = =   = =   =  ----   --    +\n"
	#define SCRAM7			"\n                  Windows NT SCRAMNet+ Diagnostics         \n"

	/* Number of tests. */
	#define NUM_MEMTESTS	11
	#define NUM_LOOPTESTS	14
	#define NUM_NETTESTS	7

	/* Test IDs (array members, actually). */
	#define IDT_LONG1		0
	#define IDT_LONG0		1
	#define IDT_SHORT1		2
	#define IDT_SHORT0		3
	#define IDT_BYTE1		4
	#define IDT_BYTE0		5
	#define IDT_ALTBIT		6
	#define IDT_ACR1		7
	#define IDT_ACR0		8
	#define IDT_ACRBIT		9
	#define IDT_ADDR		10

	#define IDT_FILTER		0
	#define IDT_SMFIFO		1
	#define IDT_INTFIFO		2
	#define IDT_INT			3
	#define IDT_BYTESW		4
	#define IDT_TXRX		5
	#define IDT_MEMLOOP		6
	#define IDT_HIPRO		7
	#define IDT_VIRTPG		8
	#define IDT_RXID		9
	#define IDT_RXINT		10
	#define IDT_GPCOUNT		11
	#define IDT_ERRMASK		12
	#define IDT_LMODE		13

	#define IDT_NODECOMM	0
	#define IDT_GOLD		1
	#define IDT_BURST		2
	#define IDT_COMPCOMM	3
	#define IDT_PLUS		4
	#define IDT_OURNODE		5
	#define IDT_ALLNODE		6

	#define IDA_MEMTEST		0
	#define IDA_LOOPTEST	1
	#define IDA_NETTEST		2
	#define IDA_MAXERR		3
	#define IDA_PROCPRI		4
	#define IDA_SCRNSAV		5
	#define IDA_INTNUM		6
	#define IDA_MEMSIZE		7

	/* Bitmap info. */
	#define LED_WIDTH		60
	#define LED_OFF			0
	#define GLED_ON			1
	#define RLED_ON			2

	/* Control IDs. */
	#define IDC_EDITSTAT	900
	#define IDC_EDITOUTP	910
	#define IDC_STATUS		930
	#define IDC_PROGBAR		940
	#define IDC_CANCEL		950
	#define IDC_EDITTMP		999

	/* Registry keys. */
	#define MEM_KEY			"SOFTWARE\\Systran\\SCRAMNet\\WinDiags\\MemTest"
	#define LOOP_KEY		"SOFTWARE\\Systran\\SCRAMNet\\WinDiags\\LoopTest"
	#define NET_KEY			"SOFTWARE\\Systran\\SCRAMNet\\WinDiags\\NetTest"
	#define OPT_KEY			"SOFTWARE\\Systran\\SCRAMNet\\WinDiags"

	/* Hct.h defines. */
	#define SWITCH_FIBER_LOOPBACK   1
	#define WIRE_LOOPBACK           2
	#define MAN_FIBER_LOOPBACK      3
	#define MAC_LOOPBACK            4

	/* Object structure for interrupt testing. */
	typedef struct tagSignals {
		HANDLE hEvent;
		HANDLE hISR;
	} SIGNALS, *PSIGNALS;

#endif

/* Prototypes (windiags.c) */
BOOL InitApplication( HINSTANCE );
BOOL InitInstance( HINSTANCE, int );
void InitSplash( HWND );
void KillSplash( HWND );
void InitText( HWND );
LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK AboutProc( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK MemProc( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK LoopProc( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK NetProc( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK OptsProc( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK RunProc( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK SetProc( HWND, UINT, WPARAM, LPARAM );
void CancelProc( HWND * );
LRESULT CALLBACK CancelWndProc( HWND, UINT, WPARAM, LPARAM );
LRESULT ReadRegistry( int );
LRESULT WriteRegistry( int );

/* filefnc.c */
void FileInitialize( HWND );
BOOL FileOpenDlg( HWND, PSTR, PSTR );
BOOL FileSaveDlg( HWND, PSTR, PSTR );
static long FileLength( FILE * );
BOOL FileRead( HWND, PSTR );
BOOL FileWrite( HWND, PSTR );

/* printfnc.c */
BOOL PrintInitialize( void );
BOOL PrinterSetup( HWND );
BOOL CALLBACK PrintDlgProc( HWND, UINT, UINT, LONG );
BOOL CALLBACK PrintAbortProc( HDC, short );
BOOL PrintFile( HANDLE, HWND, HWND );

/* patch.c */
long RunTests( void );
BOOL DoLWR1( void );
BOOL DoLWR0( void );
BOOL DoWR1( void );
BOOL DoWR0( void );
BOOL DoBR1( void );
BOOL DoBR0( void );
BOOL DoMptn( void );
BOOL DoAR1( void );
BOOL DoAR0( void );
BOOL DoAptn( void );
BOOL DoAddr( void );
BOOL DoDFltr( void );
BOOL DoSMF( void );
BOOL DoIntF( void );
BOOL DoInt( void );
BOOL DoBSwp( void );
BOOL DoTxRx( void );
BOOL DoMlpb( void );
BOOL DoHPDF( void );
BOOL DoVPge( void );
BOOL DoRID( void );
BOOL DoRxOr( void );
BOOL DoGPC( void );
BOOL DoEMask( void );
BOOL DoMNode( void );
BOOL DoGoldP( void );
BOOL DoBurst( void );
BOOL DoHCT( void );
BOOL DoMPlus( void );
void tsttick( unsigned long );
void mem_test_display( unsigned long, int );
void disp_catgry_title( char * );
void InsertLine( char *, int );
void BumpStatusBar( int );

/* Test files (memory). */
void scr_lmem_r1s( FILE *, unsigned long, unsigned long ); // scr_lr1s.c
void scr_lmem_r0s( FILE *, unsigned long, unsigned long ); // scr_lr0s.c
void scr_wmem_r1s( FILE *, unsigned long, unsigned long ); // scr_wr1s.c
void scr_wmem_r0s( FILE *, unsigned long, unsigned long ); // scr_wr0s.c
void scr_bmem_r1s( FILE *, unsigned long, unsigned long ); // scr_br1s.c
void scr_bmem_r0s( FILE *, unsigned long, unsigned long ); // scr_br0s.c
void scr_mem_ptn( FILE *, unsigned long, unsigned long ); // scr_mptn.c
void scr_acr_r1s( FILE *, unsigned long, unsigned long ); // scr_ar1s.c
void scr_acr_r0s( FILE *, unsigned long, unsigned long ); // scr_ar0s.c
void scr_acr_ptn( FILE *, unsigned long, unsigned long ); // scr_aptn.c
void scr_addr_dd( FILE *, unsigned long, unsigned long ); // scr_addr.c

/* Test files (loopback). */
int sp_mec_lp(); // sp_maclp.c
void scr_dfltr_dd( FILE *, unsigned long, int, unsigned long ); // scr_dftr.c
void scr_smf_dd( FILE *, unsigned long, int, unsigned long ); // scr_smf.c
void Scram_Int( void * ); // int_hndl.c
int Equal_if( unsigned long *, long, int ); // scr_idat.c
void scr_int_dat( unsigned long int *, unsigned long int, unsigned long int, unsigned long int *); // scr_idat.c
int scr_rndm_dat( unsigned long int *, unsigned long int, unsigned long int *); // scr_idat.c
void scr_if_dd( FILE *, unsigned long, int, unsigned long ); // scr_if.c
void scr_int_dd( FILE *, unsigned long, int, unsigned long ); // scr_int.c
void scr_bswp_dd( FILE *, unsigned long, int, unsigned long ); // scr_bswp.c
static void delay( void ); // sp_rxtx.c
static void set_test_case( unsigned indx ); // sp_rxtx.c
static unsigned int_fifo_count( void ); // sp_rxtx.c
void check_test_case( FILE *, unsigned, unsigned short, unsigned long * ); // sp_rxtx.c
void sp_rxtx_dd( FILE *, unsigned long, int, unsigned long ); // sp_rxtx.c
void scr_mlpb_dd( FILE *, unsigned long, int, unsigned long ); // scr_mlpb.c
void scr_hpdf_dd( FILE *, unsigned long, int, unsigned long ); // scr_hpdf.c
void sp_vpge_dd( FILE *, unsigned long, int, unsigned long ); // sp_vpge.c
void scr_txrx_dd( FILE *, unsigned long, int, unsigned long ); // scr_txrx.c
void sp_rxor_dd( FILE *, unsigned long, int, unsigned long ); // sp_rxor.c
static void set_gpc_test( int ); // sp_gpctr.c
static void do_gpc_test( FILE *, int ); // sp_gpctr.c
static void check_gpc_test( FILE *, int, unsigned long* ); // sp_gpctr.c
void sp_gpcounter_dd( FILE *, unsigned long, int, unsigned long ); // sp_gpctr.c	 
void set_errmask_test( int, int ); // sp_ermsk.c
void do_errmask_test( FILE*, int, unsigned long*, int ); // sp_ermsk.c
void sp_errmask_dd( FILE*, unsigned long, int, unsigned long ); // sp_ermsk.c

/* Test files (network). */
void sleep( int ); // sleep.c
void mnd_init( int, unsigned long int *, unsigned long int * ); // scr_mnd.c
int mnd_sync( int, int, unsigned long int *, unsigned int ); // scr_mnd.c
void scr_mnd_dd( FILE*, unsigned long, unsigned long, unsigned long, unsigned long ); // scr_mnd.c
void sp_gld_pls( FILE*, unsigned long, unsigned long, unsigned long, unsigned long ); // sp_gld_p.c
void scr_bst_dd( FILE*, unsigned long, unsigned long, unsigned long, unsigned long ); // scr_bst.c
void scr_mnd_old( FILE*, unsigned long, unsigned long, unsigned long, unsigned long ); // scr_mndo.c
static void init_test( int ); // sp_netcm.c
static void synchronize( int, int ); // sp_netcm.c
static void check_pattern( FILE*, unsigned long ); // sp_netcm.c
void sp_netcom_dd( FILE*, unsigned long, unsigned long, unsigned long, unsigned long ); // sp_netcm.c


// Globals required to get the core tests working
int brdNum;
int nCancel;
int sp_plus_flag;
unsigned scr_memory_mapped;
unsigned scr_registers_mapped;



