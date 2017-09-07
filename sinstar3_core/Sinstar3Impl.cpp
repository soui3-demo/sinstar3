#include "StdAfx.h"
#include "Sinstar3Impl.h"

#include <initguid.h>

class CAutoContext
{
public:
	CAutoContext(void ** ppCtx, void * pValue)
	{
		m_ppCtx = ppCtx;
		*m_ppCtx = pValue;		
	}

	~CAutoContext()
	{
		*m_ppCtx = NULL;
	}

	void ** m_ppCtx;
};

CSinstar3Impl::CSinstar3Impl(ITextService *pTxtSvr)
:m_pTxtSvr(pTxtSvr)
,m_pInputWnd(NULL)
,m_pStatusWnd(NULL)
,m_pCurImeContext(NULL)
{
	theModule->AddRef();

 	m_pInputWnd = new CInputWnd(m_inputState.GetInputContext());
	m_pStatusWnd = new CStatusWnd();
	m_pStatusWnd->Create();
	m_pInputWnd->Create();

	m_inputState.SetInputListener(this);

	SOUI::CSimpleWnd::Create(_T("sinstar3_msg_recv"),WS_DISABLED|WS_POPUP,WS_EX_TOOLWINDOW,0,0,0,0,HWND_MESSAGE,NULL);
	ISComm_Login(m_hWnd);
}

CSinstar3Impl::~CSinstar3Impl(void)
{
	ISComm_Logout(m_hWnd);
	SOUI::CSimpleWnd::DestroyWindow();

	m_pInputWnd->DestroyWindow();
	m_pStatusWnd->DestroyWindow();
	delete m_pStatusWnd;
	delete m_pInputWnd;


	theModule->Release();
}


void CSinstar3Impl:: ProcessKeyStoke(LPVOID lpImeContext,UINT vkCode,LPARAM lParam,BOOL bKeyDown,BOOL *pbEaten)
{
	if(!bKeyDown)
	{
		*pbEaten = FALSE;
		return;
	}
	CAutoContext autoCtx(&m_pCurImeContext,lpImeContext);
	BYTE byKeyState[256];
	GetKeyboardState(byKeyState);
	*pbEaten = m_inputState.TestKeyDown(vkCode,lParam,byKeyState);
}

void CSinstar3Impl:: TranslateKey(LPVOID lpImeContext,UINT vkCode,UINT uScanCode,BOOL bKeyDown,BOOL *pbEaten)
{
	CAutoContext autoCtx(&m_pCurImeContext,lpImeContext);

	*pbEaten = TRUE;

	BYTE byKeyState[256];
	GetKeyboardState(byKeyState);
	if(m_inputState.HandleKeyDown(vkCode,uScanCode,byKeyState))
	{
		m_pInputWnd->UpdateUI();
	}
}

void CSinstar3Impl::OnIMESelect(BOOL bSelect)
{
}

void CSinstar3Impl::OnSetCaretPosition(POINT pt,int nHei)
{
	m_pInputWnd->MoveTo(pt,nHei);
}

void CSinstar3Impl::OnSetFocusSegmentPosition(POINT pt,int nHei)
{
}

void CSinstar3Impl::OnCompositionStarted()
{
	m_pInputWnd->Show(TRUE);
}

void CSinstar3Impl::OnCompositionChanged()
{
}

void CSinstar3Impl::OnCompositionTerminated()
{
 	m_pInputWnd->Show(FALSE);
}

void CSinstar3Impl::OnSetFocus(BOOL bFocus)
{
	SLOG_INFO("GetThreadID="<<GetCurrentThreadId()<<" focus="<<bFocus);
	m_pStatusWnd->Show(bFocus);
	m_pInputWnd->Show(bFocus);
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


void CSinstar3Impl::OnFinalRelease()
{

}

HRESULT CSinstar3Impl::OnQueryInterface(REFIID riid, void **ppvObject)
{
	return E_NOINTERFACE;
}

LRESULT CSinstar3Impl::OnSvrNotify(UINT uMsg, WPARAM wp, LPARAM lp)
{
	PMSGDATA pMsg=ISComm_OnSeverNotify(m_hWnd,wp,lp);
	if(wp == NT_KEYIN)
	{

	}
	else if(wp==NT_COMPINFO)
	{
		CDataCenter::getSingleton().Lock();
		CMyData &myData = CDataCenter::getSingleton().GetData();
		myData.m_compInfo.SetSvrCompInfo(ISComm_GetCompInfo());
		CDataCenter::getSingleton().Unlock();

		EventSvrNotify *evt= new EventSvrNotify(this);
		evt->wp = wp;
		evt->lp = lp;
		SNotifyCenter::getSingleton().FireEventAsync(evt);
		evt->Release();
	}

	return 0;
}

HWND CSinstar3Impl::GetHwnd() const
{
	return m_hWnd;
}

void CSinstar3Impl::OnInputStart()
{
	if(!m_pCurImeContext) return;
	m_pTxtSvr->StartComposition(m_pCurImeContext);
}

void CSinstar3Impl::OnInputEnd(const SStringT & strInput,int nDelay)
{
	if(!m_pCurImeContext) return;
	SStringW strResult = S_CT2W(strInput);
	m_pTxtSvr->UpdateResultAndCompositionStringW(m_pCurImeContext,strResult,strResult.GetLength(),NULL,0);
	m_pTxtSvr->EndComposition(m_pCurImeContext);
	m_pInputWnd->Hide(nDelay);
}

BOOL CSinstar3Impl::GoNextCandidatePage()
{
	return m_pInputWnd->GoNextCandidatePage();
}

BOOL CSinstar3Impl::GoPrevCandidatePage()
{
	return m_pInputWnd->GoPrevCandidatePage();
}

short CSinstar3Impl::SelectCandidate(short iCand)
{
	return m_pInputWnd->SelectCandidate(iCand);
}

void CSinstar3Impl::CloseInputWnd(int nDelayMS)
{
	m_pInputWnd->Hide(nDelayMS);
}

BOOL CSinstar3Impl::SetOpenStatus(BOOL bOpen)
{
	SASSERT(m_pCurImeContext);
	return m_pTxtSvr->SetOpenStatus(m_pCurImeContext,bOpen);
}

BOOL CSinstar3Impl::GetOpenStatus()
{
	SASSERT(m_pCurImeContext);
	return m_pTxtSvr->GetOpenStatus(m_pCurImeContext);
}


