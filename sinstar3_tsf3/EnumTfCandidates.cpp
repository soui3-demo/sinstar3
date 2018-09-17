// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#include "private.h"
#include "EnumTfCandidates.h"

HRESULT CEnumTfCandidates::CreateInstance(_Out_ CEnumTfCandidates **ppobj, _In_ const CSampleImeArray<ITfCandidateString*> &rgelm, UINT currentNum)
{
    if (ppobj == nullptr)
    {
        return E_INVALIDARG;
    }
    *ppobj = nullptr;

    *ppobj = new (std::nothrow) CEnumTfCandidates(rgelm, currentNum);
    if (*ppobj == nullptr) 
    {
        return E_OUTOFMEMORY;
    }

    return S_OK;
}

HRESULT CEnumTfCandidates::CreateInstance(REFIID riid, _Out_ void **ppvObj, _In_ const CSampleImeArray<ITfCandidateString*> &rgelm, UINT currentNum)
{
    if (ppvObj == nullptr)
    {
        return E_POINTER;
    }
    *ppvObj = nullptr;

    *ppvObj = new (std::nothrow) CEnumTfCandidates(rgelm, currentNum);
    if (*ppvObj == nullptr) 
    {
        return E_OUTOFMEMORY;
    }

    return ((CEnumTfCandidates*)(*ppvObj))->QueryInterface(riid, ppvObj);
}

CEnumTfCandidates::CEnumTfCandidates(_In_ const CSampleImeArray<ITfCandidateString*> &rgelm, UINT currentNum)
{
    _rgelm = rgelm;
    _currentCandidateStrIndex = currentNum;
}

CEnumTfCandidates::~CEnumTfCandidates()
{
}



//
// IEnumTfCandidates methods
//
STDMETHODIMP CEnumTfCandidates::Next(ULONG ulCount, _Out_ ITfCandidateString **ppObj, _Out_ ULONG *pcFetched)
{
    ULONG fetched = 0;
    if (ppObj == nullptr)
    {
        return E_INVALIDARG;
    }
    *ppObj = nullptr;

    while ((fetched < ulCount) && (_currentCandidateStrIndex < _rgelm.Count()))
    {
        *ppObj = *_rgelm.GetAt(_currentCandidateStrIndex);
        _currentCandidateStrIndex++;
        fetched++;
    }

    if (pcFetched)
    {
        *pcFetched = fetched;
    }

    return (fetched == ulCount) ? S_OK : S_FALSE;
}

STDMETHODIMP CEnumTfCandidates::Skip(ULONG ulCount)
{
    while ((0 < ulCount) && (_currentCandidateStrIndex < _rgelm.Count()))
    {
        _currentCandidateStrIndex++;
        ulCount--;
    }

    return (0 < ulCount) ? S_FALSE : S_OK;
}

STDMETHODIMP CEnumTfCandidates::Reset()
{
    _currentCandidateStrIndex = 0;
    return S_OK;
}

STDMETHODIMP CEnumTfCandidates::Clone(_Out_ IEnumTfCandidates **ppEnum)
{
    return CreateInstance(__uuidof(IEnumTfCandidates), (void**)ppEnum, _rgelm, _currentCandidateStrIndex);
}