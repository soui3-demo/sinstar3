// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#include "stdafx.h"
#include "DisplayAttributeInfo.h"

//+---------------------------------------------------------------------------
//
// The registry key and values
//
//----------------------------------------------------------------------------

// the registry values of the custmized display attributes
const WCHAR CDisplayAttributeInfoInput::_s_szValueName[] = L"DisplayAttributeInput";

// The descriptions
const WCHAR CDisplayAttributeInfoInput::_s_szDescription[] = L"Sinstar3 Display Attribute Input";

//+---------------------------------------------------------------------------
//
// DisplayAttribute
//
//----------------------------------------------------------------------------

const TF_DISPLAYATTRIBUTE CDisplayAttributeInfoInput::_s_DisplayAttribute =
{
	{ TF_CT_NONE, 0 },    // text color
	{ TF_CT_NONE, 0 },                      // background color (TF_CT_NONE => app default)
	TF_LS_NONE,								// underline style
	FALSE,                                  // underline boldness
	{ TF_CT_NONE, 0 },    // underline color
	TF_ATTR_INPUT                           // attribute info
};

CDisplayAttributeInfoInput::CDisplayAttributeInfoInput()
{
	_pguid = &c_guidDispAttrInput;
	_pDisplayAttribute = &_s_DisplayAttribute;
	_pDescription = _s_szDescription;
	_pValueName = _s_szValueName;
}

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
    _pValueName = NULL;
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

//+---------------------------------------------------------------------------
//
// ITfDisplayAttributeInfo::GetGUID
//
//----------------------------------------------------------------------------

STDAPI CDisplayAttributeInfo::GetGUID(_Out_ GUID *pguid)
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
// ITfDisplayAttributeInfo::GetDescription
//
//----------------------------------------------------------------------------

STDAPI CDisplayAttributeInfo::GetDescription(_Out_ BSTR *pbstrDesc)
{
    BSTR tempDesc;

    if (pbstrDesc == NULL)
    {
        return E_INVALIDARG;
    }

    *pbstrDesc = NULL;

    if ((tempDesc = SysAllocString(_pDescription)) == NULL)
    {
        return E_OUTOFMEMORY;
    }

    *pbstrDesc = tempDesc;

    return S_OK;
}

//+---------------------------------------------------------------------------
//
// ITfDisplayAttributeInfo::GetAttributeInfo
//
//----------------------------------------------------------------------------

STDAPI CDisplayAttributeInfo::GetAttributeInfo(_Out_ TF_DISPLAYATTRIBUTE *ptfDisplayAttr)
{
    if (ptfDisplayAttr == NULL)
    {
        return E_INVALIDARG;
    }

    // return the default display attribute.
    *ptfDisplayAttr = *_pDisplayAttribute;

    return S_OK;
}

//+---------------------------------------------------------------------------
//
// ITfDisplayAttributeInfo::SetAttributeInfo
//
//----------------------------------------------------------------------------

STDAPI CDisplayAttributeInfo::SetAttributeInfo(_In_ const TF_DISPLAYATTRIBUTE *ptfDisplayAttr)
{
    ptfDisplayAttr;

    return E_NOTIMPL;
}

//+---------------------------------------------------------------------------
//
// ITfDisplayAttributeInfo::Reset
//
//----------------------------------------------------------------------------

STDAPI CDisplayAttributeInfo::Reset()
{
    return SetAttributeInfo(_pDisplayAttribute);
}
