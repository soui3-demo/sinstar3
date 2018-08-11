#include "stdafx.h"
#include "SpCharWnd.h"

namespace SOUI {
	CSpCharWnd::CSpCharWnd(SEventSet *pEvtSet,ICmdListener *pListener)
		:CImeWnd(pEvtSet,UIRES.LAYOUT.wnd_spchar)
		, m_pCmdListener(pListener)
	{
	}

	CSpCharWnd::~CSpCharWnd()
	{
	}

	void CSpCharWnd::OnReposition(CPoint pt)
	{
		CDataCenter::getSingletonPtr()->GetData().m_ptSpchar = pt;
	}

	void CSpCharWnd::OnClose()
	{
		DestroyWindow();
	}

	void CSpCharWnd::OnInsert(int nID)
	{
		SWindow *pWnd = FindChildByID(nID);
		SStringT str = pWnd->GetWindowText();
		m_pCmdListener->OnCommand(CMD_INPUTSPCHAR, (LPARAM)(LPCTSTR)str);
	}

	void CSpCharWnd::OnCatalogChanged(EventArgs * e)
	{
		EventLBSelChanged *e2 = sobj_cast<EventLBSelChanged>(e);
		SASSERT(e2);
		SASSERT(e2->nNewSel < m_spLayouts.GetCount());
		SStringW spChars = m_spLayouts[e2->nNewSel];
		int i = 0;
		for (; i < spChars.GetLength() && i<50; i++)
		{
			wchar_t spchar = spChars[i];
			SStringW strSpchar(&spchar, 1);
			SWindow *pCell = FindChildByID(R.id.cell_00 + i);
			pCell->SetWindowText(S_CW2T(strSpchar));
			pCell->EnableWindow(TRUE);
		}
		for (; i < 50; i++)
		{
			SWindow *pCell = FindChildByID(R.id.cell_00 + i);
			pCell->SetWindowText(NULL);
			pCell->EnableWindow(FALSE);
		}
	}

	void CSpCharWnd::OnCellHover(EventArgs * e)
	{
		SWindow *pSender = sobj_cast<SWindow>(e->sender);
		SASSERT(pSender);
		m_pZoomin->SetWindowText(pSender->GetWindowText());
	}

	int CSpCharWnd::OnCreate(LPCREATESTRUCT pCS)
	{
		return OnRecreateUI(pCS);
	}

	int CSpCharWnd::OnRecreateUI(LPCREATESTRUCT lpCreateStruct)
	{
		int nRet = __super::OnCreate(lpCreateStruct);
		if (nRet != 0) return nRet;

		m_pZoomin = FindChildByID(R.id.sp_zoomin);
		SASSERT(m_pZoomin);
		SStringT strSpcharCfg = theModule->GetDataPath() + _T("\\data\\spchar.xml");
		pugi::xml_document xmlDoc;
		if (xmlDoc.load_file(strSpcharCfg))
		{
			m_spLayouts.RemoveAll();
			SListBox *pListBox = FindChildByID2<SListBox>(R.id.lst_catalog);
			pugi::xml_node nodeLayout = xmlDoc.child(L"spchar").child(L"layout");
			while (nodeLayout)
			{
				pListBox->AddString(nodeLayout.attribute(L"name").as_string());
				m_spLayouts.Add(nodeLayout.attribute(L"chars").as_string());
				nodeLayout = nodeLayout.next_sibling(L"layout");
			}
			pListBox->SetCurSel(0);
			EventLBSelChanged evt(NULL);
			evt.nNewSel = 0;
			OnCatalogChanged(&evt);
		}

		if (CDataCenter::getSingletonPtr()->GetData().m_ptSpchar.x < 0)
		{
			CenterWindow(GetActiveWindow());
		}
		else
		{
			CPoint &pt = CDataCenter::getSingletonPtr()->GetData().m_ptSpchar;
			SetWindowPos(NULL, pt.x, pt.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
		}
		return 0;
	}

}

