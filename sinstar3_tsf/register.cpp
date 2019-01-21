// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
#include "stdafx.h"
#include "../helper/helper.h"
#include <imm.h>
#pragma comment(lib,"imm32.lib")

static const WCHAR RegInfo_Prefix_CLSID[] = L"CLSID\\";
static const WCHAR RegInfo_Key_InProSvr32[] = L"InProcServer32";
static const WCHAR RegInfo_Key_ThreadModel[] = L"ThreadingModel";
#define TEXTSERVICE_MODEL        L"Apartment"

#define CLSID_STRLEN 38
#define TEXTSERVICE_ICON_INDEX  0
#define	SINSTAR3_IME_FILE_NAME	_T("sinstar3_ime.ime")

#if _MSC_VER < 1600
/* For Windows 8 */
const GUID GUID_TFCAT_TIPCAP_IMMERSIVESUPPORT =
{ 0x13A016DF, 0x560B, 0x46CD, { 0x94, 0x7A, 0x4C, 0x3A, 0xF1, 0xE0, 0xE3, 0x5D } };

const GUID GUID_TFCAT_TIPCAP_SYSTRAYSUPPORT =
{ 0x25504FB4, 0x7BAB, 0x4BC1, { 0x9C, 0x69, 0xCF, 0x81, 0x89, 0x0F, 0x0E, 0xF5 } };

#endif


static const GUID SupportCategoriesWin8Later[] = {
	GUID_TFCAT_TIP_KEYBOARD,
	GUID_TFCAT_TIPCAP_INPUTMODECOMPARTMENT,
	GUID_TFCAT_TIPCAP_IMMERSIVESUPPORT, 
	GUID_TFCAT_TIPCAP_SYSTRAYSUPPORT,
};

static const GUID SupportCategories[] = {
	GUID_TFCAT_TIP_KEYBOARD,
	GUID_TFCAT_TIPCAP_INPUTMODECOMPARTMENT,
};

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
    HRESULT hr = S_FALSE;

    WCHAR achIconFile[MAX_PATH] = {'\0'};
    DWORD cchA = 0;
    cchA = GetModuleFileName(theModule->GetModule(), achIconFile, MAX_PATH);
    cchA = cchA >= MAX_PATH ? (MAX_PATH - 1) : cchA;
    achIconFile[cchA] = '\0';

    ULONG lenOfDesc = (ULONG)wcslen(PRODUCT_WNAMEVER);

	HKL hKLSinstar3 = GetKeyboardLayoutFromFileName(SINSTAR3_IME_FILE_NAME);

	if (Helper_IsWin8orLater())
	{
		SOUI::SComPtr<ITfInputProcessorProfileMgr> pInputProcessorProfileMgr;
		hr = pInputProcessorProfileMgr.CoCreateInstance(CLSID_TF_InputProcessorProfiles, NULL, CLSCTX_ALL);
		if (FAILED(hr))
			return FALSE;

		hr = pInputProcessorProfileMgr->RegisterProfile(c_clsidSinstar3TSF,
			TEXTSERVICE_LANGID,
			c_guidProfile,
			PRODUCT_WNAMEVER,
			lenOfDesc,
			achIconFile,
			cchA,
			(UINT)TEXTSERVICE_ICON_INDEX, NULL, 0, TRUE, 0);
	}
	else
	{
		SOUI::SComPtr<ITfInputProcessorProfiles> pInputProcessorProfiles;
		hr = pInputProcessorProfiles.CoCreateInstance(CLSID_TF_InputProcessorProfiles, NULL, CLSCTX_INPROC_SERVER);
		if (FAILED(hr))
			return FALSE;

		hr = pInputProcessorProfiles->Register(c_clsidSinstar3TSF);
		if (FAILED(hr))
			return FALSE;

		hr = pInputProcessorProfiles->AddLanguageProfile(
			c_clsidSinstar3TSF,
			TEXTSERVICE_LANGID,
			c_guidProfile,
			PRODUCT_WNAMEVER,
			lenOfDesc,
			achIconFile,
			cchA,
			(UINT)TEXTSERVICE_ICON_INDEX);
		if (FAILED(hr))
			return FALSE;

		hr = pInputProcessorProfiles->SubstituteKeyboardLayout(
			c_clsidSinstar3TSF, TEXTSERVICE_LANGID, c_guidProfile, hKLSinstar3);
	}
	return SUCCEEDED(hr);
}

