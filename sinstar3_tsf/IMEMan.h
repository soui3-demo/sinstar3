#pragma once

#include <imm.h>
#include <msctf.h>
#include <shlwapi.h>
#include <tchar.h>

#include <limits>
#include <string>
#include <map>
#include <list>
#include <vector>

#include "../include/sinstar3_guids.h"

using namespace std;
typedef basic_string<TCHAR> tstring;

#define MAX_KEY_LENGTH 255
#define CONTROL_SET	_T("ControlSet")

#define CLSID_STRLEN 38


#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "Imm32.lib")

#define LOTP_INPUTPROCESSOR 1
#define LOTP_KEYBOARDLAYOUT 2

typedef struct tagLAYOUTORTIPPROFILE {
	DWORD  dwProfileType;       // InputProcessor or HKL 
	LANGID langid;              // language id 
	CLSID  clsid;               // CLSID of tip 
	GUID   guidProfile;         // profile description 
	GUID   catid;               // category of tip 
	DWORD  dwSubstituteLayout;  // substitute hkl 
	DWORD  dwFlags;             // Flags 
	WCHAR  szId[MAX_PATH];      // KLID or TIP profile for string 
} LAYOUTORTIPPROFILE;

typedef UINT (WINAPI *PTF_ENUMENABLEDLAYOUTORTIP)(
	LPCWSTR pszUserReg,
	LPCWSTR pszSystemReg,
	LPCWSTR pszSoftwareReg,
	LAYOUTORTIPPROFILE *pLayoutOrTipProfile,
	UINT uBufLength);

typedef HRESULT (WINAPI *PTF_SETDEFAULTLAYOUTORTIP)(LPCWSTR psz,DWORD dwOp);

class CIMEMan
{
public:
	CIMEMan(void);
	~CIMEMan(void);

	static HKL GetKeyboardLayoutFromFileName( LPCTSTR pszImeName) 
	{ 
		HKL hRet=0;
		int nKL=GetKeyboardLayoutList(0,NULL);
		HKL *pKLList=new HKL[nKL];
		TCHAR szName[MAX_PATH];
		GetKeyboardLayoutList(nKL,pKLList);
		for(int i=0;i<nKL;i++)
		{
			_tcscpy_s( szName, _T(""));
			ImmGetIMEFileName(pKLList[i],szName,MAX_PATH);
			if(_tcsicmp(szName,pszImeName)==0)
			{
				hRet=pKLList[i];
				break;
			}
		}
		delete []pKLList;
		return hRet;
	}

	//
	// 从注册表中找到ime所在位置，得到KLID
	//
	static BOOL GetImeKLID( HKEY hKey, PCTSTR pszImeFileName, PTSTR pwszKLID, size_t nMaxSize) 
	{ 
		BOOL	bFound = FALSE;
		TCHAR	szMsg[256] = _T("");

		TCHAR	achKey[MAX_KEY_LENGTH];		// buffer for subkey name
		DWORD	cbName;						// size of name string 
		TCHAR	szClass[MAX_PATH] = _T("");	// buffer for class name 
		DWORD	cchClassName = MAX_PATH;	// size of class string 
		DWORD	cSubKeys=0;					// number of subkeys 
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
		if ( cSubKeys != 0)
		{
			//_stprintf_s( szMsg, _T("\nNumber of subkeys: %d\n"), cSubKeys);
			//OutputDebugString( szMsg);

			for ( i = 0; i < cSubKeys; i++) 
			{ 
				cbName = MAX_KEY_LENGTH;
				retCode = RegEnumKeyEx(hKey, i,
					achKey, 
					&cbName, 
					NULL, 
					NULL, 
					NULL, 
					&ftLastWriteTime); 
				if ( retCode == ERROR_SUCCESS) 
				{
					//_stprintf_s( szMsg, _T("(%d) %s\n"), i+1, achKey);
					//OutputDebugString( szMsg);

					HKEY hSubKey = NULL;
					retCode = RegOpenKeyEx( hKey, achKey, 0, KEY_READ, &hSubKey);
					if ( ERROR_SUCCESS == retCode)
					{
						DWORD dwValueMaxLen = MAX_PATH;
						WCHAR szValue[MAX_PATH] = L"";
						retCode = RegQueryValueEx( hSubKey, _T("IME file"), NULL, NULL, (LPBYTE)szValue, &dwValueMaxLen);
						if ( ERROR_SUCCESS == retCode)
						{
							//_stprintf_s( szMsg, _T("value: %s\n"), szValue);
							//OutputDebugString( szMsg);

							if ( _tcsicmp( szValue, pszImeFileName) == 0)
							{
								// found it
								bFound = TRUE;
								if ( nMaxSize > cbName)
								{
									_tcscpy_s( pwszKLID, nMaxSize, achKey);
								}
							}
						}
						else
						{
							//_stprintf_s( szMsg, _T("RegQueryValueEx failed, error code: %d\n"), GetLastError());
							//OutputDebugString( szMsg);
						}

						RegCloseKey( hSubKey);
					}
				}

				if ( bFound)
				{
					break;
				}
			}
		}

		return bFound;
	}

