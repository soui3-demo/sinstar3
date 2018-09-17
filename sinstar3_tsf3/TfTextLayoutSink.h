// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

class CSinstar3Tsf;

class CTfTextLayoutSink : public CUnknown, public ITfTextLayoutSink
{
public:
    CTfTextLayoutSink(_In_ CSinstar3Tsf *pTextService);
    virtual ~CTfTextLayoutSink();

    // ITfTextLayoutSink
    STDMETHODIMP OnLayoutChange(_In_ ITfContext *pContext, TfLayoutCode lcode, _In_ ITfContextView *pContextView);

    HRESULT _StartLayout(_In_ ITfContext *pContextDocument, TfEditCookie ec, _In_ ITfRange *pRangeComposition);
    VOID _EndLayout();

    HRESULT _GetTextExt(_Out_ RECT *lpRect);
    ITfContext* _GetContextDocument() { return _pContextDocument; };

    virtual VOID _LayoutChangeNotification(_In_ RECT *lpRect) = 0;
    virtual VOID _LayoutDestroyNotification() = 0;

private:
    HRESULT _AdviseTextLayoutSink();
    HRESULT _UnadviseTextLayoutSink();

private:
    ITfRange* _pRangeComposition;
    ITfContext* _pContextDocument;
    TfEditCookie _tfEditCookie;
    CSinstar3Tsf* _pTextService;
    DWORD _dwCookieTextLayoutSink;
public:
	IUNKNOWN_BEGIN(ITfTextLayoutSink)
	IUNKNOWN_END()
};
