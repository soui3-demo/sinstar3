#pragma once
#include "../include/module_ref.h"

class CImeModule : public CModuleRef
{
public:
	CImeModule(HINSTANCE hInst, LPCTSTR pszDataPath);
	~CImeModule(void);

	LPCTSTR GetDataPath() const{return m_szDataPath;}

	DWORD GetSysInfoFlags() const {return m_dwSystemInfoFlags;}
	void SetSysInfoFlags(DWORD dwFlags){m_dwSystemInfoFlags = dwFlags;}
protected:
	TCHAR m_szDataPath[MAX_PATH];
	DWORD m_dwSystemInfoFlags;
};

extern CImeModule *theModule;