//////////////////////////////////////////////////////////////////////
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
//  TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//  PARTICULAR PURPOSE.
//
//  Copyright (C) 2003  Microsoft Corporation.  All rights reserved.
//
//  DisplayAttribureInfo.h
//
//          CDisplayAttributeInfo class
//          CDisplayAttributeInfoInput class
//          CDisplayAttributeInfoConverted class
//
//////////////////////////////////////////////////////////////////////

#ifndef DISPLAYATTRIBUTEINFO_H
#define DISPLAYATTRIBUTEINFO_H

//+---------------------------------------------------------------------------
//
// CDisplayAttributeInfo class
//
//----------------------------------------------------------------------------

class CDisplayAttributeInfo : public CUnknown,public ITfDisplayAttributeInfo
{
public:
    CDisplayAttributeInfo();
    ~CDisplayAttributeInfo();

    // IUnknown
	IUNKNOWN_BEGIN(ITfDisplayAttributeInfo)
	IUNKNOWN_END()

    // ITfDisplayAttributeInfo
    STDMETHODIMP GetGUID(GUID *pguid);
    STDMETHODIMP GetDescription(BSTR *pbstrDesc);
    STDMETHODIMP GetAttributeInfo(TF_DISPLAYATTRIBUTE *ptfDisplayAttr);
    STDMETHODIMP SetAttributeInfo(const TF_DISPLAYATTRIBUTE *ptfDisplayAttr);
    STDMETHODIMP Reset();

protected:
    const GUID *_pguid;
    const TF_DISPLAYATTRIBUTE *_pDisplayAttribute;
    const WCHAR *_pszDescription;
    const TCHAR *_pszValueName;
};

//+---------------------------------------------------------------------------
//
// CDisplayAttributeInfoInput class
//
//----------------------------------------------------------------------------

class CDisplayAttributeInfoInput : public CDisplayAttributeInfo
{
public:
    CDisplayAttributeInfoInput()
    {
        _pguid = &c_guidDisplayAttributeInput;
        _pDisplayAttribute = &_s_DisplayAttribute;
        _pszDescription = _s_szDescription;
        _pszValueName = _s_szValueName;
    }

    static const TF_DISPLAYATTRIBUTE _s_DisplayAttribute;
    static const WCHAR _s_szDescription[];
    static const TCHAR _s_szValueName[];
};

//+---------------------------------------------------------------------------
//
// CDisplayAttributeInfoConverted class
//
//----------------------------------------------------------------------------

class CDisplayAttributeInfoConverted : public CDisplayAttributeInfo
{
public:
    CDisplayAttributeInfoConverted()
    {
        _pguid = &c_guidDisplayAttributeConverted;
        _pDisplayAttribute = &_s_DisplayAttribute;
        _pszDescription = _s_szDescription;
        _pszValueName = _s_szValueName;
    }

    static const TF_DISPLAYATTRIBUTE _s_DisplayAttribute;
    static const WCHAR _s_szDescription[];
    static const TCHAR _s_szValueName[];
};

class CDisplayAttributeInfoTargetConverted : public CDisplayAttributeInfo
{
public:
	CDisplayAttributeInfoTargetConverted()
	{
		_pguid = &c_guidDisplayAttributeTargetConverted;
		_pDisplayAttribute = &_s_DisplayAttribute;
		_pszDescription = _s_szDescription;
		_pszValueName = _s_szValueName;
	}

	static const TF_DISPLAYATTRIBUTE _s_DisplayAttribute;
	static const WCHAR _s_szDescription[];
	static const TCHAR _s_szValueName[];
};
#endif DISPLAYATTRIBUTEINFO_H
