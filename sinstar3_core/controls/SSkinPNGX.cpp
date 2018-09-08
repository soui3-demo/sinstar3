#include "StdAfx.h"
#include "SSkinPNGX.h"
#include <helper/SplitString.h>
#include <interface/imgdecoder-i.h>
#include <interface/render-i.h>


namespace SOUI
{

HRESULT SSkinPNGX::OnAttrDelay(const SStringW &strValue,BOOL bLoading)
{
	SStringWList strDelays;
	int nSegs = SplitString(strValue,L',',strDelays);
	m_nDelays.RemoveAll();
	for(int i=0;i<nSegs;i++)
	{
		int nDelay=0,nRepeat=1;
		SStringW strSub = strDelays[i];
		int nReaded = swscanf(strSub,L"%d[%d]",&nDelay,&nRepeat);
		for(int j=0;j<nRepeat;j++)
			m_nDelays.Add(nDelay);
	}
	return S_FALSE;
}

int SSkinPNGX::LoadFromFile(LPCTSTR pszFileName)
{
	return 0;
}

int SSkinPNGX::LoadFromMemory(LPVOID pBits,size_t szData)
{
	return 0;
}

int SSkinPNGX::GetStates()
{
	return m_nDelays.GetCount();
}

SIZE SSkinPNGX::GetSkinSize()
{
	CSize szSkin;
	if(m_pngx)
	{
		szSkin.cx = m_pngx->Width();
		szSkin.cy = m_pngx->Height();

		if(m_bVert)
			szSkin.cy/=m_nDelays.GetCount();
		else
			szSkin.cx/=m_nDelays.GetCount();
	}
	return szSkin;
}

long SSkinPNGX::GetFrameDelay(int iFrame/*=-1*/)
{
	if(iFrame == -1)
		iFrame = m_iFrame;
	SASSERT(iFrame>=0 && iFrame < GetStates());
	return m_nDelays[iFrame];
}

void SSkinPNGX::_Draw(IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState,BYTE byAlpha/*=0xFF*/)
{
	if(dwState!=-1) SelectActiveFrame(dwState);
	if(m_pngx)
	{
		CRect rcSrc(CPoint(0,0),GetSkinSize());
		if(m_bVert)
			rcSrc.OffsetRect(0,rcSrc.Height()*dwState);
		else
			rcSrc.OffsetRect(rcSrc.Width()*dwState,0);
		pRT->DrawBitmapEx(rcDraw,m_pngx,rcSrc,EM_STRETCH,byAlpha);
	}
}

}//end of namespace SOUI