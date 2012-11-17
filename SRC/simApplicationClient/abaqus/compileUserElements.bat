@echo off
echo.

:: provide path to OpenFresco SRC, Visual Studio and Intel Fortran directories
:: (update this according to the specific installations on your local machine)
set "pathSRC=C:\Users\Andreas\Documents\OpenFresco\SourceCode\trunk\SRC"
set "pathVC=C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC"
set "pathIF=C:\Program Files (x86)\Intel\Compiler\11.1\048\bin"

echo Initializing amd64 (intel64) instruction set:
echo =============================================
call "%pathVC%\vcvarsall.bat" amd64
call "%pathIF%\ifortvars.bat" intel64

echo.
echo Compiling socket.c:
echo ===================
call cl.exe "%pathSRC%\simApplicationClient\c\socket.c" /c /O2 /D_CRT_SECURE_NO_DEPRECATE /DF77_NAME_UPPER /DUSE_FORT_STDCALL /DMIXED_STRING_LEN

echo.
echo Compiling socketf.c:
echo ====================
call cl.exe "%pathSRC%\simApplicationClient\fortran\socketf.c" /c /O2 /D_CRT_SECURE_NO_DEPRECATE /DF77_NAME_UPPER /DUSE_FORT_STDCALL /DMIXED_STRING_LEN

echo.
echo Compiling Abaqus user element:
echo ==============================
call ifort genericClient_exp.for /c /O2 /DABQ_WIN86_64 /iface:cref /recursive /nologo /heap-arrays:1
rename genericClient_exp.obj expUserElmt.obj
echo done compiling genericClient_exp.for

echo.
echo Combining files into single element object file:
echo ================================================
call lib /out:genericClient_exp.obj expUserElmt.obj socket.obj socketf.obj
del expUserElmt.obj

echo.
echo Compiling Abaqus user element:
echo ==============================
call ifort genericClient_imp.for /c /O2 /DABQ_WIN86_64 /iface:cref /recursive /nologo /heap-arrays:1
rename genericClient_imp.obj impUserElmt.obj
echo done compiling genericClient_imp.for

echo.
echo Combining files into single element object file:
echo ================================================
call lib /out:genericClient_imp.obj impUserElmt.obj socket.obj socketf.obj
del impUserElmt.obj
del socket.obj
del socketf.obj

goto :eof
