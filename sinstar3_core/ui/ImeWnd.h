#pragma once
#include "skinawarewnd.h"

namespace SOUI
{

class CImeWnd : public CSkinAwareWnd
{
public:
	CImeWnd(SEventSet *pEvtSets,LPCTSTR pszLayout);
	virtual ~CImeWnd();

	HWND Create(LPCTSTR pszTitle,HWND hParent=NULL);
	void Show(BOOL bShow);

protected:
	virtual SWND OnSetSwndCapture(SWND swnd);
	virtual BOOL OnReleaseSwndCapture();

	BOOL m_canReleaseCapture;

protected:

	BOOL OnSetCursor(HWND wnd, UINT nHitTest, UINT message);
	void OnMouseMove(UINT nFlags, CPoint point);

	LRESULT OnMouseEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);

	BEGIN_MSG_MAP_EX(CImeWnd)
		MESSAGE_RANGE_HANDLER_EX(WM_MOUSEFIRST, WM_MOUSELAST, OnMouseEvent)
		MSG_WM_SETCURSOR(OnSetCursor)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_ACTIVATE(OnActivate)
		CHAIN_MSG_MAP(CSkinAwareWnd)
	END_MSG_MAP()
};

}
