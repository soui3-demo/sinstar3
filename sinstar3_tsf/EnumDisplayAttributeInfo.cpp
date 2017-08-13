//////////////////////////////////////////////////////////////////////
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
//  TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//  PARTICULAR PURPOSE.
//
//  Copyright (C) 2003  Microsoft Corporation.  All rights reserved.
//
//  EnumDisplayAttributeInfo.cpp
//
//          ITfEnumDisplayAttributeInfo implementation.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sinstar3_tsf.h"
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

    _iIndex = 0;
    _cRef = 1;
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

STDAPI CEnumDisplayAttributeInfo::Clone(IEnumTfDisplayAttributeInfo **ppEnum)
{
    CEnumDisplayAttributeInfo *pClone;

    if (ppEnum == NULL)
        return E_INVALIDARG;

    *ppEnum = NULL;

    if ((pClone = new CEnumDisplayAttributeInfo) == NULL)
        return E_OUTOFMEMORY;

    // the clone should match this object's state
    pClone->_iIndex = _iIndex;

    *ppEnum = pClone;

    return S_OK;
}

//+---------------------------------------------------------------------------
//
// Next
//
// Returns an array of display attribute info objects supported by this service.
//----------------------------------------------------------------------------

STDAPI CEnumDisplayAttributeInfo::Next(ULONG ulCount, ITfDisplayAttributeInfo **rgInfo, ULONG *pcFetched)
{
    ITfDisplayAttributeInfo *pDisplayAttributeInfo;
    ULONG cFetched;

    cFetched = 0;

    if (ulCount == 0)
        return S_OK;

    while (cFetched < ulCount)
    {
        if (_iIndex > 2)
            break;

        if (_iIndex == 0)
        {
            if ((pDisplayAttributeInfo = new CDisplayAttributeInfoInput()) == NULL)
                return E_OUTOFMEMORY;
        }
        else if (_iIndex == 1)
        {
            if ((pDisplayAttributeInfo = new CDisplayAttributeInfoConverted()) == NULL)
                return E_OUTOFMEMORY;
 
        }
		else if (_iIndex == 2)
		{
			if ((pDisplayAttributeInfo = new CDisplayAttributeInfoTargetConverted()) == NULL)
				return E_OUTOFMEMORY;

		}

        rgInfo[cFetched++] = pDisplayAttributeInfo;
        _iIndex++;
    }

    if (pcFetched != NULL)
    {
        // technically this is only legal if ulCount == 1, but this sample doesn't check
        *pcFetched = cFetched;
    }

    return (cFetched == ulCount) ? S_OK : S_FALSE;
}

//+---------------------------------------------------------------------------
//
// Reset
//
// Resets the enumeration.
//----------------------------------------------------------------------------

STDAPI CEnumDisplayAttributeInfo::Reset()
{
    _iIndex = 0;
    return S_OK;
}

//+---------------------------------------------------------------------------
//
// Skip
//
// Skips past objects in the enumeration.
//----------------------------------------------------------------------------

STDAPI CEnumDisplayAttributeInfo::Skip(ULONG ulCount)
{
    // there is only a single item to enum
    // so it can be skipped and avoid any overflow errors
    if (ulCount > 0 && _iIndex == 0)
    {
        _iIndex++;
    }
    return S_OK;
}

