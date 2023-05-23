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

if not exist %outpath% mkdir %outpath%

set /A bitwidth=datawidth*8
rem %~dp0\binToDat.exe %filepath% 16
if exist %filepath%\* (
    echo dir
    xcopy %filepath% %outpath%\%filename% /E /Y /F /I
    del %outpath%\%filename%\*.dat
    for %%f in (%outpath%\%filename%\*) do (
        "%~dp0\binToDat.exe" %%f %datawidth%
        move %%f%bitwidth%.dat %%f-%bitwidth%.dat
    )
) else (
    echo file
    copy %filepath% %outpath%
    "%~dp0\binToDat.exe" %outpath%\%filename% %datawidth%
    move %outpath%\%filename%%bitwidth%.dat %outpath%\%basename%-%bitwidth%.dat
    C:\Keil_v5\ARM\ARMCLANG\bin\fromelf.exe --text -c "%dirname%%basename%.axf" --output "%dirname%%basename%.txt"
)

rem pause
