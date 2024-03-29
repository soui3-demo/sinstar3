// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include <helper/SAdapterBase.h>
#include "DataCenter.h"

struct IGroupValidListener{
	virtual void OnGroupVaildChanged(int iGroup,bool bValid) = 0;
};

class CGroupAdapter : public SMcAdapterBase
{
public:
	CGroupAdapter(IGroupValidListener *pListener);

	int AddGroup(const GROUPINFO & gi);

	void RemoveAll();
protected:

	virtual int getCount();

	virtual void getView(int position, SWindow * pItem, pugi::xml_node xmlTemplate);

	virtual SStringW GetColumnName(int iCol) const;

	bool OnCheckChanged(EventArgs *e);
protected:
	SArray<GROUPINFO> m_arrGroupInfo;
	IGroupValidListener	* m_pListener;
};

class CMainDlg : public SHostWnd, public TAutoEventMapReg<CMainDlg>,IGroupValidListener
{
	SAutoRefPtr<CGroupAdapter> m_groupAdapter;
	SProgress	* m_prog;
	CDataCenter	  m_dataCenter;
public:
	CMainDlg();
	~CMainDlg();

protected:
	void OnClose();
	void OnMaximize();
	void OnRestore();
	void OnMinimize();
	void OnSize(UINT nType, CSize size);
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);

	bool IsDataReady() const;

protected:
	virtual void OnGroupVaildChanged(int iGroup,bool bValid);

	void OnMenuSelected(EventArgs *e);

	void OnProgStart(EventArgs *e);
	void OnProgRun(EventArgs *e);
	void OnProgEnd(EventArgs *e);

	void OnGroupChanged();

	void OnLvContextMenu(EventArgs *e);
	void OnLoadWordRate();
	void OnImportWordRate();
	void OnSaveWordRateAsText();
	void OnSaveWordRateAsDat();

	//soui消息
	EVENT_MAP_BEGIN()
		EVENT_NAME_COMMAND(L"btn_close", OnClose)
		EVENT_NAME_COMMAND(L"btn_min", OnMinimize)
		EVENT_NAME_COMMAND(L"btn_max", OnMaximize)
		EVENT_NAME_COMMAND(L"btn_restore", OnRestore)
		EVENT_HANDLER(EventSelectMenu::EventID,OnMenuSelected)
		EVENT_HANDLER(EventProgStart::EventID,OnProgStart)
		EVENT_HANDLER(EventProgRun::EventID,OnProgRun)
		EVENT_HANDLER(EventProgEnd::EventID,OnProgEnd)
		EVENT_ID_HANDLER(R.id.lv_plgroup,EventCtxMenu::EventID,OnLvContextMenu)
		EVENT_ID_COMMAND(R.id.btn_load_wordrate,OnLoadWordRate)
		EVENT_ID_COMMAND(R.id.btn_import_wordrate,OnImportWordRate)
		EVENT_ID_COMMAND(R.id.btn_saveas_txt,OnSaveWordRateAsText)
		EVENT_ID_COMMAND(R.id.btn_saveas_wordrate,OnSaveWordRateAsDat)
	EVENT_MAP_END()
		
protected:
	//HostWnd真实窗口消息处理
	BEGIN_MSG_MAP_EX(CMainDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_SIZE(OnSize)
		CHAIN_MSG_MAP(SHostWnd)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()
};
