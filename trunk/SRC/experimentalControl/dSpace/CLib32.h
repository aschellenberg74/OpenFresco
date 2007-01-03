/* clib32.h ******************************************************************
*                                                                            *
* Header file for Host-RTP Interface Library                                 *
*                                                                            *
* basic utilities to control RTP board functions and to                      *
* access RTP board memory                                                    *
*                                                                            *
* for PC-bus and network version                                             *
*                                                                            *
* Vs 1.0   17-Mar-1999                                                       *
* Vs 1.1   02-Nov-1999  DS1005 support added                                 *
* Vs 1.11  05-May-2000  new error code added                                 *
* Vs 1.2   22-Nov-2000  DS1104 support added                                 *
*                                                                            *
*                                                                            *
* Copyright (C) 1999 - 2000 by dSPACE GmbH, Paderborn                        *
*****************************************************************************/
/* $RCSfile$ $Revision$ $Date$ */

#ifndef _INC_CLIB32

#define _INC_CLIB32

#ifdef __cplusplus
  extern "C" {
#endif

#include "dstypes.h"

/****************************************************************************/
/****************************************************************************/
/*   symbolic constants                                                     */

#define DS_MIN_BOARD_IDX        0
#define DS_MAX_BOARD_IDX        19

#define DS_BOARD_NAME_LENGTH    8
#define DS_DATE_LENGTH          14   /* date format YYYYMMDDHHMMSS          */
#define DS_PATH_LENGTH          255  /* max. length of object file path     */
#define DS_NAME_LENGTH          15   /* max. length of network client name  */
#define DS_HOST_APP_NAME_LENGTH 8

#define DS_TRUE   1
#define DS_FALSE  0


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


/****************************************************************************/
/****************************************************************************/
/*   error codes                                                            */

#define DS_NO_ERROR                     0

#define DS_NO_FREE_HOST_APP_IDX         1000
#define DS_BOARD_NOT_PRESENT            1001
#define DS_BOARD_NOT_REGISTERED         1002
#define DS_INVALID_BOARD_INDEX          1003
#define DS_BOARD_IS_LOCKED              1004
#define DS_PROGRAM_IS_LOCKED            1005
#define DS_BOARD_NOT_LOCKED             1006
#define DS_MAP_AREA_LOCKED              1007
#define DS_SEMAPHORE_NOT_AVAILABLE      1008
#define DS_NO_FREE_BOARD_INDEX          1009
#define DS_INVALID_BOARD_TYPE           1010
#define DS_PORT_ADDRESS_IN_USE          1011
#define DS_NO_FREE_MEM                  1012
#define DS_NO_MEM_DESCRIPTOR            1013
#define DS_MEMB_NOT_FOUND               1014
#define DS_MEMB_NO_ACCESS               1015
#define DS_INVALID_HOSTMEM_SECTION      1016
#define DS_NO_MEM_ALLOCATED             1017
#define DS_INVALID_TRAP                 1018
#define DS_TRAP_IN_USE                  1019
#define DS_SLAVE_TIMEOUT                1020
#define DS_BLOCK_OVERFLOW               1021
#define DS_STP_FAILURE                  1022
#define DS_ICR_FAILURE                  1023
#define DS_INVALID_NAME                 1024
#define DS_DUPLICATE_NAMES              1025
#define DS_INVALID_MAPPING_ADDR         1026
#define DS_INVALID_OFFSET               1027
#define DS_INT_NOT_DISABLED             1028
#define DS_MASTER_NOT_REGISTERED        1029
#define DS_DATA_NOT_AVAILABLE           1030
#define DS_TOO_MUCH_DATA                1031
#define DS_INVALID_PARAMETER            1032
#define DS_INVALID_MEMORY_MODULE        1033
#define DS_ILLEGAL_MEMORY_SIZE          1034
#define DS_NO_DEVICE_ATTACHED           1035
#define DS_ACCESS_TIMEOUT               1036
#define DS_PROCESSOR_NOT_RESET          1037
#define DS_PROTOCOL_ERROR               1038
#define DS_INVALID_BOARD_VERSION        1039
#define DS_INVALID_STATE                1040

#define DS_APPLICATION_NOT_STARTED      2000
#define DS_APPLICATION_NOT_FOUND        2001
#define DS_INVALID_APPLICATION          2002
#define DS_INVALID_MAPFILE              2003
#define DS_MEMORY_ALLOCATION_FAILED     2004

#define DS_MAPFILE_NOT_FOUND            2100
#define DS_OUT_OF_MEMORY                2101
#define DS_ILLEGAL_VARIABLE_NAME        2102
#define DS_NO_GLOBAL_MAPFILE_SYMBOLS    2103
#define DS_UNKNOWN_MAPFILE_TYPE         2104
#define DS_UNKNOWN_MAPFILE_FORMAT       2105

#define DS_CMD_COMMUNICATION_ERROR      2200
#define DS_CMD_ILLEGAL_HOST_INDEX       2201       
#define DS_CMD_QUEUE_FULL               2203
#define DS_CMD_SERVICE_NOT_INITIALIZED  2204
#define DS_CMD_TIMEOUT                  2205
#define DS_CMD_NO_SERVICE_AVAILABLE     2206

#define DS_DEVICE_DRIVER_NOT_FOUND      3001
#define DS_NOT_CONNECTED_TO_DAEMON      3001
#define DS_NET_ERROR                    3002
#define DS_SVC_NET_ERROR                3003
#define DS_INVALID_FUNCTION             3004
#define DS_VXD_NOT_LOADED               3005
#define DS_CONNECTION_IN_USE            3006
#define DS_ALREADY_CONNECTED            3007
#define DS_NOT_CONNECTED_TO_BOX         3008
#define DS_TCP_NOT_LOADED               3009
#define DS_INVALID_CLIENT               3011
#define DS_LOWER_DRIVER_NOT_FOUND       3012

#define DS_ADDRESS_ERROR                4001  
#define DS_INVALID_RETURN_DATA          4003
#define DS_DRV_CALL_FAILED              4004
#define DS_NOT_ENOUGH_MEMORY            4005
#define DS_RESOURCE_CONFLICT            4006


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

/*** RTP board access control (user level) ***/

int DS_board_index(char *board_name,unsigned int *board_index);

int DS_board_spec(unsigned int board_index, struct board_spec *board);

int DS_lock_program(unsigned int board_index);

int DS_register_host_app(char *appname);

int DS_unlock_program(unsigned int board_index);

int DS_unregister_host_app(void);


/*** RTP board access ***/

int DS_has_pending_interrupt(unsigned int board_index, int *intr_state);

int DS_interrupt_rtp(unsigned int board_index);

int DS_is_reset(unsigned int board_index, int *reset_state);

int DS_reset_rtp(unsigned int board_index);

int DS_restart_rtp(unsigned int board_index);


/*** RTP memory access ***/

int DS_get_var_addr(unsigned int board_index,
                    char         *varname,
                    UInt32       *addr);

int DS_alloc_mem(unsigned int board_index, UInt32 requirement,
                 UInt32 *address);

int DS_free_mem(unsigned int board_index, UInt32 address);

int DS_read_8(unsigned int board_index, UInt32 offset,
              unsigned int count, UInt8 *data);

int DS_read_16(unsigned int board_index, UInt32 offset,
               unsigned int count, UInt16 *data);

int DS_read_32(unsigned int board_index, UInt32 offset,
               unsigned int count, UInt32 *data);

int DS_read_64(unsigned int board_index, UInt32 offset,
               unsigned int count, UInt64 *data);

int DS_write_8(unsigned int board_index, UInt32 offset,
               unsigned int count, UInt8 *data);

int DS_write_16(unsigned int board_index, UInt32 offset,
                unsigned int count, UInt16 *data);

int DS_write_32(unsigned int board_index, UInt32 offset,
                unsigned int count, UInt32 *data);

int DS_write_64(unsigned int board_index, UInt32 offset,
                unsigned int count, UInt64 *data);


/*** TMS320 - IEEE floating-point format conversion ***/

UInt32  DS_cvt_ieee_to_ti(Float32 IEEE_32);

Float32 DS_cvt_ti_to_ieee(UInt32  C30);


#ifdef __cplusplus
  }
#endif

#endif    /* _INC_CLIB32 */


