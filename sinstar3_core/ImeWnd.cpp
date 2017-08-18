#include "stdafx.h"
#include "ImeWnd.h"

namespace SOUI
{


CImeWnd::CImeWnd(LPCTSTR pszLayout):SHostWnd(pszLayout)
{

}

BOOL CImeWnd::OnSetCursor(HWND wnd, UINT nHitTest, UINT message)
{
	CSimpleWnd::SetCapture();
	SetMsgHandled(FALSE);
	return TRUE;
}

void CImeWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	SetMsgHandled(FALSE);
	CRect rcWnd = SWindow::GetWindowRect();
	if(!rcWnd.PtInRect(point))
	{
		::ReleaseCapture();
	}
}

HWND CImeWnd::Create()
{
	return CSimpleWnd::Create(_T("SinstarWindow"),WS_DISABLED|WS_POPUP,WS_EX_TOOLWINDOW,0,0,0,0,NULL,NULL);
}

void CImeWnd::Show(BOOL bShow)
{
	if(!IsWindow()) return;
	if(bShow)
		SetWindowPos(HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE|SWP_SHOWWINDOW);
	else
		ShowWindow(SW_HIDE);
}

}
