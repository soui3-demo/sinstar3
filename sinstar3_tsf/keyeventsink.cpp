#include "stdafx.h"
#include "sinstar3_tsf.h"
#include "editsession.h"

STDAPI CSinstar3Tsf::OnSetFocus(BOOL fForeground)
{
	SLOG_INFO("ITfKeyEventSink::OnSetFocus, fForeground:"<<fForeground);
    return S_OK;
}

STDAPI CSinstar3Tsf::OnTestKeyDown(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BOOL *pfEaten)
{
	if (!m_pSinstar3 || _IsKeyboardDisabled()/* || !_IsKeyboardOpen()*/)
	{
		*pfEaten = FALSE;
		return S_OK;
	}

	m_pSinstar3->ProcessKeyStoke(pContext,(UINT)wParam,lParam, TRUE, pfEaten);

	SLOGFMTI("OnTestKeyDown: wParam:%08x,lparam:%08x",(DWORD)wParam, (DWORD)lParam);
    return S_OK;
}

STDAPI CSinstar3Tsf::OnTestKeyUp(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BOOL *pfEaten)
{
	if (!m_pSinstar3 || _IsKeyboardDisabled()/* || !_IsKeyboardOpen()*/)
	{
		*pfEaten = FALSE;
		return S_OK;
	}

	m_pSinstar3->ProcessKeyStoke(pContext,(UINT)wParam, lParam,FALSE, pfEaten);

	return S_OK;
}


STDAPI CSinstar3Tsf::OnKeyDown(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BOOL *pfEaten)
{
	HRESULT hr = E_FAIL;
	SLOGFMTI("OnKeyDown: %08x %08x", (DWORD)wParam, (DWORD)lParam);
	_bInKeyProc=TRUE;

	OnTestKeyDown(pContext,wParam,lParam,pfEaten);
 	if(*pfEaten)
	{
		m_pSinstar3->TranslateKey(pContext,(UINT)wParam, MapVirtualKey((UINT)wParam,0), TRUE, pfEaten);
	}
	_bInKeyProc=FALSE;
	return S_OK;
}

STDAPI CSinstar3Tsf::OnKeyUp(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BOOL *pfEaten)
{
	SLOGFMTI("OnKeyUp: %08x %08x", (DWORD)wParam, (DWORD)lParam);
	_bInKeyProc=TRUE;
	OnTestKeyUp(pContext,wParam,lParam,pfEaten);
	if(*pfEaten)
	{
		m_pSinstar3->TranslateKey(pContext,(UINT)wParam, MapVirtualKey((UINT)wParam,0), FALSE, pfEaten);
	}
	_bInKeyProc=FALSE;

    return S_OK;
}

STDAPI CSinstar3Tsf::OnPreservedKey(ITfContext *pic, REFGUID rguid, BOOL *pfEaten)
{
	SLOGFMTI("OnPreservedKey");
	*pfEaten=m_pSinstar3->OnHotkey(pic,rguid);
	return S_OK;
}

BOOL CSinstar3Tsf::_InitKeyEventSink()
{
    ITfKeystrokeMgr *pKeystrokeMgr;
    HRESULT hr;

    if (_pThreadMgr->QueryInterface(IID_ITfKeystrokeMgr, (void **)&pKeystrokeMgr) != S_OK)
        return FALSE;

    hr = pKeystrokeMgr->AdviseKeyEventSink(_tfClientId, (ITfKeyEventSink *)this, TRUE);

    pKeystrokeMgr->Release();

    return (hr == S_OK);
}

void CSinstar3Tsf::_UninitKeyEventSink()
{
    ITfKeystrokeMgr *pKeystrokeMgr;

    if (_pThreadMgr->QueryInterface(IID_ITfKeystrokeMgr, (void **)&pKeystrokeMgr) != S_OK)
        return;

    pKeystrokeMgr->UnadviseKeyEventSink(_tfClientId);

    pKeystrokeMgr->Release();
}
