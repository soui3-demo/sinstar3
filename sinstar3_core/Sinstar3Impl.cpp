#include "StdAfx.h"
#include "Sinstar3Impl.h"
#include "Utils.h"
#include <initguid.h>
#include "ui/SkinMananger.h"
#include "SouiEnv.h"

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

const TCHAR * KSinstar3WndName = _T("sinstar3_msg_recv_20180801");

CSinstar3Impl::CSinstar3Impl(ITextService *pTxtSvr)
:m_pTxtSvr(pTxtSvr)
,m_pInputWnd(NULL)
,m_pStatusWnd(NULL)
,m_pTipWnd(NULL)
, m_pSpcharWnd(NULL)
,m_pConfig(NULL)
,m_pCurImeContext(NULL)
, m_cmdHandler(this)
{
	theModule->AddRef();

	addEvent(EVENTID(EventSvrNotify));
	addEvent(EVENTID(EventSetSkin));

	m_inputState.GetInputContext()->settings.Load(theModule->GetCfgIni());

 	m_pInputWnd = new CInputWnd(this,m_inputState.GetInputContext(),this);
	m_pStatusWnd = new CStatusWnd(this,this);
	m_pTipWnd = new STipWnd(this);
	m_pStatusWnd->Create(_T("Sinstar3_Status"));
	m_pInputWnd->Create(_T("Sinstar3_Input"));
	m_pTipWnd->Create(_T("sinstar3_tip"));
	m_cmdHandler.SetTipWnd(m_pTipWnd);
	m_inputState.SetInputListener(this);
	
	m_pInputWnd->SetAnchorPosition(CDataCenter::getSingleton().GetData().m_ptInput);
	m_pInputWnd->SetFollowCaret(m_inputState.GetInputContext()->settings.bMouseFollow);

	SLOG_INFO("status:"<<m_pStatusWnd->m_hWnd<<", input:"<<m_pInputWnd->m_hWnd);
	SOUI::CSimpleWnd::Create(KSinstar3WndName,WS_DISABLED|WS_POPUP,WS_EX_TOOLWINDOW,0,0,0,0,HWND_MESSAGE,NULL);
	CUtils::ChangeWindowMessageFilter(WM_COPYDATA, MSGFLT_ADD);
	ISComm_Login(m_hWnd);

}

CSinstar3Impl::~CSinstar3Impl(void)
{
	ISComm_Logout(m_hWnd);
	SOUI::CSimpleWnd::DestroyWindow();
	m_pInputWnd->DestroyWindow();
	m_pStatusWnd->DestroyWindow();
	m_pTipWnd->DestroyWindow();
	delete m_pStatusWnd;
	delete m_pInputWnd;
	delete m_pTipWnd;

	if (m_pConfig)
	{
		m_pConfig->DestroyWindow();
		m_pConfig = NULL;
	}
	if (m_pSpcharWnd)
	{
		m_pSpcharWnd->DestroyWindow();
		m_pSpcharWnd = NULL;
	}
	m_inputState.GetInputContext()->settings.Save(theModule->GetCfgIni());

	if (!m_strLoadedFontFile.IsEmpty())
	{
		RemoveFontResourceEx(m_strLoadedFontFile, FR_PRIVATE , NULL);
	}

	theModule->Release();
}


void CSinstar3Impl:: ProcessKeyStoke(LPVOID lpImeContext,UINT vkCode,LPARAM lParam,BOOL bKeyDown,BOOL *pbEaten)
{
	CAutoContext autoCtx(&m_pCurImeContext,lpImeContext);
	BYTE byKeyState[256];
	GetKeyboardState(byKeyState);
	*pbEaten = m_inputState.TestKeyDown(vkCode,lParam,byKeyState);
}

void CSinstar3Impl:: TranslateKey(LPVOID lpImeContext,UINT vkCode,UINT uScanCode,BOOL bKeyDown,BOOL *pbEaten)
{
	if(!bKeyDown)
	{
		*pbEaten = FALSE;
		return;
	}

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
	m_inputState.OnImeSelect(bSelect);
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
	SLOG_INFO("");
}

void CSinstar3Impl::OnCompositionChanged()
{
}

void CSinstar3Impl::OnCompositionTerminated()
{
	SLOG_INFO("");
	m_inputState.ClearContext(CPC_ALL);
}

