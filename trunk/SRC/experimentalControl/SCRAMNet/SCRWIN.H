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

#if  defined (DLL_DEF)                                                
#undef DLL_DEF
#define DLL_DEF  __declspec(dllexport) 
#endif

#if !defined (DLL_DEF)
#define DLL_DEF  __declspec(dllimport) 
#endif

#if !defined (C_CALL_CONV)
#define CALL_CONV  _stdcall 
#endif

#if defined (C_CALL_CONV)
#define CALL_CONV  _cdecl 
#endif

#ifdef _win64
#undef CALL_CONV
#define CALL_CONV  FASTCALL
#endif

/* 
------------------------------------------------------------------------------
32 BIT COMPILER DEPENDENT TYPE DEFINITIONS.
------------------------------------------------------------------------------
*/

   typedef volatile unsigned long int * SCR_LONG_PTR;
   typedef volatile unsigned short int * SCR_SHORT_PTR;
   typedef volatile unsigned char * SCR_BYTE_PTR;
   typedef volatile unsigned long * DWORDPTR;
   typedef volatile unsigned short * WORDPTR;
   typedef volatile unsigned char * BYTEPTR;
   //DLC 8-5-05 added new data type to use for holding address values
   typedef DWORD64 SCR_ADDR_TYPE;

   /* DLC 3-20-08 - modified type to fource 32-bit */
/*
#ifndef NTPCI_COMPATABLE 
	typedef short	SCR_INT;
#else
   typedef	int 	SCR_INT;
#endif
*/
   typedef	DWORD32 	SCR_INT;

#endif /* __SCRWIN__ */
