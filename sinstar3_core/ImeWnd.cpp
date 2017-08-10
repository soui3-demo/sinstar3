#include "stdafx.h"
#include "ImeWnd.h"

namespace SOUI
{


CImeWnd::CImeWnd():SHostWnd(UIRES.LAYOUT.XML_MAINWND)
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
	CRect rcWnd = GetWindowRect();
	if(!rcWnd.PtInRect(point))
	{
		::ReleaseCapture();
	}
}

void CImeWnd::SetCompStr(const SStringT &strComp)
{
	m_strComp = strComp;
	if(IsWindow())
		FindChildByID(R.id.txt_comps)->SetWindowText(m_strComp);
}

SStringT CImeWnd::GetCompStr() const
{
	return m_strComp;
}

}
