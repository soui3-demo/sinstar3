#pragma once

#include "sinstar3_tsf.h"
#include <imm.h>
#pragma comment(lib,"imm32.lib")

class CEditSessionBase : public ITfEditSession
{
public:
    CEditSessionBase(CSinstar3Tsf *pTextService, ITfContext *pContext)
    {
        _cRef = 1;
        _pContext = pContext;
        _pContext->AddRef();

        _pTextService = pTextService;
        _pTextService->AddRef();
    }
    virtual ~CEditSessionBase()
    {
        _pContext->Release();
        _pTextService->Release();
    }
	
	IUNKNOWN_BEGIN(IID_ITfEditSession,ITfEditSession)
	IUNKNOWN_END()

    virtual STDMETHODIMP DoEditSession(TfEditCookie ec) = 0;

protected:
    ITfContext *_pContext;
    CSinstar3Tsf *_pTextService;

};

class CEditSessionEndComposition : public CEditSessionBase
{
public:
	CEditSessionEndComposition(CSinstar3Tsf *pTextService, ITfContext *pContext) 
		: CEditSessionBase(pTextService, pContext)
	{
	}
	// ITfEditSession
	STDMETHODIMP DoEditSession(TfEditCookie ec)
	{
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
};


class CEditSessionGetTextExtent : public CEditSessionBase
{
public:

	CEditSessionGetTextExtent(CSinstar3Tsf *pTextService, ITfContext *pContext, ITfContextView *pContextView) 
		: CEditSessionBase(pTextService, pContext)
	{
		_pContextView = pContextView;
		_pContextView->AddRef();
	}

	~CEditSessionGetTextExtent()
	{
		_pContextView->Release();
	}

	BOOL GetLastLineRect( TfEditCookie ec, ITfRange* range, int nLen, RECT& rcLast, BOOL bUseSavedPos)
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

