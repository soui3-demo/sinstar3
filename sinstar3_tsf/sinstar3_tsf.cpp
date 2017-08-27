#include "stdafx.h"
#include "sinstar3_tsf.h"
#include "editsession.h"

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

PCTSTR pszAppList_Use_Cursor[] = 
{
	// IE Series
	_T("iexplore.exe")
};

PCTSTR pszAppList_Use_Ms_Method[] = 
{
	// Adobe Series
	_T("Photoshop.exe"),
	_T("Illustrator.exe"),
	_T("DreamWeaver.exe"),
	_T("Flash.exe"),
	_T("soffice.exe"),
	_T("soffice.bin"),

	// Kingsoft Series
	_T("wps.exe"),	// Kingsoft Writer
	_T("wpp.exe"),	// Kingsoft Presentation
	_T("et.exe")	// Kingsoft SpreadSheet
};


//+---------------------------------------------------------------------------
//
// ctor
//
//----------------------------------------------------------------------------

CSinstar3Tsf::CSinstar3Tsf()
{
	_cRef = 1;

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
	
	_bUseCursorPos = FALSE;
	_bUseMSMethod = FALSE;

	TCHAR szFileName[MAX_PATH] = _T("");
	GetModuleFileName( NULL, szFileName, ARRAYSIZE( szFileName));

	PCTSTR pszPos = _tcsrchr( szFileName, _T('\\'));
	if ( pszPos != NULL)
	{
		for ( size_t i = 0; i < ARRAYSIZE( pszAppList_Use_Cursor); i++)
		{
			if ( _tcsicmp( pszPos + 1, pszAppList_Use_Cursor[i]) == 0)
			{
				_bUseCursorPos = TRUE;
				break;
			}
		}

		for ( size_t i = 0; i < ARRAYSIZE( pszAppList_Use_Ms_Method); i++)
		{
			if ( _tcsicmp( pszPos + 1, pszAppList_Use_Ms_Method[i]) == 0)
			{
				_bUseMSMethod = TRUE;
				break;
			}
		}
	}

	_bPosSaved = FALSE;
}

//+---------------------------------------------------------------------------
//
// dtor
//
//----------------------------------------------------------------------------

CSinstar3Tsf::~CSinstar3Tsf()
{
	Deactivate();
    theModule->Release();
}

STDAPI CSinstar3Tsf::Activate(ITfThreadMgr *pThreadMgr, TfClientId tfClientId)
{
	SLOGFMTF("Activate %x %x", pThreadMgr, tfClientId);

	SHORT sSt=GetKeyState(VK_KANA);
	if(GetKeyState(VK_KANA)&0x01)
	{//自动取消假名输入模式
		INPUT input[2];
		input[0].type=INPUT_KEYBOARD;
		input[0].ki.wVk=VK_KANA;
		input[0].ki.dwFlags=0;
		input[1].type=INPUT_KEYBOARD;
		input[1].ki.wVk=VK_KANA;
		input[1].ki.dwFlags=KEYEVENTF_KEYUP;
		BOOL bRet=SendInput(2,input,sizeof(INPUT));
		_ASSERT(bRet);
	}

	_pThreadMgr = pThreadMgr;
    _pThreadMgr->AddRef();
    _tfClientId = tfClientId;	

	_dwLastLayoutChangeTime = GetTickCount();

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
	//
	// Initialize display guid atom
	//
	if (!_InitDisplayAttributeGuidAtom())
		goto ExitError;	

	if ( !_InitThreadCompartment())
		goto ExitError;

	OnSetThreadFocus();

	if(!_InitSinstar3())
 		goto ExitError;

	
	return S_OK;

ExitError:
    Deactivate(); // cleanup any half-finished init
    return E_FAIL;
}

//+---------------------------------------------------------------------------
//
// Deactivate
//
//----------------------------------------------------------------------------

