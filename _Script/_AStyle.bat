@echo on
@echo +=======================================================+
@echo +         Execute Astyle source code prettifier         +
@echo +=======================================================+
@echo off

@SET ASTYLE_BIN="C:\Program Files (x86)\AStyle\bin\AStyle.exe"
@SET ASTYLE_OPTION="AStyle_BSP.txt"

if "%1"=="" (
	echo Usage: AStyle.bat file or folder
	goto END
)

if exist "%1\" (
	%ASTYLE_BIN% --options=%ASTYLE_OPTION% --ascii --recursive "%1\*.c,*.h"
) else (
	%ASTYLE_BIN% --options=%ASTYLE_OPTION% --ascii %1
)

:END
pause