//+---------------------------------------------------------------------------
//
//  UnregisterProfiles
//
//----------------------------------------------------------------------------

void UnregisterProfiles()
{
    HRESULT hr = S_OK;

    ITfInputProcessorProfileMgr *pITfInputProcessorProfileMgr = NULL;
    hr = CoCreateInstance(CLSID_TF_InputProcessorProfiles, NULL, CLSCTX_INPROC_SERVER,
        IID_ITfInputProcessorProfileMgr, (void**)&pITfInputProcessorProfileMgr);
    if (FAILED(hr))
    {
        goto Exit;
    }

    hr = pITfInputProcessorProfileMgr->UnregisterProfile(c_clsidSinstar3TSF, TEXTSERVICE_LANGID, c_guidProfile, 0);
    if (FAILED(hr))
    {
        goto Exit;
    }

Exit:
    if (pITfInputProcessorProfileMgr)
    {
        pITfInputProcessorProfileMgr->Release();
    }

    return;
}

//+---------------------------------------------------------------------------
//
//  RegisterCategories
//
//----------------------------------------------------------------------------

BOOL RegisterCategories()
{
    ITfCategoryMgr* pCategoryMgr = NULL;
    HRESULT hr = S_OK;

    hr = CoCreateInstance(CLSID_TF_CategoryMgr, NULL, CLSCTX_INPROC_SERVER, IID_ITfCategoryMgr, (void**)&pCategoryMgr);
    if (FAILED(hr))
    {
        return FALSE;
    }

	if(Helper_IsWin8orLater())
	{
		for each(GUID guid in SupportCategoriesWin8Later)
		{
			hr = pCategoryMgr->RegisterCategory(c_clsidSinstar3TSF, guid, c_clsidSinstar3TSF);
		}
	}else
	{
		for each(GUID guid in SupportCategories)
		{
			hr = pCategoryMgr->RegisterCategory(c_clsidSinstar3TSF, guid, c_clsidSinstar3TSF);
		}
	}
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
    ITfCategoryMgr* pCategoryMgr = S_OK;
    HRESULT hr = S_OK;

    hr = CoCreateInstance(CLSID_TF_CategoryMgr, NULL, CLSCTX_INPROC_SERVER, IID_ITfCategoryMgr, (void**)&pCategoryMgr);
    if (FAILED(hr))
    {
        return;
    }

	if(Helper_IsWin8orLater())
	{
		for each(GUID guid in SupportCategoriesWin8Later)
		{
			hr = pCategoryMgr->UnregisterCategory(c_clsidSinstar3TSF, guid, c_clsidSinstar3TSF);
		}
	}else
	{
		for each(GUID guid in SupportCategories)
		{
			hr = pCategoryMgr->UnregisterCategory(c_clsidSinstar3TSF, guid, c_clsidSinstar3TSF);
		}
	}

    pCategoryMgr->Release();

    return;
}

//+---------------------------------------------------------------------------
//
// RecurseDeleteKey
//
// RecurseDeleteKey is necessary because on NT RegDeleteKey doesn't work if the
// specified key has subkeys
//----------------------------------------------------------------------------

LONG RecurseDeleteKey(_In_ HKEY hParentKey, _In_ LPCTSTR lpszKey)
{
    HKEY regKeyHandle = NULL;
    LONG res = 0;
    FILETIME time;
    WCHAR stringBuffer[256] = {'\0'};
    DWORD size = ARRAYSIZE(stringBuffer);

    if (RegOpenKey(hParentKey, lpszKey, &regKeyHandle) != ERROR_SUCCESS)
    {
        return ERROR_SUCCESS;
    }

    res = ERROR_SUCCESS;
    while (RegEnumKeyEx(regKeyHandle, 0, stringBuffer, &size, NULL, NULL, NULL, &time) == ERROR_SUCCESS)
    {
        stringBuffer[ARRAYSIZE(stringBuffer)-1] = '\0';
        res = RecurseDeleteKey(regKeyHandle, stringBuffer);
        if (res != ERROR_SUCCESS)
        {
            break;
        }
        size = ARRAYSIZE(stringBuffer);
    }
    RegCloseKey(regKeyHandle);

    return res == ERROR_SUCCESS ? RegDeleteKey(hParentKey, lpszKey) : res;
}

//+---------------------------------------------------------------------------
//
//  RegisterServer
//
//----------------------------------------------------------------------------
const BYTE GuidSymbols[] = {
	3, 2, 1, 0, '-', 5, 4, '-', 7, 6, '-', 8, 9, '-', 10, 11, 12, 13, 14, 15
};

