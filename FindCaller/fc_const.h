#pragma once

const WCHAR * KTitle_X64 = L"sinstar3_findcaller_20180807_x64";
const WCHAR * KTitle_X86 = L"sinstar3_findcaller_20180807_x86";

const WCHAR * KFindCallerClass = L"FindCaller";

#ifdef _WIN64
const WCHAR * KTitle = KTitle_X64;
#else
const WCHAR * KTitle = KTitle_X86;
#endif 