void CSinstar3Impl::OnSetFocus(BOOL bFocus)
{
	SLOG_INFO("GetThreadID="<<GetCurrentThreadId()<<" focus="<<bFocus);
	if(bFocus) m_pTxtSvr->SetConversionMode(FullNative);
	m_pStatusWnd->Show(bFocus && !m_inputState.GetInputContext()->settings.bHideStatus);
	
	if (bFocus)
	{
		if (m_inputState.IsTypeing()) m_pInputWnd->Show(TRUE);
	}
	else
	{
		m_pInputWnd->Show(FALSE,FALSE);
		if (m_pSpcharWnd) m_pSpcharWnd->DestroyWindow();
	}
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
	m_pStatusWnd->Show(bOpen && !m_inputState.GetInputContext()->settings.bHideStatus);
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
	if(wp==NT_COMPINFO)
	{
		CDataCenter::getSingleton().Lock();
		CMyData &myData = CDataCenter::getSingleton().GetData();
		myData.m_compInfo.SetSvrCompInfo(ISComm_GetCompInfo());

		TCHAR szBuf[10]={0};
		int i=0;

		SStringT strHotKeyFile = SStringT().Format(_T("%s\\hotkey_%s.txt"),theModule->GetDataPath(),myData.m_compInfo.strCompName);
		//加载特定的自定义状态及语句输入状态开关
		GetPrivateProfileString(_T("hotkey"),_T("umode"),_T(""),szBuf,2,strHotKeyFile);
		g_SettingsG.hkUserDefSwitch=szBuf[0];
		if(g_SettingsG.hkUserDefSwitch==0)
		{
			g_SettingsG.hkUserDefSwitch=myData.m_compInfo.cWild;
		}

		g_SettingsG.bFastUMode=1;
		if(GetPrivateProfileString(_T("settings"),_T("FastUMode"),NULL,szBuf,10,strHotKeyFile))
			g_SettingsG.bFastUMode=_ttoi(szBuf);
		else if(g_SettingsG.hkUserDefSwitch<'a' || g_SettingsG.hkUserDefSwitch>'z') //不是字母，默认关闭
			g_SettingsG.bFastUMode=0;

		szBuf[0]=0;
		if(GetPrivateProfileString(_T("hotkey"),_T("sentence"),_T(""),szBuf,2,strHotKeyFile))
		{
			g_SettingsG.hkSentSwitch=szBuf[0];
		}else
		{//默认设置为“；”，如果是编码则取消
			if(!myData.m_compInfo.IsCompChar(';'))
				g_SettingsG.hkSentSwitch=';';
			else
				g_SettingsG.hkSentSwitch=0;
		}

		CDataCenter::getSingleton().Unlock();

		EventSvrNotify evt(this);
		evt.wp = wp;
		evt.lp = lp;
		FireEvent(evt);
		return 1;
	}
	else if (wp == NT_FLMINFO)
	{
		LOGFONT lf = { 0 };
		if (m_strLoadedFontFile)
		{
			RemoveFontResourceEx(m_strLoadedFontFile, FR_PRIVATE, NULL);
			m_strLoadedFontFile.Empty();
		}
		if (ISComm_GetFlmInfo()->szAddFont[0])
		{//需要特殊字体
			SLOG_INFO("NT_FLMINFO,font:" << ISComm_GetFlmInfo()->szAddFont);

			SStringW strFontFile = CDataCenter::getSingleton().GetData().getFontFile(S_CA2W(ISComm_GetFlmInfo()->szAddFont));
			if (!strFontFile.IsEmpty())
			{
				m_strLoadedFontFile = theModule->GetDataPath() + _T("\\") + S_CW2T(strFontFile);
				AddFontResourceEx(m_strLoadedFontFile, FR_PRIVATE , NULL);
			}
			m_pInputWnd->OnFlmInfo(ISComm_GetFlmInfo());
		}
		return 1;
	}
	else
	{
		return m_inputState.OnSvrNotify((UINT)wp,pMsg)?1:0;
	}
}

LRESULT CSinstar3Impl::OnAsyncCopyData(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PCOPYDATASTRUCT pCds = (PCOPYDATASTRUCT)lParam;
	HWND hSender = (HWND)wParam;
	if (pCds->dwData == CMD_CHANGESKIN)
	{//change skin
		SStringA strUtf8((const char *)pCds->lpData, pCds->cbData);
		SStringT strPath = S_CA2T(strUtf8, CP_UTF8);
		ChangeSkin(strPath);
	}

	if (pCds->lpData) free(pCds->lpData);
	free(pCds);

	return LRESULT();
}

