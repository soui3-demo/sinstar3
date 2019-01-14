#include "stdafx.h"
#include "ImeModule.h"
#include "UiWnd.h"
CImeModule::CImeModule(HINSTANCE hInst, LPCTSTR pszSvrPath):CModuleRef(hInst),m_dwSystemInfoFlags(0)
{
	CUiWnd::RegisterClass(hInst);
	_tcscpy(m_szSvrPath, pszSvrPath);
	m_hMutex = CreateMutex(NULL, FALSE, SINSTAR3_MUTEX);
	if (!m_hMutex && GetLastError() == ERROR_ACCESS_DENIED)
	{
		m_hMutex = OpenMutex(SYNCHRONIZE, FALSE, SINSTAR3_MUTEX);
	}
}

CImeModule::~CImeModule(void)
{
	if(m_hMutex) CloseHandle(m_hMutex);
	CUiWnd::UnregisterClass(GetModule());
}
