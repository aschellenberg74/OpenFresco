@echo off
echo.

:: provide path to Visual Studio VC directory
:: (update this according to the specific installation on your local machine)
set "pathVC=C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC"

echo Initializing amd64 (intel64) instruction set:
echo =============================================
call "%pathVC%\vcvarsall.bat" amd64

echo.
echo Compiling socket.c:
echo ===================
call cl.exe socket.c /c /O2 /D_CRT_SECURE_NO_DEPRECATE /DF77_NAME_UPPER /DUSE_FORT_STDCALL /DMIXED_STRING_LEN

goto :eof
