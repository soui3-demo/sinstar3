#include "stdafx.h"
#include "sinstar3_tsf.h"
#include "EditSession.h"



BOOL CSinstar3Tsf::_IsCompositing() const
{
    return _pComposition != NULL;
}


void CSinstar3Tsf::_StartComposition(ITfContext *pContext)
{
	if(IsCompositing()) return;
	CEsStartComposition *pStartCompositionEditSession = new CEsStartComposition(this, pContext);
	SLOG_INFO("pContext:"<<pContext);

	_bCompositing = TRUE;
	_AdviseTextLayoutSink(pContext);
	HRESULT hr;
	pContext->RequestEditSession(_tfClientId, pStartCompositionEditSession, TF_ES_SYNC | TF_ES_READWRITE, &hr);
	pStartCompositionEditSession->Release();
}

BOOL CSinstar3Tsf::_GetSegRange(TfEditCookie ec,ITfRange **pRange,int nLeft,int nRight)
{
	LONG cch=0;
	assert(_IsCompositing());
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
	if(pComposition == _pComposition)
	{
		SLOG_INFO("CSinstar3Tsf::OnCompositionTerminated,TfEditCookie:"<<ecWrite<< " pComposition:"<<pComposition);
		ITfContext *pCtx = GetImeContext();
		_TerminateComposition(ecWrite,pCtx,true);
		ReleaseImeContext(pCtx);
	}else
	{
		SLOG_WARN("pCompsition:"<<pComposition<<" _pComposition:"<<_pComposition);
	}
	return S_OK;
}


void CSinstar3Tsf::_EndComposition(ITfContext *pContext)
{
	SLOG_INFO("_EndComposition IsCompositing()="<<_IsCompositing()<<" inKeyProc:"<<_bInKeyProc);
	if(!_IsCompositing()) return;
    CEsEndComposition *pEditSession = new CEsEndComposition(this, pContext);
    HRESULT hr;
	pContext->RequestEditSession(_tfClientId, pEditSession, (_bInKeyProc?TF_ES_SYNC:TF_ES_ASYNCDONTCARE) | TF_ES_READWRITE, &hr);
	pEditSession->Release();
	_bCompositing = FALSE;
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
	SLOG_INFO("pComposition:"<<_pComposition<<" bCompositing:"<<_bCompositing);
	_pComposition = NULL;
	_bCompositing = FALSE;
	if ( pCtx != NULL)
	{
		_UnadviseTextLayoutSink(pCtx);
	}
	if(m_pSinstar3) 
	{
		m_pSinstar3->OnCompositionTerminated(bClearCtx);
	}
}
