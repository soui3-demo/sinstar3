// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MainDlg.h"	
#include "../include/FileHelper.h"
#include "GroupInfoDlg.h"
#include <ShellAPI.h>
#include "WordRate.h"
#include "../include/reg.h"

CGroupAdapter::CGroupAdapter(IGroupValidListener *pListener):m_pListener(pListener)
{

}

SOUI::SStringW CGroupAdapter::GetColumnName(int iCol) const
{
	const wchar_t * pszColNames[] = {
		L"col_group",L"col_size",L"col_editor",L"col_remark"
	};
	return pszColNames[iCol];
}

void CGroupAdapter::getView(int position, SWindow * pItem, pugi::xml_node xmlTemplate)
{
	if (pItem->GetChildrenCount() == 0)
	{
		pItem->InitFromXml(xmlTemplate);
	}
	SCheckBox *pCheck = pItem->FindChildByID2<SCheckBox>(R.id.chk_group_name);
	pCheck->SetWindowText(S_CW2T(m_arrGroupInfo[position].szName));
	pCheck->GetEventSet()->subscribeEvent(EventSwndStateChanged::EventID,Subscriber(&CGroupAdapter::OnCheckChanged,this));
	pCheck->GetEventSet()->setMutedState(true);
	pCheck->SetCheck(m_arrGroupInfo[position].bValid);
	pCheck->GetEventSet()->setMutedState(false);

	pItem->FindChildByID(R.id.txt_group_size)->SetWindowText(SStringT().Format(_T("%d"), m_arrGroupInfo[position].dwCount));
	pItem->FindChildByID(R.id.txt_editor)->SetWindowText(S_CW2T(m_arrGroupInfo[position].szEditor));
	pItem->FindChildByID(R.id.txt_remark)->SetWindowText(S_CW2T(m_arrGroupInfo[position].szRemark));
}

int CGroupAdapter::getCount()
{
	return m_arrGroupInfo.GetCount();
}

void CGroupAdapter::RemoveAll()
{
	m_arrGroupInfo.RemoveAll();
	notifyDataSetChanged();
}

int  CGroupAdapter::AddGroup(const GROUPINFO & gi)
{
	return m_arrGroupInfo.Add(gi);
}

bool CGroupAdapter::OnCheckChanged(EventArgs *e)
{
	EventSwndStateChanged *e2=sobj_cast<EventSwndStateChanged>(e);
	if(e2->CheckState(WndState_Check))
	{
		SWindow *pCheck = sobj_cast<SWindow>(e->sender);
		SItemPanel *pItemPanel = (SItemPanel*)pCheck->GetRoot();
		int iItem=pItemPanel->GetItemIndex();
		if(m_pListener) m_pListener->OnGroupVaildChanged(iItem,!!pCheck->IsChecked());
	}
	return true;
}


/////////////////////////////////////////////////////////////////////
CMainDlg::CMainDlg() : SHostWnd(_T("LAYOUT:XML_MAINWND"))
{
}

CMainDlg::~CMainDlg()
{
}

