#pragma once

namespace SOUI
{
	class SToggle2 : public SToggle
	{
		SOUI_CLASS_NAME(SToggle2,L"toggle2")
	public:
		SToggle2(void);
		~SToggle2(void);

	protected:
		virtual CSize GetDesiredSize(int wid, int hei) override;

		void OnPaint(IRenderTarget *pRT);

		SOUI_MSG_MAP_BEGIN()
			MSG_WM_PAINT_EX(OnPaint)
		SOUI_MSG_MAP_END()

	protected:
		SOUI_ATTRS_BEGIN()
			ATTR_SKIN(L"skin1",m_pSkin1,TRUE)
			ATTR_SKIN(L"skin2",m_pSkin2,TRUE)
		SOUI_ATTRS_END()

		ISkinObj * m_pSkin1;
		ISkinObj * m_pSkin2;
	};

}
