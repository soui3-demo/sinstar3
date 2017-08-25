#include "StdAfx.h"
#include "StatusWnd.h"

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

}
