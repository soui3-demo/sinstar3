#pragma once

#include <map>
#include <list>
#include <vector>
#include <string>

typedef std::basic_string<TCHAR> tstring;

class ImeMgr
{
public:
	static HKL GetKeyboardLayoutFromFileName(LPCTSTR pszImeName);

	//
	// 21:58 2010-11-1
	// Ren zhijie
	// 从注册表中找到ime jp所在位置，得到KLID
	//
	static BOOL GetImeKLID(HKEY hKey, PCTSTR pszImeFileName, PTSTR pwszKLID, size_t nMaxSize);

	static BOOL AddToPreloadList(PCTSTR pwszKLID);

	static BOOL RemoveFromPreloadList(PCTSTR pwszKLID);

	static BOOL DeleteLayout(PCTSTR pszKeyPath, PCTSTR pwszKLID);

	static BOOL DeleteLayout(PCTSTR pwszKLID);

	// BAIDU_IME_FILE_NAME, JP_IME_DESC
	static BOOL InstalIME(PCTSTR pszIMEFileName, PCTSTR pszDesc);

	static BOOL RemoveIME(PCTSTR pszFileName);

	//+---------------------------------------------------------------------------
	//
	// RecurseDeleteKey
	//
	// RecurseDeleteKey is necessary because on NT RegDeleteKey doesn't work if the
	// specified key has subkeys
	//----------------------------------------------------------------------------
	static LONG RecurseDeleteKey(HKEY hParentKey, LPCTSTR lpszKey);

	// BAIDU_IME_FILE_NAME
	static BOOL Enable(PCTSTR pszIMEFileName, BOOL bEnable);

	static HKL Load(PCTSTR pszIMEFileName);

	static BOOL Unload(PCTSTR pszIMEFileName);


	static BOOL GetPreloadList(std::vector<tstring>& vectorOrder);

	static BOOL GetKLIDMap(HKEY hKPreload, std::map< tstring, unsigned int>& mapPreLoad);

	static BOOL KLIDMapToVector(std::map< tstring, unsigned int>& mapPreLoad, std::vector<tstring>& vectorOrder);
};

