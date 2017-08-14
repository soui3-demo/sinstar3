#include "stdafx.h"
#include "UiWnd.h"
#include "CompStrEx.h"
#include "../include/sinstar3_guids.h"

BOOL CUiWnd::IsCompositing()
{
	CImeContext *pCtx=(CImeContext *)GetImeContext();
	if(!pCtx) return FALSE;
	BOOL bRet=FALSE;
	CCompStrEx *pCompStr=(CCompStrEx *)ImmLockIMCC(pCtx->_lpContext->hCompStr);
	if(pCompStr)
	{
		bRet=pCompStr->bComposing;
		ImmUnlockIMCC(pCtx->_lpContext->hCompStr);
	}
	ReleaseImeContext(pCtx);
	return bRet;
}

void CUiWnd::StartComposition(LPVOID lpImeContext)
{
	_ASSERT(lpImeContext && !IsCompositing());
	CImeContext *pCtx=(CImeContext *)lpImeContext;
	CCompStrEx *pCompStr=(CCompStrEx *)ImmLockIMCC(pCtx->_lpContext->hCompStr);
	_ASSERT(pCompStr);
	pCompStr->Init();
	pCompStr->bComposing=TRUE;
	ImmUnlockIMCC(pCtx->_lpContext->hCompStr);
	pCtx->GenerateMessage(WM_IME_STARTCOMPOSITION,0,0);
	pCtx->GenerateMessage(WM_IME_NOTIFY,IMN_SETCOMPOSITIONWINDOW,0);
	if(m_pSinstar3) m_pSinstar3->OnCompositionStarted();
}

//使用指定数据替换当前编码串
// nLeft,nRight: [-1,-1]:在当前光标位置插入
//				 [0,-1]:替换全部内容
//				 [left,right]:替换该范围，都不能为负值
void CUiWnd::ReplaceSelCompositionW(LPVOID lpImeContext,int nLeft,int nRight,WCHAR *wszComp,int nLen)
{
#ifdef _UNICODE
	_ASSERT(lpImeContext);
	_ASSERT(m_pSinstar3);
	if(!IsCompositing()) StartComposition(lpImeContext);
	CImeContext *pCtx=(CImeContext *)lpImeContext;
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

		if(pCompStr->dwCompStrLen==0) 	EndComposition(lpImeContext);

	}else
	{
		SLOGFMTF("ReplaceSelCompositionW: lock compstr failed");
	}
	ImmUnlockIMCC(pCtx->_lpContext->hCompStr);
#else
	#error "have not been implemented ReplaceSelCompositionA"
#endif//_UNICODE

}

void CUiWnd::UpdateResultAndCompositionStringW(LPVOID lpImeContext,WCHAR *wszResultStr,int nResStrLen,WCHAR *wszCompStr,int nCompStrLen)
{
#ifdef _UNICODE
	_ASSERT(lpImeContext);
	_ASSERT(m_pSinstar3);
	if(!IsCompositing()) StartComposition(lpImeContext);
	CImeContext *pCtx=(CImeContext *)lpImeContext;
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
		SLOGFMTF("ReplaceSelCompositionW: lock compstr failed");
	}
#else
	SLOGFMTF("not implemented ReplaceSelCompositionA");
#endif//_UNICODE
}

void CUiWnd::EndComposition(LPVOID lpImeContext)
{
	_ASSERT(lpImeContext);
	CImeContext *pCtx=(CImeContext *)lpImeContext;
	CCompStrEx *pCompStr=(CCompStrEx *)ImmLockIMCC(pCtx->_lpContext->hCompStr);
	if(pCompStr && pCompStr->dwCompStrLen)
	{
		TCHAR szResult[MAXCOMPSIZE*2];
		DWORD dwResStrLen=pCompStr->dwCompStrLen;
		_tcsncpy(szResult,pCompStr->szCompStr,pCompStr->dwCompStrLen);
		pCompStr->Init();
		_tcsncpy(pCompStr->szResultStr,szResult,dwResStrLen);
		pCompStr->dwResultStrLen=dwResStrLen;
		pCtx->GenerateMessage(WM_IME_COMPOSITION,0,GCS_RESULTSTR|GCS_COMP);
	}
	pCompStr->bComposing=FALSE;
	ImmUnlockIMCC(pCtx->_lpContext->hCompStr);
	pCtx->GenerateMessage(WM_IME_NOTIFY,IMN_CLOSECANDIDATE,0);
	pCtx->GenerateMessage(WM_IME_ENDCOMPOSITION,0,0);
	if(m_pSinstar3) m_pSinstar3->OnCompositionTerminated();

}

