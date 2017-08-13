#include "stdafx.h"


LONG g_cRefDll = -1; // -1 /w no refs, for win95 InterlockedIncrement/Decrement compat


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


#define MAX_TRACE	1024
void Helper_Trace(LPCTSTR pszFormat,...)
{
#ifdef _DEBUG
	static int nIdx=0;
	nIdx++;
	va_list args; 
	TCHAR buf[MAX_TRACE+1];
	TCHAR szFmt[200];
	if(pszFormat[0]=='\n')
	{
		_stprintf_s(szFmt,_T("\n%d\t:%s"),nIdx,pszFormat+1);
	}else
	{
		_stprintf_s(szFmt,_T("\n%d\t:%s"),nIdx,pszFormat);
	}

	va_start( args, pszFormat );
	_vsntprintf(buf,MAX_TRACE, szFmt,args);
	va_end (args);
	buf[MAX_TRACE]=0;
	OutputDebugString(buf);
#ifdef LOGFILE
	{
		FILE *f=_tfopen(LOGFILE,_T("a+"));
		if(f)
		{
			_ftprintf(f,buf);
			fclose(f);
		}
	}
#endif
#endif
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