STDAPI CSinstar3Tsf::Deactivate()
{
	SLOGFMTF("Deactivate");
	//确保输入过程结束
	if(_IsComposing())
	{
		_ASSERT(_pThreadMgr);
		ITfContext *pCtx=(ITfContext *)GetImeContext();
		if(pCtx) 
			EndComposition(pCtx);
		else if(m_pSinstar3)
			m_pSinstar3->OnCompositionTerminated();
		ReleaseImeContext(pCtx);
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
	_bPosSaved = FALSE;

    return S_OK;
}

STDMETHODIMP CSinstar3Tsf::OnLayoutChange(ITfContext *pContext, TfLayoutCode lcode, ITfContextView *pContextView)
{
	SLOG_INFO("OnLayoutChange _pComposition:"<<_pComposition);
	switch (lcode)
	{
	case TF_LC_CHANGE:
		_bGetLayoutChangeMsg=TRUE;
		if (IsCompositing())
		{
			CEsGetTextExtent *pEditSession;

			if ((pEditSession = new CEsGetTextExtent(this, pContext, pContextView)) != NULL)
			{
				HRESULT hr;
				HRESULT hrSession;
				//由于这个接口可能是延时定时器触发的，因此不能使用TF_ES_SYNC标志来同步执行
				hr = pContext->RequestEditSession(_GetClientId(), pEditSession, TF_ES_ASYNCDONTCARE | TF_ES_READ, &hrSession);
				pEditSession->Release();
			}
		}

		// 处理完之后，设置一个时间标记
		_dwLastLayoutChangeTime = GetTickCount();

		break;

	case TF_LC_DESTROY:
		{
			SLOGFMTF("LayoutChange:Destroy");
		}
		break;

	case TF_LC_CREATE:
		{
			SLOGFMTF("LayoutChange:Create");
		}
		break;

	default:
		break;
	}

	return S_OK;
}

STDMETHODIMP CSinstar3Tsf::Show(	HWND hwndParent,LANGID langid,REFGUID rguidProfile)
{
	CCoreLoader::GetInstance().Sinstar3_OpenConfig(hwndParent);
	return S_OK;
}

STDMETHODIMP CSinstar3Tsf::GetDisplayName(BSTR* pbstrName)
{
	*pbstrName=SysAllocString(L"BAIDUTYPE");
	return S_OK;
}


BOOL CSinstar3Tsf::IsCompositing()
{
	return _IsComposing();
}


HRESULT CSinstar3Tsf::_AdviseTextLayoutSink(ITfContext *pContext)
{
	HRESULT hr;
	ITfSource *pSource = NULL;

	hr = E_FAIL;

	if (FAILED(pContext->QueryInterface(IID_ITfSource, (void **)&pSource)))
		goto Exit;

	if (FAILED(pSource->AdviseSink(IID_ITfTextLayoutSink, (ITfTextLayoutSink *)this, &_dwCookieTextLayoutSink)))
		goto Exit;

	hr = S_OK;

Exit:
	if (pSource != NULL)
		pSource->Release();
	return hr;
}

HRESULT CSinstar3Tsf::_UnadviseTextLayoutSink(ITfContext *pContext)
{
	HRESULT hr;
	ITfSource *pSource = NULL;

	hr = E_FAIL;
	if(_dwCookieTextLayoutSink==TF_INVALID_COOKIE) goto Exit;

	if (FAILED(pContext->QueryInterface(IID_ITfSource, (void **)&pSource)))
		goto Exit;

	if (FAILED(pSource->UnadviseSink(_dwCookieTextLayoutSink)))
		goto Exit;

	hr = S_OK;

Exit:
	if (pSource != NULL)
		pSource->Release();
	return hr;
}

void CSinstar3Tsf::StartComposition(LPVOID pImeContext)
{
	_StartComposition((ITfContext*)pImeContext);
}


void CSinstar3Tsf::EndComposition(LPVOID pImeContext)
{
	_EndComposition((ITfContext*)pImeContext);
}


void CSinstar3Tsf::ReplaceSelCompositionW(LPVOID pImeContext,int nLeft,int nRight,const WCHAR* wszComp,int nLen)
{
	_ChangeComposition((ITfContext*)pImeContext,nLeft,nRight,NULL,0);
}

void CSinstar3Tsf::UpdateResultAndCompositionStringW(LPVOID lpImeContext,const WCHAR *wszResultStr,int nResStrLen,const WCHAR *wszCompStr,int nCompStrLen)
{
	_UpdateResultAndCompositionStringW((ITfContext*)lpImeContext,wszResultStr,nResStrLen,wszCompStr,nCompStrLen);
}

int  CSinstar3Tsf::MoveCaretPos(LPVOID lpImeContext,int nPos,BOOL bSet)
{
	return _MoveCaretPos((ITfContext*)lpImeContext,nPos,bSet);
}


LPVOID CSinstar3Tsf::GetImeContext()
{

	if(!_pThreadMgr) return NULL;
	HRESULT         hr;

	if(IsCompositing())
	{
		ITfRange *pRange;
		ITfContext *pContext;
		hr=_pComposition->GetRange(&pRange);
		if(SUCCEEDED(hr) && pRange)
		{
			hr=pRange->GetContext(&pContext);
			pRange->Release();
			if(SUCCEEDED(hr)&&pContext)	return pContext;
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
				return pContext;
			}
		}
	}
	return NULL;
}

