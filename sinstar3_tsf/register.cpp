//////////////////////////////////////////////////////////////////////
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
//  TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//  PARTICULAR PURPOSE.
//
//  Copyright (C) 2003  Microsoft Corporation.  All rights reserved.
//
//  Register.cpp
//
//          Server registration code.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <imm.h>
#pragma comment(lib,"imm32.lib")
#define CLSID_STRLEN 38  // strlen("{xxxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxx}")

static const TCHAR KInfoKeyPrefix[] = TEXT("CLSID\\");
static const TCHAR KInProcSvr32[] = TEXT("InProcServer32");
static const TCHAR KModelName[] = TEXT("ThreadingModel");
static const TCHAR KTextServiceModel[] = TEXT("Apartment");

#define TEXTSERVICE_ICON_INDEX  0
#define	SINSTAR3_IME_FILE_NAME	_T("sinstar3_ime.ime")


static HKL GetKeyboardLayoutFromFileName(LPCTSTR pszImeName)
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
//+---------------------------------------------------------------------------
//
//  RegisterProfiles
//
//----------------------------------------------------------------------------

BOOL RegisterProfiles()
{
    ITfInputProcessorProfiles *pInputProcessProfiles;
    WCHAR achIconFile[MAX_PATH];
    HRESULT hr;

    hr = CoCreateInstance(CLSID_TF_InputProcessorProfiles, NULL, CLSCTX_INPROC_SERVER,
                          IID_ITfInputProcessorProfiles, (void**)&pInputProcessProfiles);

    if (hr != S_OK)
        return E_FAIL;

    hr = pInputProcessProfiles->Register(c_clsidSinstar3TSF);

    if (hr != S_OK)
        goto Exit;

    GetModuleFileNameW(theModule->GetModule(), achIconFile, ARRAYSIZE(achIconFile));

    hr = pInputProcessProfiles->AddLanguageProfile(c_clsidSinstar3TSF,
                                  TEXTSERVICE_LANGID, 
                                  c_guidProfile, 
                                  PRODUCT_WNAME, 
                                  (ULONG)wcslen(PRODUCT_WNAME),
                                  achIconFile,
                                  (ULONG)wcslen(achIconFile),
                                  TEXTSERVICE_ICON_INDEX);


	if (hr != S_OK)
		goto Exit;
	//
	// 22:04 2010-11-1 Ren zhi jie
	// 考虑到兼容性，我们需要同时提供ime, tsf两种框架下的日文输入法，这样即使用户设置了不使用高级文字服务，仍可以通过我们提供的ime输入法。
	// 另一方面， 如果系统开启了高级文字服务，用户打开word进行文字编辑，在选择日文输入法时，为了避免系统同时显示我们的ime, tsf两种模式的输入法，
	// 可以使用SubstituteKeyboardLayout函数来设置，这样就只显示tsf输入法给用户选择了。
	//
	HKL hKLBaiduJP=GetKeyboardLayoutFromFileName( SINSTAR3_IME_FILE_NAME);
	if ( hKLBaiduJP)
	{
		hr = pInputProcessProfiles->SubstituteKeyboardLayout( c_clsidSinstar3TSF, TEXTSERVICE_LANGID, c_guidProfile, hKLBaiduJP);
		SLOGFMTI("SubstituteKeyboardLayout:%d",SUCCEEDED(hr));
	}

	hr = pInputProcessProfiles->EnableLanguageProfile( c_clsidSinstar3TSF, TEXTSERVICE_LANGID, c_guidProfile, TRUE);
	hr = pInputProcessProfiles->EnableLanguageProfileByDefault( c_clsidSinstar3TSF, TEXTSERVICE_LANGID, c_guidProfile, TRUE);


Exit:
    pInputProcessProfiles->Release();
    return (hr == S_OK);
}

//+---------------------------------------------------------------------------
//
//  UnregisterProfiles
//
//----------------------------------------------------------------------------

void UnregisterProfiles()
{
    ITfInputProcessorProfiles *pInputProcessProfiles;
    HRESULT hr;

    hr = CoCreateInstance(CLSID_TF_InputProcessorProfiles, NULL, CLSCTX_INPROC_SERVER,
                          IID_ITfInputProcessorProfiles, (void**)&pInputProcessProfiles);

    if (hr != S_OK)
        return;

    pInputProcessProfiles->Unregister(c_clsidSinstar3TSF);
    pInputProcessProfiles->Release();
}

//+---------------------------------------------------------------------------
//
//  RegisterCategories
//
//----------------------------------------------------------------------------

