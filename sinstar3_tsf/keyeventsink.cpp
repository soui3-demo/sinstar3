#include "stdafx.h"
#include "sinstar3_tsf.h"

STDAPI CBaiduJPTSF::OnSetFocus(BOOL fForeground)
{
	Helper_Trace(L"ITfKeyEventSink::OnSetFocus, fForeground: %d", fForeground);
    return S_OK;
}

STDAPI CBaiduJPTSF::OnTestKeyDown(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BOOL *pfEaten)
{
	if (!m_pBaiduJP3 || _IsKeyboardDisabled()/* || !_IsKeyboardOpen()*/)
	{
		*pfEaten = FALSE;
		return S_OK;
	}

	m_pBaiduJP3->ProcessKeyStoke(pContext,wParam,lParam, TRUE, pfEaten);

	Helper_Trace(_T("OnTestKeyDown: wParam:%08x,lparam:%08x"),wParam,lParam);
    return S_OK;
}
STDAPI CBaiduJPTSF::OnTestKeyUp(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BOOL *pfEaten)
{
	if (!m_pBaiduJP3 || _IsKeyboardDisabled()/* || !_IsKeyboardOpen()*/)
	{
		*pfEaten = FALSE;
		return S_OK;
	}

	m_pBaiduJP3->ProcessKeyStoke(pContext,wParam, lParam,FALSE, pfEaten);

	return S_OK;
}

#include "editsession.h"

STDAPI CBaiduJPTSF::OnKeyDown(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BOOL *pfEaten)
{
	HRESULT hr = E_FAIL;
	Helper_Trace(L"OnKeyDown: %x %x", wParam, lParam);
	_bInKeyProc=TRUE;

	OnTestKeyDown(pContext,wParam,lParam,pfEaten);
 	if(*pfEaten)
	{
		m_pBaiduJP3->TranslateKey(pContext,wParam, MapVirtualKey(wParam,0), TRUE, pfEaten);
	}
	_bInKeyProc=FALSE;
	return S_OK;
}

STDAPI CBaiduJPTSF::OnKeyUp(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BOOL *pfEaten)
{
	Helper_Trace(L"OnKeyUp: %x %x", wParam, lParam);
	OnTestKeyUp(pContext,wParam,lParam,pfEaten);
	if(*pfEaten)
	{
		m_pBaiduJP3->TranslateKey(pContext,wParam, MapVirtualKey(wParam,0), FALSE, pfEaten);
	}
    return S_OK;
}

STDAPI CBaiduJPTSF::OnPreservedKey(ITfContext *pic, REFGUID rguid, BOOL *pfEaten)
{
	Helper_Trace(L"OnPreservedKey");
	*pfEaten=m_pBaiduJP3->OnHotkey(pic,rguid);
	return S_OK;
}

BOOL CBaiduJPTSF::_InitKeyEventSink()
{
    ITfKeystrokeMgr *pKeystrokeMgr;
    HRESULT hr;

    if (_pThreadMgr->QueryInterface(IID_ITfKeystrokeMgr, (void **)&pKeystrokeMgr) != S_OK)
        return FALSE;

    hr = pKeystrokeMgr->AdviseKeyEventSink(_tfClientId, (ITfKeyEventSink *)this, TRUE);

    pKeystrokeMgr->Release();

    return (hr == S_OK);
}

void CBaiduJPTSF::_UninitKeyEventSink()
{
    ITfKeystrokeMgr *pKeystrokeMgr;

    if (_pThreadMgr->QueryInterface(IID_ITfKeystrokeMgr, (void **)&pKeystrokeMgr) != S_OK)
        return;

    pKeystrokeMgr->UnadviseKeyEventSink(_tfClientId);

    pKeystrokeMgr->Release();
}
