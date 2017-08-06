#include "stdafx.h"
#include "CompWndInline.h"
#include "CompStrEx.h"

#include "mem_dc.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////

CCompWndInline::CCompWndInline()
{
	////////////////////
	m_pdwClause = 0;
	m_pszString = 0;
	m_pAttributes = 0;

	m_nCountClause = 0; // count of clause
	m_nCountAttributes = 0; // count of attributes.
	m_nStringLength = 0; // count of characters

	m_hWndOwner = 0;

	m_ptStartPosition.x = 0;
	m_ptStartPosition.y = 0;

	m_ptSelectedClause.x = 0;
	m_ptSelectedClause.y = 0;

	/////////////////////////////
	m_hPenLight = ::CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	m_hPenHeavy = ::CreatePen(PS_SOLID, 1, RGB(0, 0, 0));

	m_hbrDashedLine = CreateDashedLineBrush();
	m_hbrCrusor = ::CreateSolidBrush(RGB(0, 0, 0));

	/////////////////////////////
	m_nCaretPos = 0;

	/////////////////////////////
	m_cyDefaultTextHeight = 0;

	/////////////////////////////
	::SetRectEmpty(&m_rcLastLine);

	m_fntComp=0;
	m_bValid=FALSE;
}

CCompWndInline::~CCompWndInline()
{
	::DeleteObject(m_hPenLight);
	::DeleteObject(m_hPenHeavy);

	::DeleteObject(m_hbrDashedLine);
	::DeleteObject(m_hbrCrusor);

	Clear();
}

HBRUSH CCompWndInline::CreateDashedLineBrush()
{
	CMemDC hdcTemp(3, 1, 24);
	::SetPixel(hdcTemp, 0, 0, RGB(0, 0, 0));
	::SetPixel(hdcTemp, 1, 0, RGB(0, 0, 0));
	::SetPixel(hdcTemp, 2, 0, RGB(255, 255, 255));

	HBITMAP hBmpTemp = hdcTemp.Detach();
	HBRUSH hbrRet = ::CreatePatternBrush(hBmpTemp);
	::DeleteObject(hBmpTemp);

	return hbrRet;
}


LRESULT CCompWndInline::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_ERASEBKGND:
		return OnEraseBkgnd(wParam);

	case UM_GETCANDPOS:
		return  OnGetCandPos(wParam,(LPPOINT)lParam);

	case WM_SETFONT:
		return OnSetFont(wParam,lParam);

 	case UM_UPDATECONTEXT:
 		return  OnUpdateContext(wParam,lParam);

	case WM_PAINT:
		return ::DefWindowProc(m_hWnd, WM_PAINT, 0, 0);

	case WM_SETCURSOR:
		return  OnSetCursor(wParam, lParam);
	}
	return __super::WindowProc(uMsg, wParam, lParam);
}

LRESULT CCompWndInline::OnSetFont(WPARAM wParam,LPARAM lParam)
{
	m_fntComp=(HFONT)wParam;
	return 1;
}

void CCompWndInline::GetLastLineRect(RECT* prc) const
{
	*prc = m_rcLastLine;
}

void CCompWndInline::UpdateLayeredWindow()
{
	RECT rc;

	::GetClientRect(m_hWnd, &rc);

	int const nWidth = (int)(rc.right - rc.left);
	int const nHeight = (int)(rc.bottom - rc.top);

	if(nWidth <= 0 || nHeight <= 0)
	{
		return;
	}

	CMemDC hdcMem(nWidth, nHeight);

	RECT rcErase = { 0, 0, nWidth, nHeight };
	COLORREF const clrOldBkColor = ::SetBkColor(hdcMem.m_hDC, RGB(255, 255, 0));
	::ExtTextOut(hdcMem.m_hDC, 0, 0, ETO_OPAQUE, &rcErase, NULL, 0, NULL);
	::SetBkColor(hdcMem.m_hDC, clrOldBkColor);

	HFONT const hOldFont = (HFONT)::SelectObject(hdcMem.m_hDC, m_fntComp);
	RECT rcLastLine = { 0 };
	DrawAllText(hdcMem.m_hDC, m_ptStartPosition.x, nWidth, &rcLastLine);
	::SelectObject(hdcMem.m_hDC, hOldFont);

	////////////////////////////////////////////////
	::ClientToScreen(m_hWnd, (POINT*)&rcLastLine);
	::ClientToScreen(m_hWnd, (POINT*)&rcLastLine + 1);
	if(rcLastLine.right < rcLastLine.left)
	{
		int xTemp = rcLastLine.left;
		rcLastLine.left = rcLastLine.right;
		rcLastLine.right = xTemp;
	}
	m_rcLastLine = rcLastLine;

	UpdateLayeredWindow(hdcMem.m_hDC);
}

