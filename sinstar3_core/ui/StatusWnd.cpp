#include "StdAfx.h"
#include "StatusWnd.h"
#include <helper/SMenu.h>

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
		menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_BOTTOMALIGN,pt.x,pt.y,m_hWnd);
	}

	void CStatusWnd::OnInitMenuPopup(HMENU menuPopup, UINT nIndex, BOOL bSysMenu)
	{
		if(nIndex == 6)
		{
			InitSkinMenu(menuPopup,theModule->GetDataPath());
		}
		SLOG_INFO("OnInitMenuPopup, nIndex:"<<nIndex);
	}

	void CStatusWnd::InitSkinMenu(HMENU hMenu, const SStringT &strSkinPath)
	{
		SMenu menu(hMenu);

		menu.AppendMenu(0,630,L"skin1",0);
		menu.AppendMenu(0,631,L"skin2",0);
		menu.AppendMenu(0,632,L"skin3",0);
		menu.AppendMenu(0,633,L"skin4",0);
		::CheckMenuItem(hMenu,630,MF_CHECKED|MF_BYCOMMAND);
		menu.Detach();

	}

}
