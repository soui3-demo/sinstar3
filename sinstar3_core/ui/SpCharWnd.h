#pragma once
#include "ImeWnd.h"
namespace SOUI
{
	class CSpCharWnd : public CImeWnd
	{
	public:
		CSpCharWnd(SEventSet *pEvtSet);
		~CSpCharWnd();

	protected:
		void OnClose();
		EVENT_MAP_BEGIN()
			EVENT_ID_COMMAND(R.id.btn_close, OnClose)
		EVENT_MAP_END()

	};
}
