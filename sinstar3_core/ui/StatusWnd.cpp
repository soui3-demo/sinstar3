#include "StdAfx.h"
#include "StatusWnd.h"
#include <helper/SMenu.h>

#define CMD_MENU_FIRST	 220
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

		CRect rcWorkArea;
		SystemParametersInfo(SPI_GETWORKAREA,0,&rcWorkArea,0);
		SetWindowPos(HWND_TOPMOST,rcWorkArea.right-GetWindowRect().Width(),rcWorkArea.bottom-GetWindowRect().Height(),0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
		return 0;
	}

	void CStatusWnd::OnRButtonUp(UINT nFlags,CPoint pt)
	{
		SMenu menu;
		menu.LoadMenu(UIRES.smenu.context);
		ClientToScreen(&pt);
		m_pSkinManager.ClearMap();
		int nRet = menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_BOTTOMALIGN|TPM_RETURNCMD,pt.x,pt.y,m_hWnd);
		if(nRet>=CMD_MENU_FIRST && nRet <=CMD_MENU_FIRST+1000)
		{//select menu
			m_pSkinManager.SetSkin(nRet);
		}
		m_pSkinManager.ClearMap();
	}

	void CStatusWnd::OnInitMenuPopup(HMENU menuPopup, UINT nIndex, BOOL bSysMenu)
	{
		if(GetMenuContextHelpId(menuPopup)==2)
		{
			m_pSkinManager.InitSkinMenu(menuPopup,theModule->GetDataPath()+_T("\\skins"),CMD_MENU_FIRST);
		}
	}

	void CStatusWnd::OnDragStatus(EventArgs *e)
	{
		EventDragMove *e2 = sobj_cast<EventDragMove>(e);
		CRect rcWnd;
		CSimpleWnd::GetWindowRect(&rcWnd);
		SetWindowPos(HWND_TOPMOST,rcWnd.left+e2->ptMove.x,rcWnd.top+e2->ptMove.y,0,0,SWP_NOSIZE|SWP_NOACTIVATE);
	}

}
