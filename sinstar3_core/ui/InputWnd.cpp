#include "StdAfx.h"
#include "InputWnd.h"

#define SIZE_BELOW 5

namespace SOUI
{
	CInputWnd::CInputWnd(const InputContext *pCtx)
		:CImeWnd(UIRES.LAYOUT.wnd_composition)
		,m_bLocated(FALSE)
		,m_nCaretHeight(30)
		,m_pInputContext(pCtx)
		,m_iPage(0)
	{
	}

	CInputWnd::~CInputWnd(void)
	{
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

		return 0;
	}


	void CInputWnd::OnSetSkin(EventArgs *e)
	{
		OnDestroy();
		CREATESTRUCT cs;
		cs.cx=0;
		cs.cy=0;
		OnCreate(&cs);
		UpdateUI();
	}

	void CInputWnd::OnBtnNextPage()
	{
	}

	void CInputWnd::OnBtnPrevPage()
	{
	}


	void CInputWnd::UpdateUI()
	{
		switch(m_pInputContext->inState)
		{
		case INST_CODING:
			if(m_pInputContext->compMode == IM_SPELL)
			{
				SWindow * compSpell = FindChildByID(R.id.comp_spell);
				compSpell->SetVisible(TRUE,TRUE);
				compSpell->FindChildByID(R.id.txt_comps)->SetWindowText(SStringT(m_pInputContext->szInput,m_pInputContext->cInput));
			}
			else
			{
				SWindow * compNormal = FindChildByID(R.id.comp_normal);
				compNormal->SetVisible(TRUE,TRUE);
				compNormal->FindChildByID(R.id.txt_comps)->SetWindowText(SStringT(m_pInputContext->szInput,m_pInputContext->cInput));
			}
			//update candidate
			{
				FindChildByID(R.id.cand_normal)->SetVisible(TRUE,TRUE);
				SWindow * pCandContainer = FindChildByID(R.id.cand_container);

				int nPageSize = GetCandMax(pCandContainer);
				int iBegin = m_iPage * nPageSize;
				int iEnd   = smin(iBegin + nPageSize,m_pInputContext->sCandCount);

				SWindow * pCand = pCandContainer->GetWindow(GSW_FIRSTCHILD);
				int iCand = iBegin;
				while(pCand && iCand<iEnd)
				{
					if(pCand->IsClass(SCandView::GetClassName()))
					{
						SCandView *pCand2 = (SCandView*)pCand;
						pCand2->SetVisible(TRUE,TRUE);
						pCand2->SetCandData(SStringT(m_pInputContext->szInput,m_pInputContext->cInput),m_pInputContext->ppbyCandInfo[iCand]);
						iCand ++;
					}
					pCand = pCand->GetWindow(GSW_NEXTSIBLING);
				}

				while(iCand < iBegin + nPageSize && pCand)
				{
					if(pCand->IsClass(SCandView::GetClassName()))
					{
						SCandView *pCand2 = (SCandView*)pCand;
						pCand2->SetVisible(FALSE,TRUE);
						iCand ++;
					}
					pCand = pCand->GetWindow(GSW_NEXTSIBLING);
				}
			}
			break;
		case INST_USERDEF:
			{
				SWindow * compUmode = FindChildByID(R.id.comp_umode);
				compUmode->SetVisible(TRUE,TRUE);
				compUmode->FindChildByID(R.id.txt_comps)->SetWindowText(SStringT(m_pInputContext->szInput,m_pInputContext->cInput));
			}
			break;
		}
	}

	int CInputWnd::GetCandMax(SWindow *pCandContainer) const
	{
		int nRet = 0;
		SWindow * pCand = pCandContainer->GetWindow(GSW_FIRSTCHILD);
		while(pCand)
		{
			if(pCand->IsClass(SCandView::GetClassName()))
			{
				nRet ++;
			}
			pCand = pCand->GetWindow(GSW_NEXTSIBLING);
		}
		return nRet;
	}

}
