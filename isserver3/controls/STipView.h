#pragma once

namespace SOUI
{
	SEVENT_BEGIN(EventSwitchTip, EVT_EXTERNAL_BEGIN + 800)
		bool bNext;
	SEVENT_END()

	class STipView : public SWindow
	{
		SOUI_CLASS_NAME(STipView,L"tipview")
	public:
		STipView();
		~STipView();

	protected:
		void OnLButtonDown(UINT uFlag, CPoint pt);
		void OnRButtonDown(UINT uFlag, CPoint pt);

		SOUI_MSG_MAP_BEGIN()
			MSG_WM_LBUTTONDOWN(OnLButtonDown)
			MSG_WM_LBUTTONDBLCLK(OnLButtonDown)
			MSG_WM_RBUTTONDOWN(OnRButtonDown)
			MSG_WM_RBUTTONDBLCLK(OnRButtonDown)
		SOUI_MSG_MAP_END()
	};

}

