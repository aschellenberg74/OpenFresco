@echo off
cls
set revndot=18.0

set PLATFORM_DIR=winx64
set IFORT_PLATFORM=intel64
set BIT_TARGET=64

call "%IFORT_COMPILER15%\bin\compilervars.bat" %IFORT_PLATFORM% vs2012

set INCLUDE=%AWP_ROOT180%\ansys\customize\Include;%INCLUDE%
set LIB=%AWP_ROOT180%\ansys\Custom\Lib\%PLATFORM_DIR%;%LIB%

echo.
echo       **************************************************************
echo.
echo       This is the Mechanical APDL Revision %revndot% ANSCUST batch file.  It is
echo       used to  link User Programmable Features into  versions of the
echo       Mechanical APDL program on Microsoft Windows %BIT_TARGET%-bit systems.
echo.
echo                   ******   IMPORTANT !!!! ******
echo.
echo       The user subroutine source files provided reside
echo       in the folder: {InstallDir}\customize\user 
echo.
echo       Please copy the source you wish to modify from this directory
echo       into your working directory [default {InstallDir}\custom\user\%PLATFORM_DIR%]
echo       to include them in your link.  If you wish to use a working directory other
echo       than the default, copy the ANSCUST.BAT, ansysex.def and ansys.lrf files from
echo       {InstallDir}\custom\user\%PLATFORM_DIR%] to your working directory and run
echo       ANSCUST.BAT from there.
echo.
echo       ******************************************************************
echo.
pause

:ASKAERO
SET AERO=FALSE
echo.
set /P ANSW=Do you want to link the Wind Turbine Aeroelastic library with Mechanical APDL? (Y or N):
if /I "%ANSW%"=="" GOTO :PLEASE
if /I %ANSW%==Y (
   SET AERO=TRUE
   goto :COMPILE
)

if /I %ANSW%==N (
    goto :COMPILE
)

:PLEASE
echo.
echo "Please answer Y or N!"
echo.
goto :ASKAERO

:COMPILE

del /q compile.log compile_error.txt >NUL 2>&1
if exist *.obj del /Q *.obj >NUL 2>&1
if exist ANSYS.exe del /Q ANSYS.exe >NUL 2>&1
if exist ANSYS.exp del /Q ANSYS.exp >NUL 2>&1
if exist ANSYS.lib del /Q ANSYS.lib >NUL 2>&1
if exist ANSYS.map del /Q ANSYS.map >NUL 2>&1


set CUSTMACROS=/DNOSTDCALL /DARGTRAIL /DPCWIN64_SYS /DPCWINX64_SYS /DPCWINNT_SYS /DCADOE_ANSYS 

for %%P in ( "*.F" ) do (

   echo.
   echo. COMPILING FORTRAN FILE %%P
   echo.
:: ifort %CUSTMACROS% /D__EFL /O2 /fpp /4Yportlib /auto /c /Fo.\ /DFORTRAN /MD /watch:source %%P >>compile.log  2>&1
   ifort %CUSTMACROS% /D__EFL /DFORTRAN /O2 /fpp /4Yportlib /auto /c /Fo.\ /MD /watch:source %%P >>compile.log  2>&1

)

for %%P in ( "*.c" ) do (

   echo.
   echo. COMPILING C FILE %%P
   echo.
   cl %CUSTMACROS% /DCURVEFIT_EXPORTS /D_X86=1 /DOS_WIN32 /DWIN32 /D__STDC__ /O2 /Gy- /EHsc /Zi  /c /W3 /MD %%P >>compile.log 2>&1

)

for %%P in ( "*.cpp" ) do (

   echo.
   echo. COMPILING C++ FILE %%P
   echo.
   cl %CUSTMACROS% /DCURVEFIT_EXPORTS /D_X86=1 /DOS_WIN32 /DWIN32 /D__STDC__ /O2 /Gy- /EHsc /Zi  /c /W3 /MD %%P >>compile.log 2>&1

)

if exist compile.log (

   FINDSTR /I /C:": error" compile.log >compile_error.txt
   if !errorlevel!==0 goto :COMPILEFAIL
   del /Q compile_error.txt

)

:LINK

if exist ANSYS.exe del /Q ANSYS.exe
if "%AERO%" == "TRUE" (

   type ansys.lrf >ansys.lrf.sav
   echo "%AWP_ROOT180%\ansys\Custom\User\%PLATFORM_DIR%\Aeroelastic\*.obj">>ansys.lrf
   @echo on
   link @ansys.lrf
   type ansys.lrf.sav >ansys.lrf
   del /q ansys.lrf.sav

) ELSE (

   @echo on
   link @ansys.lrf

)

@echo off
IF %ERRORLEVEL% GEQ 1 goto LINKFAIL

echo.
echo       ************************************************************************
echo.
echo           LINK WAS SUCCESSFUL!
echo.
echo       ************************************************************************
echo.

:COPYDLL
echo.
echo       ****************************************************************************
echo.
echo       THE NEXT QUESTION WILL GIVE YOU THE OPPORTUNITY TO COPY THE NECESSARY
echo       RUNTIME DLLs.  NOTE, THIS ONLY NEEDS TO BE DONE ONCE.
echo       YOU CAN ANSWER "N" FOR ALL SUBSEQUENT INVOCATIONS OF %0.
echo.
echo       ****************************************************************************
echo.
set /P ANSW=Do you want to copy the runtime DLLs? (Y or N):
if /I %ANSW%==Y (
REM   copy /y ..\..\..\bin\%PLATFORM_DIR%\*.dll .
   copy /y "%AWP_ROOT180%\ansys\Bin\%PLATFORM_DIR%\"*.dll .
   goto :END
)
if /I %ANSW%==N (
    goto :END
)
echo.
echo "Please answer Y or N!"
echo.
goto COPYDLL

goto :END

:COMPILEFAIL
@echo off
echo.
echo.      ******************************************************************************
echo.
echo.        COMPILER ERROR!  CHECK compile.log FOR MORE INFORMATION
echo.
echo.
echo.      ******************************************************************************
echo.
goto :END

:LINKFAIL
echo.
echo.      ******************************************************************************
echo.
echo.         LINK ERROR!
echo.
echo.      ******************************************************************************
echo.
goto :END

:END

endlocal
