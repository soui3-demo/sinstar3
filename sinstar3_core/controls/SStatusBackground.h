#pragma once

namespace SOUI
{
	class SStatusBackground : public SWindow
	{
		SOUI_CLASS_NAME(SStatusBackground,L"status_bg")
	public:
		enum MODE{
			MODE_EXTEND=0,
			MODE_SHRINK,
		};

		SStatusBackground(void);
		~SStatusBackground(void);

	protected:
		virtual CSize GetDesiredSize(int nParentWid, int nParentHei);

	protected:
		void OnPaint(IRenderTarget *pRT);

		SOUI_MSG_MAP_BEGIN()
			MSG_WM_PAINT_EX(OnPaint)
		SOUI_MSG_MAP_END()

	protected:
		SOUI_ATTRS_BEGIN()
			ATTR_SKIN(L"bg_extend",m_pSkinExtend,TRUE)
			ATTR_SKIN(L"bg_shrink",m_pSkinShrink,TRUE)
			ATTR_ENUM_BEGIN(L"mode",MODE,TRUE)
				ATTR_ENUM_VALUE(L"extend",MODE_EXTEND)
				ATTR_ENUM_VALUE(L"shrink",MODE_SHRINK)
			ATTR_ENUM_END(m_mode)
		SOUI_ATTRS_END()
		void SetMode(SStatusBackground::MODE mode);
		ISkinObj * m_pSkinExtend;
		ISkinObj * m_pSkinShrink;

		MODE 	   m_mode;
	};

}
