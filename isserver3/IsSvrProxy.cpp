#include "stdafx.h"
#include "IsSvrProxy.h"
#include <helper/STime.h>
#include <com-loader.hpp>
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

#define TIMERID_CHECK_CLIENT 500
#define SPAN_CHECK_CLIENT 50

#define TIMERID_PENDING_CMD 600
#define SPAN_PENDING_CMD    100

static void DoSomething()
{
	MSG msg;
	while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
}

namespace SOUI {
	namespace IPC
	{
		BOOL  SCreateInstance(IObjRef **ppIpcFactory);
	}
}

CIsSvrProxy::CIsSvrProxy(const SStringT &strDataPath,const SStringT & strSvrPath)
	:m_strDataPath(strDataPath)
    ,m_strSvrPath(strSvrPath)
	,m_hCoreModule(NULL)
	,m_pCore(NULL)
	,m_funIsCore_Create(NULL)
	,m_funIsCore_Destroy(NULL)
	, m_pKeyMapDlg(NULL)
	, m_pBuildIndexProg(NULL)
	, m_pCurModalDlg(NULL)
	, m_pPendingCmd(NULL)
	,m_pFocusConn(NULL)
{
	m_uMsgTaskbarCreated = RegisterWindowMessage(TEXT("TaskbarCreated"));
}


CIsSvrProxy::~CIsSvrProxy()
{
	if (m_pKeyMapDlg) m_pKeyMapDlg->DestroyWindow();
	if (m_pBuildIndexProg) m_pBuildIndexProg->DestroyWindow();
	if(m_pPendingCmd) free(m_pPendingCmd);
}

static SStringT GetVersionInfo(DWORD &dwVer)
{
	TCHAR szFileName[MAX_PATH];
	GetModuleFileName(NULL, szFileName, MAX_PATH);
	TCHAR szDesc[200];
	Helper_PEVersion(szFileName, &dwVer, NULL, szDesc);
	return SStringT(szDesc);
}


void * CIsSvrProxy::GetSecurityAttr() const
{
	return Helper_BuildLowIntegritySA();
}

void CIsSvrProxy::ReleaseSecurityAttr(void * psa) const
{
	SECURITY_ATTRIBUTES *psa2 = (SECURITY_ATTRIBUTES *)psa;
	Helper_FreeSa(psa2);
}

int CIsSvrProxy::OnCreate(LPCREATESTRUCT pCS)
{
	CAutoRefPtr<SOUI::IIpcFactory> ipcFac;
	IPC::SCreateInstance((IObjRef**)&ipcFac);
	ipcFac->CreateIpcServer(&m_ipcSvr);
	m_ipcSvr->Init((ULONG_PTR)m_hWnd, this);
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
		if (!m_pCore->Init(m_hWnd, this, S_CT2A(m_strSvrPath)))
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
		GetVersionInfo(dwVer);
		SStringW strTrayTip = GETSTRING(L"@string/tray_tip");
		strTrayTip.Replace(L"\\n", L"\n");
		SStringT strTipFmt = S_CW2T(strTrayTip, CP_GB);
		TCHAR szVer[100];
		Helper_VersionString(dwVer, szVer);
		SStringT strTip = SStringT().Format(strTipFmt, szVer);
		m_trayIcon.Init(m_hWnd, strTip);
		if(m_pCore->IsShowTray()) m_trayIcon.Show();

		char szConfig[MAX_PATH];
		m_pCore->GetConfigIni(szConfig, MAX_PATH);
		m_nUpdateInterval = GetPrivateProfileIntA("update", "interval", 30, szConfig);

		SetTimer(TIMERID_CHECK_UPDATE, SPAN_CHECK_UPDATE, NULL);
		SetTimer(TIMERID_DATA_REPORT, SPAN_DATA_REPORT1, NULL);
		SetTimer(TIMERID_CHECK_CLIENT,SPAN_CHECK_CLIENT,NULL);
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
	g_SettingsG->Save(m_strDataPath);
	g_SettingsUI->Save(m_strDataPath);
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
	KillTimer(TIMERID_DELAY_EXIT);
}

