@echo off

rem echo %*
set filepath=%1
set outpath=%3
if [%3] == [] set outpath="bin"
for /F "delims=" %%i in (%filepath%) do set dirname=%%~dpi
for /F "delims=" %%i in (%filepath%) do set filename=%%~nxi
for /F "delims=" %%i in (%filepath%) do set basename=%%~ni

if not exist %outpath% mkdir %outpath%

rem %~dp0\binToDat.exe %filepath% 16
if exist %filepath%\* (
    echo dir
    xcopy %filepath% %outpath%\%filename% /E /Y /F /I
    del %outpath%\%filename%\*.dat
    for %%f in (%outpath%\%filename%\*) do (
        "bin2ascii.exe" %outpath%\SPIM.bin 
        move main16.dat %outpath%\SPIM.bin
    )
) else (
    echo file
    copy %filepath% %outpath%
    "bin2ascii.exe" %outpath%\SPIM.bin 
    move main16.dat %outpath%\SPIM.bin 
    C:\Keil_v5\ARM\ARMCLANG\bin\fromelf.exe --text -c "%dirname%%basename%.axf" --output "%dirname%%basename%.txt"
)

rem pause
