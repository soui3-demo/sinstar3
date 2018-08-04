#include "StdAfx.h"
#include "StatusWnd.h"
#include <helper/SMenuEx.h>
#include <HtmlHelp.h>
#include "ConfigDlg.h"
#include "../InputState.h"

#pragma comment(lib,"htmlhelp.lib")

#define MAX_SKINS	 80
namespace SOUI
{
	CStatusWnd::CStatusWnd(SEventSet *pEvtSets, ICmdListener *pListener)
		:CImeWnd(pEvtSets,UIRES.LAYOUT.wnd_status_bar)
		, m_pCmdListener(pListener)
		, m_skinManager(pEvtSets)
	{
	}

	CStatusWnd::~CStatusWnd(void)
	{
	}

	int CStatusWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		int nRet = __super::OnCreate(lpCreateStruct);
		if(nRet != 0) return nRet;

		m_pBackGround = FindChildByID2<SStatusBackground>(R.id.status_bg);

		CRect rcWnd = GetWindowRect();
		CRect rcWorkArea;
		SystemParametersInfo(SPI_GETWORKAREA,0,&rcWorkArea,0);

		CPoint pt =CDataCenter::getSingletonPtr()->GetData().m_ptStatus;
		if (pt.x < 0 || pt.y<0)
		{
			CSize szWnd = GetDesiredSize(-1, -1);
			pt.x = rcWorkArea.right - rcWnd.Width();
			pt.y = rcWorkArea.bottom - rcWnd.Height();
		}
		if(pt.x + rcWnd.Width() > rcWorkArea.right)
			pt.x = rcWorkArea.right - rcWnd.Width();
		if(pt.y + rcWnd.Height()> rcWorkArea.bottom)
			pt.y = rcWorkArea.bottom - rcWnd.Height();
		SetWindowPos(HWND_TOPMOST,pt.x,pt.y,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);

