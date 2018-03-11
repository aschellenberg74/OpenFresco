@echo off
echo.

:: provide path to Visual Studio VC directory
:: (update this according to the specific installation on your local machine)
::set "pathVC=C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC"
::set "pathVC=C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build"
set "pathIC=C:\Program Files (x86)\IntelSWTools\compilers_and_libraries\windows\bin"

echo Initializing x64 (intel64) instruction set:
echo ===========================================
::call "%pathVC%\vcvarsall.bat" x64
call "%pathIC%\iclvars.bat" intel64

echo.
echo Compiling tcp_socketf.c:
echo ========================
call del tcp_socketf.log
call del tcp_socketf.obj
::call cl tcp_socketf.c /c /O2 /DUSE_FORT_STDCALL /DF77_NAME_UPPER /DMIXED_STRING_LEN >>socketf.log 2>&1
call icl tcp_socketf.c /c /O2 /DF77_NAME_UPPER >>tcp_socketf.log 2>&1

goto :eof
