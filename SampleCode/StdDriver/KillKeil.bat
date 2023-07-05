::rd  /S /Q Project\MDKARM(uV5)\Listings 
 
del *.crf /Q /S
del *.o   /Q /S
del *.d   /Q /S
del *.axf /Q /S
del *.htm /Q /S
del *.sct /Q /S
del *.dep /Q /S
 
del *.lst /Q /S
del *.map /Q /S
 
del *.uvguix.*   /Q /S
del *.uvgui.*   /Q /S
del JLinkLog.txt /Q /S
 
del *.iex /s
del *.tra /s
del *.bak /s
del *.ddk /s
del *.edk /s
del *.lnp /s
del *.mpf /s
del *.mpj /s
del *.obj /s
del *.omf /s
::del *.opt /s  ::¤£¤¹??°£JLINKªº?¸m
del *.plg /s
del *.rpt /s
del *.tmp /s
del *.__i /s
del *.i /s

:: IAR C-STAT and C-RUN
:: Comment this out if you use C-Stat or C-Run to compile/analyze your project
del *.ewt /Q /S
del *.ewd /Q /S
del *.eww /Q /S

del *.crun /s
del *.dbgdt	/s
del *.cspy /s
del *.cspy.* /s
del *.xcl /s
del *.dni /s
del *.wsdt /s
del *.wspos /s

exit