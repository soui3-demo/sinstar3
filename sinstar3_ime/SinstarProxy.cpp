#include "stdafx.h"
#include "SinstarProxy.h"

CSinstarProxy::CSinstarProxy(HWND hClient, ITextService *pTxtService):m_conn(pTxtService)
{
	m_conn.SetLocalId(hClient,1<<12);
	HWND hSvr = FindWindow(NULL, SINSTAR3_SERVER_HWND);
	SASSERT(hSvr);

	m_conn.ConnectTo(hSvr);
}


CSinstarProxy::~CSinstarProxy()
{
	m_conn.Disconnect();
}

void CSinstarProxy::OnIMESelect(BOOL bSelect)
{
	Param_OnImeSelect param;
	param.bSelect = bSelect;
	m_conn.CallFun(&param);
}

void CSinstarProxy::OnCompositionStarted()
{
	Param_OnCompositionStarted param;
	m_conn.CallFun(&param);
}

void CSinstarProxy::OnCompositionChanged()
{
	Param_OnCompositionChanged param;
	m_conn.CallFun(&param);
}

void CSinstarProxy::OnCompositionTerminated(bool bClearCtx)
{
	Param_OnCompositionTerminated param;
	param.bClearCtx = bClearCtx;
	m_conn.CallFun(&param);
}

void CSinstarProxy::OnSetCaretPosition(POINT pt, int nHei)
{
	Param_OnSetCaretPosition param;
	param.pt = pt;
	param.nHei = nHei;
	m_conn.CallFun(&param);
}

void CSinstarProxy::OnSetFocusSegmentPosition(POINT pt, int nHei)
{
	Param_OnSetFocusSegmentPosition param;
	param.pt = pt;
	param.nHei = nHei;
	m_conn.CallFun(&param);
}

void CSinstarProxy::ProcessKeyStoke(LPVOID lpImeContext, UINT vkCode, LPARAM lParam, BOOL bKeyDown, BOOL * pbEaten)
{
	Param_ProcessKeyStoke param;
	param.lpImeContext = lpImeContext;
	param.vkCode = vkCode;
	param.lParam = lParam;
	param.bKeyDown = bKeyDown;
	param.bEaten = false;
	m_conn.CallFun(&param);
	*pbEaten = param.bEaten;
}

void CSinstarProxy::TranslateKey(LPVOID lpImeContext, UINT vkCode, UINT uScanCode, BOOL bKeyDown, BOOL * pbEaten)
{
	Param_TranslateKey param;
	param.lpImeContext = lpImeContext;
	param.vkCode = vkCode;
	param.uScanCode = uScanCode;
	param.bKeyDown = bKeyDown;
	param.bEaten = false;
	m_conn.CallFun(&param);
	*pbEaten = param.bEaten;
}

void CSinstarProxy::OnSetFocus(BOOL bFocus)
{
	Param_OnSetFocus param;
	param.bFocus = bFocus;
	m_conn.CallFun(&param);
}

int CSinstarProxy::GetCompositionSegments()
{
	Param_GetCompositionSegments param;
	param.nSegs = 0;
	m_conn.CallFun(&param);
	return param.nSegs;
}

int CSinstarProxy::GetCompositionSegmentEnd(int iSeg)
{
	Param_GetCompositionSegmentEnd param;
	param.iSeg = iSeg;
	param.iEnd = -1;
	m_conn.CallFun(&param);
	return param.iEnd;
}

int CSinstarProxy::GetCompositionSegmentAttr(int iSeg)
{
	Param_GetCompositionSegmentAttr param;
	param.iSeg = iSeg;
	param.nAttr = 0;
	m_conn.CallFun(&param);
	return param.nAttr;
}

void CSinstarProxy::OnOpenStatusChanged(BOOL bOpen)
{
	Param_OnOpenStatusChanged param;
	param.bOpen = bOpen;
	m_conn.CallFun(&param);
}

void CSinstarProxy::OnConversionModeChanged(EInputMethod uMode)
{
	Param_OnConversionModeChanged param;
	param.uMode = uMode;
	m_conn.CallFun(&param);
}

void CSinstarProxy::ShowHelp()
{
	Param_ShowHelp param;
	m_conn.CallFun(&param);
}

EInputMethod CSinstarProxy::GetDefInputMode()
{
	Param_GetDefInputMode param;
	param.uMode = HalfAlphanumeric;
	m_conn.CallFun(&param);
	return param.uMode;
}

////////////////////////////////////////////////////////////////////////////////
void CClientConnection::OnInputStringW( Param_InputStringW & param)
{
	m_pTxtService->InputStringW(param.buf.c_str(), param.buf.length());

}

void CClientConnection::OnIsCompositing( Param_IsCompositing & param)
{
	param.bRet = m_pTxtService->IsCompositing();

}

void CClientConnection::OnStartComposition( Param_StartComposition & param)
{
	m_pTxtService->StartComposition(param.lpImeContext);

}

void CClientConnection::OnReplaceSelCompositionW( Param_ReplaceSelCompositionW & param)
{
	m_pTxtService->ReplaceSelCompositionW(param.lpImeContext, param.nLeft, param.nRight, param.buf.c_str(), param.buf.length());

}

void CClientConnection::OnUpdateResultAndCompositionStringW( Param_UpdateResultAndCompositionStringW & param)
{
	m_pTxtService->UpdateResultAndCompositionStringW(param.lpImeContext, param.resultStr.c_str(), param.resultStr.length(), param.compStr.c_str(), param.compStr.length());

}

void CClientConnection::OnEndComposition( Param_EndComposition & param)
{
	m_pTxtService->EndComposition(param.lpImeContext);

}

void CClientConnection::OnGetImeContext( Param_GetImeContext & param)
{
	param.lpImeContext = m_pTxtService->GetImeContext();

}

void CClientConnection::OnReleaseImeContext( Param_ReleaseImeContext & param)
{
	m_pTxtService->ReleaseImeContext(param.lpImeContext);

}

void CClientConnection::OnSetConversionMode( Param_SetConversionMode & param)
{
	m_pTxtService->SetConversionMode(param.mode);

}

void CClientConnection::OnGetConversionMode( Param_GetConversionMode & param)
{
	param.mode = m_pTxtService->GetConversionMode();

}

void CClientConnection::OnSetOpenStatus( Param_SetOpenStatus & param)
{
	param.bRet = m_pTxtService->SetOpenStatus(param.lpImeContext, param.bOpen);

}

void CClientConnection::OnGetOpenStatus( Param_GetOpenStatus & param)
{
	param.bOpen = m_pTxtService->GetOpenStatus(param.lpImeContext);

}