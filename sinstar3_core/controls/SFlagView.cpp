#include "stdafx.h"
#include "SFlagView.h"

namespace SOUI {
	SFlagView::SFlagView():m_bSpellFlag(FALSE)
	{
		m_bMsgTransparent = FALSE;
	}


	SFlagView::~SFlagView()
	{
	}

	void SFlagView::SetImeFlagData(const IMEFLAGDATA * pData)
	{
		m_imeFlag = NULL;
		if (pData)
		{
			GETRENDERFACTORY->CreateBitmap(&m_imeFlag);
			m_imeFlag->Init(pData->wid, pData->hei, pData->rgba);
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
		if (m_bSpellFlag)
		{//拼音状态
			if (m_defFlag)
			{
				m_defFlag->Draw(pRT, rcClient, 1);	//绘制默认图中的第二个子图.
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
			m_defFlag->Draw(pRT, rcClient, 0);//绘制默认图中的第一个子图.
		}
	}
}

