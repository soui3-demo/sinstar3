#include "stdafx.h"
#include "SpCharWnd.h"

namespace SOUI {


	class SZoominWnd : public SDropDownWnd
	{
	public:
		SZoominWnd(ISDropDownOwner * pOwner) :SDropDownWnd(pOwner), m_pNavWnd(NULL) {}
		virtual BOOL Create(pugi::xml_node popupStyle) {
			HWND hParent = m_pOwner->GetDropDownOwner()->GetContainer()->GetHostHwnd();
			HWND hWnd = CSimpleWnd::Create(NULL, WS_POPUP, WS_EX_TOPMOST | WS_EX_TOOLWINDOW, 0, 0, 0, 0, hParent, 0);
			if (!hWnd) return FALSE;
			m_pOwner->OnCreateDropDown(this);
			if (popupStyle)
			{
				InitFromXml(popupStyle);
				m_pNavWnd = FindChildByName(L"txt_zoomin");
				SASSERT(m_pNavWnd);
			}
			return TRUE;
		}

		void SetWndText(LPCTSTR strNavChar) {
			SASSERT(m_pNavWnd);
			m_pNavWnd->SetWindowText(strNavChar);
		}

	private:
		SWindow * m_pNavWnd;
	};
	

	CSpCharWnd::CSpCharWnd(SEventSet *pEvtSet,ICmdListener *pListener)
		:CImeWnd(pEvtSet,UIRES.LAYOUT.wnd_spchar)
		, m_pCmdListener(pListener), m_pZoominWnd(NULL)
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
		if (m_pZoominWnd)
		{
			CRect rcCell= pSender->GetWindowRect();
			ClientToScreen(&rcCell);
			m_pZoominWnd->SetWndText(pSender->GetWindowText());
			CRect rcZoomin = m_pZoominWnd->GetWindowRect();
			CPoint pt = rcCell.TopLeft();
			pt.x -= (rcZoomin.Width() - rcCell.Width()) / 2;
			pt.y -= rcZoomin.Height()+20;

			m_pZoominWnd->SetWindowPos(HWND_TOPMOST, pt.x, pt.y, 0, 0, SWP_SHOWWINDOW| SWP_NOACTIVATE|SWP_NOSIZE);
		}
	}

	int CSpCharWnd::OnCreate(LPCREATESTRUCT pCS)
	{
		return OnRecreateUI(pCS);
	}

	int CSpCharWnd::OnRecreateUI(LPCREATESTRUCT lpCreateStruct)
	{
		int nRet = __super::OnCreate(lpCreateStruct);
		if (nRet != 0) return nRet;

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
	
	SWindow * CSpCharWnd::GetDropDownOwner()
	{
		return this;
	}

	void CSpCharWnd::OnCreateDropDown(SDropDownWnd * pDropDown)
	{		
	}

	void CSpCharWnd::OnDestroyDropDown(SDropDownWnd * pDropDown)
	{
		m_pZoominWnd = NULL;
	}
	void CSpCharWnd::OnInitFinished(pugi::xml_node xmlNode)
	{
		__super::OnInitFinished(xmlNode);
		if (!m_pZoominWnd)
		{
			m_pZoominWnd = new SZoominWnd(this);
			m_pZoominWnd->Create(xmlNode.child(L"popupStyle"));
			SASSERT(m_pZoominWnd);
		}
	}
	bool CSpCharWnd::OnMouseLeave(EventArgs *)
	{
		if (m_pZoominWnd)
		{
			m_pZoominWnd->ShowWindow(SW_HIDE);			
		}
		return true;
	}
	void CSpCharWnd::OnDestroy()
	{
		if (m_pZoominWnd)
		{
			m_pZoominWnd->DestroyWindow();
		}
	}
}