void CCompWndInline::UpdateLayeredWindow(HDC hdcSrc)
{
	HDC hdc = ::GetDC(NULL);

	RECT rc;
	::GetWindowRect(m_hWnd, &rc);
	SIZE cxyWindow = { rc.right - rc.left, rc.bottom - rc.top };
	POINT ptSrc = { 0, 0 };

	::UpdateLayeredWindow(m_hWnd, hdc, (POINT*)&rc, &cxyWindow, hdcSrc, &ptSrc, RGB(255, 255, 0), NULL, ULW_COLORKEY);

	::ReleaseDC(NULL, hdc);
}

LRESULT CCompWndInline::OnEraseBkgnd(WPARAM wParam)
{
	return 1;
}

LRESULT CCompWndInline::OnSetCursor(WPARAM wParam, LPARAM lParam)
{
	if(0 != m_hWndOwner)
	{
		CURSORINFO infoCursor;
		infoCursor.cbSize = sizeof(CURSORINFO);
		::GetCursorInfo(&infoCursor);

		LRESULT nHitTestCode = ::SendMessage(m_hWndOwner, WM_NCHITTEST, 0, MAKELPARAM(infoCursor.ptScreenPos.x, infoCursor.ptScreenPos.y));

		return ::SendMessage(m_hWndOwner, WM_SETCURSOR, (WPARAM)m_hWndOwner, MAKELPARAM(nHitTestCode, HIWORD(lParam)));
	}
	else
	{
		return ::DefWindowProc(m_hWnd, WM_SETCURSOR, wParam, lParam);
	}
}

LRESULT CCompWndInline::OnUpdatePos()
{
	UpdatePos();
	UpdateLayeredWindow();
	return 0;
}

LRESULT CCompWndInline::OnUpdateContext(WPARAM wParam,LPARAM lParam)
{
	MoveCompWindow((HIMC)wParam,m_ptStartPosition);
	UpdateLayeredWindow();
	return 1;
}

LRESULT CCompWndInline::OnGetCandPos(WPARAM wParam,LPPOINT pPt)
{
	RECT rc;

	::GetClientRect(m_hWnd, &rc);

	int const nWidth = (int)(rc.right - rc.left);
	int const nHeight = (int)(rc.bottom - rc.top);

	if(nWidth <= 0 || nHeight <= 0)
	{
		pPt->x = 0;
		pPt->y = 0;
		return 0;
	}

	m_ptSelectedClause.x = 0;
	m_ptSelectedClause.y = 0;

	CMemDC hdcMem(1, 1);

	HFONT const hOldFont = (HFONT)::SelectObject(hdcMem.m_hDC, m_fntComp);
	RECT rcLastLine = { 0 };
	BOOL bRet = DrawAllText(hdcMem.m_hDC, m_ptStartPosition.x, nWidth, &rcLastLine, TRUE);
	::SelectObject(hdcMem.m_hDC, hOldFont);

	if(bRet)
	{
		pPt->x = m_ptSelectedClause.x;
		pPt->y = m_ptStartPosition.y + m_ptSelectedClause.y;
	}
	else
	{
		*pPt = m_ptStartPosition;
	}

	return 0;
}

void CCompWndInline::SetClause(PCTSTR pszString,
							   BYTE* pAttributes,
							   const DWORD* pdwClause,
							   int nCountAttributes,
							   int nCountClause,
							   int nCursorPos)
{
	Clear();

	if(0 == pszString || 0 == *pszString)
	{
		return;
	}

	int const nStringLength = ::lstrlen(pszString);
	m_nStringLength = nStringLength;

	//////////////////////
	// allocate memory
	m_pdwClause = (DWORD*)::malloc(sizeof(DWORD) * nCountClause +
		sizeof(TCHAR) * (nStringLength + 1) +
		sizeof(BYTE) * nCountAttributes +
		sizeof(SIZE) * nStringLength);

	///////////////////
	// copy pdwClause
	::memcpy(m_pdwClause, pdwClause, sizeof(DWORD) * nCountClause);
	m_nCountClause = nCountClause;

	///////////////////
	// copy pszString
	m_pszString = (PTSTR)(m_pdwClause + nCountClause);
	::memcpy(m_pszString, pszString, sizeof(TCHAR) * nStringLength);
	m_pszString[nStringLength] = 0;

	///////////////////
	// copy pAttributes
	m_pAttributes = (BYTE*)&m_pszString[nStringLength + 1];
	::memcpy(m_pAttributes, pAttributes, sizeof(BYTE) * nCountAttributes);
	m_nCountAttributes = nCountAttributes;

	///////////////////
	m_pTextSize = (SIZE*)(m_pAttributes + nCountAttributes);

	///////////////////
	m_nCaretPos = nCursorPos;
}

