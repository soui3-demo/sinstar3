#include "StdAfx.h"
#include "SCaption2.h"

namespace SOUI
{
	SCaption2::SCaption2(void):m_bDraging(FALSE)
	{
		GetEventSet()->addEvent(EVENTID(EventDragMove));
	}

	SCaption2::~SCaption2(void)
	{
	}

	void SCaption2::OnLButtonDown(UINT nFlags,CPoint pt)
	{
		SetCapture();
		m_bDraging = TRUE;
		m_ptClick = pt;
	}

	void SCaption2::OnLButtonUp(UINT nFlags,CPoint pt)
	{
		m_bDraging = FALSE;
		ReleaseCapture();
	}

	void SCaption2::OnMouseMove(UINT nFlags,CPoint pt)
	{
		if(m_bDraging)
		{
			EventDragMove evt(this);
			evt.ptMove = pt-m_ptClick;
			FireEvent(evt);
		}
	}

}
