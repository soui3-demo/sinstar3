#include "stdafx.h"
#include "SLineComp.h"

namespace SOUI
{
	SLineComp::SLineComp()
	{
	}


	SLineComp::~SLineComp()
	{
	}

	CSize SLineComp::GetDesiredSize(int nParentWid, int nParentHei)
	{
		if (m_lines)
		{
			CSize szWord = m_lines->GetSkinSize();
			szWord.cx *= GetWindowText().GetLength();
			return szWord;
		}
		else
		{
			return __super::GetDesiredSize(nParentWid, nParentHei);
		}
	}

	void SLineComp::OnPaint(IRenderTarget *pRT)
	{
		if (m_lines)
		{
			SStringT strTxt = GetWindowText();
			CRect rcWnd = GetClientRect();
			CSize szSkin = m_lines->GetSkinSize();
			CPoint pt = rcWnd.TopLeft();
			pt.y += (rcWnd.Height() - szSkin.cy) / 2;
			CRect rcWord(pt, szSkin);
			for (int i = 0; i < strTxt.GetLength(); i++)
			{
				m_lines->Draw(pRT, rcWord, strTxt[i] - '1');
				rcWord.OffsetRect(rcWord.Width(), 0);
			}
		}
		else
		{
			__super::OnPaint(pRT);
		}
	}
}
