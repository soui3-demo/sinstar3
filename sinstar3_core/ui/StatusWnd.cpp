#include "StdAfx.h"
#include "StatusWnd.h"
#include <helper/SMenu.h>

#define CMD_MENU_FIRST	 220
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

		DWORD dwPos=-1;
		theModule->GetRegKey().QueryDWORDValue(_T("status_pos"),dwPos);
		CPoint pt(GET_X_LPARAM(dwPos),GET_Y_LPARAM(dwPos));
		if(pt.x<0 || pt.y<0)
		{
			CRect rcWorkArea;
			SystemParametersInfo(SPI_GETWORKAREA,0,&rcWorkArea,0);
			pt.x = rcWorkArea.right-GetWindowRect().Width();
			pt.y = rcWorkArea.bottom-GetWindowRect().Height();
		}
		SetWindowPos(HWND_TOPMOST,pt.x,pt.y,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
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

		CPoint pt = rcWnd.TopLeft()+e2->ptMove;

		RECT rcWorkArea;
		SystemParametersInfo(SPI_GETWORKAREA,0,&rcWorkArea,0);

		if(pt.x-rcWorkArea.left<=SIZE_MAGNETIC) pt.x=rcWorkArea.left;
		if(pt.y-rcWorkArea.top<SIZE_MAGNETIC) pt.y=rcWorkArea.top;
		if(rcWorkArea.right-pt.x-rcWnd.Width()<SIZE_MAGNETIC) pt.x=rcWorkArea.right-rcWnd.Width();
		if(rcWorkArea.bottom-pt.y-rcWnd.Height()<SIZE_MAGNETIC) pt.y=rcWorkArea.bottom-rcWnd.Height();
		SetWindowPos(NULL,pt.x,pt.y,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
	}

	void CStatusWnd::OnDestroy()
	{
		CRect rcWnd;
		CSimpleWnd::GetWindowRect(&rcWnd);
		theModule->GetRegKey().SetDWORDValue(_T("status_pos"),MAKELPARAM(rcWnd.left,rcWnd.top));
		__super::OnDestroy();
	}

}
