#pragma once

struct CheckUpdateResult {
	SStringW strUrl;
	SStringW strInfo;
	SStringW strNewUpdateUrl;
	DWORD	 dwClientVer;
	DWORD	 dwServerVer;

	DWORD    dwClientCurVer;
	DWORD	 dwServerCurVer;
};

class CUpdateInfoDlg : public SHostDialog
{
public:
	CUpdateInfoDlg(CheckUpdateResult * pResult);
	~CUpdateInfoDlg();

protected:
	BOOL OnInitDialog(HWND wnd, LPARAM lInitParam);
	BEGIN_MSG_MAP_EX(CKeyMapDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		CHAIN_MSG_MAP(__super)
	END_MSG_MAP()

private:
	CheckUpdateResult * m_pResult;
};

