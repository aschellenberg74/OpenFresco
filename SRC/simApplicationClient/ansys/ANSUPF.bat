@echo off
:: cls

del /q compile.log compile_error.txt link.log link_error.txt >NUL 2>&1

if "%PROCESSOR_ARCHITECTURE%"=="AMD64" (

   set PLATFORM_DIR=winx64
   set IFORT_PLATFORM=intel64
   set MACHINE_TARGET=X64

) else if "%PROCESSOR_ARCHITEW6432%"=="AMD64" (

   set PLATFORM_DIR=winx64
   set IFORT_PLATFORM=intel64
   set MACHINE_TARGET=X64

) else if "%PROCESSOR_ARCHITECTURE%"=="x86" (

   set PLATFORM_DIR=intel
   set IFORT_PLATFORM=ia32
   set MACHINE_TARGET=X86
)

call "%IFORT_COMPILER15%\bin\compilervars.bat" %IFORT_PLATFORM% vs2012

set INCLUDE=%AWP_ROOT180%\ansys\customize\Include;%INCLUDE%
set LIB=%AWP_ROOT180%\ansys\Custom\Lib\%PLATFORM_DIR%;%LIB%

setlocal ENABLEDELAYEDEXPANSION

for /f "eol= tokens=1 delims=. " %%U in (ansupf.lst) DO (
   set UPFFILE=%%ULib
   del /q *.obj !UPFFILE!.lib !UPFFILE!.dll !UPFFILE!.lrf !UPFFILE!.map !UPFFILE!.def !UPFFILE!.exp >NUL 2>&1
REM   del /q !UPFFILE!.obj !UPFFILE!.lib !UPFFILE!.dll !UPFFILE!.lrf !UPFFILE!.map !UPFFILE!.def !UPFFILE!.exp >NUL 2>&1
   echo EXPORTS>!UPFFILE!ex.def
   echo.  >>!UPFFILE!ex.def
   echo %%U>>!UPFFILE!ex.def
   "%AWP_ROOT180%\ansys\Custom\user\%PLATFORM_DIR%\upcase" !UPFFILE!ex.def
   echo -out:!UPFFILE!.dll>!UPFFILE!.lrf
   echo -def:!UPFFILE!ex.def>>!UPFFILE!.lrf
   echo -dll>>!UPFFILE!.lrf
   echo -machine:%MACHINE_TARGET%>>!UPFFILE!.lrf
   echo -map>>!UPFFILE!.lrf
   echo -manifest:embed>>!UPFFILE!.lrf
   echo -defaultlib:ANSYS.lib>>!UPFFILE!.lrf
   echo. >>!UPFFILE!.lrf
   echo *.obj>>!UPFFILE!.lrf

   if "%PLATFORM_DIR%"=="winx64" (

   for %%p in ( "*.F" ) do ifort -D__EFL -DNOSTDCALL -DARGTRAIL /O2 -DPCWIN64_SYS -DPCWINX64_SYS -DPCWINNT_SYS -DCADOE_ANSYS /fpp /4Yportlib /auto /c /Fo.\ /DFORTRAN /MD /watch:source %%p >>compile.log  2>&1

   for %%p in ( "*.c" ) do cl -DNOSTDCALL -DARGTRAIL /O2 /Gy- /EHsc /DPCWINNT_SYS /DPCWIN64_SYS /DPCWINX64_SYS /DCADOE_ANSYS /DCURVEFIT_EXPORTS /Zi  /c /W3 /D_X86=1 /DOS_WIN32 /DWIN32 /D__STDC__ /MD %%p >>compile.log 2>&1
@echo off

   for %%p in ( "*.cpp" ) do cl -DNOSTDCALL -DARGTRAIL /O2 /Gy- /EHsc /DPCWINNT_SYS /DPCWIN64_SYS /DPCWINX64_SYS /DCADOE_ANSYS /DCURVEFIT_EXPORTS /Zi  /c /W3 /D_X86=1 /DOS_WIN32 /DWIN32 /D__STDC__ /MD %%p >>compile.log 2>&1

   )

   if "%PLATFORM_DIR%"=="intel" (

   for %%p in ( "*.F" ) do ifort -D__EFL -DNOSTDCALL -DARGTRAIL /O2 -DPCWINNT_SYS -DCADOE_ANSYS /fpp /align:rec4byte /4Yportlib /auto /c /Fo.\ /DFORTRAN /MD /watch:source %%p >>compile.log  2>&1

   for %%p in ( "*.c" ) do cl -DNOSTDCALL -DARGTRAIL /O2 /Gy- /EHsc /DPCWINNT_SYS /DCADOE_ANSYS /DCURVEFIT_EXPORTS /Zi /Zp4 /c /W3 /D_X86=1 /DOS_WIN32 /DWIN32 /D__STDC__ /MD %%p >>compile.log 2>&1
   @echo off

   for %%p in ( "*.cpp" ) do cl -DNOSTDCALL -DARGTRAIL /O2 /Gy- /EHsc /DPCWINNT_SYS /DCADOE_ANSYS /DCURVEFIT_EXPORTS /Zi /Zp4 /c /W3 /D_X86=1 /DOS_WIN32 /DWIN32 /D__STDC__ /MD %%p >>compile.log 2>&1
   )

   if exist compile.log (

      FINDSTR /I /C:": error" compile.log >compile_error.txt
      if !errorlevel!==0 goto :COMPILEFAIL

   )

   link @!UPFFILE!.lrf >>link.log 2>&1
   IF !ERRORLEVEL! GEQ 1 goto LINKFAIL
REM   move /y "%AWP_ROOT180%\ansys\bin\%PLATFORM_DIR%\!UPFFILE!.dll" "%AWP_ROOT180%\ansys\bin\%PLATFORM_DIR%\!UPFFILE!.dll.orig" 
REM   copy /y !UPFFILE!.dll "%AWP_ROOT180%\ansys\bin\%PLATFORM_DIR%"

   echo.
   echo.      ************************************************************************
   echo.
   echo.      !UPFFILE!.dll ^(%%U^) has been successfully built.
   echo.
   echo.      ************************************************************************
   echo.

)

endlocal

REM set ANS_USER_PATH=%CD%
REM echo. *** ANS_USER_PATH: %ANS_USER_PATH% ***
del /q compile_error.txt >NUL 2>&1
goto :EOF

:COMPILEFAIL
echo.
echo.       ************************************************************************
echo.
echo.          UPF COMPILER ERROR!  Check compile.log for more information.
echo.
echo.
echo.      ************************************************************************
echo.
REM msg * ***** UPF COMPILER ERROR!  Check compile.log for more information. *****
goto :EOF

:LINKFAIL
copy /y link.log link_error.txt
endlocal
echo.
echo.      ************************************************************************
echo.
echo.          UPF LINK FAILED!  Check link.log for more information
echo.
echo.
echo.      ************************************************************************
echo.

