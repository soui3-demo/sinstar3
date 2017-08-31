#include "StdAfx.h"
#include "InputWnd.h"

#define SIZE_BELOW 5

namespace SOUI
{
	CInputWnd::CInputWnd(void)
		:CImeWnd(UIRES.LAYOUT.wnd_composition)
		,m_bLocated(FALSE)
		,m_nCaretHeight(30)
		,m_pCandContainer(NULL)
		,m_pCompWnd(NULL)
	{
	}

	CInputWnd::~CInputWnd(void)
	{
	}


	void CInputWnd::SetCompStr(const SStringT &strComp)
	{
		m_inputInfo.strComposition = strComp;
		OnInputInfoChanged(TRUE,FALSE);
	}

	SStringT CInputWnd::GetCompStr() const
	{
		return m_inputInfo.strComposition;
	}

	void CInputWnd::MoveTo(CPoint pt,int nCaretHeight)
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

	void CInputWnd::Show(BOOL bShow)
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

	int CInputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		int nRet = __super::OnCreate(lpCreateStruct);
		if(nRet != 0) return nRet;

		m_pCompWnd = FindChildByName(R.name.txt_comps);
		m_pCandContainer = FindChildByName(R.name.cand_container);
		SASSERT(m_pCandContainer&&m_pCompWnd);
		m_nPageSize = 0;
		SWindow * pCand = m_pCandContainer->GetWindow(GSW_FIRSTCHILD);
		while(pCand)
		{
			if(pCand->IsClass(SCandView::GetClassName()))
				m_nPageSize++;
			pCand = pCand->GetWindow(GSW_NEXTSIBLING);
		}
		return 0;
	}

	void CInputWnd::SetCandidateInfo(const SArray<SStringT> & strCands, const SArray<SStringT> & strComps,int nSize)
	{
		m_iPage = 0;
		m_inputInfo.nCands = nSize;
		m_inputInfo.strCands.Copy(strCands);
		m_inputInfo.strComps.Copy(strComps);
		FindChildByID(R.id.btn_prevpage)->SetVisible(FALSE,TRUE);
		FindChildByID(R.id.btn_nextpage)->SetVisible(nSize > m_nPageSize,TRUE);
			
		OnInputInfoChanged(FALSE,TRUE);
	}


	void CInputWnd::OnSetSkin(EventArgs *e)
	{
		OnDestroy();
		CREATESTRUCT cs;
		cs.cx=0;
		cs.cy=0;
		OnCreate(&cs);
		OnInputInfoChanged(TRUE,TRUE);
	}

	void CInputWnd::OnInputInfoChanged(BOOL bComp,BOOL bCand)
	{
		if(bComp)
		{
			m_pCompWnd->SetWindowText(m_inputInfo.strComposition);
		}

		if(bCand)
		{
			int iBegin = m_iPage * m_nPageSize;
			int iEnd   = smin(iBegin + m_nPageSize,m_inputInfo.nCands);


			SWindow * pCand = m_pCandContainer->GetWindow(GSW_FIRSTCHILD);
			int iCand = iBegin;
			while(pCand && iCand<iEnd)
			{
				if(pCand->IsClass(SCandView::GetClassName()))
				{
					SCandView *pCand2 = (SCandView*)pCand;
					pCand2->SetVisible(TRUE);
					pCand2->SetData(m_inputInfo.strComposition,m_inputInfo.strCands[iCand],m_inputInfo.strComps[iCand]);
					iCand ++;
				}
				pCand = pCand->GetWindow(GSW_NEXTSIBLING);
			}

			while(iCand < iBegin + m_nPageSize && pCand)
			{
				if(pCand->IsClass(SCandView::GetClassName()))
				{
					SCandView *pCand2 = (SCandView*)pCand;
					pCand2->SetVisible(FALSE);
					iCand ++;
				}
			}
		}

	}

	void CInputWnd::OnBtnNextPage()
	{
		if(m_iPage*m_nPageSize<m_inputInfo.nCands)
		{
			m_iPage ++;
			FindChildByID(R.id.btn_prevpage)->SetVisible(TRUE,TRUE);
			if(m_iPage*m_nPageSize>=m_inputInfo.nCands)
				FindChildByID(R.id.btn_nextpage)->SetVisible(FALSE,TRUE);
		}
	}

	void CInputWnd::OnBtnPrevPage()
	{
		if(m_iPage>0)
		{
			m_iPage--;
			FindChildByID(R.id.btn_nextpage)->SetVisible(TRUE,TRUE);
			if(m_iPage==0)
				FindChildByID(R.id.btn_prevpage)->SetVisible(FALSE,TRUE);
		}
	}

	void CInputWnd::ClearCands()
	{
		m_inputInfo.strCands.RemoveAll();
		m_inputInfo.strComps.RemoveAll();
		m_inputInfo.nCands= 0;
		FindChildByID(R.id.btn_nextpage)->SetVisible(FALSE,TRUE);
		FindChildByID(R.id.btn_prevpage)->SetVisible(FALSE,TRUE);
	}

}