BOOL RegisterCategories()
{
    ITfCategoryMgr *pCategoryMgr;
    HRESULT hr;

    hr = CoCreateInstance(CLSID_TF_CategoryMgr, NULL, CLSCTX_INPROC_SERVER, 
                          IID_ITfCategoryMgr, (void**)&pCategoryMgr);

    if (hr != S_OK)
        return FALSE;

    //
    // register this text service to GUID_TFCAT_TIP_KEYBOARD category.
    //
    hr = pCategoryMgr->RegisterCategory(c_clsidSinstar3TSF,
                                        GUID_TFCAT_TIP_KEYBOARD, 
                                        c_clsidSinstar3TSF);

    //
    // register this text service to GUID_TFCAT_DISPLAYATTRIBUTEPROVIDER category.
    //
    hr = pCategoryMgr->RegisterCategory(c_clsidSinstar3TSF,
                                        GUID_TFCAT_DISPLAYATTRIBUTEPROVIDER, 
                                        c_clsidSinstar3TSF);


    pCategoryMgr->Release();
    return (hr == S_OK);
}

//+---------------------------------------------------------------------------
//
//  UnregisterCategories
//
//----------------------------------------------------------------------------

void UnregisterCategories()
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
    pCategoryMgr->UnregisterCategory(c_clsidSinstar3TSF,
                                     GUID_TFCAT_TIP_KEYBOARD, 
                                     c_clsidSinstar3TSF);

    //
    // unregister this text service from GUID_TFCAT_DISPLAYATTRIBUTEPROVIDER category.
    //
    pCategoryMgr->UnregisterCategory(c_clsidSinstar3TSF,
                                     GUID_TFCAT_DISPLAYATTRIBUTEPROVIDER, 
                                     c_clsidSinstar3TSF);

    pCategoryMgr->Release();
    return;
}

//+---------------------------------------------------------------------------
//
// CLSIDToString
//
//----------------------------------------------------------------------------

BOOL CLSIDToString(REFGUID refGUID, TCHAR *pch)
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
LONG RecurseDeleteKey(HKEY hParentKey, LPCTSTR lpszKey)
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

//+---------------------------------------------------------------------------
//
//  RegisterServer
//
//----------------------------------------------------------------------------

BOOL RegisterServer()
{
    DWORD dw;
    HKEY hKey;
    HKEY hSubKey;
    BOOL fRet;
    TCHAR achIMEKey[ARRAYSIZE(KInfoKeyPrefix) + CLSID_STRLEN];
    TCHAR achFileName[MAX_PATH];

    if (!CLSIDToString(c_clsidSinstar3TSF, achIMEKey + ARRAYSIZE(KInfoKeyPrefix) - 1))
        return FALSE;
    memcpy(achIMEKey, KInfoKeyPrefix, sizeof(KInfoKeyPrefix)-sizeof(TCHAR));

    if (fRet = RegCreateKeyEx(HKEY_CLASSES_ROOT, achIMEKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dw)
            == ERROR_SUCCESS)
    {
        fRet &= RegSetValueEx(hKey, NULL, 0, REG_SZ, (BYTE *)PRODUCT_NAME, (int)(_tcslen(PRODUCT_NAME)+1)*sizeof(TCHAR))
            == ERROR_SUCCESS;

        if (fRet &= RegCreateKeyEx(hKey, KInProcSvr32, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hSubKey, &dw)
            == ERROR_SUCCESS)
        {
            dw = GetModuleFileName(theModule->GetModule(), achFileName, ARRAYSIZE(achFileName));

            fRet &= RegSetValueEx(hSubKey, NULL, 0, REG_SZ, (BYTE *)achFileName, (lstrlen(achFileName)+1)*sizeof(TCHAR)) == ERROR_SUCCESS;
            fRet &= RegSetValueEx(hSubKey, KModelName, 0, REG_SZ, (BYTE *)KTextServiceModel, (int)(_tcslen(KTextServiceModel)+1)*sizeof(TCHAR)) == ERROR_SUCCESS;
            RegCloseKey(hSubKey);
        }
        RegCloseKey(hKey);
    }

    return fRet;
}

//+---------------------------------------------------------------------------
//
//  UnregisterServer
//
//----------------------------------------------------------------------------

void UnregisterServer()
{
    TCHAR achIMEKey[ARRAYSIZE(KInfoKeyPrefix) + CLSID_STRLEN];

    if (!CLSIDToString(c_clsidSinstar3TSF, achIMEKey + ARRAYSIZE(KInfoKeyPrefix) - 1))
        return;
    memcpy(achIMEKey, KInfoKeyPrefix, sizeof(KInfoKeyPrefix)-sizeof(TCHAR));

    RecurseDeleteKey(HKEY_CLASSES_ROOT, achIMEKey);
}
