#include "stdafx.h"
#include "SCompBackground.h"

namespace SOUI
{
	SCompBackground::SCompBackground()
	{
	}


	SCompBackground::~SCompBackground()
	{
	}

	CSize SCompBackground::GetDesiredSize(int nParentWid, int nParentHei)
	{
		CSize szRet = __super::GetDesiredSize(nParentWid, nParentHei);
		if (m_pBgSkin)
		{
			CSize szBg = m_pBgSkin->GetSkinSize();
			if (!GetLayoutParam()->IsSpecifiedSize(Vert) && szRet.cy < szBg.cy)
			{
				szRet.cy = szBg.cy;
			}
			if (!GetLayoutParam()->IsSpecifiedSize(Horz) && szRet.cx < szBg.cx)
			{
				szRet.cx = szBg.cx;
			}
		}
		return szRet;
	}

}
