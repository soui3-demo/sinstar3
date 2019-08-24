#include "StdAfx.h"
#include "StatusWnd.h"
#include <helper/SMenuEx.h>

#include "../Utils.h"
#include "../InputState.h"
#include "../ShellExecuteMonitor.h"
#include "../../../include/FileHelper.h"
#include <ShellAPI.h>

namespace SOUI
{
	class CDenoteDlg: public SHostWnd, public SDpiHandler<CDenoteDlg>
	{
	public:
		CDenoteDlg():SHostWnd(UIRES.LAYOUT.dlg_donate){}

		virtual void OnFinalMessage(HWND hWnd){
			__super::OnFinalMessage(hWnd);
			delete this;
		}

		void OnClose()
		{
			DestroyWindow();
		}
		EVENT_MAP_BEGIN()
			EVENT_ID_COMMAND(IDCANCEL,OnClose)
		EVENT_MAP_END()

	protected:
		BEGIN_MSG_MAP_EX(CDenoteDlg)
			CHAIN_MSG_MAP(SDpiHandler<CDenoteDlg>)
			CHAIN_MSG_MAP(SHostWnd)
		END_MSG_MAP()
	};

	static int PopupMenuEndID(int nStart)
	{
		if (nStart % 100 == 0) return nStart + 100;
		else return (nStart + 99) / 100 * 100;
	}

	CStatusWnd::CStatusWnd(SEventSet *pEvtSets, ICmdListener *pListener)
		:CImeWnd(pEvtSets,UIRES.LAYOUT.wnd_status_bar)
		, m_pCmdListener(pListener)
		,m_anchorMode(AMH_NULL|AMV_NULL)
	{
	}

	CStatusWnd::~CStatusWnd(void)
	{
	}


	HWND CStatusWnd::Create()
	{
		HWND hWnd = __super::Create();
		if(hWnd)
		{
			CPoint pt = g_SettingsG->ptStatus;
			if(pt.x==-1 || pt.y==-1)
			{
				m_anchorMode = AMH_RIGHT|AMV_BOTTOM;
			}else
			{
				SetWindowPos(HWND_TOPMOST,pt.x,pt.y,0,0,SWP_NOACTIVATE|SWP_NOSIZE);
			}
			UpdateUI();
		}
		return hWnd;
	}

