#include "stdafx.h"


BOOL IsRangeCovered(TfEditCookie ec, ITfRange *pRangeTest, ITfRange *pRangeCover)
{
	LONG lResult;

	if (pRangeCover->CompareStart(ec, pRangeTest, TF_ANCHOR_START, &lResult) != S_OK ||
		lResult > 0)
	{
		return FALSE;
	}

	if (pRangeCover->CompareEnd(ec, pRangeTest, TF_ANCHOR_END, &lResult) != S_OK ||
		lResult < 0)
	{
		return FALSE;
	}

	return TRUE;
}


//+---------------------------------------------------------------------------
//
// AdviseSink
//
//----------------------------------------------------------------------------

BOOL AdviseSink(IUnknown *pSourceIn, IUnknown *pSink, REFIID riid, DWORD *pdwCookie)
{
    ITfSource *pSource;
    HRESULT hr;

    if (pSourceIn->QueryInterface(IID_ITfSource, (void **)&pSource) != S_OK)
        return FALSE;

    hr = pSource->AdviseSink(riid, pSink, pdwCookie);

    pSource->Release();

    if (hr != S_OK)
    {
        // make sure we don't try to Unadvise pdwCookie later
        *pdwCookie = TF_INVALID_COOKIE;
        return FALSE;
    }

    return TRUE;
}

//+---------------------------------------------------------------------------
//
// UnadviseSink
//
//----------------------------------------------------------------------------

void UnadviseSink(IUnknown *pSourceIn, DWORD *pdwCookie)
{
    ITfSource *pSource;

    if (*pdwCookie == TF_INVALID_COOKIE)
        return; // never Advised

    if (pSourceIn->QueryInterface(IID_ITfSource, (void **)&pSource) == S_OK)
    {
        pSource->UnadviseSink(*pdwCookie);
        pSource->Release();
    }

    *pdwCookie = TF_INVALID_COOKIE;
}

//+---------------------------------------------------------------------------
//
// AdviseSingleSink
//
//----------------------------------------------------------------------------

BOOL AdviseSingleSink(TfClientId tfClientId, IUnknown *pSourceIn, IUnknown *pSink, REFIID riid)
{
    ITfSourceSingle *pSource;
    HRESULT hr;

    if (pSourceIn->QueryInterface(IID_ITfSourceSingle, (void **)&pSource) != S_OK)
        return FALSE;

    hr = pSource->AdviseSingleSink(tfClientId, riid, pSink);

    pSource->Release();

    return (hr == S_OK);
}

//+---------------------------------------------------------------------------
//
// UnadviseSingleSink
//
//----------------------------------------------------------------------------

void UnadviseSingleSink(TfClientId tfClientId, IUnknown *pSourceIn, REFIID riid)
{
    ITfSourceSingle *pSource;

    if (pSourceIn->QueryInterface(IID_ITfSourceSingle, (void **)&pSource) == S_OK)
    {
        pSource->UnadviseSingleSink(tfClientId, riid);
        pSource->Release();
    }
}