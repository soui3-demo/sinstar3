#include "stdafx.h"
#include "UpdateInfoDlg.h"
#include "../helper/helper.h"

CUpdateInfoDlg::CUpdateInfoDlg(EventCheckUpdateResult * pResult):SHostDialog(UIRES.LAYOUT.dlg_update_info),m_pResult(pResult)
{
}


CUpdateInfoDlg::~CUpdateInfoDlg()
{
}

BOOL CUpdateInfoDlg::OnInitDialog(HWND wnd, LPARAM lInitParam)
{
	TCHAR szVer[50];
	Helper_VersionString(m_pResult->dwCurVer, szVer);
	FindChildByID(R.id.txt_ver_cur)->SetWindowText(szVer);
	Helper_VersionString(m_pResult->dwNewVer, szVer);
	FindChildByID(R.id.txt_ver_new)->SetWindowText(szVer);

	SLink *pMore = FindChildByID2<SLink>(R.id.link_more);
	pMore->SetAttribute(L"href", m_pResult->strUrl);
	pMore->SetAttribute(L"tip", m_pResult->strUrl);

	SChatEdit *pEdit = FindChildByID2<SChatEdit>(R.id.edit_info);
	pEdit->AppendFormatText(m_pResult->strInfo,FALSE,FALSE);
	pEdit->SetSel(0);
	pEdit->SSendMessage(WM_VSCROLL,SB_TOP,0);
	return 0;
}
