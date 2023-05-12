@echo on
@echo +============================================+
@echo        Convert IAR Projects to v6.21 projects                
@echo +============================================+
@echo off
SETLOCAL ENABLEDELAYEDEXPANSION

@set IAR_CONV=IARConv.exe

if exist iar_conv.log del iar_conv.log


set /a total=0
for /f "usebackq" %%d in (`"dir ..\*IAR /ad /b /s"`) do (
    if exist "%%d\*.ewp" (
        for /f "usebackq" %%e in (`"dir %%d\*.ewp /b /s"`) do (    
             set /a total=!total!+1
        )

    )    
)

set /a cnt=0
for /f "usebackq" %%d in (`"dir ..\*IAR /ad /b /s"`) do (
    if exist "%%d\*.ewp" (
        for /f "usebackq" %%e in (`"dir %%d\*.ewp /b /s"`) do (    
             set /a cnt=!cnt!+1
             @echo on
             @echo %%e
             @echo [!cnt! / !total!] Building %%~nxe ... 
             @echo off
             %IAR_CONV% "%%e" >> iar_conv.log
        )
    ) else (
        echo ERROR: IAR project file not found in %%d >> iar_conv.log
    )    
)
