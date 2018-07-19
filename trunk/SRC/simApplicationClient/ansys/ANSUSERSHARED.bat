@echo off
set revn=180
set revndot=18.0
cls

del /q compile.log compile_error.txt >NUL 2>&1

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

echo.
echo.           This is the Mechanical APDL %revndot% ANSUSERSHARED script.  It is used
echo.           to build a DLL of User Programmable Features
echo.           for the Mechanical APDL program.
echo.
echo.
echo.           NOTE:  The user subroutine source file(s) are expected to
echo.                  reside in this directory.
echo.

:REPEAT
cls
set SRCFILE=
echo.
echo. Enter one of the following choices to create your
echo. User Programmable Feature DLL:
echo.

echo. USOLBEG, USOLFIN, USERCREEP, USERCR, USERFRIC, USERHYPER, USERELEM
echo. USRSHIFT, USRSURF116, UANBEG, UANFIN, UELMATX, UITBEG, UITFIN, ULDBEG, ULDFIN
echo. USSBEG USSFIN, USER01, USER02, USER03, USER04, USER05, USER06, USER07, USER08
echo. USER09, USER10, USERCV, USERFX, USEROU, USREFL, USERMAT, USERSWSTRAIN
echo. USERINTER, USERCNPROP USER_TBELASTIC UCNVRG UTIMEINC, USERFLD, USERWEAR, USERCZM
echo. USERMATTH USERTHSTRAIN USERINISTATE
echo.
echo. Enter Carriage Return (Enter^) to Quit.
echo.
echo. Enter a User Programmable Feature Source Filename Without The File Extension. 
echo. For Example: USERMAT or usermat.  The Filename is case insensitive.
echo.

set /P SRCFILE=Enter a User Programmable Feature Source Filename: 
set UPFFILE=%SRCFILE%Lib
if "%SRCFILE%"=="" GOTO :EOF

if not exist %SRCFILE%.F (

   echo.
   echo. %SRCFILE%.F Does Not Exist!
   echo.
   pause
   goto :REPEAT

)

goto :COMPILE

:COMPILE

del /q *.obj %UPFFILE%.lib %UPFFILE%.dll %UPFFILE%.map %UPFFILE%.exp %UPFFILE%ex.def %UPFFILE%.lrf >NUL 2>&1
REM del /q %SRCFILE%.obj %UPFFILE%.lib %UPFFILE%.dll %UPFFILE%.map %UPFFILE%.exp %UPFFILE%ex.def %UPFFILE%.lrf >NUL 2>&1

if "%PLATFORM_DIR%"=="winx64" (
   set SHAREDMACS=/DNOSTDCALL /DARGTRAIL /DPCWIN64_SYS /DPCWINX64_SYS /DPCWINNT_SYS /DCADOE_ANSYS
   set CMACS=/DCURVEFIT_EXPORTS /D_X86=1 /DOS_WIN32 /DWIN32 /D__STDC__ 

   for %%p in ( "*.F" )   do ifort %SHAREDMACS% /D__EFL /DFORTRAN /O2 /fpp /4Yportlib /auto /c /Fo.\ /MD /watch:source %%p >>compile.log  2>&1
   for %%p in ( "*.c" )   do cl    %SHAREDMACS% %CMACS% /O2 /Gy- /EHsc /Zi  /c /W3 /MD %%p >>compile.log 2>&1
@echo off
   for %%p in ( "*.cpp" ) do cl    %SHAREDMACS% %CMACS% /O2 /Gy- /EHsc /Zi  /c /W3 /MD %%p >>compile.log 2>&1

)

if "%PLATFORM_DIR%"=="intel" (
   set SHAREDMACS=/DNOSTDCALL /DARGTRAIL /DPCWINNT_SYS /DCADOE_ANSYS
   set CMACS=/DCURVEFIT_EXPORTS /D_X86=1 /DOS_WIN32 /DWIN32 /D__STDC__

   for %%p in ( "*.F" )   do ifort %SHAREDMACS% /D__EFL /DFORTRAN /O2 /fpp /align:rec4byte /4Yportlib /auto /c /Fo.\ /MD /watch:source %%p >>compile.log  2>&1

   for %%p in ( "*.c" )   do cl    %SHAREDMACS% %CMACS% /O2 /Gy- /EHsc /Zi /Zp4 /c /W3 /MD %%p >>compile.log 2>&1
@echo off

   for %%p in ( "*.cpp" ) do cl    %SHAREDMACS% %CMACS% /O2 /Gy- /EHsc /Zi /Zp4 /c /W3 /MD %%p >>compile.log 2>&1
)

if exist compile.log (

   FINDSTR /I /C:": error" compile.log >compile_error.txt
   if !errorlevel!==0 goto :COMPILEFAIL

)

echo EXPORTS >%UPFFILE%ex.def
echo.  >>%UPFFILE%ex.def
echo %SRCFILE% >>%UPFFILE%ex.def
if /I "%SRCFILE%"=="USERMAT" (

   echo USERMAT1D >>%UPFFILE%ex.def
   echo USERMAT3D >>%UPFFILE%ex.def
   echo USERMATBM >>%UPFFILE%ex.def
   echo USERMATPS >>%UPFFILE%ex.def
)

"%AWP_ROOT180%\ansys\Custom\user\%PLATFORM_DIR%\upcase" %UPFFILE%ex.def

echo -out:%UPFFILE%.dll>%UPFFILE%.lrf
echo -def:%UPFFILE%ex.def>>%UPFFILE%.lrf
echo -dll>>%UPFFILE%.lrf
echo -machine:%MACHINE_TARGET%>>%UPFFILE%.lrf
echo -map>>%UPFFILE%.lrf
echo -manifest:embed>>%UPFFILE%.lrf
echo -defaultlib:ANSYS.lib>>%UPFFILE%.lrf
echo.  >>%UPFFILE%.lrf

REM echo %SRCFILE%.obj>>%UPFFILE%.lrf
echo *.obj>>%UPFFILE%.lrf

link @%UPFFILE%.lrf

echo.
echo.      ************************************************************************
echo.
echo.      %UPFFILE%.dll HAS BEEN SUCCESSFULLY BUILT.
echo.
echo.      Set the environment variable ANS_USER_PATH to the directory where the
echo.      %UPFFILE%.dll resides and run ansys%revn% to use your newly generated
echo.      user shared library.
echo.
echo.      ************************************************************************
echo.
del /q compile_error.txt >NUL 2>&1
pause
goto :REPEAT

:COMPILEFAIL
@echo off
del /q compile_error.txt >NUL 2>&1
echo.
echo.      ************************************************************************************
echo.
echo.          UPF COMPILER ERROR!  CHECK compile.log FOR MORE INFORMATION.
echo.
echo.
echo.      ************************************************************************************
echo.
goto :EOF

:LINKFAIL
echo.
echo.      ************************************************************************
echo.
echo.          UPF LINK ERROR!
echo.
echo.
echo.      ************************************************************************
echo.
goto :EOF

endlocal

