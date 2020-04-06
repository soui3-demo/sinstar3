#include "stdafx.h"
#include "Minidump.h"
#include <windows.h>
#include <DbgHelp.h>
#include <ShellAPI.h>
#pragma comment(lib, "DbgHelp.lib")

static TCHAR s_dumpFile[MAX_PATH]=_T("minidump.dmp");
static TCHAR s_crashReporter[MAX_PATH]={0};
// 创建Dump文件  
//   
void CreateDumpFile(LPCTSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException)
{
	// 创建Dump文件  
	//  
	HANDLE hDumpFile = CreateFile(lpstrDumpFilePathName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

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
	ShellExecute(NULL,_T("open"),s_crashReporter,NULL,NULL,SW_SHOWNORMAL);
}

// 处理Unhandled Exception的回调函数  
//  
LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException)
{
	// 这里弹出一个错误对话框并退出程序  
	//  
	//FatalAppExit(-1, _T("*** Unhandled Exception! ***"));
	CreateDumpFile(s_dumpFile, pException);
	return EXCEPTION_EXECUTE_HANDLER;
}

void CMinidump::Enable()
{
	// 设置处理Unhandled Exception的回调函数  
	//   
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
}

void CMinidump::SetDumpFile(LPCTSTR pszDumpFile)
{
	_tcscpy_s(s_dumpFile,MAX_PATH,pszDumpFile);
}

void CMinidump::SetCrashReporter(LPCTSTR pszCrashReporter)
{
	_tcscpy_s(s_crashReporter,MAX_PATH,pszCrashReporter);
}