void CIsSvrProxy::OnClientLogout() {
	if (m_pCore->CanQuitNow())
	{
		SetTimer(TIMERID_DELAY_EXIT, SPAN_DELAY_EXIT, NULL);
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
		CAutoRefPtr<IBitmap> pImg;
		if (pCompData && imgX->LoadFromMemory(pCompData->GetData(), pCompData->GetLength()) >= 1)
		{
			GETRENDERFACTORY->CreateBitmap(&pImg);
			pImg->Init(imgX->GetFrame(0));
		}
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
				::CheckMenuItem(menu.m_hMenu,R.id.menu_auto_exit, MF_BYCOMMAND | (m_pCore->IsAutoQuit()?MF_CHECKED:0));
				::CheckMenuItem(menu.m_hMenu, R.id.menu_auto_run, MF_BYCOMMAND | (IsAutoRun() ? MF_CHECKED : 0));
				int nScale = SDpiHelper::getScale(GetDesktopWindow());
				//nScale=100;
				menu.TrackPopupMenu(0, pt.x, pt.y, m_hWnd,NULL,nScale);
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
		Quit(0);
	}
	else if (uID == TIMERID_CHECK_UPDATE)
	{
		KillTimer(uID);
		CheckUpdate(false);
	}
	else if (uID == TIMERID_DATA_REPORT)
	{
		
		CWorker::getSingletonPtr()->ReportUserInfo();
		KillTimer(uID);
		SetTimer(uID, SPAN_DATA_REPORT2, NULL);
	}else if(uID == TIMERID_CHECK_CLIENT)
	{
		m_ipcSvr->CheckConnectivity();
	}
	else if (uID == TIMERID_PENDING_CMD)
	{
		SASSERT(m_pPendingCmd);
		if (m_pCore->IsDataReady())
		{
			KillTimer(TIMERID_PENDING_CMD);
			OnCopyData(NULL, m_pPendingCmd);
			free(m_pPendingCmd);
			m_pPendingCmd = NULL;
		}
	}
	else
	{
		SetMsgHandled(FALSE);
	}
}

void CIsSvrProxy::OnMenuExit(UINT uNotifyCode, int nID, HWND wndCtl)
{
	Quit(0);
}

void CIsSvrProxy::OnMenuAutoExit(UINT uNotifyCode, int nID, HWND wndCtl)
{
	bool bAutoExit = !m_pCore->IsAutoQuit();
	m_pCore->SetAutoQuit(bAutoExit);
	if (bAutoExit && IsAutoRun())
	{
		SetAutoRun(false);
	}
}

void CIsSvrProxy::OnMenuSettings(UINT uNotifyCode, int nID, HWND wndCtl)
{
	CConfigDlg * pConfig = new CConfigDlg(this);
	pConfig->Create(m_hWnd,WS_POPUP,WS_EX_TOPMOST,0,0,0,0);
	pConfig->SendMessage(WM_INITDIALOG);
	pConfig->SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
}

void CIsSvrProxy::OnMenuAutoRun(UINT uNotifyCode, int nID, HWND wndCtl)
{
	bool bAutoRun = !IsAutoRun();
	if (bAutoRun) m_pCore->SetAutoQuit(false);
	SetAutoRun(bAutoRun);
}


void CIsSvrProxy::CheckUpdate(bool bManual)
{
	char szConfig[MAX_PATH];
	char szDate[100];
	STime timeToday = STime::GetCurrentTime();
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

	char szUri[500];
	GetPrivateProfileStringA("update", "url", "http://soime.cm/sinstar3_update.xml", szUri, 500, szConfig);

	CWorker::getSingletonPtr()->CheckUpdate(szUri, bManual);
}


