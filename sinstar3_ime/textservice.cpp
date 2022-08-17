#include "stdafx.h"
#include "UiWnd.h"
#include "CompStrEx.h"
#include "../include/sinstar3_guids.h"



BOOL CUiWnd::InputStringW(LPCWSTR pszBuf, int nLen)
{
	if (IsCompositing())
	{
		return FALSE;
	}
	CImeContext * pCtx = GetImeContext();
	if (!pCtx) return FALSE;
	StartComposition(pCtx);
	UpdateResultAndCompositionStringW(pCtx, pszBuf, nLen, NULL, 0);
	EndComposition(pCtx);
	ReleaseImeContext(pCtx);
	return TRUE;
}

BOOL CUiWnd::IsCompositing() const
{
	return m_bCompositing;
}

void CUiWnd::StartComposition(CImeContext * imeContext)
{
	if (!imeContext) return;
	if(!IsCompositing())
	{
		CImeContext *pCtx=(CImeContext *)imeContext;
		CCompStrEx *pCompStr=(CCompStrEx *)ImmLockIMCC(pCtx->_lpContext->hCompStr);
		_ASSERT(pCompStr);
		ImmUnlockIMCC(pCtx->_lpContext->hCompStr);
		pCtx->GenerateMessage(WM_IME_STARTCOMPOSITION,0,0);
		pCtx->GenerateMessage(WM_IME_NOTIFY,IMN_SETCOMPOSITIONWINDOW,0);
		if(m_pSinstar3) m_pSinstar3->OnCompositionStarted(true);
		m_bCompositing = TRUE;
	}
}

//使用指定数据替换当前编码串
// nLeft,nRight: [-1,-1]:在当前光标位置插入
//				 [0,-1]:替换全部内容
//				 [left,right]:替换该范围，都不能为负值
void CUiWnd::ReplaceSelCompositionW(CImeContext * imeContext,int nLeft,int nRight,const WCHAR *wszComp,int nLen)
{
	if (!imeContext) return;
#ifdef _UNICODE
	_ASSERT(m_pSinstar3);
	if(!IsCompositing()) StartComposition(imeContext);
	CImeContext *pCtx=(CImeContext *)imeContext;
	CCompStrEx *pCompStr=(CCompStrEx *)ImmLockIMCC(pCtx->_lpContext->hCompStr);
	if(pCompStr)
	{
		pCompStr->Insert(m_pSinstar3,nLeft,nRight,wszComp,nLen);
		pCtx->GenerateMessage(WM_IME_COMPOSITION,0,GCS_CURSORPOS|GCS_COMP|GCS_COMPREAD);
		if(m_pSinstar3)
		{
			m_pSinstar3->OnCompositionChanged();
			if(pCompStr->GetTargetPos()==-1)
			{
				POINT pt={-1,-1};
				m_pSinstar3->OnSetFocusSegmentPosition(pt,0);
			}
		}
		pCtx->GenerateMessage(WM_IME_NOTIFY,IMN_CHANGECANDIDATE,0);
		pCtx->GenerateMessage(WM_IME_NOTIFY,IMN_OPENCANDIDATE,0);

		if(pCompStr->dwCompStrLen==0) 	EndComposition(imeContext);

	}else
	{
		SLOGFMTI("ReplaceSelCompositionW: lock compstr failed");
	}
	ImmUnlockIMCC(pCtx->_lpContext->hCompStr);
#else
	#error " ReplaceSelCompositionA have not been implemented"
#endif//_UNICODE

}

