@echo off
echo.

:: provide path to Visual Studio and Intel Fortran directories
:: (update this according to the specific installations on your local machine)
set "pathVC=C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC"
set "pathIF=C:\Program Files (x86)\Intel\Compiler\11.1\048\bin"

echo Initializing amd64 (intel64) instruction set:
echo =============================================
call "%pathVC%\vcvarsall.bat" amd64
call "%pathIF%\ifortvars.bat" intel64

echo.
echo Choose method to compile and link your UPF:
echo ===========================================
echo.
set /P upfType=Enter the UPF type (upf, dll, exe): 
if "%upfType%"=="" goto :eof

echo.
echo Compiling ANSYS user programmable feature:
echo ==========================================
if "%upfType%"=="upf" (
    call ANSUPF
) else if "%upfType%"=="dll" (
    call ANSUSERSHARED
) else if "%upfType%"=="exe" (
    call ANSCUST
)
echo Done compiling user programmable feature.
echo.

goto :eof