void CIsSvrProxy::OnCheckUpdateResult(EventArgs *e)
{
	EventCheckUpdateResult *e2  = sobj_cast<EventCheckUpdateResult>(e);
	if (!e2->bServerOK)
	{
		if (e2->bManual)
		{
			SMessageBox(GetDesktopWindow(), _T("升级服务器不可用!"), _T("提示"), MB_OK | MB_ICONINFORMATION);
		}
		return;
	}
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
			ShowModal(&updateDlg);
		}
		else if (e2->bManual)
		{
			SMessageBox(GetDesktopWindow(), _T("没有发现新版本!"), _T("提示"), MB_OK | MB_ICONINFORMATION);
		}
	}
}

INT_PTR CIsSvrProxy::ShowModal(SHostDialog *pDlg)
{
	if(m_pCurModalDlg)
	{
		m_pCurModalDlg->EndDialog(-1);
		m_pCurModalDlg = NULL;
	}
	m_pCurModalDlg = pDlg;
	INT_PTR uRet = pDlg->DoModal(GetDesktopWindow());
	m_pCurModalDlg=NULL;
	return uRet;
}

void CIsSvrProxy::Quit(int nCode)
{
	if(m_pCurModalDlg) m_pCurModalDlg->EndDialog(-1);
	PostQuitMessage(nCode);
}

bool CIsSvrProxy::IsAutoRun() const
{
	CRegKey reg;
	LONG ret = reg.Open(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), KEY_READ | KEY_WOW64_64KEY);
	if (ret == ERROR_SUCCESS)
	{
		TCHAR szPath[MAX_PATH] = { 0 };
		ULONG len = MAX_PATH;
		ret = reg.QueryStringValue(_T("sinstar3_server"), szPath, &len);
		reg.Close();
		if (ret == ERROR_SUCCESS)
		{
			TCHAR szPathExe[MAX_PATH];
			len = MAX_PATH;
			GetModuleFileName(NULL, szPathExe, len);
			if (_tcsicmp(szPath, szPathExe) == 0)
				return true;
		}
	}
	return false;
}

bool CIsSvrProxy::SetAutoRun(bool bAutoRun) const
{
	CRegKey reg;
	LONG ret = reg.Open(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), KEY_READ | KEY_WRITE | KEY_WOW64_64KEY);
	if (ret == ERROR_SUCCESS)
	{
		if (!bAutoRun)
		{
			reg.DeleteValue(_T("sinstar3_server"));
		}
		else
		{
			TCHAR szPathExe[MAX_PATH];
			ULONG len = MAX_PATH;
			GetModuleFileName(NULL, szPathExe, len);
			reg.SetStringValue(_T("sinstar3_server"), szPathExe);
		}
		reg.Close();
		return true;
	}
	return false;
}

void CIsSvrProxy::OnEndSession(BOOL bEnding, UINT uLogOff)
{
	OnDestroy();
}

void CIsSvrProxy::OnUpdateNow()
{
	CheckUpdate(true);
}

