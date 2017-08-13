#include "stdafx.h"
#include "sinstar3_tsf.h"
#include "EditSession.h"

STDAPI CBaiduJPTSF::OnCompositionTerminated(TfEditCookie ecWrite, ITfComposition *pComposition)
{
	XLOG_FUNCTION;
	Helper_Trace(_T("OnCompositionTerminated"));
	ITfContext *pCtx=(ITfContext *)GetImeContext();
	_TerminateComposition(ecWrite,pCtx);
	ReleaseImeContext(pCtx);

	// releae our cached composition
	if (_pComposition != NULL)
	{
		_pComposition->Release();
		_pComposition = NULL;
	}
	
    return S_OK;
}


BOOL CBaiduJPTSF::_IsComposing()
{
    return _pComposition != NULL;
}


void CBaiduJPTSF::_SetComposition(ITfComposition *pComposition)
{
    _pComposition = pComposition;
}


void CBaiduJPTSF::_StartComposition(ITfContext *pContext)
{
    CEditSessionStartComposition *pStartCompositionEditSession;
	Helper_Trace(_T("$$$$$_StartComposition"));

	_AdviseTextLayoutSink(pContext);
    if (pStartCompositionEditSession = new CEditSessionStartComposition(this, pContext))
    {
        HRESULT hr;
        // A synchronus document write lock is requred.
        // the CStartCompositionEditSession will do all the work when the
        // CStartCompositionEditSession::DoEditSession method is called by the context
		HRESULT hrSession;
		hr = pContext->RequestEditSession(_tfClientId, pStartCompositionEditSession, TF_ES_SYNC | TF_ES_READWRITE, &hrSession);
        pStartCompositionEditSession->Release();
    }
}

BOOL CBaiduJPTSF::_SetCompositionDisplayAttributes(TfEditCookie ec, ITfContext *pContext,ITfRange *pRange, TF_DA_ATTR_INFO attr)
{
	ITfProperty *pDisplayAttributeProperty;
	HRESULT hr;

	hr = E_FAIL;

	// get our the display attribute property
	if (pContext->GetProperty(GUID_PROP_ATTRIBUTE, &pDisplayAttributeProperty) == S_OK)
	{
		VARIANT var;
		// set the value over the range
		// the application will use this guid atom to lookup the acutal rendering information
		var.vt = VT_I4; // set a TfGuidAtom
		switch(attr)
		{
		case TF_ATTR_INPUT:var.lVal=_gaDisplayAttributeInput;break;
		case TF_ATTR_CONVERTED:var.lVal=_gaDisplayAttributeConverted;break;
		case TF_ATTR_TARGET_CONVERTED:var.lVal=_gaDisplayAttributeTargetConverted;break;
		default:var.lVal=_gaDisplayAttributeInput;break;
		}

		hr = pDisplayAttributeProperty->SetValue(ec, pRange, &var);

		pDisplayAttributeProperty->Release();
	}

	return (hr == S_OK);	
}

BOOL CBaiduJPTSF::_GetSegRange(TfEditCookie ec,ITfRange **pRange,int nLeft,int nRight)
{
	LONG cch=0;
	assert(_IsComposing());
	if(S_OK!=_pComposition->GetRange(pRange)) return FALSE;
	(*pRange)->ShiftStart(ec,nLeft,&cch,NULL);
	assert(cch==nLeft);
	(*pRange)->Collapse(ec,TF_ANCHOR_START);
	if(nRight>nLeft) 
	{
		(*pRange)->ShiftEnd(ec,nRight-nLeft,&cch,NULL);
		assert(cch==nRight-nLeft);
	}
	return TRUE;
}

void CBaiduJPTSF::_TerminateComposition(TfEditCookie ec, ITfContext *pContext)
{
	_bPosSaved = FALSE;

	XLOG_FUNCTION;
    if (_pComposition != NULL)
    {
		_ClearCompositionDisplayAttributes(ec,pContext);
		if(m_pBaiduJP3) m_pBaiduJP3->OnCompositionTerminated();
		_pComposition->EndComposition(ec);

		if ( pContext != NULL)
		{
			_UnadviseTextLayoutSink(pContext);
		}

		_pComposition->Release();
		_SetComposition(NULL);
    }
}


void CBaiduJPTSF::_EndComposition(ITfContext *pContext)
{
	XLOG_FUNCTION;

    CEditSessionEndComposition *pEditSession;
    HRESULT hr;

    if (pEditSession = new CEditSessionEndComposition(this, pContext))
    {
        pContext->RequestEditSession(_tfClientId, pEditSession, (_bInKeyProc?TF_ES_SYNC:TF_ES_ASYNCDONTCARE) | TF_ES_READWRITE, &hr);
        pEditSession->Release();
    }
}

BOOL CBaiduJPTSF::_EndCompositionEx()
{
	if(!_IsComposing()) return FALSE;
	HRESULT hr;
	CComPtr<ITfRange> pRange;
	CComPtr<ITfContext> pCtx;
	hr=_pComposition->GetRange(&pRange);
	if(pRange)
	{
		hr=pRange->GetContext(&pCtx);
		if(pCtx) _EndComposition(pCtx);
	}
	if((!pRange || !pCtx) && m_pBaiduJP3)
	{
		_bPosSaved = FALSE;
		m_pBaiduJP3->OnCompositionTerminated();
		return FALSE;
	}
	return TRUE;
}


void CBaiduJPTSF::_ChangeComposition(ITfContext *pContext,int nLeft,int nRight,WCHAR* wszComp,int nLen)
{
	HRESULT hr;

	CEditSessionKeyIn *pEditSession;
	if (pEditSession = new CEditSessionKeyIn(this, pContext,nLeft,nRight,wszComp,nLen))
	{
		pContext->RequestEditSession(_tfClientId, pEditSession, (_bInKeyProc?TF_ES_SYNC:TF_ES_ASYNCDONTCARE) | TF_ES_READWRITE, &hr);
		pEditSession->Release();
	}
}

void CBaiduJPTSF::_UpdateResultAndCompositionStringW(ITfContext * pContext,WCHAR *wszResultStr,int nResStrLen,WCHAR *wszCompStr,int nCompStrLen)
{
	HRESULT hr;

	CEditSessionUpdateResultAndComp *pEditSession;
	if (pEditSession = new CEditSessionUpdateResultAndComp(this, pContext,wszResultStr,nResStrLen,wszCompStr,nCompStrLen))
	{
		pContext->RequestEditSession(_tfClientId, pEditSession, (_bInKeyProc?TF_ES_SYNC:TF_ES_ASYNCDONTCARE) | TF_ES_READWRITE, &hr);
		pEditSession->Release();
	}
}

int  CBaiduJPTSF::_MoveCaretPos(ITfContext *pContext,int nPos,BOOL bSet)
{
	if(!_IsComposing()) return 0;
	CEditSessionMoveCaret *pEditSession;
	HRESULT hr;

	if (pEditSession = new CEditSessionMoveCaret(this, pContext,nPos,bSet,_pComposition))
	{
		pContext->RequestEditSession(_tfClientId, pEditSession, TF_ES_ASYNCDONTCARE | TF_ES_READWRITE, &hr);
		pEditSession->Release();
	}
	return 1;
}

