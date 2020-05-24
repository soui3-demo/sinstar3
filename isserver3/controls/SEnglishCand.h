#pragma once

namespace SOUI
{
#define UM_FLMINFO	(WM_APP+10000)
	class SEnglishCand : public SWindow
	{
		SOUI_CLASS_NAME(SEnglishCand,L"EnCand")
	public:
		SEnglishCand(void);
		~SEnglishCand(void);

		void SetCandData(const BYTE* pbyCandData);
	protected:
		virtual CSize GetDesiredSize(int nParentWid, int nParentHei);
		virtual BOOL OnUpdateToolTip(CPoint pt, SwndToolTipInfo &tipInfo);

		void OnPaint(IRenderTarget *pRT);
		LRESULT OnFlmInfo(UINT uMsg, WPARAM, LPARAM lp);
		void OnLButtonUp(UINT nFlags,CPoint pt);

		SOUI_MSG_MAP_BEGIN()
			MESSAGE_HANDLER_EX(UM_FLMINFO,OnFlmInfo)
			MSG_WM_PAINT_EX(OnPaint)
			MSG_WM_LBUTTONUP(OnLButtonUp)
		SOUI_MSG_MAP_END()
	protected:
		SOUI_ATTRS_BEGIN()
			ATTR_STRINGT(L"index",m_strIndex,TRUE)
			ATTR_STRINGT(L"cand",m_strCand,TRUE)
			ATTR_STRINGT(L"phonetic", m_strPhonetic,TRUE)
			ATTR_COLOR(L"colorIndex",m_crIndex,TRUE)
			ATTR_COLOR(L"colorCand",m_crCand,TRUE)
			ATTR_COLOR(L"colorPhonetic", m_crPhonetic,TRUE)
			ATTR_BOOL(L"showPhonetic",m_bShowPhonetic,TRUE)
		SOUI_ATTRS_END()
		SStringT m_strIndex;
		SStringT m_strCand;
		SStringT m_strPhonetic;

		COLORREF m_crIndex;
		COLORREF m_crCand;
		COLORREF m_crPhonetic;
		BOOL	 m_bShowPhonetic;

		SAutoRefPtr<IFont> m_ftPhonetic;
	};

}