void CUiWnd::UpdateResultAndCompositionStringW(CImeContext * imeContext,const WCHAR *wszResultStr,int nResStrLen,const WCHAR *wszCompStr,int nCompStrLen)
{
	if (!imeContext) return;
#ifdef _UNICODE
	_ASSERT(m_pSinstar3);
	if(!IsCompositing()) StartComposition(imeContext);
	CImeContext *pCtx=(CImeContext *)imeContext;
	CCompStrEx *pCompStr=(CCompStrEx *)ImmLockIMCC(pCtx->_lpContext->hCompStr);
	if(pCompStr)
	{
		pCompStr->Init();
		pCompStr->Insert(m_pSinstar3,0,-1,wszCompStr,nCompStrLen);
		pCompStr->dwResultStrLen=nResStrLen;
		_tcsncpy(pCompStr->szResultStr,wszResultStr,nResStrLen);
		pCtx->GenerateMessage(WM_IME_COMPOSITION,0,GCS_CURSORPOS|GCS_COMP|GCS_COMPREAD|GCS_RESULTSTR);
		pCtx->GenerateMessage(WM_IME_NOTIFY,IMN_CHANGECANDIDATE,0);
		if(m_pSinstar3)
		{
			m_pSinstar3->OnCompositionChanged();
			POINT pt={-1,-1};
			m_pSinstar3->OnSetFocusSegmentPosition(pt,0);
		}
		pCtx->GenerateMessage(WM_IME_NOTIFY,IMN_SETCOMPOSITIONWINDOW,0);
		ImmUnlockIMCC(pCtx->_lpContext->hCompStr);
	}else
	{
		SLOGFMTI("ReplaceSelCompositionW: lock compstr failed");
	}
#else
	SLOGFMTI("not implemented ReplaceSelCompositionA");
#endif//_UNICODE
}

void CUiWnd::EndComposition(CImeContext * imeContext)
{
	if (!imeContext) return;
	CImeContext *pCtx = imeContext;
	CCompStrEx *pCompStr = (CCompStrEx *)ImmLockIMCC(pCtx->_lpContext->hCompStr);
	if (pCompStr && pCompStr->dwCompStrLen)
	{
		TCHAR szResult[MAXCOMPSIZE * 2];
		DWORD dwResStrLen = pCompStr->dwCompStrLen;
		_tcsncpy(szResult, pCompStr->szCompStr, pCompStr->dwCompStrLen);
		pCompStr->Init();
		_tcsncpy(pCompStr->szResultStr, szResult, dwResStrLen);
		pCompStr->dwResultStrLen = dwResStrLen;
		pCtx->GenerateMessage(WM_IME_COMPOSITION, 0, GCS_RESULTSTR | GCS_COMP);
	}
	ImmUnlockIMCC(pCtx->_lpContext->hCompStr);
	pCtx->GenerateMessage(WM_IME_NOTIFY, IMN_CLOSECANDIDATE, 0);
	pCtx->GenerateMessage(WM_IME_ENDCOMPOSITION, 0, 0);
	m_bCompositing = FALSE;
}


CImeContext * CUiWnd::GetImeContext() const
{
	if(m_pCurContext) 
		return m_pCurContext;
	CImeContext *pContext=new CImeContext;
	HIMC hIMC=(HIMC)GetWindowLongPtr(m_hWnd,IMMGWLP_IMC);
	if (!pContext->Init(hIMC))
	{
		delete pContext;
		return NULL;
	}
	return pContext;
}

void  CUiWnd::ReleaseImeContext(CImeContext * imeContext) const
{
	if(m_pCurContext == imeContext) 
		return;
	if(imeContext) 
		delete imeContext;
}

void  CUiWnd::SetConversionMode(EInputMethod mode)
{
	//todo:hjx
}

EInputMethod CUiWnd::GetConversionMode()
{
	return FullNative;
}

void CUiWnd::SetOpenStatus(CImeContext * imeContext,BOOL bOpen)
{
	if(!imeContext) return;
	ImmSetOpenStatus(imeContext->_hIMC,bOpen);
}

BOOL CUiWnd::GetOpenStatus(CImeContext * imeContext) const
{
	if(!imeContext) return FALSE;
	return ImmGetOpenStatus(imeContext->_hIMC);
}

DWORD CUiWnd::GetActiveWnd() const
{
	return (DWORD)(ULONG_PTR)::GetActiveWindow();
}

