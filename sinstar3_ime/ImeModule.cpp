#include "stdafx.h"
#include "ImeModule.h"
#include "UiWnd.h"
#include "../helper/helper.h"


CImeModule::CImeModule(HINSTANCE hInst, LPCTSTR pszSvrPath,LPCTSTR pszInstallPath):CModuleRef(hInst),m_dwSystemInfoFlags(0)
{
	CUiWnd::RegisterClass(hInst);
	_tcscpy(m_szSvrPath, pszSvrPath);
	_tcscpy(m_szInstallPath,pszInstallPath);
	SECURITY_ATTRIBUTES *psa = Helper_BuildLowIntegritySA();
	m_hMutex = CreateMutex(psa, FALSE, SINSTAR3_MUTEX);
	Helper_FreeSa(psa);
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
