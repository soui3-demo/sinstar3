// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MainDlg.h"	


CMainDlg::CMainDlg() : SHostWnd(_T("LAYOUT:XML_MAINWND"))
{
}

CMainDlg::~CMainDlg()
{
}

BOOL CMainDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	SMCListView *pPlView= FindChildByID2<SMCListView>(R.id.lv_plgroup);
	m_groupAdapter.Attach(new CGroupAdapter);
	pPlView->SetAdapter(m_groupAdapter);
	m_prog = FindChildByID2<SProgress>(R.id.prog_import);
	return 0;
}
//TODO:消息映射
void CMainDlg::OnClose()
{
	SNativeWnd::DestroyWindow();
}

void CMainDlg::OnMaximize()
{
	SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
}
void CMainDlg::OnRestore()
{
	SendMessage(WM_SYSCOMMAND, SC_RESTORE);
}
void CMainDlg::OnMinimize()
{
	SendMessage(WM_SYSCOMMAND, SC_MINIMIZE);
}

void CMainDlg::OnSize(UINT nType, CSize size)
{
	SetMsgHandled(FALSE);	
	SWindow *pBtnMax = FindChildByName(L"btn_max");
	SWindow *pBtnRestore = FindChildByName(L"btn_restore");
	if(!pBtnMax || !pBtnRestore) return;
	
	if (nType == SIZE_MAXIMIZED)
	{
		pBtnRestore->SetVisible(TRUE);
		pBtnMax->SetVisible(FALSE);
	}
	else if (nType == SIZE_RESTORED)
	{
		pBtnRestore->SetVisible(FALSE);
		pBtnMax->SetVisible(TRUE);
	}
}

void CMainDlg::OnMenuSelected(EventArgs *e)
{
	EventSelectMenu *e2=sobj_cast<EventSelectMenu>(e);
	switch(e2->m_id)
	{
	case R.id.menu_load_sys_lib:
		m_dataCenter.LoadSysPL();
		break;
	default:
		break;
	}
}

void CMainDlg::OnProgStart(EventArgs *e)
{
	EventProgStart *e2 = sobj_cast<EventProgStart>(e);
	m_prog->SetVisible(TRUE,TRUE);
	m_prog->SetRange(0,e2->dwMax);
}

void CMainDlg::OnProgRun(EventArgs *e)
{
	EventProgRun *e2 = sobj_cast<EventProgRun>(e);
	m_prog->SetValue(e2->dwValue);
}

void CMainDlg::OnProgEnd(EventArgs *e)
{
	m_prog->SetVisible(FALSE,TRUE);
	m_groupAdapter->RemoveAll();
	const std::vector<GROUPINFO> & group = m_dataCenter.GetGroup();
	for(int i=0;i<group.size();i++)
	{
		m_groupAdapter->AddGroup(group[i]);
	}
	m_groupAdapter->notifyDataSetChanged();
}

