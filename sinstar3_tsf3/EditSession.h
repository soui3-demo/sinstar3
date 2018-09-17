// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved


#pragma once

class CSinstar3Tsf;

class CEditSessionBase : public CUnknown, public ITfEditSession
{
public:
    CEditSessionBase(_In_ CSinstar3Tsf *pTextService, _In_ ITfContext *pContext);
    virtual ~CEditSessionBase();

    // ITfEditSession
    virtual STDMETHODIMP DoEditSession(TfEditCookie ec) = 0;

protected:
    ITfContext *_pContext;
    CSinstar3Tsf *_pTextService;

public:
	IUNKNOWN_BEGIN(ITfEditSession)
	IUNKNOWN_END()
};