BOOL   CSinstar3Tsf::ReleaseImeContext(LPVOID lpImeContext)
{
	if(!lpImeContext) return FALSE;
	ITfContext *pContext=(ITfContext*)lpImeContext;
	try
	{
		pContext->Release();
	}
	catch (...)
	{
		
	}
	return TRUE;
}

BOOL CSinstar3Tsf::GetLanguageBarItemMgr(ITfLangBarItemMgr **ppLangBarMgr,GUID *pclsidTIP)
{
	if(!_pThreadMgr) return NULL;
	*pclsidTIP=c_clsidSinstar3TSF;
	return SUCCEEDED(_pThreadMgr->QueryInterface(IID_ITfLangBarItemMgr,(void **)ppLangBarMgr));
}

BOOL CSinstar3Tsf::_InitDisplayAttributeGuidAtom()
{
	ITfCategoryMgr *pCategoryMgr;
	HRESULT hr;

	if (CoCreateInstance(CLSID_TF_CategoryMgr,
		NULL, 
		CLSCTX_INPROC_SERVER, 
		IID_ITfCategoryMgr, 
		(void**)&pCategoryMgr) != S_OK)
	{
		return FALSE;
	}

	// register the display attribute for input text.
	hr = pCategoryMgr->RegisterGUID(c_guidDisplayAttributeInput, &_gaDisplayAttributeInput);
	// register the display attribute for the converted text.
	hr = pCategoryMgr->RegisterGUID(c_guidDisplayAttributeConverted, &_gaDisplayAttributeConverted);
	// register the display attribute for the target converted text.
	hr = pCategoryMgr->RegisterGUID(c_guidDisplayAttributeTargetConverted, &_gaDisplayAttributeTargetConverted);

	pCategoryMgr->Release();

	return (hr == S_OK);
}

CLogStateListener g_LogListener;

BOOL CSinstar3Tsf::_InitSinstar3()
{
	m_pSinstar3=CCoreLoader::GetInstance().Sinstar3_Create(this,theModule->GetModule());

	HostInfo hostInfo={theModule->GetDataPath(),&g_LogListener};
	CCoreLoader::GetInstance().Sinstar3_SetHostInfo(&hostInfo);

	if(!m_pSinstar3) return FALSE;
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
		CCoreLoader::GetInstance().Sinstar3_Delete(m_pSinstar3);
		m_pSinstar3=NULL;
	}
	return TRUE;
}

