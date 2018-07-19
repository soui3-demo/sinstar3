#include "stdafx.h"
#include "ImeMgr.h"
#include <shlwapi.h>
#pragma comment(lib,"imm32.lib")

using namespace std;

#define MAX_KEY_LENGTH 255
#define CLSID_STRLEN 38

const TCHAR * CONTROL_SET = _T("ControlSet");


HKL ImeMgr::GetKeyboardLayoutFromFileName(LPCTSTR pszImeName)
{
	HKL hRet = 0;
	int nKL = GetKeyboardLayoutList(0, NULL);
	HKL *pKLList = new HKL[nKL];
	TCHAR szName[MAX_PATH];
	GetKeyboardLayoutList(nKL, pKLList);
	for (int i = 0; i<nKL; i++)
	{
		_tcscpy_s(szName, _T(""));
		ImmGetIMEFileName(pKLList[i], szName, MAX_PATH);
		if (_tcsicmp(szName, pszImeName) == 0)
		{
			hRet = pKLList[i];
			break;
		}
	}
	delete[]pKLList;
	return hRet;
}

//
// 21:58 2010-11-1
// Ren zhijie
// 从注册表中找到ime jp所在位置，得到KLID
//
BOOL ImeMgr::GetImeKLID(HKEY hKey, PCTSTR pszImeFileName, PTSTR pwszKLID, size_t nMaxSize)
{
	BOOL	bFound = FALSE;
	TCHAR	szMsg[256] = _T("");

	TCHAR	achKey[MAX_KEY_LENGTH];		// buffer for subkey name
	DWORD	cbName;						// size of name string 
	TCHAR	szClass[MAX_PATH] = _T("");	// buffer for class name 
	DWORD	cchClassName = MAX_PATH;	// size of class string 
	DWORD	cSubKeys = 0;					// number of subkeys 
	DWORD	cbMaxSubKey;				// longest subkey size 
	DWORD	cchMaxClass;				// longest class string 
	DWORD	cValues;					// number of values for key 
	DWORD	cchMaxValue;				// longest value name 
	DWORD	cbMaxValueData;				// longest value data 
	DWORD	cbSecurityDescriptor;		// size of security descriptor 
	FILETIME ftLastWriteTime;			// last write time 

	DWORD i, retCode;

	// Get the class name and the value count. 
	retCode = RegQueryInfoKey(
		hKey,					// key handle 
		szClass,				// buffer for class name 
		&cchClassName,			// size of class string 
		NULL,					// reserved 
		&cSubKeys,				// number of subkeys 
		&cbMaxSubKey,			// longest subkey size 
		&cchMaxClass,			// longest class string 
		&cValues,				// number of values for this key 
		&cchMaxValue,			// longest value name 
		&cbMaxValueData,		// longest value data 
		&cbSecurityDescriptor,	// security descriptor 
		&ftLastWriteTime);		// last write time 

								// Enumerate the subkeys, until RegEnumKeyEx fails.
	if (cSubKeys != 0)
	{
		for (i = 0; i < cSubKeys; i++)
		{
			cbName = MAX_KEY_LENGTH;
			retCode = RegEnumKeyEx(hKey, i,
				achKey,
				&cbName,
				NULL,
				NULL,
				NULL,
				&ftLastWriteTime);
			if (retCode == ERROR_SUCCESS)
			{
				HKEY hSubKey = NULL;
				retCode = RegOpenKeyEx(hKey, achKey, 0, KEY_READ, &hSubKey);
				if (ERROR_SUCCESS == retCode)
				{
					DWORD dwValueMaxLen = MAX_PATH;
					WCHAR szValue[MAX_PATH] = L"";
					retCode = RegQueryValueEx(hSubKey, _T("IME file"), NULL, NULL, (LPBYTE)szValue, &dwValueMaxLen);
					if (ERROR_SUCCESS == retCode)
					{

						if (_tcsicmp(szValue, pszImeFileName) == 0)
						{
							// found it
							bFound = TRUE;
							if (nMaxSize > cbName)
							{
								_tcscpy_s(pwszKLID, nMaxSize, achKey);
							}
						}
					}
					else
					{
						//_stprintf_s( szMsg, _T("RegQueryValueEx failed, error code: %d\n"), GetLastError());
						//OutputDebugString( szMsg);
					}

					RegCloseKey(hSubKey);
				}
			}

			if (bFound)
			{
				break;
			}
		}
	}

	return bFound;
}

