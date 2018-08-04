#include "stdafx.h"
#include "SpCharWnd.h"

namespace SOUI {
	CSpCharWnd::CSpCharWnd(SEventSet *pEvtSet,ICmdListener *pListener)
		:CImeWnd(pEvtSet,UIRES.LAYOUT.wnd_spchar)
		, m_pCmdListener(pListener)
	{
	}

	CSpCharWnd::~CSpCharWnd()
	{
	}

	void CSpCharWnd::OnClose()
	{
		DestroyWindow();
	}

	void CSpCharWnd::OnInsert()
	{
		m_pCmdListener->OnCommand(CMD_INPUTSPCHAR, (LPARAM)_T("haha"));
	}
}

