#pragma once
#include "ImeWnd.h"

namespace SOUI {

#define TIMER_DELAY_HIDE 100

	class STipWnd : public CImeWnd
	{
	public:
		STipWnd(SEventSet *pEvtSets);
		~STipWnd();

		void SetTip(const SStringT & strTitle, const SStringT & strTip);

	protected:
		void OnTimer(UINT_PTR idEvent);
		void OnMouseMove(UINT nFlags, CPoint point);
		BEGIN_MSG_MAP_EX(STipWnd)
			MSG_WM_MOUSEMOVE(OnMouseMove)
			MSG_WM_TIMER(OnTimer)
			CHAIN_MSG_MAP(CImeWnd)
		END_MSG_MAP()

	protected:
		void OnClose();
		EVENT_MAP_BEGIN()
			EVENT_ID_COMMAND(R.id.btn_close, OnClose)
		EVENT_MAP_END()

	};
}

