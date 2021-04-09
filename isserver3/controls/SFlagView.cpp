#include "stdafx.h"
#include "SFlagView.h"
#include <helper/SDIBHelper.h>

namespace SOUI {
	SFlagView::SFlagView():m_bSpellFlag(FALSE),m_crTheme(CR_INVALID),m_bCapMode(FALSE)
	{
		m_bMsgTransparent = FALSE;
	}


	SFlagView::~SFlagView()
	{
	}

	void SFlagView::SetImeFlagData(LPBYTE pData,DWORD dwLen)
	{
		m_imeFlag = NULL;
		if (pData)
		{
			m_imeFlag.Attach(SResLoadFromMemory::LoadImage(pData, dwLen));
			if(m_crTheme != CR_INVALID)
			{
				SDIBHelper::Colorize(m_imeFlag,m_crTheme);
			}
		}
		Invalidate();
	}

	void SFlagView::ShowSpellFlag(BOOL bSpell)
	{
		m_bSpellFlag = bSpell;
		Invalidate();
	}

	void SFlagView::OnPaint(IRenderTarget *pRT)
	{
		CRect rcClient = GetClientRect();
		if(m_bCapMode && m_capFlag)
		{
			m_capFlag->DrawByIndex(pRT, rcClient, 0);
			return;
		}
		if (m_bSpellFlag)
		{//拼音状态
			if (m_defFlag)
			{
				m_defFlag->DrawByIndex(pRT, rcClient, 1);	//绘制默认图中的第二个子图.
			}
		}
		else if (m_imeFlag)
		{
			CRect rcImg(0, 0, m_imeFlag->Width(), m_imeFlag->Height());
			UINT mode = MAKELONG(EM_STRETCH, kMedium_FilterLevel);
			pRT->DrawBitmapEx(rcClient, m_imeFlag, rcImg, mode);
		}
		else if (m_defFlag)
		{
			m_defFlag->DrawByIndex(pRT, rcClient, 0);//绘制默认图中的第一个子图.
		}
	}

	void SFlagView::UpdateCapitalMode(BOOL bCap)
	{
		if(m_bCapMode != bCap)
		{
			m_bCapMode = bCap;
			Invalidate();
		}
	}

}

