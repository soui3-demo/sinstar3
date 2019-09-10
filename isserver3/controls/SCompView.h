#pragma once

namespace SOUI
{
	class SCompCaret
	{
	public:
		SCompCaret();
	public:
		void Draw(IRenderTarget * pRT, const CRect &rc) ;
		bool OnNextFrame();
		CRect GetCaretRect() const;
		void Reset();
	public:
		SOUI_ATTRS_BEGIN()
			ATTR_BOOL(L"animate", m_bAniCaret, FALSE)
			ATTR_COLOR(L"color", m_crCaret, FALSE)
			ATTR_INT(L"speed", m_nFrames, FALSE)
			ATTR_INTERPOLATOR(L"interpolator", m_AniInterpolator, FALSE)
			ATTR_CHAIN_PTR(m_AniInterpolator, 0)
		SOUI_ATTRS_BREAK()
	protected:
		bool	m_bDrawCaret;						/**< is caret drawing */
		int		m_iFrame;
		BYTE	m_byAlpha;

		BOOL	m_bAniCaret;
		COLORREF m_crCaret;
		int		m_nFrames;
		SAutoRefPtr<IInterpolator> m_AniInterpolator;
		CRect	m_rcCaret;
	};

	class SCompView : public SWindow, public ITimelineHandler
	{
		SOUI_CLASS_NAME(SCompView,L"compView")
	protected:
		SCompCaret	m_caret;
	public:
		SCompView();
		~SCompView();

	public:
		SOUI_ATTRS_BEGIN()
			ATTR_CHAIN(m_caret, 0)
		SOUI_ATTRS_END()
	protected:
		virtual CSize GetDesiredSize(int wid, int hei) override;
		virtual void OnNextFrame() override;

	protected:
		void OnPaint(IRenderTarget *pRT);
		void OnShowWindow(BOOL bShow, UINT nStatus);

		SOUI_MSG_MAP_BEGIN()
			MSG_WM_PAINT_EX(OnPaint)
			MSG_WM_SHOWWINDOW(OnShowWindow)
		SOUI_MSG_MAP_END()
	};
}
