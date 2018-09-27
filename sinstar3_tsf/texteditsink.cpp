#include "stdafx.h"
#include "sinstar3_tsf.h"
#include "editsession.h"

STDAPI CSinstar3Tsf::OnEndEdit(ITfContext *pContext, TfEditCookie ecReadOnly, ITfEditRecord *pEditRecord)
{
    BOOL fSelectionChanged;
	SLOGFMTI("ITfTextEditSink::OnEndEdit cookie:%u", ecReadOnly);

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

    return S_OK;
}


//+---------------------------------------------------------------------------
//
// _InitTextEditSink
//
// Init a text edit sink on the topmost context of the document.
// Always release any previous sink.
//----------------------------------------------------------------------------

BOOL CSinstar3Tsf::_InitTextEditSink(ITfDocumentMgr *pDocMgr)
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
