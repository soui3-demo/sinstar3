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

UINT CShellExecuteMonitor::Run()
{
	HANDLE hProc = ShellExe(m_shellExecuteInfo->strOp,m_shellExecuteInfo->strFileName);
	if (!hProc)
	{
		PostMessage(m_hWndRecv, UM_PROCESSEXIT, 0, (LPARAM)this);
		return -1;
	}
	HANDLE hWaitObjs[2] = { m_evtStop,hProc };
	DWORD dwRet = WaitForMultipleObjects(2, hWaitObjs, FALSE, INFINITE);
	if (dwRet == WAIT_OBJECT_0 + 1)
	{
		GetExitCodeProcess(hProc, &m_exitCode);
		PostMessage(m_hWndRecv, UM_PROCESSEXIT, 1, (LPARAM)this);
	}
	CloseHandle(hProc);
	return 0;
}
