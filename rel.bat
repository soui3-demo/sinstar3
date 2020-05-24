cd /d %~dp0
copy release\iscore.dll /y sinstar3.rel\program\
copy release\sinstar3_ime.dll /y sinstar3.rel\program\
copy release\sinstar3_tsf.dll /y sinstar3.rel\program\
copy release\RegisterCore.dll /y sinstar3.rel\program\
copy release\tipdict.dll /y sinstar3.rel\server\
copy release\isserver3.exe /y sinstar3.rel\program\
copy release\FindCaller.exe /y sinstar3.rel\program\
copy release\FindCallerUI.exe /y sinstar3.rel\program\
copy release\crashreporter.exe /y sinstar3.rel\program\
copy release\register.exe /y sinstar3.rel\
mkdir sinstar3.rel\tools
copy release\CompBuilder.exe /y sinstar3.rel\tools\
copy release\PlEditor.exe /y sinstar3.rel\tools\
mkdir sinstar3.rel\program\x64
copy x64\release\*.dll /y sinstar3.rel\program\x64\
copy x64\release\FindCaller.exe /y sinstar3.rel\program\x64\