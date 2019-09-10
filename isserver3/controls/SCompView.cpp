#include "stdafx.h"
#include "SCompView.h"

namespace SOUI
{
	SCompCaret::SCompCaret(): m_bDrawCaret(true),  m_iFrame(0), m_byAlpha(0xFF)
		, m_crCaret(RGBA(0, 0, 0, 255)), m_nFrames(30), m_bAniCaret(FALSE)
	{
		m_AniInterpolator.Attach(CREATEINTERPOLATOR(SAccelerateInterpolator::GetClassName()));
	}

	void SCompCaret::Reset()
	{
		m_bDrawCaret = true;
		m_iFrame = 0;
		m_byAlpha = 255;
	}

	void SCompCaret::Draw(IRenderTarget * pRT, const CRect & rc)
	{
		if (!m_bDrawCaret)
			return;
		SColor cr(m_crCaret);
		cr.a = cr.a*m_byAlpha / 255;
		pRT->FillSolidRect(&rc, cr.toCOLORREF());
		m_rcCaret = rc;
	}

	bool SCompCaret::OnNextFrame()
	{
		m_iFrame++;
		if (m_bAniCaret)
		{
			if (m_iFrame%m_nFrames == 0)
			{
				m_iFrame = 0;
			}
			m_byAlpha = (BYTE)(255 * m_AniInterpolator->getInterpolation((1.0f - m_iFrame*1.0f / m_nFrames)));
			return true;
		}
		else
		{
			if (m_iFrame%m_nFrames == 0)
			{
				m_bDrawCaret = !m_bDrawCaret;
				m_iFrame = 0;
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	CRect SCompCaret::GetCaretRect() const
	{
		return m_rcCaret;
	}


	///////////////////////////////////////////////////////////////////
	SCompView::SCompView()
	{
	}


	SCompView::~SCompView()
	{
	}

	CSize SCompView::GetDesiredSize(int wid, int hei)
	{
		CSize szRet = SWindow::GetDesiredSize(wid, hei);
		if (GetLayoutParam()->IsWrapContent(Horz))
		{
			szRet.cx += 1;
		}
		return szRet;
	}

	void SCompView::OnNextFrame()
	{
		bool bInvalid = m_caret.OnNextFrame();
		if (bInvalid)
		{
			CRect rcCaret = m_caret.GetCaretRect();
			if (!rcCaret.IsRectEmpty())
			{
				InvalidateRect(rcCaret);
			}
		}
	}

	void SCompView::OnPaint(IRenderTarget * pRT)
	{
		SPainter painter;
		BeforePaint(pRT, painter);
		CRect rcClient = GetClientRect();
		SStringT strText = GetWindowText();
		CSize szText;
		pRT->MeasureText(strText, strText.GetLength(), &szText);
		CRect rcText = rcClient;
		rcText.DeflateRect(0, (rcClient.Height() - szText.cy) / 2);
		rcText.right = rcText.left + szText.cx;
		CRect rcClip;
		pRT->GetClipBox(&rcClip);
		if (!(rcText & rcClip).IsRectEmpty())
		{
			pRT->TextOut(rcText.left, rcText.top, strText, strText.GetLength());
		}
		CRect rcCaret = rcText;
		rcCaret.left = rcText.right;
		rcCaret.right = rcCaret.left + 1;
		if (!(rcCaret & rcClip).IsRectEmpty())
		{
			m_caret.Draw(pRT, rcCaret);
		}
		AfterPaint(pRT, painter);
	}

	void SCompView::OnShowWindow(BOOL bShow, UINT nStatus)
	{
		__super::OnShowWindow(bShow, nStatus);
		if (!IsVisible(TRUE))
		{
			GetContainer()->UnregisterTimelineHandler(this);
		}
		else
		{
			GetContainer()->RegisterTimelineHandler(this);
			m_caret.Reset();
		}
	}

}
