#include "StdAfx.h"
#include "Sinstar3Impl.h"

#include <initguid.h>


CSinstar3Impl::CSinstar3Impl(ITextService *pTxtSvr,HINSTANCE hInst)
:m_pTxtSvr(pTxtSvr)
,m_hInst(hInst)
{
	theCore->AddRef();
	m_pImeWnd = new CImeWnd();
}

CSinstar3Impl::~CSinstar3Impl(void)
{
	m_pImeWnd->DestroyWindow();
	delete m_pImeWnd;
	theCore->Release();
}


void CSinstar3Impl:: ProcessKeyStoke(LPVOID lpImeContext,UINT vkCode,LPARAM lParam,BOOL bKeyDown,BOOL *pbEaten)
{
}

void CSinstar3Impl:: TranslateKey(LPVOID lpImeContext,UINT vkCode,UINT uScanCode,BOOL bKeyDown,BOOL *pbEaten)
{

}

void CSinstar3Impl::OnIMESelect(BOOL bSelect)
{

}

void CSinstar3Impl::OnSetCaretPosition(POINT pt,int nHei)
{
}

void CSinstar3Impl::OnSetFocusSegmentPosition(POINT pt,int nHei)
{
}

void CSinstar3Impl::OnCompositionStarted()
{
	m_pImeWnd->Create(NULL);
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

