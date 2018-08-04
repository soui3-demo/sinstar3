#include "stdafx.h"
#include "SpCharWnd.h"

namespace SOUI {
	CSpCharWnd::CSpCharWnd(SEventSet *pEvtSet):CImeWnd(pEvtSet,UIRES.LAYOUT.wnd_spchar)
	{
	}


	CSpCharWnd::~CSpCharWnd()
	{
	}
	void CSpCharWnd::OnClose()
	{
		DestroyWindow();
	}
}

