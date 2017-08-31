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

		SStringT strCands[] = {L"启程输入法3",L"SOUI",L"启程软件",L"fuck",L"启程输入法3",L"SOUI",L"启程软件",L"fuck"};
		SStringT strComps[] = {L"abzd",L"aaaa",L"bbbb",L"",L"abzd",L"aaza",L"bbbb",L""};
		int nOffset= strComp.GetLength()%6;
		int nSize = smin(5,ARRAYSIZE(strComps)-nOffset-1);
		SetCandidateInfo(strCands+nOffset,strComps+nOffset,nSize);

		OnInputInfoChanged();
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

	void CInputWnd::SetCandidateInfo(const SStringT * strCands, const SStringT * strComps,int nSize)
	{
		SASSERT(nSize <= m_nCandSize);
		m_inputInfo.nCands = nSize;
		m_inputInfo.strCands.RemoveAll();
		for(int i=0;i<nSize;i++)
		{
			m_inputInfo.strCands.Add(strCands[i]);
		}
		m_inputInfo.strComps.RemoveAll();
		for(int i=0;i<nSize;i++)
		{
			m_inputInfo.strComps.Add(strComps[i]);
		}
	}


	void CInputWnd::OnSetSkin(EventArgs *e)
	{
		OnDestroy();
		CREATESTRUCT cs;
		cs.cx=0;
		cs.cy=0;
		OnCreate(&cs);
		OnInputInfoChanged();
	}

	void CInputWnd::OnInputInfoChanged()
	{
		m_pCompWnd->SetWindowText(m_inputInfo.strComposition);

		SWindow * pCand = m_pCandContainer->GetWindow(GSW_FIRSTCHILD);
		int iCand = 0;
		while(pCand && iCand<m_inputInfo.nCands)
		{
			if(pCand->IsClass(SCandView::GetClassName()))
			{
				SCandView *pCand2 = (SCandView*)pCand;
				pCand2->SetVisible(TRUE);
				pCand2->SetData(m_inputInfo.strCands[iCand],m_inputInfo.strComps[iCand]);
				iCand ++;
			}
			pCand = pCand->GetWindow(GSW_NEXTSIBLING);
		}

		while(iCand < m_nCandSize && pCand)
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
