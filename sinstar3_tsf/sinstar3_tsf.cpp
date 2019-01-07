#include "stdafx.h"
#include "sinstar3_tsf.h"
#include "editsession.h"
#include "../helper/helper.h"
#define TIMERID_CHKDEFIME 100



/* static */
HRESULT CSinstar3Tsf::CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppvObj)
{
    CSinstar3Tsf *pCase;
    HRESULT hr;

    if (ppvObj == NULL)
        return E_INVALIDARG;

    *ppvObj = NULL;

    if (NULL != pUnkOuter)
        return CLASS_E_NOAGGREGATION;

    if ((pCase = new CSinstar3Tsf) == NULL)
        return E_OUTOFMEMORY;

    hr = pCase->QueryInterface(riid, ppvObj);

    pCase->Release(); // caller still holds ref if hr == S_OK

    return hr;
}

//+---------------------------------------------------------------------------
//
// ctor
//
//----------------------------------------------------------------------------

CSinstar3Tsf::CSinstar3Tsf()
{
    theModule->AddRef();

	Create(0, WS_POPUP, HWND_MESSAGE, 0, theModule->GetModule());
    //
    // Initialize the thread manager pointer.
    //
    _pThreadMgr = NULL;

    //
    // Initialize the numbers for ThreadMgrEventSink.
    //
    _dwThreadMgrEventSinkCookie = TF_INVALID_COOKIE;
	_dwThreadFocusSinkCookie = TF_INVALID_COOKIE;

    //
    // Initialize the numbers for TextEditSink.
    //
    _pTextEditSinkContext = NULL;
    _dwTextEditSinkCookie = TF_INVALID_COOKIE;
	//
	// for input mode cookie
	//
	_dwThreadInputModeSinkCookie = TF_INVALID_COOKIE;
	_dwThreadKeyboardSinkCookie = TF_INVALID_COOKIE;

    //
    // Initialize the composition object pointer.
    //

	_dwCookieTextLayoutSink = TF_INVALID_COOKIE;

	m_pSinstar3=NULL;

	_bHasFocus=FALSE;

	_bInKeyProc=FALSE;
	_bInEditDocument = FALSE;	
	
	_ec = 0;

	_bCompsiting = FALSE;

}

//+---------------------------------------------------------------------------
//
// dtor
//
//----------------------------------------------------------------------------

CSinstar3Tsf::~CSinstar3Tsf()
{
	Destroy();
	Deactivate();
    theModule->Release();
}

STDAPI CSinstar3Tsf::ActivateEx(ITfThreadMgr *pThreadMgr, TfClientId tfClientId, DWORD dwFlags)
{
	SLOGFMTI("Activate %p %d", pThreadMgr, (int)tfClientId);

	_pThreadMgr = pThreadMgr;
	_pThreadMgr->AddRef();
	_tfClientId = tfClientId;
	_dwActivateFlag = dwFlags;

	_bKeyDownTested = FALSE;
	_bKeyUpTested = FALSE;
	//
	// Initialize ThreadMgrEventSink.
	//
	if (!_InitThreadMgrEventSink())
		goto ExitError;

	// 
	//  If there is the focus document manager already,
	//  advise the TextEditSink.
	// 
	ITfDocumentMgr *pDocMgrFocus;
	if ((_pThreadMgr->GetFocus(&pDocMgrFocus) == S_OK) &&
		(pDocMgrFocus != NULL))
	{
		_bInEditDocument = _InitTextEditSink(pDocMgrFocus);
		pDocMgrFocus->Release();
	}

	//
	// Initialize KeyEventSink
	//
	if (!_InitKeyEventSink())
		goto ExitError;

	if (!_InitThreadCompartment())
		goto ExitError;

	OnSetThreadFocus();

	if (!_InitSinstar3())
		goto ExitError;


	return S_OK;

ExitError:
	Deactivate(); // cleanup any half-finished init
	return E_FAIL;
}

HWND CSinstar3Tsf::GetActiveWnd()
{
	HWND hWnd = NULL;
	SOUI::SComPtr<ITfDocumentMgr> pDocumentMgr;
	SOUI::SComPtr<ITfContext> pContext;
	SOUI::SComPtr<ITfContextView> pContextView;
	
	if(!_pThreadMgr) return NULL;
	HRESULT hr = _pThreadMgr->GetFocus(&pDocumentMgr);
	if(hr == S_OK) hr = pDocumentMgr->GetTop(&pContext);
	if(hr == S_OK) hr = pContext->GetActiveView(&pContextView);
	if(hr == S_OK) hr = pContextView->GetWnd(&hWnd);

	if (hWnd == NULL) hWnd = ::GetFocus();
	return hWnd;
}

