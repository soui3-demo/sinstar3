// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

#include "../include/unknown.h"
//+---------------------------------------------------------------------------
//
// CLangBarItemButton class
//
//----------------------------------------------------------------------------

class CLangBarItemButton : public ITfLangBarItemButton,
	public ITfSource, public CUnknown
{
public:
	CLangBarItemButton(int iconOn,int iconOff,LPCWSTR pszIconDesc);
	~CLangBarItemButton();

	void SetStatus(bool bOn);

	// ITfLangBarItem
	STDMETHODIMP GetInfo(TF_LANGBARITEMINFO *pInfo);
	STDMETHODIMP GetStatus(DWORD *pdwStatus);
	STDMETHODIMP Show(BOOL fShow);
	STDMETHODIMP GetTooltipString(BSTR *pbstrToolTip);

	// ITfLangBarItemButton
	STDMETHODIMP OnClick(TfLBIClick click, POINT pt, const RECT *prcArea);
	STDMETHODIMP InitMenu(ITfMenu *pMenu);
	STDMETHODIMP OnMenuSelect(UINT wID);
	STDMETHODIMP GetIcon(HICON *phIcon);
	STDMETHODIMP GetText(BSTR *pbstrText);

	// ITfSource
	STDMETHODIMP AdviseSink(REFIID riid, IUnknown *punk, DWORD *pdwCookie);
	STDMETHODIMP UnadviseSink(DWORD dwCookie);

private:
	ITfLangBarItemSink *_pLangBarItemSink;
	TF_LANGBARITEMINFO _tfLangBarItemInfo;

	int				 _iconIdOn;
	int				 _iconIdOff;
	bool 			 _status;
public:
	IUNKNOWN_BEGIN(ITfLangBarItemButton)
		IUNKNOWN_ADD_IID(ITfLangBarItem)
		IUNKNOWN_ADD_IID(ITfSource)
	IUNKNOWN_END()
};