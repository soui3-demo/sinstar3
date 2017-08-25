#include "StdAfx.h"
#include "SCandView.h"

namespace SOUI
{
	SCandView::SCandView(void)
	{
		m_bDisplay=0;
	}

	SCandView::~SCandView(void)
	{
	}

	void SCandView::OnPaint(IRenderTarget *pRT)
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

		pRT->SetTextColor(m_crComp);
		pRT->TextOut(pt.x,pt.y,m_strCand,m_strCand.GetLength());
		pRT->MeasureText(m_strCand,m_strCand.GetLength(),&szBlock);
		pt.x += szBlock.cx;

		TCHAR cWild = 0;
		{
			CDataCenter::AutoLocker dataCenterAutoLocker = CDataCenter::GetAutoLockerInstance();
			cWild = dataCenterAutoLocker->getCompInfo().cWild;
		}
		for(int i=0;i<m_strComp.GetLength();i++)
		{
			if(m_strComp[i] == cWild)
				pRT->SetTextColor(m_crWild);
			else
				pRT->SetTextColor(m_crComp);
			pRT->TextOut(pt.x,pt.y,(LPCTSTR)m_strComp+i,1);
			pRT->MeasureText((LPCTSTR)m_strComp+i,1,&szBlock);
			pt.x += szBlock.cx;
		}

		pRT->SetTextColor(crDef);
		AfterPaint(pRT,painter);
	}


	void SCandView::SetData(const SStringT & strText, const SStringT &strComp)
	{
		m_strCand = strText;
		m_strComp = strComp;
		RequestRelayout();
	}

	CSize SCandView::GetDesiredSize(int nParentWid, int nParentHei)
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

		pRT->MeasureText(m_strComp,m_strComp.GetLength(),&sz);
		szRet.cx += sz.cx;
		szRet.cy = smax(szRet.cy,sz.cy);

		pRT->Release();
		return szRet;
	}

}
