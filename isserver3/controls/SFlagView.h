#pragma once

namespace SOUI
{
	class SFlagView : public SWindow
	{
		SOUI_CLASS_NAME(SFlagView,L"FlagView")
	public:
		SFlagView();
		~SFlagView();

		void SetImeFlagData(LPBYTE pData,DWORD dwLen);
		void ShowSpellFlag(BOOL bSpell);

		void UpdateCapitalMode(BOOL bCap);

		SOUI_ATTRS_BEGIN()
			ATTR_SKIN(L"defaultFlag",m_defFlag,TRUE)
			ATTR_SKIN(L"capFlag",m_capFlag,TRUE)
			ATTR_COLOR(L"colorTheme",m_crTheme,TRUE)
		SOUI_ATTRS_END()

	protected:
		void OnPaint(IRenderTarget *pRT);
		SOUI_MSG_MAP_BEGIN()
			MSG_WM_PAINT_EX(OnPaint)
		SOUI_MSG_MAP_END()

	private:
		SAutoRefPtr<ISkinObj> m_defFlag;
		SAutoRefPtr<ISkinObj> m_capFlag;
		SAutoRefPtr<IBitmap>  m_imeFlag;
		BOOL				  m_bSpellFlag;
		COLORREF			  m_crTheme;
		BOOL				  m_bCapMode;
	};
}
