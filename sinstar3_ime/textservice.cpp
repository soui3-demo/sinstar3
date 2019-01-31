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
	UINT64 pCtx = GetImeContext();
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

void CUiWnd::StartComposition(UINT64 imeContext)
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
		if(m_pSinstar3) m_pSinstar3->OnCompositionStarted();
		m_bCompositing = TRUE;
	}
}

//使用指定数据替换当前编码串
// nLeft,nRight: [-1,-1]:在当前光标位置插入
//				 [0,-1]:替换全部内容
//				 [left,right]:替换该范围，都不能为负值
void CUiWnd::ReplaceSelCompositionW(UINT64 imeContext,int nLeft,int nRight,const WCHAR *wszComp,int nLen)
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
	#error "have not been implemented ReplaceSelCompositionA"
#endif//_UNICODE

}

void CUiWnd::UpdateResultAndCompositionStringW(UINT64 imeContext,const WCHAR *wszResultStr,int nResStrLen,const WCHAR *wszCompStr,int nCompStrLen)
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

void CUiWnd::EndComposition(UINT64 imeContext)
{
	if (!imeContext) return;
	CImeContext *pCtx = (CImeContext *)imeContext;
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


UINT64 CUiWnd::GetImeContext()
{
	if(m_pCurContext) return (UINT64)m_pCurContext;
	CImeContext *pContext=new CImeContext;
	HIMC hIMC=(HIMC)GetWindowLongPtr(m_hWnd,IMMGWLP_IMC);
	if(pContext->Init(hIMC)) return (UINT64)pContext;
	delete pContext;
	return NULL;
}

BOOL   CUiWnd::ReleaseImeContext(UINT64 imeContext)
{
	if(m_pCurContext == (CImeContext*)imeContext) return TRUE;
	if(!imeContext) return FALSE;
	delete (CImeContext*)imeContext;
	return TRUE;
}

void  CUiWnd::SetConversionMode(EInputMethod mode)
{
	UINT64 nCtx = GetImeContext();
	CImeContext *pCtx=(CImeContext *)nCtx;
	if(pCtx)
	{
		DWORD dwMode=0;
		switch( mode)
		{
		case HalfAlphanumeric:
			dwMode = IME_CMODE_ROMAN;						
			break;

		case HalfNative:
			dwMode = IME_CMODE_ROMAN | IME_CMODE_NATIVE;	
			break;

		case FullNative:
			dwMode = IME_CMODE_ROMAN | IME_CMODE_FULLSHAPE | IME_CMODE_NATIVE;
			break;

		}
		//首先保证键盘状态同步
		if(mode != HalfAlphanumeric)
		{
			if(!GetOpenStatus(nCtx)) SetOpenStatus(nCtx,TRUE);
			ImmSetConversionStatus(pCtx->_hIMC,dwMode,0);
		}else
		{
			if(GetOpenStatus(nCtx)) SetOpenStatus(nCtx,FALSE);
		}
	}
	ReleaseImeContext(nCtx);
}

EInputMethod CUiWnd::GetConversionMode()
{
	EInputMethod eInputMode =m_pSinstar3?m_pSinstar3->GetDefInputMode():FullNative;

	UINT64 nCtx = GetImeContext();
	CImeContext *pCtx=(CImeContext *)nCtx;
	if(!pCtx) return HalfAlphanumeric;
	DWORD dwMode=0,dwSent=0;
	ImmGetConversionStatus(pCtx->_hIMC,&dwMode,&dwSent);
	ReleaseImeContext(nCtx);

	switch( dwMode)
	{
	case IME_CMODE_ROMAN | IME_CMODE_FULLSHAPE| IME_CMODE_NATIVE:	
		eInputMode = FullNative;
		break;

	case IME_CMODE_ROMAN| IME_CMODE_NATIVE:	
		eInputMode = HalfNative;
		break;

	case IME_CMODE_ROMAN:																				// 0x10
		eInputMode = HalfAlphanumeric;
		break;
	}
	return eInputMode;
}

BOOL CUiWnd::SetOpenStatus(UINT64 imeContext,BOOL bOpen)
{
	if(!imeContext) return FALSE;
	return ImmSetOpenStatus(((CImeContext *)imeContext)->_hIMC,bOpen);
}

BOOL CUiWnd::GetOpenStatus(UINT64 imeContext) const
{
	if(!imeContext) return FALSE;
	return ImmGetOpenStatus(((CImeContext *)imeContext)->_hIMC);
}

DWORD CUiWnd::GetActiveWnd() const
{
	return (DWORD)::GetActiveWindow();
}

