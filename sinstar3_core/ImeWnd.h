#pragma once

namespace SOUI
{

class CImeWnd : public SHostWnd
{
public:
	CImeWnd(LPCTSTR pszLayout);

protected:

	BOOL OnSetCursor(HWND wnd, UINT nHitTest, UINT message);
	void OnMouseMove(UINT nFlags, CPoint point);

	BEGIN_MSG_MAP_EX(CImeWnd)
		MSG_WM_SETCURSOR(OnSetCursor)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		CHAIN_MSG_MAP(__super)
	END_MSG_MAP()
};

}