int  CUiWnd::MoveCaretPos(LPVOID lpImeContext,int nPos,BOOL bSet)
{
	_ASSERT(lpImeContext);
	CImeContext *pCtx=(CImeContext *)lpImeContext;
	CCompStrEx *pCompStr=(CCompStrEx *)ImmLockIMCC(pCtx->_lpContext->hCompStr);
	int nRet=0;
	if(pCompStr)
	{
		DWORD dwOldPos=pCompStr->dwCursorPos;
		pCompStr->MoveCaret(nPos,bSet);
		nRet=pCompStr->dwCursorPos;
		if(dwOldPos != pCompStr->dwCursorPos)	//需要GCS_COMPREAD|GCS_COMP,否则在word中不能显示
		{
			pCtx->GenerateMessage(WM_IME_COMPOSITION,0,GCS_CURSORPOS|GCS_COMPREAD|GCS_COMP);
			pCtx->GenerateMessage(WM_IME_NOTIFY,IMN_CHANGECANDIDATE,0);
			if(m_pSinstar3) m_pSinstar3->OnCompositionChanged();
		}
	}
	ImmUnlockIMCC(pCtx->_lpContext->hCompStr);
	return nRet;
}

LPVOID CUiWnd::GetImeContext()
{
	if(m_pCurContext) return m_pCurContext;
	CImeContext *pContext=new CImeContext;
	HIMC hIMC=(HIMC)GetWindowLongPtr(m_hWnd,IMMGWLP_IMC);
	if(pContext->Init(hIMC)) return pContext;
	delete pContext;
	return NULL;
}

BOOL   CUiWnd::ReleaseImeContext(LPVOID lpImeContext)
{
	if(m_pCurContext == (CImeContext*)lpImeContext) return TRUE;
	if(!lpImeContext) return FALSE;
	delete (CImeContext*)lpImeContext;
	return TRUE;
}

void  CUiWnd::SetConversionMode(EInputMethod mode)
{
	CImeContext *pCtx=(CImeContext *)GetImeContext();
	if(pCtx)
	{
		DWORD dwMode=0;
		switch( mode)
		{
		case Hiragana:
		case HalfAlphanumeric:
			dwMode = IME_CMODE_ROMAN | IME_CMODE_FULLSHAPE | IME_CMODE_NATIVE;						// 0x19
			break;

		case FullKatakana:
			dwMode = IME_CMODE_ROMAN | IME_CMODE_FULLSHAPE | IME_CMODE_KATAKANA | IME_CMODE_NATIVE;	// 0x1b
			break;

		case FullAlphanumeric:
			dwMode = IME_CMODE_ROMAN | IME_CMODE_FULLSHAPE;													// 0x18
			break;

		case HalfKatakana:
			dwMode = IME_CMODE_ROMAN | IME_CMODE_KATAKANA | IME_CMODE_NATIVE;							// 0x13
			break;

		}
		//首先保证键盘状态同步
		if(mode != HalfAlphanumeric)
		{
			if(!GetOpenStatus(pCtx)) SetOpenStatus(pCtx,TRUE);
			ImmSetConversionStatus(pCtx->_hIMC,dwMode,0);
		}else
		{
			if(GetOpenStatus(pCtx)) SetOpenStatus(pCtx,FALSE);
		}
	}
	ReleaseImeContext(pCtx);
}

EInputMethod CUiWnd::GetConversionMode()
{
	EInputMethod eInputMode =m_pSinstar3?m_pSinstar3->GetDefInputMode():Hiragana;

	CImeContext *pCtx=(CImeContext *)GetImeContext();
	if(!pCtx) return HalfAlphanumeric;
	DWORD dwMode=0,dwSent=0;
	ImmGetConversionStatus(pCtx->_hIMC,&dwMode,&dwSent);
	ReleaseImeContext(pCtx);

	switch( dwMode)
	{
	case IME_CMODE_ROMAN | IME_CMODE_FULLSHAPE | IME_CMODE_NATIVE:						// 0x19
		eInputMode = Hiragana;
		break;

	case IME_CMODE_ROMAN | IME_CMODE_FULLSHAPE | IME_CMODE_KATAKANA | IME_CMODE_NATIVE:	// 0x1b
		eInputMode = FullKatakana;
		break;

	case IME_CMODE_ROMAN | IME_CMODE_FULLSHAPE:													// 0x18
		eInputMode = FullAlphanumeric;
		break;

	case IME_CMODE_ROMAN | IME_CMODE_KATAKANA | IME_CMODE_NATIVE:						// 0x13
		eInputMode = HalfKatakana;
		break;

	case IME_CMODE_ROMAN:																				// 0x10
		eInputMode = HalfAlphanumeric;
		break;
	}
	return eInputMode;
}