BOOL ImeMgr::AddToPreloadList(PCTSTR pwszKLID)
{
	if (pwszKLID == NULL)
	{
		return TRUE;
	}

	if (_tcslen(pwszKLID) == 0)
	{
		return TRUE;
	}

	HKEY hKPreload = 0;
	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, _T("Keyboard Layout\\preload"), 0, KEY_ALL_ACCESS, &hKPreload))
	{
		map< tstring, unsigned int> mapPreLoad;
		GetKLIDMap(hKPreload, mapPreLoad);

		TCHAR szKLID[256] = _T("");
		_tcscpy_s(szKLID, pwszKLID);
		_tcslwr_s(szKLID);

		if (mapPreLoad.find(szKLID) == mapPreLoad.end())
		{
			vector<tstring> vectorOrder;
			vectorOrder.reserve(mapPreLoad.size() + 1);

			KLIDMapToVector(mapPreLoad, vectorOrder);

			// 新安装的，加入到列表最后
			vectorOrder.push_back(szKLID);

			TCHAR szEntry[32];
			for (unsigned int i = 0; i < vectorOrder.size(); i++)
			{
				_stprintf_s(szEntry, _T("%d"), i + 1);
				RegSetValueEx(hKPreload, szEntry, 0, REG_SZ, (LPBYTE)(vectorOrder[i].c_str()), (vectorOrder[i].length() + 1) * sizeof(TCHAR));
			}
		}

		RegCloseKey(hKPreload);
	}

	return TRUE;
}

BOOL ImeMgr::RemoveFromPreloadList(PCTSTR pwszKLID)
{
	if (pwszKLID == NULL)
	{
		return TRUE;
	}

	if (_tcslen(pwszKLID) == 0)
	{
		return TRUE;
	}

	HKEY hKPreload = 0;
	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, _T("Keyboard Layout\\preload"), 0, KEY_ALL_ACCESS, &hKPreload))
	{
		map< tstring, unsigned int> mapPreLoad;
		GetKLIDMap(hKPreload, mapPreLoad);

		TCHAR szKLID[256] = _T("");
		_tcscpy_s(szKLID, pwszKLID);
		_tcslwr_s(szKLID);

		if (mapPreLoad.find(szKLID) != mapPreLoad.end())
		{
			mapPreLoad.erase(szKLID);

			vector<tstring> vectorOrder;
			vectorOrder.reserve(mapPreLoad.size() + 1);

			KLIDMapToVector(mapPreLoad, vectorOrder);

			//
			// 重新写入
			//
			if (vectorOrder.size() > 0)
			{
				TCHAR szEntry[32];
				for (unsigned int i = 0; i < vectorOrder.size(); i++)
				{
					_stprintf_s(szEntry, _T("%d"), i + 1);
					RegSetValueEx(hKPreload, szEntry, 0, REG_SZ, (LPBYTE)(vectorOrder[i].c_str()), (vectorOrder[i].length() + 1) * sizeof(TCHAR));
				}

				_stprintf_s(szEntry, _T("%d"), vectorOrder.size() + 1);
				RegDeleteValue(hKPreload, szEntry);
			}
			else
			{
				//
				// 如果只有我们一个输入法了，删除我们的，把英文美式键盘加进来。
				//
				TCHAR szValue[32] = _T("00000409");
				RegSetValueEx(hKPreload, _T("1"), 0, REG_SZ, (LPBYTE)(szValue), (_tcslen(szValue) + 1) * sizeof(TCHAR));
			}
		}

		RegCloseKey(hKPreload);
	}

	return TRUE;
}

BOOL ImeMgr::DeleteLayout(PCTSTR pszKeyPath, PCTSTR pwszKLID)
{
	TCHAR szFullPath[256];
	_stprintf_s(szFullPath, _T("%s\\%s"), pszKeyPath, pwszKLID);
	LSTATUS nStatus = SHDeleteKey(HKEY_LOCAL_MACHINE, szFullPath);

	TCHAR szMsg[256];
	_stprintf_s(szMsg, _T("try to delete: %s, result: %d\n"), szFullPath, nStatus);
	return (nStatus == ERROR_SUCCESS);
}

