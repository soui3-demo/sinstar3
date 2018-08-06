#include "stdafx.h"
#include "STipWnd.h"

namespace SOUI {

	STipWnd::STipWnd(SEventSet *pEvtSets):CImeWnd(pEvtSets,UIRES.LAYOUT.wnd_tip)
	{
	}


	STipWnd::~STipWnd()
	{
	}

	void STipWnd::SetTip(const SStringT & strTitle, const SStringT & strTip)
	{
		FindChildByID(R.id.txt_tip_title)->SetWindowTextW(strTitle);
		FindChildByID(R.id.txt_tip_content)->SetWindowTextW(strTip);
		SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_SHOWWINDOW);
		CSimpleWnd::SetTimer(TIMER_DELAY_HIDE, 5000, NULL);
	}

	void STipWnd::OnTimer(UINT_PTR idEvent)
	{
		if (idEvent == TIMER_DELAY_HIDE)
		{
			OnClose();
		}
		else
		{
			SetMsgHandled(FALSE);
		}
	}

	void STipWnd::OnWindowPosChanging(LPWINDOWPOS lpWndPos)
	{
		if (!(lpWndPos->flags&SWP_NOSIZE))
		{
			CRect rcWorkArea;
			SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
			lpWndPos->x = (rcWorkArea.Width() - lpWndPos->cx) / 2 + rcWorkArea.left;
			lpWndPos->y = (rcWorkArea.Height() - lpWndPos->cy) / 2 + rcWorkArea.top;
			lpWndPos->flags &= ~SWP_NOMOVE;
		}
	}

	void STipWnd::OnMouseMove(UINT nFlags, CPoint point)
	{
		CSimpleWnd::SetTimer(TIMER_DELAY_HIDE, 3000, NULL);
		SetMsgHandled(FALSE);
	}

	void STipWnd::OnClose()
	{
		DestroyWindow();
	}
}
