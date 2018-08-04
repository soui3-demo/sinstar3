#include "stdafx.h"
#include "SkinMgrDlg.h"

namespace SOUI {
	CSkinMgrDlg::CSkinMgrDlg(SEventSet *pEvtSets):CSkinAwareWnd(pEvtSets, UIRES.LAYOUT.dlg_skinmgr)
	{
	}


	CSkinMgrDlg::~CSkinMgrDlg()
	{
	}
	void CSkinMgrDlg::OnClose()
	{
		DestroyWindow();
	}

	BOOL CSkinMgrDlg::OnInitDialog(HWND wnd, LPARAM lParam)
	{

		return FALSE;
	}
}
