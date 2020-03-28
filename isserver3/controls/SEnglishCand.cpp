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
		CRect rcText;
		GetTextRect(rcText);
		CPoint pt = rcText.TopLeft();
		CSize szBlock;
		UINT uAlign = GetStyle().GetTextAlign();
		pRT->MeasureText(_T("A"),1,&szBlock);
		if(uAlign & DT_VCENTER)
		{
			pt.y += (rcText.Height()-szBlock.cy)/2;
		}else if(uAlign & DT_BOTTOM)
		{
			pt.y = rcText.bottom - szBlock.cy;
		}

		pRT->SetTextColor(m_crIndex);
		pRT->MeasureText(m_strIndex,m_strIndex.GetLength(),&szBlock);
		pRT->TextOut(pt.x,pt.y,m_strIndex,m_strIndex.GetLength());
		pt.x += szBlock.cx;

		pRT->SetTextColor(m_crCand);
		pRT->TextOut(pt.x,pt.y,m_strCand,m_strCand.GetLength());
		pRT->MeasureText(m_strCand,m_strCand.GetLength(),&szBlock);
		pt.x += szBlock.cx;

		if(!m_strPhonetic.IsEmpty() && m_bShowPhonetic)
		{
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
		if(pflmInfo)
		{
			SLOG_INFO("font:" << pflmInfo->szAddFont);
			SStringW strFontInfo=L"face:";
			strFontInfo += pflmInfo->szAddFont;

			m_ftPhonetic = SFontPool::getSingleton().GetFont(strFontInfo,GetScale());
		}else
		{
			m_ftPhonetic=NULL;
		}
		return 1;
	}


	void SEnglishCand::SetCandData(const BYTE* pbyCandData)
	{
		const BYTE* p=pbyCandData;
		m_strCand = SStringW((const WCHAR*)(p+1),p[0]);
		p += p[0]*2 + 1;
		if (p[0] > 0)
			m_strPhonetic = SStringW((const WCHAR*)(p+1),p[0]);
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
