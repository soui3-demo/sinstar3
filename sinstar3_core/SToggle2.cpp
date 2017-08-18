#include "StdAfx.h"
#include "SToggle2.h"

SToggle2::SToggle2(void)
{
}

SToggle2::~SToggle2(void)
{
}

void SOUI::SToggle2::OnPaint(IRenderTarget *pRT)
{
	ISkinObj *pSkin = GetToggle()?m_pSkin2:m_pSkin1;
	int iState = IIF_STATE3(GetState(),0,1,2);
	CRect rcWnd = GetClientRect();
	pSkin->Draw(pRT,rcWnd,iState);
}