void CCompWndInline::Clear()
{
	DWORD* const pdwClause = m_pdwClause;

	m_pdwClause = 0;
	m_pszString = 0;
	m_pAttributes = 0;

	m_nCountClause = 0;
	m_nCountAttributes = 0;
	m_nStringLength = 0;

	if(0 != pdwClause)
	{
		::free(pdwClause);
	}
}

BOOL CCompWndInline::IsSelected(TCHAR chAttributes)
{
	return ATTR_TARGET_CONVERTED == ((BYTE)chAttributes);
}

void CCompWndInline::UpdatePos()
{
	WINDOWINFO wndInfo;
	wndInfo.cbSize = sizeof(WINDOWINFO);

	::GetWindowInfo(m_hWndOwner, &wndInfo);

	int nWidth = (int)(wndInfo.rcClient.right - wndInfo.rcClient.left);

	CMemDC hdcMem(1, 1);
	HFONT const hOldFont = (HFONT)::SelectObject(hdcMem.m_hDC, m_fntComp);
	int nHeight = CalcWindowHeight(hdcMem.m_hDC, m_ptStartPosition.x, nWidth);
	::SelectObject(hdcMem.m_hDC, hOldFont);

	::SetWindowPos(m_hWnd, 0,
		wndInfo.rcClient.left, m_ptStartPosition.y + wndInfo.rcClient.top, nWidth, nHeight, SWP_NOACTIVATE|SWP_NOZORDER);
}

int CCompWndInline::GetNormalTextHeight(HDC hdc)
{
	return (m_cyDefaultTextHeight + 2);
}

void CCompWndInline::InitDefaultTextHeight(HDC hdc)
{
	SIZE cxyNormal;
	::GetTextExtentPoint32(hdc, _T("A"), 1, &cxyNormal);

	m_cyDefaultTextHeight = (int)cxyNormal.cy;
}

void CCompWndInline::SetStartPosition(int x, int y)
{
	m_ptStartPosition.x = x;
	m_ptStartPosition.y = y;
}


BOOL CCompWndInline::LocateSelectedClause(int iStartText, int nCount, RECT* prcText)
{
	PCTSTR pszText = m_pszString + iStartText;

	int const iEndText = (iStartText + nCount);

	int xClauseLeft = (int)prcText->left;

	for(int i = iStartText ; i < iEndText ; )
	{
		int iClauseEnd = iEndText;
		for(int j = 0 ; j < m_nCountClause ; j++)
		{
			if(i < (int)m_pdwClause[j])
			{
				if(iEndText > (int)m_pdwClause[j])
				{
					iClauseEnd = (int)m_pdwClause[j];
				}
				break;
			}
		}

		if(ATTR_TARGET_CONVERTED == m_pAttributes[i])
		{
			m_ptSelectedClause.x = xClauseLeft;
			m_ptSelectedClause.y = prcText->top;
			return TRUE;
		}

		int xClauseRight = xClauseLeft;
		for( ; i < iClauseEnd ; i++)
		{
			SIZE* pTextSize = &m_pTextSize[i];
			if(pTextSize->cx > 0)
			{
				xClauseRight += pTextSize->cx;
			}
		}

		xClauseLeft = xClauseRight;
	}

	return FALSE;
}

