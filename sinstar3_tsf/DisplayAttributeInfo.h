// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved


#pragma once

//+---------------------------------------------------------------------------
//
// CDisplayAttributeInfo class
//
//----------------------------------------------------------------------------

class CDisplayAttributeInfo : public ITfDisplayAttributeInfo, public CUnknown
{
public:
    CDisplayAttributeInfo();
    ~CDisplayAttributeInfo();

    // ITfDisplayAttributeInfo
    STDMETHODIMP GetGUID(_Out_ GUID *pguid);
    STDMETHODIMP GetDescription(_Out_ BSTR *pbstrDesc);
    STDMETHODIMP GetAttributeInfo(_Out_ TF_DISPLAYATTRIBUTE *pTSFDisplayAttr);
    STDMETHODIMP SetAttributeInfo(_In_ const TF_DISPLAYATTRIBUTE *ptfDisplayAttr);
    STDMETHODIMP Reset();

protected:
    const GUID* _pguid;
    const TF_DISPLAYATTRIBUTE* _pDisplayAttribute;
    const WCHAR* _pDescription;
    const WCHAR* _pValueName;

public:
	IUNKNOWN_BEGIN(ITfDisplayAttributeInfo)
	IUNKNOWN_END()
};

//+---------------------------------------------------------------------------
//
// CDisplayAttributeInfoInput class
//
//----------------------------------------------------------------------------

class CDisplayAttributeInfoInput : public CDisplayAttributeInfo
{
public:
    CDisplayAttributeInfoInput();

    static const TF_DISPLAYATTRIBUTE _s_DisplayAttribute;
    static const WCHAR _s_szDescription[];
    static const WCHAR _s_szValueName[];
};
