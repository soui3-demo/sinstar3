#include "StdAfx.h"
#include "ConfigDlg.h"

namespace SOUI
{
	CConfigDlg::CConfigDlg(void):SHostDialog(UIRES.LAYOUT.dlg_config)
	{
	}

	CConfigDlg::~CConfigDlg(void)
	{
	}

	//CSettingsLocal		g_SettingsL;
	//CSettingsCompSpec   g_SettingsCompSpec;
	
	void CConfigDlg::UpdataCtrl()
	{

	}

	BOOL CConfigDlg::OnInitDialog( HWND wnd, LPARAM lParam )
	{
		UpdataCtrl();
		return FALSE;
	}
}
