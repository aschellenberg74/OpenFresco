*comdeck,impcom                                                         jas sys
#include "computer.h"

c         *** copyright(c) 2009 SAS IP, Inc.  All rights reserved.
c         *** ansys, inc.

c  Notice - changes to this include file DO NOT cause a recompile of
c           all routines which reference it (there are thousands).  If
c           a change is made which is other than a comment it should
c           be checked in on Friday to hit the weekend full recompile.
c
c *** mpg impcom.inc < all: implicit none, intrinsic, tiny/huge
c
c *** use either declare option of compiler or implicit none
#if defined(HP_SYS)
      implicit none
#elif defined (DECAXPOSF_SYS) || defined (SOLARIS_SYS)
      implicit none
#elif defined (SGIR8K_SYS) || defined (SGIR4K_SYS)
      implicit none
#elif defined (PCWINNT_SYS)
      implicit none
#elif defined(LINUXIA32_SYS) || defined(LINUXIA64_SYS)
      implicit none
#elif defined(LINUXOP64_SYS) || defined(LINUXEM64T_SYS)
      implicit none
#else
      implicit undefined (a-z)
#endif

c#if !defined(HP_SYS) && !defined(RS6000_SYS)
c#  if !defined(PCWINNT_SYS)
c#     external getenv
c#  endif
c#endif


c *** 'GENERIC' intrinsic functions.
      intrinsic   int, dble, cmplx, aint, anint, nint, abs, mod, sign,
     x            dim, max, min, sqrt, exp, log, log10, sin, cos, tan,
     x            asin, acos, atan, atan2, sinh, cosh, tanh, dint, trim,
     x            adjustr, adjustl, len_trim

c *** 'SPECIFIC' intrinsic functions.
      intrinsic   ichar, char, dprod, len, index, aimag, conjg

c *** complex*16 intrinsic functions.
#if  !defined(SGI_SYS) && !defined(PCWINNT_SYS)
      intrinsic    cdsqrt, dconjg, cdlog,  cdexp, dcmplx
#endif


c *** double precision intrinsics with complex*16 arguments
#if !defined(SGI_SYS) && !defined(PCWINNT_SYS) 
      intrinsic    cdabs, dreal, dimag
#endif


c *** the following are non-standard f77 bit functions.  Add the
c *** appropriate ifdef statements

#if defined(RS6000_SYS) || defined(HP_SYS) || defined (SGI_SYS)
      intrinsic  btest,  ibclr, ibset, ishft
      intrinsic  iand  , ior   , ieor
#elif defined(FUJITSU_SYS)
      intrinsic  btest,  ibclr, ibset, ishft
      intrinsic  iand  , ior   , ieor
#elif defined(SOLARIS_SYS) && !defined (FUJITSU_SYS)
      intrinsic  btest,  ibclr, ibset, ishft
      intrinsic  iand  , ior   , ieor
#elif defined (DECAXPOSF_SYS) || defined (LINUXIA32_SYS)
      intrinsic  btest,  ibclr, ibset, ishft
      intrinsic  iand  , ior   , ieor
#elif defined (LINUXIA64_SYS)
      intrinsic  btest,  ibclr, ibset, ishft
      intrinsic  iand  , ior   , ieor
#elif defined (LINUXOP64_SYS) || defined (LINUXEM64T_SYS)
      intrinsic  btest,  ibclr, ibset, ishft
      intrinsic  iand  , ior   , ieor
#elif defined (PCWINNT_SYS)
#else
      external   btest,  ibclr, ibset, ishft
      external   iand,   ior,   ieor
      logical    btest
      integer    ibclr, ibset, ishft
      integer    iand,   ior,   ieor
#endif

#if !defined (PCWINNT_SYS)
      intrinsic  not
#endif


c ***  end of intrinsics


c *** define BIGINT and SMALLINT:

c        *****  Define some intermediate parameters (overflow problems)  *****

      integer BIGINT,SMALLINT,INT_2_30,INT_2_30_1
      LONGINT BIGINT8,SMALLINT8,INT8_2,INT8_62,
     x INT8_2_62,INT8_2_62_1

      parameter (INT8_2=2,INT8_62=62)
      parameter (INT_2_30=2**30)
      parameter (INT_2_30_1=INT_2_30-1)

c --- if longint2 is defined, then int*8's are supported on this platform ---
#if defined(LONGINT2)
      parameter (INT8_2_62=INT8_2**INT8_62)
      parameter (INT8_2_62_1=INT8_2_62-1)
#else
c --- define the parameters for int*8's even though they aren't supported ---
      parameter (INT8_2_62=2**30)
      parameter (INT8_2_62_1=INT8_2_62-1)
#endif


c        ******  Define SMALLINT8 and BIGINT8  ******
      parameter (BIGINT8   = INT8_2_62+INT8_2_62_1,
     x           SMALLINT8 = -INT8_2_62-INT8_2_62)


c        ******  Define SMALLINT and BIGINT  ******
#if defined (SGIR4K_SYS)
c  **** THIS IS HERE BECAUSE OF A F90 COMPILER BUG FOR N32
      parameter (BIGINT    = INT_2_30+INT_2_30_1,
     x           SMALLINT  = -INT_2_30-INT_2_30+2)
#elif defined (LINUXOP64_SYS)
      parameter (BIGINT    = INT_2_30+INT_2_30_1,
     x           SMALLINT  = 0)
#else
      parameter (BIGINT    = INT_2_30+INT_2_30_1,
     x           SMALLINT  = -INT_2_30-INT_2_30)
#endif


c --- define the NULL for Heap & memory pointers ---
      integer    HEAPPTRNULL
      PTRFTN     PTRFTNNULL, PTRZERO
      parameter (HEAPPTRNULL = SMALLINT+2,
     x           PTRFTNNULL  = 0,
     x           PTRZERO     = 0)


c ***  define TINY and HUGE:
c *** "EXPON" and "NEXPON" are used because the analyzer will assume
c *** that "TINY" and "HUGE" are arrays if (100) or (-100) are used.
      integer   EXPON, NEXPON
      parameter (EXPON=100, NEXPON=-100)
      double precision   TINY, HUGE
      parameter ( TINY=2.0d0**NEXPON, HUGE=2.0d0**EXPON )
c ***  NOTE: TINY and HUGE are also the names of F90 intrinsic functions.
c ***  impcom.inc can NOT be included if these F90 intrinsics are desired.

