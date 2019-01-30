#include "StdAfx.h"
#include "SCandView.h"

namespace SOUI
{
	SCandView::SCandView(void):m_byRate(0),m_cWild(0), m_crShadow(CR_INVALID), m_ptShadowOffset(1,1)
	{
		m_crCand[CAND_NORMAL] = RGBA(0, 0, 0, 255);
		m_crCand[CAND_GBK] = m_crCand[CAND_FORECAST] = 
			m_crCand[CAND_BLENDPY] = m_crCand[CAND_USERDEF] = 
			m_crCand[CAND_USERCMD] = CR_INVALID;
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

		if (m_crShadow != CR_INVALID)
		{//draw shadow
			COLORREF crOld = pRT->SetTextColor(m_crShadow);
			CPoint pt2 = pt + m_ptShadowOffset;
			pRT->TextOut(pt2.x, pt2.y, m_strCand, m_strCand.GetLength());
			pRT->SetTextColor(crOld);
		}

		COLORREF crCand = CR_INVALID;
		switch (m_byRate)
		{
		case RATE_GBK:
			crCand = m_crCand[CAND_GBK];
			break;
		case RATE_FORECAST:
			crCand = m_crCand[CAND_FORECAST];
			break;
		case RATE_MIXSP:
			crCand = m_crCand[CAND_BLENDPY];
			break;
		case RATE_USERDEF:
		case RATE_USERCMD:
			crCand = m_crCand[CAND_USERDEF];
			break;
		default:
			crCand = m_crCand[CAND_NORMAL];
			break;
		}
		pRT->SetTextColor(crCand);
		pRT->TextOut(pt.x,pt.y,m_strCand,m_strCand.GetLength());
		pRT->MeasureText(m_strCand,m_strCand.GetLength(),&szBlock);
		pt.x += szBlock.cx;

		if(m_cWild!=0 && m_strInput.Find(m_cWild)!=-1)
		{
			for(int i=0;i<m_strComp.GetLength();i++)
			{
				if(i<m_strInput.GetLength() && m_strInput[i] == m_cWild)
					pRT->SetTextColor(m_crWild);
				else
					pRT->SetTextColor(m_crComp);
				pRT->TextOut(pt.x,pt.y,(LPCTSTR)m_strComp+i,1);
				pRT->MeasureText((LPCTSTR)m_strComp+i,1,&szBlock);
				pt.x += szBlock.cx;
			}
		}else if(m_strComp.GetLength()>m_strInput.GetLength())
		{
			SStringT strComp = m_strComp.Right(m_strComp.GetLength()-m_strInput.GetLength());
			pRT->SetTextColor(m_crComp);
			pRT->TextOut(pt.x,pt.y,(LPCTSTR)strComp,strComp.GetLength());
		}

		pRT->SetTextColor(crDef);
		AfterPaint(pRT,painter);
	}


	void SCandView::SetCandData(TCHAR cWild,const SStringT& strInput,const BYTE* pbyCandData)
	{
		m_cWild = cWild;
		m_strInput = strInput;

		m_byRate = pbyCandData[0];
		const BYTE * p = pbyCandData+1;
		m_strCand = S_CA2T(SStringA((const char*)p+1,p[0]));
		if(cWild!=0 && m_byRate != RATE_USERCMD)
		{
			p+=p[0]+1;
			m_strComp = S_CA2T(SStringA((const char*)p+1,p[0]));
		}else
		{
			m_strComp.Empty();
		}
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
		if (m_crShadow != CR_INVALID)
		{
			szRet.cx += m_ptShadowOffset.x;
			szRet.cy += m_ptShadowOffset.y;
		}
		szRet.cx += sz.cx;
		szRet.cy = smax(szRet.cy,sz.cy);

		SStringT strComp;
		if(m_cWild!=0 && m_strInput.Find(m_cWild)!=-1)
		{
			strComp = m_strComp;
		}else if(m_strComp.GetLength()>m_strInput.GetLength())
		{
			strComp = m_strComp.Right(m_strComp.GetLength()-m_strInput.GetLength());
		}
		if(!strComp.IsEmpty()) 
		{
			pRT->MeasureText(strComp,strComp.GetLength(),&sz);
			szRet.cx += sz.cx;
			szRet.cy = smax(szRet.cy,sz.cy);
		}
		pRT->Release();
		return szRet;
	}

}
