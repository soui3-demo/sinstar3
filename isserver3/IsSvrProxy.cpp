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
#include <process.h>

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

#define TIMERID_AUTO_SAVE_SETTING 700
#define SPAN_AUTO_SAVE_SETTING 5000	//5 seconds

#define TIMERID_RECONN 800
#define SPAN_RECONN 2000	//2 seconds

#define TIMERID_BACKUP	900
#define SPAN_BACKUP		60*60*1000	//60 minutes.
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

CIsSvrProxy * CIsSvrProxy::_this = NULL;
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
	_this = this;
	m_uMsgTaskbarCreated = RegisterWindowMessage(TEXT("TaskbarCreated"));
}


CIsSvrProxy::~CIsSvrProxy()
{
	if (m_pKeyMapDlg) m_pKeyMapDlg->DestroyWindow();
	if (m_pBuildIndexProg) m_pBuildIndexProg->DestroyWindow();
	if(m_pPendingCmd) free(m_pPendingCmd);
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
		if (!m_pCore->Init(m_hWnd, this, m_strSvrPath))
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
		TCHAR szExe[MAX_PATH];
		GetModuleFileName(NULL,szExe,MAX_PATH);
		WORD wVer[4];
		SDpiHelper::PEVersion(szExe,wVer[0],wVer[1],wVer[2],wVer[3]);
		SStringT strVer=SStringT().Format(_T("%u.%u.%u.%u"),wVer[0],wVer[1],wVer[2],wVer[3]);
		SStringW strTrayTip = GETSTRING(L"@string/tray_tip");
		strTrayTip.Replace(L"\\n", L"\n");
		SStringT strTipFmt = S_CW2T(strTrayTip);
		SStringT strTip = SStringT().Format(strTipFmt, strVer.c_str());
		m_trayIcon.Init(m_hWnd, strTip);
		if(g_SettingsG->bShowTray) m_trayIcon.Show();

		SetTimer(TIMERID_CHECK_UPDATE, SPAN_CHECK_UPDATE, NULL);
		SetTimer(TIMERID_DATA_REPORT, SPAN_DATA_REPORT1, NULL);
		SetTimer(TIMERID_CHECK_CLIENT,SPAN_CHECK_CLIENT,NULL);
		SetTimer(TIMERID_AUTO_SAVE_SETTING,SPAN_AUTO_SAVE_SETTING,NULL);
		SetTimer(TIMERID_BACKUP,SPAN_BACKUP,NULL);
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

	CDataCenter::getSingletonPtr()->GetData().saveSpeed();
}

