#pragma once
namespace SOUI
{
	typedef struct tagBLUREQUITY {
		char szFrom[7];
		char szTo[7];
	}BLUREQUILT;

	class CAddBlurDlg: public SHostDialog
	{
	public:
		CAddBlurDlg();
		~CAddBlurDlg();

		SStringA m_strFrom;
		SStringA m_strTo;
	protected:
		void OnOK();

		void OnCbxTuneChange(EventArgs *e);
		void OnCbxRhymeChange(EventArgs *e);
		void OnCbxFullChange(EventArgs *e);

		EVENT_MAP_BEGIN()
			EVENT_ID_HANDLER(R.id.cbx_tune_blur, EventCBSelChange::EventID, OnCbxTuneChange)
			EVENT_ID_HANDLER(R.id.cbx_rhyme_blur, EventCBSelChange::EventID, OnCbxRhymeChange)
			EVENT_ID_HANDLER(R.id.cbx_full_blur, EventCBSelChange::EventID, OnCbxFullChange)
			EVENT_ID_COMMAND(R.id.btn_ok,OnOK)
		EVENT_MAP_END()
	protected:
		BOOL OnInitDialog(HWND wnd, LPARAM lInitParam);

		BEGIN_MSG_MAP_EX(CAddBlurDlg)
			MSG_WM_INITDIALOG(OnInitDialog)
			CHAIN_MSG_MAP(__super)
		END_MSG_MAP()
	};

}

