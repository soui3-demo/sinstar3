#include "stdafx.h"
#include "IsSvrProxy.h"
#include <helper\smenuex.h>
#include "AboutDlg.h"
#include "KeyMapDlg.h"

#define TIMERID_DELAY_EXIT	200
#define SPAN_DELAY_EXIT		5000

CIsSvrProxy::CIsSvrProxy(const SStringT & strDataPath)
	:m_strDataPath(strDataPath)
	, m_tts(strDataPath)
	,m_hCoreModule(NULL)
	,m_pCore(NULL)
	,m_funIsCore_Create(NULL)
	,m_funIsCore_Destroy(NULL)
	, m_pKeyMapDlg(NULL)
	, m_pBuildIndexProg(NULL)
{
	m_uMsgTaskbarCreated = RegisterWindowMessage(TEXT("TaskbarCreated"));
}


CIsSvrProxy::~CIsSvrProxy()
{
	if (m_pKeyMapDlg) m_pKeyMapDlg->DestroyWindow();
	if (m_pBuildIndexProg) m_pBuildIndexProg->DestroyWindow();
}

int CIsSvrProxy::OnCreate(LPCREATESTRUCT pCS)
{
	m_hCoreModule = LoadLibrary(_T("iscore.dll"));
	if (!m_hCoreModule) return -1;
	int nRet = 0;
	do {
		m_funIsCore_Create = (funIscore_Create)GetProcAddress(m_hCoreModule, "Iscore_Create");
		m_funIsCore_Destroy = (funIscore_Destroy)GetProcAddress(m_hCoreModule, "Iscore_Destroy");
		if (!m_funIsCore_Create || !m_funIsCore_Destroy)
		{
			nRet = -1;
			break;
		}
		m_pCore = m_funIsCore_Create();
		if (!m_pCore)
		{
			nRet = -1;
			break;
		}
		if (!m_pCore->Init(m_hWnd, this, S_CT2A(m_strDataPath)))
		{
			m_funIsCore_Destroy(m_pCore);
			m_pCore = NULL;
			nRet = -1;
			break;
		}
	} while (0);
	if (nRet != 0)
	{
		FreeLibrary(m_hCoreModule);
		m_hCoreModule = NULL;
	}
	else
	{
		m_trayIcon.Init(m_hWnd, _T("sinstar3 server"));
		if(m_pCore->IsShowTray()) m_trayIcon.Show();
	}
	return nRet;
}

void CIsSvrProxy::OnDestroy()
{
	m_trayIcon.Hide();
	if (m_pCore)
	{
		m_funIsCore_Destroy(m_pCore);
		m_pCore = NULL;
	}
	if (m_hCoreModule)
	{
		FreeLibrary(m_hCoreModule);
		m_hCoreModule = NULL;
	}
}

void CIsSvrProxy::OnBuildIndexProg(int index, PROGTYPE uType, unsigned int dwData)
{
	if (uType == PT_MAX)
	{
		m_pBuildIndexProg = new CBuildIndexProgWnd();
		m_pBuildIndexProg->Create(NULL, WS_POPUP, WS_EX_TOOLWINDOW, 0, 0, 0, 0);
		m_pBuildIndexProg->CenterWindow(GetDesktopWindow());
		m_pBuildIndexProg->SetPage(index);
		m_pBuildIndexProg->SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
	}
	else if (uType == PT_PROG)
	{
		m_pBuildIndexProg->SetProg(dwData);
	}
	else if (uType == PT_END)
	{
		m_pBuildIndexProg->DestroyWindow();
		m_pBuildIndexProg = NULL;
	}
}

void CIsSvrProxy::OnBuildShapePhraseIndex(PROGTYPE uType, unsigned int dwData) {
	OnBuildIndexProg(0, uType, dwData);
}
void CIsSvrProxy::OnBuildSpellPhraseIndex(PROGTYPE uType, unsigned int dwData) {
	OnBuildIndexProg(1, uType, dwData);
}
void CIsSvrProxy::OnBuildSpellPhraseIndex2(PROGTYPE uType, unsigned int dwData) {
	OnBuildIndexProg(2, uType, dwData);
}

void CIsSvrProxy::OnClientActive() {
	m_trayIcon.AnimateIcon();
}

void CIsSvrProxy::OnClientLogin() {
	CSimpleWnd::KillTimer(TIMERID_DELAY_EXIT);
}

