#include "stdafx.h"
#include "STipView.h"

namespace SOUI
{
	STipView::STipView()
	{
		GetEventSet()->addEvent(EVENTID(EventSwitchTip));
	}

	STipView::~STipView()
	{
	}

	void STipView::OnLButtonDown(UINT uFlag, CPoint pt)
	{
		EventSwitchTip evt(this);
		evt.bNext = true;
		FireEvent(evt);
	}

	void STipView::OnRButtonDown(UINT uFlag, CPoint pt)
	{
		EventSwitchTip evt(this);
		evt.bNext = false;
		FireEvent(evt);
	}

}
