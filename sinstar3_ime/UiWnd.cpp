#include "StdAfx.h"
#include "UiWnd.h"
#include "CompStrEx.h"

#define HEI_LINEMARGIN	3
#define TIMERID_CHKDEFIME	200


LRESULT CALLBACK UIWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	CUiWnd *pUiWnd=(CUiWnd *)GetWindowLongPtr(hWnd,GWLP_USERDATA);
	switch(uMsg)
	{
	case WM_NCCREATE:
		_ASSERT(pUiWnd==NULL);
		pUiWnd=new CUiWnd;
		pUiWnd->Attach(hWnd);
		break;
	case WM_NCDESTROY:
		pUiWnd->WindowProc(uMsg,wParam,lParam);
		pUiWnd->Detatch();
		delete pUiWnd;
		return 0;
	}
	if(pUiWnd)	return pUiWnd->WindowProc(uMsg,wParam,lParam);
	else return DefWindowProc(hWnd,uMsg,wParam,lParam);
}


BOOL CUiWnd::RegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wc;

	// register class of UI window.
	wc.cbSize         = sizeof(WNDCLASSEX);
	wc.style          = CS_IME;
	wc.lpfnWndProc    = UIWndProc;
	wc.cbClsExtra     = 0;
	wc.cbWndExtra     = 2 * sizeof(LONG_PTR);
	wc.hInstance      = (HINSTANCE)hInstance;
	wc.hCursor        = LoadCursor( NULL, IDC_ARROW );
	wc.hIcon          = NULL;
	wc.lpszMenuName   = (LPTSTR)NULL;
	wc.lpszClassName  = UICLASSNAME;
	wc.hbrBackground  = NULL;
	wc.hIconSm        = NULL;

	if( !RegisterClassEx( (LPWNDCLASSEX)&wc ) )
		return FALSE;
	return CSimpleWnd::RegisterWndClass(hInstance);
}

void CUiWnd::UnregisterClass(HINSTANCE hInstance)
{
	::UnregisterClass(UICLASSNAME,hInstance);
	CSimpleWnd::UnregisterWndClass(hInstance);

}

CUiWnd::CUiWnd(void)
{
	m_pSinstar3=NULL;
	m_pCurContext=NULL;
	m_nFontHei=20;
	m_fntComp=0;
	m_bActivate=TRUE;
}

CUiWnd::~CUiWnd(void)
{
	if(m_fntComp) DeleteObject(m_fntComp);
}

BOOL CUiWnd::IsIMEMessage(UINT message)
{
	switch(message)
	{
	case WM_IME_STARTCOMPOSITION:
	case WM_IME_ENDCOMPOSITION:
	case WM_IME_COMPOSITION:
	case WM_IME_NOTIFY:
	case WM_IME_SETCONTEXT:
	case WM_IME_CONTROL:
	case WM_IME_COMPOSITIONFULL:
	case WM_IME_SELECT:
	case WM_IME_CHAR:
		return TRUE;
	default:
		return FALSE;
	}
}

LRESULT CUiWnd::OnPaint()
{
	PAINTSTRUCT ps;
	MSG sMsg;
	// for safety
	BeginPaint(m_hWnd, &ps);
	EndPaint(m_hWnd, &ps);

	// some application will not remove the WM_PAINT messages
	PeekMessage(&sMsg, m_hWnd, WM_PAINT, WM_PAINT, PM_REMOVE|PM_NOYIELD);
	return (0L);
}

LRESULT CUiWnd::WindowProc(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	HIMC hIMC=(HIMC)GetWindowLongPtr(m_hWnd,IMMGWLP_IMC);
	if(!hIMC && IsIMEMessage(uMsg)) return 0;
//	Helper_Trace(_T("UIWndProc, uMsg:%08X(%d),wParam:%08X(%d),lParam:%08X(%d)"),uMsg,uMsg,wParam,wParam,lParam,lParam);
	switch(uMsg)
	{
	case WM_MOUSEACTIVATE:
		return MA_NOACTIVATE;
	case WM_CREATE:
		return OnCreate();
	case WM_DESTROY:
		return OnDestroy();
	case WM_NCDESTROY:
		AttachToIMC(FALSE);
		return 0;
	case WM_PAINT:
		return OnPaint();
	case WM_TIMER:
		return OnTimer(wParam);
	case WM_IME_NOTIFY:
		return OnImeNotify(wParam,lParam);
	case WM_IME_CONTROL:
		return OnImeControl(wParam,lParam);
	case WM_IME_SELECT:
		return OnImeSelect(wParam,lParam);
	case WM_IME_SETCONTEXT:
		return OnSetContext(wParam,lParam);
	case WM_IME_STARTCOMPOSITION:
		if(m_pSinstar3 && m_pSinstar3->ShowCompWnd())
		{
			m_wndComp.m_bValid=TRUE;
			m_wndComp.ShowWindow(SW_SHOWNOACTIVATE);
		}
		return 0;
	case WM_IME_COMPOSITION:
		if(m_wndComp.m_bValid)
		{
			m_wndComp.SendMessage(UM_UPDATECONTEXT,(WPARAM)hIMC,0);
			OnImeNotify(IMN_SETCOMPOSITIONWINDOW,0);
		}
		return 0;
	case WM_IME_ENDCOMPOSITION:
		m_wndComp.ShowWindow(SW_HIDE);
		m_wndComp.SendMessage(UM_UPDATECONTEXT,(WPARAM)hIMC,0);//清空数据
		m_wndComp.m_bValid=FALSE;
		return 0;
	}

	return __super::WindowProc(uMsg,wParam,lParam);
}

