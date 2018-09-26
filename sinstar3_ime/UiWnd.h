#pragma once
#include "simplewnd.h"
#include "CompWndInline.h"

#define  UICLASSNAME _T("sinstar3_uiwnd")

class CUiWnd :
	public CSimpleWnd,
	public ITextService
{
public:
	CUiWnd(void);
	~CUiWnd(void);

	static BOOL RegisterClass(HINSTANCE hInstance);
	static void UnregisterClass(HINSTANCE hInstance);

	//UIWnd
	LRESULT WindowProc(UINT uMsg,WPARAM wParam,LPARAM lParam);
	LRESULT OnSetContext(BOOL bActivate,LPARAM lParam);
	LRESULT OnImeSelect(BOOL bSelect,LPARAM lParam);
	LRESULT OnImeNotify(WPARAM wParam,LPARAM lParam);
	LRESULT OnImeControl(WPARAM wParam,LPARAM lParam);
	LRESULT OnPaint();
	LRESULT OnCreate();
	LRESULT OnDestroy();
	LRESULT OnTimer(WPARAM nEventID);

	//ITextService
	BOOL InputStringW(LPCWSTR pszBuf, int nLen);
	BOOL IsCompositing() const;
	void StartComposition(LPVOID lpImeContext);
	void ReplaceSelCompositionW(LPVOID lpImeContext,int nLeft,int nRight,const WCHAR *wszComp,int nLen);
	void UpdateResultAndCompositionStringW(LPVOID lpImeContext,const WCHAR *wszResultStr,int nResStrLen,const WCHAR *wszCompStr,int nCompStrLen);
	void EndComposition(LPVOID lpImeContext);
	int  MoveCaretPos(LPVOID lpImeContext,int nPos,BOOL bSet);
	LPVOID GetImeContext();
	BOOL   ReleaseImeContext(LPVOID lpImeContext);
	void  SetConversionMode(EInputMethod mode);
	EInputMethod GetConversionMode();
	BOOL SetOpenStatus(LPVOID lpImeContext,BOOL bOpen);
	BOOL GetOpenStatus(LPVOID lpImeContext) const;

	ISinstar* m_pSinstar3;
	CImeContext *m_pCurContext;
	BOOL		m_bCompositing;

	int        m_nFontHei;
	HFONT		m_fntComp;
	BOOL		m_bActivate;
	CCompWndInline m_wndComp;//在notepad中使用的Inline窗口
private:
	BOOL _InitSinstar3();
	BOOL _UninitSinstar3();
	BOOL IsIMEMessage(UINT message);
	POINT GetAbsPos(HWND hWnd,DWORD dwStyle,POINT ptCur,RECT rc);
	BOOL AttachToIMC(BOOL bAttach);
public:
	BOOL IsDefaultIme(void);
};

