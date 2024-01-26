py -3 hexdump.py SPIM_DMM_RUN_CODE.hex

".\srec_cat.exe" ..\Release\SPIM_DMM_RUN_CODE.hex -intel -crop 0x00100000 0x002FFFFF -o aprom.hex -intel -obs=16
".\srec_cat.exe" aprom.hex -intel -offset -0x00100000  -o aprom.bin -binary
".\srec_cat.exe" ..\Release\SPIM_DMM_RUN_CODE.hex -intel -crop 0x80000000 0x81ffffff -offset 0x80000000 -o spim_rom.bin -binary

pause
