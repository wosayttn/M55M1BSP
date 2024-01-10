@echo off

rem -----------
rem Run Doxygen
rem -----------

rmdir /s/q html
del /q *.chm
"c:\Program Files\doxygen\doxygen.exe" > doxygen.log 2> doxygen.err

rem -----------------------------------------------------------------------
rem Filter out some warnings
rem -----------------------------------------------------------------------

findstr /i /v /G:doxygen_ignore.txt doxygen.err > doxygen2.err

rmdir /s/q html
