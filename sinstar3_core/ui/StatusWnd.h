#pragma once

#include "ImeWnd.h"
#include "SkinMananger.h"

namespace SOUI
{
	class CStatusWnd :
		public CImeWnd
	{
	public:
		CStatusWnd(void);
		~CStatusWnd(void);

	protected:
		int OnCreate(LPCREATESTRUCT lpCreateStruct);

		void OnRButtonUp(UINT nFlags,CPoint pt);
		
		void OnInitMenuPopup(HMENU menuPopup, UINT nIndex, BOOL bSysMenu);

		BEGIN_MSG_MAP_EX(CStatusWnd)
			MSG_WM_INITMENUPOPUP(OnInitMenuPopup)
			MSG_WM_CREATE(OnCreate)
			MSG_WM_RBUTTONUP(OnRButtonUp)
			CHAIN_MSG_MAP(CImeWnd)
		END_MSG_MAP()

	protected:

		void OnDragStatus(EventArgs *e);
		void OnSetSkin(EventArgs *e);

		void OnBtnExtend();
		void OnBtnShrink();
		
		EVENT_MAP_BEGIN()
			EVENT_HANDLER(EventDragMove::EventID,OnDragStatus)
			EVENT_HANDLER(EventSetSkin::EventID,OnSetSkin)
			EVENT_ID_COMMAND(R.id.btn_status_shrink,OnBtnShrink)
			EVENT_ID_COMMAND(R.id.btn_status_extend,OnBtnExtend)
		EVENT_MAP_END()
	protected:
		SStatusBackground * m_pBackGround;
		CSkinMananger  m_pSkinManager;
	};

}
