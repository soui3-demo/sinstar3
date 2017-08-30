#pragma once

#include "ImeWnd.h"
#include "SkinMananger.h"
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

		int GetCandPageSize() const {return m_nCandSize;}
		void SetCandidateInfo(const SStringT * strCands, const SStringT * strComps, int nSize);

		class InputInfo
		{
		public:
			SStringT strComposition;
			SArray<SStringT> strCands;
			SArray<SStringT> strComps;
			int nCands;
		};
	protected:
		void OnInputInfoChanged();
	protected:
		void OnSetSkin(EventArgs *e);
		EVENT_MAP_BEGIN()
			EVENT_HANDLER(EventSetSkin::EventID,OnSetSkin)
		EVENT_MAP_END()

	protected:
		int OnCreate(LPCREATESTRUCT lpCreateStruct);

		BEGIN_MSG_MAP_EX(CInputWnd)
			MSG_WM_CREATE(OnCreate)
			CHAIN_MSG_MAP(__super)
		END_MSG_MAP()
		
		SWindow * m_pCandContainer;
		SWindow * m_pCompWnd;
		CPoint			 m_ptCaret;
		int				 m_nCaretHeight;
		BOOL			 m_bLocated;
		BOOL			 m_bShow;

		int				 m_nCandSize;


		InputInfo		 m_inputInfo;

	};

}
