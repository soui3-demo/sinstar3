#pragma once
class SLineSkin : public SSkinObjBase
{
	SOUI_CLASS_NAME(SSkinColorRect, L"lineskin")
public:
	SLineSkin();
~SLineSkin(); 
protected:
	virtual void _DrawByIndex(IRenderTarget *pRT, LPCRECT rcDraw, int iState, BYTE byAlpha)  const;

	virtual BOOL IgnoreState() { return GetStates() < 2; }

	virtual int GetStates();
	virtual ISkinObj * Scale(int nScale);
	SOUI_ATTRS_BEGIN()
		ATTR_COLOR(L"left_normal", m_crStates[0], FALSE)
		ATTR_COLOR(L"left_hover", m_crStates[1], FALSE)
		ATTR_COLOR(L"left_pushdown", m_crStates[2], FALSE)
		ATTR_COLOR(L"left_disable", m_crStates[3], FALSE)		
		ATTR_COLOR(L"top_normal", m_crStates[4], FALSE)
		ATTR_COLOR(L"top_hover", m_crStates[5], FALSE)
		ATTR_COLOR(L"top_pushdown", m_crStates[6], FALSE)
		ATTR_COLOR(L"top_disable", m_crStates[7], FALSE)
		ATTR_COLOR(L"right_normal", m_crStates[8], FALSE)
		ATTR_COLOR(L"right_hover", m_crStates[9], FALSE)
		ATTR_COLOR(L"right_pushdown", m_crStates[10], FALSE)
		ATTR_COLOR(L"right_disable", m_crStates[11], FALSE)
		ATTR_COLOR(L"bottom_normal", m_crStates[12], FALSE)
		ATTR_COLOR(L"bottom_hover", m_crStates[13], FALSE)
		ATTR_COLOR(L"bottom_pushdown", m_crStates[14], FALSE)
		ATTR_COLOR(L"bottom_disable", m_crStates[15], FALSE)
		ATTR_LAYOUTSIZE4(L"line_wid",m_szLine,FALSE)
	SOUI_ATTRS_END()
	COLORREF m_crStates[16];
	SLayoutSize m_szLine[4];
};