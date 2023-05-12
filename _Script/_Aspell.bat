@echo on
@echo +=======================================================+
@echo +         Execute Aspell Spelling Check                 +
@echo +=======================================================+
@echo off

@SET ASPELL_PATH="C:\Aspell\bin"
@SET ASPELL_BIN="aspell.exe"
@SET ASPELL_PARAM=check --mode=ccpp --dont-backup

@SET FOLDER=..\Library\Device
for /f "usebackq delims=" %%d in (`"dir %FOLDER%\*.c /b/s"`) do (
    @echo %%d
    pushd %ASPELL_PATH%
    %ASPELL_BIN% %ASPELL_PARAM% "%%d"
    popd
)
for /f "usebackq delims=" %%d in (`"dir %FOLDER%\*.h /b/s"`) do (
    @echo %%d
    pushd %ASPELL_PATH%
    %ASPELL_BIN% %ASPELL_PARAM% "%%d"
    popd
)

@SET FOLDER=..\Library\StdDriver
for /f "usebackq delims=" %%d in (`"dir %FOLDER%\*.c /b/s"`) do (
    @echo %%d
    pushd %ASPELL_PATH%
    %ASPELL_BIN% %ASPELL_PARAM% "%%d"
    popd
)
for /f "usebackq delims=" %%d in (`"dir %FOLDER%\*.h /b/s"`) do (
    @echo %%d
    pushd %ASPELL_PATH%
    %ASPELL_BIN% %ASPELL_PARAM% "%%d"
    popd
)

@SET FOLDER=..\SampleCode\RegBased
for /f "usebackq delims=" %%d in (`"dir %FOLDER%\*.c /b/s"`) do (
    @echo %%d
    pushd %ASPELL_PATH%
    %ASPELL_BIN% %ASPELL_PARAM% "%%d"
    popd
)
for /f "usebackq delims=" %%d in (`"dir %FOLDER%\*.h /b/s"`) do (
    @echo %%d
    pushd %ASPELL_PATH%
    %ASPELL_BIN% %ASPELL_PARAM% "%%d"
    popd
)

@SET FOLDER=..\SampleCode\StdDriver
for /f "usebackq delims=" %%d in (`"dir %FOLDER%\*.c /b/s"`) do (
    @echo %%d
    pushd %ASPELL_PATH%
    %ASPELL_BIN% %ASPELL_PARAM% "%%d"
    popd
)
for /f "usebackq delims=" %%d in (`"dir %FOLDER%\*.h /b/s"`) do (
    @echo %%d
    pushd %ASPELL_PATH%
    %ASPELL_BIN% %ASPELL_PARAM% "%%d"
    popd
)
