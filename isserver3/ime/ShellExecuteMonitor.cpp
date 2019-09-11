#include "stdafx.h"
#include "ShellExecuteMonitor.h"
#include <shellapi.h>

CShellExecuteMonitor::CShellExecuteMonitor(SHELLEXECUTEDATA *pData, HWND hMsgRecv)
{
	m_shellExecuteInfo = new SHELLEXECUTEDATA;
	m_shellExecuteInfo->nType = pData->nType;
	m_shellExecuteInfo->strFileName = pData->strFileName;
	m_shellExecuteInfo->strOp = pData->strOp;
	m_hWndRecv = hMsgRecv;
	m_exitCode = -1;
}


CShellExecuteMonitor::~CShellExecuteMonitor()
{
	delete m_shellExecuteInfo;
}


HANDLE CShellExecuteMonitor::ShellExe(LPCTSTR pszOp,LPCTSTR pszFileName)
{
	SHELLEXECUTEINFO ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = pszOp;
	ShExecInfo.lpFile = pszFileName;
	ShExecInfo.lpParameters = NULL;
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_SHOWDEFAULT;
	ShExecInfo.hInstApp = NULL;
	ShellExecuteEx(&ShExecInfo);
	return ShExecInfo.hProcess;
}

static SYSTEMTIME OnTime64toSystemTime(__time64_t& itime)
{
	struct tm *temptm = _localtime64(&itime);
	SYSTEMTIME st = { 1900 + temptm->tm_year,
		1 + temptm->tm_mon,
		temptm->tm_wday,
		temptm->tm_mday,
		temptm->tm_hour,
		temptm->tm_min,
		temptm->tm_sec,
		0 };
	return st;
}

SYSTEMTIME CShellExecuteMonitor::GetFileTime(LPCTSTR pszFileName)
{
	struct _stat64i32 statbuf;
	_tstat64i32(pszFileName, &statbuf);
	return OnTime64toSystemTime(statbuf.st_mtime);
}

UINT CShellExecuteMonitor::Run(LPARAM lp)
{	
	HANDLE hProc = ShellExe(m_shellExecuteInfo->strOp,m_shellExecuteInfo->strFileName);
	if (!hProc)
	{
		PostMessage(m_hWndRecv, UM_PROCESSEXIT, 0, (LPARAM)this);
		return -1;
	}
	SYSTEMTIME time1 = GetFileTime(m_shellExecuteInfo->strFileName);

	HANDLE hWaitObjs[2] = { m_evtStop,hProc };
	for(;;)
	{
		DWORD dwRet = WaitForMultipleObjects(2, hWaitObjs, FALSE, 1000);

		if (dwRet == WAIT_OBJECT_0)
		{//quit thread
			break;
		}
		SYSTEMTIME time2 = GetFileTime(m_shellExecuteInfo->strFileName);
		if (memcmp(&time1, &time2, sizeof(SYSTEMTIME)) != 0)
		{//file changed
			time1 = time2;
			PostMessage(m_hWndRecv, UM_FILEUPDATED, 1, (LPARAM)this);
		}
		if (dwRet == WAIT_OBJECT_0 + 1)
		{//exit
			GetExitCodeProcess(hProc, &m_exitCode);
			PostMessage(m_hWndRecv, UM_PROCESSEXIT, 1, (LPARAM)this);
			break;
		}
	}
	CloseHandle(hProc);
	return 0;
}
