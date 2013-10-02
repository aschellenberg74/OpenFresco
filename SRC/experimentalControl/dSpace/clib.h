/* clib.h ********************************************************************
*                                                                            *
* Header file for Host-DSP Interface Library                                 *
*                                                                            *
* basic utilities to control DSP board functions and to                      *
* access DSP board memory                                                    *
*                                                                            *
* for PC-bus and network version                                             *
*                                                                            *
* Vs 1.0   29-Nov-1994                                                       *
* Vs 1.1   17-Jan-1996  Windows NT support added                             *
* Vs 1.2   17-Apr-1996  DS1004 support                                       *
* Vs 1.3   23-May-1996  new error code DSP_DATA_NOT_AVAILABLE                *
* Vs 1.4   25-Nov-1996  new error code DSP_SVC_NET_ERROR                     *
* Vs 1.41  08-Apr-1997  DS1201 and DS230x support / memory check size DS100x *
* Vs 1.42  07-Sep-1997  DS1103 support                                       *
* Vs 1.43  17-Feb-1998  new error code DSP_ILLEGAL_MEMORY_SIZE               *
* Vs 1.49  11-Mar-1998  DS1401 support (partial DS813 support)               *
* Vs 1.50  02-Oct-1998  new error code DSP_PROCESSOR_NOT_RESET               *
* Vs 1.51  12-Feb-1999  new error code DSP_ACCESS_TIMEOUT                    *
* Vs 1.52  26-Feb-1999  inclusion of dstypes with "" instead of <>           *
* Vs 1.53  05-May-1999  new error code DSP_PROTOCOL_ERROR                    *
* Vs 1.60  12-Jul-1999  DS1005 support                                       *
* Vs 1.61  23-Sep-1999  new error code DSP_INVALID_BOARD_VERSION             *
* Vs 1.70  19-May-2000  DS1104 support                                       *
* Vs 1.80  03-Feb-2003  DS1006 support                                       *
* Vs 1.91  03-Dec-2004  Temporary modifications in intermediate versions     *
*                       removed                                              *
* Vs 1.92  28-Jul-2006  new error code DSP_INVALID_HOST_APP_ID               *
* Vs 1.93  07-Nov-2008  DSP_MAX_BOARD_IDX now 79                             *
* Vs 1.94  30-Nov-2009  mirror memory access function prototypes added       *
* Vs 1.95  12-Jan-2010  error code DSP_DAQ_SOCKET_NOT_AVAILABLE added        *
* Vs 1.96  22-Jan-2010  new error codes for mirror memory                    *
* Vs 1.97  20-Apr-2010  DS_system_is_connected added                         *
*                                                                            *
* Copyright (C) 1995 - 2010 by dSPACE GmbH, Germany                          *
*****************************************************************************/

/* $RCSfile: clib.h $ $Revision: 1.11 $ $Date: 2010/04/20 18:32:26MESZ $ */

#ifndef _INC_CLIB

#define _INC_CLIB

