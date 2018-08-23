#include "stdafx.h"
#include "BuildIndexProgWnd.h"

namespace SOUI
{
	CBuildIndexProgWnd::CBuildIndexProgWnd():SHostWnd(UIRES.LAYOUT.wnd_build_index_prog)
	{
	}


	CBuildIndexProgWnd::~CBuildIndexProgWnd()
	{
	}

	void CBuildIndexProgWnd::OnFinalMessage(HWND hWnd)
	{
		__super::OnFinalMessage(hWnd);
		delete this;
	}

}
