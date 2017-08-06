#pragma once
#include "simplewnd.h"

#define UM_GETCANDPOS	(WM_USER+900)	//获得当前编辑文节的左上角坐标,WPARAM:hWnd-编辑窗口句柄,LPARAM:&pt,位置为相对于编辑窗口的左上角坐标
#define UM_UPDATECONTEXT (WM_USER+901)	//更新数据

class CCompWndInline : public CSimpleWnd
{
public:

	explicit CCompWndInline();
	~CCompWndInline();

	void UpdateLayeredWindow();

	void SetClause(PCTSTR pszString,
		BYTE* pAttributes,
		const DWORD* pdwClause,
		int nCountAttributes,
		int nCountClause,
		int nCursorPos);

	void Clear();

	void SetStartPosition(int x, int y);

	void UpdatePos();

	void GetLastLineRect(RECT* prc) const;

private:

	void UpdateLayeredWindow(HDC hdcSrc);

	// nWidth is the right border of window
	int CalcWindowHeight(HDC hdc, int x, int nWidth);

protected:

	LRESULT OnEraseBkgnd(WPARAM wParam);
	LRESULT OnSetCursor(WPARAM wParam, LPARAM lParam);
	LRESULT OnUpdatePos();
	LRESULT OnGetCandPos(WPARAM wParam,LPPOINT pPt);
	LRESULT OnUpdateContext(WPARAM wParam,LPARAM lParam);
	LRESULT OnSetFont(WPARAM wParam,LPARAM lParam);

private:

	// nWidth is the right border of window
	BOOL DrawAllText(HDC hdc, int x, int nWidth, RECT* prcLastLine, BOOL bLocateClause = FALSE);

	PTSTR CalcCharacterExtentPoint(HDC hdc, PTSTR pszCharacter, SIZE* pSize);

	int GetNormalTextHeight(HDC hdc);

	static BOOL IsSelected(TCHAR chAttributes);

	BOOL LocateSelectedClause(int iStartText, int nCount, RECT* prcText);

	void DrawTextLine(HDC hdc, int iStartText, int nCount, RECT* prcText);

	void InitDefaultTextHeight(HDC hdc);

private:

	static HBRUSH CreateDashedLineBrush();

	static int CalcLastLineRect(HDC hdc, PTSTR pszText, int nCount, RECT* prcLastLine);

public:

	virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void MoveCompWindow(HIMC hIMC,POINT pt);
	BOOL	m_bValid;
private:

	HPEN m_hPenLight;
	HPEN m_hPenHeavy;

	HBRUSH m_hbrDashedLine;
	HBRUSH m_hbrCrusor;

	DWORD* m_pdwClause;
	PTSTR m_pszString;
	BYTE* m_pAttributes;
	SIZE* m_pTextSize;
	int m_nCaretPos;

	int m_nCountClause; // count of clause
	int m_nCountAttributes; // count of attributes.
	int m_nStringLength; // count of characters

	int m_cyDefaultTextHeight; // height of "A"

	HWND m_hWndOwner;

	POINT m_ptStartPosition;

	POINT m_ptSelectedClause;

	RECT m_rcLastLine;

	HFONT m_fntComp;

};