#ifdef __cplusplus
extern "C" {
#endif

#include "dstypes.h"

/****************************************************************************/
/****************************************************************************/
/*   symbolic constants                                                     */

#define DSP_MIN_BOARD_IDX        0
#define DSP_MAX_BOARD_IDX        79

#define DSP_BOARD_NAME_LENGTH    8
#define DSP_DATE_LENGTH          14  /* date format YYYYMMDDHHMMSS          */
#define DSP_PATH_LENGTH          255 /* max. length of object file path     */
#define DSP_NAME_LENGTH          15  /* max. length of network client name  */
#define DSP_HOST_APP_NAME_LENGTH 8

#define DSP_TRUE   1
#define DSP_FALSE  0


#define TYPE_DS1002   0x1
#define TYPE_DS1003   0x2
#define TYPE_DS1004   0x3
#define TYPE_DS1102   0x4
#define TYPE_DS1201   0x5
#define TYPE_DS2301   0x6
#define TYPE_DS2302   0x7
#define TYPE_DS1103   0x8
#define TYPE_DS1401   0x9
#define TYPE_DS1005   0xa
#define TYPE_DS1104   0xb
#define TYPE_DS1006   0xc

#define DSP_IRQ_10 0
#define DSP_IRQ_11 1
#define DSP_IRQ_12 2
#define DSP_IRQ_15 3


/****************************************************************************/
/****************************************************************************/
/*   error codes                                                            */

#define DSP_NO_ERROR                    0

#define DSP_NO_FREE_HOST_APP_IDX     1000
#define DSP_BOARD_NOT_PRESENT        1001
#define DSP_BOARD_NOT_REGISTERED     1002
#define DSP_INVALID_BOARD_INDEX      1003
#define DSP_BOARD_IS_LOCKED          1004
#define DSP_PROGRAM_IS_LOCKED        1005
#define DSP_BOARD_NOT_LOCKED         1006
#define DSP_MAP_AREA_LOCKED          1007
#define DSP_SEMAPHORE_NOT_AVAILABLE  1008
#define DSP_NO_FREE_BOARD_INDEX      1009
#define DSP_INVALID_BOARD_TYPE       1010
#define DSP_PORT_ADDRESS_IN_USE      1011
#define DSP_NO_FREE_MEM              1012
#define DSP_NO_MEM_DESCRIPTOR        1013
#define DSP_MEMB_NOT_FOUND           1014
#define DSP_MEMB_NO_ACCESS           1015
#define DSP_INVALID_HOSTMEM_SECTION  1016
#define DSP_NO_MEM_ALLOCATED         1017
#define DSP_INVALID_TRAP             1018
#define DSP_TRAP_IN_USE              1019
#define DSP_SLAVE_TIMEOUT            1020
#define DSP_BLOCK_OVERFLOW           1021
#define DSP_STP_FAILURE              1022
#define DSP_ICR_FAILURE              1023
#define DSP_INVALID_NAME             1024
#define DSP_DUPLICATE_NAMES          1025
#define DSP_INVALID_MAPPING_ADDR     1026
#define DSP_INVALID_OFFSET           1027
#define DSP_INT_NOT_DISABLED         1028
#define DSP_MASTER_NOT_REGISTERED    1029
#define DSP_DATA_NOT_AVAILABLE       1030
#define DSP_TOO_MUCH_DATA            1031
#define DSP_INVALID_PARAMETER        1032
#define DSP_INVALID_MEMORY_MODULE    1033
#define DSP_ILLEGAL_MEMORY_SIZE      1034
#define DSP_NO_DEVICE_ATTACHED       1035
#define DSP_ACCESS_TIMEOUT           1036
#define DSP_PROCESSOR_NOT_RESET      1037
#define DSP_PROTOCOL_ERROR           1038
#define DSP_INVALID_BOARD_VERSION    1039
#define DSP_INVALID_STATE            1040
#define DSP_INVALID_HOST_APP_ID      1041
#define DSP_NO_MIRROR_MEM_ALLOCATED  1042
#define DSP_MIRROR_COMMAND_TIMEOUT   1043
#define DSP_MIRROR_COMMAND_FAILED    1044
#define DSP_MIRROR_INVALID_ADDR	     1045

/* return codes referring to errors on searching drivers or network errors */
#define DSP_DEVICE_DRIVER_NOT_FOUND  3001
#define DSP_NOT_CONNECTED_TO_DAEMON  3001
#define DSP_NET_ERROR                3002
#define DSP_SVC_NET_ERROR            3003
#define DSP_INVALID_FUNCTION         3004
#define DSP_VXD_NOT_LOADED           3005
#define DSP_CONNECTION_IN_USE        3006
#define DSP_ALREADY_CONNECTED        3007
#define DSP_NOT_CONNECTED_TO_BOX     3008
#define DSP_TCP_NOT_LOADED           3009
#define DSP_ALLOC_SEL_FAILED         3010  /* only with Matlab mex files */
#define DSP_INVALID_CLIENT           3011
#define DSP_LOWER_DRIVER_NOT_FOUND   3012
#define DSP_DAQ_SOCKET_NOT_AVAILABLE 3013
#define DSP_NET_TIMEOUT              3014

#define DSP_ADDRESS_ERROR            4001  /* only MS Windows version */
#define DSP_REENTRY_ERROR            4002  /* only DOS Net version */
#define DSP_INVALID_RETURN_DATA      4003
#define DSP_DRV_CALL_FAILED          4004
#define DSP_NOT_ENOUGH_MEMORY        4005
#define DSP_RESOURCE_CONFLICT        4006

#define DSP_LOCK_REQ_QUEUED          6000  /* only MS Windows version */


/****************************************************************************/
/****************************************************************************/
/*   structure definitions                                                  */

#ifndef _BOARD_SPEC_DEFINED

typedef struct{
    UInt16 io_port;
    UInt16 ad_sel_reg;
    UInt16 primary_ram_size;
}spec_1002_tp;

typedef struct{
    UInt16 io_port;
    UInt16 ad_sel_reg;
    UInt16 local0_ram_size;
    UInt16 local1_ram_size;
    UInt16 global_ram_size;
}spec_1003_tp;

typedef struct{
    UInt16 io_port;
    UInt16 ram_size;
    UInt16 master_board_index;
}spec_1004_tp;

typedef struct{
    UInt16 io_port;
    UInt16 filler;
    UInt32 local_ram_size;
    UInt32 global_ram_size;
}spec_1005_tp;

typedef struct{
    UInt16 io_port;
    UInt16 filler;
    UInt32 global_ram_size;
}spec_1006_tp;

typedef struct{
    UInt16 io_port;
    UInt16 primary_ram_size;
}spec_1102_tp;

typedef struct{
    UInt16 io_port;
    UInt16 filler;
    UInt32 local_ram_size;
    UInt32 global_ram_size;
}spec_1103_tp;

typedef struct{
    UInt32 ram_address;
    UInt32 ram_size;
    UInt32 config_ram_address;
}spec_1104_tp;

typedef struct{
    UInt16 io_port;
}spec_1201_tp;

typedef struct{
    UInt16 io_port;
    UInt16 filler;
    UInt32 local_ram_size;
    UInt32 global_ram_size;
}spec_1401_tp;

typedef struct{
    UInt16 io_port;
}spec_2301_tp;

typedef struct{
    UInt16 io_port;
}spec_2302_tp;

typedef struct board_spec{
    UInt16 board_type;
    char board_name[9];     /* 8 characters plus nullbyte */
    char filler;            /* force 16 bit word alignment */
    union{
        spec_1002_tp spec_1002;
        spec_1003_tp spec_1003;
        spec_1004_tp spec_1004;
        spec_1005_tp spec_1005;
        spec_1006_tp spec_1006;
        spec_1102_tp spec_1102;
        spec_1103_tp spec_1103;
        spec_1104_tp spec_1104;
        spec_1201_tp spec_1201;
        spec_1401_tp spec_1401;
        spec_2301_tp spec_2301;
        spec_2302_tp spec_2302;
    }type_spec;
} board_spec_tp;

typedef board_spec_tp *board_spec_ptp;

#define _BOARD_SPEC_DEFINED

#endif  /* _BOARD_SPEC_DEFINED */


/****************************************************************************/
/****************************************************************************/
/*   function prototypes                                                    */

#ifndef _FAR
#  ifdef _WIN32
#    define _FAR
#  else
#    define _FAR far
#  endif
#endif


/*** network functions ***/

int DSP_client_name(char _FAR *client_name);

int DSP_net_connect(char _FAR *client_name);

int DSP_net_disconnect(void);

int DSP_net_get_error(int *net_err, int *net_sub_err, char **err_msg);

/****************************************************************************/


/*** DSP access control (user level) ***/

int DSP_api_version(unsigned int *version);

int DSP_board_index(char _FAR *board_name,unsigned int *board_index);

int DSP_board_name(unsigned int board_index, char _FAR *board_name);

int DSP_board_spec(unsigned int board_index, struct board_spec _FAR *board);

int DSP_board_type(unsigned int board_index, UInt16 *board_type);

int DSP_get_dsp_app(unsigned int board_index, char _FAR *pathstring,
                    char _FAR *datestring);

int DSP_lock_board(unsigned int board_index);

int DSP_lock_program(unsigned int board_index);

int DSP_register_host_app(char _FAR *appname);

int DSP_unlock_board(unsigned int board_index);

int DSP_unlock_program(unsigned int board_index);

int DSP_unregister_host_app(void);

/****************************************************************************/

/*** DSP access control (system level) ***/


int DSP_register_board(struct board_spec _FAR *board,
                       unsigned int *board_index);

int DSP_set_hostmem_section(unsigned int board_index, UInt32 base,
                            UInt32 upper_lim);

int DSP_set_dsp_app(unsigned int board_index, char _FAR *pathstring,
                    char _FAR *datestring);

/****************************************************************************/


/*** DSP board access ***/

int DSP_address_select_reg(unsigned int board_index, UInt8 *ad_sel_reg);

int DSP_has_pending_interrupt(unsigned int board_index, int *intr_state);

int DSP_interrupt_control_reg(unsigned int board_index, UInt8 *ic_reg);

int DSP_interrupt_dsp(unsigned int board_index);

int DSP_io_error(unsigned int board_index, int *error);

int DSP_is_reset(unsigned int board_index, int *reset_state);

int DSP_is_running(unsigned int board_index, int *run_state);

int DSP_page_select_reg(unsigned int board_index, UInt8 *psr_reg);

int DSP_port_base_address(unsigned int board_index, UInt16 *pb_address);

int DSP_reset_dsp(unsigned int board_index);

int DSP_reset_slave(unsigned int board_index);

int DSP_reset_testbus_ctrl(unsigned int board_index);

int DSP_restart_dsp(unsigned int board_index);

int DSP_setup_reg(unsigned int board_index, UInt8 *stp_reg);

int DSP_status_reg(unsigned int board_index, UInt8 *status_reg);

int DSP_testbus_ctrl_addr_reg(unsigned int board_index, UInt8 *addr_reg);

int DSP_testbus_ctrl_data_reg(unsigned int board_index, UInt16 *data_reg);

int DSP_access_mode(unsigned int board_index,int state);

/****************************************************************************/


/*** DSP watchdog control (DS1002 and DS1003) ***/

int DSP_set_watchdog_period(unsigned int board_index, UInt8 period);

int DSP_start_watchdog(unsigned int board_index);

int DSP_stop_watchdog(unsigned int board_index);

int DSP_watchdog_restart_mode(unsigned int board_index);

int DSP_watchdog_shutdown_mode(unsigned int board_index);

int DSP_watchdog_timing_reg(unsigned int board_index, UInt8 *wdt_reg);

/****************************************************************************/


/*** DSP memory access ***/
 
int DSP_alloc_mem(unsigned int board_index, UInt32 requirement,
                  UInt32 *address);

int DSP_free_mem(unsigned int board_index, UInt32 address);

int DSP_max_block_size(unsigned int board_index, UInt32 *block_size);

int DSP_read(unsigned int board_index, UInt32 offset, UInt32 *value);

int DSP_read_block(unsigned int board_index, UInt32 offset,
                   unsigned int count, UInt32 _FAR *data);

int DSP_read_dual_port_memory(unsigned int board_index,
                              UInt32 offs,UInt32 *value);

int DSP_read_phs_bus(unsigned int board_index,UInt32 offs,UInt32 *value);

int DSP_write(unsigned int board_index, UInt32 offset, UInt32 value);

int DSP_write_block(unsigned int board_index, UInt32 offset,
                    unsigned int count, UInt32 _FAR *data);

int DSP_write_dual_port_memory(unsigned int board_index,
                               UInt32 offs, UInt32 value);

int DSP_write_phs_bus(unsigned int board_index, UInt32 offs, UInt32 value);

int DSP_read_eeprom_word(unsigned int board_idx, UInt16 addr, UInt32 *value);

int DS_clear_flash(unsigned int board_index, UInt32 offset, UInt32 size);

int DS_read16(unsigned int board_index, UInt32 offset, UInt16 *value);

int DS_read32(unsigned int board_index, UInt32 offset, UInt32 *value);

int DS_read64(unsigned int board_index, UInt32 offset, UInt64 *value);

int DS_read_block16(unsigned int board_index, UInt32 offset,
                     unsigned int count, UInt16 _FAR *data);

int DS_read_block32(unsigned int board_index, UInt32 offset,
                     unsigned int count, UInt32 _FAR *data);

int DS_read_block64(unsigned int board_index, UInt32 offset,
                     unsigned int count, UInt64 _FAR *data);

int DS_write16(unsigned int board_index, UInt32 offset, UInt16 value);

int DS_write32(unsigned int board_index, UInt32 offset, UInt32 value);

int DS_write64(unsigned int board_index, UInt32 offset, UInt64 value);

int DS_write_block16(unsigned int board_index, UInt32 offset,
                      unsigned int count, UInt16 _FAR *data);

int DS_write_block32(unsigned int board_index, UInt32 offset,
                      unsigned int count, UInt32 _FAR *data);

int DS_write_block64(unsigned int board_index, UInt32 offset,
                      unsigned int count, UInt64 _FAR *data);

/****************************************************************************/


/*** TMS320 - IEEE floating-point format conversion ***/

UInt32 DSP_cvt_ieee_to_ti(Float32 IEEE_32);

Float32 DSP_cvt_ti_to_ieee(UInt32 C30);


/****************************************************************************/


/*** Mirror memory access functions ***/


int DS_mirror_write16(unsigned int      board_index, 
                      UInt32            address,
                      unsigned int      count,
                      UInt16            *data);

int DS_mirror_read16(unsigned int      board_index,
                     UInt32            address,
                     unsigned int      count,
                     UInt16            *data);


int DS_mirror_write32(unsigned int      board_index,
                      UInt32            address,
                      unsigned int      count,
                      UInt32            *data);


int DS_mirror_read32(unsigned int      board_index,
                     UInt32            address,
                     unsigned int      count, 
                     UInt32            *data);


int DS_mirror_write64(unsigned int      board_index, 
                      UInt32            address,
                      unsigned int      count, 
                      UInt64            *data);


int DS_mirror_read64(unsigned int      board_index, 
                     UInt32            address,
                     unsigned int      count,
                     UInt64            *data);


int DS_start_daq(unsigned int board_index,
                 UInt32 flags_addr,
                 UInt32 buffer_addr,
                 UInt32 buffer_byte_length,
                 UInt32 pre_trigger_buffer_length,
                 UInt32 *DAQHandle);


int DS_stop_daq(unsigned int board_index,
                UInt32 DAQHandle);


int DS_stop_all_daqs(unsigned int board_index);


int DS_system_is_connected(unsigned int board_index,
                           int *connected); 



#ifdef __cplusplus
}
#endif


#endif    /* _INC_CLIB */


