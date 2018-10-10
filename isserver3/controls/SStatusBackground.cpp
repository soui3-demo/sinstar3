#include "StdAfx.h"
#include "SStatusBackground.h"

namespace SOUI
{
	SStatusBackground::SStatusBackground(void)
	{
	}

	SStatusBackground::~SStatusBackground(void)
	{
	}

	CSize SStatusBackground::GetDesiredSize(int nParentWid, int nParentHei)
	{
		if (!m_pBgSkin)
		{
			return __super::GetDesiredSize(nParentWid, nParentHei);
		}
		return m_pBgSkin->GetSkinSize();
	}
}
