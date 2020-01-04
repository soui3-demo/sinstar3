#include "StdAfx.h"
#include "GroupInfoDlg.h"

CGroupInfoDlg::CGroupInfoDlg(void):SHostDialog(UIRES.LAYOUT.dlg_group)
{
	memset(&m_groupInfo,0,sizeof(m_groupInfo));
}

CGroupInfoDlg::~CGroupInfoDlg(void)
{
}

void CGroupInfoDlg::OnOK()
{
	SStringA strName = S_CT2A(FindChildByID(R.id.edit_group_name)->GetWindowText());
	SStringA strEditor = S_CT2A(FindChildByID(R.id.edit_group_editor)->GetWindowText());
	SStringA strRemark = S_CT2A(FindChildByID(R.id.edit_group_remark)->GetWindowText());
	strcpy_s(m_groupInfo.szName,49,strName.c_str());
	strcpy_s(m_groupInfo.szEditor,49,strEditor.c_str());
	strcpy_s(m_groupInfo.szRemark,199,strRemark.c_str());
	EndDialog(IDOK);
}

BOOL CGroupInfoDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	FindChildByID(R.id.edit_group_name)->SetWindowText(S_CA2T(m_groupInfo.szName));
	FindChildByID(R.id.edit_group_editor)->SetWindowText(S_CA2T(m_groupInfo.szEditor));
	FindChildByID(R.id.edit_group_remark)->SetWindowText(S_CA2T(m_groupInfo.szRemark));
	return TRUE;
}
