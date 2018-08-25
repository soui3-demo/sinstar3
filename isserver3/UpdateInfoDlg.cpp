#include "stdafx.h"
#include "UpdateInfoDlg.h"
#include "../helper/helper.h"

CUpdateInfoDlg::CUpdateInfoDlg(CheckUpdateResult * pResult):SHostDialog(UIRES.LAYOUT.dlg_update_info),m_pResult(pResult)
{
}


CUpdateInfoDlg::~CUpdateInfoDlg()
{
}

BOOL CUpdateInfoDlg::OnInitDialog(HWND wnd, LPARAM lInitParam)
{
	TCHAR szVer[50];
	Helper_VersionString(m_pResult->dwClientCurVer, szVer);
	FindChildByID(R.id.txt_client_ver_cur)->SetWindowText(szVer);
	Helper_VersionString(m_pResult->dwClientVer, szVer);
	FindChildByID(R.id.txt_client_ver_new)->SetWindowText(szVer);

	Helper_VersionString(m_pResult->dwServerCurVer, szVer);
	FindChildByID(R.id.txt_svr_ver_cur)->SetWindowText(szVer);
	Helper_VersionString(m_pResult->dwServerVer, szVer);
	FindChildByID(R.id.txt_svr_ver_new)->SetWindowText(szVer);

	SLink *pMore = FindChildByID2<SLink>(R.id.link_more);
	pMore->SetAttribute(L"href", m_pResult->strUrl);
	pMore->SetAttribute(L"tip", m_pResult->strUrl);

	FindChildByID2<SChatEdit>(R.id.edit_info)->AppendFormatText(m_pResult->strInfo);

	return 0;
}