LRESULT CUiWnd::OnTimer(WPARAM nEventID)
{
	return 0;
}

LRESULT CUiWnd::OnImeSelect(BOOL bSelect,LPARAM lParam)
{
	Helper_Trace(_T("OnImeSelect,bSelect:%d,lParam:%08x(%d)"),bSelect,lParam,lParam);
	AttachToIMC(bSelect);
	OnImeNotify(IMN_SETOPENSTATUS,0);
	OnImeNotify(IMN_SETCONVERSIONMODE,0);
	if(m_pSinstar3) m_pSinstar3->OnIMESelect(bSelect);
	return 0;
}

POINT CUiWnd::GetAbsPos(HWND hWnd,DWORD dwStyle,POINT ptCur,RECT rc)
{
	POINT pt;
	if(dwStyle==CFS_DEFAULT)
		GetCaretPos(&pt);
	else if(dwStyle==CFS_EXCLUDE)
		pt.x=ptCur.x,
		pt.y=rc.bottom-m_nFontHei+HEI_LINEMARGIN;
	else
		pt=ptCur;
	ClientToScreen(hWnd,&pt);
	return pt;
}

LRESULT CUiWnd::OnImeControl(WPARAM wParam,LPARAM lParam)
{
	Helper_Trace(_T("OnImeControl, wParam:%08X(%d),lParam:%08X(%d)"),wParam,wParam,lParam,lParam);
	LPINPUTCONTEXT lpIMC;
	HIMC hIMC=(HIMC)GetWindowLongPtr(m_hWnd,IMMGWLP_IMC);
	_ASSERT(hIMC);
	if (!(lpIMC = ImmLockIMC(hIMC)))
		return 1L;
	LRESULT lRet=1;
	switch (wParam)
	{
	case IMC_GETCANDIDATEPOS:
		*(LPCANDIDATEFORM)lParam  = lpIMC->cfCandForm[0]; 
		lRet = 0;
		break;

	case IMC_GETCOMPOSITIONWINDOW:
		*(LPCOMPOSITIONFORM)lParam  = lpIMC->cfCompForm; 
		lRet = 0;
		break;

	default:
		break;
	}
	ImmUnlockIMC(hIMC);
	return lRet;
}

