#include "stdafx.h"
#include "sinstar3_tsf.h"
#include "EditSession.h"



BOOL CSinstar3Tsf::_IsComposing() const
{
    return _pComposition != NULL;
}


void CSinstar3Tsf::_StartComposition(ITfContext *pContext)
{
    CEsStartComposition *pStartCompositionEditSession;
	SLOG_INFO("$$$$$_StartComposition");

	_AdviseTextLayoutSink(pContext);
    if (pStartCompositionEditSession = new CEsStartComposition(this, pContext))
    {
        HRESULT hr;
        // A synchronous document write lock is required.
        // the CEsStartComposition will do all the work when the
        // CEsStartComposition::DoEditSession method is called by the context
		HRESULT hrSession;
		hr = pContext->RequestEditSession(_tfClientId, pStartCompositionEditSession, (_bInKeyProc?TF_ES_SYNC:TF_ES_ASYNCDONTCARE) | TF_ES_READWRITE, &hrSession);
        pStartCompositionEditSession->Release();
    }
}

BOOL CSinstar3Tsf::_SetCompositionDisplayAttributes(TfEditCookie ec, ITfContext *pContext,ITfRange *pRange, TF_DA_ATTR_INFO attr)
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

BOOL CSinstar3Tsf::_GetSegRange(TfEditCookie ec,ITfRange **pRange,int nLeft,int nRight)
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


STDAPI CSinstar3Tsf::OnCompositionTerminated(TfEditCookie ecWrite, ITfComposition *pComposition)
{
	SLOG_INFO("OnCompositionTerminated,TfEditCookie:"<<ecWrite<< " pComposition:"<<pComposition);
	{
		SASSERT(pComposition && pComposition == _pComposition);
		ITfContext *pCtx=(ITfContext *)GetImeContext();
		_TerminateComposition(ecWrite,pCtx);
		ReleaseImeContext(pCtx);
	}
	return S_OK;
}


void CSinstar3Tsf::_EndComposition(ITfContext *pContext)
{
    CEsEndComposition *pEditSession;
    HRESULT hr;

    if (pEditSession = new CEsEndComposition(this, pContext))
    {
        pContext->RequestEditSession(_tfClientId, pEditSession, (_bInKeyProc?TF_ES_SYNC:TF_ES_ASYNCDONTCARE) | TF_ES_READWRITE, &hr);
        pEditSession->Release();
    }
}

BOOL CSinstar3Tsf::_EndCompositionEx()
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
	return TRUE;
}


void CSinstar3Tsf::_ChangeComposition(ITfContext *pContext,int nLeft,int nRight,const WCHAR* wszComp,int nLen)
{
	HRESULT hr;

	CEsChangeComposition *pEditSession;
	if (pEditSession = new CEsChangeComposition(this, pContext,nLeft,nRight,wszComp,nLen))
	{
		pContext->RequestEditSession(_tfClientId, pEditSession, (_bInKeyProc?TF_ES_SYNC:TF_ES_ASYNCDONTCARE) | TF_ES_READWRITE, &hr);
		pEditSession->Release();
	}
}

void CSinstar3Tsf::_UpdateResultAndCompositionStringW(ITfContext * pContext,const WCHAR *wszResultStr,int nResStrLen,const WCHAR *wszCompStr,int nCompStrLen)
{
	HRESULT hr;

	CEsUpdateResultAndComp *pEditSession;
	if (pEditSession = new CEsUpdateResultAndComp(this, pContext,wszResultStr,nResStrLen,wszCompStr,nCompStrLen))
	{
		pContext->RequestEditSession(_tfClientId, pEditSession, (_bInKeyProc?TF_ES_SYNC:TF_ES_ASYNCDONTCARE) | TF_ES_READWRITE, &hr);
		pEditSession->Release();
	}
}

int  CSinstar3Tsf::_MoveCaretPos(ITfContext *pContext,int nPos,BOOL bSet)
{
	if(!_IsComposing()) return 0;
	CEsMoveCaret *pEditSession;
	HRESULT hr;

	if (pEditSession = new CEsMoveCaret(this, pContext,nPos,bSet,_pComposition))
	{
		pContext->RequestEditSession(_tfClientId, pEditSession, TF_ES_ASYNCDONTCARE | TF_ES_READWRITE, &hr);
		pEditSession->Release();
	}
	return 1;
}


void CSinstar3Tsf::_TerminateComposition(TfEditCookie ecWrite,ITfContext *pCtx)
{
	_ClearCompositionDisplayAttributes(ecWrite,pCtx);

	if ( pCtx != NULL)
	{
		_UnadviseTextLayoutSink(pCtx);
	}
	if(m_pSinstar3) m_pSinstar3->OnCompositionTerminated();

	_pComposition = NULL;
	_bPosSaved = FALSE;
}
