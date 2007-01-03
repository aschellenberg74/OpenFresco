

#ifndef DRIVER

#include <winioctl.h>

#endif // DRIVER

// IOCTL info, needs to be visible for application

#define SHELLDRV_IOCTL_INDEX  0x0080


#define GET_DRIVER_LOG			CTL_CODE(FILE_DEVICE_UNKNOWN,		\
										 SHELLDRV_IOCTL_INDEX + 0,	\
										 METHOD_BUFFERED,			\
										 FILE_ANY_ACCESS)

#define GET_IRP_HIST			CTL_CODE(FILE_DEVICE_UNKNOWN,		\
										 SHELLDRV_IOCTL_INDEX + 1,	\
										 METHOD_BUFFERED,			\
										 FILE_ANY_ACCESS)

#define GET_PATH_HIST			CTL_CODE(FILE_DEVICE_UNKNOWN,		\
										 SHELLDRV_IOCTL_INDEX + 2,	\
										 METHOD_BUFFERED,			\
										 FILE_ANY_ACCESS)

#define GET_ERROR_LOG			CTL_CODE(FILE_DEVICE_UNKNOWN,		\
										 SHELLDRV_IOCTL_INDEX + 3,	\
										 METHOD_BUFFERED,			\
										 FILE_ANY_ACCESS)

#define GET_ATTACHED_DEVICES	CTL_CODE(FILE_DEVICE_UNKNOWN,		\
										 SHELLDRV_IOCTL_INDEX + 4,	\
										 METHOD_BUFFERED,			\
										 FILE_ANY_ACCESS)

#define SET_IRP_HIST_SIZE		CTL_CODE(FILE_DEVICE_UNKNOWN,		\
										 SHELLDRV_IOCTL_INDEX + 5,	\
										 METHOD_BUFFERED,			\
										 FILE_ANY_ACCESS)

#define SET_PATH_HIST_SIZE		CTL_CODE(FILE_DEVICE_UNKNOWN,		\
										 SHELLDRV_IOCTL_INDEX + 6,	\
										 METHOD_BUFFERED,			\
										 FILE_ANY_ACCESS)

#define SET_ERROR_LOG_SIZE		CTL_CODE(FILE_DEVICE_UNKNOWN,		\
										 SHELLDRV_IOCTL_INDEX + 7,	\
										 METHOD_BUFFERED,			\
										 FILE_ANY_ACCESS)

// we will handle this in test driver code for a sample of
// handling IOCTLs
#define GET_DRIVER_INFO			CTL_CODE(FILE_DEVICE_UNKNOWN,		\
										 SHELLDRV_IOCTL_INDEX + 8,	\
										 METHOD_BUFFERED,			\
										 FILE_ANY_ACCESS)


