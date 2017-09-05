#include "StdAfx.h"
#include "SSpellView.h"

namespace SOUI
{
	static LPCTSTR g_szLine[]={_T("-"),_T("|"),_T("/"),_T("\\"),_T("<"),_T("*")};

	SSpellView::SSpellView(void)
		:m_ctx(NULL)
		,m_crSpell(RGBA(200,220,255,255))
		,m_crResult(RGBA(200,200,200,255))
		,m_crEdit(RGBA(255,255,0,255))
		,m_crCaret(RGBA(51,102,204,255))
	{
	}

	SSpellView::~SSpellView(void)
	{
	}

	void SSpellView::OnPaint(IRenderTarget *pRT)
	{
		if(!m_ctx || m_ctx->compMode!=IM_SPELL) return;

		SPainter painter;
		BeforePaint(pRT,painter);


		CRect rcClient = GetClientRect();
		CPoint pt = rcClient.TopLeft();

		POINT pts[2];

		//分３段显示预测结果
		pRT->SetTextColor(m_crResult);
		SStringT strLeft = S_CA2T(SStringA((char*)m_ctx->szWord,m_ctx->byCaret*2));
		SpTextOut(pRT,pt,strLeft);
		
		pRT->SetTextColor(m_crEdit);
		SStringT strEdit = S_CA2T(SStringA((char*)m_ctx->szWord[m_ctx->byCaret],2));
		pts[0]=pt;
		CSize sz = SpTextOut(pRT,pt,strEdit);
		pts[1]=pt;
		pts[0].y+=sz.cy +1;
		pts[1].y+=sz.cy +1;

		pRT->SetTextColor(m_crResult);
		SStringT strRight = S_CA2T(SStringA((char*)(m_ctx->szWord+m_ctx->byCaret+1),
			(m_ctx->bySyllables-m_ctx->byCaret-1)*2));
		SpTextOut(pRT,pt,strRight);

		//显示当前音节的拼音
		if(m_ctx->bySyCaret != 0xFF)
		{
			const SPELLINFO *lpSpi=m_ctx->spellData+m_ctx->byCaret;
			SStringT strLeft = S_CA2T(SStringA(lpSpi->szSpell,m_ctx->bySyCaret));
			CSize sz ;
			pRT->MeasureText(strLeft,strLeft.GetLength(),&sz);
			pts[0].x = pt.x + sz.cx;
			pts[0].y = pt.y;
			pts[1].x = pt.x + sz.cx;
			pts[1].y = pt.y + sz.cy;
		}
		pRT->SetTextColor(m_crSpell);
		SStringT strSpell = S_CA2T(SStringA(m_ctx->spellData[m_ctx->byCaret].szSpell,m_ctx->spellData[m_ctx->byCaret].bySpellLen));
		SpTextOut(pRT,pt,strSpell);

		//draw caret
		CAutoRefPtr<IPen> pen,oldPen;
		pRT->CreatePen(PS_SOLID,m_crCaret,1,&pen);
		pRT->SelectObject(pen,(IRenderObj**)&oldPen);
		pRT->DrawLines(pts,2);
		pRT->SelectObject(oldPen);

		if(m_ctx->bPYBiHua)
		{//显示笔画码
			int i=0;
			SpTextOut(pRT,pt,_T("+"));
			while(m_ctx->szBiHua[i])
			{
				SpTextOut(pRT,pt,g_szLine[m_ctx->szBiHua[i]-'1']);
				i++;
			}
		}

		AfterPaint(pRT,painter);
	}

	CSize SSpellView::SpTextOut(IRenderTarget *pRT,CPoint &pt,const SStringT &str)
	{
		CSize sz;
		pRT->MeasureText(str,str.GetLength(),&sz);
		pRT->TextOut(pt.x,pt.y,str,str.GetLength());
		pt.x += sz.cx;
		return sz;
	}

	CSize SSpellView::GetDesiredSize(int nParentWid, int nParentHei)
	{
		if(!m_ctx || m_ctx->compMode!=IM_SPELL) return CSize();

		CAutoRefPtr<IRenderTarget> pRT;
		GETRENDERFACTORY->CreateRenderTarget(&pRT,0,0);
		BeforePaintEx(pRT);
		CSize sz,szRet;
		SStringT strResult = S_CA2T(SStringA((char*)m_ctx->szWord,m_ctx->bySyllables*2));
		pRT->MeasureText(strResult,strResult.GetLength(),&sz);
		szRet = sz;
		SStringT strSpell = S_CA2T(SStringA(m_ctx->spellData[m_ctx->byCaret].szSpell,m_ctx->spellData[m_ctx->byCaret].bySpellLen));
		pRT->MeasureText(strSpell,strSpell.GetLength(),&sz);
		szRet.cx += sz.cx;
		if(m_ctx->bPYBiHua)
		{//显示笔画码
			int i=0;
			pRT->MeasureText(_T("+"),1,&sz);
			szRet.cx += sz.cx;

			while(m_ctx->szBiHua[i])
			{
				LPCTSTR pszBihua = g_szLine[m_ctx->szBiHua[i]-'1'];
				pRT->MeasureText(pszBihua,-1,&sz);
				szRet.cx += sz.cx;
				i++;
			}
		}

		return szRet;
	}

	void SSpellView::UpdateByContext(const InputContext *pCtx)
	{
		m_ctx = pCtx;
		Invalidate();
	}

}
