copy debug\sinstar3_ime.dll /y %Systemroot%\SysWOW64\sinstar3_ime.ime
copy x64\debug\sinstar3_ime.dll /y %Systemroot%\System32\sinstar3_ime.ime
regsvr32 %Systemroot%\System32\sinstar3_ime.ime