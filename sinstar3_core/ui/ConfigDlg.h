#pragma once

namespace SOUI
{

	class CConfigDlg: public SHostDialog
	{
	public:
		CConfigDlg(void);
		~CConfigDlg(void);

	protected:
		BOOL OnInitDialog(HWND wnd, LPARAM lParam);

		void OnGotoPage(int nID);
		void OnSettingRangeChanged(EventArgs *e);
		void OnSettingViewPortChanged(EventArgs *e);
		EVENT_MAP_BEGIN()
			EVENT_ID_HANDLER(R.id.sv_setting,EventScrollViewSizeChanged::EventID,OnSettingRangeChanged)
			EVENT_ID_HANDLER(R.id.sv_setting,EventScrollViewOriginChanged::EventID,OnSettingViewPortChanged)
			EVENT_ID_COMMAND_RANGE(R.id.cfg_habit,R.id.cfg_about,OnGotoPage)
		EVENT_MAP_END()

		BEGIN_MSG_MAP_EX(CConfigDlg)
			MSG_WM_INITDIALOG(OnInitDialog)
			CHAIN_MSG_MAP(SHostDialog)
		END_MSG_MAP()

		int	m_yTitles[R.id.cfg_about-R.id.cfg_habit];
	};

}