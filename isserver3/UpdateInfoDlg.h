#pragma once

#include "global_def.h"

class CUpdateInfoDlg : public SHostDialog
{
public:
	CUpdateInfoDlg(EventCheckUpdateResult * pResult);
	~CUpdateInfoDlg();

protected:
	BOOL OnInitDialog(HWND wnd, LPARAM lInitParam);
	BEGIN_MSG_MAP_EX(CKeyMapDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		CHAIN_MSG_MAP(__super)
	END_MSG_MAP()

private:
	EventCheckUpdateResult * m_pResult;
};

