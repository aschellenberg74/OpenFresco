/*
 *****************************************************************************
 **
 **    SCRAMNET ioctl command definition file.
 **
 *****************************************************************************
*/

#define CORE_VERSION     "1.64"
#define COPYRIGHT        "Copyright 1997"

struct rw_scr
{
    char  rw_flag;                 /* Read/write SCRAMNet register flag. */
    char  csr_reg;                 /* Csr register number to read or write. */
    unsigned short csr_arg; /* The agrument to be written to a SCRAMNet */
};         /*  register or a buffer to be filled by a SCRAMNet register read. */
 
struct rd_fifo
{
    union
    {
        unsigned int ntr_addr;   /* Interrupt fifo contents; registers 4 & 5. */
        unsigned short int ntr_fifo[2];
    } scr_fifo_ptr;

    char        mt_flag;      /* Interrupt fifo empty flag. */
    char        smff;          /* Shared memory fifo full flag. */
    char        smfne;          /* Shared memory fifo empty flag. */
    char        smfhf;          /* Shared memory fifo half full flag. */
    char        rtff;          /* Receiver transmitter fifo full flag. */
    char        iff;             /* Interrupt fifo full flag. */
    char        smfs;          /* Shared memory fifo skew flag. */
    char        cd;              /* Carrier detect flag. */
    char        bb;              /* Bad byte detect flag. */
    char        rxo;             /* Receiver overflow flag. */
    char        dum1;
    char        dum2;
};

/*
******************************************************************************
**
**    List of ioctl agrument definitions.
**
******************************************************************************
*/

#define REG_RD         1          /* Read SCRAMNet register. */
#define REG_WRT      0          /* Write SCRAMNet register. */
#define REG_MDFY    4          /* Modify SCRAMNet register. */
#define SCR_CSR0    0          /* Identifying a particular csr register. */
#define SCR_CSR1    1          /* Identifying a particular csr register. */
#define SCR_CSR2    2          /* Identifying a particular csr register. */
#define SCR_CSR3    3          /* Identifying a particular csr register. */
#define SCR_CSR4    4          /* Identifying a particular csr register. */
#define SCR_CSR5    5          /* Identifying a particular csr register. */
#define SCR_CSR6    6          /* Identifying a particular csr register. */

#define INTALL        1      /* Agrument supplement to ioctl command */
                                /*  TX_INT and RX_INT. */

#define INTACR        4      /* Agrument supplement to ioctl command */
                              /*  TX_INT and RX_INT. */

#define INTOWN        8      /* Agrument supplement to ioctl command */
                              /*  RX_INT. This flag will allow the host */
                              /*  to be interrupted by reception of it's */
                              /*  interrupt messages returning back off */
                              /*  the network. */

#define FLT_ALL  0xc00 /* Agrument supplement to ioctl commands */
                              /*  DATA_FLT and SCR_LNK. This agrument */
                              /*  causes all shared memory writes to */
                              /* be filtered for changes. */ 

#define FLT_OFF  0     /* Agrument supplement to ioctl commands */
                              /*  DATA_FLT and SCR_LNK. This agrument */
                               /*  causes no shared memory writes to be */
                              /*  filered for changes. This means all */
                              /*  writes to the shared memory result in */
                              /*  network traffic. */

#define FLT_HGH 0x400  /* Agrument supplement to ioctl commands */
                              /*  DATA_FLT and SCR_LNK. This agrument */
                              /*  causes the shared memory addresses */
                              /*  higher than the first 4k bytes to be */
                              /*  data driven. */

#define RST_LNK  16      /* This agrument supplement to the ioctl */
                              /*  command 'SCR_LNK' resets SCRAMNet */
                              /*  removing it from the network. */ 

#define CLR_SIG     1       /* This agrumnet supplements the ioctl */
                              /*  command 'INTSIG'. It will remove the.*/
                              /*  calling process from the signal queue. */ 

#define SET_SIG     0       /* This agrument supplements the ioctl */
                              /*  command 'INTSIG' and complements the */
                               /*  agrument 'CLR_SIG'. It will put the */
                              /*  calling process into the signal queue. */ 

