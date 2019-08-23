#pragma once
#include "../include/module_ref.h"

class CImeModule : public CModuleRef
{
public:
	CImeModule(HINSTANCE hInst, LPCTSTR pszSvrPath,LPCTSTR pszInstallPath);
	~CImeModule(void);

	LPCTSTR GetSvrPath() const{return m_szSvrPath;}
	LPCTSTR GetInstallPath() const{return m_szInstallPath;}

	DWORD GetSysInfoFlags() const {return m_dwSystemInfoFlags;}
	void SetSysInfoFlags(DWORD dwFlags){m_dwSystemInfoFlags = dwFlags;}
protected:
	TCHAR m_szSvrPath[MAX_PATH];
	TCHAR	m_szInstallPath[MAX_PATH];

	DWORD m_dwSystemInfoFlags;
	HANDLE m_hMutex;
};

extern CImeModule *theModule;