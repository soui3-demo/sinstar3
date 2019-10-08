#include "stdafx.h"
#include "sinstar3_tsf.h"

//+---------------------------------------------------------------------------
//
// OnInitDocumentMgr
//
// Sink called by the framework just before the first context is pushed onto
// a document.
//----------------------------------------------------------------------------

STDAPI CSinstar3Tsf::OnInitDocumentMgr(ITfDocumentMgr *pDocMgr)
{
	SLOGFMTI("OnInitDocumentMgr");

    return S_OK;
}

//+---------------------------------------------------------------------------
//
// OnUninitDocumentMgr
//
// Sink called by the framework just after the last context is popped off a
// document.
//----------------------------------------------------------------------------

STDAPI CSinstar3Tsf::OnUninitDocumentMgr(ITfDocumentMgr *pDocMgr)
{
	SLOGFMTI("OnUninitDocumentMgr");
    return S_OK;
}

//+---------------------------------------------------------------------------
//
// OnSetFocus
//
// Sink called by the framework when focus changes from one document to
// another.  Either document may be NULL, meaning previously there was no
// focus document, or now no document holds the input focus.
//----------------------------------------------------------------------------

STDAPI CSinstar3Tsf::OnSetFocus(ITfDocumentMgr *pDocMgrFocus, ITfDocumentMgr *pDocMgrPrevFocus)
{
	SLOGFMTI("ITfThreadMgrEventSink::OnSetFocus, pDocMgrFocus is %p, pDocMgrPrevFocus is %p", pDocMgrFocus, pDocMgrPrevFocus);
    //
    // Whenever focus is changed, initialize the TextEditSink.
    //
	_bInEditDocument = _InitTextEditSink(pDocMgrFocus);	
	
	if(m_pSinstar3)
	{
		m_pSinstar3->OnSetFocus(_bHasFocus && _bInEditDocument);
	}

    return S_OK;
}

STDAPI CSinstar3Tsf::OnSetThreadFocus()
{
	SLOGFMTI("OnSetThreadFocus");

	_bHasFocus=TRUE;
	if(m_pSinstar3)
	{
		m_pSinstar3->OnSetFocus(_bHasFocus && _bInEditDocument);
	}
	return S_OK;
}

STDAPI CSinstar3Tsf::OnKillThreadFocus()
{
	SLOGFMTI("OnKillThreadFocus");

	if (IsCompositing())
	{
		ITfContext *pCtx = GetImeContext();
		_EndComposition(pCtx);
		ReleaseImeContext(pCtx);
	}
	_bHasFocus = FALSE;
	if (m_pSinstar3)
	{
		m_pSinstar3->OnSetFocus(FALSE);
	}
	return S_OK;
}

//+---------------------------------------------------------------------------
//
// OnPushContext
//
// Sink called by the framework when a context is pushed.
//----------------------------------------------------------------------------

STDAPI CSinstar3Tsf::OnPushContext(ITfContext *pContext)
{
	SLOGFMTI("###PushContext =%p",pContext);
    return S_OK;
}

//+---------------------------------------------------------------------------
//
// OnPopContext
//
// Sink called by the framework when a context is popped.
//----------------------------------------------------------------------------

STDAPI CSinstar3Tsf::OnPopContext(ITfContext *pContext)
{
	SLOGFMTI("PopContext =%p",pContext);
    return S_OK;
}

//+---------------------------------------------------------------------------
//
// _InitThreadMgrEventSink
//
// Advise our sink.
//----------------------------------------------------------------------------

BOOL CSinstar3Tsf::_InitThreadMgrEventSink()
{
    SOUI::SComPtr<ITfSource> pSource;

    if (_pThreadMgr->QueryInterface(IID_ITfSource, (void **)&pSource) != S_OK)
        return FALSE;

	BOOL bRet = TRUE;
	HRESULT hr = S_OK;
    hr = pSource->AdviseSink(IID_ITfThreadMgrEventSink, (ITfThreadMgrEventSink *)this, &_dwThreadMgrEventSinkCookie);
	if (FAILED(hr))
	{
        _dwThreadMgrEventSinkCookie = TF_INVALID_COOKIE;
		bRet = FALSE;
	}
	hr = pSource->AdviseSink(IID_ITfThreadFocusSink, (ITfThreadFocusSink *)this, &_dwThreadFocusSinkCookie);
	if (FAILED(hr))
	{
		_dwThreadFocusSinkCookie = TF_INVALID_COOKIE;
		bRet = FALSE;
	}

    return bRet;
}

//+---------------------------------------------------------------------------
//
// _UninitThreadMgrEventSink
//
// Unadvise our sink.
//----------------------------------------------------------------------------

void CSinstar3Tsf::_UninitThreadMgrEventSink()
{
    ITfSource *pSource;

    if (_pThreadMgr->QueryInterface(IID_ITfSource, (void **)&pSource) == S_OK)
    {
		if (_dwThreadMgrEventSinkCookie != TF_INVALID_COOKIE)
		{
			pSource->UnadviseSink(_dwThreadMgrEventSinkCookie);
		}
		if (_dwThreadFocusSinkCookie != TF_INVALID_COOKIE)
		{
			pSource->UnadviseSink(_dwThreadFocusSinkCookie);
		}
        pSource->Release();

		_dwThreadMgrEventSinkCookie = TF_INVALID_COOKIE;
		_dwThreadFocusSinkCookie = TF_INVALID_COOKIE;
    }
}
