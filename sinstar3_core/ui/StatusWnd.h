#pragma once

#include "ImeWnd.h"
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
		int InitSkinMenu(HMENU hMenu, const SStringW &strSkinPath,int nStartId);
		SMap<int,SStringT> m_mapSkin;	//map of ID->skin path
	};

}