STDAPI CSinstar3Tsf::Activate(ITfThreadMgr *pThreadMgr, TfClientId tfClientId)
{
	return ActivateEx(pThreadMgr, tfClientId,0);
}

//+---------------------------------------------------------------------------
//
// Deactivate
//
//----------------------------------------------------------------------------

STDAPI CSinstar3Tsf::Deactivate()
{
	SLOGFMTI("Deactivate");
	//确保输入过程结束
	if(_IsComposing())
	{
		SASSERT(_pThreadMgr);
		UINT64 nCtx = GetImeContext();
		if(nCtx) 
			EndComposition(nCtx);
		ReleaseImeContext(nCtx);
	}
	if (_pThreadMgr)
	{
		_InitTextEditSink(NULL);
		_UninitThreadCompartment();
		_UninitThreadMgrEventSink();
		_UninitKeyEventSink();
	}

    if (_pThreadMgr != NULL)
    {
		OnKillThreadFocus();
		_UninitSinstar3();
		_pThreadMgr->Release();
		_pThreadMgr = NULL;
	}

    _tfClientId = TF_CLIENTID_NULL;
	_bInEditDocument = FALSE;

    return S_OK;
}

STDMETHODIMP CSinstar3Tsf::OnLayoutChange(ITfContext *pContext, TfLayoutCode lcode, ITfContextView *pContextView)
{
	SLOG_INFO("OnLayoutChange _pComposition:"<<_pComposition);
	switch (lcode)
	{
	case TF_LC_CHANGE:
		{
			SLOGFMTI("TF_LC_CHANGE");
			CEsGetTextExtent *pEditSession  = new CEsGetTextExtent(this, pContext, pContextView);
			HRESULT hrSession;
			pContext->RequestEditSession(_tfClientId, pEditSession, TF_ES_SYNC | TF_ES_READ, &hrSession);
			pEditSession->Release();
		}

		break;

	case TF_LC_DESTROY:
		{
			SLOGFMTI("TF_LC_DESTROY");
		}
		break;

	case TF_LC_CREATE:
		{
			SLOGFMTI("TF_LC_CREATE");
		}
		break;

	default:
		break;
	}

	return S_OK;
}

STDMETHODIMP CSinstar3Tsf::Show(	HWND hwndParent,LANGID langid,REFGUID rguidProfile)
{
	return S_OK;
}

STDMETHODIMP CSinstar3Tsf::GetDisplayName(BSTR* pbstrName)
{
	*pbstrName=SysAllocString(L"Sinstar3");
	return S_OK;
}



//ITextService

