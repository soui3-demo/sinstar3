#include "stdafx.h"
#include "IsSvrProxy.h"
#include <helper/smenuex.h>
#include <helper/STime.h>
#include "settings/ConfigDlg.h"
#include "ui/UpdateInfoDlg.h"
#include "ui/KeyMapDlg.h"
#include "../helper/helper.h"
#include "../include/reg.h"
#include "worker.h"

#define TIMERID_DELAY_EXIT	200
#define SPAN_DELAY_EXIT		5000
#define TIMERID_CHECK_UPDATE 300
#define SPAN_CHECK_UPDATE	1000

#define TIMERID_DATA_REPORT 400
#define SPAN_DATA_REPORT1	50
#define SPAN_DATA_REPORT2	24*60*60*1000	//1 day

static void DoSomething()
{
	MSG msg;
	while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
}

CIsSvrProxy::CIsSvrProxy(const SStringT & strDataPath)
	:m_strDataPath(strDataPath)
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

static SStringT GetVersionInfo(DWORD &dwVer)
{
	TCHAR szFileName[MAX_PATH];
	GetModuleFileName(NULL, szFileName, MAX_PATH);
	TCHAR szDesc[200];
	Helper_PEVersion(szFileName, &dwVer, NULL, szDesc);
	return SStringT(szDesc);
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
		DWORD dwVer = 0;
		SStringT strTip = GetVersionInfo(dwVer);
		strTip += _T("\n版本:");
		TCHAR szVer[100];
		Helper_VersionString(dwVer, szVer);
		strTip += szVer;
		m_trayIcon.Init(m_hWnd, strTip);
		if(m_pCore->IsShowTray()) m_trayIcon.Show();

		char szConfig[MAX_PATH];
		m_pCore->GetConfigIni(szConfig, MAX_PATH);
		m_nUpdateInterval = GetPrivateProfileIntA("update", "interval", 30, szConfig);

		CSimpleWnd::SetTimer(TIMERID_CHECK_UPDATE, SPAN_CHECK_UPDATE, NULL);
		CSimpleWnd::SetTimer(TIMERID_DATA_REPORT, SPAN_DATA_REPORT1, NULL);
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

LRESULT CIsSvrProxy::OnBuildIndexProg(UINT uMsg, WPARAM wp, LPARAM lp)
{
	int iPage = uMsg - UM_BUILD_INDEX_PROG0;
	PROGTYPE uType = (PROGTYPE)wp;
	int nProg = (int)lp;
	if (uType == PT_MAX)
	{
		m_pBuildIndexProg = new CBuildIndexProgWnd();
		m_pBuildIndexProg->Create(NULL, WS_POPUP, WS_EX_TOOLWINDOW, 0, 0, 0, 0);
		m_pBuildIndexProg->CenterWindow(GetDesktopWindow());
		m_pBuildIndexProg->SetPage(iPage,nProg);
		m_pBuildIndexProg->SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
	}
	else if (uType == PT_PROG)
	{
		m_pBuildIndexProg->SetProg(nProg);
	}
	else if (uType == PT_END)
	{
		m_pBuildIndexProg->DestroyWindow();
		m_pBuildIndexProg = NULL;
	}
	return 0;
}


void CIsSvrProxy::OnBuildShapePhraseIndex(PROGTYPE uType, unsigned int dwData) {
	_OnBuildIndexProg(0, uType, dwData);
}

void CIsSvrProxy::OnBuildSpellPhraseIndex(PROGTYPE uType, unsigned int dwData) {
	_OnBuildIndexProg(1, uType, dwData);
}
void CIsSvrProxy::OnBuildSpellPhraseIndex2(PROGTYPE uType, unsigned int dwData) {
	_OnBuildIndexProg(2, uType, dwData);
}

void CIsSvrProxy::OnImportUserDict(PROGTYPE uType, unsigned int dwData) {
	_OnBuildIndexProg(3, uType, dwData);
}


void CIsSvrProxy::_OnBuildIndexProg(int indexMode, PROGTYPE uType, unsigned int dwData)
{
	static int nMax = 0, nPrevProg = 0;
	if (uType == PT_MAX) nMax = dwData, nPrevProg = 0;
	if (uType == PT_PROG)
	{
		if (dwData - nPrevProg < nMax / 200)
		{
			return;
		}
		nPrevProg = dwData;
	}
	SendMessage(UM_BUILD_INDEX_PROG0+ indexMode, uType, dwData);
	DoSomething();
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

int CIsSvrProxy::GetUpdateInterval() const
{
	return m_nUpdateInterval;
}

void CIsSvrProxy::OnUpdateIntervalChanged(int nInterval)
{
	m_nUpdateInterval = nInterval;
	char szConfig[MAX_PATH];
	m_pCore->GetConfigIni(szConfig, MAX_PATH);
	WritePrivateProfileStringA("update", "interval", SStringA().Format("%d",m_nUpdateInterval), szConfig);
}

void CIsSvrProxy::OnShowKeyMap(IDataBlock * pCompData, LPCSTR pszName, LPCSTR pszUrl) {
	CAutoRefPtr<IImgDecoderFactory> imgDecoder;
	SComLoader comLoader;
	comLoader.CreateInstance(_T("imgdecoder-gdip.dll"), (IObjRef**)&imgDecoder);
	if (imgDecoder)
	{
		CAutoRefPtr<IImgX> imgX;
		imgDecoder->CreateImgX(&imgX);
		if (imgX->LoadFromMemory(pCompData->GetData(), pCompData->GetLength()) == 0)
		{
			SLOG_WARN("load key map image failed!");
			return;
		}
		if (imgX->GetFrameCount() == 0)
		{
			SLOG_WARN("key map image has 0 frames!");
			return;
		}
		CAutoRefPtr<IBitmap> pImg;
		GETRENDERFACTORY->CreateBitmap(&pImg);
		pImg->Init(imgX->GetFrame(0));

		if (m_pKeyMapDlg)
		{
			m_pKeyMapDlg->DestroyWindow();
		}
		m_pKeyMapDlg = new CKeyMapDlg(pImg, pszName, pszUrl);
		m_pKeyMapDlg->SetListener(this);
		m_pKeyMapDlg->Create(NULL, WS_POPUP | WS_VISIBLE, WS_EX_TOOLWINDOW | WS_EX_TOPMOST, 0, 0, 0, 0);
		m_pKeyMapDlg->SendMessage(WM_INITDIALOG);
		m_pKeyMapDlg->CenterWindow(GetDesktopWindow());
		imgDecoder = NULL;
	}
}

int CIsSvrProxy::TtsGetSpeed()
{
	return CWorker::getSingletonPtr()->GetSpeed();
}

int CIsSvrProxy::TtsGetVoice(bool bCh)
{
	return CWorker::getSingletonPtr()->GetVoice(bCh);
}

void CIsSvrProxy::TtsSetSpeed(int nSpeed) {
	CWorker::getSingletonPtr()->SetSpeed(nSpeed);
}

void CIsSvrProxy::TtsSpeakText(const wchar_t* pText, int nLen, bool bChinese) {
	CWorker::getSingletonPtr()->SpeakWText(pText, nLen, bChinese);
}

void CIsSvrProxy::TtsSetVoice(bool bCh, int iToken) {
	CWorker::getSingletonPtr()->SetVoice(bCh, iToken);
}

int CIsSvrProxy::TtsGetTokensInfo(bool bCh, wchar_t token[][MAX_TOKEN_NAME_LENGHT], int nBufSize) { 
	return CWorker::getSingletonPtr()->GetTokensInfo(bCh, token, nBufSize);
}

DWORD CIsSvrProxy::OnQueryVersion() const
{
	DWORD dwVer;
	GetVersionInfo(dwVer);
	return dwVer;
}


LRESULT CIsSvrProxy::OnTrayNotify(UINT uMsg, WPARAM wp, LPARAM lp)
{
	switch (lp)
	{
	case WM_LBUTTONDBLCLK:
		break;
	case WM_RBUTTONUP:
		{
			SMenu menu;
			if (menu.LoadMenu(UIRES.smenu.context_tray))
			{
				POINT pt;
				GetCursorPos(&pt);
				SetForegroundWindow(m_hWnd);
				::CheckMenuItem(menu.m_hMenu,R.id.menu_auto, MF_BYCOMMAND | (m_pCore->IsAutoQuit()?MF_CHECKED:0));
				menu.TrackPopupMenu(0, pt.x, pt.y, m_hWnd);
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
	else if (uID == TIMERID_CHECK_UPDATE)
	{
		CSimpleWnd::KillTimer(uID);
		CheckUpdate(false);
	}
	else if (uID == TIMERID_DATA_REPORT)
	{
		
		CWorker::getSingletonPtr()->ReportUserInfo();
		KillTimer(uID);
		CSimpleWnd::SetTimer(uID, SPAN_DATA_REPORT2, NULL);
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

void CIsSvrProxy::OnMenuAutoExit(UINT uNotifyCode, int nID, HWND wndCtl)
{
	m_pCore->SetAutoQuit(!m_pCore->IsAutoQuit());
}

void CIsSvrProxy::OnMenuSettings(UINT uNotifyCode, int nID, HWND wndCtl)
{
	CConfigDlg config(this);
	if (ID_CHECK_UPDATE_NOW == config.DoModal(m_hWnd))
	{
		CheckUpdate(true);
	}
}


void CIsSvrProxy::CheckUpdate(bool bManual)
{
	char szConfig[MAX_PATH];
	char szDate[100];
	CTime timeToday = CTime::GetCurrentTime();
	m_pCore->GetConfigIni(szConfig, MAX_PATH);
	if (!bManual)
	{
		if (m_nUpdateInterval == 0) return;
		int nDay, nMonth, nYear;
		GetPrivateProfileStringA("update", "date", "0-0-0", szDate, 100, szConfig);
		sscanf(szDate, "%d-%d-%d", &nMonth, &nDay, &nYear);
		int nDays = 360 * (timeToday.GetYear() - nYear);
		nDays += 30 * (timeToday.GetMonth() - nMonth);
		nDays += timeToday.GetDay() - nDay;
		if (nDays<m_nUpdateInterval) return;
	}

	//update date
	sprintf(szDate, "%d-%d-%d", timeToday.GetMonth(), timeToday.GetDay(), timeToday.GetYear());
	WritePrivateProfileStringA("update", "date", szDate, szConfig);

	CWorker::getSingletonPtr()->CheckUpdate(szConfig, bManual);
}


void CIsSvrProxy::OnCheckUpdateResult(EventArgs *e)
{
	EventCheckUpdateResult *e2  = sobj_cast<EventCheckUpdateResult>(e);
	char szConfig[MAX_PATH];
	m_pCore->GetConfigIni(szConfig, MAX_PATH);
	WritePrivateProfileStringA("update", "url", S_CW2A(e2->strNewUpdateUrl), szConfig);

	CRegKey reg;
	LONG ret = reg.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\SetoutSoft\\sinstar3"), KEY_READ | KEY_WOW64_64KEY);
	if (ret == ERROR_SUCCESS)
	{
		TCHAR szClientPath[MAX_PATH];
		ULONG len = MAX_PATH;
		reg.QueryStringValue(_T("path_client"), szClientPath, &len);
		reg.Close();

		TCHAR szSvrPath[MAX_PATH];
		GetModuleFileName(NULL, szSvrPath, MAX_PATH);
		DWORD dwVerSvr = 0;
		Helper_PEVersion(szSvrPath, &dwVerSvr, NULL, NULL);

		if (e2->dwNewVer > dwVerSvr)
		{//found new ver
			e2->dwCurVer = dwVerSvr;

			CUpdateInfoDlg updateDlg(e2);
			updateDlg.DoModal(GetDesktopWindow());
		}
		else if (e2->bManual)
		{
			SMessageBox(GetDesktopWindow(), _T("没有发现新版本!"), _T("提示"), MB_OK | MB_ICONINFORMATION);
		}
	}
}
