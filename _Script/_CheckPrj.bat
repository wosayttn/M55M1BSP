@echo off

SETLOCAL ENABLEDELAYEDEXPANSION
if exist CheckPrj.log del CheckPrj.log

rem //-------------------------------------------------
rem  Keil Project Setting check 
rem //-------------------------------------------------

echo Keil Project Setting Check >> CheckPrj.log

for /f "usebackq delims=" %%d in (`"dir ..\SampleCode\*KEIL /ad/b/s"`) do (
rem     echo %%d found >> CheckPrj.log    
     if exist "%%d\Nu_Link_Driver.ini" (
        findstr /m "ChipName=M251" "%%d\Nu_Link_Driver.ini"
        if !errorlevel!==1  echo Oops, %%d\Nu_Link_Driver.ini ChipName M251 setting error. >> CheckPrj.log
    ) else (
        echo Oops, %%d\Nu_Link_Driver.ini does not exist. >> CheckPrj.log
    )  
    
    if exist %%d\*.uvoptx (
        findstr /m "<pMon>Bin.Nu_Link.dll" "%%d\*.uvoptx"
        if !errorlevel!==1  echo Oops, %%d\*.uvoptx Debugger Nu-Link setting error >> CheckPrj.log
    ) else (
        echo Oops, %%d\*.uvoptx does not exist. >> CheckPrj.log
    ) 

rem    if exist %%d\*.uvproj (
rem        findstr /m "<Optim>3</Optim>" "%%d\*.uvproj"
rem         if !errorlevel!==1  echo Oops, %%d\*.uvproj Optimize setting error >> CheckPrj.log
rem    )  
rem    if exist %%d\*.uvproj (
rem        findstr /m "<UpdateFlashBeforeDebugging>0" "%%d\*.uvproj"  
rem         if !errorlevel!==0  echo Oops, %%d\*.uvproj UpdateFlash option is missed>> CheckPrj.log
rem    ) 
    
rem //-------------------------------------------------
rem This batch file is to check the Keil Project Option
rem Option --> Device --> Database(NuMicro Cortex-M Database) + NM1120EC1AE
rem //-------------------------------------------------
    if exist "%%d\*.uvprojx" (
        findstr /m "<Device>ARMCM23" "%%d\*.uvprojx"
        if !errorlevel!==1  echo Oops, %%d\*.uvprojx Device ARMCM23 setting error. >> CheckPrj.log

        findstr /m "<pArmCC>" "%%d\*.uvprojx"
        if !errorlevel!==0  echo Oops, %%d\*.uvprojx does not use default compiler version. >> CheckPrj.log
    ) else (
        echo Oops, %%d\*.uvprojx does not exist. >> CheckPrj.log
    )
    
rem :NEXT
)

rem //--------------------
rem Check IAR folder
rem //--------------------

echo IAR Project Setting Check >> CheckPrj.log

for /f "usebackq delims=" %%d in (`"dir ..\SampleCode\ /ad/b/s"`) do (
    if exist "%%d\*KEIL" (
        if not exist %%d\*IAR (
            echo Oops, %%d IAR project not found. >> CheckPrj.log    
        )
    )   
)

rem //--------------------------
rem Check IAR Project file
rem //--------------------------

for /f "usebackq delims=" %%d in (`"dir ..\SampleCode\*IAR /ad/b/s"`) do (
     if not exist "%%d\*.ewd" (
        echo Oops, %%d\ EWD file missing. >> CheckPrj.log
     ) else (
        findstr /m "Nu-Link_IAR\.dll" "%%d\*.ewd"
        if !errorlevel!==1  echo Oops, %%d\*.ewd Debugger Nu-Link setting error >> CheckPrj.log
     )

     if not exist "%%d\*.ewp" (
         echo Oops, %%d\ EWP file missing. >> CheckPrj.log
     ) else (
         echo %%d\*.ewp /b/s
         for /f "usebackq delims=" %%e in (`"dir "%%d\*.ewp" /b/s"`) do (    
             for /f %%N in ('find /c "Nuvoton M251AE series"^< "%%e"') do set /a cnt=%%N
rem             if !cnt!==2 (
rem                echo Oops, Convert IAR project to remove Debug Project in %%e. >> CheckPrj.log
rem             ) else (
rem                if not !cnt!==1  echo Oops, Might have wrong device error in %%e. >> CheckPrj.log
             if cnt==0 (
                echo Oops, %%d\%%e.ewp Device M251AE setting error. >> CheckPrj.log
             )
         )

        findstr /m "<state>Reset_Handler<" "%%d\*.ewp"
        if !errorlevel!==1  echo Oops, %%d\*.ewp Linker Program Entry setting error. >> CheckPrj.log
    )

    if not exist "%%d\*.eww" (
        echo Oops, %%d\ EWW file missing. >> CheckPrj.log
     )

    if not exist "%%d\*.icf" (
        echo Oops, %%d\ ICF file missing. >> CheckPrj.log
    )
 )

:END