const WCHAR HexDigits[] = L"0123456789ABCDEF";

BOOL CLSIDToString(REFGUID refGUID, WCHAR *pCLSIDString)
{
	WCHAR* pTemp = pCLSIDString;
	const BYTE* pBytes = (const BYTE *)&refGUID;

	DWORD j = 0;
	pTemp[j++] = L'{';
	for (int i = 0; i < sizeof(GuidSymbols) && j < (CLSID_STRLEN - 2); i++)
	{
		if (GuidSymbols[i] == '-')
		{
			pTemp[j++] = L'-';
		}
		else
		{
			pTemp[j++] = HexDigits[(pBytes[GuidSymbols[i]] & 0xF0) >> 4];
			pTemp[j++] = HexDigits[(pBytes[GuidSymbols[i]] & 0x0F)];
		}
	}

	pTemp[j++] = L'}';
	pTemp[j] = L'\0';

	return TRUE;
}
BOOL RegisterServer()
{
    DWORD copiedStringLen = 0;
    HKEY regKeyHandle = NULL;
    HKEY regSubkeyHandle = NULL;
    BOOL ret = FALSE;
    WCHAR achIMEKey[ARRAYSIZE(RegInfo_Prefix_CLSID) + CLSID_STRLEN] = {'\0'};
    WCHAR achFileName[MAX_PATH] = {'\0'};

    if (!CLSIDToString(c_clsidSinstar3TSF, achIMEKey + ARRAYSIZE(RegInfo_Prefix_CLSID) - 1))
    {
        return FALSE;
    }

    memcpy(achIMEKey, RegInfo_Prefix_CLSID, sizeof(RegInfo_Prefix_CLSID) - sizeof(WCHAR));

    if (RegCreateKeyEx(HKEY_CLASSES_ROOT, achIMEKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &regKeyHandle, &copiedStringLen) == ERROR_SUCCESS)
    {
        if (RegSetValueEx(regKeyHandle, NULL, 0, REG_SZ, (const BYTE *)PRODUCT_WNAMEVER, (_countof(PRODUCT_WNAMEVER))*sizeof(WCHAR)) != ERROR_SUCCESS)
        {
            goto Exit;
        }

        if (RegCreateKeyEx(regKeyHandle, RegInfo_Key_InProSvr32, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &regSubkeyHandle, &copiedStringLen) == ERROR_SUCCESS)
        {
            copiedStringLen = GetModuleFileNameW(theModule->GetModule(), achFileName, ARRAYSIZE(achFileName));
            copiedStringLen = (copiedStringLen >= (MAX_PATH - 1)) ? MAX_PATH : (++copiedStringLen);
            if (RegSetValueEx(regSubkeyHandle, NULL, 0, REG_SZ, (const BYTE *)achFileName, (copiedStringLen)*sizeof(WCHAR)) != ERROR_SUCCESS)
            {
                goto Exit;
            }

            if (RegSetValueEx(regSubkeyHandle, RegInfo_Key_ThreadModel, 0, REG_SZ, (const BYTE *)TEXTSERVICE_MODEL, (_countof(TEXTSERVICE_MODEL)) * sizeof(WCHAR)) != ERROR_SUCCESS)
            {
                goto Exit;
            }

            ret = TRUE;
        }
    }

Exit:
    if (regSubkeyHandle)
    {
        RegCloseKey(regSubkeyHandle);
        regSubkeyHandle = NULL;
    }
    if (regKeyHandle)
    {
        RegCloseKey(regKeyHandle);
        regKeyHandle = NULL;
    }

    return ret;
}

//+---------------------------------------------------------------------------
//
//  UnregisterServer
//
//----------------------------------------------------------------------------

void UnregisterServer()
{
    WCHAR achIMEKey[ARRAYSIZE(RegInfo_Prefix_CLSID) + CLSID_STRLEN] = {'\0'};

    if (!CLSIDToString(c_clsidSinstar3TSF, achIMEKey + ARRAYSIZE(RegInfo_Prefix_CLSID) - 1))
    {
        return;
    }

    memcpy(achIMEKey, RegInfo_Prefix_CLSID, sizeof(RegInfo_Prefix_CLSID) - sizeof(WCHAR));

    RecurseDeleteKey(HKEY_CLASSES_ROOT, achIMEKey);
}
