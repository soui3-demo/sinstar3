#pragma once
#include <helper/SDpiHelper.hpp>
#include "global_def.h"

class CUpdateInfoDlg : public SHostDialog , public SDpiHandler<CUpdateInfoDlg>
{
public:
	CUpdateInfoDlg(EventCheckUpdateResult * pResult);
	~CUpdateInfoDlg();

protected:
	BOOL OnInitDialog(HWND wnd, LPARAM lInitParam);
	BEGIN_MSG_MAP_EX(CKeyMapDlg)
		CHAIN_MSG_MAP(SDpiHandler<CUpdateInfoDlg>)
		MSG_WM_INITDIALOG(OnInitDialog)
		CHAIN_MSG_MAP(SHostDialog)
	END_MSG_MAP()

private:
	EventCheckUpdateResult * m_pResult;
};

