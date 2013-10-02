/* dstypes.h ************************************************************

  Implementation platform dependent data type definitions

  common version for PC / UNIX / ALPHA / C40 / PPC / F240 / C16x

  1993-98 by dSPACE GmbH, Paderborn

  $RCSfile: Dstypes.h $ $Revision: 1.2 $ $Date: 2003/08/07 16:57:00MESZ $
************************************************************************/

#ifndef __DSTYPES_H__
#define __DSTYPES_H__

/************************************************************************
  data type definitions for Host programs
************************************************************************/

#if defined _DSHOST || defined _MSC_VER || defined __TURBOC__ || defined __UNIX__ || defined(MATLAB_MEX_FILE)

/* defined identically for OpenInterface */
#ifndef _MCHPUB_
typedef char                    Int8;
typedef unsigned char           UInt8;
typedef short int               Int16;
typedef unsigned short int      UInt16;
typedef long int                Int32;
typedef unsigned long int       UInt32;

typedef Int8 *                  Int8Ptr;
typedef UInt8 *                 UInt8Ptr;
typedef Int16 *                 Int16Ptr;
typedef UInt16 *                UInt16Ptr;
typedef Int32 *                 Int32Ptr;
typedef UInt32 *                UInt32Ptr;
#endif

typedef struct { UInt32 low; Int32 high;  } Int64;
typedef struct { UInt32 low; UInt32 high; } UInt64;
typedef float                   Float32;
typedef double                  Float64;

typedef Int64 *                 Int64Ptr;
typedef UInt64 *                UInt64Ptr;
typedef Float32 *               Float32Ptr;
typedef Float64 *               Float64Ptr;

/************************************************************************
  data type definitions for Alpha application programs
************************************************************************/

#elif defined _DS1004 || defined __alpha__

typedef double                  dsfloat;

typedef char                    Int8;
typedef unsigned char           UInt8;
typedef short int               Int16;
typedef unsigned short int      UInt16;
typedef int                     Int32;
typedef unsigned int            UInt32;
typedef long int                Int64;
typedef unsigned long int       UInt64;
typedef float                   Float32;
typedef double                  Float64;

typedef Int8 *                  Int8Ptr;
typedef UInt8 *                 UInt8Ptr;
typedef Int16 *                 Int16Ptr;
typedef UInt16 *                UInt16Ptr;
typedef Int32 *                 Int32Ptr;
typedef UInt32 *                UInt32Ptr;
typedef Int64 *                 Int64Ptr;
typedef UInt64 *                UInt64Ptr;
typedef Float32 *               Float32Ptr;
typedef Float64 *               Float64Ptr;

/************************************************************************
  data type definitions for DS1003, DS1201 and DS1102 application programs
************************************************************************/

#elif defined _DS1003 || defined _DS1102 || defined _DS1201 || defined _TMS320C40

typedef float                   dsfloat;

typedef char                    Int8;
typedef unsigned char           UInt8;
typedef int                     Int16;
typedef unsigned int            UInt16;
typedef long int                Int32;
typedef unsigned long int       UInt32;
typedef struct { UInt32 low; Int32 high;  } Int64;
typedef struct { UInt32 low; UInt32 high; } UInt64;
typedef float                   Float32;

typedef Int8 *                  Int8Ptr;
typedef UInt8 *                 UInt8Ptr;
typedef Int16 *                 Int16Ptr;
typedef UInt16 *                UInt16Ptr;
typedef Int32 *                 Int32Ptr;
typedef UInt32 *                UInt32Ptr;
typedef Int64 *                 Int64Ptr;
typedef UInt64 *                UInt64Ptr;
typedef float *                 Float32Ptr;

/************************************************************************
  data type definitions for PPC application programs
************************************************************************/

#elif defined _DS1005 || defined _DS1103 || defined _DS1104 || defined _DS1401 || defined _603 || defined _603e || defined _604 || defined _604e || defined _RPCU

typedef double                  dsfloat;

typedef char                    Int8;
typedef unsigned char           UInt8;
typedef short                   Int16;
typedef unsigned short          UInt16;
typedef int                     Int32;
typedef unsigned int            UInt32;
typedef struct { UInt32 low; Int32 high;  } Int64;
typedef struct { UInt32 low; UInt32 high; } UInt64;
typedef float                   Float32;
typedef double                  Float64;

typedef Int8 *                  Int8Ptr;
typedef UInt8 *                 UInt8Ptr;
typedef Int16 *                 Int16Ptr;
typedef UInt16 *                UInt16Ptr;
typedef Int32 *                 Int32Ptr;
typedef UInt32 *                UInt32Ptr;
typedef Int64 *                 Int64Ptr;
typedef UInt64 *                UInt64Ptr;
typedef Float32 *               Float32Ptr;
typedef Float64 *               Float64Ptr;

/************************************************************************
  data type definitions for X86-32 application programs
************************************************************************/

#elif defined _DS1006 

typedef double                  dsfloat;

typedef char                    Int8;
typedef unsigned char           UInt8;
typedef short                   Int16;
typedef unsigned short          UInt16;
typedef int                     Int32;
typedef unsigned int            UInt32;
//TBD: check enidians
typedef struct { UInt32 low; Int32 high;  } Int64;
typedef struct { UInt32 low; UInt32 high; } UInt64;
typedef long long               Long64;
typedef unsigned long long      ULong64;
typedef float                   Float32;
typedef double                  Float64;

typedef Int8 *                  Int8Ptr;
typedef UInt8 *                 UInt8Ptr;
typedef Int16 *                 Int16Ptr;
typedef UInt16 *                UInt16Ptr;
typedef Int32 *                 Int32Ptr;
typedef UInt32 *                UInt32Ptr;
typedef Int64 *                 Int64Ptr;
typedef UInt64 *                UInt64Ptr;
typedef Long64 *                Long64Ptr;
typedef ULong64 *               ULong64Ptr;
typedef Float32 *               Float32Ptr;
typedef Float64 *               Float64Ptr;



/************************************************************************
  data type definitions for TMS320C2xx and C164 application programs
************************************************************************/

#elif defined _TMS320C2XX || defined _C166 || defined __C166__

typedef char                    Int8;
typedef unsigned char           UInt8;
typedef int                     Int16;
typedef unsigned int            UInt16;
typedef long                    Int32;
typedef unsigned long           UInt32;
typedef struct { UInt32 low; UInt32 high; } UInt64;

typedef Int8 *                  Int8Ptr;
typedef UInt8 *                 UInt8Ptr;
typedef Int16 *                 Int16Ptr;
typedef UInt16 *                UInt16Ptr;
typedef Int32 *                 Int32Ptr;
typedef UInt32 *                UInt32Ptr;

/************************************************************************
  data type definitions for Motorola CPU32 (MC68336)
************************************************************************/

#elif defined _MC68336 || _DS1401_DIO_TP1

typedef signed char             Int8;
typedef unsigned char           UInt8;
typedef short int               Int16;
typedef unsigned short int      UInt16;
typedef int                     Int32;
typedef unsigned int            UInt32;

typedef Int8 *                  Int8Ptr;
typedef UInt8 *                 UInt8Ptr;
typedef Int16 *                 Int16Ptr;
typedef UInt16 *                UInt16Ptr;
typedef Int32 *                 Int32Ptr;
typedef UInt32 *                UInt32Ptr;


#else
  #error target platform undefined
#endif

#endif /* __DSTYPES_H__ */

