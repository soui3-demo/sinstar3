// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
#include "stdafx.h"
#include "sinstar3_tsf.h"
#include "LanguageBar.h"
#include "Globals.h"
#include "TsfModule.h"
#include "resource.h"

#define LANGBAR_ITEM_DESC   L"Sample Text Service Button"

//+---------------------------------------------------------------------------
//
// ctor
//
//----------------------------------------------------------------------------

CLangBarItemButton::CLangBarItemButton(int iconOn,int iconOff,LPCWSTR pszIconDesc)
:_status(false)
,_iconIdOn(iconOn)
,_iconIdOff(iconOff)
{
	theModule->AddRef();

	//
	// initialize TF_LANGBARITEMINFO structure.
	//
	_tfLangBarItemInfo.clsidService = c_clsidSinstar3TSF;    // This LangBarItem belongs to this TextService.
	_tfLangBarItemInfo.guidItem = c_guidLangBarItemButton;   // GUID of this LangBarItem.
	_tfLangBarItemInfo.dwStyle = TF_LBI_STYLE_BTN_MENU;      // This LangBar is a button type with a menu.
	_tfLangBarItemInfo.ulSort = 0;                           // The position of this LangBar Item is not specified.
	wcscpy_s(_tfLangBarItemInfo.szDescription, ARRAYSIZE(_tfLangBarItemInfo.szDescription), pszIconDesc);                        // Set the description of this LangBar Item.

	// Initialize the sink pointer to NULL.
	_pLangBarItemSink = NULL;
}

//+---------------------------------------------------------------------------
//
// dtor
//
//----------------------------------------------------------------------------

CLangBarItemButton::~CLangBarItemButton()
{
	theModule->Release();
}


//+---------------------------------------------------------------------------
//
// GetInfo
//
//----------------------------------------------------------------------------

STDAPI CLangBarItemButton::GetInfo(TF_LANGBARITEMINFO *pInfo)
{
	*pInfo = _tfLangBarItemInfo;
	return S_OK;
}

//+---------------------------------------------------------------------------
//
// GetStatus
//
//----------------------------------------------------------------------------

STDAPI CLangBarItemButton::GetStatus(DWORD *pdwStatus)
{
	*pdwStatus = 0;
	return S_OK;
}

//+---------------------------------------------------------------------------
//
// Show
//
//----------------------------------------------------------------------------

STDAPI CLangBarItemButton::Show(BOOL fShow)
{
	return E_NOTIMPL;
}

//+---------------------------------------------------------------------------
//
// GetTooltipString
//
//----------------------------------------------------------------------------

STDAPI CLangBarItemButton::GetTooltipString(BSTR *pbstrToolTip)
{
	*pbstrToolTip = SysAllocString(LANGBAR_ITEM_DESC);

	return (*pbstrToolTip == NULL) ? E_OUTOFMEMORY : S_OK;
}

//+---------------------------------------------------------------------------
//
// OnClick
//
//----------------------------------------------------------------------------

STDAPI CLangBarItemButton::OnClick(TfLBIClick click, POINT pt, const RECT *prcArea)
{
	return S_OK;
}

//+---------------------------------------------------------------------------
//
// InitMenu
//
//----------------------------------------------------------------------------

STDAPI CLangBarItemButton::InitMenu(ITfMenu *pMenu)
{

	return S_OK;
}

//+---------------------------------------------------------------------------
//
// OnMenuSelect
//
//----------------------------------------------------------------------------

STDAPI CLangBarItemButton::OnMenuSelect(UINT wID)
{

	return S_OK;
}

//+---------------------------------------------------------------------------
//
// GetIcon
//
//----------------------------------------------------------------------------

STDAPI CLangBarItemButton::GetIcon(HICON *phIcon)
{
	*phIcon = (HICON)LoadImage(theModule->GetModule(), MAKEINTRESOURCE(_status?_iconIdOn:_iconIdOff), IMAGE_ICON, 16, 16, 0);

	return (*phIcon != NULL) ? S_OK : E_FAIL;
}

