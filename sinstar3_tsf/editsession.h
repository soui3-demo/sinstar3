#pragma once

#include "sinstar3_tsf.h"

class CEditSessionBase : public CUnknown,public ITfEditSession
{
public:
    CEditSessionBase(CSinstar3Tsf *pTextService, ITfContext *pContext);
	virtual ~CEditSessionBase(){}
	
    virtual STDMETHODIMP DoEditSession(TfEditCookie ec) = 0;

	CSinstar3Tsf * GetTextService(){ return _pTextService;};
	ITfContext * GetContext() {return _pContext;}
	ISinstar * GetSinstar3() {
		return _pTextService->m_pSinstar3;
	}
protected:
    CComPtr<ITfContext> _pContext;
    CComPtr<CSinstar3Tsf> _pTextService;

public:
	IUNKNOWN_BEGIN(ITfEditSession)
	IUNKNOWN_END()
};

/////////////////////////////////////////////////////////////////////////////////////
class CEsKeyHandler : public CEditSessionBase
{
public:
	CEsKeyHandler(CSinstar3Tsf *pTextService, ITfContext *pContext,WPARAM wParam, LPARAM lParam)
		:CEditSessionBase(pTextService,pContext)
		,_wParam(wParam)
		,_lParam(lParam)
	{

	}

	STDMETHODIMP DoEditSession(TfEditCookie ec);

private:
	WPARAM _wParam;
	LPARAM _lParam;
};

class CEsStartComposition : public CEditSessionBase
{
public:
	CEsStartComposition(CSinstar3Tsf *pTextService, ITfContext *pContext);

	// ITfEditSession
	STDMETHODIMP DoEditSession(TfEditCookie ec);
};

class CEsEndComposition : public CEditSessionBase
{
public:
	CEsEndComposition(CSinstar3Tsf *pTextService, ITfContext *pContext);
	// ITfEditSession
	STDMETHODIMP DoEditSession(TfEditCookie ec);
};


class CEsGetTextExtent : public CEditSessionBase
{
public:

	CEsGetTextExtent(CSinstar3Tsf *pTextService, ITfContext *pContext, ITfContextView *pContextView);

	STDMETHODIMP DoEditSession(TfEditCookie ec);
private:

	CComPtr<ITfContextView> _pContextView;
};



class CEsChangeComposition:public CEditSessionBase
{
public:
	CEsChangeComposition(CSinstar3Tsf *pTextService, ITfContext *pContext,int nLeft,int nRight,LPCWSTR pszBuf,int nLen);
	~CEsChangeComposition();

	// ITfEditSession
	STDMETHODIMP DoEditSession(TfEditCookie ec);
private:
	LPWSTR m_pszBuf;
	int		m_nLen;
	int		m_nLeft,m_nRight;
};


class CEsMoveCaret:public CEditSessionBase
{
public:
	CEsMoveCaret(CSinstar3Tsf *pTextService, ITfContext *pContext,int nPos,BOOL bSet,ITfComposition *pComposition);

	// ITfEditSession
	STDMETHODIMP DoEditSession(TfEditCookie ec);
private:
	CComPtr<ITfComposition> m_pComposition;
	int m_nPos;
	BOOL m_bSet;
};


class CEsUpdateResultAndComp:public CEditSessionBase
{
public:
	CEsUpdateResultAndComp(CSinstar3Tsf *pTextService, ITfContext *pContext,LPCWSTR pszResultStr,int nResStrLen,LPCWSTR pszCompStr,int nCompStrLen);
	~CEsUpdateResultAndComp();

	// ITfEditSession
	STDMETHODIMP DoEditSession(TfEditCookie ec);
private:
	WCHAR *m_pszResultStr,*m_pszCompStr;
	int    m_nResStrLen,m_nCompStrLen;
};
