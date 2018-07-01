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
		CInputWnd(InputContext * pCtx);
		~CInputWnd(void);

		void MoveTo(CPoint pt,int nCaretHeight);
		void Show(BOOL bShow, BOOL bClearLocateInfo=TRUE);
		void Hide(int nDelay);
		void UpdateUI();
		
		BOOL GoNextCandidatePage();
		BOOL GoPrevCandidatePage();
		short SelectCandidate(short iCand);
	protected:
		int GetCandMax(SWindow *pWnd) const;
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
		void OnTimer(UINT_PTR idEvent);
		BEGIN_MSG_MAP_EX(CInputWnd)
			MSG_WM_TIMER(OnTimer)
			MSG_WM_CREATE(OnCreate)
			CHAIN_MSG_MAP(__super)
		END_MSG_MAP()
		CPoint			 m_ptCaret;
		int				 m_nCaretHeight;
		BOOL			 m_bLocated;
		BOOL			 m_bShow;

		short			 m_cPageSize;
		InputContext	* m_pInputContext;
	};

}
