pushd ..

del vssver.scc /s
del vssver2.scc /s
del *.bak /s
del *.CHHsieh3 /s
del *.dep /s
del .git* /s

@rem del *.bin /s
@rem del *.doc /s
@rem del *.docx /s
@rem del *.txt /s
@rem del *.uvopt /s

del *.tra /s
del *.sfr /s
del *.old /s
del *.new /s
del *"Backup of"* /s
del .\Document\*.jpg /s
del .\Document\*.txt /s
del .\Document\html /s

@echo off
SETLOCAL ENABLEDELAYEDEXPANSION

echo ***********************************************
echo ***** Processing Keil Version Source Code *****
echo ***********************************************

echo Remove Output temprary files
for /f "usebackq delims=" %%d in (`"dir *obj /ad/b/s"`) do (
	echo %%d | findstr FMC_IAP > nul
	if !errorlevel!==1 (
		rd /s/q "%%d"
	) else (
		echo Keep %%d
	)
)
for /f "usebackq delims=" %%d in (`"dir *lst /ad/b/s"`) do rd /s/q "%%d"

echo ***********************************************
echo ***** Processing IAR Version Source Code ******
echo ***********************************************

echo Remove Output temprary files
for /f "usebackq delims=" %%d in (`"dir *Debug /ad/b/s"`) do rd /s/q "%%d"
for /f "usebackq delims=" %%d in (`"dir *Release /ad/b/s"`) do (
	echo %%d | findstr FMC_IAP > nul
	if !errorlevel!==1 (
		rd /s/q "%%d"
	) else (
		echo Keep %%d
	)
)
for /f "usebackq delims=" %%d in (`"dir *setting /ad/b/s"`) do rd /s/q "%%d"

echo *******************************************
echo ***** Remove read-only file attribute *****
echo *******************************************
attrib -R *.* /s

popd
