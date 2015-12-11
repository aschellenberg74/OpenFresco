
//
// We use a set of dummy states to delineate the actions we take
// on receipt and completion of an IRP.  These also appear below.
//

//
// In STATE_REMOVED, we immediately fail any received I/O requests
//
#define STATE_REMOVED           0X00000000

//
// In STATE_SURPRISE_REMOVED, we immediately fail all incoming requests
//
#define STATE_SURPRISE_REMOVED   0x00010000

//
// In STATE_NEVER_STARTED, we also immediately fail all incoming requests
//
#define STATE_NEVER_STARTED     0x00020000

//
// Dummy State -- When the state is < this value, no H/W access is
// allowed
//
#define STATE_ALL_BELOW_NO_HW   0x00030000	// ******************

//
// In STATE_REMOVE_PENDING, we also immediately fail all incoming requests
//
#define STATE_REMOVE_PENDING    0x00100000

//
// Dummy state -- When an IRP arrives at the driver, if the current
// device state is below this value, it is immediately failed
//
#define STATE_ALL_BELOW_FAIL    0x00FF0000	// ******************

//
// In STATE_STARTED, requests are processed and removed from the
// queues normally
//
#define STATE_STARTED           0X01000000

//
// Dummy state -- When an IRP arrives at the driver, if the current
// device state is above this value, it is queued, not initiated on
// the device (even if the device is free)
//
#define STATE_ALL_ABOVE_QUEUE   0x0FFF0000	// ******************

//
// Dummy State -- When an IRP is COMPLETED on the device, if
// the device state is below this value, we'll start another
// IRP in an attempt to drain the queue of pending requests.
//
#define STATE_ALL_BELOW_DRAIN   STATE_ALL_ABOVE_QUEUE	// ******************

//
// In STATE_STOP_PENDING, we queue incoming requests
//
#define STATE_STOP_PENDING      0x10000000

//
// In STATE_STOPPED, we queue incoming requests
//
#define STATE_STOPPED           0x10010000


char * StatusName( NTSTATUS status);
/* Return string name representing Nt status code  */

char * StateName( unsigned long int state);
/* Return string name representing Driver State    */

char * CurrentIrqlName( void );
/* Determine Current IRQL and return String representation */

VOID PnpScrRequestIncrement(PSCRAM_DEVICE_EXTENSION devExt);
/*  This function is used to track the number of requests that
 *  are presently queued for the device indicated by the passed
 *  Device Extension. */

VOID PnpScrRequestDecrement(PSCRAM_DEVICE_EXTENSION devExt);
/*  This function is used to track the number of requests that
 *  are presently queued for the device indicated by the passed
 *  Device Extension. */

VOID PnpScrWaitForStop(PSCRAM_DEVICE_EXTENSION devExt);
/*  Using the event set by the PnpScrRequestIncrement() and 
 *  PnpScrRequestDecrement() functions, this function waits until
 *  the device can be stopped.*/

VOID PnpScrWaitForRemove(PSCRAM_DEVICE_EXTENSION devExt);
/* Using the event set by the PnpScrRequestIncrement() and 
 * PnpScrRequestDecrement() functions, this function waits until
 * the device can be removed.*/

NTSTATUS PnpScrCanRemoveDevice(PSCRAM_DEVICE_EXTENSION devExt, PIRP Irp);
/* This routine determines if the device cab be safely removed. */

NTSTATUS PnpScrCanStopDevice(PSCRAM_DEVICE_EXTENSION devExt, PIRP Irp);
/* This routine determines if the device cab be safely Stoped */

char* CurrentIrqlName( void );
/* Determine Current IRQL and return String representation */

VOID PnpScrProcessQueuedRequests(IN OUT PSCRAM_DEVICE_EXTENSION devExt);
/* This interface checks the current state of the read and write
   queues, and starts requests on the device if either are not busy. */

VOID PnpScrClearQueues(PSCRAM_DEVICE_EXTENSION DevExt);

VOID PnpScrStartWriteIrp(PDEVICE_OBJECT DeviceObject, PIRP Irp);

VOID PnpScrStartReadIrp(PDEVICE_OBJECT DeviceObject, PIRP Irp);


// #define DEBUG_LOG_ERROR(status) 	DbgPrint("%s: Log Status=%s \n",DRIVER_NAME,StatusName(status))	