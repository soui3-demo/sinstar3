#include "StdAfx.h"
#include "StatusWnd.h"
#include <helper/SMenu.h>
#include "ConfigDlg.h"

#define SIZE_MAGNETIC	 5
namespace SOUI
{
	CStatusWnd::CStatusWnd(void):CImeWnd(UIRES.LAYOUT.wnd_status_bar)
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

		CPoint pt =CDataCenter::GetAutoLockerInstance()->GetData().m_ptStatus;
		if(pt.x<0) pt.x = 0;
		if(pt.y<0) pt.y = 0;
		if(pt.x + rcWnd.Width() > rcWorkArea.right)
			pt.x = rcWorkArea.right - rcWnd.Width();
		if(pt.y + rcWnd.Height()> rcWorkArea.bottom)
			pt.y = rcWorkArea.bottom - rcWnd.Height();

		FindChildByID(R.id.txt_comp)->SetWindowText(CDataCenter::GetAutoLockerInstance()->GetData().m_compInfo.strCompName);

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
		SMenu menu;
		menu.LoadMenu(UIRES.smenu.context);
		ClientToScreen(&pt);
		m_skinManager.ClearMap();
		int nRet = menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_BOTTOMALIGN|TPM_RETURNCMD,pt.x,pt.y,m_hWnd);
		if(nRet>=CMD_MENU_DEF && nRet <=CMD_MENU_DEF+100)
		{//select menu
			m_skinManager.SetSkin(nRet);
		}else if(nRet == 100)
		{//system config
			CConfigDlg configDlg;
			configDlg.DoModal();
		}
		m_skinManager.ClearMap();
	}

	void CStatusWnd::OnInitMenuPopup(HMENU menuPopup, UINT nIndex, BOOL bSysMenu)
	{
		if(GetMenuContextHelpId(menuPopup)==2)
		{
			SStringT strCurSkin = CDataCenter::GetAutoLockerInstance()->GetData().m_strSkin;
			if(strCurSkin.IsEmpty())
			{
				CheckMenuItem(menuPopup,CMD_MENU_DEF,MF_CHECKED|MF_BYCOMMAND);
			}
			m_skinManager.InitSkinMenu(menuPopup,theModule->GetDataPath()+_T("\\skins"),CMD_MENU_DEF,strCurSkin);
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

		CDataCenter::GetAutoLockerInstance()->GetData().m_ptStatus = pt;
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

	void CStatusWnd::OnCompInfo(EventArgs *e)
	{
		if(!IsWindow())
			return;

		EventSvrNotify *e2 = sobj_cast<EventSvrNotify>(e);
		if(e2->wp == NT_COMPINFO)
		{
			FindChildByID(R.id.txt_comp)->SetWindowText(CDataCenter::GetAutoLockerInstance()->GetData().m_compInfo.strCompName);
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

}
