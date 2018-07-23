#include "stdafx.h"
#include "Minidump.h"
#include <windows.h>
#include <DbgHelp.h>

#pragma comment(lib, "DbgHelp.lib")

static WCHAR s_dumpFile[MAX_PATH]=L"minidump.dmp";

// 创建Dump文件  
//   
void CreateDumpFile(LPCWSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException)
{
    // 创建Dump文件  
    //  
    HANDLE hDumpFile = CreateFileW(lpstrDumpFilePathName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    // Dump信息  
    //  
    MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
    dumpInfo.ExceptionPointers = pException;
    dumpInfo.ThreadId = GetCurrentThreadId();
    dumpInfo.ClientPointers = FALSE;

    // 写入Dump文件内容  
    //  
    MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);

    CloseHandle(hDumpFile);
}

HMODULE GetCurrentModule(BOOL bRef = FALSE)
{
	HMODULE hModule = NULL;
	if (GetModuleHandleEx(bRef ? GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS : (GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
		| GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT), (LPCWSTR)GetCurrentModule, &hModule))
	{
		return hModule;
	}
	return NULL;
}

template<size_t _size>
bool CreateDumpFileName(wchar_t (&dumpfilename)[_size])
{
	HMODULE hModule = GetCurrentModule();
	if (hModule)
	{
		wchar_t szCurrentDir[MAX_PATH];
		int bRet=GetModuleFileNameW(hModule, szCurrentDir, sizeof(szCurrentDir));
		if (bRet > 0)
		{
			SYSTEMTIME sys;
			GetLocalTime(&sys);
			
			LPWSTR lpInsertPos = wcsrchr(szCurrentDir, _T('\\'));
			wcscpy(lpInsertPos + 1, _T("\0"));
			wsprintf(dumpfilename, _T("%ssinstar%4d%02d%02d%02d%02d%02d%03d.dmp"), szCurrentDir, sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
			return true;
		}
	}
	return false;
}

// 处理Unhandled Exception的回调函数  
//  
LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException)
{
    // 这里弹出一个错误对话框并退出程序  
    //  
    //FatalAppExit(-1, _T("*** Unhandled Exception! ***"));
	wchar_t dmpfilename[MAX_PATH];
	if (CreateDumpFileName(dmpfilename))
	{
		CreateDumpFile(dmpfilename, pException);
	}
	else
	{
		CreateDumpFile(s_dumpFile, pException);
	}
    return EXCEPTION_EXECUTE_HANDLER;
}

void CMinidump::Enable()
{
    // 设置处理Unhandled Exception的回调函数  
    //   
    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
}

void CMinidump::SetDumpFile(LPCWSTR pszDumpFile)
{
	wcscpy_s(s_dumpFile,MAX_PATH,pszDumpFile);
}
