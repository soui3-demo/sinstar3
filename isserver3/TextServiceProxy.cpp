#include "stdafx.h"
#include "TextServiceProxy.h"
#include "Sinstar3Impl.h"


CSvrConnection::CSvrConnection()
{
}

CSvrConnection::~CSvrConnection(void)
{
}

BOOL CSvrConnection::InputStringW(LPCWSTR pszBuf, int nLen)
{
	Param_InputStringW param;
	param.buf = wstring(pszBuf, nLen);
	CallFun(&param);
	return param.bRet;
}

BOOL CSvrConnection::IsCompositing() const
{
	Param_IsCompositing param;
	CallFun(&param);
	return param.bRet;
}


void CSvrConnection::StartComposition(LPVOID lpImeContext)
{
	Param_StartComposition param;
	param.lpImeContext = lpImeContext;
	CallFun(&param);
}

void CSvrConnection::ReplaceSelCompositionW(LPVOID lpImeContext,int nLeft,int nRight,const WCHAR *wszComp,int nLen)
{
	Param_ReplaceSelCompositionW param;
	param.lpImeContext = lpImeContext;
	param.nLeft = nLeft;
	param.nRight = nRight;
	param.buf = wstring(wszComp,nLen);
	CallFun(&param);
}

void CSvrConnection::UpdateResultAndCompositionStringW(LPVOID lpImeContext,const WCHAR *wszResultStr,int nResStrLen,const WCHAR *wszCompStr,int nCompStrLen)
{
	Param_UpdateResultAndCompositionStringW param;
	param.lpImeContext = lpImeContext;
	param.resultStr = wstring(wszResultStr,nResStrLen);
	param.compStr = wstring(wszCompStr,nCompStrLen);
	CallFun(&param);
}

void CSvrConnection::EndComposition(LPVOID lpImeContext)
{
	Param_EndComposition param;
	param.lpImeContext = lpImeContext;
	CallFun(&param);
}

LPVOID CSvrConnection::GetImeContext()
{
	Param_GetImeContext param;
	CallFun(&param);
	return param.lpImeContext;
}

BOOL CSvrConnection::ReleaseImeContext(LPVOID lpImeContext)
{
	Param_ReleaseImeContext param;
	param.lpImeContext =lpImeContext;
	CallFun(&param);
	return param.bRet;
}

void CSvrConnection::SetConversionMode(EInputMethod mode)
{
	Param_SetConversionMode param;
	param.mode = mode;
	CallFun(&param);
}

EInputMethod CSvrConnection::GetConversionMode()
{
	Param_GetConversionMode param;
	CallFun(&param);
	return param.mode;
}

BOOL CSvrConnection::SetOpenStatus(LPVOID lpImeContext,BOOL bOpen)
{
	Param_SetOpenStatus param;
	param.lpImeContext = lpImeContext;
	param.bOpen = bOpen;
	CallFun(&param);
	return param.bRet;
}

BOOL CSvrConnection::GetOpenStatus(LPVOID lpImeContext) const
{
	Param_GetOpenStatus param;
	param.lpImeContext = lpImeContext;
	CallFun(&param);
	return param.bOpen;
}

//////////////////////////////////////////////////////////////////////////
void CSvrConnection::HandleCreate(Param_Create & param)
{
	SASSERT(!m_pSinstar);
	m_pSinstar.Attach(new CSinstar3Impl(this, param.hOwner));
}

void CSvrConnection::HandleDestroy(Param_Destroy & param)
{
	SASSERT(m_pSinstar);
	m_pSinstar = NULL;
}

void CSvrConnection::HandleOnImeSelect(Param_OnImeSelect & param)
{
	m_pSinstar->OnIMESelect(param.bSelect);

}

void CSvrConnection::HandleOnCompositionStarted(Param_OnCompositionStarted &param)
{
	m_pSinstar->OnCompositionStarted();

}

void CSvrConnection::HandleOnCompositionChanged(Param_OnCompositionChanged &param)
{
	m_pSinstar->OnCompositionChanged();

}

void CSvrConnection::HandleOnCompositionTerminated(Param_OnCompositionTerminated &param)
{
	m_pSinstar->OnCompositionTerminated(param.bClearCtx);

}

void CSvrConnection::HandleOnSetCaretPosition(Param_OnSetCaretPosition &param)
{
	m_pSinstar->OnSetCaretPosition(param.pt,param.nHei);

}

void CSvrConnection::HandleOnSetFocusSegmentPosition(Param_OnSetFocusSegmentPosition &param)
{
	m_pSinstar->OnSetFocusSegmentPosition(param.pt,param.nHei);

}

void CSvrConnection::HandleProcessKeyStoke(Param_ProcessKeyStoke &param)
{
	m_pSinstar->ProcessKeyStoke(param.lpImeContext,param.vkCode,param.lParam,param.bKeyDown,&param.bEaten);

}

void CSvrConnection::HandleTranslateKey(Param_TranslateKey &param)
{
	m_pSinstar->TranslateKey(param.lpImeContext,param.vkCode,param.uScanCode,param.bKeyDown,&param.bEaten);

}

void CSvrConnection::HandleOnSetFocus(Param_OnSetFocus &param)
{
	m_pSinstar->OnSetFocus(param.bFocus);

}

void CSvrConnection::HandleGetCompositionSegments(Param_GetCompositionSegments &param)
{
	param.nSegs = m_pSinstar->GetCompositionSegments();

}

void CSvrConnection::HandleGetCompositionSegmentEnd(Param_GetCompositionSegmentEnd &param)
{
	param.iEnd = m_pSinstar->GetCompositionSegmentEnd(param.iSeg);

}

void CSvrConnection::HandleGetCompositionSegmentAttr(Param_GetCompositionSegmentAttr &param)
{
	param.nAttr = m_pSinstar->GetCompositionSegmentAttr(param.iSeg);

}

void CSvrConnection::HandleOnOpenStatusChanged(Param_OnOpenStatusChanged &param)
{
	m_pSinstar->OnOpenStatusChanged(param.bOpen);

}

void CSvrConnection::HandleOnConversionModeChanged(Param_OnConversionModeChanged &param)
{
	m_pSinstar->OnConversionModeChanged(param.uMode);

}

void CSvrConnection::HandleShowHelp(Param_ShowHelp &param)
{
	m_pSinstar->ShowHelp();

}

void CSvrConnection::HandleGetDefInputMode(Param_GetDefInputMode &param)
{
	param.uMode = m_pSinstar->GetDefInputMode();

}