	static BOOL AddToPreloadList( PCTSTR pwszKLID)
	{
		if ( pwszKLID == NULL)
		{
			return TRUE;
		}

		if ( _tcslen( pwszKLID) == 0)
		{
			return TRUE;
		}

		HKEY hKPreload = 0;
		if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_CURRENT_USER, _T("Keyboard Layout\\preload"), 0, KEY_ALL_ACCESS, &hKPreload))
		{		
			map< tstring, unsigned int> mapPreLoad;
			GetKLIDMap( hKPreload, mapPreLoad);

			TCHAR szKLID[256] = _T("");
			_tcscpy_s( szKLID, pwszKLID);
			_tcslwr_s( szKLID);

			if ( mapPreLoad.find( szKLID) == mapPreLoad.end())
			{
				vector<tstring> vectorOrder;
				vectorOrder.reserve( mapPreLoad.size() + 1);			

				KLIDMapToVector( mapPreLoad, vectorOrder);

				// 新安装的，加入到列表最后
				vectorOrder.push_back( szKLID);

				TCHAR szEntry[32];
				for( unsigned int i = 0; i < vectorOrder.size(); i++)
				{				
					_stprintf_s(szEntry,_T("%d"),i+1);
					RegSetValueEx( hKPreload, szEntry, 0, REG_SZ,(LPBYTE)(vectorOrder[i].c_str()), (DWORD)(vectorOrder[i].length() + 1) * sizeof(TCHAR));
				}
			}

			RegCloseKey(hKPreload);
		}

		return TRUE;
	}

	static BOOL RemoveFromPreloadList( PCTSTR pwszKLID)
	{
		if ( pwszKLID == NULL)
		{
			return TRUE;
		}

		if ( _tcslen( pwszKLID) == 0)
		{
			return TRUE;
		}

		HKEY hKPreload = 0;
		if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_CURRENT_USER, _T("Keyboard Layout\\preload"), 0, KEY_ALL_ACCESS, &hKPreload))
		{		
			map< tstring, unsigned int> mapPreLoad;
			GetKLIDMap( hKPreload, mapPreLoad);

			TCHAR szKLID[256] = _T("");
			_tcscpy_s( szKLID, pwszKLID);
			_tcslwr_s( szKLID);

			if ( mapPreLoad.find( szKLID) != mapPreLoad.end())
			{
				mapPreLoad.erase( szKLID);

				vector<tstring> vectorOrder;
				vectorOrder.reserve( mapPreLoad.size() + 1);

				KLIDMapToVector( mapPreLoad, vectorOrder);

				//
				// 重新写入
				//
				if ( vectorOrder.size() > 0)
				{
					TCHAR szEntry[32];
					for( unsigned int i = 0; i < vectorOrder.size(); i++)
					{				
						_stprintf_s(szEntry,_T("%d"),i+1);
						RegSetValueEx( hKPreload, szEntry, 0, REG_SZ,(LPBYTE)(vectorOrder[i].c_str()), (DWORD)(vectorOrder[i].length() + 1) * sizeof(TCHAR));
					}

					//for( int i = 0; i < dwIndex - 1; i++)
					//{
					//	_stprintf_s(szEntry,_T("%d"),i+1);
					//	RegSetValueEx( hKPreload, szEntry, 0, REG_SZ, (LPBYTE)pInfo[i], 9*sizeof(TCHAR));
					//}
					_stprintf_s( szEntry,_T("%d"), vectorOrder.size()+1);
					RegDeleteValue( hKPreload, szEntry);
				}
				else
				{
					//
					// 如果只有我们一个输入法了，删除我们的，把英文美式键盘加进来。
					//
					TCHAR szValue[32] = _T("00000409");
					RegSetValueEx( hKPreload, _T("1"), 0, REG_SZ, (LPBYTE)(szValue), (DWORD)(_tcslen(szValue) + 1) * sizeof(TCHAR));
				}
			}

			RegCloseKey(hKPreload);
		}

		return TRUE;
	}

	static BOOL DeleteLayout( PCTSTR pszKeyPath, PCTSTR pwszKLID)
	{
		TCHAR szFullPath[256];
		_stprintf_s( szFullPath, _T("%s\\%s"), pszKeyPath, pwszKLID);
		LSTATUS nStatus = SHDeleteKey ( HKEY_LOCAL_MACHINE, szFullPath);

		TCHAR szMsg[256];
		_stprintf_s( szMsg, _T("try to delete: %s, result: %d\n"), szFullPath, nStatus);
		return (nStatus == ERROR_SUCCESS);
	}

	static BOOL DeleteLayout( PCTSTR pwszKLID)
	{
		if ( pwszKLID == NULL)
		{
			return TRUE;
		}

		if ( _tcslen( pwszKLID) == 0)
		{
			return TRUE;
		}

		TCHAR szRegPath[256] = _T("SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts");
		DeleteLayout( szRegPath, pwszKLID);

		HKEY hKey = NULL;
		LONG nRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE, _T("SYSTEM"), 0, KEY_READ, &hKey);
		if ( ERROR_SUCCESS == nRet)
		{
			TCHAR	szMsg[256] = _T("");

			TCHAR	achKey[MAX_KEY_LENGTH];		// buffer for subkey name
			DWORD	cbName;						// size of name string 
			TCHAR	szClass[MAX_PATH] = _T("");	// buffer for class name 
			DWORD	cchClassName = MAX_PATH;	// size of class string 
			DWORD	cSubKeys=0;					// number of subkeys 
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
			if ( cSubKeys != 0)
			{
				_stprintf_s( szMsg, _T("\nNumber of subkeys: %d\n"), cSubKeys);
				OutputDebugString( szMsg);

				for ( i = 0; i < cSubKeys; i++) 
				{ 
					cbName = MAX_KEY_LENGTH;
					retCode = RegEnumKeyEx(hKey, i,
						achKey, 
						&cbName, 
						NULL, 
						NULL, 
						NULL, 
						&ftLastWriteTime); 
					if ( retCode == ERROR_SUCCESS) 
					{
						_stprintf_s( szMsg, _T("(%d) %s\n"), i+1, achKey);
						OutputDebugString( szMsg);

						if ( _tcsnicmp( achKey, CONTROL_SET, _tcslen( CONTROL_SET)) == 0)
						{
							_stprintf_s( szRegPath, _T("SYSTEM\\%s\\Control\\Keyboard Layouts"), achKey);
							DeleteLayout( szRegPath, pwszKLID);					
						}
					}
				}
			}

			RegCloseKey( hKey);
		}

		return TRUE;
	}

	// BAIDU_IME_FILE_NAME, JP_IME_DESC
	static BOOL InstalIME( PCTSTR pszIMEFileName, PCTSTR pszDesc)
	{
		TCHAR szMsg[256] = _T("");
		TCHAR szFileName[MAX_PATH] = _T("");
		GetSystemDirectory( szFileName, MAX_PATH);
		PathAppend( szFileName, pszIMEFileName);
		if ( !ImmInstallIME( szFileName, pszDesc))
		{
			_stprintf_s( szMsg, _T("ImmInstallIME failed, error code: %d\n"), GetLastError());
			OutputDebugString( szMsg);
			return FALSE;
		}

		return TRUE;
	}

	static BOOL RemoveIME( PCTSTR pszFileName)
	{	
		TCHAR szKLID[256] = _T("");
		HKEY hKey = NULL;
		LONG nRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts"), 0, KEY_READ, &hKey);
		if ( ERROR_SUCCESS == nRet)
		{
			GetImeKLID( hKey, pszFileName, szKLID, 256);
			RegCloseKey( hKey);
		}	

		if ( _tcslen( szKLID) != NULL)
		{
			RemoveFromPreloadList( szKLID);
			DeleteLayout( szKLID);
		}

		//
		// unload ime
		//
		HKL hBaiduIme = GetKeyboardLayoutFromFileName( pszFileName);
		if ( hBaiduIme != NULL)
		{
			UnloadKeyboardLayout( hBaiduIme);
		}

		return TRUE;
	}

	static BOOL RemoveTSF( PCTSTR pszFileName)
	{
		typedef HRESULT (STDAPICALLTYPE *PFNREG)(void);
		HINSTANCE hDLL = LoadLibrary( pszFileName);
		if ( hDLL != NULL)
		{
			PFNREG pfnDllRegisterServer = (PFNREG)GetProcAddress(hDLL, "DllUnregisterServer");
			if ( pfnDllRegisterServer != NULL)
			{
				HRESULT hr = pfnDllRegisterServer();
				TCHAR szMsg[512] = _T("");
				_stprintf_s( szMsg, _T("\"%s\" DllUnregisterServer return %x\n"), pszFileName, hr);
				OutputDebugString( szMsg);
			}
			FreeLibrary( hDLL);
		}

		return TRUE;
	}

	static BOOL CLSIDToString(REFGUID refGUID, TCHAR *pch)
	{
		static const BYTE GuidMap[] = {3, 2, 1, 0, '-', 5, 4, '-', 7, 6, '-',
			8, 9, '-', 10, 11, 12, 13, 14, 15};

		static const TCHAR szDigits[] = _T("0123456789ABCDEF");

		int i;
		TCHAR *p = pch;

		const BYTE * pBytes = (const BYTE *) &refGUID;

		*p++ =_T('{');
		for (i = 0; i < sizeof(GuidMap); i++)
		{
			if (GuidMap[i] == '-')
			{
				*p++ = _T('-');
			}
			else
			{
				*p++ = szDigits[ (pBytes[GuidMap[i]] & 0xF0) >> 4 ];
				*p++ = szDigits[ (pBytes[GuidMap[i]] & 0x0F) ];
			}
		}

		*p++ =_T('}');
		*p   =0;

		return TRUE;
	}

	//+---------------------------------------------------------------------------
	//
	// RecurseDeleteKey
	//
	// RecurseDeleteKey is necessary because on NT RegDeleteKey doesn't work if the
	// specified key has subkeys
	//----------------------------------------------------------------------------
	static LONG RecurseDeleteKey(HKEY hParentKey, LPCTSTR lpszKey)
	{
		HKEY hKey;
		LONG lRes;
		FILETIME time;
		TCHAR szBuffer[256];
		DWORD dwSize = ARRAYSIZE(szBuffer);

		if (RegOpenKey(hParentKey, lpszKey, &hKey) != ERROR_SUCCESS)
			return ERROR_SUCCESS; // assume it couldn't be opened because it's not there

		lRes = ERROR_SUCCESS;
		while (RegEnumKeyEx(hKey, 0, szBuffer, &dwSize, NULL, NULL, NULL, &time)==ERROR_SUCCESS)
		{
			szBuffer[ARRAYSIZE(szBuffer)-1] = '\0';
			lRes = RecurseDeleteKey(hKey, szBuffer);
			if (lRes != ERROR_SUCCESS)
				break;
			dwSize = ARRAYSIZE(szBuffer);
		}
		RegCloseKey(hKey);

		return lRes == ERROR_SUCCESS ? RegDeleteKey(hParentKey, lpszKey) : lRes;
	}

	static void UnregisterProfiles( REFCLSID rclsid)
	{
		ITfInputProcessorProfiles *pInputProcessProfiles;
		HRESULT hr;

		hr = CoCreateInstance(CLSID_TF_InputProcessorProfiles, NULL, CLSCTX_INPROC_SERVER,
			IID_ITfInputProcessorProfiles, (void**)&pInputProcessProfiles);

		if (hr != S_OK)
			return;

		pInputProcessProfiles->Unregister( rclsid);
		pInputProcessProfiles->Release();
	}

	//+---------------------------------------------------------------------------
	//
	//  UnregisterCategories
	//
	//----------------------------------------------------------------------------

	static void UnregisterCategories( REFCLSID rclsid)
	{
		ITfCategoryMgr *pCategoryMgr;
		HRESULT hr;

		hr = CoCreateInstance(CLSID_TF_CategoryMgr, NULL, CLSCTX_INPROC_SERVER, 
			IID_ITfCategoryMgr, (void**)&pCategoryMgr);

		if (hr != S_OK)
			return;

		//
		// unregister this text service from GUID_TFCAT_TIP_KEYBOARD category.
		//
		pCategoryMgr->UnregisterCategory(rclsid,
			GUID_TFCAT_TIP_KEYBOARD, 
			rclsid);

		//
		// unregister this text service from GUID_TFCAT_DISPLAYATTRIBUTEPROVIDER category.
		//
		pCategoryMgr->UnregisterCategory(rclsid,
			GUID_TFCAT_DISPLAYATTRIBUTEPROVIDER, 
			rclsid);

		pCategoryMgr->Release();
		return;
	}

	//+---------------------------------------------------------------------------
	//
	//  UnregisterServer
	//
	//----------------------------------------------------------------------------

	static void UnregisterServer( REFCLSID rclsid)
	{
		static const TCHAR c_szInfoKeyPrefix[] = TEXT("CLSID\\");

		TCHAR achIMEKey[ARRAYSIZE(c_szInfoKeyPrefix) + CLSID_STRLEN];

		if (!CLSIDToString( rclsid, achIMEKey + ARRAYSIZE(c_szInfoKeyPrefix) - 1))
			return;
		memcpy(achIMEKey, c_szInfoKeyPrefix, sizeof(c_szInfoKeyPrefix)-sizeof(TCHAR));

		RecurseDeleteKey(HKEY_CLASSES_ROOT, achIMEKey);
	}

	static BOOL RemoveTSF( REFCLSID rclsid)
	{
		UnregisterProfiles( rclsid);
		UnregisterCategories( rclsid);
		UnregisterServer( rclsid);
		return TRUE;
	}

	// BAIDU_IME_FILE_NAME, c_clsidBaidujpTSF, c_guidProfile
	static BOOL IsDefaultIme( PCTSTR pszIMEFileName, REFCLSID rclsid, REFGUID guidProfile)
	{
		if( IsVistaOrAbove())
		{
			HMODULE hInputDLL = LoadLibrary(TEXT("input.dll"));
			if( hInputDLL == NULL)
			{
				return FALSE;
			}

			//
			// enum enabled layout
			//
			UINT nNum = 0;
			LAYOUTORTIPPROFILE* pProfile = NULL;

			PTF_ENUMENABLEDLAYOUTORTIP pfnEnumEnabledLayoutOrTip = (PTF_ENUMENABLEDLAYOUTORTIP)GetProcAddress(hInputDLL, "EnumEnabledLayoutOrTip");
			if( pfnEnumEnabledLayoutOrTip != NULL)
			{
				nNum = pfnEnumEnabledLayoutOrTip( NULL, NULL, NULL, NULL, 0);
				if ( nNum > 0)
				{
					pProfile = (LAYOUTORTIPPROFILE*)malloc( sizeof( LAYOUTORTIPPROFILE) * nNum);
					if ( pProfile != NULL)
					{
						nNum = pfnEnumEnabledLayoutOrTip( NULL, NULL, NULL, pProfile, nNum);
					}
				}
			}

			BOOL bIsDefaultIME = FALSE;
			LANGID idLang = TEXTSERVICE_LANGID;
			for ( UINT i = 0; i < nNum; i++)
			{
				//
				// 找到了我们的tip
				//
				if ( idLang == pProfile[i].langid && IsEqualGUID( pProfile[i].clsid, rclsid) && IsEqualGUID( pProfile[i].guidProfile, guidProfile))
				{
					if ( pProfile[i].dwFlags == 1)
					{
						bIsDefaultIME = TRUE;
					}
					break;
				}
			}

			if ( pProfile != NULL)
			{
				free ( pProfile);
			}

			FreeLibrary( hInputDLL);

			return bIsDefaultIME;
		}
		else
		{
			HKL hDefIME = 0;
			SystemParametersInfo(SPI_GETDEFAULTINPUTLANG, 0, &hDefIME, 0);
			HKL hKlBaidu = GetKeyboardLayoutFromFileName( pszIMEFileName);
			if( hKlBaidu == hDefIME)
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
	}

	static BOOL SetDefaultImeVista( REFCLSID rclsid, REFGUID guidProfile, BOOL bSet)
	{
		TCHAR szMsg[512];

		BOOL bRet = FALSE;

		HMODULE hInputDLL = LoadLibrary(TEXT("input.dll"));
		if( hInputDLL == NULL)
		{
			return FALSE;
		}

		// swprintf(wsLang,L"0x%04X:0x%08X",((DWORD)hKlDef)&0x0000FFFF,(DWORD)hKlDef);

		//
		// The string format of the layout list is:
		// <LangID 1>:<KLID 1>;[...<LangID N>:<KLID N> 
		// for example:
		// 0804:00000804
		//

		// The string format of the text service profile list is:
		// <LangID 1>:{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}; 
		// for example:
		//  0411:{8AEF41B2-882A-444E-8FAB-0D4189BDC80C}{2DBC159B-424B-417A-906D-BAE4B0A6967E}
		//		

		//
		// 读取保存在配置文件中上个默认输入法的ID(可能为空)
		//
		TCHAR szSavedID[256] = _T("");
		//TCHAR szIniFile[MAX_PATH] = _T("");
		//GetModuleFileName( NULL, szIniFile, sizeof(szIniFile) / sizeof(TCHAR));
		//PathRemoveFileSpec( szIniFile);
		//PathAppend( szIniFile, _T("setting.ini"));
		//CImeUtil::Comm_GetIniPath( szIniFile, ARRAYSIZE(szIniFile));		
		//GetPrivateProfileString( _T("ime_sys_setting"), _T("default_ime"), _T(""), szSavedID, sizeof( szSavedID) / sizeof(TCHAR), szIniFile);

		//
		// enum enabled layout
		//
		UINT nNum = 0;
		LAYOUTORTIPPROFILE* pProfile = NULL;

		PTF_ENUMENABLEDLAYOUTORTIP pfnEnumEnabledLayoutOrTip = (PTF_ENUMENABLEDLAYOUTORTIP)GetProcAddress(hInputDLL, "EnumEnabledLayoutOrTip");
		if( pfnEnumEnabledLayoutOrTip != NULL)
		{
			nNum = pfnEnumEnabledLayoutOrTip( NULL, NULL, NULL, NULL, 0);
			if ( nNum > 0)
			{
				pProfile = (LAYOUTORTIPPROFILE*)malloc( sizeof( LAYOUTORTIPPROFILE) * nNum);
				if ( pProfile != NULL)
				{
					nNum = pfnEnumEnabledLayoutOrTip( NULL, NULL, NULL, pProfile, nNum);
				}
			}
		}

		//
		// 找到我们的输入法ID，不要用合成的办法
		//
		TCHAR szID[512] = _T("");
		LANGID idLang = TEXTSERVICE_LANGID;
		for ( UINT i = 0; i < nNum; i++)
		{
			if ( idLang == pProfile[i].langid && IsEqualGUID( pProfile[i].clsid, rclsid) && IsEqualGUID( pProfile[i].guidProfile, guidProfile))
			{
				_tcscpy_s( szID, pProfile[i].szId);
				break;
			}
		}

		_stprintf_s( szMsg, _T("our ime is the %s\n"), szID);
		OutputDebugString( szMsg);

		if ( _tcslen( szID) != 0)
		{
			BOOL bIsDefault = FALSE;
			BOOL bSavedValid = FALSE;

			TCHAR szFirstIme[256] = _T("");
			TCHAR szFirstJPIme[256] = _T("");

			for ( UINT i = 0; i < nNum; i++)
			{
				_stprintf_s( szMsg, _T("dwProfileType: %d, dwFlags: %d, szId: %s\n"), 
					pProfile[i].dwProfileType, pProfile[i].dwFlags, pProfile[i].szId);
				OutputDebugString( szMsg);

				if ( pProfile[i].dwFlags == 0)
				{
					if ( _tcslen( szFirstIme) == 0)
					{
						_tcscpy_s( szFirstIme, pProfile[i].szId);
					}

					if ( pProfile[i].langid == idLang && _tcslen( szFirstJPIme) == 0)
					{
						_tcscpy_s( szFirstJPIme, pProfile[i].szId);
					}
				}

				//
				// 找到现在的默认输入法
				//.
				if ( pProfile[i].dwFlags == 1)
				{
					_stprintf_s( szMsg, _T("default ime is the %s\n"), pProfile[i].szId);
					OutputDebugString( szMsg);

					if ( _tcsicmp( szID, pProfile[i].szId) == 0)
					{
						_stprintf_s( szMsg, _T("our ime is the default ime\n"));
						OutputDebugString( szMsg);

						bIsDefault = TRUE;					
					}
					else
					{
						//
						// 要设置我们的输入法为默认的输入法，先保存原来的默认输入法
						//
						if ( bSet)
						{
							//WritePrivateProfileString( _T("ime_sys_setting"), _T("default_ime"), pProfile[i].szId, szIniFile);
							break;
						}
					}
				}

				if ( _tcsicmp( szSavedID, pProfile[i].szId) == 0)
				{
					bSavedValid = TRUE;
				}
			}		

			if ( bSet)
			{
				if( bIsDefault)
				{
					//
					// 要设置我们的输入法为默认输入法，如果它已经是默认输入法了，退出。
					//
					return TRUE;
				}
			}		
			else
			{
				if ( bIsDefault)
				{
					if ( bSavedValid)
					{
						_tcscpy_s( szID, szSavedID);
					}
					else
					{
						if ( _tcslen( szFirstJPIme) > 0)
						{
							_tcscpy_s( szID, szFirstJPIme);
						}
						else if ( _tcslen( szFirstIme) > 0)
						{
							_tcscpy_s( szID, szFirstIme);
						}
						else
						{
							return FALSE;
						}
					}
				}
				else
				{
					//
					// 要取消我们的输入法为默认输入法，如果它已经不是默认输入法了，退出。
					//
					return TRUE;
				}
			}

			PTF_SETDEFAULTLAYOUTORTIP pfnSetDefaultLayoutOrTip = (PTF_SETDEFAULTLAYOUTORTIP)GetProcAddress(hInputDLL, "SetDefaultLayoutOrTip");
			if( pfnSetDefaultLayoutOrTip != NULL)
			{
				bRet = pfnSetDefaultLayoutOrTip( szID, 0);
				_stprintf_s( szMsg, _T("set %s to default ime, result: %d\n"), szID, bRet);
				OutputDebugString( szMsg);
			}
		}

		if ( pProfile != NULL)
		{
			free ( pProfile);
		}

		FreeLibrary( hInputDLL);

		return bRet;
	}

	static BOOL ChangeDefaultIME(PCTSTR pszKLID)
	{
		TCHAR szMsg[256] = _T("");

		BOOL bRet = FALSE;

		if ( pszKLID == NULL || _tcslen( pszKLID) == 0)
		{
			_stprintf_s( szMsg, _T("invalid pszKLID\n"));
			OutputDebugString( szMsg);
			return FALSE;
		}
		else
		{
			_stprintf_s( szMsg, _T("try to set %s as default ime\n"), pszKLID);
			OutputDebugString( szMsg);
		}

		HKL hKl = LoadKeyboardLayout( pszKLID, KLF_ACTIVATE|KLF_REORDER);
		if ( hKl == NULL)
		{
			_stprintf_s( szMsg, _T("Error: LoadKeyboardLayout failed, klid: %s, Error code: %d\n"), pszKLID, GetLastError());
			OutputDebugString( szMsg);
			return FALSE;
		}

		//
		// 如果已经是默认输入法，就不用再设置了
		//
		HKL hDefIME = 0;
		SystemParametersInfo( SPI_GETDEFAULTINPUTLANG, 0, &hDefIME, 0);	
		if( hKl == hDefIME) 
		{
			_stprintf_s( szMsg, _T("This ime is already the default ime\n"));
			OutputDebugString( szMsg);
			return TRUE;
		}

		HKEY hKPreload = 0;
		if( ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER,_T("Keyboard Layout\\preload"), 0, KEY_ALL_ACCESS, &hKPreload))
		{
			map< tstring, unsigned int> mapPreLoad;
			GetKLIDMap( hKPreload, mapPreLoad);

			TCHAR szKLID[256] = _T("");
			_tcscpy_s( szKLID, pszKLID);
			_tcslwr_s( szKLID);

			//
			// 如果klid所表示的输入法被用户通过控制面板 -> 语言区域设置 删除了
			// 那么在preload list中就不会出现。
			//
			vector<tstring> vectorOrder;
			vectorOrder.reserve( mapPreLoad.size() + 1);
			vectorOrder.push_back( szKLID);
			if ( mapPreLoad.find( szKLID) != mapPreLoad.end())
			{
				mapPreLoad.erase( szKLID);
			}

			KLIDMapToVector( mapPreLoad, vectorOrder);

			for( unsigned int i = 0; i < vectorOrder.size(); i++)
			{
				TCHAR szEntry[32];
				_stprintf_s( szEntry,_T("%d"), i+1);
				RegSetValueEx( hKPreload, szEntry, 0, REG_SZ,(LPBYTE)(vectorOrder[i].c_str()), (DWORD)(vectorOrder[i].length() + 1) * sizeof(TCHAR));
			}

			//for(DWORD i=0;i<dwIndex;i++)
			//{
			//	_stprintf_s(szEntry,_T("%d"),i+1);
			//	RegSetValueEx(hKPreload,szEntry,0,REG_SZ,(LPBYTE)szImeIDs[i],9*sizeof(TCHAR));
			//}
			//free(szImeIDs);

			RegCloseKey( hKPreload);
		}

		bRet = SystemParametersInfo( SPI_SETDEFAULTINPUTLANG, 0, &hKl, SPIF_SENDCHANGE);

		_stprintf_s( szMsg, _T("SystemParametersInfo return %d\n"), bRet);
		OutputDebugString( szMsg);

		return bRet;
	}

	// BAIDU_IME_FILE_NAME, c_clsidBaidujpTSF, c_guidProfile
	static BOOL EnsureDefault(PCTSTR pszIMEFileName, REFCLSID rclsid, REFGUID guidProfile)
	{
		TCHAR szKLID[256] = _T("");
		HKEY hKey = NULL;
		LONG nRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts"), 0, KEY_READ, &hKey);
		if ( ERROR_SUCCESS == nRet)
		{
			GetImeKLID( hKey, pszIMEFileName, szKLID, 256);
			RegCloseKey( hKey);
		}

		TCHAR szMsg[256] = _T("");
		_stprintf_s( szMsg, _T("klid for %s is %s\n"), pszIMEFileName, szKLID);
		OutputDebugString( szMsg);

		HKL hKl = LoadKeyboardLayout( szKLID, KLF_ACTIVATE|KLF_REORDER);
		if ( hKl == NULL)
		{
			_stprintf_s( szMsg, _T("Error: LoadKeyboardLayout failed, klid: %s, Error code: %d\n"), szKLID, GetLastError());
			OutputDebugString( szMsg);
			return FALSE;
		}

		//
		// 如果已经是默认输入法，就不用再enable了
		//
		HKL hDefIME = 0;
		SystemParametersInfo( SPI_GETDEFAULTINPUTLANG, 0, &hDefIME, 0);	
		if( hKl == hDefIME) 
		{
			_stprintf_s( szMsg, _T("This ime is already the default ime\n"));
			OutputDebugString( szMsg);
			return TRUE;
		}

		BOOL bRet = SystemParametersInfo( SPI_SETDEFAULTINPUTLANG, 0, &hKl, SPIF_SENDCHANGE);
		_stprintf_s( szMsg, _T("SystemParametersInfo return %d\n"), bRet);
		OutputDebugString( szMsg);

		return bRet;
	}

	static BOOL XP_DefaultIME_Clear( PCTSTR pszIMEFileName)
	{
		TCHAR szMsg[256] = _T("");

		HKL hDefIME=0;
		SystemParametersInfo(SPI_GETDEFAULTINPUTLANG,0,&hDefIME,0);
		HKL hKlBaidu=GetKeyboardLayoutFromFileName(pszIMEFileName);

		_stprintf_s( szMsg, _T("hDefIME: %x, hKlBaidu: %x\n"), hDefIME, hKlBaidu);
		OutputDebugString( szMsg);

		if(hKlBaidu!=hDefIME) return TRUE;

		TCHAR szBaiduJPKLID[256] = _T("");
		HKEY hKey = NULL;
		LONG nRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts"), 0, KEY_READ, &hKey);
		if ( ERROR_SUCCESS == nRet)
		{
			GetImeKLID( hKey, pszIMEFileName, szBaiduJPKLID, 256);
			RegCloseKey( hKey);
		}

		TCHAR szKLID[256] = _T("");

		if( _tcslen( szBaiduJPKLID) != 0)
		{
			HKEY hKPreload = 0;
			if( ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER,_T("Keyboard Layout\\preload"), 0, KEY_ALL_ACCESS, &hKPreload))
			{
				TCHAR szEntry[32];
				TCHAR szImeIDs[32];
				DWORD dwSizeEntry = sizeof(szEntry)/sizeof(TCHAR);
				DWORD dwSizeID = sizeof(szImeIDs);
				DWORD dwIndex = 0;
				while( RegEnumValue( hKPreload, dwIndex, szEntry, &dwSizeEntry, NULL, NULL,(LPBYTE)szImeIDs, &dwSizeID) == ERROR_SUCCESS)
				{
					if( _tcsicmp( szBaiduJPKLID, szImeIDs) !=0)
					{
						_tcscpy_s( szKLID, szImeIDs);
						break;
					}

					dwIndex++;
					dwSizeEntry = sizeof(szEntry)/sizeof(TCHAR);
					dwSizeID = sizeof(szImeIDs);
				}

				RegCloseKey(hKPreload);
			}
		}

		//
		// preload list中只有我们一个输入法，就把英文美式键盘设为默认输入法
		//
		if ( _tcslen( szKLID) == 0)
		{
			_tcscpy_s( szKLID, _T("00000409"));
		}

		return ChangeDefaultIME( szKLID);
	}

	static BOOL XP_DefaultIME_Set( PCTSTR pszIMEFileName)
	{
		TCHAR szKLID[256] = _T("");
		HKEY hKey = NULL;
		LONG nRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts"), 0, KEY_READ, &hKey);
		if ( ERROR_SUCCESS == nRet)
		{
			GetImeKLID( hKey, pszIMEFileName, szKLID, 256);
			RegCloseKey( hKey);
		}

		TCHAR szMsg[256] = _T("");
		_stprintf_s( szMsg, _T("klid for %s is %s\n"), pszIMEFileName, szKLID);
		OutputDebugString( szMsg);

		return ChangeDefaultIME( szKLID);
	}

	// BAIDU_IME_FILE_NAME
	static BOOL Enable( PCTSTR pszIMEFileName, BOOL bEnable)
	{
		//
		// 1. 把ime从preload list中移除
		//
		TCHAR szKLID[256] = _T("");
		HKEY hKey = NULL;
		LONG nRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts"), 0, KEY_READ, &hKey);
		if ( ERROR_SUCCESS == nRet)
		{
			GetImeKLID( hKey, pszIMEFileName, szKLID, 256);
			RegCloseKey( hKey);
		}

		if ( _tcslen( szKLID) != NULL)
		{
			if ( bEnable)
			{
				AddToPreloadList( szKLID);
			}
			else
			{
				RemoveFromPreloadList( szKLID);
			}
		}

		if ( !bEnable)
		{
			HKL hRet=0;
			int nKL=GetKeyboardLayoutList(0,NULL);
			HKL *pKLList=new HKL[nKL];

			GetKeyboardLayoutList(nKL,pKLList);
			for(int i=0;i<nKL;i++)
			{
				TCHAR szName[256];
				ImmGetIMEFileName( pKLList[i], szName, ARRAYSIZE(szName));

				if( _tcsicmp( szName, pszIMEFileName) == 0)
				{
					UnloadKeyboardLayout( pKLList[i]);
					break;
				}
			}

			delete []pKLList;
		}

		return TRUE;
	}

	// c_clsidBaidujpTSF, c_guidProfile
	static BOOL Enable( REFCLSID rclsid, REFGUID guidProfile, BOOL bEnable)
	{
		BOOL bCom = FALSE;
		HRESULT hr = CoInitialize(NULL);
		if ( SUCCEEDED(hr))
		{
			bCom = TRUE;
		}

		//
		// disable tsf
		//
		ITfInputProcessorProfiles *pInputProcessProfiles;
		hr = CoCreateInstance(CLSID_TF_InputProcessorProfiles, NULL, CLSCTX_INPROC_SERVER,
			IID_ITfInputProcessorProfiles, (void**)&pInputProcessProfiles);
		if ( hr == S_OK)
		{
			hr = pInputProcessProfiles->EnableLanguageProfile( rclsid, TEXTSERVICE_LANGID, guidProfile, bEnable);
			pInputProcessProfiles->Release();
		}

		if ( bCom)
		{
			CoUninitialize();
		}		

		return (hr == S_OK);
	}

	// BAIDU_IME_FILE_NAME, c_clsidBaidujpTSF, c_guidProfile
	static BOOL SetDefaultIME( PCTSTR pszIMEFileName, REFCLSID rclsid, REFGUID guidProfile, BOOL bSet)
	{	
		if( IsVistaOrAbove())
		{
			return SetDefaultImeVista( rclsid, guidProfile, bSet);
		}
		else
		{
			if ( bSet)
			{
				return XP_DefaultIME_Set( pszIMEFileName);
			}
			else
			{
				return XP_DefaultIME_Clear( pszIMEFileName);
			}
		}
	}

	static HKL Load( PCTSTR pszIMEFileName)
	{
		TCHAR szKLID[256] = _T("");
		HKEY hKey = NULL;
		LONG nRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts"), 0, KEY_READ, &hKey);
		if ( ERROR_SUCCESS == nRet)
		{
			GetImeKLID( hKey, pszIMEFileName, szKLID, 256);
			RegCloseKey( hKey);
		}

		if ( _tcslen( szKLID) == 0)
		{
			return NULL;
		}

		HKL hKL = LoadKeyboardLayout( szKLID, KLF_ACTIVATE|KLF_REPLACELANG|KLF_SETFORPROCESS);
		return hKL;
	}

	static BOOL Unload( PCTSTR pszIMEFileName)
	{
		BOOL bOK = FALSE;
		//HKL hPre = ActivateKeyboardLayout( HKL_PREV, 0);
		HKL hKlBaidu = GetKeyboardLayoutFromFileName( pszIMEFileName);
		if( hKlBaidu != NULL)
		{
			bOK = UnloadKeyboardLayout( hKlBaidu);
		}

		return bOK;
	}

	static BOOL IsVistaOrAbove()
	{
		OSVERSIONINFOEX osvi;
		DWORDLONG dwlConditionMask = 0;
		int op=VER_GREATER_EQUAL;

		// Initialize the OSVERSIONINFOEX structure.

		ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		osvi.dwMajorVersion = 6;
		osvi.dwMinorVersion = 0;
		osvi.wServicePackMajor = 0;
		osvi.wServicePackMinor = 0;

		// Initialize the condition mask.

		VER_SET_CONDITION( dwlConditionMask, VER_MAJORVERSION, op );
		VER_SET_CONDITION( dwlConditionMask, VER_MINORVERSION, op );
		VER_SET_CONDITION( dwlConditionMask, VER_SERVICEPACKMAJOR, op );
		VER_SET_CONDITION( dwlConditionMask, VER_SERVICEPACKMINOR, op );

		// Perform the test.

		return VerifyVersionInfo(
			&osvi, 
			VER_MAJORVERSION | VER_MINORVERSION | 
			VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR,
			dwlConditionMask);
	}

	static BOOL GetPreloadList( vector<tstring>& vectorOrder)
	{
		HKEY hKPreload = 0;
		if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_CURRENT_USER, _T("Keyboard Layout\\preload"), 0, KEY_ALL_ACCESS, &hKPreload))
		{
			map< tstring, unsigned int> mapPreLoad;
			GetKLIDMap( hKPreload, mapPreLoad);

			vectorOrder.reserve( mapPreLoad.size() + 1);

			KLIDMapToVector( mapPreLoad, vectorOrder);

			RegCloseKey(hKPreload);
		}

		return TRUE;
	}

	static BOOL GetKLIDMap( HKEY hKPreload, map< tstring, unsigned int>& mapPreLoad)
	{
		TCHAR szEntry[32];
		TCHAR szImeIDs[32];
		DWORD dwSizeEntry = sizeof(szEntry)/sizeof(TCHAR);
		DWORD dwSizeID = sizeof(szImeIDs);
		DWORD dwIndex = 0;
		while( RegEnumValue( hKPreload, dwIndex, szEntry, &dwSizeEntry, NULL, NULL,(LPBYTE)szImeIDs, &dwSizeID) == ERROR_SUCCESS)
		{
			_tcslwr_s( szImeIDs);
			if ( mapPreLoad.find( szImeIDs) == mapPreLoad.end())
			{
				mapPreLoad[szImeIDs] = _tcstoul( szEntry, NULL, 10);
			}

			dwIndex++;
			dwSizeEntry = sizeof(szEntry)/sizeof(TCHAR);
			dwSizeID = sizeof(szImeIDs);
		}
		return TRUE;
	}

	static BOOL KLIDMapToVector( map< tstring, unsigned int>& mapPreLoad, vector<tstring>& vectorOrder)
	{
		while ( mapPreLoad.size() != 0)
		{
			unsigned int nMinValue = (numeric_limits<unsigned int>::max)();
			tstring strKLID;
			for ( map< tstring, unsigned int>::iterator i = mapPreLoad.begin(); i != mapPreLoad.end(); i++)
			{
				if ( i->second <= nMinValue)
				{
					nMinValue = i->second;
					strKLID = i->first;
				}
			}

			vectorOrder.push_back(strKLID);
			mapPreLoad.erase( strKLID);
		}
		return TRUE;
	}

	static BOOL SwitchToOtherIME()
	{
		HKL hKL = ActivateKeyboardLayout( (HKL)HKL_NEXT, KLF_SETFORPROCESS);
		//HKL hKL = LoadKeyboardLayout( _T("00000409"), KLF_ACTIVATE | KLF_SETFORPROCESS);		
		return TRUE;
	}

};
