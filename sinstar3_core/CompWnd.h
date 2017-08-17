#pragma once

#include "ImeWnd.h"

namespace SOUI
{
	class CCompWnd :
		public CImeWnd
	{
	public:
		CCompWnd(void);
		~CCompWnd(void);

		void SetCompStr(const SStringT &strComp);
		SStringT GetCompStr() const;

		void MoveTo(CPoint pt,int nCaretHeight);
		void Show(BOOL bShow);
	protected:
		SStringT m_strComp;

		CPoint			 m_ptCaret;
		int				 m_nCaretHeight;
		BOOL			 m_bLocated;
		BOOL			 m_bShow;

	};

}