void CIsSvrProxy::OnClientLogout() {
	if (m_pCore->CanQuitNow())
	{
		CSimpleWnd::SetTimer(TIMERID_DELAY_EXIT, SPAN_DELAY_EXIT, NULL);
	}
}

void CIsSvrProxy::OnShowTray(bool bTray) {
	if (bTray)
	{
		m_trayIcon.Show();
	}
	else
	{
		m_trayIcon.Hide();
	}
}

void CIsSvrProxy::OnKeyMapFree(CKeyMapDlg * pWnd)
{
	m_pKeyMapDlg = NULL;
}

void CIsSvrProxy::OnShowKeyMap(IDataBlock * pCompData, LPCSTR pszName, LPCSTR pszUrl) {
	SOUI::IBitmap *pBmp = SResLoadFromMemory::LoadImage(pCompData->GetData(), pCompData->GetLength());
	if (pBmp)
	{
		if (m_pKeyMapDlg)
		{
			m_pKeyMapDlg->DestroyWindow();
		}
		m_pKeyMapDlg = new CKeyMapDlg(pBmp, pszName, pszUrl);
		m_pKeyMapDlg->SetListener(this);
		m_pKeyMapDlg->Create(NULL, WS_POPUP | WS_VISIBLE, WS_EX_TOOLWINDOW | WS_EX_TOPMOST, 0, 0, 0, 0);
		m_pKeyMapDlg->SendMessage(WM_INITDIALOG);
		m_pKeyMapDlg->CenterWindow(GetDesktopWindow());
		pBmp->Release();

	}
}

int CIsSvrProxy::TtsGetSpeed()
{
	return m_tts.GetSpeed();
}

int CIsSvrProxy::TtsGetVoice(bool bCh)
{
	return m_tts.GetVoice(bCh);
}

void CIsSvrProxy::TtsSetSpeed(int nSpeed) {
	m_tts.SetSpeed(nSpeed);
}

void CIsSvrProxy::TtsSpeakText(const wchar_t* pText, int nLen, bool bChinese) {
	m_tts.SpeakWText(pText, nLen, bChinese);
}

void CIsSvrProxy::TtsSetVoice(bool bCh, int iToken) {
	m_tts.SetVoice(bCh, iToken);
}

int CIsSvrProxy::TtsGetTokensInfo(bool bCh, wchar_t token[][MAX_TOKEN_NAME_LENGHT], int nBufSize) { 
	return m_tts.GetTokensInfo(bCh, token, nBufSize);
}


LRESULT CIsSvrProxy::OnTrayNotify(UINT uMsg, WPARAM wp, LPARAM lp)
{
	switch (lp)
	{
	case WM_LBUTTONDBLCLK:
		break;
	case WM_RBUTTONUP:
		{
			SMenuEx tmenuex;
			if (tmenuex.LoadMenu(UIRES.smenu.menu_context))
			{
				POINT pt;
				GetCursorPos(&pt);
				SetForegroundWindow(m_hWnd);
				tmenuex.CheckMenuItem(R.id.menu_auto, MF_BYCOMMAND | (m_pCore->IsAutoQuit()?MF_CHECKED:0));
				tmenuex.TrackPopupMenu(0, pt.x, pt.y, m_hWnd);
				PostMessage(WM_NULL);
			}
		}
		break;
	}
	return 0;
}

LRESULT CIsSvrProxy::OnTaskbarCreated(UINT uMsg, WPARAM wp, LPARAM lp)
{
	if (m_pCore->IsShowTray())
	{
		m_trayIcon.Show();
	}
	return 0;
}

void CIsSvrProxy::OnTimer(UINT_PTR uID)
{
	if (uID == TIMERID_DELAY_EXIT)
	{
		PostQuitMessage(0);
	}
	else
	{
		SetMsgHandled(FALSE);
	}
}

void CIsSvrProxy::OnMenuExit(UINT uNotifyCode, int nID, HWND wndCtl)
{
	PostQuitMessage(0);
}

void CIsSvrProxy::OnMenuAbout(UINT uNotifyCode, int nID, HWND wndCtl)
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CIsSvrProxy::OnMenuAutoExit(UINT uNotifyCode, int nID, HWND wndCtl)
{
	m_pCore->SetAutoQuit(!m_pCore->IsAutoQuit());
}