		if (m_pCmdListener->GetInputContext()->settings.bFullStatus)
			OnBtnExtend();
		else
			OnBtnShrink();
		UpdateUI();
		return 0;
	}

	void CStatusWnd::UpdateUI()
	{
		UpdateToggleStatus(BTN_ALL);
		UpdateCompInfo();
	}

	void CStatusWnd::OnRButtonUp(UINT nFlags,CPoint pt)
	{
		const MSG * pMsg = GetCurrentMessage();
		SHostWnd::OnMouseEvent(pMsg->message,pMsg->wParam,pMsg->lParam);

		OnMenuClick();
	}

	void CStatusWnd::OnInitMenuPopup(SMenuEx* menuPopup, UINT nIndex)
	{
		CSettingsLocal & settings = m_pCmdListener->GetInputContext()->settings;
		switch (menuPopup->GetContextHelpId())
		{
		case 100:
		{//main menu
			menuPopup->CheckMenuItem(R.id.follow_caret, MF_BYCOMMAND | settings.bMouseFollow ? MF_CHECKED : 0);
			menuPopup->CheckMenuItem(R.id.hide_statusbar, MF_BYCOMMAND | settings.bHideStatus ? MF_CHECKED : 0);
			menuPopup->CheckMenuItem(R.id.input_big5, MF_BYCOMMAND | settings.bInputBig5 ? MF_CHECKED : 0);
			break;
		}
		case 2:
		{
			SStringT strCurSkin = CDataCenter::getSingletonPtr()->GetData().m_strSkin;
			if (strCurSkin.IsEmpty())
			{
				menuPopup->CheckMenuItem(R.id.skin_def, MF_BYCOMMAND|MF_CHECKED);
			}
			m_skinManager.InitSkinMenu(menuPopup, theModule->GetDataPath() + _T("\\skins"), R.id.skin_def, strCurSkin);
			break;
		}
		case 4://comp select
		{
			const SArray<CNameTypePair> &comps = CDataCenter::getSingleton().UpdateCompList();
			int idStart = R.id.comp_start;
			menuPopup->DeleteMenu(R.id.comp_start,MF_BYCOMMAND);
			int iSelComp = CDataCenter::getSingleton().GetSelectCompIndex();
			for (size_t i = 0; i < comps.GetCount(); i++)
			{
				SStringA strText = SStringA().Format("%s[%s]", comps[i].strName, comps[i].strType);
				UINT flag = MF_BYPOSITION;
				if (iSelComp == i) flag |= MF_CHECKED;
				menuPopup->InsertMenu(-1, flag, idStart + (int)i, S_CA2T(strText));
			}
			break;
		}
		case 5://blend input
		{
			BOOL bCe2 = 0, bCe3 = 0, bCa4 = 0;
			ISComm_Bldsp_Get(&bCe2, &bCe3, &bCa4);
			menuPopup->CheckMenuItem(R.id.spell_one,MF_BYCOMMAND | (g_SettingsG.bBlendSpWord ? MF_CHECKED : 0));
			menuPopup->CheckMenuItem(R.id.spell_two, MF_BYCOMMAND | (bCe2 ? MF_CHECKED : 0));
			menuPopup->CheckMenuItem(R.id.spell_three, MF_BYCOMMAND | (bCe3 ? MF_CHECKED : 0));
			menuPopup->CheckMenuItem(R.id.spell_all, MF_BYCOMMAND | (bCa4 ? MF_CHECKED : 0));
			menuPopup->CheckMenuItem(R.id.userdef, MF_BYCOMMAND | (g_SettingsG.bBlendUD ? MF_CHECKED : 0));

			break;
		}
		case 6://svr data manager
		{
			LPCSTR pszPages = ISComm_Svr_Pages();
			if (pszPages)
			{
				int nPos = 1;
				UINT uID = R.id.svr_showicon;
				while (*pszPages)
				{
					SStringA strName = pszPages;
					if (*pszPages == '\n')
						menuPopup->InsertMenu(nPos++, MF_BYPOSITION | MF_SEPARATOR, 0 , _T(""));
					else
						menuPopup->InsertMenu(nPos++, MF_BYPOSITION | MF_STRING, uID, S_CA2T(strName));
					uID++;
					pszPages += strName.GetLength() + 1;
				}
			}
			menuPopup->CheckMenuItem(R.id.svr_showicon, MF_BYCOMMAND | ISComm_SvrTray_Get() ? MF_CHECKED : 0);

			break;
		}
		}

	}


	void CStatusWnd::OnBtnExtend()
	{
		m_pCmdListener->GetInputContext()->settings.bFullStatus = TRUE;

		m_pBackGround->SetMode(SStatusBackground::MODE_EXTEND);

		FindChildByID(R.id.btn_status_shrink)->SetVisible(TRUE,TRUE);
		FindChildByID(R.id.btn_status_extend)->SetVisible(FALSE,TRUE);
		FindChildByID(R.id.status_extend)->SetVisible(TRUE,TRUE);
	}

	void CStatusWnd::OnBtnShrink()
	{
		m_pCmdListener->GetInputContext()->settings.bFullStatus = FALSE;
		m_pBackGround->SetMode(SStatusBackground::MODE_SHRINK);

		FindChildByID(R.id.btn_status_shrink)->SetVisible(FALSE,TRUE);
		FindChildByID(R.id.btn_status_extend)->SetVisible(TRUE,TRUE);
		FindChildByID(R.id.status_extend)->SetVisible(FALSE,TRUE);

	}

	void CStatusWnd::UpdateCompInfo()
	{
		SWindow *pText = FindChildByID(R.id.txt_comp);
		SFlagView * pFlagView = FindChildByID2<SFlagView>(R.id.img_logo);
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
				if (g_SettingsG.compMode == IM_SHAPECODE)
					pText->SetWindowText(_T("临时拼音"));
				else
					pText->SetWindowText(_T("启程拼音"));
			}

		}
	}

	void CStatusWnd::UpdateToggleStatus(DWORD flags)
	{
		CSettingsLocal & settings = m_pCmdListener->GetInputContext()->settings;

		if(flags & BTN_CHARMODE){
			SToggle * toggle = FindChildByID2<SToggle>(R.id.btn_charmode);
			if (toggle) toggle->SetToggle(settings.bCharMode);
		}
		if(flags & BTN_SOUND){
			SToggle * toggle = FindChildByID2<SToggle>(R.id.btn_sound);
			if (toggle) toggle->SetToggle(!settings.bSound);
		}
		if (flags & BTN_RECORD) {
			SToggle * toggle = FindChildByID2<SToggle>(R.id.btn_record);
			if (toggle) toggle->SetToggle(!settings.bRecord);
		}
		if (flags & BTN_ENGLISHMODE)
		{
			SToggle * toggle = FindChildByID2<SToggle>(R.id.btn_english);
			if (toggle) toggle->SetToggle(!settings.bEnglish);
		}
	}

	void CStatusWnd::OnCompInfo(EventArgs *e)
	{
		if(!IsWindow())
			return;

		EventSvrNotify *e2 = sobj_cast<EventSvrNotify>(e);
		if(e2->wp == NT_COMPINFO)
		{
			UpdateCompInfo();
		}
	}

	void CStatusWnd::OnSwitchCharMode(EventArgs *e)
	{
		SToggle * toggle = sobj_cast<SToggle>(e->sender);
		if(toggle)
		{
			CSettingsLocal & settings = m_pCmdListener->GetInputContext()->settings;
			settings.bCharMode = toggle->GetToggle();
		}
	}

	void CStatusWnd::OnSwitchRecord(EventArgs *e)
	{
		SToggle * toggle = sobj_cast<SToggle>(e->sender);
		if(toggle)
		{
			CSettingsLocal & settings = m_pCmdListener->GetInputContext()->settings;
			settings.bRecord = !toggle->GetToggle();
		}
	}

	void CStatusWnd::OnSwitchSound(EventArgs *e)
	{
		SToggle * toggle = sobj_cast<SToggle>(e->sender);
		if(toggle)
		{
			CSettingsLocal & settings = m_pCmdListener->GetInputContext()->settings;
			settings.bSound = !toggle->GetToggle();
		}

	}

	void CStatusWnd::OnSwitchEnglish(EventArgs * e)
	{
		SToggle * toggle = sobj_cast<SToggle>(e->sender);
		if (toggle)
		{
			CSettingsLocal & settings = m_pCmdListener->GetInputContext()->settings;
			settings.bEnglish = !toggle->GetToggle();
		}
	}

	void CStatusWnd::OnUpdateBtnTooltip(EventArgs *e)
	{
		CSettingsLocal & settings = m_pCmdListener->GetInputContext()->settings;
		EventSwndUpdateTooltip *e2 = sobj_cast<EventSwndUpdateTooltip>(e);
		SASSERT(e2);
		switch (e2->idFrom)
		{
		case R.id.btn_charmode:
			e2->bUpdated = TRUE;
			e2->strToolTip = SStringT().Format(_T("标点模式:%s"), settings.bCharMode? _T("中文"):_T("英文"));
			break;
		case R.id.btn_makeword:
			e2->bUpdated = TRUE;
			e2->strToolTip = _T("剪贴板造词");
			break;
		case R.id.btn_record:
			e2->bUpdated = TRUE;
			e2->strToolTip = SStringT().Format(_T("记录输入状态:%s"), settings.bRecord ? _T("启用") : _T("禁用"));
			break;
		case R.id.btn_sound:
			e2->bUpdated = TRUE;
			e2->strToolTip = SStringT().Format(_T("语音较对:%s"), settings.bSound ? _T("启用") : _T("禁用"));
			break;
		case R.id.btn_english:
			e2->bUpdated = TRUE;
			e2->strToolTip = SStringT().Format(_T("单词输入:%s"), settings.bEnglish ? _T("启用") : _T("禁用"));
			break;
		case R.id.btn_status_extend:
			e2->bUpdated = TRUE;
			e2->strToolTip = _T("展开状态栏");
			break;
		case R.id.btn_status_shrink:
			e2->bUpdated = TRUE;
			e2->strToolTip = _T("收缩状态栏");
			break;
		}
	}
	void CStatusWnd::OnBtnMakeWord()
	{
		m_pCmdListener->OnCommand(CMD_MAKEWORD,0);
	}

	void CStatusWnd::OnLogoClick()
	{
		m_pCmdListener->OnCommand(CMD_INPUTMODE, 0);
	}

	void CStatusWnd::OnMenuClick()
	{
		CPoint pt;
		GetCursorPos(&pt);
		SMenuEx menu;
		BOOL bLoad = menu.LoadMenu(UIRES.smenu.context);
		m_skinManager.ClearMap();
		SLOG_INFO("before trackpopupmenu");
		int nRet = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_BOTTOMALIGN | TPM_RETURNCMD, pt.x, pt.y, m_hWnd);
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
			m_pCmdListener->OnCommand(CMD_SKINMGR, 0);
		}
		else if (nRet >= R.id.comp_start && nRet < R.id.comp_start + 50)
		{//comps
			int iComp = nRet - R.id.comp_start;
			const SArray<CNameTypePair> & compList = CDataCenter::getSingleton().GetCompList();
			if (iComp < (int)compList.GetCount())
			{
				ISComm_Comp_Open(compList[iComp].strName);
			}
		}
		else if (nRet > R.id.svr_showicon && nRet < R.id.svr_showicon + 50)
		{//svr page
			LPCSTR pszPages = ISComm_Svr_Pages();
			int uID = R.id.svr_showicon + 1;
			while (uID <= nRet)
			{
				pszPages += strlen(pszPages) + 1;
				uID++;
			}
			ISComm_ShowServer(pszPages, (char)strlen(pszPages));
		}
		else if (nRet == R.id.svr_showicon)
		{//show icon
			BOOL bTray = !ISComm_SvrTray_Get();
			ISComm_SvrTray_Set(bTray);
		}
		else if (nRet == R.id.spell_one)
		{
			g_SettingsG.bBlendSpWord = !g_SettingsG.bBlendSpWord;
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
			g_SettingsG.bBlendUD = !g_SettingsG.bBlendUD;
		}
		else if (nRet == R.id.key_map)
		{
			m_pCmdListener->OnCommand(CMD_KEYMAP, 0);
		}
		else if (nRet == R.id.follow_caret)
		{
			m_pCmdListener->OnCommand(CMD_FOLLOWCARET, 0);
		}
		else if (nRet == R.id.hide_statusbar)
		{
			m_pCmdListener->OnCommand(CMD_HIDESTATUSBAR, 0);
		}
		else if (nRet == R.id.input_big5)
		{
			CSettingsLocal & settings = m_pCmdListener->GetInputContext()->settings;
			settings.bInputBig5 = !settings.bInputBig5;
		}
		else if (nRet == R.id.key_speed)
		{
			m_pCmdListener->OnCommand(CMD_KEYSPEED, 0);
		}
		else if (nRet == R.id.help)
		{
			OnHelpClick();
		}
		else if (nRet = R.id.open_spchar)
		{
			m_pCmdListener->OnCommand(CMD_OPENSPCHAR, 0);
		}

		m_skinManager.ClearMap();
	}

	void CStatusWnd::OnHelpClick()
	{
		SStringT path= SStringT().Format(_T("%s\\sinstar3.chm>main"), theModule->GetDataPath());
		HtmlHelp(NULL, path, HH_DISPLAY_TOPIC, 0);
	}

}