LRESULT CUiWnd::OnImeNotify(WPARAM wParam,LPARAM lParam)
{
//	Helper_Trace(_T("OnImeNotify, wParam:%08X(%d),lParam:%08X(%d)"),wParam,wParam,lParam,lParam);
	HIMC hIMC=(HIMC)GetWindowLongPtr(m_hWnd,IMMGWLP_IMC);
	switch(wParam)
	{
	case IMN_OPENSTATUSWINDOW:
		if(m_pSinstar3) m_pSinstar3->OnSetFocus(TRUE);
		//if(m_wndComp.m_bValid) m_wndComp.ShowWindow(SW_SHOWNOACTIVATE);
		m_wndComp.ShowWindow(m_wndComp.m_bValid ? SW_SHOWNOACTIVATE : SW_HIDE);
		Helper_Trace(_T("IMN_OPENSTATUSWINDOW"));
		break;
	case IMN_CLOSESTATUSWINDOW:
		if(m_pSinstar3) m_pSinstar3->OnSetFocus(FALSE);
		if(m_wndComp.m_bValid) m_wndComp.ShowWindow(SW_HIDE);
		Helper_Trace(_T("IMN_CLOSESTATUSWINDOW"));
		break;
	case IMN_OPENCANDIDATE:
		OnImeNotify(IMN_SETCOMPOSITIONFONT,0);		break;
	case IMN_CHANGECANDIDATE:
		break;
	case IMN_CLOSECANDIDATE:
		break;
	case IMN_SETOPENSTATUS:
		if(m_pSinstar3) m_pSinstar3->OnOpenStatusChanged(ImmGetOpenStatus(hIMC));
		break;
	case IMN_SETCONVERSIONMODE:
		if(m_pSinstar3)
		{
			EInputMethod mode=GetConversionMode();
			if(ImmGetOpenStatus(hIMC)==FALSE) mode=HalfAlphanumeric;
			m_pSinstar3->OnConversionModeChanged(mode);
		}
		break;
	case IMN_SETCOMPOSITIONWINDOW:
		if(m_pSinstar3 && hIMC && m_bActivate)
		{
			LPINPUTCONTEXT lpIMC=(LPINPUTCONTEXT)ImmLockIMC(hIMC);
			if(lpIMC)
			{
				CCompStrEx *pCompStr=(CCompStrEx*)ImmLockIMCC(lpIMC->hCompStr);
				if(pCompStr)
				{
					POINT pt=GetAbsPos(lpIMC->hWnd,lpIMC->cfCompForm.dwStyle,lpIMC->cfCompForm.ptCurrentPos,lpIMC->cfCompForm.rcArea);
					if(m_wndComp.m_bValid)
					{
						ScreenToClient(lpIMC->hWnd,&pt);
						m_wndComp.MoveCompWindow(hIMC,pt);
						RECT rc;
						m_wndComp.GetLastLineRect(&rc);
						pt.x=rc.left,pt.y=rc.top;
					}else
					{		
						IMECHARPOSITION charPos={sizeof(charPos),pCompStr->GetTargetPos(),0};
						COMPOSITIONFORM compForm={0};
						if(charPos.dwCharPos==-1) charPos.dwCharPos=0;
						if(ImmRequestMessage(hIMC,IMR_COMPOSITIONWINDOW,(LPARAM)&compForm))
						{
							pt=GetAbsPos(lpIMC->hWnd,compForm.dwStyle,compForm.ptCurrentPos,compForm.rcArea);
						}else if(ImmRequestMessage(hIMC,IMR_QUERYCHARPOSITION,(LPARAM)&charPos))
						{
							pt=charPos.pt;
						} 
					}
					pt.y-=HEI_LINEMARGIN;
					m_pSinstar3->OnSetCaretPosition(pt,m_nFontHei);
					Helper_Trace(_T("IMN_SETCOMPOSITIONWINDOW,pt=(%d,%d)"),pt.x,pt.y);
					ImmUnlockIMCC(lpIMC->hCompStr);
				}
				ImmUnlockIMC(hIMC);
			}
		}
		break;
	case IMN_SETCANDIDATEPOS:
		if(m_pSinstar3 && hIMC)
		{
			LPINPUTCONTEXT lpIMC=(LPINPUTCONTEXT)ImmLockIMC(hIMC);
			if(lpIMC)
			{
				CCompStrEx *pCompStr=(CCompStrEx*)ImmLockIMCC(lpIMC->hCompStr);
				if(pCompStr)
				{
					if(pCompStr->GetTargetPos()!=-1)
					{
						POINT pt;
						if(m_wndComp.m_bValid)
						{
							m_wndComp.SendMessage(UM_GETCANDPOS,(WPARAM)lpIMC->hWnd,(LPARAM)&pt);
							ClientToScreen(lpIMC->hWnd,&pt);
						}else
						{

							IMECHARPOSITION charPos={sizeof(charPos),0};
							charPos.dwCharPos=pCompStr->GetTargetPos();
							if(charPos.dwCharPos!=DWORD(-1) && ImmRequestMessage(hIMC,IMR_QUERYCHARPOSITION,(LPARAM)&charPos))
							{
								pt=charPos.pt;
							}else
							{
								pt=GetAbsPos(lpIMC->hWnd,lpIMC->cfCandForm[0].dwStyle,lpIMC->cfCandForm[0].ptCurrentPos,lpIMC->cfCandForm[0].rcArea);
							}
						}
						pt.y-=HEI_LINEMARGIN;
						Helper_Trace(_T("IMN_SETCANDIDATEPOS,pt=(%d,%d)"),pt.x,pt.y);
						m_pSinstar3->OnSetFocusSegmentPosition(pt,m_nFontHei);
					}
					ImmUnlockIMCC(lpIMC->hCompStr);
				}
				ImmUnlockIMC(hIMC);
			}
		}
		break;
	case IMN_SETCOMPOSITIONFONT:
		if(hIMC){
			LPINPUTCONTEXT lpIMC=(LPINPUTCONTEXT)ImmLockIMC(hIMC);
			if(lpIMC)
			{
				if(m_fntComp) DeleteObject(m_fntComp);
				m_fntComp=0;

				
				LOGFONT lf;
				
				if(!ImmRequestMessage(hIMC,IMR_COMPOSITIONFONT,(LPARAM)&lf))
				{
					memcpy(&lf,&lpIMC->lfFont.W,sizeof(LOGFONT));
				}
				m_fntComp=CreateFontIndirectW(&lf);
				m_wndComp.SendMessage(WM_SETFONT,(WPARAM)m_fntComp,0);
				
				IMECHARPOSITION charPos={sizeof(charPos),0,0};
				if(ImmRequestMessage(hIMC,IMR_QUERYCHARPOSITION,(LPARAM)&charPos))
				{
					m_nFontHei=charPos.cLineHeight+HEI_LINEMARGIN*2;
				}
				else
				{
					SIZE sz;
					HDC hdc=GetDC(lpIMC->hWnd);
					HGDIOBJ hOldFont=SelectObject(hdc,m_fntComp);
					GetTextExtentPoint(hdc,_T("A"),1,&sz);
					SelectObject(hdc,hOldFont);
					ReleaseDC(lpIMC->hWnd,hdc);
					m_nFontHei=sz.cy+HEI_LINEMARGIN*2;
				}

				OnImeNotify(IMN_SETCOMPOSITIONWINDOW,0);
			}
			ImmUnlockIMC(hIMC);
		}
		break;
	}
	return 0;
}

