#include "StdAfx.h"
#include "Sinstar3Impl.h"


#include <initguid.h>


CBaiduJP3Imp::CBaiduJP3Imp(CTextService *pTxtSvr,HINSTANCE hInst)
{

}

CBaiduJP3Imp::~CBaiduJP3Imp(void)
{
}


void CBaiduJP3Imp::Init(CTextService * pTxtSvc)
{

}


void CBaiduJP3Imp:: ProcessKeyStoke(LPVOID lpImeContext,UINT vkCode,LPARAM lParam,BOOL bKeyDown,BOOL *pbEaten)
{
}

void CBaiduJP3Imp:: TranslateKey(LPVOID lpImeContext,UINT vkCode,UINT uScanCode,BOOL bKeyDown,BOOL *pbEaten)
{

}

void CBaiduJP3Imp::OnIMESelect(BOOL bSelect)
{

}

void CBaiduJP3Imp::OnSetCaretPosition(POINT pt,int nHei)
{
}

void CBaiduJP3Imp::OnSetFocusSegmentPosition(POINT pt,int nHei)
{
}

void CBaiduJP3Imp::OnCompositionStarted()
{
}

void CBaiduJP3Imp::OnCompositionChanged()
{
}

void CBaiduJP3Imp::OnCompositionTerminated()
{

}

void CBaiduJP3Imp::OnSetFocus(BOOL bFocus)
{
}

int  CBaiduJP3Imp::GetCompositionSegments()
{
	return 0;
}

int  CBaiduJP3Imp::GetCompositionSegmentEnd(int iSeg)
{
	return 0;
}

int CBaiduJP3Imp::GetCompositionSegmentAttr(int iSeg)
{
	return 0;
}

BOOL CBaiduJP3Imp::OnHotkey(LPVOID lpImeContext,REFGUID guidHotKey)
{

	return FALSE;
}

void CBaiduJP3Imp::OnOpenStatusChanged(BOOL bOpen)
{

}

void CBaiduJP3Imp::OnConversionModeChanged(EInputMethod nMode)
{

}

void CBaiduJP3Imp::ShowHelp()
{
}

EInputMethod CBaiduJP3Imp::GetDefInputMode()
{
	return FullAlphanumeric;
}

BOOL CBaiduJP3Imp::ShowCompWnd()
{
	return FALSE;
}

LRESULT CBaiduJP3Imp::OnWildMessage(WPARAM wParam,LPARAM lParam)
{
	return E_NOTIMPL;
}

HMODULE CBaiduJP3Imp::GetModule()
{
	return NULL;
}

void CBaiduJP3Imp::CheckDefIME()
{

}

INT_PTR CBaiduJP3Imp::MessageBox(HWND hWnd,LPCTSTR lpText,LPCTSTR lpCaption,UINT uType)
{
	return 0;
}

void CBaiduJP3Imp::OnFinalRelease()
{

}

HRESULT CBaiduJP3Imp::OnQueryInterface(REFIID riid, void **ppvObject)
{
	return E_NOINTERFACE;
}
