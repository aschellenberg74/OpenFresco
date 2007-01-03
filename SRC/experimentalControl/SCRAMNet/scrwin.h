/*
------------------------------------------------------------------------
 SCRWIN.H  - System macro definition file
------------------------------------------------------------------------
*/

#ifndef __SCRWIN__
#define __SCRWIN__

/*
------------------------------------------------------------------------
  System Header Files
------------------------------------------------------------------------
*/

#include <windows.h>
#include <conio.h>

/*
------------------------------------------------------------------------------
  Typedefs
------------------------------------------------------------------------------
*/
/* The following macro is intended to help resolve some of the complexity of 
   dll funtion declerations. Functions exported from DLLs under Borland 4.5
   or from 16-bit DLLs (WIN16 defined) are exported useing the CALLBACK and 
   export keywords.

   Example:     
        BOOL CALLBACK _export scram_init( void );

   Microsoft and Borland 5.0 users work with the __delspec keyword. The __delspec 
   keyword comes in two varities based on weather the funtion is to imported or exported.

   Example:
        __declspec(dllexport) BOOL scram_init( void );  // for the DLL exporting the function
                            
        __declspec(dllimport) BOOL scram_init( void ); // for the application importing the function.

   This macro is designed so that if you define DLL_DEF in the DLL code exporting the functions it 
   will help resolve the function definition apropriately.

   In Visual C++ this can be done from the IDE by adding the definition from the settings dialog
   where you can add Preprocessor Definitiions. With Borland IDE go to Options|Project|Compiler|defines 
   and enter the words you want to define.
   This can also be done by from the command line with the /D flag. 
   Definition can also be placed directly in the source code with #define directives.
*/


#if defined (WIN16) && defined (DLL_DEF)
#undef DLL_DEF
#define DLL_DEF far __pascal _export
#endif

#if defined (WIN16) && !defined (DLL_DEF)
#define DLL_DEF far __pascal
#endif

#if !defined(WIN16) && defined (DLL_DEF)                                                
#undef DLL_DEF
#define DLL_DEF  __declspec(dllexport) 
#endif

#if !defined (WIN16) && !defined (DLL_DEF)
#define DLL_DEF  __declspec(dllimport) 
#endif

#if !defined (WIN16) && !defined (C_CALL_CONV)
#define CALL_CONV  __stdcall 
#endif

#if !defined (WIN16) && defined (C_CALL_CONV)
#define CALL_CONV  __cdecl 
#endif

/* 
------------------------------------------------------------------------------
the following macro is for use in applications where the keyword export needs 
to be included for 16-bit application but excluded for 32-bit applications.
------------------------------------------------------------------------------
*/

#ifdef WIN16
#define EXPORT16 __export
#else
#define EXPORT16
#endif

/* 
------------------------------------------------------------------------------
16/32 BIT COMPILER DEPENDENT TYPE DEFINITIONS.
------------------------------------------------------------------------------
*/

#ifdef WIN16
   typedef volatile unsigned long int huge * SCR_LONG_PTR;
   typedef volatile unsigned short int huge * SCR_SHORT_PTR;
   typedef volatile unsigned char huge * SCR_BYTE_PTR;
   typedef volatile unsigned short __huge * USHUGE;
   typedef volatile unsigned long _huge * DWORDPTR;
   typedef volatile unsigned short huge * WORDPTR;
   typedef volatile unsigned char huge * BYTEPTR;
   typedef unsigned int					SCR_INT;
#else
   typedef volatile unsigned long int * SCR_LONG_PTR;
   typedef volatile unsigned short int * SCR_SHORT_PTR;
   typedef volatile unsigned char * SCR_BYTE_PTR;
   typedef volatile unsigned long * DWORDPTR;
   typedef volatile unsigned short * WORDPTR;
   typedef volatile unsigned char * BYTEPTR;
#ifndef NTPCI_COMPATABLE 
	typedef short	SCR_INT;
#else
   typedef					int 	SCR_INT;
#endif
#endif

#ifndef _WINNT
typedef unsigned short int USINT;
typedef unsigned char UCHAR;
typedef unsigned long ULINT;
#define DEF_CARD_NUM 0
#endif

#endif /* __SCRWIN__ */
