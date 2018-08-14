#pragma once
#include "ImeWnd.h"
#include "../InputState.h"

namespace SOUI
{
	class SDropdownWnd;
	class CSpCharWnd : public CImeWnd, public ISDropDownOwner
	{
	public:
		CSpCharWnd(SEventSet *pEvtSet,ICmdListener *pListener);
		~CSpCharWnd();

	protected:
		virtual void OnReposition(CPoint pt);
		virtual int OnRecreateUI(LPCREATESTRUCT lpCreateStruct);
		virtual SWindow * GetDropDownOwner();
		virtual void OnCreateDropDown(SDropDownWnd * pDropDown);
		virtual void OnDestroyDropDown(SDropDownWnd * pDropDown);
		virtual void OnInitFinished(pugi::xml_node xmlNode);
		bool OnMouseLeave(EventArgs*);
		void OnDestroy();
	protected:
		void OnClose();
		void OnInsert(int nID);
		void OnCatalogChanged(EventArgs *e);
		void OnCellHover(EventArgs *e);
		EVENT_MAP_BEGIN()
			EVENT_ID_COMMAND(R.id.btn_close, OnClose)
			EVENT_ID_RANGE_HANDLER(R.id.cell_00,R.id.cell_49,EventSwndMouseHover::EventID,OnCellHover)
			EVENT_ID_COMMAND_RANGE(R.id.cell_00, R.id.cell_49, OnInsert)
			EVENT_ID_HANDLER(R.id.lst_catalog,EventLBSelChanged::EventID,OnCatalogChanged)
			EVENT_NAME_HANDLER(L"cell_parent",EventSwndMouseLeave::EventID,OnMouseLeave)
		EVENT_MAP_END()

	protected:
		int OnCreate(LPCREATESTRUCT pCS);
		BEGIN_MSG_MAP_EX(CSpCharWnd)
			MSG_WM_CREATE(OnCreate)
			MSG_WM_DESTROY(OnDestroy)
			CHAIN_MSG_MAP(__super)
		END_MSG_MAP()

		ICmdListener * m_pCmdListener;
		SStringWList m_spLayouts;
		//SWindow		 * m_pZoomin;
		SDropdownWnd * m_pDropDownWnd;
	};
}
