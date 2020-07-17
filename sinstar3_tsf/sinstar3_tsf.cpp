#include "stdafx.h"
#include "sinstar3_tsf.h"
#include "editsession.h"
#include "../helper/helper.h"
#include "../include/global.h"
#include "Minidump.h"
#include "EnumDisplayAttributeInfo.h"
#include "DisplayAttributeInfo.h"
#include "Shlwapi.h"

static bool g_bInstallDump = false;
#define UM_ASYNC_FOCUS	(WM_USER+8000)

/* static */
HRESULT CSinstar3Tsf::CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppvObj)
{
    CSinstar3Tsf *pCase;
    HRESULT hr;

	if(!g_bInstallDump)
	{
		g_bInstallDump = true;
		
		TCHAR szModule[MAX_PATH];
		GetModuleFileName(NULL,szModule,MAX_PATH);
		LPCTSTR pName=_tcsrchr(szModule,'\\');
		if(pName)
			pName++;
		else
			pName=szModule;

		TCHAR szPath[MAX_PATH];
		_stprintf_s(szPath,MAX_PATH,_T("%s\\log\\%s.dmp"),theModule->GetInstallPath(),pName);
		CMinidump::SetDumpFile(szPath);
		CMinidump::Enable();
	}

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
	
	_bCompositing = FALSE;

	_gaDisplayAttributeInput = 0;

	Create(theModule->GetModule());
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

	if (!_InitDisplayAttributeGuidAtom())
		goto ExitError;

	OnSetThreadFocus();

	Helper_ChangeWindowMessageFilter(SOUI::UM_CALL_FUN, MSGFLT_ADD);
	Helper_ChangeWindowMessageFilter(UM_GETPROCPATH, MSGFLT_ADD);
	Helper_ChangeWindowMessageFilter(UM_RECONN, MSGFLT_ADD);

	if(!_InitSinstar3((HWND)GetActiveWnd()))
	{
		SetTimer(m_hWnd,TID_INIT,200,NULL);
	}


	return S_OK;

ExitError:
	Deactivate(); // cleanup any half-finished init
	return E_FAIL;
}

DWORD CSinstar3Tsf::GetActiveWnd() const
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
	return (DWORD)(ULONG_PTR)hWnd;
}

void CSinstar3Tsf::_SyncFocus(BOOL bFocus)
{
	_bHasFocus=bFocus;
	PostMessage(UM_ASYNC_FOCUS,0,0);
}

void CSinstar3Tsf::OnAsyncFocus()
{
	if(m_pSinstar3)
	{
		BOOL bFocus = _bHasFocus && _bInEditDocument;
		SLOG_INFO("OnAsyncFocus,hasFocus:"<<_bHasFocus<<" inEdit:"<<_bInEditDocument);
		if(bFocus)
			m_pSinstar3->OnSetFocus(TRUE,GetActiveWnd());
		else
			m_pSinstar3->OnSetFocus(FALSE,0);
	}
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
	KillTimer(m_hWnd,TID_INIT);
	//确保输入过程结束
	if(_IsCompositing())
	{
		SASSERT(_pThreadMgr);
		ITfContext * pCtx = GetImeContext();
		if(pCtx) 
		{	
			_EndComposition(pCtx);
			ReleaseImeContext(pCtx);
		}
	}
	if (_pThreadMgr)
	{
		_InitTextEditSink(NULL);
		_UninitThreadCompartment();
		_UninitThreadMgrEventSink();
		_UninitKeyEventSink();
	}

    if (_pThreadMgr)
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


HRESULT CSinstar3Tsf::_AdviseTextLayoutSink(ITfContext *pContext)
{
	if (_dwCookieTextLayoutSink != TF_INVALID_COOKIE)
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

	if (_dwCookieTextLayoutSink == TF_INVALID_COOKIE)
		return S_FALSE;

	if (FAILED(pContext->QueryInterface(IID_ITfSource, (void **)&pSource)))
		return E_FAIL;

	if (FAILED(pSource->UnadviseSink(_dwCookieTextLayoutSink)))
		return E_FAIL;

	_dwCookieTextLayoutSink = TF_INVALID_COOKIE;
	return S_OK;
}


BOOL CSinstar3Tsf::_UninitSinstar3()
{
	if (m_pSinstar3)
	{
		m_pSinstar3->OnIMESelect(FALSE);
		delete m_pSinstar3;
		m_pSinstar3 = NULL;
	}
	return TRUE;
}

BOOL CSinstar3Tsf::_InitSinstar3(HWND hWnd)
{
	SASSERT(!m_pSinstar3);
	if(!PathFileExists(theModule->GetSvrPath()) && GetLastError()!=5)//last error 5 is access deny
		return FALSE;
	m_pSinstar3 = new CSinstarProxy(this);

	if (!m_pSinstar3->Init(m_hWnd, theModule->GetSvrPath()))
	{
		delete m_pSinstar3;
		m_pSinstar3 = NULL;
		return FALSE;
	}
	OnChange(GUID_COMPARTMENT_KEYBOARD_OPENCLOSE);
	OnChange(GUID_COMPARTMENT_KEYBOARD_INPUTMODE_CONVERSION);

	m_pSinstar3->NotifyScaleInfo(hWnd);

	m_pSinstar3->OnIMESelect(TRUE);
	_SyncFocus(_bHasFocus);
	return TRUE;
}

void CSinstar3Tsf::OnReconnReady()
{
	if(_pThreadMgr)
	{
		_UninitSinstar3();
		_InitSinstar3((HWND)GetActiveWnd());
		SetOpenStatus(TRUE);//auto open ime
	}
}

LRESULT CSinstar3Tsf::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;
	if(uMsg == WM_TIMER)
	{
		if(wParam == TID_INIT)
		{
			if(_InitSinstar3((HWND)GetActiveWnd()))
			{
				KillTimer(m_hWnd,wParam);
			}
			return 0;
		}
	}else if(uMsg == UM_ASYNC_FOCUS)
	{
		OnAsyncFocus();
		return 0;
	}
	if (m_pSinstar3)
	{
		BOOL bHandled = m_pSinstar3->ProcessWindowMessage(m_hWnd, uMsg, wParam, lParam, result);
		if (bHandled) return result;
	}
	return CSimpleWnd::WindowProc(uMsg, wParam, lParam);
}
//ITextService

