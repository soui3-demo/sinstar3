#include "StdAfx.h"
#include "CompWnd.h"

namespace SOUI
{
	CCompWnd::CCompWnd(void):CImeWnd(UIRES.LAYOUT.wnd_composition)
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

}
