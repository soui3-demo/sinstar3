#pragma once
#include "SkinAwareWnd.h"

namespace SOUI {
	class CSkinMgrDlg :public CSkinAwareWnd
	{
	public:
		CSkinMgrDlg(SEventSet *pEvtSets);
		~CSkinMgrDlg();

		void OnClose();

		EVENT_MAP_BEGIN()
			EVENT_ID_COMMAND(R.id.btn_close, OnClose)
		EVENT_MAP_END()


	protected:
		BOOL OnInitDialog(HWND wnd, LPARAM lParam);

		BEGIN_MSG_MAP_EX(CSkinMgrDlg)
			MSG_WM_INITDIALOG(OnInitDialog)
			CHAIN_MSG_MAP(CSkinAwareWnd)
		END_MSG_MAP()
	};
}
