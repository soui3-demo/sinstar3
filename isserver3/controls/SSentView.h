#pragma once

namespace SOUI
{
	class SSentView : public SStatic
	{
		SOUI_CLASS_NAME(SSentView,L"sentView")
	public:
		SSentView();
		~SSentView();

		void SetSent(const SStringT & strInput, int nHeaderCount);
		void SetSelCount(int nLen);
		void SetActive(bool bSentInput);
	public:
		SOUI_ATTRS_BEGIN()
			ATTR_COLOR(L"colorInput", m_crInput, TRUE)
			ATTR_COLOR(L"colorSelected", m_crSel, TRUE)
			ATTR_COLOR(L"colorRemain",m_crRemain,TRUE)
			ATTR_COLOR(L"colorInactive",m_crInactive,TRUE)
		SOUI_ATTRS_END()

	protected:
		virtual void OnDrawLine(IRenderTarget *pRT, LPCTSTR pszBuf, int iBegin, int cchText, LPRECT pRect, UINT uFormat);
	protected:
		COLORREF m_crSel;
		COLORREF m_crRemain;
		COLORREF m_crInput;
		COLORREF m_crInactive;
		int		 m_nSelCount;
		int		 m_nHeaderCount;
		bool	 m_bSentInput;
	};
}
