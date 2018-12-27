#include "stdafx.h"
#include "SinstarProxy.h"

CSinstarProxy::CSinstarProxy(HWND hClient, ITextService *pTxtService)
	:m_hSvr(NULL)
	,m_hClient(hClient)
	,m_pTxtService(pTxtService)
{
	m_hSvr = FindWindow(NULL, SINSTAR3_SERVER_HWND);
	SASSERT(m_hSvr);
	SASSERT(m_hClient);
	Login(m_hSvr);
}


CSinstarProxy::~CSinstarProxy()
{
	Logout(m_hSvr);
}

void CSinstarProxy::CallFun(FunParams_Base * param)
{
	__super::CallFun(m_hSvr, param);
}


HWND CSinstarProxy::GetIpcObjectID()
{
	return m_hClient;
}

void CSinstarProxy::OnIMESelect(BOOL bSelect)
{
	Param_OnImeSelect param;
	param.bSelect = bSelect;
	CallFun(&param);
}

void CSinstarProxy::OnCompositionStarted()
{
	Param_OnCompositionStarted param;
	CallFun(&param);
}

void CSinstarProxy::OnCompositionChanged()
{
	Param_OnCompositionChanged param;
	CallFun(&param);
}

void CSinstarProxy::OnCompositionTerminated(bool bClearCtx)
{
	Param_OnCompositionTerminated param;
	param.bClearCtx = bClearCtx;
	CallFun(&param);
}

void CSinstarProxy::OnSetCaretPosition(POINT pt, int nHei)
{
	Param_OnSetCaretPosition param;
	param.pt = pt;
	param.nHei = nHei;
	CallFun(&param);
}

void CSinstarProxy::OnSetFocusSegmentPosition(POINT pt, int nHei)
{
	Param_OnSetFocusSegmentPosition param;
	param.pt = pt;
	param.nHei = nHei;
	CallFun(&param);
}

void CSinstarProxy::ProcessKeyStoke(LPVOID lpImeContext, UINT vkCode, LPARAM lParam, BOOL bKeyDown, BOOL * pbEaten)
{
	Param_ProcessKeyStoke param;
	param.lpImeContext = lpImeContext;
	param.vkCode = vkCode;
	param.lParam = lParam;
	param.bKeyDown = bKeyDown;
	param.bEaten = false;
	CallFun(&param);
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
	CallFun(&param);
	*pbEaten = param.bEaten;
}

void CSinstarProxy::OnSetFocus(BOOL bFocus)
{
	Param_OnSetFocus param;
	param.bFocus = bFocus;
	CallFun(&param);
}

int CSinstarProxy::GetCompositionSegments()
{
	Param_GetCompositionSegments param;
	param.nSegs = 0;
	CallFun(&param);
	return param.nSegs;
}

int CSinstarProxy::GetCompositionSegmentEnd(int iSeg)
{
	Param_GetCompositionSegmentEnd param;
	param.iSeg = iSeg;
	param.iEnd = -1;
	CallFun(&param);
	return param.iEnd;
}

int CSinstarProxy::GetCompositionSegmentAttr(int iSeg)
{
	Param_GetCompositionSegmentAttr param;
	param.iSeg = iSeg;
	param.nAttr = 0;
	CallFun(&param);
	return param.nAttr;
}

void CSinstarProxy::OnOpenStatusChanged(BOOL bOpen)
{
	Param_OnOpenStatusChanged param;
	param.bOpen = bOpen;
	CallFun(&param);
}

void CSinstarProxy::OnConversionModeChanged(EInputMethod uMode)
{
	Param_OnConversionModeChanged param;
	param.uMode = uMode;
	CallFun(&param);
}

void CSinstarProxy::ShowHelp()
{
	Param_ShowHelp param;
	CallFun(&param);
}

EInputMethod CSinstarProxy::GetDefInputMode()
{
	Param_GetDefInputMode param;
	param.uMode = HalfAlphanumeric;
	CallFun(&param);
	return param.uMode;
}

////////////////////////////////////////////////////////////////////////////////
LRESULT CTextServiceProxy::OnInputStringW(SIpcObject *pIpcObj, Param_InputStringW & param)
{
	m_pTxtService->InputStringW(param.buf.c_str(), param.buf.length());
	return LRESULT(1);
}

LRESULT CTextServiceProxy::OnIsCompositing(SIpcObject *pIpcObj, Param_IsCompositing & param)
{
	param.bRet = m_pTxtService->IsCompositing();
	return LRESULT(1);
}

LRESULT CTextServiceProxy::OnStartComposition(SIpcObject *pIpcObj, Param_StartComposition & param)
{
	m_pTxtService->StartComposition(param.lpImeContext);
	return LRESULT(1);
}

LRESULT CTextServiceProxy::OnReplaceSelCompositionW(SIpcObject *pIpcObj, Param_ReplaceSelCompositionW & param)
{
	m_pTxtService->ReplaceSelCompositionW(param.lpImeContext, param.nLeft, param.nRight, param.buf.c_str(), param.buf.length());
	return LRESULT(1);
}

LRESULT CTextServiceProxy::OnUpdateResultAndCompositionStringW(SIpcObject *pIpcObj, Param_UpdateResultAndCompositionStringW & param)
{
	m_pTxtService->UpdateResultAndCompositionStringW(param.lpImeContext, param.resultStr.c_str(), param.resultStr.length(), param.compStr.c_str(), param.compStr.length());
	return LRESULT(1);
}

LRESULT CTextServiceProxy::OnEndComposition(SIpcObject *pIpcObj, Param_EndComposition & param)
{
	m_pTxtService->EndComposition(param.lpImeContext);
	return LRESULT(1);
}

LRESULT CTextServiceProxy::OnGetImeContext(SIpcObject *pIpcObj, Param_GetImeContext & param)
{
	param.lpImeContext = m_pTxtService->GetImeContext();
	return LRESULT(1);
}

LRESULT CTextServiceProxy::OnReleaseImeContext(SIpcObject *pIpcObj, Param_ReleaseImeContext & param)
{
	m_pTxtService->ReleaseImeContext(param.lpImeContext);
	return LRESULT(1);
}

LRESULT CTextServiceProxy::OnSetConversionMode(SIpcObject *pIpcObj, Param_SetConversionMode & param)
{
	m_pTxtService->SetConversionMode(param.mode);
	return LRESULT(1);
}

LRESULT CTextServiceProxy::OnGetConversionMode(SIpcObject *pIpcObj, Param_GetConversionMode & param)
{
	param.mode = m_pTxtService->GetConversionMode();
	return LRESULT(1);
}

LRESULT CTextServiceProxy::OnSetOpenStatus(SIpcObject *pIpcObj, Param_SetOpenStatus & param)
{
	param.bRet = m_pTxtService->SetOpenStatus(param.lpImeContext, param.bOpen);
	return LRESULT(1);
}

LRESULT CTextServiceProxy::OnGetOpenStatus(SIpcObject *pIpcObj, Param_GetOpenStatus & param)
{
	param.bOpen = m_pTxtService->GetOpenStatus(param.lpImeContext);
	return LRESULT(1);
}
