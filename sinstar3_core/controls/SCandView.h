#pragma once

namespace SOUI
{
	class SCandView : public SWindow
	{
		SOUI_CLASS_NAME(SCandView,L"candidate")
	public:
		SCandView(void);
		~SCandView(void);

		void SetData(const SStringT & strText, const SStringT &strComp);
	protected:
		virtual CSize GetDesiredSize(int nParentWid, int nParentHei);

		void OnPaint(IRenderTarget *pRT);

		SOUI_MSG_MAP_BEGIN()
			MSG_WM_PAINT_EX(OnPaint)
		SOUI_MSG_MAP_END()
	protected:
		SOUI_ATTRS_BEGIN()
			ATTR_STRINGT(L"index",m_strIndex,TRUE)
			ATTR_STRINGT(L"cand",m_strCand,TRUE)
			ATTR_STRINGT(L"comp",m_strComp,TRUE)
			ATTR_COLOR(L"colorIndex",m_crIndex,TRUE)
			ATTR_COLOR(L"colorCand",m_crCand,TRUE)
			ATTR_COLOR(L"colorComp",m_crComp,TRUE)
			ATTR_COLOR(L"colorWild",m_crWild,TRUE)
		SOUI_ATTRS_END()

		SStringT m_strIndex;
		SStringT m_strCand;
		SStringT m_strComp;

		COLORREF m_crIndex;
		COLORREF m_crCand;
		COLORREF m_crComp;
		COLORREF m_crWild;
	};

}
