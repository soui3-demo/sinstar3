// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

#include "Globals.h"

class CEnumTfCandidates : public CUnknown,public IEnumTfCandidates
{
protected:
    // constructor/destructor
    CEnumTfCandidates(_In_ const CSampleImeArray<ITfCandidateString*> &rgelm, UINT currentNum);

    virtual ~CEnumTfCandidates(void);

public:
    // create instance
    static HRESULT CreateInstance(_Out_ CEnumTfCandidates **ppobj, _In_ const CSampleImeArray<ITfCandidateString*> &rgelm, UINT currentNum = 0);
    static HRESULT CreateInstance(REFIID riid, _Out_ void **ppvObj, _In_ const CSampleImeArray<ITfCandidateString*> &rgelm, UINT currentNum = 0);

    // IEnumTfCandidates methods
    virtual STDMETHODIMP Next(ULONG ulCount, _Out_ ITfCandidateString **ppObj, _Out_ ULONG *pcFetched);
    virtual STDMETHODIMP Skip(ULONG ulCount);
    virtual STDMETHODIMP Reset();
    virtual STDMETHODIMP Clone(_Out_ IEnumTfCandidates **ppEnum);

protected:
    CSampleImeArray<ITfCandidateString*> _rgelm;
    UINT _currentCandidateStrIndex;

public:
	IUNKNOWN_BEGIN(IEnumTfCandidates)
	IUNKNOWN_END()
};
