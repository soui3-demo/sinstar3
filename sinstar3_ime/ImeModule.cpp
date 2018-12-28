#include "stdafx.h"
#include "ImeModule.h"
#include "UiWnd.h"
CImeModule::CImeModule(HINSTANCE hInst, LPCTSTR pszSvrPath):CModuleRef(hInst),m_dwSystemInfoFlags(0)
{
	CUiWnd::RegisterClass(hInst);
	_tcscpy(m_szSvrPath, pszSvrPath);
}

CImeModule::~CImeModule(void)
{
	CUiWnd::UnregisterClass(GetModule());
}