void CSinstar3Tsf::UpdateCompAttr(ITfContext *pContext,TfEditCookie ec,ITfRange *pRangeComposition)
{
	//做一个简单的处理，当range为空时Core中的数据可能和TSF中的数据不一致
	BOOL bEmpty=FALSE;
	if(!pRangeComposition) return;
	if(pRangeComposition->IsEmpty(ec,&bEmpty)==S_OK && bEmpty) return;

	pRangeComposition->Collapse(ec,TF_ANCHOR_START);
	int nSegs=m_pSinstar3->GetCompositionSegments();
	int nBegin=0;
	for(int i=0;i<nSegs;i++)
	{
		LONG cch=0;
		int nEnd=m_pSinstar3->GetCompositionSegmentEnd(i);
		pRangeComposition->ShiftEnd(ec,nEnd-nBegin,&cch,NULL);
		_SetCompositionDisplayAttributes(ec, pContext, pRangeComposition,(TF_DA_ATTR_INFO)m_pSinstar3->GetCompositionSegmentAttr(i));
		nBegin=nEnd;
		pRangeComposition->Collapse(ec,TF_ANCHOR_END);
	}
}

//+---------------------------------------------------------------------------
//
// _ClearCompositionDisplayAttributes
//
//----------------------------------------------------------------------------

void CSinstar3Tsf::_ClearCompositionDisplayAttributes(TfEditCookie ec, ITfContext *pContext)
{
	CComPtr<ITfRange> pRangeComposition;
	CComPtr<ITfProperty> pDisplayAttributeProperty;

	if(!pContext) return;
	// get the compositon range.
	if (!_pComposition || _pComposition->GetRange(&pRangeComposition) != S_OK)
		return;

	// get our the display attribute property
	if (pContext->GetProperty(GUID_PROP_ATTRIBUTE, &pDisplayAttributeProperty) == S_OK)
	{
		// clear the value over the range
		pDisplayAttributeProperty->Clear(ec, pRangeComposition);
	}
}

BOOL CSinstar3Tsf::RegisterIMEHotKey(REFGUID guidHotKey,LPCWSTR pszName,const PRESERVEDKEY *pKey)
{
	ITfKeystrokeMgr *pKeystrokeMgr;
	HRESULT hr;
	const TF_PRESERVEDKEY *pKeyTF=(const TF_PRESERVEDKEY*)pKey;

	if (_pThreadMgr->QueryInterface(IID_ITfKeystrokeMgr, (void **)&pKeystrokeMgr) != S_OK)
		return FALSE;

	hr = pKeystrokeMgr->PreserveKey(_tfClientId, 
		guidHotKey,
		pKeyTF,
		pszName,
		pszName?wcslen(pszName):0);
	pKeystrokeMgr->Release();

	return (hr == S_OK);
}

BOOL CSinstar3Tsf::UnregisterIMEHotKey(REFGUID guidHotKey,const PRESERVEDKEY *pKey)
{
	ITfKeystrokeMgr *pKeystrokeMgr;

	if (_pThreadMgr->QueryInterface(IID_ITfKeystrokeMgr, (void **)&pKeystrokeMgr) != S_OK)
		return FALSE;

	const TF_PRESERVEDKEY *pKeyTF=(const TF_PRESERVEDKEY*)pKey;
	pKeystrokeMgr->UnpreserveKey(guidHotKey, pKeyTF);
	pKeystrokeMgr->Release();
	return TRUE;
}

BOOL CSinstar3Tsf::SetOpenStatus(LPVOID lpImeContext,BOOL bOpen)
{
	return _SetKeyboardOpen(bOpen);
}

BOOL CSinstar3Tsf::GetOpenStatus(LPVOID lpImeContext)
{
	return _IsKeyboardOpen();
}

void CSinstar3Tsf::OnStartComposition(ITfComposition *pComposition)
{
	_pComposition = pComposition;
	if(m_pSinstar3) m_pSinstar3->OnCompositionStarted();
}
