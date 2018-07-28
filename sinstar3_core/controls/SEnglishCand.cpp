#include "StdAfx.h"
#include "SEnglishCand.h"

namespace SOUI
{
	SEnglishCand::SEnglishCand(void)
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

		if(!m_strPhontic.IsEmpty())
		{//todo:select phontic font
			pRT->SetTextColor(m_crPhontic);
			pRT->TextOut(pt.x,pt.y,(LPCTSTR)m_strPhontic, m_strPhontic.GetLength());
		}

		pRT->SetTextColor(crDef);
		AfterPaint(pRT,painter);
	}


	void SEnglishCand::SetCandData(const BYTE* pbyCandData)
	{
		const char * p = (const char *)pbyCandData;		
		m_strCand = S_CA2T(SStringA(p+1,p[0]));
		p += p[0] + 1;
		if (p[0] > 0)
			m_strPhontic.Format(_T("[%s]"), S_CA2T(SStringA(p + 1, p[0])));
		else
			m_strPhontic.Empty();
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

		if(!m_strPhontic.IsEmpty())
		{
			pRT->MeasureText(m_strPhontic, m_strPhontic.GetLength(),&sz);
			szRet.cx += sz.cx;
			szRet.cy = smax(szRet.cy,sz.cy);
		}
		pRT->Release();
		return szRet;
	}

}
