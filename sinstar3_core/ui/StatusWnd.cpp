#include "StdAfx.h"
#include "StatusWnd.h"
#include <helper/SMenuEx.h>
#include "ConfigDlg.h"
#include "../InputState.h"

#define SIZE_MAGNETIC	 5
#define MAX_SKINS	 80
namespace SOUI
{
	CStatusWnd::CStatusWnd(ICmdListener *pListener):CImeWnd(UIRES.LAYOUT.wnd_status_bar), m_pCmdListener(pListener)
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
		if(pt.x<0) pt.x = 0;
		if(pt.y<0) pt.y = 0;
		if(pt.x + rcWnd.Width() > rcWorkArea.right)
			pt.x = rcWorkArea.right - rcWnd.Width();
		if(pt.y + rcWnd.Height()> rcWorkArea.bottom)
			pt.y = rcWorkArea.bottom - rcWnd.Height();

		UpdateCompInfo();
		{
			SToggle * toggle = FindChildByID2<SToggle>(R.id.btn_charmode);
			if(toggle) toggle->SetToggle(g_SettingsL.bCharMode);
		}
		{
			SToggle * toggle = FindChildByID2<SToggle>(R.id.btn_sound);
			if(toggle) toggle->SetToggle(g_SettingsL.bSound);
		}
		{
			SToggle * toggle = FindChildByID2<SToggle>(R.id.btn_record);
			if(toggle) toggle->SetToggle(g_SettingsL.bSound);
		}


		SetWindowPos(HWND_TOPMOST,pt.x,pt.y,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
		return 0;
	}

	void CStatusWnd::OnRButtonUp(UINT nFlags,CPoint pt)
	{
		const MSG * pMsg = GetCurrentMessage();
		SHostWnd::OnMouseEvent(pMsg->message,pMsg->wParam,pMsg->lParam);

		SMenuEx menu;
		BOOL bLoad=menu.LoadMenu(UIRES.smenu.context);
		ClientToScreen(&pt);
		m_skinManager.ClearMap();
		SLOG_INFO("before trackpopupmenu");
		int nRet = menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_BOTTOMALIGN|TPM_RETURNCMD,pt.x,pt.y, m_hWnd);
		SLOG_INFO("after trackpopupmenu"<<" nRet:"<<nRet);
		if (nRet == R.id.config)
		{//system config
			CConfigDlg configDlg;
			configDlg.DoModal();
		}else if(nRet>=R.id.skin_def && nRet <= R.id.skin_def + MAX_SKINS)
		{//select menu
			m_skinManager.SetSkin(nRet);
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
			int uID = R.id.svr_showicon+1;
			while (uID<=nRet)
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
			ISComm_Bldsp_Get(NULL,&bValid, NULL);
			bValid = !bValid;
			ISComm_Bldsp_Set(BLDSP_CE3, 0, bValid, 0);
		}
		else if (nRet == R.id.spell_all)
		{
			BOOL bValid = 0;
			ISComm_Bldsp_Get(NULL, NULL, &bValid);
			bValid = !bValid;
			ISComm_Bldsp_Set(BLDSP_CA4,0,0, bValid);
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
			g_SettingsL.bMouseFollow = !g_SettingsL.bMouseFollow;
		}
		else if (nRet == R.id.hide_statusbar)
		{
			m_pCmdListener->OnCommand(CMD_HIDESTATUSBAR, 0);
		}
		else if (nRet == R.id.input_big5)
		{
			g_SettingsL.bInputBig5 = !g_SettingsL.bInputBig5;
		}

