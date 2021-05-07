// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#include "stdafx.h"
#include "DisplayAttributeInfo.h"
#include "EnumDisplayAttributeInfo.h"

//+---------------------------------------------------------------------------
//
// ctor
//
//----------------------------------------------------------------------------

CEnumDisplayAttributeInfo::CEnumDisplayAttributeInfo()
{
    theModule->AddRef();
    _index = 0;
}

//+---------------------------------------------------------------------------
//
// dtor
//
//----------------------------------------------------------------------------

CEnumDisplayAttributeInfo::~CEnumDisplayAttributeInfo()
{
    theModule->Release();
}


//+---------------------------------------------------------------------------
//
// IEnumTfDisplayAttributeInfo::Clone
//
// Returns a copy of the object.
//----------------------------------------------------------------------------

STDAPI CEnumDisplayAttributeInfo::Clone( IEnumTfDisplayAttributeInfo **ppEnum)
{
    CEnumDisplayAttributeInfo* pClone = NULL;

    if (ppEnum == NULL)
    {
        return E_INVALIDARG;
    }

    *ppEnum = NULL;

    pClone = new  CEnumDisplayAttributeInfo();
    if ((pClone) == NULL)
    {
        return E_OUTOFMEMORY;
    }

    // the clone should match this object's state
    pClone->_index = _index;

    *ppEnum = pClone;

    return S_OK;
}

//+---------------------------------------------------------------------------
//
// IEnumTfDisplayAttributeInfo::Next
//
// Returns an array of display attribute info objects supported by this service.
//----------------------------------------------------------------------------

const int MAX_DISPLAY_ATTRIBUTE_INFO = 1;

STDAPI CEnumDisplayAttributeInfo::Next(ULONG ulCount, ITfDisplayAttributeInfo **rgInfo,  ULONG *pcFetched)
{
    ULONG fetched;

    fetched = 0;

    if (ulCount == 0)
    {
        return S_OK;
    }
    if (rgInfo == NULL)
    {
        return E_INVALIDARG;
    }
    *rgInfo = NULL;

    while (fetched < ulCount)
    {
        ITfDisplayAttributeInfo* pDisplayAttributeInfo = NULL;

        if (_index == 0)
        {   
            pDisplayAttributeInfo = new CDisplayAttributeInfoInput();
            if ((pDisplayAttributeInfo) == NULL)
            {
                return E_OUTOFMEMORY;
            }
        }
        else if (_index == 1)
        {
            pDisplayAttributeInfo = new CDisplayAttributeInfoConverted();
            if ((pDisplayAttributeInfo) == nullptr)
            {
                return E_OUTOFMEMORY;
            }

        }
        else
        {
            break;
        }

        *rgInfo = pDisplayAttributeInfo;
        rgInfo++;
        fetched++;
        _index++;
    }

    if (pcFetched != NULL)
    {
        // technically this is only legal if ulCount == 1, but we won't check
        *pcFetched = fetched;
    }

    return (fetched == ulCount) ? S_OK : S_FALSE;
}

//+---------------------------------------------------------------------------
//
// IEnumTfDisplayAttributeInfo::Reset
//
// Resets the enumeration.
//----------------------------------------------------------------------------

STDAPI CEnumDisplayAttributeInfo::Reset()
{
    _index = 0;
    return S_OK;
}

//+---------------------------------------------------------------------------
//
// IEnumTfDisplayAttributeInfo::Skip
//
// Skips past objects in the enumeration.
//----------------------------------------------------------------------------

STDAPI CEnumDisplayAttributeInfo::Skip(ULONG ulCount)
{
    if ((ulCount + _index) > MAX_DISPLAY_ATTRIBUTE_INFO || (ulCount + _index) < ulCount)
    {
        _index = MAX_DISPLAY_ATTRIBUTE_INFO;
        return S_FALSE;
    }
    _index += ulCount;
    return S_OK;
}
