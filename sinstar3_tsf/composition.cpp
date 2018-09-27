#include "stdafx.h"
#include "sinstar3_tsf.h"
#include "EditSession.h"



BOOL CSinstar3Tsf::_IsComposing() const
{
    return _pComposition != NULL;
}


void CSinstar3Tsf::_StartComposition(ITfContext *pContext)
{
	if(IsCompositing()) return;
	CEsStartComposition *pStartCompositionEditSession = new CEsStartComposition(this, pContext);
	SLOG_INFO("pContext:"<<pContext);

	_bCompsiting = TRUE;
	_AdviseTextLayoutSink(pContext);
	HRESULT hr;
	pContext->RequestEditSession(_tfClientId, pStartCompositionEditSession, TF_ES_SYNC | TF_ES_READWRITE, &hr);
	pStartCompositionEditSession->Release();
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
	SASSERT(pComposition && pComposition == _pComposition);
	ITfContext *pCtx=(ITfContext *)GetImeContext();
	_TerminateComposition(ecWrite,pCtx,true);
	ReleaseImeContext(pCtx);

	return S_OK;
}


void CSinstar3Tsf::_EndComposition(ITfContext *pContext)
{
	SLOG_INFO("_EndComposition IsCompositing()="<<IsCompositing());
	if(!IsCompositing()) return;
    CEsEndComposition *pEditSession = new CEsEndComposition(this, pContext);
    HRESULT hr;
	pContext->RequestEditSession(_tfClientId, pEditSession, (_bInKeyProc?TF_ES_SYNC:TF_ES_ASYNCDONTCARE) | TF_ES_READWRITE, &hr);
	pEditSession->Release();
}

BOOL CSinstar3Tsf::_EndCompositionEx()
{
	if(!_IsComposing()) return FALSE;
	HRESULT hr;
	SOUI::SComPtr<ITfRange> pRange;
	SOUI::SComPtr<ITfContext> pCtx;
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

void CSinstar3Tsf::_TerminateComposition(TfEditCookie ecWrite,ITfContext *pCtx, bool bClearCtx)
{
	if ( pCtx != NULL)
	{
		_UnadviseTextLayoutSink(pCtx);
	}
	if(m_pSinstar3) m_pSinstar3->OnCompositionTerminated(bClearCtx);

	_pComposition = NULL;
	_bCompsiting = FALSE;
	SLOG_INFO("_pComposition = NULL, _bCompsiting = FALSE");
}
