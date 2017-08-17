#include "stdafx.h"
#include "editsession.h"

//////////////////////////////////////////////////////////////////////////
CEditSessionBase::CEditSessionBase(CSinstar3Tsf *pTextService, ITfContext *pContext)
:_pTextService(pTextService)
,_pContext(pContext)
{
	_cRef = 1;
}


//////////////////////////////////////////////////////////////////////////
CEditSessionEndComposition::CEditSessionEndComposition(CSinstar3Tsf *pTextService, ITfContext *pContext) 
: CEditSessionBase(pTextService, pContext)
{

}

STDMETHODIMP CEditSessionEndComposition::DoEditSession(TfEditCookie ec)
{
	SLOG_INFO("DoEditSession:TfEditCookie:"<<ec);
	if(_pTextService->IsCompositing())
	{
		SLOG_WARN("CEditSessionEndComposition::DoEditSession not in compositing");
		return S_OK;
	}
	ITfRange * pRange;
	if ( _pTextService->_pComposition->GetRange( &pRange) == S_OK && pRange != NULL)
	{
		TF_SELECTION sel={0};
		sel.style.ase = TF_AE_NONE;
		sel.style.fInterimChar = FALSE;
		sel.range=pRange;
		pRange->Collapse(ec,TF_ANCHOR_END);
		_pContext->SetSelection(ec,1,&sel);
		pRange->Release();
	}
	_pTextService->_TerminateComposition(ec, _pContext);
	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
CEditSessionGetTextExtent::CEditSessionGetTextExtent(CSinstar3Tsf *pTextService, ITfContext *pContext, ITfContextView *pContextView) 
: CEditSessionBase(pTextService, pContext)
, _pContextView(pContextView)
{
}


STDMETHODIMP CEditSessionGetTextExtent::DoEditSession(TfEditCookie ec)
{
	ULONG uFatched=0;
	CComPtr<ITfRange> pRange;
	HRESULT hr;
	RECT rc;
	LONG cch=0;

	if ( _pTextService->_pComposition == NULL || _pTextService->m_pSinstar3==NULL) return S_FALSE;



	ITfRange* range = NULL;
	if ( _pTextService->_pComposition->GetRange( &range) == S_OK && range != NULL)
	{
		RECT rcLast = { -1, -1, -1, -1};
		int nLen=_pTextService->m_pSinstar3->GetCompositionSegmentEnd(_pTextService->m_pSinstar3->GetCompositionSegments()-1);

		GetLastLineRect( ec, range, nLen, rcLast, TRUE);			


		_pTextService->m_pSinstar3->OnSetCaretPosition( *(POINT*)&rcLast, rcLast.bottom - rcLast.top);			
		SLOGFMTF("SetCaret Pos 1:%d,%d, height: %d",rcLast.left,rcLast.top, rcLast.bottom - rcLast.top);

		range->Release();
	}

	int nSeg=_pTextService->m_pSinstar3->GetCompositionSegments();
	int nBegin=0;
	memset(&rc,0xff,sizeof(RECT));
	for(int i=0;i<nSeg;i++)
	{
		UINT uAttr=_pTextService->m_pSinstar3->GetCompositionSegmentAttr(i);
		if(uAttr==RG_ATTR_TARGET_CONVERTED || uAttr==RG_ATTR_TARGET_NOTCONVERTED)
		{
			//_pTextService->_GetSegRange(ec,&pRange,nBegin,nBegin);
			//hr=_pContextView->GetTextExt(ec, pRange, &rc, &fClipped);
			int nEnd = _pTextService->m_pSinstar3->GetCompositionSegmentEnd(i);
			_pTextService->_GetSegRange(ec,&pRange,nBegin, nEnd);
			hr = GetLastLineRect( ec, pRange, nEnd-nBegin, rc, FALSE);
			break;
		}
		nBegin=_pTextService->m_pSinstar3->GetCompositionSegmentEnd(i);
	}
	if(rc.bottom-rc.top>=1)
	{//获得有效的候选位置
		SLOGFMTF("SetCaret Pos 2:%d,%d, height: %d",rc.left, rc.top, rc.bottom - rc.top);
		_pTextService->m_pSinstar3->OnSetFocusSegmentPosition(*(POINT*)&rc,rc.bottom-rc.top);
	}else if(rc.left==-1 && rc.top==-1)
	{
		SLOGFMTF("SetCaret Pos 3:%d,%d, height: %d",rc.left, rc.top, 0);
		_pTextService->m_pSinstar3->OnSetFocusSegmentPosition(*(POINT*)&rc,0);
	}		

	return S_OK;
}

BOOL CEditSessionGetTextExtent::GetLastLineRect(TfEditCookie ec, ITfRange* range, int nLen, RECT& rcLast, BOOL bUseSavedPos)
{
	RECT rt = { 0, 0, 20, 20 };

	{

		_pTextService->ShowCandidateWindow();

		RECT rc;
		BOOL fClipped;
		HRESULT hr = _pContextView->GetTextExt( ec, range, &rc, &fClipped);
		if ( hr == S_OK)
		{
			SLOGFMTF("GetTextExt, 0, %d, %d, %d, %d", rc.left, rc.top, rc.right, rc.bottom);
			rcLast = rc;
		}

		for ( int i = 1; i < nLen; i++)
		{
			LONG cch;
			if ( range->ShiftStart( ec, 1, &cch, NULL) == S_OK)
			{
				if ( _pContextView->GetTextExt( ec, range, &rc, &fClipped) == S_OK)
				{
					SLOGFMTF("GetTextExt, %d, %d, %d, %d, %d", i, rc.left, rc.top, rc.right, rc.bottom);

					if ( rc.bottom > rcLast.bottom)
					{
						rcLast = rc;
					}

					if ( rc.left < rcLast.left)
					{
						rcLast = rc;
					}
				}
			}
		}
	}


	return TRUE;
}



//////////////////////////////////////////////////////////////////////////
CEditSessionStartComposition::CEditSessionStartComposition(CSinstar3Tsf *pTextService, ITfContext *pContext) 
: CEditSessionBase(pTextService, pContext)
{

}

STDMETHODIMP CEditSessionStartComposition::DoEditSession(TfEditCookie ec)
{
	CComPtr<ITfInsertAtSelection> pInsertAtSelection;
	CComPtr<ITfRange> pRangeInsert;
	CComPtr<ITfContextComposition> pContextComposition;
	ITfComposition *pComposition = NULL;
	HRESULT hr = E_FAIL;

	// A special interface is required to insert text at the selection
	hr = _pContext->QueryInterface(IID_ITfInsertAtSelection, (void **)&pInsertAtSelection);
	ASSERT_HR(hr);

	// insert the text
	hr = pInsertAtSelection->InsertTextAtSelection(ec, TF_IAS_QUERYONLY, NULL, 0, &pRangeInsert);
	ASSERT_HR(hr);

	// get an interface on the context to deal with compositions
	hr = _pContext->QueryInterface(IID_ITfContextComposition, (void **)&pContextComposition);
	ASSERT_HR(hr);

	// start the new composition
	hr = pContextComposition->StartComposition(ec, pRangeInsert, _pTextService, &pComposition);
	ASSERT_HR(hr);
	ASSERT_RET(pComposition, return E_FAIL);

	if(_pTextService->IsCompositing())
		return S_OK;

	// Store the pointer of this new composition object in the instance 
	// of the CTextService class. So this instance of the CTextService 
	// class can know now it is in the composition stage.
	_pTextService->_SetComposition(pComposition);
	_pTextService->m_pSinstar3->OnCompositionStarted();

	// 
	//  set selection to the adjusted range
	// 
	TF_SELECTION tfSelection;
	tfSelection.range = pRangeInsert;
	tfSelection.style.ase = TF_AE_NONE;
	tfSelection.style.fInterimChar = FALSE;
	_pContext->SetSelection(ec, 1, &tfSelection);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
CEditSessionKeyIn::CEditSessionKeyIn(CSinstar3Tsf *pTextService, ITfContext *pContext,int nLeft,int nRight,LPCWSTR pszBuf,int nLen) : CEditSessionBase(pTextService, pContext)
{
	if(nLen) 
	{
		m_pszBuf=new WCHAR[nLen];
		wcsncpy(m_pszBuf,pszBuf,nLen);
	}else
	{
		m_pszBuf=NULL;
	}
	m_nLeft=nLeft,m_nRight=nRight;
	m_nLen=nLen;
}

CEditSessionKeyIn::~CEditSessionKeyIn()
{
	if(m_pszBuf) delete []m_pszBuf;
}


STDMETHODIMP CEditSessionKeyIn::DoEditSession(TfEditCookie ec)
{
	if(!_pTextService->IsCompositing()) 
		_pTextService->_StartComposition(_pContext);
	CComPtr<ITfRange> pRangeComposition;
	CComPtr<ITfRange> pRangeSel;
	TF_SELECTION tfSelection;
	ULONG cFetched;
	BOOL fCovered;
	// first, test where a keystroke would go in the document if an insert is done
	if (_pContext->GetSelection(ec, TF_DEFAULT_SELECTION, 1, &tfSelection, &cFetched) != S_OK || cFetched != 1)
		return S_FALSE;

	pRangeSel.Attach(tfSelection.range);

	// is the insertion point covered by a composition?
	if (_pTextService->_pComposition->GetRange(&pRangeComposition) == S_OK)
	{
		fCovered = IsRangeCovered(ec, pRangeSel, pRangeComposition);
		if(fCovered)
		{
			if(m_nLeft==0 && m_nRight==-1)
			{//全部替换
				pRangeSel->ShiftStartToRange(ec,pRangeComposition,TF_ANCHOR_START);
				pRangeSel->ShiftEndToRange(ec,pRangeComposition,TF_ANCHOR_END);
			}else	if(m_nLeft!=-1 && m_nRight!=-1)
			{//替换指定范围
				LONG cch=0;
				pRangeSel->ShiftStartToRange(ec,pRangeComposition,TF_ANCHOR_START);
				pRangeSel->ShiftStart(ec,m_nLeft,&cch,NULL);
				pRangeSel->Collapse(ec,TF_ANCHOR_START);
				pRangeSel->ShiftEnd(ec,m_nRight-m_nLeft,&cch,NULL);
			}
		}

		if (!fCovered)
		{
			return S_OK;
		}
	}

	// insert the text
	// Use SetText here instead of InsertTextAtSelection because a composition is already started
	// Don't allow the app to adjust the insertion point inside our composition
	if (pRangeSel->SetText(ec, 0, m_pszBuf, m_nLen) != S_OK)
		return S_OK;

	// update the selection, and make it an insertion point just past
	// the inserted text.
	pRangeSel->Collapse(ec, TF_ANCHOR_END);
	_pContext->SetSelection(ec, 1, &tfSelection);
	//
	// set the display attribute to the composition range.
	//
	_pTextService->UpdateCompAttr(_pContext,ec,pRangeComposition);
	if(_pTextService->m_pSinstar3) _pTextService->m_pSinstar3->OnCompositionChanged();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
CEditSessionMoveCaret::CEditSessionMoveCaret(CSinstar3Tsf *pTextService, 
											 ITfContext *pContext,
											 int nPos,
											 BOOL bSet,
											 ITfComposition *pComposition) 
											 : CEditSessionBase(pTextService, pContext)
											 ,m_pComposition(pComposition)
											 ,m_nPos(nPos)
											 ,m_bSet(bSet)
{
}


STDMETHODIMP CEditSessionMoveCaret::DoEditSession(TfEditCookie ec)
{
	CComPtr<ITfRange> pRangeComposition;
	TF_SELECTION tfSelection;
	ULONG cFetched;
	BOOL fCovered;

	// first, test where a keystroke would go in the document if an insert is done
	if (_pContext->GetSelection(ec, TF_DEFAULT_SELECTION, 1, &tfSelection, &cFetched) != S_OK || cFetched != 1)
		return S_FALSE;

	// is the insertion point covered by a composition?
	if (m_pComposition->GetRange(&pRangeComposition) == S_OK)
	{
		fCovered = IsRangeCovered(ec, tfSelection.range, pRangeComposition);
		if(fCovered)
		{
			LONG cch=0;
			if(m_bSet)
			{
				tfSelection.range->ShiftStartToRange(ec,pRangeComposition,TF_ANCHOR_START);
				tfSelection.range->ShiftStart(ec,m_nPos,&cch,NULL);
			}else
			{
				tfSelection.range->ShiftStart(ec,m_nPos,&cch,NULL);
			}
			tfSelection.range->Collapse(ec,TF_ANCHOR_START);
		}

		if (!fCovered)
		{
			return S_OK;
		}
	}
	_pContext->SetSelection(ec, 1, &tfSelection);
	if(_pTextService->m_pSinstar3) _pTextService->m_pSinstar3->OnCompositionChanged();
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
CEditSessionUpdateResultAndComp::CEditSessionUpdateResultAndComp(CSinstar3Tsf *pTextService, 
																 ITfContext *pContext,
																 LPCWSTR pszResultStr,
																 int nResStrLen,
																 LPCWSTR pszCompStr,
																 int nCompStrLen) 
																 : CEditSessionBase(pTextService, pContext)
{
	if(pszResultStr)
	{
		if(nResStrLen==-1) nResStrLen=wcslen(pszResultStr);
		m_nResStrLen=nResStrLen;
		m_pszResultStr=new WCHAR[m_nResStrLen];
		wcsncpy(m_pszResultStr,pszResultStr,m_nResStrLen);
	}else
	{
		m_pszResultStr=NULL;
		m_nResStrLen=0;
	}
	if(pszCompStr)
	{
		if(nCompStrLen==-1) nCompStrLen=wcslen(pszCompStr);
		m_nCompStrLen=nCompStrLen;
		m_pszCompStr=new WCHAR[m_nCompStrLen];
		wcsncpy(m_pszCompStr,pszCompStr,m_nCompStrLen);
	}else
	{
		m_pszCompStr=NULL;
		m_nCompStrLen=0;
	}
}

CEditSessionUpdateResultAndComp::~CEditSessionUpdateResultAndComp()
{
	if(m_pszCompStr) delete []m_pszCompStr;
	if(m_pszResultStr) delete []m_pszResultStr;
}

STDMETHODIMP CEditSessionUpdateResultAndComp::DoEditSession(TfEditCookie ec)
{
	CComPtr<ITfRange> pRangeComposition;
	CComPtr<ITfProperty> pDisplayAttributeProperty;


	BOOL fEmpty=TRUE;

	if(!_pTextService->_IsComposing())
	{
		_pTextService->_StartComposition(_pContext);
	}
	CComPtr<ITfComposition> pCompostion=_pTextService->_pComposition;
	//将当前数据上屏
	pCompostion->GetRange(&pRangeComposition);

	// get our the display attribute property
	if (_pContext->GetProperty(GUID_PROP_ATTRIBUTE, &pDisplayAttributeProperty) == S_OK)
	{
		// clear the value over the range
		pDisplayAttributeProperty->Clear(ec, pRangeComposition);
	}

	if(m_pszResultStr)
	{
		pRangeComposition->SetText(ec,0,m_pszResultStr,m_nResStrLen);
	}

	if(pRangeComposition->IsEmpty(ec,&fEmpty)==S_OK && !fEmpty)
	{
		pRangeComposition->Collapse(ec,TF_ANCHOR_END);
		pCompostion->ShiftStart(ec,pRangeComposition);
	}

	TF_SELECTION tfSelection;
	tfSelection.style.ase = TF_AE_NONE;
	tfSelection.style.fInterimChar = FALSE;
	tfSelection.range=pRangeComposition;
	//插入新的编码
	if(m_pszCompStr)
	{
		_pTextService->_bPosSaved = FALSE;

		POINT pt={-1,-1};
		_pTextService->m_pSinstar3->OnSetFocusSegmentPosition(pt,0);
		pRangeComposition->SetText(ec,0,m_pszCompStr,m_nCompStrLen);

		pRangeComposition->Clone(&tfSelection.range);
		tfSelection.range->Collapse(ec,TF_ANCHOR_END);
		_pContext->SetSelection(ec,1,&tfSelection);
		tfSelection.range->Release();

		_pTextService->UpdateCompAttr(_pContext,ec,pRangeComposition);

		if(_pTextService->m_pSinstar3) _pTextService->m_pSinstar3->OnCompositionChanged();
	}else
	{
		_pContext->SetSelection(ec,1,&tfSelection);
		_pTextService->_EndComposition(_pContext);
	}
	return S_OK;
}