//+---------------------------------------------------------------------------
//
// GetText
//
//----------------------------------------------------------------------------

STDAPI CLangBarItemButton::GetText(BSTR *pbstrText)
{
	*pbstrText = SysAllocString(LANGBAR_ITEM_DESC);

	return (*pbstrText == NULL) ? E_OUTOFMEMORY : S_OK;
}

//+---------------------------------------------------------------------------
//
// AdviseSink
//
//----------------------------------------------------------------------------

#define TEXTSERVICE_LANGBARITEMSINK_COOKIE 0x0fab0fac

STDAPI CLangBarItemButton::AdviseSink(REFIID riid, IUnknown *punk, DWORD *pdwCookie)
{
	//
	// Allow only ITfLangBarItemSink interface.
	//
	if (!IsEqualIID(IID_ITfLangBarItemSink, riid))
		return CONNECT_E_CANNOTCONNECT;

	//
	// support only one sink.
	//
	if (_pLangBarItemSink != NULL)
		return CONNECT_E_ADVISELIMIT;

	//
	// Query the ITfLangBarItemSink interface and store it into _pLangBarItemSink.
	//
	if (punk->QueryInterface(IID_ITfLangBarItemSink, (void **)&_pLangBarItemSink) != S_OK)
	{
		_pLangBarItemSink = NULL;
		return E_NOINTERFACE;
	}

	//
	// return our cookie.
	//
	*pdwCookie = TEXTSERVICE_LANGBARITEMSINK_COOKIE;
	return S_OK;
}

//+---------------------------------------------------------------------------
//
// UnadviseSink
//
//----------------------------------------------------------------------------

STDAPI CLangBarItemButton::UnadviseSink(DWORD dwCookie)
{
	// 
	// Check the given cookie.
	// 
	if (dwCookie != TEXTSERVICE_LANGBARITEMSINK_COOKIE)
		return CONNECT_E_NOCONNECTION;

	//
	// If there is no connected sink, just fail.
	//
	if (_pLangBarItemSink == NULL)
		return CONNECT_E_NOCONNECTION;

	_pLangBarItemSink->Release();
	_pLangBarItemSink = NULL;

	return S_OK;
}

void CLangBarItemButton::SetStatus(bool bOn)
{
	if(_status = bOn)
		return;
	_status = bOn;
	if (_pLangBarItemSink) 
	{
		_pLangBarItemSink->OnUpdate(TF_LBI_STATUS | TF_LBI_ICON);
	}

	return;
}

//+---------------------------------------------------------------------------
//
// _InitLanguageBar
//
//----------------------------------------------------------------------------

BOOL CSinstar3Tsf::_InitLanguageBar()
{
	ITfLangBarItemMgr *pLangBarItemMgr;
	BOOL fRet;

	if (_pThreadMgr->QueryInterface(IID_ITfLangBarItemMgr, (void **)&pLangBarItemMgr) != S_OK)
		return FALSE;

	fRet = FALSE;

	if ((_pLangBarItem = new CLangBarItemButton(IDI_IME_ON,IDI_IME_OFF,L"input state")) == NULL)
		goto Exit;

	if (pLangBarItemMgr->AddItem(_pLangBarItem) != S_OK)
	{
		_pLangBarItem->Release();
		_pLangBarItem = NULL;
		goto Exit;
	}

	fRet = TRUE;

Exit:
	pLangBarItemMgr->Release();
	return fRet;
}

//+---------------------------------------------------------------------------
//
// _UninitLanguageBar
//
//----------------------------------------------------------------------------

void CSinstar3Tsf::_UninitLanguageBar()
{
	ITfLangBarItemMgr *pLangBarItemMgr;

	if (_pLangBarItem == NULL)
		return;

	if (_pThreadMgr->QueryInterface(IID_ITfLangBarItemMgr, (void **)&pLangBarItemMgr) == S_OK)
	{
		pLangBarItemMgr->RemoveItem(_pLangBarItem);
		pLangBarItemMgr->Release();
	}

	_pLangBarItem->Release();
	_pLangBarItem = NULL;
}
