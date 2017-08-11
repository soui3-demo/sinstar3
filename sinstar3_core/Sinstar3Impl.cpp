#include "StdAfx.h"
#include "Sinstar3Impl.h"

#include <initguid.h>


CSinstar3Impl::CSinstar3Impl(ITextService *pTxtSvr,HINSTANCE hInst)
:m_pTxtSvr(pTxtSvr)
,m_hInst(hInst)
,m_pImeWnd(NULL)
,m_nCaretHeight(30)
{
	theCore->AddRef();
 	m_pImeWnd = new CImeWnd();
}

CSinstar3Impl::~CSinstar3Impl(void)
{
	if(m_pImeWnd->IsWindow())
		m_pImeWnd->DestroyWindow();
	delete m_pImeWnd;
	theCore->Release();
}


void CSinstar3Impl:: ProcessKeyStoke(LPVOID lpImeContext,UINT vkCode,LPARAM lParam,BOOL bKeyDown,BOOL *pbEaten)
{
	*pbEaten = TRUE;
	if(!bKeyDown)
	{
		return;
	}
	if(isprint(vkCode))
	{
		SStringT strComp = m_pImeWnd->GetCompStr();
		if(strComp.IsEmpty())
			m_pTxtSvr->StartComposition(lpImeContext);
		strComp.Append(vkCode);
		m_pImeWnd->SetCompStr(strComp);
	}else if(vkCode == VK_ESCAPE || vkCode == VK_RETURN)
	{
		m_pTxtSvr->UpdateResultAndCompositionStringW(lpImeContext,L"Æô³ÌÊäÈë·¨3",6,NULL,0);
		m_pTxtSvr->EndComposition(lpImeContext);
		m_pImeWnd->SetCompStr(_T(""));
	}
}

void CSinstar3Impl:: TranslateKey(LPVOID lpImeContext,UINT vkCode,UINT uScanCode,BOOL bKeyDown,BOOL *pbEaten)
{

}

void CSinstar3Impl::OnIMESelect(BOOL bSelect)
{

}

void CSinstar3Impl::OnSetCaretPosition(POINT pt,int nHei)
{
	m_ptCaret = pt;
	m_nCaretHeight = nHei;
	if(m_pImeWnd->IsWindow())
	{
		m_pImeWnd->SetWindowPos(0,m_ptCaret.x,m_ptCaret.y+m_nCaretHeight+5,0,0,SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);
	}
}

void CSinstar3Impl::OnSetFocusSegmentPosition(POINT pt,int nHei)
{
}

void CSinstar3Impl::OnCompositionStarted()
{
	m_pImeWnd->Create(NULL,WS_POPUP|WS_DISABLED,WS_EX_TOOLWINDOW,0,0,0,0);
	m_pImeWnd->SetWindowPos(HWND_TOPMOST,m_ptCaret.x,m_ptCaret.y+m_nCaretHeight+5,0,0,SWP_NOSIZE|SWP_SHOWWINDOW|SWP_NOACTIVATE);
}

void CSinstar3Impl::OnCompositionChanged()
{
}

void CSinstar3Impl::OnCompositionTerminated()
{
 	m_pImeWnd->DestroyWindow();
}

void CSinstar3Impl::OnSetFocus(BOOL bFocus)
{
}

int  CSinstar3Impl::GetCompositionSegments()
{
	return 0;
}

int  CSinstar3Impl::GetCompositionSegmentEnd(int iSeg)
{
	return 0;
}

int CSinstar3Impl::GetCompositionSegmentAttr(int iSeg)
{
	return 0;
}

BOOL CSinstar3Impl::OnHotkey(LPVOID lpImeContext,REFGUID guidHotKey)
{

	return FALSE;
}

void CSinstar3Impl::OnOpenStatusChanged(BOOL bOpen)
{

}

void CSinstar3Impl::OnConversionModeChanged(EInputMethod nMode)
{

}

void CSinstar3Impl::ShowHelp()
{
}

EInputMethod CSinstar3Impl::GetDefInputMode()
{
	return FullAlphanumeric;
}

BOOL CSinstar3Impl::ShowCompWnd()
{
	return FALSE;
}

LRESULT CSinstar3Impl::OnWildMessage(WPARAM wParam,LPARAM lParam)
{
	return E_NOTIMPL;
}

HMODULE CSinstar3Impl::GetModule()
{
	return m_hInst;
}

void CSinstar3Impl::CheckDefIME()
{

}

INT_PTR CSinstar3Impl::MessageBox(HWND hWnd,LPCTSTR lpText,LPCTSTR lpCaption,UINT uType)
{
	return 0;
}

void CSinstar3Impl::OnFinalRelease()
{

}

HRESULT CSinstar3Impl::OnQueryInterface(REFIID riid, void **ppvObject)
{
	return E_NOINTERFACE;
}

