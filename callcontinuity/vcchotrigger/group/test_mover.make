FINAL :
	echo Copying test DLL to z\sys\bin
	copy %epocroot%epoc32\release\winscw\udeb\t_vcchotrigger.dll %epocroot%epoc32\release\winscw\UDEB\z\sys\bin /y

MAKMAKE FREEZE LIB CLEANLIB RESOURCE RELEASABLES CLEAN BLD SAVESPACE :
