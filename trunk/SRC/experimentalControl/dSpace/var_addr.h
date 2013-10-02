/* var_addr.h ***********************************************************
*                                                                       *
* Evaluation of variable addresses in DSP memory                        *
*                                                                       *
* Copyright (C) 1993 - 1996 by dSPACE GmbH, Paderborn                   *
************************************************************************/

/* $RCSfile: var_addr.h $ $Revision: 1.1 $ $Date: 1996/03/06 15:03:16MEZ $ */

#ifndef _VAR_ADDR
#define _VAR_ADDR


#ifdef __cplusplus
extern "C" {
#endif


#include <dstypes.h>

#define VAR_NO_ERROR		0

/* error definitions for VAR_init_addr_table() */
#define VAR_FILE_NOT_FOUND	1     /* error opening linker map file */
#define VAR_OUT_OF_MEMORY	2   /* cannot allocate required memory */
#define VAR_NO_GLOBAL_SYM	3     /* no global symbols in map file */
#define VAR_FORMAT_ERROR	4     /* bad format in linker map file */

/* error definition for VAR_get_addr() */
#define VAR_ILLEGAL_NAME	1           /* variable name not found */

int VAR_init_addr_table (char *filename, void **addr_table);

int VAR_get_addr (char *varname, UInt32 *addr, void *addr_table);

void VAR_dispose_addr_table (void *addr_table);


#ifdef __cplusplus
}
#endif


#endif