BOOL ImeMgr::DeleteLayout(PCTSTR pwszKLID)
{
	if (pwszKLID == NULL)
	{
		return TRUE;
	}

	if (_tcslen(pwszKLID) == 0)
	{
		return TRUE;
	}

	TCHAR szRegPath[256] = _T("SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts");
	DeleteLayout(szRegPath, pwszKLID);

	HKEY hKey = NULL;
	LONG nRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SYSTEM"), 0, KEY_READ, &hKey);
	if (ERROR_SUCCESS == nRet)
	{
		TCHAR	szMsg[256] = _T("");

		TCHAR	achKey[MAX_KEY_LENGTH];		// buffer for subkey name
		DWORD	cbName;						// size of name string 
		TCHAR	szClass[MAX_PATH] = _T("");	// buffer for class name 
		DWORD	cchClassName = MAX_PATH;	// size of class string 
		DWORD	cSubKeys = 0;					// number of subkeys 
		DWORD	cbMaxSubKey;				// longest subkey size 
		DWORD	cchMaxClass;				// longest class string 
		DWORD	cValues;					// number of values for key 
		DWORD	cchMaxValue;				// longest value name 
		DWORD	cbMaxValueData;				// longest value data 
		DWORD	cbSecurityDescriptor;		// size of security descriptor 
		FILETIME ftLastWriteTime;			// last write time 

		DWORD i, retCode;

		// Get the class name and the value count. 
		retCode = RegQueryInfoKey(
			hKey,					// key handle 
			szClass,				// buffer for class name 
			&cchClassName,			// size of class string 
			NULL,					// reserved 
			&cSubKeys,				// number of subkeys 
			&cbMaxSubKey,			// longest subkey size 
			&cchMaxClass,			// longest class string 
			&cValues,				// number of values for this key 
			&cchMaxValue,			// longest value name 
			&cbMaxValueData,		// longest value data 
			&cbSecurityDescriptor,	// security descriptor 
			&ftLastWriteTime);		// last write time 

									// Enumerate the subkeys, until RegEnumKeyEx fails.
		if (cSubKeys != 0)
		{
			_stprintf_s(szMsg, _T("\nNumber of subkeys: %d\n"), cSubKeys);
			OutputDebugString(szMsg);

			for (i = 0; i < cSubKeys; i++)
			{
				cbName = MAX_KEY_LENGTH;
				retCode = RegEnumKeyEx(hKey, i,
					achKey,
					&cbName,
					NULL,
					NULL,
					NULL,
					&ftLastWriteTime);
				if (retCode == ERROR_SUCCESS)
				{
					_stprintf_s(szMsg, _T("(%d) %s\n"), i + 1, achKey);
					OutputDebugString(szMsg);

					if (_tcsnicmp(achKey, CONTROL_SET, _tcslen(CONTROL_SET)) == 0)
					{
						_stprintf_s(szRegPath, _T("SYSTEM\\%s\\Control\\Keyboard Layouts"), achKey);
						DeleteLayout(szRegPath, pwszKLID);
					}
				}
			}
		}

		RegCloseKey(hKey);
	}

	return TRUE;
}

BOOL ImeMgr::InstalIME(PCTSTR pszIMEFileName, PCTSTR pszDesc)
{
	TCHAR szMsg[256] = _T("");
	TCHAR szFileName[MAX_PATH] = _T("");
	GetSystemDirectory(szFileName, MAX_PATH);
	PathAppend(szFileName, pszIMEFileName);
	if (!ImmInstallIME(szFileName, pszDesc))
	{
		_stprintf_s(szMsg, _T("ImmInstallIME failed, error code: %d\n"), GetLastError());
		OutputDebugString(szMsg);
		return FALSE;
	}

	return TRUE;
}

BOOL ImeMgr::RemoveIME(PCTSTR pszFileName)
{
	TCHAR szKLID[256] = _T("");
	HKEY hKey = NULL;
	LONG nRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts"), 0, KEY_READ, &hKey);
	if (ERROR_SUCCESS == nRet)
	{
		GetImeKLID(hKey, pszFileName, szKLID, 256);
		RegCloseKey(hKey);
	}

	if (_tcslen(szKLID) != NULL)
	{
		RemoveFromPreloadList(szKLID);
		DeleteLayout(szKLID);
	}

	//
	// unload ime
	//
	HKL hkl = GetKeyboardLayoutFromFileName(pszFileName);
	if (hkl != NULL)
	{
		UnloadKeyboardLayout(hkl);
	}

	return TRUE;
}

//+---------------------------------------------------------------------------
//
// RecurseDeleteKey
//
// RecurseDeleteKey is necessary because on NT RegDeleteKey doesn't work if the
// specified key has subkeys
//----------------------------------------------------------------------------

LONG ImeMgr::RecurseDeleteKey(HKEY hParentKey, LPCTSTR lpszKey)
{
	HKEY hKey;
	LONG lRes;
	FILETIME time;
	TCHAR szBuffer[256];
	DWORD dwSize = ARRAYSIZE(szBuffer);

	if (RegOpenKey(hParentKey, lpszKey, &hKey) != ERROR_SUCCESS)
		return ERROR_SUCCESS; // assume it couldn't be opened because it's not there

	lRes = ERROR_SUCCESS;
	while (RegEnumKeyEx(hKey, 0, szBuffer, &dwSize, NULL, NULL, NULL, &time) == ERROR_SUCCESS)
	{
		szBuffer[ARRAYSIZE(szBuffer) - 1] = '\0';
		lRes = RecurseDeleteKey(hKey, szBuffer);
		if (lRes != ERROR_SUCCESS)
			break;
		dwSize = ARRAYSIZE(szBuffer);
	}
	RegCloseKey(hKey);

	return lRes == ERROR_SUCCESS ? RegDeleteKey(hParentKey, lpszKey) : lRes;
}