BOOL CSinstar3Tsf::InputStringW(LPCWSTR pszBuf, int nLen) {
	if(IsCompositing())
	{
		return FALSE;
	}else 
	{
		UINT64 nCtx = GetImeContext();
		if(nCtx)
		{
			StartComposition(nCtx);
			UpdateResultAndCompositionStringW(nCtx,pszBuf,nLen,NULL,0);
			EndComposition(nCtx);
			ReleaseImeContext(nCtx);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CSinstar3Tsf::IsCompositing() const
{
	return _bCompsiting;
}


HRESULT CSinstar3Tsf::_AdviseTextLayoutSink(ITfContext *pContext)
{
	if(_dwCookieTextLayoutSink != TF_INVALID_COOKIE)
		return S_OK;

	SOUI::SComPtr<ITfSource> pSource;

	if (FAILED(pContext->QueryInterface(IID_ITfSource, (void **)&pSource)))
		return E_FAIL;

	if (FAILED(pSource->AdviseSink(IID_ITfTextLayoutSink, (ITfTextLayoutSink *)this, &_dwCookieTextLayoutSink)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSinstar3Tsf::_UnadviseTextLayoutSink(ITfContext *pContext)
{
	SOUI::SComPtr<ITfSource> pSource;

	if(_dwCookieTextLayoutSink==TF_INVALID_COOKIE)
		return S_FALSE;

	if (FAILED(pContext->QueryInterface(IID_ITfSource, (void **)&pSource)))
		return E_FAIL;

	if (FAILED(pSource->UnadviseSink(_dwCookieTextLayoutSink)))
		return E_FAIL;

	_dwCookieTextLayoutSink = TF_INVALID_COOKIE;
	return S_OK;
}

void CSinstar3Tsf::StartComposition(UINT64 imeContext)
{
	_StartComposition((ITfContext*)imeContext);
}


void CSinstar3Tsf::EndComposition(UINT64 imeContext)
{
	_EndComposition((ITfContext*)imeContext);
}


void CSinstar3Tsf::ReplaceSelCompositionW(UINT64 imeContext,int nLeft,int nRight,const WCHAR* wszComp,int nLen)
{
	_ChangeComposition((ITfContext*)imeContext,nLeft,nRight,NULL,0);
}

void CSinstar3Tsf::UpdateResultAndCompositionStringW(UINT64 imeContext,const WCHAR *wszResultStr,int nResStrLen,const WCHAR *wszCompStr,int nCompStrLen)
{
	_UpdateResultAndCompositionStringW((ITfContext*)imeContext,wszResultStr,nResStrLen,wszCompStr,nCompStrLen);
}


UINT64 CSinstar3Tsf::GetImeContext()
{

	if(!_pThreadMgr) return NULL;
	HRESULT         hr;

	if(_pComposition)
	{
		ITfRange *pRange;
		ITfContext *pContext;
		hr=_pComposition->GetRange(&pRange);
		if(SUCCEEDED(hr) && pRange)
		{
			hr=pRange->GetContext(&pContext);
			pRange->Release();
			if(SUCCEEDED(hr)&&pContext)	return (UINT64)pContext;
		}
	}else
	{
		ITfDocumentMgr  *pFocusDoc;
		hr = _pThreadMgr->GetFocus(&pFocusDoc);
		if(SUCCEEDED(hr) && pFocusDoc != NULL)
		{
			ITfContext *pContext;
			hr = pFocusDoc->GetTop(&pContext);
			pFocusDoc->Release();
			if(SUCCEEDED(hr))
			{
				return (UINT64)pContext;
			}
		}
	}
	return NULL;
}

BOOL   CSinstar3Tsf::ReleaseImeContext(UINT64 imeContext)
{
	if(!imeContext) return FALSE;
	ITfContext *pContext=(ITfContext*)imeContext;
	try
	{
		pContext->Release();
	}
	catch (...)
	{
		
	}
	return TRUE;
}

BOOL CSinstar3Tsf::_InitSinstar3()
{
	m_pSinstar3 = new CSinstarProxy(this);
	Helper_ChangeWindowMessageFilter(UM_CALL_FUN, MSGFLT_ADD);

	m_pSinstar3->Init(m_hWnd, theModule->GetSvrPath());

	m_pSinstar3->OnIMESelect(_bHasFocus);
 	m_pSinstar3->OnSetFocus(_bHasFocus && _bInEditDocument);
 	OnChange(GUID_COMPARTMENT_KEYBOARD_OPENCLOSE);
 	OnChange(GUID_COMPARTMENT_KEYBOARD_INPUTMODE_CONVERSION);
	return TRUE;
}

BOOL CSinstar3Tsf::_UninitSinstar3()
{
	if(m_pSinstar3)
	{
		m_pSinstar3->OnIMESelect(FALSE);
		delete m_pSinstar3;
		m_pSinstar3=NULL;
	}
	return TRUE;
}

LRESULT CSinstar3Tsf::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result;
	BOOL bHandled = m_pSinstar3->ProcessWindowMessage(m_hWnd, uMsg, wParam, lParam, result);
	if (bHandled) return result;
	return __super::WindowProc(uMsg, wParam, lParam);
}

BOOL CSinstar3Tsf::SetOpenStatus(UINT64 imeContext,BOOL bOpen)
{
	return _SetKeyboardOpen(bOpen);
}

BOOL CSinstar3Tsf::GetOpenStatus(UINT64 imeContext) const
{
	return _IsKeyboardOpen();
}

void CSinstar3Tsf::OnStartComposition(TfEditCookie ec,ITfComposition *pComposition)
{
	_pComposition = pComposition;
	if(m_pSinstar3) m_pSinstar3->OnCompositionStarted();
}
