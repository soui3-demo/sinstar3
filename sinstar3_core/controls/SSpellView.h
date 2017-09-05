#pragma once

namespace SOUI
{
	class SSpellView: public SWindow
	{
		SOUI_CLASS_NAME(SSpellView,L"spellView")
	public:
		SSpellView(void);
		~SSpellView(void);

		void UpdateByContext(const InputContext *pCtx);
	protected:
		virtual CSize GetDesiredSize(int nParentWid, int nParentHei);
	protected:
		void OnPaint(IRenderTarget *pRT);

		SOUI_MSG_MAP_BEGIN()
			MSG_WM_PAINT_EX(OnPaint)
		SOUI_MSG_MAP_END()

		SOUI_ATTRS_BEGIN()
			ATTR_COLOR(L"spell",m_crSpell,TRUE)
			ATTR_COLOR(L"result",m_crResult,TRUE)
			ATTR_COLOR(L"edit",m_crEdit,TRUE)
			ATTR_COLOR(L"caret",m_crCaret,TRUE)
		SOUI_ATTRS_END()

	protected:
		CSize SpTextOut(IRenderTarget *pRT,CPoint &pt,const SStringT &str);
		COLORREF m_crSpell;
		COLORREF m_crResult;
		COLORREF m_crEdit;
		COLORREF m_crCaret;

		const InputContext * m_ctx;
	};
}
