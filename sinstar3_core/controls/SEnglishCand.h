#pragma once

namespace SOUI
{
	class SEnglishCand : public SWindow
	{
		SOUI_CLASS_NAME(SEnglishCand,L"EnCand")
	public:
		SEnglishCand(void);
		~SEnglishCand(void);

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
			ATTR_STRINGT(L"phontic", m_strPhontic,TRUE)
			ATTR_COLOR(L"colorIndex",m_crIndex,TRUE)
			ATTR_COLOR(L"colorCand",m_crCand,TRUE)
			ATTR_COLOR(L"colorPhontic", m_crPhontic,TRUE)
		SOUI_ATTRS_END()

		SStringT m_strIndex;
		SStringT m_strCand;
		SStringT m_strPhontic;

		COLORREF m_crIndex;
		COLORREF m_crCand;
		COLORREF m_crPhontic;
	};

}