	int CStatusWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		return OnRecreateUI(lpCreateStruct);
	}

	void CStatusWnd::OnReposition(CPoint pt)
	{
		g_SettingsG->ptStatus = pt;
		g_SettingsG->SetModified(true);
		UpdateAnchorMode();
	}

	int CStatusWnd::OnRecreateUI(LPCREATESTRUCT lpCreateStruct)
	{
		int nRet = __super::OnRecreateUI(lpCreateStruct);
		if (nRet != 0) return nRet;

		UpdateUI();
		return 0;
	}

	void CStatusWnd::UpdateUI()
	{
		UpdateToggleStatus(BTN_ALL, TRUE);
		UpdateCompInfo();
	}

	void CStatusWnd::UpdateAnchorMode()
	{
		CRect rcWnd;
		GetNative()->GetWindowRect(&rcWnd);
		CRect rcWorkArea;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);

		m_anchorMode = AMH_NULL|AMV_NULL;
		if(rcWnd.left == rcWorkArea.left)
			m_anchorMode |= AMH_LEFT;
		else if(rcWnd.right == rcWorkArea.right)
			m_anchorMode |= AMH_RIGHT;
		if(rcWnd.top == rcWorkArea.top)
			m_anchorMode |= AMV_TOP;
		else if(rcWnd.bottom == rcWorkArea.bottom)
			m_anchorMode |= AMV_BOTTOM;
	}

	bool CStatusWnd::onRootResize(EventArgs *e)
	{
		EventSwndSize *e2 = sobj_cast<EventSwndSize>(e);
		if(m_bResizing) return true;

		CPoint pt = g_SettingsG->ptStatus;
		CRect rcTo=CRect(pt,e2->szWnd);

		CRect rcWorkArea;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
		if(m_anchorMode & AMH_LEFT)
			rcTo.MoveToX(rcWorkArea.left);
		else if(m_anchorMode & AMH_RIGHT)
			rcTo.MoveToX(rcWorkArea.right-rcTo.Width());
		if(m_anchorMode & AMV_TOP)
			rcTo.MoveToY(rcWorkArea.top);
		else if(m_anchorMode & AMV_BOTTOM)
			rcTo.MoveToY(rcWorkArea.bottom-rcTo.Height());

		if(rcTo.left <rcWorkArea.left)
			rcWorkArea.MoveToX(rcWorkArea.left);
		if(rcTo.top < rcWorkArea.top)
			rcWorkArea.MoveToY(rcWorkArea.top);
		if(rcTo.right>rcWorkArea.right)
			rcTo.MoveToX(rcWorkArea.right-e2->szWnd.cx);
		if(rcTo.bottom>rcWorkArea.bottom)
			rcTo.MoveToY(rcWorkArea.bottom-e2->szWnd.cy);

		m_nAutoSizing++;//to prevent set SHostWnd::m_szAppSetted.
		SetWindowPos(NULL,rcTo.left,rcTo.top,rcTo.Width(),rcTo.Height(),SWP_NOZORDER|SWP_NOACTIVATE);
		m_nAutoSizing--;
		UpdateAnchorMode();
		g_SettingsG->ptStatus = rcTo.TopLeft();
		g_SettingsG->SetModified(true);
		return true;
	}

	void CStatusWnd::OnRButtonUp(UINT nFlags,CPoint pt)
	{
		const MSG * pMsg = GetCurrentMessage();
		SHostWnd::OnMouseEvent(pMsg->message,pMsg->wParam,pMsg->lParam);

		OnMenuClick();
	}

	void CStatusWnd::OnInitMenuPopup(HMENU menuPopup, UINT nIndex, BOOL bSysMenu)
	{
		SMenu smenuPopup(menuPopup);

		switch (::GetMenuContextHelpId(menuPopup))
		{
		case 1:
			{//main menu
				smenuPopup.CheckMenuItem(R.id.switch_follow_caret, MF_BYCOMMAND | g_SettingsUI->bMouseFollow ? MF_CHECKED : 0);
				smenuPopup.CheckMenuItem(R.id.switch_hide_statusbar, MF_BYCOMMAND | g_SettingsUI->bHideStatus ? MF_CHECKED : 0);
				smenuPopup.CheckMenuItem(R.id.switch_input_big5, MF_BYCOMMAND | g_SettingsUI->bInputBig5 ? MF_CHECKED : 0);
				smenuPopup.CheckMenuItem(R.id.switch_read_input, MF_BYCOMMAND | g_SettingsUI->bSound ? MF_CHECKED : 0);
				smenuPopup.CheckMenuItem(R.id.switch_record_input, MF_BYCOMMAND | g_SettingsUI->bRecord ? MF_CHECKED : 0);
				smenuPopup.CheckMenuItem(R.id.switch_word_input, MF_BYCOMMAND | g_SettingsUI->bEnglish ? MF_CHECKED : 0);
				smenuPopup.CheckMenuItem(R.id.switch_filter_gbk, MF_BYCOMMAND | g_SettingsUI->bFilterGbk ? MF_CHECKED : 0);
				break;
			}
		case 2:
			{//skin
				SStringT strCurSkin = g_SettingsG->strSkin;
				if (strCurSkin.IsEmpty())
				{
					smenuPopup.CheckMenuItem(R.id.skin_def, MF_BYCOMMAND|MF_CHECKED);
				}
				m_skinManager.InitSkinMenu(menuPopup, CDataCenter::getSingletonPtr()->GetDataPath() + _T("\\skins"), R.id.skin_def, strCurSkin);
				break;
			}
		case 4://comp select
			{
				const SArray<CNameTypePair> &comps = CDataCenter::getSingleton().UpdateCompList();
				int idStart = R.id.comp_install;
				int iSelComp = CDataCenter::getSingleton().GetSelectCompIndex();
				for (int i = 0; i < (int)comps.GetCount(); i++)
				{
					SStringA strText = SStringA().Format("%s[%s]", comps[i].strName, comps[i].strType);
					UINT flag = MF_STRING;
					if (iSelComp == i) flag |= MF_CHECKED;
					smenuPopup.AppendMenu( flag, idStart + i+1, S_CA2T(strText, CP_GB));
				}
				break;
			}
		case 5://blend input
			{
				BOOL bCe2 = 0, bCe3 = 0, bCa4 = 0;
				ISComm_Bldsp_Get(&bCe2, &bCe3, &bCa4);
				smenuPopup.CheckMenuItem(R.id.spell_one,MF_BYCOMMAND | (g_SettingsG->bBlendSpWord ? MF_CHECKED : 0));
				smenuPopup.CheckMenuItem(R.id.spell_two, MF_BYCOMMAND | (bCe2 ? MF_CHECKED : 0));
				smenuPopup.CheckMenuItem(R.id.spell_three, MF_BYCOMMAND | (bCe3 ? MF_CHECKED : 0));
				smenuPopup.CheckMenuItem(R.id.spell_all, MF_BYCOMMAND | (bCa4 ? MF_CHECKED : 0));
				smenuPopup.CheckMenuItem(R.id.userdef, MF_BYCOMMAND | (g_SettingsG->bBlendUD ? MF_CHECKED : 0));

				break;
			}
		case 6://svr data manager
			{
				smenuPopup.CheckMenuItem(R.id.svr_showicon, MF_BYCOMMAND | ISComm_SvrTray_Get() ? MF_CHECKED : 0);
				break;
			}
		case 7://tools
			{
				m_toolManager.InitToolMenu(menuPopup, CDataCenter::getSingletonPtr()->GetDataPath() + _T("\\tools"), R.id.menu_tool_base);
				break;
			}
		}
		::SetMenuContextHelpId(menuPopup,::GetMenuContextHelpId(menuPopup)+10000);
	}


	void CStatusWnd::OnBtnExtend()
	{
		g_SettingsUI->bFullStatus = TRUE;
		g_SettingsUI->SetModified(true);
		SWindow *pStatus=FindChildByID(R.id.status_extend);
		if(pStatus) 
		{
			pStatus->SetVisible(TRUE, TRUE);
			UpdateLayout();
		}
		m_pCmdListener->OnCommand(CMD_SYNCUI, BTN_STATUSMODE);
	}

	void CStatusWnd::OnBtnShrink()
	{
		g_SettingsUI->bFullStatus = FALSE;
		g_SettingsUI->SetModified(true);
		SWindow *pStatus=FindChildByID(R.id.status_shrink);
		if(pStatus) 
		{
			pStatus->SetVisible(TRUE,TRUE);
			UpdateLayout();
		}
		m_pCmdListener->OnCommand(CMD_SYNCUI, BTN_STATUSMODE);
	}

	void CStatusWnd::UpdateCompInfo2(SWindow *pParent)
	{
		SWindow *pText = pParent->FindChildByID(R.id.txt_comp);
		SFlagView * pFlagView = pParent->FindChildByID2<SFlagView>(R.id.img_logo);
		if (m_pCmdListener->GetInputContext()->compMode == IM_SHAPECODE)
		{
			if (pText) pText->SetWindowText(CDataCenter::getSingletonPtr()->GetData().m_compInfo.strCompName);
			if (pFlagView)
			{
				pFlagView->ShowSpellFlag(FALSE);
				pFlagView->SetImeFlagData(ISComm_GetCompInfo()->pImeFlagData);
			}
		}
		else
		{
			if (pFlagView) pFlagView->ShowSpellFlag(TRUE);
			if (pText)
			{
				if (g_SettingsG->compMode == IM_SHAPECODE)
					pText->SetWindowText(_T("临时拼音"));
				else
					pText->SetWindowText(_T("启程拼音"));
			}
		}
	}

	void CStatusWnd::ShowServerExit()
	{
		{
			SWindow *pStatus = FindChildByID(R.id.status_shrink);
			if(pStatus)
			{
				SWindow *pText = pStatus->FindChildByID(R.id.txt_comp);
				if (pText) pText->SetWindowText(_T("服务退出"));
			}
		}
		{
			SWindow *pStatus = FindChildByID(R.id.status_extend);
			if(pStatus)
			{
				SWindow *pText = pStatus->FindChildByID(R.id.txt_comp);
				if(pText) pText->SetWindowText(_T("服务退出"));
			}
		}
	}
	void CStatusWnd::UpdateCompInfo()
	{
		{
			SWindow *pStatus = FindChildByID(R.id.status_shrink);
			SASSERT(pStatus);
			UpdateCompInfo2(pStatus);
		}
		{
			SWindow *pStatus = FindChildByID(R.id.status_extend);
			if(pStatus)
				UpdateCompInfo2(pStatus);
		}
	}


	void CStatusWnd::UpdateMode()
	{
		if (g_SettingsUI->bFullStatus)
		{
			SWindow *pStatus = FindChildByID(R.id.status_extend);
			if(pStatus)
				pStatus->SetVisible(TRUE, TRUE);
			else
				FindChildByID(R.id.status_shrink)->SetVisible(TRUE, TRUE);
		}
		else
		{
			SWindow *pStatus = FindChildByID(R.id.status_shrink);
			if(pStatus)
				pStatus->SetVisible(TRUE, TRUE);
			else
				FindChildByID(R.id.status_extend)->SetVisible(TRUE, TRUE);
		}
	}


	bool CStatusWnd::SwitchToggle(int nID, BOOL  bToggle)
	{
		bool bRet=false;
		SWindow *pShrink = FindChildByID(R.id.status_shrink);
		SWindow *pExtend = FindChildByID(R.id.status_extend);
		if(pShrink)
		{
			SToggle *pToggle = pShrink->FindChildByID2<SToggle>(nID);
			if(pToggle) 
			{
				pToggle->SetToggle(bToggle);
				if(pToggle->IsVisible(TRUE))
					bRet = true;
			}
		}
		if(pExtend)
		{
			SToggle *pToggle = pExtend->FindChildByID2<SToggle>(nID);
			if(pToggle) 
			{
				pToggle->SetToggle(bToggle);
				if(pToggle->IsVisible(TRUE))
					bRet = true;
			}
		}
		return bRet;
	}

	void CStatusWnd::UpdateToggleStatus(DWORD flags, BOOL bInit)
	{
		if (flags & BTN_STATUSMODE)
		{
			UpdateMode();
		}
		if(flags & BTN_CHARMODE){
			bool bUpdated = SwitchToggle(R.id.btn_charmode,g_SettingsUI->bCharMode);
			if (m_pCmdListener && !bInit && !bUpdated)
			{
				TIPINFO ti(_T("标点模式改变"));
				ti.strTip.Format(_T("当前标点:%s"), g_SettingsUI->bCharMode ? _T("中文标点") : _T("英文标点"));
				m_pCmdListener->OnCommand(CMD_SHOWTIP,(LPARAM)&ti);
			}
		}
		if(flags & BTN_SOUND){
			bool bUpdated = SwitchToggle(R.id.btn_sound,!g_SettingsUI->bSound);
			if (m_pCmdListener && !bInit && !bUpdated)
			{
				TIPINFO ti(_T("语音较对改变:"));
				ti.strTip.Format(_T("当前语音较对:%s"), g_SettingsUI->bSound ? _T("打开") : _T("关闭"));
				m_pCmdListener->OnCommand(CMD_SHOWTIP, (LPARAM)&ti);
			}
		}
		if (flags & BTN_RECORD) {
			bool bUpdated = SwitchToggle(R.id.btn_record,!g_SettingsUI->bRecord);
			if (m_pCmdListener && !bInit && !bUpdated)
			{
				TIPINFO ti(_T("记忆输入改变:"));
				ti.strTip.Format(_T("当前记忆状态:%s"), g_SettingsUI->bRecord ? _T("启用") : _T("关闭"));
				m_pCmdListener->OnCommand(CMD_SHOWTIP, (LPARAM)&ti);
			}

		}
		if (flags & BTN_ENGLISHMODE)
		{
			bool bUpdated = SwitchToggle(R.id.btn_english,!g_SettingsUI->bEnglish);
			if (m_pCmdListener && !bInit && !bUpdated)
			{
				TIPINFO ti(_T("单词输入改变:"));
				ti.strTip.Format(_T("当前单词输入状态:%s"), g_SettingsUI->bEnglish ? _T("启用") : _T("关闭"));
				m_pCmdListener->OnCommand(CMD_SHOWTIP, (LPARAM)&ti);
			}
		}
		if (flags & BTN_FILTERGBK)
		{
			bool bUpdated = SwitchToggle(R.id.btn_filter_gbk,!g_SettingsUI->bFilterGbk);
			if (m_pCmdListener && !bInit && !bUpdated)
			{
				TIPINFO ti(_T("GBK过滤策略改变:"));
				ti.strTip.Format(_T("当前GBK过滤:%s"), g_SettingsUI->bFilterGbk ? _T("启用") : _T("关闭"));
				m_pCmdListener->OnCommand(CMD_SHOWTIP, (LPARAM)&ti);
			}

		}
	}


	void CStatusWnd::OnSvrNotify(EventArgs *e)
	{
		if(!IsWindow())
			return;

		EventSvrNotify *e2 = sobj_cast<EventSvrNotify>(e);
		if(e2->wp == NT_COMPINFO)
		{
			UpdateCompInfo();
		}
		else if (e2->wp == NT_SERVEREXIT)
		{
			ShowServerExit();
		}
	}

	void CStatusWnd::OnSwitchCharMode(EventArgs *e)
	{
		SToggle * toggle = sobj_cast<SToggle>(e->sender);
		if(toggle)
		{
			g_SettingsUI->bCharMode = toggle->GetToggle();
			g_SettingsUI->SetModified(true);
			m_pCmdListener->OnCommand(CMD_SYNCUI, BTN_CHARMODE);
		}
	}

	void CStatusWnd::OnSwitchRecord(EventArgs *e)
	{
		SToggle * toggle = sobj_cast<SToggle>(e->sender);
		if(toggle)
		{
			g_SettingsUI->bRecord = !toggle->GetToggle();
			g_SettingsUI->SetModified(true);
			m_pCmdListener->OnCommand(CMD_SYNCUI, BTN_RECORD);
		}
	}

	void CStatusWnd::OnSwitchSound(EventArgs *e)
	{
		SToggle * toggle = sobj_cast<SToggle>(e->sender);
		if(toggle)
		{
			g_SettingsUI->bSound = !toggle->GetToggle();
			g_SettingsUI->SetModified(true);
			m_pCmdListener->OnCommand(CMD_SYNCUI, BTN_SOUND);
		}

	}

	void CStatusWnd::OnSwitchEnglish(EventArgs * e)
	{
		SToggle * toggle = sobj_cast<SToggle>(e->sender);
		if (toggle)
		{
			g_SettingsUI->bEnglish = !toggle->GetToggle();
			g_SettingsUI->SetModified(true);
			m_pCmdListener->OnCommand(CMD_SYNCUI, BTN_ENGLISHMODE);
		}
	}

	void CStatusWnd::OnSwitchFilterGbk(EventArgs * e)
	{
		SToggle * toggle = sobj_cast<SToggle>(e->sender);
		if (toggle)
		{
			g_SettingsUI->bFilterGbk = !toggle->GetToggle();
			g_SettingsUI->SetModified(true);
			m_pCmdListener->OnCommand(CMD_SYNCUI, BTN_FILTERGBK);
		}
	}

	
	void CStatusWnd::OnUpdateBtnTooltip(EventArgs *e)
	{
		EventSwndUpdateTooltip *e2 = sobj_cast<EventSwndUpdateTooltip>(e);
		SASSERT(e2);
		SStringT strAccel;
		switch (e2->idFrom)
		{
		case R.id.btn_charmode:
			e2->bUpdated = TRUE;
			strAccel = SAccelerator::FormatAccelKey(g_SettingsG->dwHotkeys[HKI_CharMode]);
			e2->strToolTip = SStringT().Format(_T("标点模式:%s"), g_SettingsUI->bCharMode? _T("中文"):_T("英文"));
			break;
		case R.id.btn_make_phrase:
			e2->bUpdated = TRUE;
			strAccel = SAccelerator::FormatAccelKey(g_SettingsG->dwHotkeys[HKI_MakePhrase]);
			e2->strToolTip = _T("剪贴板造词");
			break;
		case R.id.btn_record:
			e2->bUpdated = TRUE;
			strAccel = SAccelerator::FormatAccelKey(g_SettingsG->dwHotkeys[HKI_Record]);
			e2->strToolTip = SStringT().Format(_T("记录输入状态:%s"), g_SettingsUI->bRecord ? _T("启用") : _T("禁用"));
			break;
		case R.id.btn_sound:
			e2->bUpdated = TRUE;
			strAccel = SAccelerator::FormatAccelKey(g_SettingsG->dwHotkeys[HKI_TTS]);
			e2->strToolTip = SStringT().Format(_T("语音较对:%s"), g_SettingsUI->bSound ? _T("启用") : _T("禁用"));
			break;
		case R.id.btn_english:
			e2->bUpdated = TRUE;
			strAccel = SAccelerator::FormatAccelKey(g_SettingsG->dwHotkeys[HKI_EnSwitch]);
			e2->strToolTip = SStringT().Format(_T("单词补全:%s"), g_SettingsUI->bEnglish ? _T("启用") : _T("禁用"));
			break;
		case R.id.btn_query:
			e2->bUpdated = TRUE;
			strAccel = SAccelerator::FormatAccelKey(g_SettingsG->dwHotkeys[HKI_Query]);
			e2->strToolTip = _T("编码反查");
			break;
		case R.id.btn_filter_gbk:
			e2->bUpdated = TRUE;
			strAccel = SAccelerator::FormatAccelKey(g_SettingsG->dwHotkeys[HKI_FilterGbk]);
			e2->strToolTip = SStringT().Format(_T("过滤GBK重码:%s"), g_SettingsUI->bFilterGbk ? _T("启用") : _T("禁用"));
			break;
		case R.id.btn_menu:
			{
				e2->bUpdated = TRUE;
				e2->strToolTip = _T("输入法菜单");
			}
			break;
		case R.id.btn_help:
			{
				e2->bUpdated = TRUE;
				e2->strToolTip = _T("打开帮助");
			}
			break;
		case R.id.btn_status_extend:
			if (e2->strToolTip.IsEmpty())
			{
				e2->bUpdated = TRUE;
				e2->strToolTip = _T("展开状态栏");
			}
			break;
		case R.id.btn_status_shrink:
			{
				e2->bUpdated = TRUE;
				e2->strToolTip = _T("收缩状态栏");
			}
			break;
		}
		if (e2->bUpdated && !strAccel.IsEmpty())
		{
			e2->strToolTip += _T(",");
			e2->strToolTip += strAccel;
		}
	}
	void CStatusWnd::OnBtnMakePhrase()
	{
		m_pCmdListener->OnCommand(CMD_HOTKEY_MAKEPHRASE,0);
	}

	void CStatusWnd::OnLogoClick()
	{
		m_pCmdListener->OnCommand(CMD_HOTKEY_INPUTMODE, 0);
	}

	void CStatusWnd::OnMenuClick()
	{
		CPoint pt;
		GetCursorPos(&pt);
		SMenu menu;
		BOOL bLoad = menu.LoadMenu(UIRES.smenu.context_status);
		m_skinManager.ClearMap();
		SLOG_INFO("before trackpopupmenu");

		DWORD dwThreadID = GetWindowThreadProcessId(m_hOwner,NULL);
		DWORD dwCurID = GetCurrentThreadId();
		AttachThreadInput(dwCurID,dwThreadID,TRUE);
		int nScale = SDpiHelper::getScale(m_hWnd);
		int nRet = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_BOTTOMALIGN | TPM_RETURNCMD, pt.x, pt.y, m_hWnd,NULL,nScale);
		AttachThreadInput(dwCurID,dwThreadID,FALSE);
		SLOG_INFO("after trackpopupmenu" << " nRet:" << nRet);
		if (nRet == R.id.config)
		{//system config
			m_pCmdListener->OnCommand(CMD_OPENCONFIG, 0);
		}
		else if (nRet >= R.id.skin_def && nRet <= R.id.skin_def + MAX_SKINS)
		{//select menu
			SStringT strSkinPath = m_skinManager.SkinPathFromID(nRet);
			m_pCmdListener->OnCommand(CMD_CHANGESKIN, (LPARAM)&strSkinPath);
		}else if(nRet == R.id.skin_mgr)
		{
			m_pCmdListener->OnCommand(CMD_OPENSKINDIR, 0);
		}
		else if (nRet == R.id.comp_install)
		{//install comp
			CFileDialogEx openDlg(TRUE, _T("cit"), 0, 6, _T("启程码表(*.cit)\0*.cit\0All files (*.*)\0*.*\0\0"));
			if (openDlg.DoModal() == IDOK)
			{
				SStringA strCompUtf8 = S_CT2A(openDlg.m_szFileName, CP_UTF8);
				if (ISACK_SUCCESS != ISComm_InstallComp(strCompUtf8, 1))
				{
					SMessageBox(GetDesktopWindow(), _T("安装编码失败,可能已经存在该编码"), _T("提示"), MB_OK | MB_ICONSTOP);
				}
			}
		}else if(nRet == R.id.skin_cloud)
		{
			ShellExecute(NULL,_T("open"),_T("http://soime.cn/skin"),NULL,NULL,SW_SHOWNORMAL);
		}
		else if (nRet > R.id.comp_install && nRet < PopupMenuEndID(R.id.comp_install))
		{//comps
			int iComp = nRet - (R.id.comp_install +1);
			const SArray<CNameTypePair> & compList = CDataCenter::getSingleton().GetCompList();
			if (iComp < (int)compList.GetCount())
			{
				ISComm_Comp_Open(compList[iComp].strName);
			}
		}
		else if (nRet == R.id.svr_showicon)
		{//show icon
			BOOL bTray = !ISComm_SvrTray_Get();
			ISComm_SvrTray_Set(bTray);
		}
		else if (nRet == R.id.spell_one)
		{
			g_SettingsG->bBlendSpWord = !g_SettingsG->bBlendSpWord;
			g_SettingsG->SetModified(true);
		}
		else if (nRet == R.id.spell_two)
		{
			BOOL bValid = 0;
			ISComm_Bldsp_Get(&bValid, NULL, NULL);
			bValid = !bValid;
			ISComm_Bldsp_Set(BLDSP_CE2, bValid, 0, 0);
		}
		else if (nRet == R.id.spell_three)
		{
			BOOL bValid = 0;
			ISComm_Bldsp_Get(NULL, &bValid, NULL);
			bValid = !bValid;
			ISComm_Bldsp_Set(BLDSP_CE3, 0, bValid, 0);
		}
		else if (nRet == R.id.spell_all)
		{
			BOOL bValid = 0;
			ISComm_Bldsp_Get(NULL, NULL, &bValid);
			bValid = !bValid;
			ISComm_Bldsp_Set(BLDSP_CA4, 0, 0, bValid);
		}
		else if (nRet == R.id.userdef)
		{
			g_SettingsG->bBlendUD = !g_SettingsG->bBlendUD;
			g_SettingsG->SetModified(true);
		}
		else if (nRet == R.id.key_map)
		{
			m_pCmdListener->OnCommand(CMD_HOTKEY_KEYMAP, 0);
		}
		else if (nRet == R.id.switch_follow_caret)
		{
			m_pCmdListener->OnCommand(CMD_FOLLOWCARET, 0);
		}
		else if (nRet == R.id.switch_hide_statusbar)
		{
			m_pCmdListener->OnCommand(CMD_HOTKEY_HIDESTATUSBAR, 0);
		}
		else if (nRet == R.id.switch_input_big5)
		{
			g_SettingsUI->bInputBig5 = !g_SettingsUI->bInputBig5;
			g_SettingsUI->SetModified(true);
		}
		else if (nRet == R.id.key_speed)
		{
			m_pCmdListener->OnCommand(CMD_KEYSPEED, 0);
		}
		else if (nRet == R.id.help)
		{
			OnHelpClick();
		}
		else if (nRet == R.id.menu_donate)
		{
			CDenoteDlg *dlgDonate = new CDenoteDlg;
			dlgDonate->Create(m_hWnd,WS_POPUP,WS_EX_TOPMOST,0,0,0,0);
			dlgDonate->SendMessage(WM_INITDIALOG);
			dlgDonate->CenterWindow(GetDesktopWindow());
			dlgDonate->SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		}
		else if (nRet >= R.id.memu_edit_userdef && nRet <=R.id.memu_edit_userjm)
		{
			int types[] = {
				FU_USERDEF,
				FU_SYMBOL,
				FU_USERCMD,
				FU_USERJM,
			};
			OnEditUserDefData(types[nRet - R.id.memu_edit_userdef]);
		}
		else if (nRet > R.id.menu_tool_base && nRet < (R.id.menu_tool_base + 99) / 100 * 100)
		{//open tools.
			SStringT strToolPath = m_toolManager.ToolPathFromID(nRet);
			m_pCmdListener->OnCommand(CMD_EXECUTETOOL, (LPARAM)&strToolPath);
		}
		else if (nRet == R.id.switch_filter_gbk)
		{
			g_SettingsUI->bFilterGbk = !g_SettingsUI->bFilterGbk;
			g_SettingsUI->SetModified(true);
			m_pCmdListener->OnCommand(CMD_SYNCUI, BTN_FILTERGBK);
		}
		else if (nRet == R.id.switch_read_input)
		{
			g_SettingsUI->bSound = !g_SettingsUI->bSound;
			g_SettingsUI->SetModified(true);
			m_pCmdListener->OnCommand(CMD_SYNCUI, BTN_SOUND);
		}
		else if (nRet == R.id.switch_record_input)
		{
			g_SettingsUI->bRecord = !g_SettingsUI->bRecord;
			g_SettingsUI->SetModified(true);
			m_pCmdListener->OnCommand(CMD_SYNCUI, BTN_RECORD);
		}
		else if (nRet == R.id.switch_word_input)
		{
			g_SettingsUI->bEnglish = !g_SettingsUI->bEnglish;
			g_SettingsUI->SetModified(true);
			m_pCmdListener->OnCommand(CMD_SYNCUI, BTN_ENGLISHMODE);
		}

		m_skinManager.ClearMap();
	}

	void CStatusWnd::OnHelpClick()
	{
		m_pCmdListener->OnCommand(CMD_OPENHELP, 0);
	}

	void CStatusWnd::OnQueryClick()
	{
		m_pCmdListener->OnCommand(CMD_HOTKEY_QUERYINFO, 0);
	}


	void CStatusWnd::OnEditUserDefData(int nType)
	{
		if (ISComm_FatctUserDefFileName(nType) == ISACK_SUCCESS)
		{
			PMSGDATA pMsgData = ISComm_GetData();
			SStringA strUtf8((char*)pMsgData->byData, pMsgData->sSize);
			SStringT strFileName = S_CA2T(strUtf8, CP_UTF8);
			SHELLEXECUTEDATA efi = { nType,_T("open"),strFileName };
			m_pCmdListener->OnCommand(CMD_EDITFILE, (LPARAM)&efi);
		}
		else
		{
			CUtils::SoundPlay(_T("error"));
		}
	}

}