	STDMETHODIMP DoEditSession(TfEditCookie ec)
	{		
		ULONG uFatched=0;
		CComPtr<ITfRange> pRange;
		HRESULT hr;
		RECT rc;
		LONG cch=0;

		if ( _pTextService->_pComposition == NULL || _pTextService->m_pSinstar3==NULL) return S_FALSE;

		//
		// 在qq，open office，甚至今朝的截屏中，这个hWnd都是隐藏的。
		// Ren zhijie 2011-01-27 23:40
		// 但是这个判断加入是为了解决win7下候选框在右下角闪烁的bug
		//

		//
		// qa报告众多软件受到影响，暂时不做判断。
		// Ren zhijie 2011-03-02 10:15
		//

		//HWND hWnd;
		//_pContextView->GetWnd(&hWnd);
		//HWND hFrame=hWnd;
		//while(hWnd)
		//{
		//	if(GetWindowLongPtr(hWnd,GWL_STYLE)&WS_POPUP) break;
		//	hWnd= ::GetParent(hWnd);
		//	if(hWnd) hFrame=hWnd;
		//}
		//if(hFrame && !IsWindowVisible(hFrame)) return S_OK;

		//HWND hWnd;
		//_pContextView->GetWnd(&hWnd);
		//HWND hFrame=hWnd;
		//while(hWnd)
		//{
		//	hWnd= (::GetWindowLongPtr(hWnd, GWL_STYLE) & WS_CHILD) ?
		//		::GetParent(hWnd) : ::GetWindow(hWnd, GW_OWNER);
		//	if(hWnd) hFrame=hWnd;
		//}
		//if(hFrame && !IsWindowVisible(hFrame)) return S_OK;

		ITfRange* range = NULL;
		if ( _pTextService->_pComposition->GetRange( &range) == S_OK && range != NULL)
		{
			RECT rcLast = { -1, -1, -1, -1};
			int nLen=_pTextService->m_pSinstar3->GetCompositionSegmentEnd(_pTextService->m_pSinstar3->GetCompositionSegments()-1);

			GetLastLineRect( ec, range, nLen, rcLast, TRUE);			

			//
			// 不能做这样的判断，多显示器的时候，坐标可能为负数
			// Ren zhijie 2011-03-15 20:28
			//

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

private:
	ITfContextView *_pContextView;
};


class CEditSessionStartComposition : public CEditSessionBase
{
public:
	CEditSessionStartComposition(CSinstar3Tsf *pTextService, ITfContext *pContext) : CEditSessionBase(pTextService, pContext)
	{
	}

	// ITfEditSession
	STDMETHODIMP DoEditSession(TfEditCookie ec)
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
};


class CEditSessionKeyIn:public CEditSessionBase
{
public:
	CEditSessionKeyIn(CSinstar3Tsf *pTextService, ITfContext *pContext,int nLeft,int nRight,LPCWSTR pszBuf,int nLen) 
		: CEditSessionBase(pTextService, pContext)
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
	~CEditSessionKeyIn()
	{
		if(m_pszBuf) delete []m_pszBuf;
	}

	// ITfEditSession
	STDMETHODIMP DoEditSession(TfEditCookie ec)
	{
		if(!_pTextService->IsCompositing()) _pTextService->_StartComposition(_pContext);
		CComPtr<ITfRange> pRangeComposition;
		TF_SELECTION tfSelection;
		ULONG cFetched;
		BOOL fCovered;
		// first, test where a keystroke would go in the document if an insert is done
		if (_pContext->GetSelection(ec, TF_DEFAULT_SELECTION, 1, &tfSelection, &cFetched) != S_OK || cFetched != 1)
			return S_FALSE;

		// is the insertion point covered by a composition?
		if (_pTextService->_pComposition->GetRange(&pRangeComposition) == S_OK)
		{
			fCovered = IsRangeCovered(ec, tfSelection.range, pRangeComposition);
			if(fCovered)
			{
				if(m_nLeft==0 && m_nRight==-1)
				{//全部替换
					tfSelection.range->ShiftStartToRange(ec,pRangeComposition,TF_ANCHOR_START);
					tfSelection.range->ShiftEndToRange(ec,pRangeComposition,TF_ANCHOR_END);
				}else	if(m_nLeft!=-1 && m_nRight!=-1)
 				{//替换指定范围
					LONG cch=0;
					tfSelection.range->ShiftStartToRange(ec,pRangeComposition,TF_ANCHOR_START);
					tfSelection.range->ShiftStart(ec,m_nLeft,&cch,NULL);
					tfSelection.range->Collapse(ec,TF_ANCHOR_START);
					tfSelection.range->ShiftEnd(ec,m_nRight-m_nLeft,&cch,NULL);
				}
			}
			
			if (!fCovered)
			{
				goto Exit;
			}
		}

		// insert the text
		// Use SetText here instead of InsertTextAtSelection because a composition is already started
		// Don't allow the app to adjust the insertion point inside our composition
		if (tfSelection.range->SetText(ec, 0, m_pszBuf, m_nLen) != S_OK)
			goto Exit;

		// update the selection, and make it an insertion point just past
		// the inserted text.
		tfSelection.range->Collapse(ec, TF_ANCHOR_END);
		_pContext->SetSelection(ec, 1, &tfSelection);
		//
		// set the display attribute to the composition range.
		//
		_pTextService->UpdateCompAttr(_pContext,ec,pRangeComposition);
 		if(_pTextService->m_pSinstar3) _pTextService->m_pSinstar3->OnCompositionChanged();
Exit:
		tfSelection.range->Release();
		return S_OK;
	}
private:
	LPWSTR m_pszBuf;
	int		m_nLen;
	int		m_nLeft,m_nRight;
};


class CEditSessionMoveCaret:public CEditSessionBase
{
public:
	CEditSessionMoveCaret(CSinstar3Tsf *pTextService, ITfContext *pContext,int nPos,BOOL bSet,ITfComposition *pComposition) 
		: CEditSessionBase(pTextService, pContext)
	{
		m_nPos=nPos;
		m_bSet=bSet;
		m_pComposition=pComposition;
		m_pComposition->AddRef();
	}
	~CEditSessionMoveCaret()
	{
		m_pComposition->Release();
	}

	// ITfEditSession
	STDMETHODIMP DoEditSession(TfEditCookie ec)
	{
		ITfRange *pRangeComposition;
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

			pRangeComposition->Release();

			if (!fCovered)
			{
				goto Exit;
			}
		}
		_pContext->SetSelection(ec, 1, &tfSelection);
		if(_pTextService->m_pSinstar3) _pTextService->m_pSinstar3->OnCompositionChanged();
Exit:
		tfSelection.range->Release();
		return S_OK;
	}
private:
	ITfComposition *m_pComposition;
	int m_nPos;
	BOOL m_bSet;
};


class CEditSessionUpdateResultAndComp:public CEditSessionBase
{
public:
	WCHAR *m_pszResultStr,*m_pszCompStr;
	int    m_nResStrLen,m_nCompStrLen;

	CEditSessionUpdateResultAndComp(CSinstar3Tsf *pTextService, ITfContext *pContext,LPCWSTR pszResultStr,int nResStrLen,LPCWSTR pszCompStr,int nCompStrLen) 
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
	~CEditSessionUpdateResultAndComp()
	{
		if(m_pszCompStr) delete []m_pszCompStr;
		if(m_pszResultStr) delete []m_pszResultStr;
	}

	// ITfEditSession
	STDMETHODIMP DoEditSession(TfEditCookie ec)
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
private:
};