#define RST_INT     16      /* This agrumnet supplements both the ioctl */
                              /*  commands 'TX INT' and 'RX_INT' as well as */
                              /*  'INT_ERR'. It will clear or reset the */
                              /*  appropriate mode of operation, stopping */
                              /*  it. */

#define CLR_INT   16   /*  This provides compatibility with the RST_INT */
                       /*  parameter used in the scr_setint_mm routine. */
                       /*  Documentation lists parameter as CLR_INT instead */
                       /*  of RST_INT.    --slj042893          

         */
/* Changed SET_INT to 32 to fix scr_int_mm().. LAC 9/13/99   */
#define SET_INT  32     /* This arguement supplements the library */
                              /*  routine scr_int_mm(). */

#define RST_BSWP 1       /* This agrument supplements the ioctl */
                               /*  command 'BYTE_SWP'. It will clear all */
                               /*  bits so that no reformatting takes place. */

#define RST_NWH 1      /* This agrument supplements the ioctl */
                              /*  command 'NTW_WRT_HST'. It will stop the */
                               /*  node from being able to write its own */
                              /*  messages from the network into shared */
                              /*  memory. */
/*
******************************************************************************
**
**    Definitions for SCRAMNet library routines.
**
******************************************************************************
*/

#define MAP      0      /* Agrument for the SCRAMNet library modules */
                          /*  indicating that scr_mem_mm or scr_reg_mm */
                          /*  should map the memory. */ 

#define UNMAP 1      /* Agrument for the SCRAMNet library modules */
                           /*  indicating that scr_mem_mm or scr_reg_mm */
                           /*  should unmap the memory. */ 

#define ACR   0x10  /* Agrument to be bit mapped into csr0 */
                           /*  turning auxcillary control ram on. */ 

#define MEM      0     /* Agrument turning regular memory on. */ 

#define RESET 1     /* Agrument to scr mem_mm or scr_reg_mm */
                           /*  indicating whether to link or unlink */
                          /*  to memory. */

#define TX_ENB 2      /* Agrument for the SCRAMNet library routine */
                           /* scr_lnk_mm. Enable the nodes transmitter. */

#define RX_ENB 1    /* Agrument for the SCRAMNet library routine */
                          /*  scr_lnk_mm. Enable the nodes receiver. */

#define AUTO_VECTOR     0     /* This is the value of SCR_VECTOR if the */
                              /* system assignes the vector to the device */

struct scr_csr
{
 unsigned short int csr_regs[16];    /* SCRAMNet Control/Status Registers */
};

/* scr_nwh_mm() has changed to scr_wml_mm() */
#define scr_nwh_mm     scr_wml_mm

#ifndef _WINNT /* Windows NT(tm) prototypes in scrproto.h */
#ifndef NO_PROTO

void scr_acr_mm( int );
void scr_bswp_mm( void );
/*
void scr_error_mm( FILE*, unsigned short );
*/
void scr_fifo_mm( int, struct rd_fifo* );
void scr_dfltr_mm( int );
int scr_fswin_mm( void );
void scr_id_mm( unsigned char*, unsigned char* );
int scr_read_int_fifo( void* );
void scr_int_mm( int, int );
void scr_lnk_mm( int );
int scr_load_mm( char*, int );
void scr_mclr_mm( int );
int scr_probe_mm( void*, unsigned short );
void scr_reset_mm( void );
void scr_rw_mm( struct rw_scr* );
int scr_save_mm( char*, int );
void scr_smem_mm( int, unsigned long );
void scr_wml_mm( int );


#else /* NO_PROTO */

void scr_acr_mm();
void scr_bswp_mm();
void scr_error_mm();
void scr_fifo_mm();
void scr_dfltr_mm();
int scr_fswin_mm();
void scr_id_mm();
int scr_read_int_fifo();
void scr_int_mm();
void scr_lnk_mm();
int scr_load_mm();
void scr_mclr_mm();
int scr_probe_mm();
void scr_reset_mm();
void scr_rw_mm();
int scr_save_mm();
void scr_smem_mm();
void scr_wml_mm();

#endif /* NO_PROTO */
#endif /* _WINNT */
