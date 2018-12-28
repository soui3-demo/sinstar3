#include "stdafx.h"
#include "ImeModule.h"
#include "UiWnd.h"
CImeModule::CImeModule(HINSTANCE hInst, LPCTSTR pszDataPath):CModuleRef(hInst),m_dwSystemInfoFlags(0)
{
	CUiWnd::RegisterClass(hInst);
	_tcscpy(m_szDataPath,pszDataPath);
}

CImeModule::~CImeModule(void)
{
	CUiWnd::UnregisterClass(GetModule());
}
