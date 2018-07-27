#pragma once

namespace SOUI
{
	class SPhraseCand : public SWindow
	{
		SOUI_CLASS_NAME(SPhraseCand,L"PhraseCand")
	public:
		SPhraseCand(void);
		~SPhraseCand(void);

		void SetCandData(const BYTE* pbyCandData);
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
			ATTR_COLOR(L"colorIndex",m_crIndex,TRUE)
			ATTR_COLOR(L"colorCand",m_crCand,TRUE)
		SOUI_ATTRS_END()

		SStringT m_strIndex;
		SStringT m_strCand;

		COLORREF m_crIndex;
		COLORREF m_crCand;
	};

}
