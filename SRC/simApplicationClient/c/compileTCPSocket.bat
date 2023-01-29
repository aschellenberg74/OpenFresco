@echo off
echo.

:: provide path to Visual Studio VC directory
:: (update this according to the specific installation on your local machine)
::set "pathVC=C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC"
::set "pathVC=C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build"
set "pathVC=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build"
::set "pathIC=C:\Program Files (x86)\IntelSWTools\compilers_and_libraries\windows\bin"

echo Initializing x64 (intel64) instruction set:
echo ===========================================
call "%pathVC%\vcvarsall.bat" x64
::call "%pathIC%\iclvars.bat" intel64

echo.
echo Compiling tcp_socket.c:
echo =======================
call del tcp_socket.log
call del tcp_socket.obj
call cl tcp_socket.c /c /O2 /D_WIN32 /D_CRT_SECURE_NO_DEPRECATE >>tcp_socket.log 2>&1
::call icl tcp_socket.c /c /O2 /D_WIN32 /D_CRT_SECURE_NO_DEPRECATE >>tcp_socket.log 2>&1

goto :eof
