@echo off

rem echo %*
set filepath=%1
set datawidth=%2
set outpath=%3
if [%2] == [] set datawidth=16
if [%3] == [] set outpath="release"
for /F "delims=" %%i in (%filepath%) do set dirname=%%~dpi
for /F "delims=" %%i in (%filepath%) do set filename=%%~nxi
for /F "delims=" %%i in (%filepath%) do set basename=%%~ni

rem %~dp0\bin2ascii.exe %filepath% 16
if exist %filepath%\* (
    echo dir
    "%~dp0\binToDat.exe" %filepath%\SPIM.bin
    move %filepath%\SPIM.bin8.dat %filepath%\SPIM.bin
) else (
    echo file
    "%~dp0\..\..\binToDat.exe" %filepath%\SPIM.bin
    move %filepath%\SPIM.bin8.dat %filepath%\SPIM.bin
    C:\Keil_v5\ARM\ARMCLANG\bin\fromelf.exe --text -c "%dirname%%basename%.axf" --output "%dirname%%basename%.txt"
)

rem pause
