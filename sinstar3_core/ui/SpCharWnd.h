#pragma once
#include "ImeWnd.h"
#include "../InputState.h"

namespace SOUI
{
	class CSpCharWnd : public CImeWnd
	{
	public:
		CSpCharWnd(SEventSet *pEvtSet,ICmdListener *pListener);
		~CSpCharWnd();

	protected:
		void OnClose();
		void OnInsert();
		EVENT_MAP_BEGIN()
			EVENT_ID_COMMAND(R.id.btn_insert, OnInsert)
			EVENT_ID_COMMAND(R.id.btn_close, OnClose)
		EVENT_MAP_END()

		ICmdListener * m_pCmdListener;
	};
}