BOOL ImeMgr::Enable(PCTSTR pszIMEFileName, BOOL bEnable)
{
	//
	// 1. 把ime从preload list中移除
	//
	TCHAR szKLID[256] = _T("");
	HKEY hKey = NULL;
	LONG nRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts"), 0, KEY_READ, &hKey);
	if (ERROR_SUCCESS == nRet)
	{
		GetImeKLID(hKey, pszIMEFileName, szKLID, 256);
		RegCloseKey(hKey);
	}

	if (_tcslen(szKLID) != NULL)
	{
		if (bEnable)
		{
			AddToPreloadList(szKLID);
		}
		else
		{
			RemoveFromPreloadList(szKLID);
		}
	}

	if (!bEnable)
	{
		HKL hRet = 0;
		int nKL = GetKeyboardLayoutList(0, NULL);
		HKL *pKLList = new HKL[nKL];

		GetKeyboardLayoutList(nKL, pKLList);
		for (int i = 0; i<nKL; i++)
		{
			TCHAR szName[256];
			ImmGetIMEFileName(pKLList[i], szName, ARRAYSIZE(szName));

			if (_tcsicmp(szName, pszIMEFileName) == 0)
			{
				UnloadKeyboardLayout(pKLList[i]);
				break;
			}
		}

		delete[]pKLList;
	}

	return TRUE;
}

HKL ImeMgr::Load(PCTSTR pszIMEFileName)
{
	TCHAR szKLID[256] = _T("");
	HKEY hKey = NULL;
	LONG nRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts"), 0, KEY_READ, &hKey);
	if (ERROR_SUCCESS == nRet)
	{
		GetImeKLID(hKey, pszIMEFileName, szKLID, 256);
		RegCloseKey(hKey);
	}

	if (_tcslen(szKLID) == 0)
	{
		return NULL;
	}

	HKL hKL = LoadKeyboardLayout(szKLID, KLF_ACTIVATE | KLF_REPLACELANG | KLF_SETFORPROCESS);
	return hKL;
}

BOOL ImeMgr::Unload(PCTSTR pszIMEFileName)
{
	BOOL bOK = FALSE;
	HKL hkl = GetKeyboardLayoutFromFileName(pszIMEFileName);
	if (hkl != NULL)
	{
		bOK = UnloadKeyboardLayout(hkl);
	}

	return bOK;
}

BOOL ImeMgr::GetPreloadList(vector<tstring>& vectorOrder)
{
	HKEY hKPreload = 0;
	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, _T("Keyboard Layout\\preload"), 0, KEY_ALL_ACCESS, &hKPreload))
	{
		map< tstring, unsigned int> mapPreLoad;
		GetKLIDMap(hKPreload, mapPreLoad);

		vectorOrder.reserve(mapPreLoad.size() + 1);

		KLIDMapToVector(mapPreLoad, vectorOrder);

		RegCloseKey(hKPreload);
	}

	return TRUE;
}

BOOL ImeMgr::GetKLIDMap(HKEY hKPreload, map<tstring, unsigned int>& mapPreLoad)
{
	TCHAR szEntry[32];
	TCHAR szImeIDs[32];
	DWORD dwSizeEntry = sizeof(szEntry) / sizeof(TCHAR);
	DWORD dwSizeID = sizeof(szImeIDs);
	DWORD dwIndex = 0;
	while (RegEnumValue(hKPreload, dwIndex, szEntry, &dwSizeEntry, NULL, NULL, (LPBYTE)szImeIDs, &dwSizeID) == ERROR_SUCCESS)
	{
		_tcslwr_s(szImeIDs);
		if (mapPreLoad.find(szImeIDs) == mapPreLoad.end())
		{
			mapPreLoad[szImeIDs] = _tcstoul(szEntry, NULL, 10);
		}

		dwIndex++;
		dwSizeEntry = sizeof(szEntry) / sizeof(TCHAR);
		dwSizeID = sizeof(szImeIDs);
	}
	return TRUE;
}

BOOL ImeMgr::KLIDMapToVector(map<tstring, unsigned int>& mapPreLoad, vector<tstring>& vectorOrder)
{
	while (mapPreLoad.size() != 0)
	{
		unsigned int nMinValue = (numeric_limits<unsigned int>::max)();
		tstring strKLID;
		for (map< tstring, unsigned int>::iterator i = mapPreLoad.begin(); i != mapPreLoad.end(); i++)
		{
			if (i->second <= nMinValue)
			{
				nMinValue = i->second;
				strKLID = i->first;
			}
		}

		vectorOrder.push_back(strKLID);
		mapPreLoad.erase(strKLID);
	}
	return TRUE;
}
