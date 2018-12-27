#include "stdafx.h"
#include "TextServiceProxy.h"


HWND CServerObject::GetSvrId()
{
	return m_hSvr;
}

HRESULT CServerObject::CreateConnection(SIpcConnection ** ppConnection) const {
	*ppConnection = new CSvrConnection(NULL);//todo.
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////


BOOL CSvrConnection::InputStringW(LPCWSTR pszBuf, int nLen)
{
	Param_InputStringW param;
	param.buf = wstring(pszBuf, nLen);
	CallFun(&param);
	return param.bRet;
}

BOOL CSvrConnection::IsCompositing() const
{

}

void CSvrConnection::StartComposition(LPVOID lpImeContext)
{

}

void CSvrConnection::ReplaceSelCompositionW(LPVOID lpImeContext,int nLeft,int nRight,const WCHAR *wszComp,int nLen)
{

}

void CSvrConnection::UpdateResultAndCompositionStringW(LPVOID lpImeContext,const WCHAR *wszResultStr,int nResStrLen,const WCHAR *wszCompStr,int nCompStrLen)
{

}

void CSvrConnection::EndComposition(LPVOID lpImeContext)
{

}

LPVOID CSvrConnection::GetImeContext()
{

}

BOOL CSvrConnection::ReleaseImeContext(LPVOID lpImeContext)
{

}

void CSvrConnection::SetConversionMode(EInputMethod mode)
{

}

EInputMethod CSvrConnection::GetConversionMode()
{

}

BOOL CSvrConnection::SetOpenStatus(LPVOID lpImeContext,BOOL bOpen)
{

}

BOOL CSvrConnection::GetOpenStatus(LPVOID lpImeContext) const
{

}

LRESULT CSvrConnection::OnImeSelect(Param_OnImeSelect & param)
{
	m_pSinstar->OnIMESelect(param.bSelect);
	return LRESULT(1);
}
