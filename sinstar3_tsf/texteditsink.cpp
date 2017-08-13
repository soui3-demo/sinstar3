#include "stdafx.h"
#include "sinstar3_tsf.h"
#include "editsession.h"

CBaiduJPTSF *g_pCurTSF=NULL;


VOID CALLBACK OnTimerProc(HWND hwnd,
						UINT uMsg,
						UINT_PTR idEvent,
						DWORD dwTime
						)
{
	KillTimer(NULL,idEvent);

	DWORD dwCurTime = GetTickCount();	
	if ( dwCurTime - g_pCurTSF->_dwLastLayoutChangeTime < 20)
		return;

	//if(!g_pCurTSF->_bGetLayoutChangeMsg)
	{
		ITfContext *pContext=(ITfContext *)g_pCurTSF->GetImeContext();
		if(pContext)
		{
			ITfContextView *pCtxView;
			pContext->GetActiveView(&pCtxView);
			if(pCtxView)
			{
				g_pCurTSF->OnLayoutChange(pContext,TF_LC_CHANGE,pCtxView);
				pCtxView->Release();
			}
			g_pCurTSF->ReleaseImeContext(pContext);
		}
	}
}

STDAPI CBaiduJPTSF::OnEndEdit(ITfContext *pContext, TfEditCookie ecReadOnly, ITfEditRecord *pEditRecord)
{
    BOOL fSelectionChanged;
    IEnumTfRanges *pEnumTextChanges;
    ITfRange *pRange;
	Helper_Trace(L"ITfTextEditSink::OnEndEdit cookie:%u", ecReadOnly);

    //
    // did the selection change?
    // The selection change includes the movement of caret as well. 
    // The caret position is represent as the empty selection range when
    // there is no selection.
    //
 	HRESULT hr=pEditRecord->GetSelectionStatus(&fSelectionChanged);
     if (SUCCEEDED(hr) && fSelectionChanged)
    {
        // If the selection is moved to out side of the current composition,
        // terminate the composition. This TextService supports only one
        // composition in one context object.
        if (_IsComposing())
        {
            TF_SELECTION tfSelection;
            ULONG cFetched;

            if (pContext->GetSelection(ecReadOnly, TF_DEFAULT_SELECTION, 1, &tfSelection, &cFetched) == S_OK && cFetched == 1)
            {
                ITfRange *pRangeComposition;
                // is the insertion point covered by a composition?
                if (_pComposition->GetRange(&pRangeComposition) == S_OK)
                {
                    if (!IsRangeCovered(ecReadOnly, tfSelection.range, pRangeComposition))
                    {
                       _EndComposition(pContext);
                    }

                    pRangeComposition->Release();
                }
				tfSelection.range->Release();
            }
        }
    }

    // text modification?
    if (pEditRecord->GetTextAndPropertyUpdates(TF_GTP_INCL_TEXT, NULL, 0, &pEnumTextChanges) == S_OK)
    {
        if (pEnumTextChanges->Next(1, &pRange, NULL) == S_OK)
        {
            //
            // pRange is the updated range.
            //
			
            pRange->Release();
        }
		if(_IsComposing())
		{
			CComPtr<ITfRange> pStart,pEnd;
			pContext->GetStart(ecReadOnly,&pStart);
			pContext->GetEnd(ecReadOnly,&pEnd);
			if(pStart && pEnd)
			{
				LONG lResult=1;
				pEnd->CompareEnd(ecReadOnly,pStart,TF_ANCHOR_START,&lResult);
				if(lResult==0)
				{
					_EndCompositionEx();
				}
			}
		}
       pEnumTextChanges->Release();
    }
	//在XP下的Word中需要这一段代码才能收到TF_LC_CHANGE消息,目前还不清楚原因。
	g_pCurTSF=this;	
	_dwLastLayoutChangeTime = GetTickCount();
	SetTimer(NULL,0,20,OnTimerProc);

    return S_OK;
}


//+---------------------------------------------------------------------------
//
// _InitTextEditSink
//
// Init a text edit sink on the topmost context of the document.
// Always release any previous sink.
//----------------------------------------------------------------------------

BOOL CBaiduJPTSF::_InitTextEditSink(ITfDocumentMgr *pDocMgr)
{
    ITfSource *pSource;
    BOOL fRet;

    // clear out any previous sink first

    if (_dwTextEditSinkCookie != TF_INVALID_COOKIE)
    {
        if (_pTextEditSinkContext->QueryInterface(IID_ITfSource, (void **)&pSource) == S_OK)
        {
            pSource->UnadviseSink(_dwTextEditSinkCookie);
            pSource->Release();
        }

        _pTextEditSinkContext->Release();
        _pTextEditSinkContext = NULL;
        _dwTextEditSinkCookie = TF_INVALID_COOKIE;
    }

    if (pDocMgr == NULL)
        return FALSE; // caller just wanted to clear the previous sink

    // setup a new sink advised to the topmost context of the document

    if (pDocMgr->GetTop(&_pTextEditSinkContext) != S_OK)
        return FALSE;

    if (_pTextEditSinkContext == NULL)
        return FALSE; // empty document, no sink possible

    fRet = FALSE;

    if (_pTextEditSinkContext->QueryInterface(IID_ITfSource, (void **)&pSource) == S_OK)
    {
        if (pSource->AdviseSink(IID_ITfTextEditSink, (ITfTextEditSink *)this, &_dwTextEditSinkCookie) == S_OK)
        {
            fRet = TRUE;
        }
        else
        {
            _dwTextEditSinkCookie = TF_INVALID_COOKIE;
        }
        pSource->Release();
    }

    if (fRet == FALSE)
    {
        _pTextEditSinkContext->Release();
        _pTextEditSinkContext = NULL;
    }

    return fRet;
}
