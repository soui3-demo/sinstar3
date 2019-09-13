#include "stdafx.h"
#include "SCompView.h"

namespace SOUI
{
	SCompCaret::SCompCaret(): m_bDrawCaret(true),  m_iFrame(0), m_byAlpha(0xFF)
		, m_crCaret(RGBA(0, 0, 0, 255)), m_nAniFrames(20), m_nShowFrames(10),m_bAniCaret(FALSE)
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
		int nFrameCount=(m_nShowFrames + m_nAniFrames*2);
		if(m_iFrame%nFrameCount==0)
			m_iFrame=0;

		if (m_bAniCaret)
		{
			if(m_iFrame<m_nShowFrames)
				m_byAlpha = 255;//visible
			else if(m_iFrame<m_nShowFrames+m_nAniFrames)
			{//fadeout
				int iFrame = m_iFrame-m_nShowFrames;
				m_byAlpha = (BYTE)(255 * m_AniInterpolator->getInterpolation(1.0f - iFrame*1.0f / m_nAniFrames));
			}else
			{//fadein
				int iFrame = m_iFrame-m_nShowFrames-m_nAniFrames;
				m_byAlpha = (BYTE)(255 * m_AniInterpolator->getInterpolation( iFrame*1.0f / m_nAniFrames));
			}
			return true;
		}
		else
		{
			if(m_iFrame == 0)
			{
				m_bDrawCaret = true;
				m_byAlpha = 255;
				return true;
			}else if(m_iFrame == m_nShowFrames)
			{
				m_bDrawCaret = false;
				return true;
			}else
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
	SCompView::SCompView():m_caretWidth(1)
	{
	}


	SCompView::~SCompView()
	{
	}

	CSize SCompView::GetDesiredSize(int wid, int hei)
	{
		SStringT strComp = GetWindowText();
		if(strComp.IsEmpty())
			SetWindowText(_T("A"));
		CSize szRet = SWindow::GetDesiredSize(wid, hei);
		if(strComp.IsEmpty())
		{
			SetWindowText(strComp);
			if(GetLayoutParam()->IsWrapContent(Horz))
				szRet.cx = 0;
		}
		if (GetLayoutParam()->IsWrapContent(Horz))
		{
			szRet.cx += m_caretWidth;
		}
		return szRet;
	}

	void SCompView::OnNextFrame()
	{
		if(!::IsWindowVisible(GetContainer()->GetHostHwnd()))
			return;
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
		CRect rcClient;
		GetTextRect(rcClient);
		SStringT strText = GetWindowText();
		CSize szText;
		pRT->MeasureText(strText, strText.GetLength(), &szText);
		CRect rcText = rcClient;
		UINT uAlign = GetStyle().GetTextAlign();
		if(uAlign & DT_VCENTER)
		{
			rcText.DeflateRect(0, (rcClient.Height() - szText.cy) / 2);
		}else if(uAlign & DT_BOTTOM)
		{
			rcText.top+= rcClient.Height() - szText.cy;
		}else
		{//top align
			rcText.bottom = rcText.top + szText.cy;
		}
		rcText.right = rcText.left + szText.cx;
		CRect rcClip;
		pRT->GetClipBox(&rcClip);
		if (!(rcText & rcClip).IsRectEmpty())
		{
			pRT->TextOut(rcText.left, rcText.top, strText, strText.GetLength());
		}
		CRect rcCaret = rcText;
		rcCaret.left = rcText.right;
		rcCaret.right = rcCaret.left + m_caretWidth;
		rcCaret.DeflateRect(0,1);
		if (!(rcCaret & rcClip).IsRectEmpty())
		{
			m_caret.Draw(pRT, rcCaret);
		}
		AfterPaint(pRT, painter);
	}

	void SCompView::OnShowWindow(BOOL bShow, UINT nStatus)
	{
		SWindow::OnShowWindow(bShow, nStatus);
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

	void SCompView::OnDestroy()
	{
		GetContainer()->UnregisterTimelineHandler(this);
		SWindow::OnDestroy();
	}

}
