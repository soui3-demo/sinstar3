// MainDlg.cpp : implementation of the CAboutDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AboutDlg.h"	
#include "../helper/helper.h"

CAboutDlg::CAboutDlg(IUpdateIntervalObserver *pObserver)
	: SHostDialog(UIRES.LAYOUT.dlg_about)
	, m_pObserver(pObserver)
{
}

CAboutDlg::~CAboutDlg()
{
}

void CAboutDlg::OnClose()
{
	EndDialog(IDOK);
}

void CAboutDlg::OnUpdateNow()
{
	EndDialog(ID_CHECK_UPDATE_NOW);
}

void CAboutDlg::OnAutoUpdateClick()
{
	SWindow *pWnd = FindChildByID(R.id.chk_auto_update);
	if (!pWnd->IsChecked())
	{
		m_pObserver->OnUpdateIntervalChanged(0);
	}
	else
	{
		SComboBox * pCbxInterval = FindChildByID2<SComboBox>(R.id.cbx_update_interval);
		int iSel = pCbxInterval->GetCurSel();
		if (iSel == -1)
		{
			pCbxInterval->SetCurSel(0);
		}
		else
		{
			int interval[3] = { 7,30,90 };
			m_pObserver->OnUpdateIntervalChanged(interval[iSel % 3]);
		}
	}
}

void CAboutDlg::OnCbUpdateIntervalSelChange(EventArgs * e)
{
	EventCBSelChange *e2 = sobj_cast<EventCBSelChange>(e);
	SASSERT(e2);
	int iSel = e2->nCurSel;
	if (iSel >= 0 && iSel < 3)
	{
		int intervals[3] = { 7,30,90 };
		m_pObserver->OnUpdateIntervalChanged(intervals[iSel]);
		FindChildByID(R.id.chk_auto_update)->SetCheck(TRUE);
	}
}

SStringT CAboutDlg::GetVersionInfo(DWORD &dwVer)
{
	TCHAR szFileName[MAX_PATH];
	GetModuleFileName(NULL, szFileName, MAX_PATH);
	TCHAR szDesc[200];
	Helper_PEVersion(szFileName, &dwVer, NULL, szDesc);
	return SStringT(szDesc);
}

BOOL CAboutDlg::OnInitDialog(HWND wnd, LPARAM lInitParam)
{
	DWORD dwVer = 0;
	SStringT strDesc = GetVersionInfo(dwVer);
	TCHAR szVer[100];
	Helper_VersionString(dwVer, szVer);
	FindChildByID(R.id.txt_ver)->SetWindowText(szVer);
	FindChildByID(R.id.txt_name)->SetWindowText(strDesc);

	int nUpdateInterval = m_pObserver->GetUpdateInterval();
	FindChildByID(R.id.chk_auto_update)->SetCheck(nUpdateInterval != 0);
	SComboBox * pCbxInterval = FindChildByID2<SComboBox>(R.id.cbx_update_interval);
	pCbxInterval->GetEventSet()->setMutedState(true);
	if(nUpdateInterval == 0)
		pCbxInterval->SetCurSel(-1);
	else if (nUpdateInterval <= 7)
		pCbxInterval->SetCurSel(0);
	else if(nUpdateInterval<=30)
		pCbxInterval->SetCurSel(1);
	else
		pCbxInterval->SetCurSel(2);
	pCbxInterval->GetEventSet()->setMutedState(false);

	return 0;
}
