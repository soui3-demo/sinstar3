//////////////////////////////////////////////////////////////////////
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
//  TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//  PARTICULAR PURPOSE.
//
//  Copyright (C) 2003  Microsoft Corporation.  All rights reserved.
//
//  DisplayAttribureInfo.cpp
//
//          ITfDisplayAttributeProvider implementation.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DisplayAttributeInfo.h"

//+---------------------------------------------------------------------------
//
// The registry key and values
//
//----------------------------------------------------------------------------

// the registry key of this text service to save the custmized display attribute
const TCHAR c_szAttributeInfoKey[] = TEXT("Software\\Baidu\\Type");

// the registry values of the custmized display attributes
const TCHAR CDisplayAttributeInfoInput::_s_szValueName[] = TEXT("DisplayAttributeInput");
const TCHAR CDisplayAttributeInfoConverted::_s_szValueName[] = TEXT("DisplayAttributeConverted");
const TCHAR CDisplayAttributeInfoTargetConverted::_s_szValueName[] = TEXT("DisplayAttributeTargetConverted");

//+---------------------------------------------------------------------------
//
// The criptions
//
//----------------------------------------------------------------------------

const WCHAR CDisplayAttributeInfoInput::_s_szDescription[] = L"Baidu Type Display Attribute Input";
const WCHAR CDisplayAttributeInfoConverted::_s_szDescription[] = L"Baidu Type Display Attribute Converted";
const WCHAR CDisplayAttributeInfoTargetConverted::_s_szDescription[] = L"Baidu Type Display Attribute Target Converted";


//+---------------------------------------------------------------------------
//
// DisplayAttribute
//
//----------------------------------------------------------------------------

const TF_DISPLAYATTRIBUTE CDisplayAttributeInfoInput::_s_DisplayAttribute =
{
    { TF_CT_NONE, 0 },     // text color
    { TF_CT_NONE, 0 },                      // background color (TF_CT_NONE => app default)
    TF_LS_SQUIGGLE,                            // underline style
    FALSE,                                  // underline boldness
    { TF_CT_NONE, 0 },     // underline color
    TF_ATTR_INPUT                           // attribute info
};

const TF_DISPLAYATTRIBUTE CDisplayAttributeInfoConverted::_s_DisplayAttribute =
{
    { TF_CT_NONE, 0 }, // text color
    { TF_CT_NONE, 0 }, // background color (TF_CT_NONE => app default)
    TF_LS_SOLID,                             // underline style
    FALSE,                                  // underline boldness
    { TF_CT_NONE,  0 },                      // underline color
    TF_ATTR_CONVERTED					 // attribute info
};

const TF_DISPLAYATTRIBUTE CDisplayAttributeInfoTargetConverted::_s_DisplayAttribute =
{
	{ TF_CT_NONE, 0}, // text color
	{ TF_CT_NONE, 0}, // background color (TF_CT_NONE => app default)
	TF_LS_SOLID,                             // underline style
	TRUE,                                  // underline boldness
	{ TF_CT_NONE, 0},                      // underline color
	TF_ATTR_TARGET_CONVERTED					 // attribute info
};

//+---------------------------------------------------------------------------
//
// ctor
//
//----------------------------------------------------------------------------

CDisplayAttributeInfo::CDisplayAttributeInfo()
{
    theModule->AddRef();

    _pguid = NULL;
    _pDisplayAttribute = NULL;
    _pszValueName = NULL;

    _cRef = 1;
}

//+---------------------------------------------------------------------------
//
// dtor
//
//----------------------------------------------------------------------------

CDisplayAttributeInfo::~CDisplayAttributeInfo()
{
    theModule->Release();
}

STDAPI CDisplayAttributeInfo::GetGUID(GUID *pguid)
{
    if (pguid == NULL)
        return E_INVALIDARG;

    if (_pguid == NULL)
        return E_FAIL;

    *pguid = *_pguid;

    return S_OK;
}

//+---------------------------------------------------------------------------
//
// GetDescription
//
//----------------------------------------------------------------------------

STDAPI CDisplayAttributeInfo::GetDescription(BSTR *pbstrDesc)
{
    BSTR bstrDesc;

    if (pbstrDesc == NULL)
        return E_INVALIDARG;

    *pbstrDesc = NULL;

    if ((bstrDesc = SysAllocString(_pszDescription)) == NULL)
        return E_OUTOFMEMORY;

    *pbstrDesc = bstrDesc;

    return S_OK;
}

//+---------------------------------------------------------------------------
//
// GetAttributeInfo
//
//----------------------------------------------------------------------------

STDAPI CDisplayAttributeInfo::GetAttributeInfo(TF_DISPLAYATTRIBUTE *ptfDisplayAttr)
{
    HKEY hKeyAttributeInfo;
    LONG lResult;
    DWORD cbData;

    if (ptfDisplayAttr == NULL)
        return E_INVALIDARG;

    if (_pszValueName == NULL)
        return E_FAIL;

    lResult = E_FAIL;

    if (RegOpenKeyEx(HKEY_CURRENT_USER, c_szAttributeInfoKey, 0, KEY_READ, &hKeyAttributeInfo) == ERROR_SUCCESS)
    {
        cbData = sizeof(*ptfDisplayAttr);

        lResult = RegQueryValueEx(hKeyAttributeInfo, _pszValueName,
                                  NULL, NULL,
                                  (LPBYTE)ptfDisplayAttr, &cbData);

        RegCloseKey(hKeyAttributeInfo);
    }

    if (lResult != ERROR_SUCCESS || cbData != sizeof(*ptfDisplayAttr))
    {
        // return the default display attribute.
        *ptfDisplayAttr = *_pDisplayAttribute;
    }

    return S_OK;
}

//+---------------------------------------------------------------------------
//
// SetAttributeInfo
//
//----------------------------------------------------------------------------

STDAPI CDisplayAttributeInfo::SetAttributeInfo(const TF_DISPLAYATTRIBUTE *ptfDisplayAttr)
{
    HKEY hKeyAttributeInfo;
    LONG lResult;

    if (_pszValueName == NULL)
        return E_FAIL;

    lResult = RegCreateKeyEx(HKEY_CURRENT_USER, c_szAttributeInfoKey, 0, TEXT(""),
                             REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL,
                             &hKeyAttributeInfo, NULL);

    if (lResult != ERROR_SUCCESS)
        return E_FAIL;

    lResult = RegSetValueEx(hKeyAttributeInfo, _pszValueName,
                            0, REG_BINARY, (const BYTE *)ptfDisplayAttr,
                            sizeof(*ptfDisplayAttr));

    RegCloseKey(hKeyAttributeInfo);

    return (lResult == ERROR_SUCCESS) ? S_OK : E_FAIL;
}

//+---------------------------------------------------------------------------
//
// Reset
//
//----------------------------------------------------------------------------

STDAPI CDisplayAttributeInfo::Reset()
{
    return SetAttributeInfo(_pDisplayAttribute);
}

