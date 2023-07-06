@echo on
@echo +============================================================================================+
@echo +	Usage:  cd to "<BSP folder>/_Script" then                                            +
@echo +         Execute "cppcheck.bat" to check all C/H files in StdDriver and SampleCode folders. +
@echo +         Execute "cppcheck.bat <file/folder>" to check specified file or folder.            +
@echo +============================================================================================+
@echo off

if "%1"=="" (
    "C:\Program Files\Cppcheck\cppcheck.exe" -j 4 --inline-suppr -UEXCLUDE_CPPCHECK --library="C:\Program Files\Cppcheck\cfg\mfc.cfg" --library="C:\Program Files\Cppcheck\cfg\NuMicro_BSP.cfg" --suppress=syntaxError --suppress=unusedFunction "..\Library\StdDriver" "..\SampleCode"
) else (
    "C:\Program Files\Cppcheck\cppcheck.exe" -j 4 --inline-suppr -UEXCLUDE_CPPCHECK --library="C:\Program Files\Cppcheck\cfg\mfc.cfg" --library="C:\Program Files\Cppcheck\cfg\NuMicro_BSP.cfg" --suppress=syntaxError --suppress=unusedFunction "%1"
)

pause
