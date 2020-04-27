#include "StdAfx.h"
#include "SCandView.h"

namespace SOUI
{
	SCandView::SCandView(void):m_byRate(0),m_cWild(0), m_crShadow(CR_INVALID), m_ptShadowOffset(1,1),m_maxCandWidth(250.0f,SLayoutSize::dp)
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

		//draw index
		pRT->SetTextColor(m_crIndex);
		pRT->TextOut(pt.x,pt.y,m_strIndex,m_strIndex.GetLength());
		pRT->MeasureText(m_strIndex,m_strIndex.GetLength(),&szBlock);
		pt.x += szBlock.cx;

		COLORREF crCand = CR_INVALID;
		if(m_bGbk)
		{
			crCand = m_crCand[CAND_GBK];
		}
		else
		{
			switch (m_byRate)
			{
			case RATE_FORECAST:
				crCand = m_crCand[CAND_FORECAST];
				break;
			case RATE_MIXSP:
				crCand = m_crCand[CAND_BLENDPY];
				break;
			case RATE_USERDEF:
			case RATE_USERJM:
				crCand = m_crCand[CAND_USERDEF];
				break;
			case RATE_USERCMD:
				crCand = m_crCand[CAND_USERCMD];
				break;
			default:
				crCand = m_crCand[CAND_NORMAL];
				break;
			}
		}
		if (crCand == CR_INVALID)
			crCand = m_crCand[CAND_NORMAL];
		pRT->SetTextColor(crCand);
		pRT->MeasureText(m_strCand,m_strCand.GetLength(),&szBlock);
		if(m_maxCandWidth.isValid())
		{
			szBlock.cx = min(szBlock.cx,m_maxCandWidth.toPixelSize(GetScale()));
		}
		if (m_crShadow != CR_INVALID)
		{//draw shadow
			COLORREF crOld = pRT->SetTextColor(m_crShadow);
			pRT->DrawText(m_strCand,m_strCand.GetLength(),CRect(pt+m_ptShadowOffset,szBlock),DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
			pRT->SetTextColor(crOld);
		}
		pRT->DrawText(m_strCand,m_strCand.GetLength(),CRect(pt,szBlock),DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
		if (m_crShadow != CR_INVALID)
		{
			pt.x += m_ptShadowOffset.x;
		}
		pt.x += szBlock.cx;

		if(m_byRate==RATE_MIXSP)
		{
			pRT->SetTextColor(m_crComp);
			SStringT strComp=SStringT().Format(_T("[%s]"),m_strComp);
			pRT->TextOut(pt.x,pt.y,(LPCTSTR)strComp,strComp.GetLength());
		}
		else if(m_cWild!=0 && m_strInput.Find(m_cWild)!=-1)
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
		m_bGbk = pbyCandData[1]!=0;
		const BYTE * p = pbyCandData+2;
		m_strCand = SStringW((const wchar_t*)(p+1),p[0]);
		m_strCand.Replace(_T('\n'),_T('¡ý'));
		if(cWild!=0 && m_byRate != RATE_USERCMD)
		{
			p+=p[0]*2+1;
			m_strComp = SStringW((const wchar_t*)(p+1),p[0]);
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
		if(m_maxCandWidth.isValid())
		{
			sz.cx = min(sz.cx,m_maxCandWidth.toPixelSize(GetScale()));
		}
		if (m_crShadow != CR_INVALID)
		{
			szRet.cx += m_ptShadowOffset.x;
			szRet.cy += m_ptShadowOffset.y;
		}
		szRet.cx += sz.cx;
		szRet.cy = smax(szRet.cy,sz.cy);

		SStringT strComp;
		if(m_byRate==RATE_MIXSP)
		{
			strComp=SStringT().Format(_T("[%s]"),m_strComp);
		}
		else if(m_cWild!=0 && m_strInput.Find(m_cWild)!=-1)
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