LRESULT CUiWnd::OnSetContext(BOOL bActivate,LPARAM lParam)
{
	Helper_Trace(_T("OnSetContext,bActivate:%d"),bActivate);
	m_bActivate=bActivate;
	if(bActivate)
	{
		AttachToIMC(TRUE);
		OnImeNotify(IMN_OPENSTATUSWINDOW,0);
	}else
	{
		OnImeNotify(IMN_CLOSESTATUSWINDOW,0);
	}
	return 0;
}

CLogStateListener g_LogListener;

BOOL CUiWnd::_InitSinstar3()
{
	m_pSinstar3=CCoreLoader::GetInstance().Sinstar3_Create(this,g_hInst);
	CCoreLoader::GetInstance().Sinstar3_SetLogStateListener(&g_LogListener);

	if(!m_pSinstar3) return FALSE;
	m_pSinstar3->OnIMESelect(m_bActivate);
	HIMC hIMC=(HIMC)GetWindowLongPtr(m_hWnd,IMMGWLP_IMC);
	m_pSinstar3->OnSetFocus(hIMC!=0);
	return TRUE;
}

BOOL CUiWnd::_UninitSinstar3()
{
	if(m_pSinstar3)
	{
		m_pSinstar3->OnIMESelect(FALSE);
		CCoreLoader::GetInstance().Sinstar3_Delete(m_pSinstar3);
		m_pSinstar3=NULL;
	}
	return TRUE;
}

LRESULT CUiWnd::OnCreate()
{
	if(g_dwSystemInfoFlags & IME_SYSINFO_WINLOGON) return -1;

	Helper_Trace(_T("CUiWnd::OnCreate,hWnd:%08x"),m_hWnd);
	_InitSinstar3();
	m_wndComp.Create(WS_EX_TOOLWINDOW|WS_EX_LAYERED|WS_EX_NOACTIVATE,WS_POPUP|WS_DISABLED,m_hWnd,0,g_hInst);
	PostMessage(WM_IME_NOTIFY,IMN_SETCONVERSIONMODE,0);
	return 0;
}

LRESULT CUiWnd::OnDestroy()
{
	_UninitSinstar3();
	m_wndComp.Destroy();
	Helper_Trace(_T("CUiWnd::OnDestroy"));
	return 0;
}

BOOL CUiWnd::AttachToIMC(BOOL bAttach)
{
	BOOL bRet=FALSE;
	HIMC hIMC=(HIMC)GetWindowLongPtr(m_hWnd,IMMGWLP_IMC);
	LPINPUTCONTEXT lpIMC=(LPINPUTCONTEXT)ImmLockIMC(hIMC);
	if(lpIMC)
	{
		//将UIWnd对象赋与Private分配的内存，方便在IMM的接口中访问当前的UIWND。
		CUiWnd **pData=(CUiWnd**)ImmLockIMCC(lpIMC->hPrivate);
		if(pData)
		{
			if(bAttach)
				*pData=this;
			else
				*pData=0;
			ImmUnlockIMCC(lpIMC->hPrivate);
			bRet=TRUE;
		}
		ImmUnlockIMC(hIMC);
	}
	return bRet;
}

BOOL CUiWnd::IsDefaultIme(void)
{
	HKL hKlCur=GetKeyboardLayout(0);
	HKL hDefIME=0;
	SystemParametersInfo(SPI_GETDEFAULTINPUTLANG, 0, &hDefIME, 0);
	return hKlCur==hDefIME;
}


