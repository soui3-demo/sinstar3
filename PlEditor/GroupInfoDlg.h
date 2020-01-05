#pragma once

#include "GroupManager.h"

class CGroupInfoDlg : public SHostDialog
{

	GROUPINFO *m_groupInfo;
public:
	CGroupInfoDlg(GROUPINFO * pGroupInfo);
	~CGroupInfoDlg(void);

protected:
	void OnOK();
	//soui消息
	EVENT_MAP_BEGIN()
		EVENT_ID_COMMAND(IDOK,OnOK)
	EVENT_MAP_END()

protected:
	BOOL OnInitDialog(HWND hWnd, LPARAM lParam);

	//HostWnd真实窗口消息处理
	BEGIN_MSG_MAP_EX(CGroupInfoDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		CHAIN_MSG_MAP(SHostDialog)
	END_MSG_MAP()

};
