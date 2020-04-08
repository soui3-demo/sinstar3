#include "StdAfx.h"
#include "SPhraseCand.h"

namespace SOUI
{
	SPhraseCand::SPhraseCand(void)
	{
		m_bDisplay=0;
	}

	SPhraseCand::~SPhraseCand(void)
	{
	}

	void SPhraseCand::OnPaint(IRenderTarget *pRT)
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
		pRT->TextOut(pt.x,pt.y,m_strIndex,m_strIndex.GetLength());
		pRT->MeasureText(m_strIndex,m_strIndex.GetLength(),&szBlock);
		pt.x += szBlock.cx;

		pRT->SetTextColor(m_crCand);
		pRT->TextOut(pt.x,pt.y,m_strCand,m_strCand.GetLength());
		pRT->MeasureText(m_strCand,m_strCand.GetLength(),&szBlock);
		pt.x += szBlock.cx;

		pRT->SetTextColor(crDef);
		AfterPaint(pRT,painter);
	}


	void SPhraseCand::SetCandData(const BYTE* pbyCandData)
	{
		const char * p = (const char *)pbyCandData;		
		m_strCand = S_CW2T(SStringW((WCHAR*)(p+3)+p[0],p[2]-p[0]));
		RequestRelayout();
	}

	CSize SPhraseCand::GetDesiredSize(int nParentWid, int nParentHei)
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

		pRT->Release();
		return szRet;
	}

}
