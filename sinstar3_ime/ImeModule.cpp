#include "stdafx.h"
#include "ImeModule.h"

CImeModule::CImeModule(HINSTANCE hInst, LPCTSTR pszDataPath):CModuleRef(hInst),m_dwSystemInfoFlags(0)
{
	_tcscpy(m_szDataPath,pszDataPath);
}

CImeModule::~CImeModule(void)
{
}