BOOL  CUiWnd::RegisterIMEHotKey(REFGUID guidHotKey,LPCWSTR pszName,const PRESERVEDKEY *pKey)
{
	return CHotKeyMgr::RegisterHotKey(guidHotKey,pKey);
}

BOOL  CUiWnd::UnregisterIMEHotKey(REFGUID guidHotKey,const PRESERVEDKEY *pKey)
{
	return CHotKeyMgr::UnregisterHotKey(guidHotKey,pKey);
}

BOOL CUiWnd::SetOpenStatus(LPVOID lpImeContext,BOOL bOpen)
{
	if(!lpImeContext) return FALSE;
	return ImmSetOpenStatus(((CImeContext *)lpImeContext)->_hIMC,bOpen);
}

BOOL CUiWnd::GetOpenStatus(LPVOID lpImeContext)
{
	if(!lpImeContext) return FALSE;
	return ImmGetOpenStatus(((CImeContext *)lpImeContext)->_hIMC);
}

#ifdef ENABLE_LANGUAGEBAR

HMODULE GetMsctfModule()
{
	static HMODULE hModMsctf = ::LoadLibrary (_T("msctf.dll"));
	return hModMsctf;
}

BOOL _QueryLangBarItemMgr(ITfLangBarItemMgr **pLangBarMgr)
{
	if(g_dwSystemInfoFlags&IME_SYSINFO_WINLOGON) return FALSE;
	BOOL bRet=FALSE;
	HMODULE hModMsctf=GetMsctfModule();
	if(hModMsctf)
	{
		typedef HRESULT (WINAPI *PTF_CREATELANGBARITEMMGR)(ITfLangBarItemMgr**);
		PTF_CREATELANGBARITEMMGR pfnCreateLangBarItemMgr = (PTF_CREATELANGBARITEMMGR)::GetProcAddress (hModMsctf, "TF_CreateLangBarItemMgr");

		if(pfnCreateLangBarItemMgr)
		{
			HRESULT hr=pfnCreateLangBarItemMgr(pLangBarMgr);
			bRet=SUCCEEDED(hr);
		}
	}

	return bRet;
}

BOOL _QueryITfThreadMgr(ITfThreadMgr** ppITfThreadMgr)
{
	typedef HRESULT (WINAPI *PTF_CREATETHREADMGR)(ITfThreadMgr**);

	return FALSE;
	if(g_dwSystemInfoFlags&IME_SYSINFO_WINLOGON) return FALSE;

	HRESULT	hResult;

	HMODULE hMSCTF=GetMsctfModule();

	if(0 == hMSCTF)
	{
		return FALSE;
	}

	static PTF_CREATETHREADMGR s_pfnCreateThreadMgr;

	if(0 == s_pfnCreateThreadMgr)
	{
		s_pfnCreateThreadMgr = (PTF_CREATETHREADMGR)::GetProcAddress (hMSCTF, "TF_CreateThreadMgr");

		if(0 == s_pfnCreateThreadMgr)
		{
			return FALSE;
		}
	}

	hResult = s_pfnCreateThreadMgr(ppITfThreadMgr);

	return (SUCCEEDED (hResult)) ;
}

BOOL CUiWnd::GetLanguageBarItemMgr(ITfLangBarItemMgr **ppLangBarMgr,GUID *pGuidTIP)
{
	ITfLangBarItemMgr *pLangBarItemMgr=NULL;
	OSVERSIONINFO ver={sizeof(ver),0};
	GetVersionEx(&ver);
	if(ver.dwMajorVersion==5 && ver.dwMinorVersion==1)//XP
	{//XP下判断是否打开高级文字服务
		HKEY hKey=0;
		if(ERROR_SUCCESS==RegOpenKeyEx(HKEY_CURRENT_USER,_T("Software\\Microsoft\\CTF"),0,KEY_READ,&hKey) && hKey)
		{
			DWORD dwEnable=0;
			DWORD dwSize=sizeof(DWORD);
			RegQueryValueEx(hKey,_T("Disable Thread Input Manager"),NULL,NULL,(LPBYTE)&dwEnable,&dwSize);
			RegCloseKey(hKey);
			if(dwEnable==1) return FALSE;
		}
	}
	if(!_QueryLangBarItemMgr(ppLangBarMgr))
		return FALSE;

	*pGuidTIP=c_clsidSinstar3TSF;
	return TRUE;
}

#endif//ENABLE_LANGUAGEBAR