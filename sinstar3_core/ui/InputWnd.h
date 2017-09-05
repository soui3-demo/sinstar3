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
		CInputWnd(const InputContext * pCtx);
		~CInputWnd(void);

		void SetCompStr(const SStringT &strComp);
		SStringT GetCompStr() const;

		void MoveTo(CPoint pt,int nCaretHeight);
		void Show(BOOL bShow);

		void UpdateUI();

		short SelectCandidate(UINT vKey);
		BOOL  TurnCandPage(UINT vKey);
	protected:
		int GetCandMax(SWindow *pWnd) const;
	protected:
		void OnSetSkin(EventArgs *e);
		BOOL OnBtnPrevPage();
		BOOL OnBtnNextPage();
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
		CPoint			 m_ptCaret;
		int				 m_nCaretHeight;
		BOOL			 m_bLocated;
		BOOL			 m_bShow;

		short			 m_iCandFirst;
		short			 m_iCandEnd;
		short			 m_cPageSize;
		const InputContext	* m_pInputContext;
	};

}