BOOL CSinstar3Tsf::InputStringW(LPCWSTR pszBuf, int nLen) {
	if(IsCompositing())
	{
		SLOG_WARN("not in compositing!!!");
		return FALSE;
	}else 
	{
		ITfContext * pCtx = GetImeContext();
		SLOG_INFO("imeContext:"<<pCtx);
		if(pCtx)
		{
			_StartComposition(pCtx);
			_UpdateResultAndCompositionStringW(pCtx,pszBuf,nLen,NULL,0);
			_EndComposition(pCtx);
			ReleaseImeContext(pCtx);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CSinstar3Tsf::IsCompositing() const
{
	SLOG_INFO("bCompositing:"<< _bCompositing);
	return _bCompositing;
}

void CSinstar3Tsf::StartComposition(UINT64 imeContext)
{
	SLOG_INFO("imeContext:" << imeContext);
	_StartComposition((ITfContext*)imeContext);
}


void CSinstar3Tsf::EndComposition(UINT64 imeContext)
{
	SLOG_INFO("imeContext:" << imeContext);
	_EndComposition((ITfContext*)imeContext);
}


void CSinstar3Tsf::ReplaceSelCompositionW(UINT64 imeContext,int nLeft,int nRight,const WCHAR* wszComp,int nLen)
{
	SLOG_INFO("imeContext:" << imeContext);
	_ChangeComposition((ITfContext*)imeContext,nLeft,nRight,NULL,0);
}

void CSinstar3Tsf::UpdateResultAndCompositionStringW(UINT64 imeContext,const WCHAR *wszResultStr,int nResStrLen,const WCHAR *wszCompStr,int nCompStrLen)
{
	SLOG_INFO("imeContext:" << imeContext<<" resultStr:"<<wszResultStr);
	_UpdateResultAndCompositionStringW((ITfContext*)imeContext,wszResultStr,nResStrLen,wszCompStr,nCompStrLen);
}


ITfContext * CSinstar3Tsf::GetImeContext()
{
	if (!_pThreadMgr)
	{
		SLOG_WARN("ThreadMgr is null");
		return NULL;
	}
	HRESULT         hr;

	ITfContext * imeCtx = 0;
	if(_pComposition)
	{
		SOUI::SComPtr<ITfRange> pRange;
		hr=_pComposition->GetRange(&pRange);
		if(SUCCEEDED(hr) && pRange)
		{
			ITfContext * pContext=NULL;
			hr=pRange->GetContext(&pContext);
			if(SUCCEEDED(hr) && pContext)	
			{
				imeCtx = pContext;
			}
		}
	}else
	{
		SOUI::SComPtr<ITfDocumentMgr>  pFocusDoc;
		hr = _pThreadMgr->GetFocus(&pFocusDoc);
		if(SUCCEEDED(hr) && pFocusDoc)
		{
			ITfContext *pContext=NULL;
			hr = pFocusDoc->GetTop(&pContext);
			if(SUCCEEDED(hr) && pContext)
			{
				imeCtx = pContext;
			}
		}
	}
	SLOG_INFO("CSinstar3Tsf::GetImeContext, imeCtx:"<<imeCtx);
	return imeCtx;
}

void   CSinstar3Tsf::ReleaseImeContext(ITfContext * imeContext)
{
	if (!imeContext)
	{
		SLOG_WARN("imeContext is 0");
		return;
	}
	SLOG_INFO("CSinstar3Tsf::ReleaseImeContext, imeCtx:"<<imeContext);
	ITfContext *pContext=(ITfContext*)imeContext;
	pContext->Release();
}

void CSinstar3Tsf::SetOpenStatus(BOOL bOpen)
{
	SLOG_INFO("SetOpenStatus bOpen:" << bOpen);
	_SetKeyboardOpen(bOpen);
}

BOOL CSinstar3Tsf::GetOpenStatus() const
{
	BOOL bRet = _IsKeyboardOpen();
	SLOG_INFO("GetOpenStatus isOpen:"<< bRet);
	return bRet;
}

void CSinstar3Tsf::OnStartComposition(TfEditCookie ec,ITfComposition *pComposition)
{
	SLOG_INFO("TfEditCookie:"<<ec<<" ITfComposition:"<< pComposition);
	SASSERT(!_pComposition);
	_pComposition = pComposition;
	if(m_pSinstar3) m_pSinstar3->OnCompositionStarted();
}


STDAPI CSinstar3Tsf::EnumDisplayAttributeInfo(IEnumTfDisplayAttributeInfo **ppEnum)
{
	CEnumDisplayAttributeInfo* pAttributeEnum = NULL;

	if (ppEnum == NULL)
	{
		return E_INVALIDARG;
	}

	*ppEnum = NULL;

	pAttributeEnum = new  CEnumDisplayAttributeInfo();
	if (pAttributeEnum == NULL)
	{
		return E_OUTOFMEMORY;
	}

	*ppEnum = pAttributeEnum;

	return S_OK;
}

//+---------------------------------------------------------------------------
//
// ITfDisplayAttributeProvider::GetDisplayAttributeInfo
//
//----------------------------------------------------------------------------

STDAPI CSinstar3Tsf::GetDisplayAttributeInfo( REFGUID guidInfo, ITfDisplayAttributeInfo **ppInfo)
{
	if (ppInfo == NULL)
	{
		return E_INVALIDARG;
	}

	*ppInfo = NULL;

	// Which display attribute GUID?
	if (IsEqualGUID(guidInfo, c_guidDispAttrInput))
	{
		*ppInfo = new  CDisplayAttributeInfoInput();
		if ((*ppInfo) == NULL)
		{
			return E_OUTOFMEMORY;
		}
	}
	else
	{
		return E_INVALIDARG;
	}


	return S_OK;
}

BOOL CSinstar3Tsf::_InitDisplayAttributeGuidAtom()
{
	ITfCategoryMgr* pCategoryMgr = NULL;
	HRESULT hr = CoCreateInstance(CLSID_TF_CategoryMgr, NULL, CLSCTX_INPROC_SERVER, IID_ITfCategoryMgr, (void**)&pCategoryMgr);

	if (FAILED(hr))
	{
		return FALSE;
	}

	// register the display attribute for input text.
	hr = pCategoryMgr->RegisterGUID(c_guidDispAttrInput, &_gaDisplayAttributeInput);
	if (FAILED(hr))
	{
		goto Exit;
	}

Exit:
	pCategoryMgr->Release();

	return (hr == S_OK);
}

BOOL CSinstar3Tsf::_SetCompositionDisplayAttributes(TfEditCookie ec, _In_ ITfContext *pContext, ITfRange* pRangeComposition)
{
	ITfProperty* pDisplayAttributeProperty = NULL;
	HRESULT hr = S_OK;
	// get our the display attribute property
	if (SUCCEEDED(pContext->GetProperty(GUID_PROP_ATTRIBUTE, &pDisplayAttributeProperty)))
	{
		VARIANT var;
		// set the value over the range
		// the application will use this guid atom to lookup the acutal rendering information
		var.vt = VT_I4; // we're going to set a TfGuidAtom
		var.lVal = _gaDisplayAttributeInput; 

		hr = pDisplayAttributeProperty->SetValue(ec, pRangeComposition, &var);

		pDisplayAttributeProperty->Release();
	}

	return (hr == S_OK);
}