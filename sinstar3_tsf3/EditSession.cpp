// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#include "Private.h"
#include "EditSession.h"
#include "Sinstar3_Tsf.h"

//+---------------------------------------------------------------------------
//
// ctor
//
//----------------------------------------------------------------------------

CEditSessionBase::CEditSessionBase(_In_ CSinstar3Tsf *pTextService, _In_ ITfContext *pContext)
{
    _pContext = pContext;
    _pContext->AddRef();

    _pTextService = pTextService;
    _pTextService->AddRef();
}

//+---------------------------------------------------------------------------
//
// dtor
//
//----------------------------------------------------------------------------

CEditSessionBase::~CEditSessionBase()
{
    _pContext->Release();
    _pTextService->Release();
}
