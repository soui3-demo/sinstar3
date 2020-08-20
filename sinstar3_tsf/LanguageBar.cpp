// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
#include "stdafx.h"
//#include "sinstar3_tsf.h"
#include "LanguageBar.h"
#include "Globals.h"
#include "TsfModule.h"
#include "resource.h"

const GUID GUID_LBI_INPUTMODE =
{ 0x2C77A81E, 0x41CC, 0x4178, { 0xA3, 0xA7, 0x5F, 0x8A, 0x98, 0x75, 0x68, 0xE6 } };	

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
	_tfLangBarItemInfo.guidItem = GUID_LBI_INPUTMODE;   // GUID of this LangBarItem.
	_tfLangBarItemInfo.dwStyle = TF_LBI_STYLE_BTN_BUTTON|TF_LBI_STYLE_SHOWNINTRAY;   
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
	*pbstrToolTip = SysAllocString(_tfLangBarItemInfo.szDescription);

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
	*pbstrText = SysAllocString(_tfLangBarItemInfo.szDescription);

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
	if(_status == bOn)
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

CLangBarItemButton* CLangBarItemButton::_InitLanguageBar(ITfThreadMgr *pThreadMgr)
{
	ITfLangBarItemMgr *pLangBarItemMgr;
	CLangBarItemButton *pRet=NULL;

	if (pThreadMgr->QueryInterface(IID_ITfLangBarItemMgr, (void **)&pLangBarItemMgr) != S_OK)
		return pRet;

	if ((pRet = new CLangBarItemButton(IDI_IME_ON,IDI_IME_OFF,L"input state")) == NULL)
		goto Exit;

	if (pLangBarItemMgr->AddItem(pRet) != S_OK)
	{
		pRet->Release();
		pRet = NULL;
		goto Exit;
	}

Exit:
	pLangBarItemMgr->Release();
	return pRet;
}

//+---------------------------------------------------------------------------
//
// _UninitLanguageBar
//
//----------------------------------------------------------------------------

bool CLangBarItemButton::_UninitLanguageBar(ITfThreadMgr *pThreadMgr,CLangBarItemButton *pLangBarItem)
{
	ITfLangBarItemMgr *pLangBarItemMgr;

	if (pLangBarItem == NULL)
		return false;

	if (pThreadMgr->QueryInterface(IID_ITfLangBarItemMgr, (void **)&pLangBarItemMgr) == S_OK)
	{
		pLangBarItemMgr->RemoveItem(pLangBarItem);
		pLangBarItemMgr->Release();
		return true;
	}
	return false;
}
