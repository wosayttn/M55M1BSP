@echo off

rem -----------
rem Run Doxygen
rem -----------

rmdir /s/q html
del /q *.chm
"c:\Program Files\doxygen\doxygen.exe" > doxygen.log 2> doxygen.err

rem -----------------------------------------------------------------------
rem Filter out such warning:
rem		argument 'None' of command @param is not found in the argument list
rem -----------------------------------------------------------------------

SETLOCAL ENABLEDELAYEDEXPANSION
if exist doxygen2.err del /q doxygen2.err

for /f "delims==" %%i in (doxygen.err) do (
	echo %%i | findstr /i "'none'" > nul
	if !errorlevel!==1 echo %%i >> doxygen2.err
)

rmdir /s/q html