void CCompWndInline::DrawTextLine(HDC hdc, int iStartText, int nCount, RECT* prcText)
{
	PCTSTR pszText = m_pszString + iStartText;

	::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, prcText, NULL, 0, NULL);

	::DrawText(hdc, pszText, nCount, prcText, DT_LEFT | DT_SINGLELINE | DT_TOP | DT_NOPREFIX);

	int const iEndText = (iStartText + nCount);

	int xClauseLeft = (int)prcText->left;

	for(int i = iStartText ; i < iEndText ; )
	{
		int iClauseEnd = iEndText;
		for(int j = 0 ; j < m_nCountClause ; j++)
		{
			if(i < (int)m_pdwClause[j])
			{
				if(iEndText > (int)m_pdwClause[j])
				{
					iClauseEnd = (int)m_pdwClause[j];
				}
				break;
			}
		}

		int const nAttribute = m_pAttributes[i];

		int xClauseRight = xClauseLeft;
		for( ; i < iClauseEnd ; i++)
		{
			SIZE* pTextSize = &m_pTextSize[i];
			if(pTextSize->cx > 0)
			{
				xClauseRight += pTextSize->cx;
			}
		}

		if(ATTR_TARGET_CONVERTED == nAttribute)
		{
			HPEN const hOldPen = (HPEN)::SelectObject(hdc, m_hPenHeavy);

			::MoveToEx(hdc, xClauseLeft + 1, (int)prcText->bottom - 1, NULL);
			::LineTo(hdc, xClauseRight - 1, (int)prcText->bottom - 1);
			::MoveToEx(hdc, xClauseLeft + 1, (int)prcText->bottom - 2, NULL);
			::LineTo(hdc, xClauseRight - 1, (int)prcText->bottom - 2);

			::SelectObject(hdc, hOldPen);
		}
		else if(ATTR_CONVERTED == nAttribute)
		{
			HPEN const hOldPen = (HPEN)::SelectObject(hdc, m_hPenLight);

			::MoveToEx(hdc, xClauseLeft + 1, (int)prcText->bottom - 1, NULL);
			::LineTo(hdc, xClauseRight - 1, (int)prcText->bottom - 1);

			::SelectObject(hdc, hOldPen);
		}
		else
		{
			RECT rcDashedLine = { xClauseLeft + 1, prcText->bottom - 1, xClauseRight - 1, prcText->bottom };
			::FillRect(hdc, &rcDashedLine, m_hbrDashedLine);
		}

		xClauseLeft = xClauseRight;
	}

	///////////////////////////
	// draw caret
	if(m_nCaretPos >= iStartText && m_nCaretPos <= iEndText)
	{
		int xCursor = prcText->left;
		for(int i = iStartText ; i <= iEndText ; i++)
		{
			if(i == m_nCaretPos)
			{
				RECT rcCaret = { xCursor, prcText->top + 2, xCursor + 2, prcText->bottom - 2 };

				::FillRect(hdc, &rcCaret, m_hbrCrusor);

				break;
			}

			if(i == iEndText)
			{
				break;
			}

			SIZE* pTextSize = &m_pTextSize[i];
			if(pTextSize->cx > 0)
			{
				xCursor += pTextSize->cx;
			}
		}
	}
}

BOOL CCompWndInline::DrawAllText(HDC hdc, int x, int nWidth, RECT* prcLastLine, BOOL bLocateClause)
{
	::SetRectEmpty(prcLastLine);

	if(0 == m_nStringLength)
	{
		return FALSE;
	}

	InitDefaultTextHeight(hdc);

	PTSTR pszDrawText = m_pszString;
	int xDrawText = x;

	int nMaxLineHeight = 0;
	int nTotalHeight = 0;
	BOOL bFirstCharacter = TRUE;

	/////////////////////////
	// information of last line
	int nCharacterCountOfLastLine = 0;
	PTSTR pszLastLine = NULL;
	RECT rcLastLine = { 0 };

	for(PTSTR pszTemp = m_pszString ; ; )
	{
		SIZE cxyCharacter;
		PTSTR pszNext = CalcCharacterExtentPoint(hdc, pszTemp, &cxyCharacter);

		if(pszNext == 1 + pszTemp)
		{
			m_pTextSize[(int)(pszTemp - m_pszString)] = cxyCharacter;
		}
		else
		{
			SIZE* pTextSize = &m_pTextSize[(int)(pszTemp - m_pszString)];
			pTextSize->cx = -1;
			pTextSize->cy = -1;
			pTextSize[1] = cxyCharacter;
		}

		if(x + cxyCharacter.cx > nWidth)
		{
			if(0 != x || !bFirstCharacter)
			{
				RECT rcText = { xDrawText, nTotalHeight, nWidth, 0 };

				nTotalHeight += (0 == nMaxLineHeight) ? GetNormalTextHeight(hdc) : nMaxLineHeight;
				nMaxLineHeight = 0;
				x = 0;

				xDrawText = 0;

				rcText.bottom = nTotalHeight;

				if((SIZE_T)pszTemp > (SIZE_T)pszDrawText)
				{
					rcLastLine = rcText;
					pszLastLine = pszDrawText;
					nCharacterCountOfLastLine = (int)(pszNext - pszDrawText);

					if(bLocateClause)
					{
						if(LocateSelectedClause((int)(pszDrawText - m_pszString), (int)(pszTemp - pszDrawText), &rcText))
						{
							return TRUE;
						}
					}
					else
					{
						DrawTextLine(hdc, (int)(pszDrawText - m_pszString), (int)(pszTemp - pszDrawText), &rcText);
					}
					
					pszDrawText = pszTemp;
				}
			}
		}

		x += cxyCharacter.cx;
		if(nMaxLineHeight < cxyCharacter.cy)
		{
			nMaxLineHeight = cxyCharacter.cy;
		}

		bFirstCharacter = FALSE;
		if(0 == *pszNext)
		{
			RECT rcText = { xDrawText, nTotalHeight, x, 0 };

			nTotalHeight += nMaxLineHeight;

			rcText.bottom = nTotalHeight;

			if((SIZE_T)pszNext > (SIZE_T)pszDrawText)
			{
				rcLastLine = rcText;
				pszLastLine = pszDrawText;
				nCharacterCountOfLastLine = (int)(pszNext - pszDrawText);

				if(bLocateClause)
				{
					if(LocateSelectedClause((int)(pszDrawText - m_pszString), (int)(pszNext - pszDrawText), &rcText))
					{
						return TRUE;
					}
				}
				else
				{
					DrawTextLine(hdc, (int)(pszDrawText - m_pszString), (int)(pszNext - pszDrawText), &rcText);
				}
			}

			if(bLocateClause)
			{
				m_ptSelectedClause.x = rcText.right;
				m_ptSelectedClause.y = rcText.top;

				return TRUE;
			}

			break;
		}
		pszTemp = pszNext;
	}

	if(0 != pszLastLine)
	{
		rcLastLine.right = CalcLastLineRect(hdc, pszLastLine, nCharacterCountOfLastLine, &rcLastLine);
		*prcLastLine = rcLastLine;
	}

	return FALSE;
}

