#include "stdafx.h"
#include "SSentView.h"

namespace SOUI
{
	SSentView::SSentView():m_crSel(RGBA(0,255,0,255)),m_crInput(RGBA(255,0,0,0)),m_nSelCount(0),m_nHeaderCount(0)
	{
	}

	SSentView::~SSentView()
	{
	}

	void SSentView::SetSent(const SStringT & strInput, int nHeaderCount)
	{
		SetWindowText(strInput);
		m_nHeaderCount = nHeaderCount;
		SASSERT(m_nHeaderCount < strInput.GetLength());
		Invalidate();
	}

	void SSentView::SetSelCount(int nLen)
	{
		m_nSelCount = nLen;
		SASSERT(m_nHeaderCount+ m_nSelCount < GetWindowText().GetLength());
		Invalidate();
	}

	void SSentView::OnDrawLine(IRenderTarget *pRT, LPCTSTR pszBuf, int iBegin, int cchText, LPRECT pRect, UINT uFormat)
	{
		if (uFormat & DT_CALCRECT)
		{
			__super::OnDrawLine(pRT, pszBuf, iBegin, cchText, pRect, uFormat);
			return;
		}
		
		CRect rc(pRect);
		CSize szTxt;
		pRT->MeasureText(pszBuf + iBegin, cchText, &szTxt);
		if (uFormat & DT_CENTER)
		{
			rc.DeflateRect((rc.Width() - szTxt.cx)/2, 0);
		}
		COLORREF crDef = pRT->GetTextColor();

		CRect range1(0, 0, m_nHeaderCount, 1);
		CRect range2(m_nHeaderCount, 0, m_nHeaderCount + m_nSelCount, 1);
		CRect range3(m_nHeaderCount + m_nSelCount, 0, GetWindowText().GetLength(), 1);

		CRect rangeDraw(iBegin, 0, iBegin + cchText, 1);

		CRect range;
		range.IntersectRect(rangeDraw, range1);
		if (!range.IsRectEmpty())
		{//draw header
			pRT->SetTextColor(m_crInput);
			pRT->TextOut(rc.left, rc.top, pszBuf + range.left, range.Width());
			pRT->MeasureText(pszBuf + range.left, range.Width(), &szTxt);
			rc.left += szTxt.cx;
		}
		range.IntersectRect(rangeDraw,range2);
		if (!range.IsRectEmpty())
		{//draw header
			pRT->SetTextColor(m_crSel);
			pRT->TextOut(rc.left, rc.top, pszBuf + range.left, range.Width());
			pRT->MeasureText(pszBuf + range.left, range.Width(), &szTxt);
			rc.left += szTxt.cx;
		}

		pRT->SetTextColor(crDef);
		range.IntersectRect(rangeDraw, range3);
		if (!range.IsRectEmpty())
		{//draw remain
			pRT->TextOut(rc.left, rc.top, pszBuf + range.left, range.Width());
			pRT->MeasureText(pszBuf + range.left, range.Width(), &szTxt);
			rc.left += szTxt.cx;
		}
	}

}
