#include "StdAfx.h"
#include "GroupInfoDlg.h"

CGroupInfoDlg::CGroupInfoDlg(GROUPINFO * pGroupInfo):SHostDialog(UIRES.LAYOUT.dlg_group),m_groupInfo(pGroupInfo)
{
}

CGroupInfoDlg::~CGroupInfoDlg(void)
{
}

void CGroupInfoDlg::OnOK()
{
	SStringW strName = S_CT2W(FindChildByID(R.id.edit_group_name)->GetWindowText());
	SStringW strEditor = S_CT2W(FindChildByID(R.id.edit_group_editor)->GetWindowText());
	SStringW strRemark = S_CT2W(FindChildByID(R.id.edit_group_remark)->GetWindowText());
	wcscpy_s(m_groupInfo->szName,49,strName.c_str());
	wcscpy_s(m_groupInfo->szEditor,49,strEditor.c_str());
	wcscpy_s(m_groupInfo->szRemark,199,strRemark.c_str());
	EndDialog(IDOK);
}

BOOL CGroupInfoDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	FindChildByID(R.id.edit_group_name)->SetWindowText(S_CW2T(m_groupInfo->szName));
	FindChildByID(R.id.edit_group_editor)->SetWindowText(S_CW2T(m_groupInfo->szEditor));
	FindChildByID(R.id.edit_group_remark)->SetWindowText(S_CW2T(m_groupInfo->szRemark));
	return TRUE;
}
