#pragma once

namespace SOUI
{
	class EventDragMove : public TplEventArgs<EventDragMove>
	{
		SOUI_CLASS_NAME(EventDragMove,L"on_drag_move")
	public:
		EventDragMove(SObject *pSender):TplEventArgs<EventDragMove>(pSender){}
		enum{EventID=EVENT_DRAGMOVE};

		CPoint ptMove;
	};

	class SCaption2 : public SWindow
	{
		SOUI_CLASS_NAME(SCaption2,L"caption2")
	public:
		SCaption2(void);
		~SCaption2(void);

	protected:
		
		void OnLButtonDown(UINT nFlags,CPoint pt);

		void OnLButtonUp(UINT nFlags,CPoint pt);

		void OnMouseMove(UINT nFlags,CPoint pt);

		SOUI_MSG_MAP_BEGIN()
			MSG_WM_LBUTTONDOWN(OnLButtonDown)
			MSG_WM_LBUTTONUP(OnLButtonUp)
			MSG_WM_MOUSEMOVE(OnMouseMove)
		SOUI_MSG_MAP_END()

		BOOL	m_bDraging;
		CPoint  m_ptClick;
	};

}
