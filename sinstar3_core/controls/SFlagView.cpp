#include "stdafx.h"
#include "SFlagView.h"

namespace SOUI {
	SFlagView::SFlagView()
	{
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

	void SFlagView::OnPaint(IRenderTarget *pRT)
	{
		CRect rcClient = GetClientRect();
		if (m_imeFlag)
		{
			CRect rcImg(0, 0, m_imeFlag->Width(), m_imeFlag->Height());
			UINT mode = MAKELONG(EM_STRETCH, kMedium_FilterLevel);
			pRT->DrawBitmapEx(rcClient, m_imeFlag, rcImg, mode);
		}
		else if (m_defFlag)
		{
			m_defFlag->Draw(pRT, rcClient, 0);
		}
	}
}