int CCompWndInline::CalcLastLineRect(HDC hdc, PTSTR pszText, int nCount, RECT* prcLastLine)
{
	RECT rcCalc = { 0 };
	::DrawText(hdc, pszText, nCount, &rcCalc, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_CALCRECT | DT_NOPREFIX);
	return prcLastLine->left + (rcCalc.right - rcCalc.left);
}

PTSTR CCompWndInline::CalcCharacterExtentPoint(HDC hdc, PTSTR pszCharacter, SIZE* pSize)
{
	PTSTR pszNext = ::CharNext(pszCharacter);
	::GetTextExtentPoint32(hdc, pszCharacter, (int)(pszNext - pszCharacter), pSize);
	pSize->cy = (m_cyDefaultTextHeight + 2);
	return pszNext;
}

int CCompWndInline::CalcWindowHeight(HDC hdc, int x, int nWidth)
{
	if(0 == m_nStringLength)
	{
		return 0;
	}

	InitDefaultTextHeight(hdc);

	int nMaxLineHeight = 0;
	int nTotalHeight = 0;
	BOOL bFirstCharacter = TRUE;

	for(PTSTR pszTemp = m_pszString ; ; )
	{
		SIZE cxyCharacter;
		PTSTR pszNext = CalcCharacterExtentPoint(hdc, pszTemp, &cxyCharacter);

		if(x + cxyCharacter.cx > nWidth)
		{
			if(0 != x || !bFirstCharacter)
			{
				nTotalHeight += (0 == nMaxLineHeight) ? GetNormalTextHeight(hdc) : nMaxLineHeight;
				nMaxLineHeight = 0;
				x = 0;
			}
		}

		x += cxyCharacter.cx;
		if(nMaxLineHeight < cxyCharacter.cy)
		{
			nMaxLineHeight = cxyCharacter.cy;
		}

		bFirstCharacter = FALSE;
		if(0 == *pszNext)
		{
			nTotalHeight += nMaxLineHeight;
			break;
		}
		pszTemp = pszNext;
	}

	return nTotalHeight;
}

void CCompWndInline::MoveCompWindow(HIMC hIMC,POINT pt)
{
	LPINPUTCONTEXT lpIMC=(LPINPUTCONTEXT)ImmLockIMC(hIMC);
	if(!lpIMC)
	{
		return;
	}
	CCompStrEx *pCompStr=(CCompStrEx *)ImmLockIMCC(lpIMC->hCompStr);
	if(!pCompStr)
	{
		ImmUnlockIMC(hIMC);
		return;
	}

	CCompWndInline::SetClause(pCompStr->szCompStr,
		pCompStr->byCompAttr,
		pCompStr->dwCompCls,
		(int)pCompStr->dwCompAttrLen,
		(int)pCompStr->dwCompClauseLen / sizeof(DWORD),
		(int)pCompStr->dwCursorPos);

	CCompWndInline::SetStartPosition(pt.x, pt.y);
	m_hWndOwner=lpIMC->hWnd;
	CCompWndInline::UpdatePos();
	CCompWndInline::UpdateLayeredWindow();
	ImmUnlockIMCC(lpIMC->hCompStr);
	ImmUnlockIMC(hIMC);
}