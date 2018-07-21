#pragma once

namespace SOUI
{

	class CConfigDlg: public SHostDialog
	{
	public:
		CConfigDlg(void);
		~CConfigDlg(void);

	protected:
		void UpdataCtrl();
		BOOL OnInitDialog(HWND wnd, LPARAM lParam);
		
		EVENT_MAP_BEGIN()			
		EVENT_MAP_END()

		BEGIN_MSG_MAP_EX(CConfigDlg)
			MSG_WM_INITDIALOG(OnInitDialog)
			CHAIN_MSG_MAP(SHostDialog)
		END_MSG_MAP()
	};

}