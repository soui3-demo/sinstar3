#pragma once
#include "../include/module_ref.h"

class CImeModule : public CModuleRef
{
public:
	CImeModule(HINSTANCE hInst, LPCTSTR pszSvrPath);
	~CImeModule(void);

	LPCTSTR GetSvrPath() const{return m_szSvrPath;}

	DWORD GetSysInfoFlags() const {return m_dwSystemInfoFlags;}
	void SetSysInfoFlags(DWORD dwFlags){m_dwSystemInfoFlags = dwFlags;}
protected:
	TCHAR m_szSvrPath[MAX_PATH];
	DWORD m_dwSystemInfoFlags;
	HANDLE m_hMutex;
};

extern CImeModule *theModule;