LRESULT CIsSvrProxy::OnBuildIndexProg(UINT uMsg, WPARAM wp, LPARAM lp)
{
	int iPage = uMsg - UM_BUILD_INDEX_PROG0;
	PROGTYPE uType = (PROGTYPE)wp;
	int nProg = (int)lp;
	if (uType == PT_MAX)
	{
		m_pBuildIndexProg = new CBuildIndexProgWnd();
		m_pBuildIndexProg->Create(GetActiveWindow(), WS_POPUP, 0, 0, 0, 0, 0);
		m_pBuildIndexProg->CenterWindow();
		m_pBuildIndexProg->SetPage(iPage,nProg);
		m_pBuildIndexProg->ShowWindow(SW_SHOW);
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
		if ((int)dwData - nPrevProg < nMax / 200)
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
	if (m_pCore->CanQuitNow() && g_SettingsG->bAutoQuit)
	{
		SetTimer(TIMERID_DELAY_EXIT, SPAN_DELAY_EXIT, NULL);
	}
}

void CIsSvrProxy::ShowTray(bool bTray) {
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
	return g_SettingsG->nUpdateInterval;
}

void CIsSvrProxy::OnUpdateIntervalChanged(int nInterval)
{
	g_SettingsG->nUpdateInterval=nInterval;
	g_SettingsG->SetModified(true);
}

void CIsSvrProxy::OnShowKeyMap(IDataBlock * pCompData, LPCWSTR pszName, LPCWSTR pszUrl) {
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
	return g_SettingsG->nTtsSpeed;
}

int CIsSvrProxy::TtsGetVoice(bool bCh)
{
	return bCh?g_SettingsG->iTtsChVoice:g_SettingsG->iTtsEnVoice;
}

void CIsSvrProxy::TtsSetSpeed(int nSpeed) {
	g_SettingsG->nTtsSpeed = nSpeed;
	g_SettingsG->SetModified(true);
	CWorker::getSingletonPtr()->SetSpeed(nSpeed);
}

void CIsSvrProxy::TtsSpeakText(const wchar_t* pText, int nLen, bool bChinese) {
	CWorker::getSingletonPtr()->SpeakWText(pText, nLen, bChinese);
}

void CIsSvrProxy::TtsSetVoice(bool bCh, int iToken) {
	if(bCh) g_SettingsG->iTtsChVoice = iToken;
	else g_SettingsG->iTtsEnVoice = iToken;
	g_SettingsG->SetModified(true);
	CWorker::getSingletonPtr()->SetVoice(bCh, iToken);
}

int CIsSvrProxy::TtsGetTokensInfo(bool bCh, wchar_t token[][MAX_TOKEN_NAME_LENGHT], int nBufSize) { 
	return CWorker::getSingletonPtr()->GetTokensInfo(bCh, token, nBufSize);
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
				::CheckMenuItem(menu.m_hMenu,R.id.menu_auto_exit, MF_BYCOMMAND | (g_SettingsG->bAutoQuit?MF_CHECKED:0));
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
	if (g_SettingsG->bShowTray)
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
	}else if(uID == SPAN_DATA_REPORT2)
	{
		CWorker::getSingletonPtr()->ReportUserInfo();
	}
	else if(uID == TIMERID_CHECK_CLIENT)
	{
		m_ipcSvr->CheckConnectivity();
	}
	else if (uID == TIMERID_PENDING_CMD)
	{
		SASSERT(m_pPendingCmd);
		if (m_pCore->IsDataReady())
		{
			KillTimer(TIMERID_PENDING_CMD);
			OnDelayCopyData(0, 0,(LPARAM)m_pPendingCmd);
			m_pPendingCmd = NULL;
		}
	}else if(uID == TIMERID_AUTO_SAVE_SETTING)
	{
		g_SettingsG->Save(m_strDataPath);
		g_SettingsUI->Save(m_strDataPath);
	}else if(uID==TIMERID_RECONN)
	{
		KillTimer(uID);
		OnCheckReconn();
	}else if(uID == TIMERID_BACKUP)
	{
		BackupData();
		KillTimer(uID);
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
	g_SettingsG->bAutoQuit = !g_SettingsG->bAutoQuit;
	g_SettingsG->SetModified(true);
	if (g_SettingsG->bAutoQuit && IsAutoRun())
	{
		SetAutoRun(false);
	}
}

void CIsSvrProxy::OnMenuSettings(UINT uNotifyCode, int nID, HWND wndCtl)
{
	CConfigDlg * pConfig = CConfigDlg::GetInstance();
	if(!pConfig)
	{
		pConfig = new CConfigDlg(this);
		pConfig->Create(m_hWnd,WS_POPUP,WS_EX_TOPMOST,0,0,0,0);
		pConfig->SendMessage(WM_INITDIALOG);
	}
	pConfig->SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
}

void CIsSvrProxy::OnMenuAutoRun(UINT uNotifyCode, int nID, HWND wndCtl)
{
	bool bAutoRun = !IsAutoRun();
	if (bAutoRun)
	{
		g_SettingsG->bAutoQuit=FALSE;
		g_SettingsG->SetModified(true);
	}
	SetAutoRun(bAutoRun);
}


void CIsSvrProxy::CheckUpdate(bool bManual)
{
	STime timeToday = STime::GetCurrentTime();
	if (!bManual)
	{
		if (g_SettingsG->nUpdateInterval == 0) 
			return;
		int nDay=0, nMonth=0, nYear=0;
		_stscanf(g_SettingsG->szUpdateDate, _T("%d-%d-%d"), &nMonth, &nDay, &nYear);
		int nDays = 360 * (timeToday.GetYear() - nYear);
		nDays += 30 * (timeToday.GetMonth() - nMonth);
		nDays += timeToday.GetDay() - nDay;
		if (nDays<g_SettingsG->nUpdateInterval) 
			return;
	}

	//update date
	_stprintf(g_SettingsG->szUpdateDate, _T("%d-%d-%d"), timeToday.GetMonth(), timeToday.GetDay(), timeToday.GetYear());
	g_SettingsG->SetModified(true);

	CWorker::getSingletonPtr()->CheckUpdate(g_SettingsG->szUpdateUrl, bManual);
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
	_tcscpy(g_SettingsG->szUpdateUrl,S_CW2T(e2->strNewUpdateUrl));
	g_SettingsG->SetModified(true);

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


LRESULT CIsSvrProxy::OnDelayCopyData(UINT uMsg,WPARAM wp,LPARAM lp)
{
	PCOPYDATASTRUCT lpCopyData = (PCOPYDATASTRUCT)lp;
	if(!m_pCore->IsDataReady())
	{
		if(m_pPendingCmd)
		{
			SLOG_WARN("discard shell cmd:"<<lpCopyData->dwData);
			free(lpCopyData);
			return 3;
		}else	
		{
			m_pPendingCmd = lpCopyData;
			SetTimer(TIMERID_PENDING_CMD,SPAN_PENDING_CMD);
			return 2;
		}
	}
	if(lpCopyData->dwData == CD_CMD_INSTALL_CIT)
	{//install cit
		SStringT strPath = S_CW2T((wchar_t*)lpCopyData->lpData);
		if(m_pCore->InstallCit(strPath))
		{
			SMessageBox(GetDesktopWindow(),_T("码表安装成功！"),_T("提示"),MB_OK|MB_ICONINFORMATION);
		}else
		{
			SMessageBox(GetDesktopWindow(),SStringT().Format(_T("码表安装失败！错误码:%d"),GetLastError()),_T("提示"),MB_OK|MB_ICONSTOP);
		}
	}else if(lpCopyData->dwData == CD_CMD_INSTALL_PLT)
	{//install plt
		SStringT strPath = S_CW2T((wchar_t*)lpCopyData->lpData);
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
			if(IDOK==SMessageBox(GetDesktopWindow(),_T("安装皮肤成功！现在使用吗?"),_T("提示"),MB_OKCANCEL|MB_ICONQUESTION))
			{
				if(!ChangeSkin(strDst))
				{
					DeleteFile(strDst);
					SMessageBox(GetDesktopWindow(),_T("应用皮肤失败!不支持的皮肤格式"),_T("错误"),MB_OK|MB_ICONSTOP);
				}
			}
		}else
		{
			SMessageBox(GetDesktopWindow(),SStringT().Format(_T("安装皮肤失败！错误码:%d"),GetLastError()),_T("提示"),MB_OK|MB_ICONSTOP);
		}

	}
	free(lpCopyData);
	return 0;
}

LRESULT CIsSvrProxy::OnCopyData(HWND hWnd,PCOPYDATASTRUCT lpCopyData)
{
	if(!(lpCopyData->dwData == CD_CMD_INSTALL_CIT
		|| lpCopyData->dwData == CD_CMD_INSTALL_PLT
		|| lpCopyData->dwData == CD_CMD_INSTALL_SKIN))
	{
		SetMsgHandled(FALSE);
		return 0;
	}
	PCOPYDATASTRUCT pData = (PCOPYDATASTRUCT)malloc(sizeof(COPYDATASTRUCT)+lpCopyData->cbData);
	pData->dwData = lpCopyData->dwData;
	pData->cbData = lpCopyData->cbData;
	pData->lpData = pData+1;
	memcpy(pData->lpData,lpCopyData->lpData,lpCopyData->cbData);
	PostMessage(UM_DELAY_COPYDATA,0,(LPARAM)pData);
	return 1;
}

void CIsSvrProxy::OnSetFocus(CSvrConnection * pConn)
{
	SLOG_INFO("pConn:" << pConn);
	if(m_pFocusConn)
	{
		if(m_pFocusConn!=pConn)
		{
			Param_OnSetFocus param;
			param.bFocus = FALSE;
			param.dwActiveWnd = 0;
			m_pFocusConn->HandleOnSetFocus(param);
		}
	}
	m_pFocusConn = pConn;
}

void CIsSvrProxy::OnKillFocus(CSvrConnection * pConn)
{
	SLOG_INFO("pConn:" << pConn<<"curFocusConn:"<<m_pFocusConn);
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

LRESULT CIsSvrProxy::OnChangeSkin(UINT uMsg, WPARAM wp, LPARAM lp)
{
	SStringT *strSkin =(SStringT*)lp;
	ChangeSkin(*strSkin);
	delete strSkin;
	return 0;
}


BOOL CIsSvrProxy::ChangeSkin(const SStringT & strSkin)
{
	SLOG_INFO("change skin, new skin:" << strSkin.c_str() << " old skin:" << g_SettingsG->strSkin.c_str() << " this:" << this);
	if(g_SettingsG->strSkin != strSkin || g_SettingsG->bEnableDebugSkin)
	{
		SStringT skinPath = strSkin;
		if(!CDataCenter::getSingletonPtr()->GetData().changeSkin(skinPath))
		{
			if(skinPath == g_SettingsG->strDebugSkinPath)
			{//restore to default skin.
				CDataCenter::getSingletonPtr()->GetData().changeSkin(SStringT());
			}else
			{
				return FALSE;
			}
		}
		if(skinPath != g_SettingsG->strDebugSkinPath || g_SettingsG->strDebugSkinPath.IsEmpty())
		{
			g_SettingsG->strSkin=skinPath;
			g_SettingsG->SetModified(true);
		}

		//notify all connection skin changed.
		m_ipcSvr->EnumClient(&CIsSvrProxy::CbNotifyConnectionsSkinChanged,0);
	}
	return TRUE;
}

void CIsSvrProxy::CbNotifyConnectionsSkinChanged(IIpcConnection *pConn, ULONG_PTR data)
{
	CSvrConnection *pSvrConn = (CSvrConnection*)pConn;
	pSvrConn->OnSkinChanged();
}

IServerCore * CIsSvrProxy::GetSvrCore()
{
	return _this->m_pCore;
}

CIsSvrProxy * CIsSvrProxy::GetInstance()
{
	return _this;
}

void CIsSvrProxy::OnDataLoaded()
{
	SetTimer(TIMERID_RECONN,SPAN_RECONN,NULL);
}

void CIsSvrProxy::OnCheckReconn()
{
	HWND hAfter = NULL;
	while(true)
	{
		HWND hFind = FindWindowEx(HWND_MESSAGE,hAfter,CLS_SINSTAR3_IME_WND,NULL);
		if(!hFind)
			break;
		if(!m_ipcSvr->FindConnection((ULONG_PTR)hFind))
		{
			SLOG_INFO("onDataLoaded, notify client to reconnect:"<<hFind);
			::PostMessage(hFind,UM_RECONN,0,0);
		}
		hAfter = hFind;
	}
}

struct BackupParam
{
	BackupParam(SStringT from,SStringT to):strFrom(from),strTo(to){}
	SStringT strFrom;
	SStringT strTo;
};

void CIsSvrProxy::BackupData()
{
	if(m_strDataPath.CompareNoCase(g_SettingsG->szBackupDir)==0)
	{
		SLOG_WARN("backup dir is same as instal dir");
		return;
	}

	if(GetFileAttributes(g_SettingsG->szBackupDir)!=INVALID_FILE_ATTRIBUTES)
	{
		_beginthread(backupProc,NULL,new BackupParam(m_strDataPath,g_SettingsG->szBackupDir));
	}
}

void CIsSvrProxy::backupProc(LPVOID pData)
{
	BackupParam *param = (BackupParam*)pData;
	BackupDir(param->strFrom,param->strTo);
	delete param;
}

static const LPCTSTR KBackupDirs[]={
		_T("server"),_T("skins")
};
int CIsSvrProxy::BackupDir(const SStringT &strFrom,const SStringT & strTo)
{
	SLOG_INFO("backup dir from "<<strFrom<<" to"<<strTo);

	for(int i=0;i<ARRAYSIZE(KBackupDirs);i++)
	{
		TCHAR szSour[MAX_PATH]={0},szDest[MAX_PATH]={0};
		_stprintf(szSour,_T("%s\\%s"),strFrom,KBackupDirs[i]);
		_stprintf(szDest,_T("%s\\%s"),strTo,KBackupDirs[i]);
		SHFILEOPSTRUCT fileOp = { 0 };

		if(GetFileAttributes(szDest)!=INVALID_FILE_ATTRIBUTES)
		{
			fileOp.wFunc = FO_DELETE;
			fileOp.fFlags = FOF_NOCONFIRMATION | FOF_SILENT | FOF_FILESONLY;
			fileOp.pFrom = szDest;
			int nRet = SHFileOperation(&fileOp);
			SLOG_INFO("delete "<<szDest<<" return "<<nRet);
			if(nRet != 0)
				return nRet;
		}
		fileOp.wFunc = FO_COPY;
		fileOp.fFlags = FOF_NOCONFIRMATION | FOF_SILENT|FOF_NOCOPYSECURITYATTRIBS|FOF_NOCONFIRMMKDIR ;
		fileOp.pFrom = szSour;
		fileOp.pTo = szDest;
		int nRet = SHFileOperation(&fileOp);
		SLOG_INFO("backup "<<KBackupDirs[i]<<" return "<<nRet);
		if(nRet != 0)
			return nRet;
	}
	return 0;
}

bool CIsSvrProxy::IsBackupDirValid(const SStringT & strDir)
{
	for(int i=0;i<ARRAYSIZE(KBackupDirs);i++)
	{
		TCHAR szDest[MAX_PATH]={0};
		_stprintf(szDest,_T("%s\\%s"),strDir,KBackupDirs[i]);
		if(GetFileAttributes(szDest)==INVALID_FILE_ATTRIBUTES)
			return false;
	}
	return true;
}


void CIsSvrProxy::CloseBackup()
{
	g_SettingsG->szBackupDir[0]=0;
	KillTimer(TIMERID_BACKUP);
}

ULONG_PTR CIsSvrProxy::OnNewConnection(IIpcHandle * pIpcHandle, IIpcConnection ** ppConn)
{
	CSvrConnection *pConn = new CSvrConnection(pIpcHandle,m_hWnd,this);
	*ppConn = pConn;
	return (ULONG_PTR)pConn->m_hWnd;
}
