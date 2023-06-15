@echo on
@echo +==========================================================================+
@echo +	Usage:  cd to "<BSP folder>/_Script" then execute "cppcheck.bat"
@echo +		    It will check all C/H files in StdDriver and SampleCode folders.
@echo +	Output: check.xml
@echo +==========================================================================+
@echo off

"C:\Program Files\Cppcheck\cppcheck.exe" -j 4 --inline-suppr -UEXCLUDE_CPPCHECK --library="C:\Program Files\Cppcheck\cfg\mfc.cfg" --library="C:\Program Files\Cppcheck\cfg\NuMicro_BSP.cfg" --suppress=syntaxError --suppress=unusedFunction --xml --xml-version=2 --output-file="check.xml" "..\Library\StdDriver" "..\SampleCode"

pause
