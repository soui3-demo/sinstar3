#include "StdAfx.h"
#include "Sinstar3Impl.h"
#include "Utils.h"
#include <initguid.h>
#include "ui/SkinMananger.h"

class CAutoContext
{
public:
	CAutoContext(UINT64 * pCtx, UINT64 value)
	{
		m_pCtx = pCtx;
		*pCtx = value;		
	}

	~CAutoContext()
	{
		*m_pCtx = NULL;
	}

	UINT64 * m_pCtx;
};

const TCHAR * KSinstar3WndName = _T("sinstar3_msg_recv_20180801");

CSinstar3Impl::CSinstar3Impl(ITextService *pTxtSvr,HWND hSvr)
:m_pTxtSvr(pTxtSvr)
,m_pInputWnd(NULL)
,m_pStatusWnd(NULL)
,m_pTipWnd(NULL)
, m_pSpcharWnd(NULL)
,m_curImeContext(NULL)
, m_cmdHandler(this)
, m_hSvr(hSvr)
{
	addEvent(EVENTID(EventSvrNotify));
	addEvent(EVENTID(EventSetSkin));

 	m_pInputWnd = new CInputWnd(this,m_inputState.GetInputContext(),this);
	m_pInputWnd->Create(_T("Sinstar3_Input"));

	m_pStatusWnd = new CStatusWnd(this,this);
	m_pStatusWnd->Create(_T("Sinstar3_Status"));
	m_inputState.SetInputListener(this);
	
	m_pInputWnd->SetAnchorPosition(CDataCenter::getSingleton().GetData().m_ptInput);
	m_pInputWnd->SetFollowCaret(g_SettingsUI->bMouseFollow);

	SLOG_INFO("status:"<<m_pStatusWnd->m_hWnd<<", input:"<<m_pInputWnd->m_hWnd);
	SOUI::CSimpleWnd::Create(KSinstar3WndName,WS_DISABLED|WS_POPUP,WS_EX_TOOLWINDOW,0,0,0,0,HWND_MESSAGE,NULL);
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

	if (m_pTipWnd)
	{
		m_pTipWnd->DestroyWindow();
		m_pTipWnd = NULL;
	}

	if (m_pSpcharWnd)
	{
		m_pSpcharWnd->DestroyWindow();
		m_pSpcharWnd = NULL;
	}

	SStringT strHotKeyFile = SStringT().Format(_T("%s\\data\\hotkey_%s.txt"), CDataCenter::getSingletonPtr()->GetDataPath(), CDataCenter::getSingleton().GetData().m_compInfo.strCompName);
	//加载特定的自定义状态及语句输入状态开关
	SStringT strHotKey = CAccelerator::FormatAccelKey(g_SettingsG->dwHotkeys[HKI_UDMode]);
	WritePrivateProfileString(_T("hotkey"), _T("umode"), strHotKey, strHotKeyFile);
	strHotKey = SStringT((TCHAR)g_SettingsG->bySentMode);
	WritePrivateProfileString(_T("hotkey"), _T("sentence"), strHotKey, strHotKeyFile);

	if (!m_strLoadedFontFile.IsEmpty())
	{
		RemoveFontResourceEx(m_strLoadedFontFile, FR_PRIVATE , NULL);
	}
}


void CSinstar3Impl:: ProcessKeyStoke(UINT64 imeContext,UINT vkCode,LPARAM lParam,BOOL bKeyDown, BYTE byKeyState[256], BOOL *pbEaten)
{
	CAutoContext autoCtx(&m_curImeContext,imeContext);
	*pbEaten = m_inputState.TestKeyDown(vkCode,lParam, byKeyState);
	SLOG_INFO("ProcessKeyStoke, code:" << vkCode << " bKeyDown:" << bKeyDown << " bEaten" << *pbEaten<<" fOpen:"<< m_inputState.m_fOpen);
}

