#include "StdAfx.h"
#include "SStatusBackground.h"

namespace SOUI
{
	SStatusBackground::SStatusBackground(void):m_mode(MODE_EXTEND)
	{
	}

	SStatusBackground::~SStatusBackground(void)
	{
	}

	CSize SStatusBackground::GetDesiredSize(int nParentWid, int nParentHei)
	{
		if(m_mode == MODE_EXTEND)
		{
			return m_pSkinExtend->GetSkinSize();
		}else
		{
			return m_pSkinShrink->GetSkinSize();
		}
	}

	void SStatusBackground::OnPaint(IRenderTarget *pRT)
	{
		CRect rcWnd = GetClientRect();
		if(m_mode == MODE_EXTEND)
		{
			return m_pSkinExtend->Draw(pRT,rcWnd,0);
		}else
		{
			return m_pSkinShrink->Draw(pRT,rcWnd,0);
		}
	}

	void SStatusBackground::SetMode(SStatusBackground::MODE mode)
	{
		m_mode = mode;
		RequestRelayout();
	}

}
