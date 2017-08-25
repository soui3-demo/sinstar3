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

		int GetCandPageSize() const {return m_nCandSize;}
		void ClearCandidate();
		void SetCandidateInfo(const SStringT * strCands, const SStringT * strComps, int nSize);
	protected:
		int OnCreate(LPCREATESTRUCT lpCreateStruct);

		BEGIN_MSG_MAP_EX(CCompWnd)
			MSG_WM_CREATE(OnCreate)
			CHAIN_MSG_MAP(__super)
		END_MSG_MAP()
		
		SWindow * m_pCandContainer;

		SStringT m_strComp;

		CPoint			 m_ptCaret;
		int				 m_nCaretHeight;
		BOOL			 m_bLocated;
		BOOL			 m_bShow;

		int				 m_nCandSize;

	};

}
