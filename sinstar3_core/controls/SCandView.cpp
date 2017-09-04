#include "StdAfx.h"
#include "SCandView.h"

namespace SOUI
{
	SCandView::SCandView(void):m_byRate(0)
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
			cWild = dataCenterAutoLocker->GetData().m_compInfo.cWild;
		}
		for(int i=0;i<m_strComp.GetLength();i++)
		{
			if(i<m_strInput.GetLength() && m_strInput[i] == cWild)
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


	void SCandView::SetCandData(const SStringT& strInput,const BYTE* pbyCandData)
	{
		m_strInput = strInput;
		m_byRate = pbyCandData[0];
		const BYTE * p = pbyCandData+1;
		m_strCand = S_CA2T(SStringA((const char*)p+1,p[0]));
		p+=p[0]+1;
		m_strComp = S_CA2T(SStringA((const char*)p+1,p[0]));
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
