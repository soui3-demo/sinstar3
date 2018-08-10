#include "stdafx.h"
#include "CompStrEx.h"
#include "uiwnd.h"

BOOL WINAPI ImeInquire(LPIMEINFO lpIMEInfo,LPTSTR lpszUIClass,DWORD dwSystemInfoFlags)
{
	if(!lpIMEInfo) return FALSE;

	lpIMEInfo->dwPrivateDataSize = sizeof(LPVOID); 
	theModule->SetSysInfoFlags(dwSystemInfoFlags);

	lpIMEInfo->fdwProperty = 
#ifdef _UNICODE
		IME_PROP_UNICODE |
#endif
//		IME_PROP_SPECIAL_UI|
		IME_PROP_NEED_ALTKEY |
//		IME_PROP_AT_CARET|
		IME_PROP_END_UNLOAD;

	lpIMEInfo->fdwConversionCaps = IME_CMODE_FULLSHAPE | IME_CMODE_NATIVE;

	lpIMEInfo->fdwSentenceCaps = IME_SMODE_NONE;
	lpIMEInfo->fdwUICaps = UI_CAP_2700;

	lpIMEInfo->fdwSCSCaps = 0 ;

	lpIMEInfo->fdwSelectCaps = SELECT_CAP_CONVERSION;

	_tcscpy(lpszUIClass,UICLASSNAME);

	return TRUE;
}

BOOL WINAPI ImeConfigure(HKL hKL,HWND hWnd, DWORD dwMode, LPVOID lpData)
{	
	switch (dwMode) {
	case IME_CONFIG_GENERAL:
		return CCoreLoader::GetInstance().Sinstar3_OpenConfig(hWnd);
	default:
		return (FALSE);
	}
}


//设置默认的组词栏位置信息
void Context_Init(LPINPUTCONTEXT lpIMC)
{
	// Init the general member of IMC.
	if (!(lpIMC->fdwInit & INIT_LOGFONT))
	{
		lpIMC->lfFont.A.lfCharSet = DEFAULT_CHARSET;
		lpIMC->fdwInit |= INIT_LOGFONT;
	}

	if (!(lpIMC->fdwInit & INIT_CONVERSION))
	{
		lpIMC->fdwConversion = IME_CMODE_ROMAN | IME_CMODE_FULLSHAPE | IME_CMODE_NATIVE;
		lpIMC->fdwInit |= INIT_CONVERSION;
	}

	if(!(lpIMC->fdwInit&INIT_COMPFORM) && lpIMC->hWnd)
	{
		lpIMC->cfCompForm.dwStyle=CFS_DEFAULT;
		GetCaretPos(&lpIMC->cfCompForm.ptCurrentPos);
		ScreenToClient(lpIMC->hWnd,&lpIMC->cfCompForm.ptCurrentPos);
		lpIMC->fdwInit|=INIT_COMPFORM;
	}
	CCompStrEx *pCompStr=(CCompStrEx*)ImmLockIMCC(lpIMC->hCompStr);
	if(pCompStr)
	{
		pCompStr->Init();
		ImmUnlockIMCC(lpIMC->hCompStr);
	}
	CUiWnd **ppUiWnd=(CUiWnd**)ImmLockIMCC(lpIMC->hPrivate);
	if(ppUiWnd)
	{
		*ppUiWnd=NULL;
		ImmUnlockIMCC(lpIMC->hPrivate);
	}
}

BOOL InitIMCC(HIMCC & hIMCC,DWORD dwSize)
{
	if(!hIMCC)
		hIMCC=ImmCreateIMCC(dwSize);
	else if(ImmGetIMCCSize(hIMCC) != dwSize)
		hIMCC = ImmReSizeIMCC(hIMCC,dwSize);
	return hIMCC!=NULL;
}

BOOL WINAPI ImeSelect(HIMC hIMC, BOOL fSelect)
{
	SLOGFMTI("\nImeSelect,hIMC=%p,fSelect=%d",hIMC,fSelect);
	LPINPUTCONTEXT lpIMC = ImmLockIMC(hIMC);
	if (!lpIMC) return FALSE;

	if(fSelect)
	{
		// Init the general member of IMC.
		if(!InitIMCC(lpIMC->hCompStr,sizeof(CCompStrEx)))
		{
			ImmUnlockIMC(hIMC);
			return FALSE;
		}
		if(!InitIMCC(lpIMC->hPrivate,sizeof(LPVOID)))
		{
			ImmUnlockIMC(hIMC);
			return FALSE;
		}
		Context_Init(lpIMC);
	}else
	{
		if (lpIMC->hCompStr)
		{
			ImmDestroyIMCC(lpIMC->hCompStr);
			lpIMC->hCompStr = NULL;
		}
		if (lpIMC->hPrivate)
		{
			ImmDestroyIMCC(lpIMC->hPrivate);
			lpIMC->hPrivate = NULL;
		}
	}
	lpIMC->fOpen = fSelect;
	ImmUnlockIMC(hIMC);
	return TRUE;
}

