copy data\sinstar3_ime.dll /y %Systemroot%\SysWOW64\sinstar3_ime.ime
copy data\x64\sinstar3_ime.dll /y %Systemroot%\System32\sinstar3_ime.ime
regsvr32 %Systemroot%\System32\sinstar3_ime.ime
rem regsvr32 data\sinstar3_tsf.dll
rem regsvr32 data\x64\sinstar3_tsf.dll
