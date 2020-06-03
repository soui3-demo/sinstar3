// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

//+---------------------------------------------------------------------------
//
// CEnumDisplayAttributeInfo
//
//----------------------------------------------------------------------------

class CEnumDisplayAttributeInfo : public IEnumTfDisplayAttributeInfo, public CUnknown
{
public:
    CEnumDisplayAttributeInfo();
    ~CEnumDisplayAttributeInfo();

    // IEnumTfDisplayAttributeInfo
    STDMETHODIMP Clone( IEnumTfDisplayAttributeInfo **ppEnum);
    STDMETHODIMP Next(ULONG ulCount,  ITfDisplayAttributeInfo **rgInfo, ULONG *pcFetched);
    STDMETHODIMP Reset();
    STDMETHODIMP Skip(ULONG ulCount);

private:
    LONG _index;    // next display attribute to enum
public:
	IUNKNOWN_BEGIN(IEnumTfDisplayAttributeInfo)
	IUNKNOWN_END()
};
