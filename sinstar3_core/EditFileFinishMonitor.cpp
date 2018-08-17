#include "stdafx.h"
#include "EditFileFinishMonitor.h"
#include <shellapi.h>

CEditFileFinishMonitor::CEditFileFinishMonitor(EDITFILEINFO *pData, HWND hMsgRecv)
{
	m_editFileInfo = new EDITFILEINFO;
	m_editFileInfo->nType = pData->nType;
	m_editFileInfo->strFileName = pData->strFileName;
	m_hWndRecv = hMsgRecv;
}


CEditFileFinishMonitor::~CEditFileFinishMonitor()
{
	delete m_editFileInfo;
}


static HANDLE OpenFile(LPCTSTR pszFileName)
{
	SHELLEXECUTEINFO ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = _T("open");
	ShExecInfo.lpFile = pszFileName;
	ShExecInfo.lpParameters = NULL;
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_SHOWDEFAULT;
	ShExecInfo.hInstApp = NULL;
	ShellExecuteEx(&ShExecInfo);
	return ShExecInfo.hProcess;
}

UINT CEditFileFinishMonitor::Run()
{
	HANDLE hProc = OpenFile(m_editFileInfo->strFileName);
	if (!hProc)
	{
		PostMessage(m_hWndRecv, UM_EDITFILEFINISH, 0, (LPARAM)this);
		return -1;
	}
	HANDLE hWaitObjs[2] = { m_evtStop,hProc };
	DWORD dwRet = WaitForMultipleObjects(2, hWaitObjs, FALSE, INFINITE);
	if (dwRet == WAIT_OBJECT_0 + 1)
	{
		PostMessage(m_hWndRecv, UM_EDITFILEFINISH, 1, (LPARAM)this);
	}
	CloseHandle(hProc);
	return 0;
}
