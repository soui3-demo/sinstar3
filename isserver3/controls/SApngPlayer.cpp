#include "stdafx.h"
#include "SApngPlayer.h"
#include "SSkinAPNG.h"

namespace SOUI
{

SApngPlayer::SApngPlayer() :m_aniSkin(NULL), m_iCurFrame(0),m_nNextInterval(0)
{

}

SApngPlayer::~SApngPlayer()
{
}


void SApngPlayer::OnPaint( IRenderTarget *pRT )
{	
	__super::OnPaint(pRT);
	if(m_aniSkin)
	{		
		m_aniSkin->DrawByIndex(pRT, GetWindowRect(),m_iCurFrame);
	}
}

void SApngPlayer::OnShowWindow( BOOL bShow, UINT nStatus )
{
	__super::OnShowWindow(bShow,nStatus);
	if(!IsVisible(TRUE))
	{
        GetContainer()->UnregisterTimelineHandler(this);
	}else if(m_aniSkin && m_aniSkin->GetStates()>1)
	{
        GetContainer()->RegisterTimelineHandler(this);
        if(m_aniSkin->GetFrameDelay()==0)
            m_nNextInterval = 90;
        else
            m_nNextInterval = m_aniSkin->GetFrameDelay()*10;
	}
}

void SApngPlayer::OnNextFrame()
{
    m_nNextInterval -= 10;
    if(m_nNextInterval <= 0 && m_aniSkin)
    {
        int nStates=m_aniSkin->GetStates();
        m_iCurFrame++;
        m_iCurFrame%=nStates;
        Invalidate();

        if(m_aniSkin->GetFrameDelay()==0)
            m_nNextInterval = 90;
        else
            m_nNextInterval =m_aniSkin->GetFrameDelay()*10;	
    }
}

HRESULT SApngPlayer::OnAttrSkin( const SStringW & strValue, BOOL bLoading )
{
	ISkinObj *pSkin = SSkinPoolMgr::getSingleton().GetSkin(strValue,GetScale());
	if(!pSkin) return E_FAIL;
	if(!pSkin->IsClass(SSkinAni::GetClassName())) return S_FALSE;
	m_aniSkin=static_cast<SSkinAni*>(pSkin);
    if(!bLoading)
    {
        m_iCurFrame = 0;
        if(m_aniSkin->GetFrameDelay()==0)
            m_nNextInterval = 90;
        else
            m_nNextInterval =m_aniSkin->GetFrameDelay()*10;	
    }
	return bLoading?S_OK:S_FALSE;
}

CSize SApngPlayer::GetDesiredSize( int nWid,int nHei )
{
	CSize szRet = __super::GetDesiredSize(nWid,nHei);
	CSize szSkin;
	if(m_aniSkin) szSkin=m_aniSkin->GetSkinSize();
	if(GetLayoutParam()->IsWrapContent(Horz))
		szRet.cx = szSkin.cx;
	else if(GetLayoutParam()->IsSpecifiedSize(Horz))
		szRet.cx = GetLayoutParam()->GetSpecifiedSize(Horz).toPixelSize(GetScale());
	if(GetLayoutParam()->IsWrapContent(Vert))
		szRet.cy = szSkin.cy;
	else if(GetLayoutParam()->IsSpecifiedSize(Vert))
		szRet.cy = GetLayoutParam()->GetSpecifiedSize(Vert).toPixelSize(GetScale());
	return szRet;
}


BOOL SApngPlayer::PlayFile( LPCTSTR pszFileName)
{
	SStringW key=S_CT2W(pszFileName);

	SSkinAni *pGifSkin = (SSkinAni*)SApplication::getSingleton().CreateSkinByName(SSkinAPNG::GetClassName());
	if(!pGifSkin) return FALSE;
	if(0==pGifSkin->LoadFromFile(pszFileName))
	{
		pGifSkin->Release();
		return FALSE;
	}

	m_aniSkin = pGifSkin;

	if(GetLayoutParam()->IsWrapContent(Any))
	{
		GetParent()->UpdateChildrenPosition();
	}
	if(IsVisible(TRUE))
	{
		GetContainer()->RegisterTimelineHandler(this);
	}
	return TRUE;
}

void SApngPlayer::OnDestroy()
{
    GetContainer()->UnregisterTimelineHandler(this);
    __super::OnDestroy();
}
}
