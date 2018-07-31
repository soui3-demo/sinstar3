#include "stdafx.h"
#include "ImeWnd.h"

namespace SOUI
{


CImeWnd::CImeWnd(SEventSet *pEvtSets,LPCTSTR pszLayout):SHostWnd(pszLayout), m_pEvtSet(pEvtSets)
{
	m_pEvtSet->subscribeEvent(EventSvrNotify::EventID,Subscriber(&CImeWnd::OnEvent, this));
	m_pEvtSet->subscribeEvent(EventSetSkin::EventID, Subscriber(&CImeWnd::OnEvent, this));
}

CImeWnd::~CImeWnd()
{
	m_pEvtSet->unsubscribeEvent(EventSvrNotify::EventID, Subscriber(&CImeWnd::OnEvent, this));
	m_pEvtSet->unsubscribeEvent(EventSetSkin::EventID, Subscriber(&CImeWnd::OnEvent, this));
}

bool CImeWnd::OnEvent(EventArgs * e)
{
	return _HandleEvent(e);
}

void CImeWnd::OnSetSkin(EventArgs *e)
{
	SendMessage(WM_DESTROY,0,0);
	SetWindowPos(0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	CREATESTRUCT cs;
	cs.cx = 0;
	cs.cy = 0;
	SendMessage(WM_CREATE, 0, (LPARAM)&cs);
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

HWND CImeWnd::Create(LPCTSTR pszTitle,HWND hParent)
{
	return CSimpleWnd::Create(pszTitle,WS_DISABLED|WS_POPUP,WS_EX_TOOLWINDOW,0,0,0,0, hParent,NULL);
}

void CImeWnd::Show(BOOL bShow)
{
	if(!IsWindow()) return;
	if(bShow)
		SetWindowPos(HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE|SWP_SHOWWINDOW);
	else if(CSimpleWnd::IsWindowVisible())
		ShowWindow(SW_HIDE);
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

SWND CImeWnd::OnSetSwndCapture(SWND swnd)
{
	SWND ret =  SwndContainerImpl::OnSetSwndCapture(swnd);
	m_canReleaseCapture = FALSE;
	SLOG_INFO("m_canReleaseCapture:FALSE");
	return ret;
}

}
