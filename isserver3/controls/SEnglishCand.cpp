#include "StdAfx.h"
#include "SEnglishCand.h"

namespace SOUI
{
	const TCHAR KPhoneticLeft[] = _T("[");
	const TCHAR KPhoneticRight[] = _T("]");

	SEnglishCand::SEnglishCand(void)
		:m_bShowPhonetic(TRUE)
		,m_crIndex(RGBA(0,0,0,255))
		,m_crPhonetic(RGBA(0,0,255,255))
	{
		m_bDisplay=0;
	}

	SEnglishCand::~SEnglishCand(void)
	{
	}

	void SEnglishCand::OnPaint(IRenderTarget *pRT)
	{
		SPainter painter;
		BeforePaint(pRT,painter);

		COLORREF crDef = pRT->GetTextColor();
		CPoint pt = GetClientRect().TopLeft();
		CSize szBlock;

		pRT->SetTextColor(m_crIndex);
		pRT->TextOut(pt.x,pt.y,m_strIndex,m_strIndex.GetLength());
		pRT->MeasureText(m_strIndex,m_strIndex.GetLength(),&szBlock);
		pt.x += szBlock.cx;

		pRT->SetTextColor(m_crCand);
		pRT->TextOut(pt.x,pt.y,m_strCand,m_strCand.GetLength());
		pRT->MeasureText(m_strCand,m_strCand.GetLength(),&szBlock);
		pt.x += szBlock.cx;

		if(!m_strPhonetic.IsEmpty() && m_bShowPhonetic)
		{//todo:select phontic font
			pRT->SetTextColor(m_crPhonetic);
			pRT->TextOut(pt.x, pt.y, KPhoneticLeft, ARRAYSIZE(KPhoneticLeft) - 1);
			pRT->MeasureText(KPhoneticLeft, ARRAYSIZE(KPhoneticLeft) - 1, &szBlock);
			pt.x += szBlock.cx;

			int hei = szBlock.cy;
			IRenderObj *pOldFont;
			if (m_ftPhonetic)
			{
				pRT->SelectObject(m_ftPhonetic, &pOldFont);
			}
			pRT->MeasureText(m_strPhonetic, m_strPhonetic.GetLength(), &szBlock);
			int offset = (hei - szBlock.cy)/2;
			pRT->TextOut(pt.x,pt.y+offset,(LPCTSTR)m_strPhonetic, m_strPhonetic.GetLength());
			pt.x += szBlock.cx;
			if (m_ftPhonetic)
			{
				pRT->SelectObject(pOldFont);
			}
			pRT->TextOut(pt.x, pt.y, KPhoneticRight, ARRAYSIZE(KPhoneticRight) - 1);
		}

		pRT->SetTextColor(crDef);
		AfterPaint(pRT,painter);
	}

	LRESULT SEnglishCand::OnFlmInfo(UINT uMsg, WPARAM, LPARAM lp)
	{
		PFLMINFO pflmInfo = (PFLMINFO)lp;
		SLOG_INFO("font:" << pflmInfo->szAddFont);
		SStringA strFontInfo="face:";
		strFontInfo += pflmInfo->szAddFont;

		m_ftPhonetic = SFontPool::getSingleton().GetFont(S_CA2W(strFontInfo),GetScale());
		return 1;
	}


	void SEnglishCand::SetCandData(const BYTE* pbyCandData)
	{
		const char * p = (const char *)pbyCandData;		
		m_strCand = S_CA2T(SStringA(p+1,p[0]), CP_GB);
		p += p[0] + 1;
		if (p[0] > 0)
			m_strPhonetic = S_CA2T(SStringA(p + 1, p[0]), CP_GB);
		else
			m_strPhonetic.Empty();
		RequestRelayout();
	}

	CSize SEnglishCand::GetDesiredSize(int nParentWid, int nParentHei)
	{
		CSize szRet,sz;
		IRenderTarget *pRT=NULL;
		GETRENDERFACTORY->CreateRenderTarget(&pRT,0,0);

		BeforePaintEx(pRT);
		pRT->MeasureText(m_strIndex,m_strIndex.GetLength(),&sz);
		szRet.cx = sz.cx;
		szRet.cy = sz.cy;

		pRT->MeasureText(m_strCand,m_strCand.GetLength(),&sz);
		szRet.cx += sz.cx;
		szRet.cy = smax(szRet.cy,sz.cy);

		if(!m_strPhonetic.IsEmpty() && m_bShowPhonetic)
		{
			pRT->MeasureText(KPhoneticLeft, ARRAYSIZE(KPhoneticLeft)-1, &sz);
			szRet.cx += sz.cx;
			IRenderObj *pOldFont;
			if (m_ftPhonetic)
			{
				pRT->SelectObject(m_ftPhonetic, &pOldFont);
			}
			pRT->MeasureText(m_strPhonetic, m_strPhonetic.GetLength(), &sz);
			if (m_ftPhonetic)
			{
				pRT->SelectObject(pOldFont);
			}
			szRet.cx += sz.cx;
			szRet.cy = smax(szRet.cy, sz.cy);

			pRT->MeasureText(KPhoneticRight, ARRAYSIZE(KPhoneticRight) - 1, &sz);
			szRet.cx += sz.cx;
		}
		pRT->Release();
		return szRet;
	}

}
