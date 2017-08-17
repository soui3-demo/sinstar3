#include "StdAfx.h"
#include "CompWnd.h"

#define SIZE_BELOW 5

namespace SOUI
{
	CCompWnd::CCompWnd(void)
		:CImeWnd(UIRES.LAYOUT.wnd_composition)
		,m_bLocated(FALSE)
		,m_nCaretHeight(30)
	{
	}

	CCompWnd::~CCompWnd(void)
	{
	}


	void CCompWnd::SetCompStr(const SStringT &strComp)
	{
		m_strComp = strComp;
		if(IsWindow())
			FindChildByID(R.id.txt_comps)->SetWindowText(m_strComp);
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

}
