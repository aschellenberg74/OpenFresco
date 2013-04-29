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
echo Running Abaqus Verification:
echo ============================
call C:\SIMULIA\Abaqus\Commands\abq6121.bat -verify -all -log

goto :eof
