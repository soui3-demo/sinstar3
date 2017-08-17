#pragma once

#include "sinstar3_tsf.h"

class CEditSessionBase : public ITfEditSession
{
public:
    CEditSessionBase(CSinstar3Tsf *pTextService, ITfContext *pContext);
	virtual ~CEditSessionBase(){}
	
    virtual STDMETHODIMP DoEditSession(TfEditCookie ec) = 0;
public:
	IUNKNOWN_BEGIN(IID_ITfEditSession,ITfEditSession)
	IUNKNOWN_END()

protected:
    CComPtr<ITfContext> _pContext;
    CComPtr<CSinstar3Tsf> _pTextService;

};


class CEditSessionStartComposition : public CEditSessionBase
{
public:
	CEditSessionStartComposition(CSinstar3Tsf *pTextService, ITfContext *pContext);

	// ITfEditSession
	STDMETHODIMP DoEditSession(TfEditCookie ec);
};

class CEditSessionEndComposition : public CEditSessionBase
{
public:
	CEditSessionEndComposition(CSinstar3Tsf *pTextService, ITfContext *pContext);
	// ITfEditSession
	STDMETHODIMP DoEditSession(TfEditCookie ec);
};


class CEditSessionGetTextExtent : public CEditSessionBase
{
public:

	CEditSessionGetTextExtent(CSinstar3Tsf *pTextService, ITfContext *pContext, ITfContextView *pContextView);

	STDMETHODIMP DoEditSession(TfEditCookie ec);

private:
	BOOL GetLastLineRect( TfEditCookie ec, ITfRange* range, int nLen, RECT& rcLast, BOOL bUseSavedPos);

	CComPtr<ITfContextView> _pContextView;
};



class CEditSessionKeyIn:public CEditSessionBase
{
public:
	CEditSessionKeyIn(CSinstar3Tsf *pTextService, ITfContext *pContext,int nLeft,int nRight,LPCWSTR pszBuf,int nLen);
	~CEditSessionKeyIn();

	// ITfEditSession
	STDMETHODIMP DoEditSession(TfEditCookie ec);
private:
	LPWSTR m_pszBuf;
	int		m_nLen;
	int		m_nLeft,m_nRight;
};


class CEditSessionMoveCaret:public CEditSessionBase
{
public:
	CEditSessionMoveCaret(CSinstar3Tsf *pTextService, ITfContext *pContext,int nPos,BOOL bSet,ITfComposition *pComposition);

	// ITfEditSession
	STDMETHODIMP DoEditSession(TfEditCookie ec);
private:
	CComPtr<ITfComposition> m_pComposition;
	int m_nPos;
	BOOL m_bSet;
};


class CEditSessionUpdateResultAndComp:public CEditSessionBase
{
public:
	CEditSessionUpdateResultAndComp(CSinstar3Tsf *pTextService, ITfContext *pContext,LPCWSTR pszResultStr,int nResStrLen,LPCWSTR pszCompStr,int nCompStrLen);
	~CEditSessionUpdateResultAndComp();

	// ITfEditSession
	STDMETHODIMP DoEditSession(TfEditCookie ec);
private:
	WCHAR *m_pszResultStr,*m_pszCompStr;
	int    m_nResStrLen,m_nCompStrLen;
};