BOOL CMainDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	SMCListView *pPlView= FindChildByID2<SMCListView>(R.id.lv_plgroup);
	m_groupAdapter.Attach(new CGroupAdapter(this));
	pPlView->SetAdapter(m_groupAdapter);
	m_prog = FindChildByID2<SProgress>(R.id.prog_import);
	return 0;
}

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
	case R.id.menu_exit:
		OnClose();
		break;
	case R.id.menu_help:
		{
			SStringW strHelp = GETSTRING(R.string.help);
			ShellExecute(m_hWnd,_T("open"),strHelp,NULL,NULL,SW_SHOWNORMAL);
		}
		break;
	case R.id.menu_about:
		{
			SHostDialog about(UIRES.LAYOUT.dlg_about);
			about.DoModal();
		}
		break;
	}
	if(!IsDataReady())
		return;

	switch(e2->m_id)
	{
	case R.id.menu_load_rate_db:
		{
			CFileDialogEx openDlg(TRUE, _T("spl"), 0, 6, _T("启程词库文件(*.spl)\0*.spl\0All files (*.*)\0*.*\0\0"));
			if(openDlg.DoModal()==IDOK)
			{
				BOOL bOK=m_dataCenter.LoadRateProvider(openDlg.m_szFileName);
				if(bOK)
				{
					SStringT strMsg=SStringT().Format(_T("加载词频数据库成功，包含词条%u"),m_dataCenter.GetRateDbSize());
					SMessageBox(m_hWnd,strMsg,_T("提示"),MB_OK|MB_ICONINFORMATION);
				}else
				{
					SMessageBox(m_hWnd,_T("打开词频数据库失败"),_T("提示"),MB_OK|MB_ICONSTOP);
				}
			}
		}
		break;
	case R.id.menu_open_lib:
		{
			CFileDialogEx openDlg(TRUE, _T("spl"), 0, 6, _T("启程词库文件(*.spl)\0*.spl\0All files (*.*)\0*.*\0\0"));
			if(openDlg.DoModal()==IDOK)
			{
				m_dataCenter.LoadPL(openDlg.m_szFileName);
			}
		}
		break;
	case R.id.menu_save_lib:
		{
			CFileDialogEx openDlg(FALSE, _T("spl"), _T("new"), 6, _T("启程词库文件(*.spl)\0*.spl\0All files (*.*)\0*.*\0\0"));
			if(openDlg.DoModal()==IDOK)
			{
				m_dataCenter.SavePL(openDlg.m_szFileName);
			}
		}
		break;
	case R.id.menu_add_group:
		{
			GROUPINFO groupInfo={0};
			CGroupInfoDlg dlg(&groupInfo);
			if(dlg.DoModal()==IDOK)
			{
				m_dataCenter.AddGroup(groupInfo);
				OnGroupChanged();
			}
		}
		break;
	case R.id.menu_modify_group:
		{
			SMCListView *pList = FindChildByID2<SMCListView>(R.id.lv_plgroup);
			int iSel = pList->GetSel();
			if(iSel!=-1)
			{
				const std::vector<GROUPINFO> & group = m_dataCenter.GetGroup();
				GROUPINFO groupInfo = group[iSel];
				CGroupInfoDlg dlg(&groupInfo);
				if(dlg.DoModal()==IDOK)
				{
					m_dataCenter.SetGroup(iSel,groupInfo);
					OnGroupChanged();
				}
			}else
			{
				SMessageBox(m_hWnd,_T("先选择分组!"),_T("提示"),MB_OK|MB_ICONSTOP);
			}
		}
		break;
	case R.id.menu_import_to_group:
		{
			SMCListView *pList = FindChildByID2<SMCListView>(R.id.lv_plgroup);
			int iSel = pList->GetSel();
			if(iSel==-1)
			{
				GROUPINFO group={0};
				wcscpy(group.szName,L"new group");
				iSel = m_dataCenter.AddGroup(group);
				if(iSel == -1)
				{
					return;
				}
			}
			CFileDialogEx openDlg(TRUE, _T("txt"), 0, 6, _T("文本词库(*.txt)\0*.txt\0All files (*.*)\0*.*\0\0"));
			if(openDlg.DoModal()==IDOK)
			{
				SStringT strMin = FindChildByID(R.id.edit_rate_min)->GetWindowText();
				SStringT strMax = FindChildByID(R.id.edit_rate_max)->GetWindowText();
				SStringT strDef = FindChildByID(R.id.edit_rate_default)->GetWindowText();
				BYTE byMin = (BYTE)_ttoi(strMin);
				BYTE byMax = (BYTE)_ttoi(strMax);
				BYTE byDef = (BYTE)_ttoi(strDef);
				m_dataCenter.Import2Group(openDlg.m_szFileName,byMin,byMax,byDef,iSel);
			}
		}
		break;
	case R.id.menu_export_from_group:
		{
			SMCListView *pList = FindChildByID2<SMCListView>(R.id.lv_plgroup);
			int iSel = pList->GetSel();
			if(iSel!=-1)
			{
				GROUPINFO groupInfo = m_dataCenter.GetGroup()[iSel];
				SStringT  defName=S_CW2T(groupInfo.szName);
				CFileDialogEx openDlg(FALSE, _T("txt"), defName, 6, _T("文本词库(*.txt)\0*.txt\0All files (*.*)\0*.*\0\0"));
				if(openDlg.DoModal()==IDOK)
				{
					m_dataCenter.ExportGroup(openDlg.m_szFileName,iSel);
				}
			}else
			{
				SMessageBox(m_hWnd,_T("先选择分组!"),_T("提示"),MB_OK|MB_ICONSTOP);
			}
		}
		break;
	case R.id.menu_clear_group:
		m_dataCenter.Clear();
		OnGroupChanged();
		break;
	case R.id.menu_del_group:
		{
			SMCListView *pList = FindChildByID2<SMCListView>(R.id.lv_plgroup);
			int iSel = pList->GetSel();
			if(iSel!=-1)
			{
				m_dataCenter.EraseGroup(iSel);
			}else
			{
				SMessageBox(m_hWnd,_T("先选择分组!"),_T("提示"),MB_OK|MB_ICONSTOP);
			}
		}
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
	SLOG_INFO("max="<<e2->dwMax);
}

