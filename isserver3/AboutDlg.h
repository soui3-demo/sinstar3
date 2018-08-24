// MainDlg.h : interface of the CAboutDlg class
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#define ID_CHECK_UPDATE_NOW	 100

struct IUpdateIntervalObserver {
	virtual int GetUpdateInterval() const = 0;
	virtual void OnUpdateIntervalChanged(int nInterval) = 0;
};

class CAboutDlg : public SHostDialog
{
public:
	CAboutDlg(IUpdateIntervalObserver *pObserver);
	~CAboutDlg();


	static SStringT GetVersionInfo(DWORD &dwVer);
protected:
	void OnClose();
	void OnUpdateNow();
	void OnAutoUpdateClick();
	void OnCbUpdateIntervalSelChange(EventArgs *e);
	//soui消息
	EVENT_MAP_BEGIN()
		EVENT_ID_HANDLER(R.id.cbx_update_interval,EventCBSelChange::EventID,OnCbUpdateIntervalSelChange)
		EVENT_ID_COMMAND(R.id.chk_auto_update, OnAutoUpdateClick)
		EVENT_ID_COMMAND(R.id.btn_update_now,OnUpdateNow)
		EVENT_ID_COMMAND(R.id.btn_close, OnClose)
	EVENT_MAP_END()
		
	//HostWnd真实窗口消息处理
	BOOL OnInitDialog(HWND wnd, LPARAM lInitParam);
	BEGIN_MSG_MAP_EX(CAboutDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		CHAIN_MSG_MAP(SHostWnd)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()

private:
	IUpdateIntervalObserver * m_pObserver;
};
