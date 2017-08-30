#include "StdAfx.h"
#include "Sinstar3Impl.h"

#include <initguid.h>


CSinstar3Impl::CSinstar3Impl(ITextService *pTxtSvr)
:m_pTxtSvr(pTxtSvr)
,m_pCompWnd(NULL)
,m_pStatusWnd(NULL)
{
	theModule->AddRef();
 	m_pCompWnd = new CInputWnd();
	m_pStatusWnd = new CStatusWnd();
	m_pStatusWnd->Create();
	m_pCompWnd->Create();
}

CSinstar3Impl::~CSinstar3Impl(void)
{
	m_pCompWnd->DestroyWindow();
	m_pStatusWnd->DestroyWindow();
	delete m_pStatusWnd;
	delete m_pCompWnd;
	theModule->Release();
}


void CSinstar3Impl:: ProcessKeyStoke(LPVOID lpImeContext,UINT vkCode,LPARAM lParam,BOOL bKeyDown,BOOL *pbEaten)
{
	*pbEaten = bKeyDown;
	return;
}

void CSinstar3Impl:: TranslateKey(LPVOID lpImeContext,UINT vkCode,UINT uScanCode,BOOL bKeyDown,BOOL *pbEaten)
{
	*pbEaten = TRUE;

	if(isprint(vkCode))
	{
		SStringT strComp = m_pCompWnd->GetCompStr();
		if(strComp.IsEmpty())
		{
			m_pTxtSvr->StartComposition(lpImeContext);
		}
		strComp.Append(vkCode);
		m_pCompWnd->SetCompStr(strComp);
		m_pTxtSvr->ReplaceSelCompositionW(lpImeContext,0,-1,strComp,strComp.GetLength());
	}else if(vkCode == VK_ESCAPE || vkCode == VK_RETURN)
	{
		m_pTxtSvr->UpdateResultAndCompositionStringW(lpImeContext,L"Æô³ÌÊäÈë·¨3",6,NULL,0);
		m_pTxtSvr->EndComposition(lpImeContext);
		m_pCompWnd->SetCompStr(_T(""));
	}
}

void CSinstar3Impl::OnIMESelect(BOOL bSelect)
{
}

void CSinstar3Impl::OnSetCaretPosition(POINT pt,int nHei)
{
	m_pCompWnd->MoveTo(pt,nHei);
}

void CSinstar3Impl::OnSetFocusSegmentPosition(POINT pt,int nHei)
{
}

void CSinstar3Impl::OnCompositionStarted()
{
	m_pCompWnd->Show(TRUE);
}

void CSinstar3Impl::OnCompositionChanged()
{
}

void CSinstar3Impl::OnCompositionTerminated()
{
 	m_pCompWnd->Show(FALSE);
}

void CSinstar3Impl::OnSetFocus(BOOL bFocus)
{
	SLOG_INFO("GetThreadID="<<GetCurrentThreadId()<<" focus="<<bFocus);
	m_pStatusWnd->Show(bFocus);
	m_pCompWnd->Show(bFocus);
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
	return FullNative;
}

BOOL CSinstar3Impl::ShowCompWnd()
{
	return FALSE;
}

LRESULT CSinstar3Impl::OnWildMessage(WPARAM wParam,LPARAM lParam)
{
	return E_NOTIMPL;
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