extern const TCHAR * KSinstar3WndName;
LRESULT CIsSvrProxy::OnCopyData(HWND hWnd,PCOPYDATASTRUCT lpCopyData)
{
	if(!(lpCopyData->dwData == CD_CMD_INSTALL_CIT
		|| lpCopyData->dwData == CD_CMD_INSTALL_PLT
		|| lpCopyData->dwData == CD_CMD_INSTALL_SKIN))
	{
		SetMsgHandled(FALSE);
		return 0;
	}
	if(!m_pCore->IsDataReady())
	{
		if(m_pPendingCmd)
		{
			SLOG_WARN("discard shell cmd:"<<lpCopyData->dwData);
			return 3;
		}else	
		{
			m_pPendingCmd = (PCOPYDATASTRUCT)malloc(sizeof(COPYDATASTRUCT)+lpCopyData->cbData);
			m_pPendingCmd->dwData = lpCopyData->dwData;
			m_pPendingCmd->cbData = lpCopyData->cbData;
			m_pPendingCmd->lpData = m_pPendingCmd+1;
			memcpy(m_pPendingCmd->lpData,lpCopyData->lpData,lpCopyData->cbData);
			SetTimer(TIMERID_PENDING_CMD,SPAN_PENDING_CMD);
			return 2;
		}
	}
	if(lpCopyData->dwData == CD_CMD_INSTALL_CIT)
	{//install cit
		SStringA strPath = S_CW2A((wchar_t*)lpCopyData->lpData);
		if(m_pCore->InstallCit(strPath))
		{
			SMessageBox(GetDesktopWindow(),_T("码表安装成功！"),_T("提示"),MB_OK|MB_ICONINFORMATION);
		}else
		{
			SMessageBox(GetDesktopWindow(),SStringT().Format(_T("码表安装失败！错误码:%d"),GetLastError()),_T("提示"),MB_OK|MB_ICONSTOP);
		}
	}else if(lpCopyData->dwData == CD_CMD_INSTALL_PLT)
	{//install plt
		SStringA strPath = S_CW2A((wchar_t*)lpCopyData->lpData);
		if(m_pCore->InstallPlt(strPath))
		{
			SMessageBox(GetDesktopWindow(),_T("词库安装成功！"),_T("提示"),MB_OK|MB_ICONINFORMATION);
		}else
		{
			SMessageBox(GetDesktopWindow(),SStringT().Format(_T("词库安装失败！错误码:%d"),GetLastError()),_T("提示"),MB_OK|MB_ICONSTOP);
		}
	}else if(lpCopyData->dwData == CD_CMD_INSTALL_SKIN)
	{
		SStringT strPath = S_CW2T((wchar_t*)lpCopyData->lpData);
		int nPos = strPath.ReverseFind('\\');
		SStringT strName = strPath.Mid(nPos);
		SStringT strDst = m_strDataPath + _T("\\skins")+strName;
		if(CopyFile(strPath,strDst,FALSE))
		{
			if(IDOK==SMessageBox(GetDesktopWindow(),_T("词库皮肤成功！现在使用吗?"),_T("提示"),MB_OKCANCEL|MB_ICONQUESTION))
			{
				if(!CDataCenter::getSingletonPtr()->GetData().changeSkin(strDst))
				{
					DeleteFile(strDst);
					SMessageBox(GetDesktopWindow(),_T("应用皮肤失败!不支持的皮肤格式"),_T("错误"),MB_OK|MB_ICONSTOP);
				}else
				{
					g_SettingsG->strSkin = strDst;
					g_SettingsG->SetModified(true);


					SStringA strSkinUtf8 = S_CW2A(S_CT2W(strDst),CP_UTF8);
					COPYDATASTRUCT cds = { 0 };
					cds.dwData = CMD_CHANGESKIN;
					cds.cbData = strSkinUtf8.GetLength()+1;
					cds.lpData = (PVOID)(LPCSTR)strSkinUtf8;

					HWND hFind = FindWindowEx(HWND_MESSAGE, NULL, NULL, KSinstar3WndName);
					while (hFind)
					{
						::SendMessage(hFind, WM_COPYDATA, (WPARAM)m_hWnd, (LPARAM)&cds);
						hFind = FindWindowEx(HWND_MESSAGE, hFind, NULL, KSinstar3WndName);
					}
				}

			}
		}else
		{
			SMessageBox(GetDesktopWindow(),SStringT().Format(_T("词库皮肤失败！错误码:%d"),GetLastError()),_T("提示"),MB_OK|MB_ICONSTOP);
		}

	}
	return 1;
}

void CIsSvrProxy::OnSetFocus(CSvrConnection * pConn)
{
	if(m_pFocusConn)
	{
		if(m_pFocusConn!=pConn)
		{
			Param_OnSetFocus param;
			param.bFocus = FALSE;
			m_pFocusConn->HandleOnSetFocus(param);
		}
	}
	m_pFocusConn = pConn;
}

void CIsSvrProxy::OnKillFocus(CSvrConnection * pConn)
{
	if(pConn == m_pFocusConn)
		m_pFocusConn=NULL;
}

void CIsSvrProxy::OnConnected(IIpcConnection * pConn)
{

}


void CIsSvrProxy::OnDisconnected(IIpcConnection * pConn)
{
	if(pConn == (IIpcConnection *)m_pFocusConn)
	{
		m_pFocusConn = NULL;
	}
}