BOOL WINAPI ImeSetActiveContext(HIMC hIMC,BOOL fFlag)
{
	SLOGFMTI("ImeSetActiveContext,hIMC=%p,fFlag=%d!!!!!!!!!!!!!!!!!!!!",hIMC,fFlag);
 	if(hIMC)
 	{
  		LPINPUTCONTEXT lpIMC=ImmLockIMC(hIMC);
  		if(!lpIMC) return FALSE;
  		
 		CUiWnd **ppUiWnd=(CUiWnd**)ImmLockIMCC(lpIMC->hPrivate);
 		if(ppUiWnd && ppUiWnd[0] && !IsBadWritePtr(ppUiWnd[0],sizeof(CUiWnd*)))
 		{
 			if(ppUiWnd[0]->m_pSinstar3) ppUiWnd[0]->m_pSinstar3->OnSetFocus(fFlag);
 		}
 		ImmUnlockIMCC(lpIMC->hPrivate); 
 		ImmUnlockIMC(hIMC);
 	}

	return TRUE;
}

// lpbKeyState: see GetKeyboardState
BOOL WINAPI ImeProcessKey(HIMC hIMC,UINT uVKey,LPARAM lKeyData,CONST LPBYTE lpbKeyState)
{
	CImeContext ctx;
	if(!ctx.Init(hIMC)) return FALSE;

	BOOL bRet=FALSE;
	CUiWnd **ppUiWnd=(CUiWnd **)ImmLockIMCC(ctx._lpContext->hPrivate);
	if(!ppUiWnd || !(*ppUiWnd))
	{
		ImmUnlockIMCC(ctx._lpContext->hPrivate);
		goto exit;
	}
	(*ppUiWnd)->m_pCurContext=&ctx;

	if((*ppUiWnd)->m_pSinstar3)
	{
		BOOL bKeyDown=(lKeyData&0x80000000)==0;
		if(bKeyDown && (*ppUiWnd)->ProcessHotKey(uVKey,lpbKeyState))
		{
			bRet=TRUE;
		}else
		{
			(*ppUiWnd)->m_pSinstar3->ProcessKeyStoke(&ctx,uVKey,lKeyData,bKeyDown,&bRet);
		}
	}
	(*ppUiWnd)->m_pCurContext=NULL;
	ImmUnlockIMCC(ctx._lpContext->hPrivate);

exit:
	ImmUnlockIMC(hIMC);
	return bRet;
}


UINT WINAPI ImeToAsciiEx (UINT uVKey,UINT uScanCode,CONST LPBYTE lpbKeyState,LPDWORD lpdwTransKey,UINT fuState,HIMC hIMC)
{
	CImeContext ctx;
	if(!ctx.Init(hIMC,lpdwTransKey)) return 0;

	CUiWnd **ppUiWnd=(CUiWnd **)ImmLockIMCC(ctx._lpContext->hPrivate);
	if(!ppUiWnd)	goto exit;
	
	(*ppUiWnd)->m_pCurContext=&ctx;
	BOOL bKeyDown=TRUE;
	if((*ppUiWnd)->m_pSinstar3)
	{
		GUID guid;
		if(bKeyDown && (*ppUiWnd)->ProcessHotKey(uVKey,lpbKeyState,&guid))
		{
			(*ppUiWnd)->m_pSinstar3->OnHotkey(&ctx,guid);
		}else
		{
			BOOL bEaten=FALSE;
			(*ppUiWnd)->m_pSinstar3->TranslateKey(&ctx,uVKey,uScanCode,bKeyDown,&bEaten);
			if(!bEaten) ctx._nMsgInBuf=0;
		}
	}
	(*ppUiWnd)->m_pCurContext=NULL;
exit:
	ImmUnlockIMCC(ctx._lpContext->hPrivate);
	return ctx._nMsgInBuf;
}

BOOL IsCompStr(HIMC hIMC)
{
	LPINPUTCONTEXT lpIMC;
	LPCOMPOSITIONSTRING lpCompStr;
	BOOL fRet = FALSE;

	if (!(lpIMC = ImmLockIMC(hIMC)))
		return FALSE;

	if (ImmGetIMCCSize(lpIMC->hCompStr) < sizeof (COMPOSITIONSTRING))
	{
		ImmUnlockIMC(hIMC);
		return FALSE;
	}

	lpCompStr = (LPCOMPOSITIONSTRING)ImmLockIMCC(lpIMC->hCompStr);

	fRet = (lpCompStr->dwCompStrLen > 0);

	ImmUnlockIMCC(lpIMC->hCompStr);
	ImmUnlockIMC(hIMC);

	return fRet;
}

BOOL WINAPI NotifyIME(HIMC hIMC,DWORD dwAction,DWORD dwIndex,DWORD dwValue)
{
	return FALSE;
}