void CMainDlg::OnProgRun(EventArgs *e)
{
	EventProgRun *e2 = sobj_cast<EventProgRun>(e);
	m_prog->SetValue(e2->dwValue);
	SLOG_INFO("value="<<e2->dwValue<<" max="<<e2->dwMax);
}

void CMainDlg::OnProgEnd(EventArgs *e)
{
	EventProgEnd *e2 = sobj_cast<EventProgEnd>(e);
	m_prog->SetVisible(FALSE,TRUE);
	if(e2->bUpdateUI)
		OnGroupChanged();
	SLOG_INFO("end");
}

void CMainDlg::OnGroupChanged()
{
	m_groupAdapter->RemoveAll();
	const std::vector<GROUPINFO> & group = m_dataCenter.GetGroup();
	for(int i=0;i<group.size();i++)
	{
		m_groupAdapter->AddGroup(group[i]);
	}
	m_groupAdapter->notifyDataSetChanged();
}

bool CMainDlg::IsDataReady() const
{
	if(!m_dataCenter.IsReady())
	{
		SMessageBox(m_hWnd,_T("正在后台整理数据,请稍等"),_T("提示"),MB_OK|MB_ICONSTOP);
		return false;
	}
	return true;
}

void CMainDlg::OnGroupVaildChanged(int iGroup,bool bValid)
{
	m_dataCenter.ValidGroup((BYTE)iGroup,bValid);
}

void CMainDlg::OnLvContextMenu(EventArgs *e)
{
	SMenu menu;
	menu.LoadMenu(UIRES.smenu.MENU_EDIT);
	EventCtxMenu *e2=sobj_cast<EventCtxMenu>(e);
	CPoint pt = e2->pt;
	ClientToScreen(&pt);

	SendMessage(WM_LBUTTONDOWN,0,MAKELPARAM(e2->pt.x,e2->pt.y));
	SendMessage(WM_LBUTTONUP,0,MAKELPARAM(e2->pt.x,e2->pt.y));

	UINT uCmd = menu.TrackPopupMenu(TPM_RETURNCMD,pt.x,pt.y,m_hWnd);
	if(uCmd != 0)
	{
		EventSelectMenu evt(NULL);
		evt.m_id = uCmd;
		OnMenuSelected(&evt);
	}
}

void CMainDlg::OnLoadWordRate()
{
	CFileDialogEx openDlg(TRUE, _T("dat"), _T("wordrate"), 6, _T("启程字频表(*.dat)\0*.dat\0All files (*.*)\0*.*\0\0"));
	if(openDlg.DoModal()==IDOK)
	{
		if(!CWordRate::LoadFromFile(openDlg.m_szFileName))
		{
			SMessageBox(m_hWnd,_T("加载字频表数据失败"),_T("错误"),MB_OK|MB_ICONSTOP);
		}else
		{
			SMessageBox(m_hWnd,_T("加载字频表数据成功"),_T("提示"),MB_OK);
		}
	}
}

void CMainDlg::OnImportWordRate()
{
	CFileDialogEx openDlg(TRUE, _T("txt"), 0, 6, _T("文本文件(*.txt)\0*.txt\0All files (*.*)\0*.*\0\0"));
	if(openDlg.DoModal()==IDOK)
	{
		CWordRate::Clear();
		int nWords = CWordRate::Import(openDlg.m_szFileName);
		SStringT strMsg = SStringT().Format(_T("成功导入%d个字频数据"),nWords);
		SMessageBox(m_hWnd,strMsg,_T("提示"),MB_OK);
	}
}

void CMainDlg::OnSaveWordRateAsText()
{
	CFileDialogEx openDlg(FALSE, _T("txt"), _T("启程字频表"), 6, _T("文本文件(*.txt)\0*.txt\0All files (*.*)\0*.*\0\0"));
	if(openDlg.DoModal()==IDOK)
	{
		CWordRate::Export(openDlg.m_szFileName);
		ShellExecute(NULL,_T("open"),openDlg.m_szFileName,NULL,NULL,SW_SHOWNORMAL);
	}
}

void CMainDlg::OnSaveWordRateAsDat()
{
	CFileDialogEx openDlg(FALSE, _T("dat"), _T("wordrate"), 6, _T("启程字频表(*.dat)\0*.dat\0All files (*.*)\0*.*\0\0"));
	if(openDlg.DoModal()==IDOK)
	{
		CWordRate::SaveToFile(openDlg.m_szFileName);
	}

}
