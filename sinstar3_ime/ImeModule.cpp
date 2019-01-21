#include "stdafx.h"
#include "ImeModule.h"
#include "UiWnd.h"
#include "../ipcobject/include/SecurityAttribute.h"


CImeModule::CImeModule(HINSTANCE hInst, LPCTSTR pszSvrPath):CModuleRef(hInst),m_dwSystemInfoFlags(0)
{
	CUiWnd::RegisterClass(hInst);
	_tcscpy(m_szSvrPath, pszSvrPath);
	SecurityAttribute sa;
	m_hMutex = CreateMutex(sa.get_attr(), FALSE, SINSTAR3_MUTEX);
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
