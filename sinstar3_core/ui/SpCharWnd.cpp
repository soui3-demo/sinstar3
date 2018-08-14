#include "stdafx.h"
#include "SpCharWnd.h"

namespace SOUI {


	class SDropdownWnd : public SDropDownWnd
	{
	public:
		SDropdownWnd(ISDropDownOwner * pOwner) :SDropDownWnd(pOwner), m_pNavWnd(NULL) {}
		virtual BOOL Create(pugi::xml_node popupStyle) {
			HWND hParent = m_pOwner->GetDropDownOwner()->GetContainer()->GetHostHwnd();
			HWND hWnd = CSimpleWnd::Create(NULL, WS_POPUP, WS_EX_TOPMOST | WS_EX_TOOLWINDOW, 0, 0, 0, 0, hParent, 0);
			if (!hWnd) return FALSE;
			m_pOwner->OnCreateDropDown(this);
			if (popupStyle)
			{
				InitFromXml(popupStyle);
				m_pNavWnd = FindChildByName(L"navtext");
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
		, m_pCmdListener(pListener), m_pDropDownWnd(NULL)
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
		static SWindow *pOldSender= pSender;
		//m_pZoomin->SetWindowText(pSender->GetWindowText());
		if (m_pDropDownWnd&&(pOldSender!= pSender))
		{
			CPoint point = pSender->GetWindowRect().TopLeft();
			ClientToScreen(&point);
			m_pDropDownWnd->SetWndText(pSender->GetWindowText());
			int cx = 100, cy = 100, x = (point.x - 36) < 0 ? 0 : point.x - 36, y = point.y - cy - 20;
			m_pDropDownWnd->SetWindowPos(HWND_TOPMOST, x, y, cx, cy, SWP_SHOWWINDOW| SWP_NOACTIVATE);
			pOldSender = pSender;
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

		//m_pZoomin = FindChildByID(R.id.sp_zoomin);
		//SASSERT(m_pZoomin);
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
		m_pDropDownWnd = NULL;
	}
	void CSpCharWnd::OnInitFinished(pugi::xml_node xmlNode)
	{
		__super::OnInitFinished(xmlNode);
		if (!m_pDropDownWnd)
		{
			m_pDropDownWnd = new SDropdownWnd(this);
			m_pDropDownWnd->Create(xmlNode.child(L"popupStyle"));
			SASSERT(m_pDropDownWnd);
		}
	}
	bool CSpCharWnd::OnMouseLeave(EventArgs *)
	{
		if (m_pDropDownWnd)
		{
			m_pDropDownWnd->ShowWindow(SW_HIDE);			
		}
		return true;
	}
	void CSpCharWnd::OnDestroy()
	{
		if (m_pDropDownWnd)
		{
			m_pDropDownWnd->DestroyWindow();
		}
	}
}