		m_skinManager.ClearMap();
	}

	void CStatusWnd::OnInitMenuPopup(SMenuEx* menuPopup, UINT nIndex)
	{
		switch (menuPopup->GetContextHelpId())
		{
		case 100:
		{//main menu
			menuPopup->CheckMenuItem(R.id.follow_caret, MF_BYCOMMAND | g_SettingsL.bMouseFollow ? MF_CHECKED : 0);
			menuPopup->CheckMenuItem(R.id.hide_statusbar, MF_BYCOMMAND | g_SettingsL.bHideStatus ? MF_CHECKED : 0);
			menuPopup->CheckMenuItem(R.id.input_big5, MF_BYCOMMAND | g_SettingsL.bInputBig5 ? MF_CHECKED : 0);
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

	void CStatusWnd::OnDragStatus(EventArgs *e)
	{
		EventDragMove *e2 = sobj_cast<EventDragMove>(e);
		CRect rcWnd;
		CSimpleWnd::GetWindowRect(&rcWnd);

		CPoint pt = rcWnd.TopLeft()+e2->ptMove;

		RECT rcWorkArea;
		SystemParametersInfo(SPI_GETWORKAREA,0,&rcWorkArea,0);

		if(pt.x-rcWorkArea.left<=SIZE_MAGNETIC) pt.x=rcWorkArea.left;
		if(pt.y-rcWorkArea.top<SIZE_MAGNETIC) pt.y=rcWorkArea.top;
		if(rcWorkArea.right-pt.x-rcWnd.Width()<SIZE_MAGNETIC) pt.x=rcWorkArea.right-rcWnd.Width();
		if(rcWorkArea.bottom-pt.y-rcWnd.Height()<SIZE_MAGNETIC) pt.y=rcWorkArea.bottom-rcWnd.Height();
		SetWindowPos(NULL,pt.x,pt.y,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);

		CDataCenter::getSingletonPtr()->GetData().m_ptStatus = pt;
	}

	void CStatusWnd::OnSetSkin(EventArgs *e)
	{
		OnDestroy();
		CREATESTRUCT cs;
		cs.cx=0;
		cs.cy=0;
		OnCreate(&cs);
	}


	void CStatusWnd::OnBtnExtend()
	{
		m_pBackGround->SetMode(SStatusBackground::MODE_EXTEND);

		FindChildByID(R.id.btn_status_extend)->SetVisible(TRUE,TRUE);
		FindChildByID(R.id.btn_status_shrink)->SetVisible(TRUE,TRUE);
		FindChildByID(R.id.btn_status_extend)->SetVisible(FALSE,TRUE);
		FindChildByID(R.id.status_extend)->SetVisible(TRUE,TRUE);
	}

	void CStatusWnd::OnBtnShrink()
	{
		m_pBackGround->SetMode(SStatusBackground::MODE_SHRINK);

		FindChildByID(R.id.btn_status_extend)->SetVisible(FALSE,TRUE);
		FindChildByID(R.id.btn_status_shrink)->SetVisible(FALSE,TRUE);
		FindChildByID(R.id.btn_status_extend)->SetVisible(TRUE,TRUE);
		FindChildByID(R.id.status_extend)->SetVisible(FALSE,TRUE);

	}

	void CStatusWnd::UpdateCompInfo()
	{
		FindChildByID(R.id.txt_comp)->SetWindowText(CDataCenter::getSingletonPtr()->GetData().m_compInfo.strCompName);
		SFlagView * pFlagView = FindChildByID2<SFlagView>(R.id.img_logo);
		if (pFlagView)
		{
			pFlagView->SetImeFlagData(ISComm_GetCompInfo()->pImeFlagData);
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
			g_SettingsL.bCharMode = toggle->GetToggle();
		}
	}

	void CStatusWnd::OnSwitchRecord(EventArgs *e)
	{
		SToggle * toggle = sobj_cast<SToggle>(e->sender);
		if(toggle)
		{
			g_SettingsL.bRecord = toggle->GetToggle();
		}
	}

	void CStatusWnd::OnSwitchSound(EventArgs *e)
	{
		SToggle * toggle = sobj_cast<SToggle>(e->sender);
		if(toggle)
		{
			g_SettingsL.bSound = toggle->GetToggle();
		}

	}

	void CStatusWnd::OnBtnMakeWord()
	{
		m_pCmdListener->OnCommand(CMD_MAKEWORD,0);
	}

}

