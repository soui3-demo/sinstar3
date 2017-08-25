#include "StdAfx.h"
#include "CompWnd.h"

#define SIZE_BELOW 5

namespace SOUI
{
	CCompWnd::CCompWnd(void)
		:CImeWnd(UIRES.LAYOUT.wnd_composition)
		,m_bLocated(FALSE)
		,m_nCaretHeight(30)
		,m_pCandContainer(NULL)
	{
	}

	CCompWnd::~CCompWnd(void)
	{
	}


	void CCompWnd::SetCompStr(const SStringT &strComp)
	{
		m_strComp = strComp;
		if(IsWindow())
		{
			FindChildByID(R.id.txt_comps)->SetWindowText(m_strComp);

			ClearCandidate();

			SStringT strCands[] = {L"启程输入法3",L"SOUI",L"启程软件",L"fuck"};
			SStringT strComps[] = {L"abzd",L"aaaa",L"bbbb",L""};
			int nOffset= strComp.GetLength()%3;
			SetCandidateInfo(strCands+nOffset,strComps+nOffset,2);
		}
	}

	SStringT CCompWnd::GetCompStr() const
	{
		return m_strComp;
	}

	void CCompWnd::MoveTo(CPoint pt,int nCaretHeight)
	{
		m_bLocated = TRUE;
		m_ptCaret = pt;
		m_nCaretHeight = nCaretHeight;
		SetWindowPos(0,m_ptCaret.x,m_ptCaret.y + m_nCaretHeight + SIZE_BELOW,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
		if(m_bShow && !IsWindowVisible())
		{
			CImeWnd::Show(TRUE);
		}
	}

	void CCompWnd::Show(BOOL bShow)
	{
		if(m_bLocated)
		{
			CImeWnd::Show(bShow);	
		}
		m_bShow = bShow;
		if(!bShow)
		{
			m_bLocated = FALSE;
		}
	}

	int CCompWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		int nRet = __super::OnCreate(lpCreateStruct);
		if(nRet != 0) return nRet;

		m_pCandContainer = FindChildByName(R.name.cand_container);
		SASSERT(m_pCandContainer);
		m_nCandSize = 0;
		SWindow * pCand = m_pCandContainer->GetWindow(GSW_FIRSTCHILD);
		while(pCand)
		{
			if(pCand->IsClass(SCandView::GetClassName()))
				m_nCandSize++;
			pCand = pCand->GetWindow(GSW_NEXTSIBLING);
		}
		return 0;
	}

	void CCompWnd::SetCandidateInfo(const SStringT * strCands, const SStringT * strComps,int nSize)
	{
		SASSERT(nSize <= m_nCandSize);
		if(IsWindow())
		{
			SWindow * pCand = m_pCandContainer->GetWindow(GSW_FIRSTCHILD);
			int iCand = 0;
			while(pCand)
			{
				if(pCand->IsClass(SCandView::GetClassName()))
				{
					SCandView *pCand2 = (SCandView*)pCand;
					pCand2->SetVisible(TRUE);
					pCand2->SetData(strCands[iCand],strComps[iCand]);
					iCand ++;
				}
				pCand = pCand->GetWindow(GSW_NEXTSIBLING);
			}
		}
	}

	void CCompWnd::ClearCandidate()
	{
		SWindow * pCand = m_pCandContainer->GetWindow(GSW_FIRSTCHILD);
		while(pCand)
		{
			if(pCand->IsClass(SCandView::GetClassName()))
			{
				pCand->SetVisible(FALSE,TRUE);
			}
			pCand = pCand->GetWindow(GSW_NEXTSIBLING);
		}
	}

}
