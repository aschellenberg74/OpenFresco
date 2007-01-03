/* lr_timer.h ***********************************************************
*                                                                       *
* Low-Resolution Timer Utilities                                        *
*                                                                       *
* uses the PC/AT's real-time clock to calibrate software delay          *
* 1 msec resolution                                                     *
*                                                                       *
* 1993 - 1996 by dSPACE GmbH, Paderborn                                        *
************************************************************************/

/* $RCSfile$ $Revision$ $Date$ */

#ifndef _LR_TIMER
#define _LR_TIMER

#ifdef __cplusplus
extern "C" {
#endif

#include <dstypes.h>

void LR_TIMER_delay (UInt32 duration);             /* duration in msec */

#ifdef __cplusplus
}
#endif

#endif
