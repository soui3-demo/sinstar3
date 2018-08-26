#pragma once

#define SINSTART3_WNDCLASS _T("sinstar3_wndclass")

class CCurrentDirSet {
public:
	CCurrentDirSet(LPCTSTR pszNewDir);
	~CCurrentDirSet();
private:
	TCHAR m_szCurDirBackup[1000];
};

class CSouiEnv : public SSingleton<CSouiEnv>
{
public:
	CSouiEnv(HINSTANCE hInst, LPCTSTR pszWorkDir);
	~CSouiEnv(void);

	SApplication * theApp();
	SComMgr * theComMgr();

	LPCTSTR SouiDllDir() const;
private:
	SApplication	* m_theApp;
	SComMgr	*		  m_pComMgr;
	SStringT		  m_strSouiDll;
};
