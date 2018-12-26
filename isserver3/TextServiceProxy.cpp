#include "stdafx.h"
#include "TextServiceProxy.h"

CTextServiceProxy::CTextServiceProxy(void)
{
}

CTextServiceProxy::~CTextServiceProxy(void)
{
}

BOOL CTextServiceProxy::InputStringW(LPCWSTR pszBuf, int nLen)
{
}

BOOL CTextServiceProxy::IsCompositing() const
{

}

void CTextServiceProxy::StartComposition(LPVOID lpImeContext)
{

}

void CTextServiceProxy::ReplaceSelCompositionW(LPVOID lpImeContext,int nLeft,int nRight,const WCHAR *wszComp,int nLen)
{

}

void CTextServiceProxy::UpdateResultAndCompositionStringW(LPVOID lpImeContext,const WCHAR *wszResultStr,int nResStrLen,const WCHAR *wszCompStr,int nCompStrLen)
{

}

void CTextServiceProxy::EndComposition(LPVOID lpImeContext)
{

}

LPVOID CTextServiceProxy::GetImeContext()
{

}

BOOL CTextServiceProxy::ReleaseImeContext(LPVOID lpImeContext)
{

}

void CTextServiceProxy::SetConversionMode(EInputMethod mode)
{

}

EInputMethod CTextServiceProxy::GetConversionMode()
{

}

BOOL CTextServiceProxy::SetOpenStatus(LPVOID lpImeContext,BOOL bOpen)
{

}

BOOL CTextServiceProxy::GetOpenStatus(LPVOID lpImeContext) const
{

}
