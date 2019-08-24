#include "stdafx.h"
#include "ImeWnd.h"

namespace SOUI
{

#define SIZE_MAGNETIC	 5

CImeWnd::CImeWnd(SEventSet *pEvtSets,LPCTSTR pszLayout)
	:CSkinAwareWnd(pEvtSets,pszLayout)
	, m_canReleaseCapture(TRUE)
	, m_hOwner(NULL)
{
}

CImeWnd::~CImeWnd()
{
}

void CImeWnd::SetOwner(HWND hOwner)
{
	m_hOwner = hOwner;
}


BOOL CImeWnd::OnSetCursor(HWND wnd, UINT nHitTest, UINT message)
{
	if(::GetCapture()!=m_hWnd)
	{
		SLOG_INFO("SetCapture");
		::SetCapture(m_hWnd);
	}
	return TRUE;
}

void CImeWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	SetMsgHandled(FALSE);
	__super::OnSetCursor(m_hWnd,HTCLIENT,WM_MOUSEMOVE);	//disable ime window receive wm_setcursor by user
	ClientToScreen(&point);
	HWND hHitWnd = ::WindowFromPoint(point);
	if(hHitWnd != m_hWnd && m_canReleaseCapture)
	{
		SLOG_INFO("ReleaseCapture");
		::ReleaseCapture();
	}
}

HWND CImeWnd::Create(HWND hParent)
{
	return SHostWnd::Create(hParent,WS_POPUP|WS_DISABLED,WS_EX_TOOLWINDOW|WS_EX_TOPMOST|WS_EX_NOACTIVATE,0,0,0,0);
}

void CImeWnd::Show(BOOL bShow)
{
	if(!IsWindow()) return;
	PostMessage(UM_AYNC_UI, bShow);
}

LRESULT CImeWnd::OnAyncUI(UINT uMsg, WPARAM wp, LPARAM lp)
{
	BOOL bShow = wp;
	if (bShow)
	{
		SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
	}
	else if (GetNative()->IsWindowVisible())
	{
		ShowWindow(SW_HIDE);
	}
	return 0;
}

LRESULT CImeWnd::OnMouseEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SetMsgHandled(FALSE);
	if(GetToolTip())
	{
		GetToolTip()->RelayEvent(GetCurrentMessage());
	}
	return 0;
}


BOOL CImeWnd::OnReleaseSwndCapture()
{
	if(!SwndContainerImpl::OnReleaseSwndCapture()) return FALSE;
	m_canReleaseCapture = TRUE;
	SLOG_INFO("m_canReleaseCapture:TRUE");
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	PostMessage(WM_MOUSEMOVE,0,MAKELPARAM(pt.x,pt.y));
	return TRUE;
}

void CImeWnd::OnDragStatus(EventArgs *e)
{
	EventDragMove *e2 = sobj_cast<EventDragMove>(e);
	CRect rcWnd;
	GetNative()->GetWindowRect(&rcWnd);

	CPoint pt = rcWnd.TopLeft() + e2->ptMove;

	RECT rcWorkArea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);

	if (pt.x - rcWorkArea.left <= SIZE_MAGNETIC) pt.x = rcWorkArea.left;
	if (pt.y - rcWorkArea.top<SIZE_MAGNETIC) pt.y = rcWorkArea.top;
	if (rcWorkArea.right - pt.x - rcWnd.Width()<SIZE_MAGNETIC) pt.x = rcWorkArea.right - rcWnd.Width();
	if (rcWorkArea.bottom - pt.y - rcWnd.Height()<SIZE_MAGNETIC) pt.y = rcWorkArea.bottom - rcWnd.Height();
	SetWindowPos(NULL, pt.x, pt.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

	OnReposition(pt);
}


int CImeWnd::OnRecreateUI(LPCREATESTRUCT lpCreateStruct)
{
	return SHostWnd::OnCreate(lpCreateStruct);
}

SWND CImeWnd::OnSetSwndCapture(SWND swnd)
{
	SWND ret =  SwndContainerImpl::OnSetSwndCapture(swnd);
	m_canReleaseCapture = FALSE;
	SLOG_INFO("m_canReleaseCapture:FALSE");
	return ret;
}


}
