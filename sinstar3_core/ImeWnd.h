#pragma once

namespace SOUI
{

class CImeWnd : public SHostWnd
{
public:
	CImeWnd(LPCTSTR pszLayout);

	HWND Create();
	void Show(BOOL bShow);
protected:

	BOOL OnSetCursor(HWND wnd, UINT nHitTest, UINT message);
	void OnMouseMove(UINT nFlags, CPoint point);

	BEGIN_MSG_MAP_EX(CImeWnd)
		MSG_WM_SETCURSOR(OnSetCursor)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		CHAIN_MSG_MAP(SHostWnd)
	END_MSG_MAP()
};

}