void CSinstar3Impl:: TranslateKey(UINT64 imeContext,UINT vkCode,UINT uScanCode,BOOL bKeyDown, BYTE byKeyState[256], BOOL *pbEaten)
{
	if(!bKeyDown)
	{
		*pbEaten = FALSE;
		return;
	}

	CAutoContext autoCtx(&m_curImeContext,imeContext);

	*pbEaten = TRUE;

	if(m_inputState.HandleKeyDown(vkCode,uScanCode, byKeyState))
	{
		m_pInputWnd->UpdateUI();
	}
	SLOG_INFO("TranslateKey, code:" << vkCode << " bKeyDown:" << bKeyDown << " bEaten" << *pbEaten << " fOpen:" << m_inputState.m_fOpen);
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

void CSinstar3Impl::OnCompositionTerminated(bool bClearCtx)
{
	SLOG_INFO("");
	if(bClearCtx) m_inputState.ClearContext(CPC_ALL);
}

void CSinstar3Impl::OnSetFocus(BOOL bFocus)
{
	SLOG_INFO("GetThreadID="<<GetCurrentThreadId()<<" focus="<<bFocus);

	BOOL bOpen = FALSE;
	UINT64 pImeCtx = m_pTxtSvr->GetImeContext();
	if (pImeCtx)
	{
		bOpen = m_pTxtSvr->GetOpenStatus(pImeCtx);
		m_pTxtSvr->ReleaseImeContext(pImeCtx);
	}

	if (bOpen)
	{
		if (bFocus)
			m_pTxtSvr->SetConversionMode(FullNative);

		m_pStatusWnd->Show(bFocus && !g_SettingsUI->bHideStatus);

		if (bFocus)
		{
			if (m_pTxtSvr->IsCompositing() || m_inputState.IsTempSpell()) m_pInputWnd->Show(TRUE);
		}
		else
		{
			m_pInputWnd->Show(FALSE, FALSE);
			if (m_pSpcharWnd) m_pSpcharWnd->DestroyWindow();
		}
	}
	else
	{
		if (m_pTxtSvr->IsCompositing())
			m_inputState.InputEnd();
		m_pInputWnd->Show(FALSE);
		m_pStatusWnd->Show(FALSE);
		if (m_pSpcharWnd) m_pSpcharWnd->DestroyWindow();
		if (m_pTipWnd) m_pTipWnd->DestroyWindow();
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

void CSinstar3Impl::OnOpenStatusChanged(BOOL bOpen)
{
	m_pStatusWnd->Show(bOpen && !g_SettingsUI->bHideStatus);
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


LRESULT CSinstar3Impl::OnSvrNotify(UINT uMsg, WPARAM wp, LPARAM lp)
{
	PMSGDATA pMsg=ISComm_OnSeverNotify(m_hWnd,wp,lp);
	if(wp==NT_COMPINFO)
	{
		CDataCenter::getSingleton().Lock();
		CMyData &myData = CDataCenter::getSingleton().GetData();
		myData.m_compInfo.SetSvrCompInfo(ISComm_GetCompInfo());

		TCHAR szBuf[100]={0};
		int i=0;

		SStringT strHotKeyFile = SStringT().Format(_T("%s\\data\\hotkey_%s.txt"), CDataCenter::getSingletonPtr()->GetDataPath(),myData.m_compInfo.strCompName);
		//加载特定的自定义状态及语句输入状态开关
		GetPrivateProfileString(_T("hotkey"),_T("umode"),_T(""),szBuf,100,strHotKeyFile);
		g_SettingsG->dwHotkeys[HKI_UDMode]=CAccelerator::TranslateAccelKey(szBuf);
		if(g_SettingsG->dwHotkeys[HKI_UDMode] ==0)
		{
			short sKey = VkKeyScan(myData.m_compInfo.cWild);
			g_SettingsG->dwHotkeys[HKI_UDMode] = LOBYTE(sKey);
		}

		szBuf[0]=0;
		if(GetPrivateProfileString(_T("hotkey"),_T("sentence"),_T(""),szBuf,2,strHotKeyFile))
		{
			g_SettingsG->bySentMode=(BYTE)szBuf[0];
		}else
		{//默认设置为“；”，如果是编码则取消
			if(!myData.m_compInfo.IsCompChar(';'))
				g_SettingsG->bySentMode =';';
			else
				g_SettingsG->bySentMode =0;
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
				m_strLoadedFontFile = CDataCenter::getSingletonPtr()->GetDataPath() + _T("\\data\\") + S_CW2T(strFontFile);
				AddFontResourceEx(m_strLoadedFontFile, FR_PRIVATE , NULL);
			}
			m_pInputWnd->OnFlmInfo(ISComm_GetFlmInfo());
		}
		return 1;
	}
	else if (wp == NT_SERVEREXIT)
	{
		UINT64 pImeCtx = m_pTxtSvr->GetImeContext();
		if (pImeCtx)
		{
			m_pTxtSvr->SetOpenStatus(pImeCtx, FALSE);
			m_pTxtSvr->ReleaseImeContext(pImeCtx);
		}
		EventSvrNotify evt(this);
		evt.wp = wp;
		evt.lp = lp;
		FireEvent(evt);
		return 1;
	}
	else
	{
		return m_inputState.OnSvrNotify((UINT)wp,pMsg)?1:0;
	}
}

LRESULT CSinstar3Impl::OnAsyncCopyData(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SLOG_INFO("begin");

	PCOPYDATASTRUCT pCds = (PCOPYDATASTRUCT)lParam;
	HWND hSender = (HWND)wParam;
	if (pCds->dwData == CMD_CHANGESKIN)
	{//change skin
		SStringA strUtf8((const char *)pCds->lpData, pCds->cbData);
		SStringT strPath = S_CA2T(strUtf8, CP_UTF8);
		SLOG_INFO("skin:"<<strPath);
		ChangeSkin(strPath);
	}
	else if (pCds->dwData == CMD_SYNCUI)
	{
		DWORD dwFlag = *(DWORD*)pCds->lpData;
		m_pStatusWnd->UpdateToggleStatus(dwFlag);
	}

	if (pCds->lpData) free(pCds->lpData);
	free(pCds);
	SLOG_INFO("end");
	return 1;
}

BOOL CSinstar3Impl::OnCopyData(HWND wnd, PCOPYDATASTRUCT pCopyDataStruct)
{
	SLOG_INFO("nLen:"<<pCopyDataStruct->cbData);
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


BOOL CSinstar3Impl::IsCompositing() const
{
	return m_pTxtSvr->IsCompositing();
}

HWND CSinstar3Impl::GetHwnd() const
{
	return m_hWnd;
}

void CSinstar3Impl::OnInputStart()
{
	if(!m_curImeContext) return;
	m_pTxtSvr->StartComposition(m_curImeContext);
}


void CSinstar3Impl::OnInputEnd()
{
	if(!m_curImeContext) return;
	m_pTxtSvr->EndComposition(m_curImeContext);
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
	m_pInputWnd->Hide(bDelay?g_SettingsG->nDelayTime*1000:0);
}

BOOL CSinstar3Impl::SetOpenStatus(BOOL bOpen)
{
	SASSERT(m_curImeContext);
	return m_pTxtSvr->SetOpenStatus(m_curImeContext,bOpen);
}

BOOL CSinstar3Impl::GetOpenStatus() const
{
	SASSERT(m_curImeContext);
	return m_pTxtSvr->GetOpenStatus(m_curImeContext);
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
	if (pWnd->GetWndType() == IME_SPCHAR)
	{
		delete pWnd;
		m_pSpcharWnd = NULL;
	}
	else if (pWnd->GetWndType() == IME_TIP)
	{
		delete pWnd;
		m_pTipWnd = NULL;
	}
}

void CSinstar3Impl::OnInputDelayHide()
{
	m_inputState.ClearContext(CPC_ALL&~CPC_INPUT);
}

extern SComMgr2 * g_ComMgr2;

BOOL CSinstar3Impl::ChangeSkin(const SStringT & strSkin)
{
	SLOG_INFO("skin:" << strSkin);
	CDataCenter::getSingletonPtr()->Lock(); //注意处理多个输入法UI线程之间的同步.
	SLOG_INFO("step1,lock ok");

	//将内置皮肤的skinpool,stylepool,SObjDefAttr保存起来.
	IUiDefInfo *pBulitinUidef =  CDataCenter::getSingletonPtr()->GetData().m_defUiDefine;

	if (CDataCenter::getSingletonPtr()->GetData().m_strSkin != strSkin)
	{
		if (!strSkin.IsEmpty())
		{//加载外部皮肤
			SLOG_INFO("step2, prepare for load skin");
			CAutoRefPtr<IResProvider> pResProvider;
			g_ComMgr2->CreateResProvider_ZIP((IObjRef**)&pResProvider);
			ZIPRES_PARAM param;
			param.ZipFile(GETRENDERFACTORY, strSkin);
			if (!pResProvider->Init((WPARAM)&param, 0))
				return FALSE;

			IUiDefInfo * pUiDef = SUiDef::CreateUiDefInfo2(pResProvider, _T("uidef:xml_init"));
			if (pUiDef->GetSkinPool() || pUiDef->GetStylePool() || pUiDef->GetObjDefAttr())
			{//不允许皮肤中存在全局的skin and style data
				pUiDef->Release();
				return FALSE;
			}

			SLOG_INFO("step3, load external skin ok");

			if (!CDataCenter::getSingletonPtr()->GetData().m_strSkin.IsEmpty())
			{//清除正在使用的外置皮肤。
				SLOG_INFO("step4, remove current in using external skin");

				IResProvider *pLastRes = SApplication::getSingleton().GetTailResProvider();
				SApplication::getSingleton().RemoveResProvider(pLastRes);
			}

			SLOG_INFO("step6, extract skin defined offset");
			CDataCenter::getSingleton().GetData().m_ptSkinOffset = CSkinMananger::ExtractSkinOffset(pResProvider);

			SLOG_INFO("step7, add new skin to sinstar3");
			SApplication::getSingleton().AddResProvider(pResProvider, NULL);
			SLOG_INFO("step8, set uidef");
			pUiDef->GetNamedColor().Merge(pBulitinUidef->GetNamedColor());
			pUiDef->GetNamedString().Merge(pBulitinUidef->GetNamedString());
			pUiDef->GetNamedDimension().Merge(pBulitinUidef->GetNamedDimension());
			SUiDef::getSingleton().SetUiDef(pUiDef);
			pUiDef->Release();
			SLOG_INFO("step9, set external skin ok");
		}
		else if (!CDataCenter::getSingletonPtr()->GetData().m_strSkin.IsEmpty())
		{//清除正在使用的外置皮肤,还原使用系统内置皮肤
			SLOG_INFO("step10, remove external skin");
			IResProvider *pLastRes = SApplication::getSingleton().GetTailResProvider();
			SApplication::getSingleton().RemoveResProvider(pLastRes);

			SLOG_INFO("step12, restore uidef");
			SUiDef::getSingleton().SetUiDef(pBulitinUidef);

			SLOG_INFO("step13, extract builtin skin defined offset");
			IResProvider *pCurRes = SApplication::getSingleton().GetHeadResProvider();
			CDataCenter::getSingleton().GetData().m_ptSkinOffset = CSkinMananger::ExtractSkinOffset(pCurRes);
		}

		SLOG_INFO("step14, save new skin name");
		CDataCenter::getSingletonPtr()->GetData().m_strSkin = strSkin;
	}

	//还原skinpool and stylepool.
	SUiDef::getSingleton().GetUiDef()->SetSkinPool(pBulitinUidef->GetSkinPool());
	SUiDef::getSingleton().GetUiDef()->SetStylePool(pBulitinUidef->GetStylePool());
	SUiDef::getSingleton().GetUiDef()->SetObjDefAttr(pBulitinUidef->GetObjDefAttr());

	SLOG_INFO("step15, notify skin changed");
	//notify skin changed
	EventSetSkin evt(this);
	FireEvent(evt);

	SLOG_INFO("step16, notify skin changed finish");

	CDataCenter::getSingletonPtr()->Unlock();
	SLOG_INFO("step17, unlock");

	return TRUE;
}

void CSinstar3Impl::OpenConfig()
{
	SASSERT(m_hSvr);
	::SendMessage(m_hSvr, WM_COMMAND, R.id.menu_settings, 0);
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

void CSinstar3Impl::ShowTip(LPCTSTR pszTitle, LPCTSTR pszContent)
{
	if (m_pTipWnd == NULL)
	{
		m_pTipWnd = new STipWnd(this);
		m_pTipWnd->Create(_T("Sinstar3_Tip"));
		m_pTipWnd->SetDestroyListener(this, IME_TIP);
	}
	m_pTipWnd->SetTip(pszTitle, pszContent);
}

void CSinstar3Impl::InputSpchar(LPCTSTR pszText)
{
	if (m_pTxtSvr->IsCompositing())
	{
		m_inputState.ClearContext(CPC_ALL);
		m_inputState.InputEnd();
		m_inputState.InputHide();
	}
	if (!m_pTxtSvr->InputStringW(pszText, (int)_tcslen(pszText)))
	{
		CUtils::SoundPlay(_T("error"));
	}
}

void CSinstar3Impl::Broadcast(UINT uCmd, LPVOID pData, DWORD nLen)
{
	COPYDATASTRUCT cds = { 0 };
	cds.dwData = uCmd;
	cds.cbData = nLen;
	cds.lpData = pData;

	SendMessage(WM_COPYDATA, (WPARAM)m_hWnd, (LPARAM)&cds);

	SLOG_INFO("broadcast, nLen:" << nLen);
	HWND hFind = FindWindowEx(HWND_MESSAGE, NULL, NULL, KSinstar3WndName);
	while (hFind)
	{
		if (hFind != m_hWnd)
		{
			::SendMessage(hFind, WM_COPYDATA, (WPARAM)m_hWnd, (LPARAM)&cds);
		}
		hFind = FindWindowEx(HWND_MESSAGE, hFind, NULL, KSinstar3WndName);
	}
}

void CSinstar3Impl::OnInputResult(const SStringT & strResult,const SStringT & strComp/*=SStringT() */)
{
	if(!m_curImeContext) return;
	SStringW strResultW = S_CT2W(strResult);
	SStringW strCompW = S_CT2W(strComp);
	m_pTxtSvr->UpdateResultAndCompositionStringW(m_curImeContext,strResult,strResult.GetLength(),strCompW,strCompW.GetLength());
}

void CSinstar3Impl::OpenInputWnd()
{
	m_pInputWnd->Show(TRUE);
}

void CSinstar3Impl::UpdateInputWnd()
{
	m_pInputWnd->UpdateUI();
}



