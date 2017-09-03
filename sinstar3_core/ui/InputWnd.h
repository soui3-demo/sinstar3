#pragma once

#include "ImeWnd.h"
#include "SkinMananger.h"
#include "../inputContext.h"

namespace SOUI
{
	class CInputWnd :
		public CImeWnd
	{
	public:
		CInputWnd(void);
		~CInputWnd(void);

		void SetCompStr(const SStringT &strComp);
		SStringT GetCompStr() const;

		void MoveTo(CPoint pt,int nCaretHeight);
		void Show(BOOL bShow);

		int GetCandPageSize() const {return m_nPageSize;}
		void SetCandidateInfo(const SArray<SStringT> & strCands, const SArray<SStringT> & strComps, int nSize);

		class InputInfo
		{
		public:
			SStringT strComposition;
			SArray<SStringT> strCands;
			SArray<SStringT> strComps;
			int nCands;
		};

		void OnInputContextChanged(const InputContext * pInputCtx);
	protected:
		void OnInputInfoChanged(BOOL bComp,BOOL bCand);
	protected:
		void OnSetSkin(EventArgs *e);
		void OnBtnPrevPage();
		void OnBtnNextPage();
		EVENT_MAP_BEGIN()
			EVENT_HANDLER(EventSetSkin::EventID,OnSetSkin)
			EVENT_ID_COMMAND(R.id.btn_prevpage,OnBtnPrevPage)
			EVENT_ID_COMMAND(R.id.btn_nextpage,OnBtnNextPage)
		EVENT_MAP_END()

	protected:
		int OnCreate(LPCREATESTRUCT lpCreateStruct);

		BEGIN_MSG_MAP_EX(CInputWnd)
			MSG_WM_CREATE(OnCreate)
			CHAIN_MSG_MAP(__super)
		END_MSG_MAP()
		void ClearCands();

		SWindow * m_pCandContainer;
		SWindow * m_pCompWnd;
		CPoint			 m_ptCaret;
		int				 m_nCaretHeight;
		BOOL			 m_bLocated;
		BOOL			 m_bShow;

		int				 m_nPageSize;
		int				 m_iPage;

		InputInfo		 m_inputInfo;

	};

}