BOOL CSinstar3Impl::OnCopyData(HWND wnd, PCOPYDATASTRUCT pCopyDataStruct)
{
	PCOPYDATASTRUCT pCds = (PCOPYDATASTRUCT)malloc(sizeof(COPYDATASTRUCT));
	pCds->dwData = pCopyDataStruct->dwData;
	pCds->cbData = pCopyDataStruct->cbData;
	if (pCds->cbData)
	{
		pCds->lpData = malloc(pCds->cbData);
		memcpy(pCds->lpData, pCopyDataStruct->lpData, pCds->cbData);
	}
	else
	{
		pCds->lpData = NULL;
	}
	PostMessage(UM_ASYNC_COPYDATA, (WPARAM)wnd, (LPARAM)pCds);
	return TRUE;
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


void CSinstar3Impl::OnInputEnd()
{
	if(!m_pCurImeContext) return;
	m_pTxtSvr->EndComposition(m_pCurImeContext);
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

void CSinstar3Impl::CloseInputWnd(BOOL bDelay)
{
	m_pInputWnd->Hide(bDelay?5000:0);
}

BOOL CSinstar3Impl::SetOpenStatus(BOOL bOpen)
{
	SASSERT(m_pCurImeContext);
	return m_pTxtSvr->SetOpenStatus(m_pCurImeContext,bOpen);
}

BOOL CSinstar3Impl::GetOpenStatus() const
{
	SASSERT(m_pCurImeContext);
	return m_pTxtSvr->GetOpenStatus(m_pCurImeContext);
}

void CSinstar3Impl::OnCommand(WORD cmd, LPARAM lp)
{
	SendMessage(WM_COMMAND, MAKELONG(0,cmd), lp);
}

InputContext * CSinstar3Impl::GetInputContext()
{
	return m_inputState.GetInputContext();
}

void CSinstar3Impl::OnSkinAwareWndDestroy(CSkinAwareWnd * pWnd)
{
	if (pWnd->GetWndType() == IME_CONFIG)
	{
		delete pWnd;
		m_pConfig = NULL;
	}
	else if (pWnd->GetWndType() == IME_SPCHAR)
	{
		delete pWnd;
		m_pSpcharWnd = NULL;
	}
}

void CSinstar3Impl::OnInputDelayHide()
{
	m_inputState.ClearContext(CPC_ALL);
}

BOOL CSinstar3Impl::ChangeSkin(const SStringT & strSkin)
{
	CDataCenter::getSingletonPtr()->Lock(); //注意处理多个输入法UI线程之间的同步.

	if (CDataCenter::getSingletonPtr()->GetData().m_strSkin != strSkin)
	{
		if (!strSkin.IsEmpty())
		{//加载外部皮肤
			CAutoRefPtr<IResProvider> pResProvider;
			CSouiEnv::getSingleton().theComMgr()->CreateResProvider_ZIP((IObjRef**)&pResProvider);
			ZIPRES_PARAM param;
			param.ZipFile(GETRENDERFACTORY, strSkin);
			if (!pResProvider->Init((WPARAM)&param, 0))
				return FALSE;

			IUiDefInfo * pUiDef = SUiDef::getSingleton().CreateUiDefInfo(pResProvider, _T("uidef:xml_init"));
			if (pUiDef->GetSkinPool())
			{//不允许皮肤中存在全局的skin数据
				pUiDef->Release();
				return FALSE;
			}

			if (!CDataCenter::getSingletonPtr()->GetData().m_strSkin.IsEmpty())
			{//清除正在使用的外置皮肤。
				IResProvider *pLastRes = SApplication::getSingleton().GetTailResProvider();
				SApplication::getSingleton().RemoveResProvider(pLastRes);
				IUiDefInfo *pUiDefInfo = SUiDef::getSingleton().GetUiDef();

				SStylePoolMgr::getSingleton().PopStylePool(pUiDefInfo->GetStylePool());
			}

			CDataCenter::getSingleton().GetData().m_ptSkinOffset = CSkinMananger::ExtractSkinOffset(pResProvider);

			SApplication::getSingleton().AddResProvider(pResProvider, NULL);
			SUiDef::getSingleton().SetUiDef(pUiDef);
			pUiDef->Release();

		}
		else if (!CDataCenter::getSingletonPtr()->GetData().m_strSkin.IsEmpty())
		{//清除正在使用的外置皮肤,还原使用系统内置皮肤
			IResProvider *pLastRes = SApplication::getSingleton().GetTailResProvider();
			SApplication::getSingleton().RemoveResProvider(pLastRes);
			IUiDefInfo *pUiDefInfo = SUiDef::getSingleton().GetUiDef();

			SStylePoolMgr::getSingleton().PopStylePool(pUiDefInfo->GetStylePool());

			IResProvider *pCurRes = SApplication::getSingleton().GetTailResProvider();
			IUiDefInfo * pUiDef = SUiDef::getSingleton().CreateUiDefInfo(pCurRes, _T("uidef:xml_init"));
			SUiDef::getSingleton().SetUiDef(pUiDef);
			pUiDef->Release();

			CDataCenter::getSingleton().GetData().m_ptSkinOffset = CSkinMananger::ExtractSkinOffset(pCurRes);
		}

		CDataCenter::getSingletonPtr()->GetData().m_strSkin = strSkin;
	}

	//notify skin changed
	EventSetSkin evt(this);
	FireEvent(evt);

	CDataCenter::getSingletonPtr()->Unlock();

	return TRUE;
}

void CSinstar3Impl::OpenConfig()
{
	if (m_pConfig == NULL)
	{
		m_pConfig = new CConfigDlg(this);
		m_pConfig->SetDestroyListener(this, IME_CONFIG);
		m_pConfig->Create(_T("Config"),NULL);
		m_pConfig->CenterWindow(GetActiveWindow());
	}
	m_pConfig->SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
}

void CSinstar3Impl::OpenSpchar()
{
	if (m_pSpcharWnd == NULL)
	{
		m_pSpcharWnd = new CSpCharWnd(this,this);
		m_pSpcharWnd->SetDestroyListener(this, IME_SPCHAR);
		m_pSpcharWnd->Create(_T("SpcharWnd"), NULL);
	}
	m_pSpcharWnd->SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOACTIVATE);
}

void CSinstar3Impl::InputSpchar(LPCTSTR pszText)
{
	if (m_inputState.IsTypeing())
	{
		m_inputState.ClearContext(CPC_ALL);
		m_inputState.InputEnd();
		m_inputState.InputHide();
	}
	LPVOID pImeCtx = m_pTxtSvr->GetImeContext();
	if (!pImeCtx)
	{
		CUtils::SoundPlay(_T("error"));
	}
	else
	{
		SStringW strTxt = S_CT2W(pszText);
		m_pTxtSvr->UpdateResultAndCompositionStringW(pImeCtx, strTxt, strTxt.GetLength(), NULL, 0);
		m_pTxtSvr->ReleaseImeContext(pImeCtx);
	}
}

void CSinstar3Impl::Broadcast(UINT uCmd, LPVOID pData, DWORD nLen)
{
	COPYDATASTRUCT cds = { 0 };
	cds.dwData = uCmd;
	cds.cbData = nLen;
	cds.lpData = pData;

	SendMessage(WM_COPYDATA, (WPARAM)m_hWnd, (LPARAM)&cds);

	HWND hFind = FindWindowEx(HWND_MESSAGE, NULL, SINSTART3_WNDCLASS, KSinstar3WndName);
	while (hFind)
	{
		if (hFind != m_hWnd)
		{
			::SendMessage(hFind, WM_COPYDATA, (WPARAM)m_hWnd, (LPARAM)&cds);
		}
		hFind = FindWindowEx(HWND_MESSAGE, hFind, SINSTART3_WNDCLASS, KSinstar3WndName);
	}
}

void CSinstar3Impl::OnInputResult(const SStringT & strResult,const SStringT & strComp/*=SStringT() */)
{
	if(!m_pCurImeContext) return;
	SStringW strResultW = S_CT2W(strResult);
	SStringW strCompW = S_CT2W(strComp);
	m_pTxtSvr->UpdateResultAndCompositionStringW(m_pCurImeContext,strResult,strResult.GetLength(),strCompW,strCompW.GetLength());
}

void CSinstar3Impl::OpenInputWnd()
{
	m_pInputWnd->Show(TRUE);
}

void CSinstar3Impl::UpdateInputWnd()
{
	m_pInputWnd->UpdateUI();
}



