#pragma once
#include "simplewnd.h"
#include "HotKeyMgr.h"
#include "CompWndInline.h"

#define  UICLASSNAME _T("sinstar3_uiwnd")

class CUiWnd :
	public CSimpleWnd,
	public CHotKeyMgr,
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
	BOOL IsCompositing();
	void StartComposition(LPVOID lpImeContext);
	void ReplaceSelCompositionW(LPVOID lpImeContext,int nLeft,int nRight,WCHAR *wszComp,int nLen);
	void UpdateResultAndCompositionStringW(LPVOID lpImeContext,WCHAR *wszResultStr,int nResStrLen,WCHAR *wszCompStr,int nCompStrLen);
	void EndComposition(LPVOID lpImeContext);
	int  MoveCaretPos(LPVOID lpImeContext,int nPos,BOOL bSet);
	LPVOID GetImeContext();
	BOOL   ReleaseImeContext(LPVOID lpImeContext);
	void  SetConversionMode(EInputMethod mode);
	EInputMethod GetConversionMode();
	BOOL  RegisterIMEHotKey(REFGUID guidHotKey,LPCWSTR pszName,const PRESERVEDKEY *pKey);
	BOOL  UnregisterIMEHotKey(REFGUID guidHotKey,const PRESERVEDKEY *pKey);
	BOOL SetOpenStatus(LPVOID lpImeContext,BOOL bOpen);
	BOOL GetOpenStatus(LPVOID lpImeContext);
#ifdef ENABLE_LANGUAGEBAR
	BOOL GetLanguageBarItemMgr(ITfLangBarItemMgr **ppLangBarMgr,GUID *pGuidTIP);
#endif//ENABLE_LANGUAGEBAR
	LRESULT DoWildRequest(WPARAM wParam,LPARAM lParam){return E_NOTIMPL;}

	ISinstar* m_pSinstar3;
	CImeContext *m_pCurContext;
	int        m_nFontHei;
	HFONT		m_fntComp;
	BOOL		m_bActivate;
	CCompWndInline m_wndComp;//在notepad中使用的Inline窗口
private:
	BOOL _InitBaiduJP3();
	BOOL _UninitBaiduJP3();
	BOOL IsIMEMessage(UINT message);
	POINT GetAbsPos(HWND hWnd,DWORD dwStyle,POINT ptCur,RECT rc);
	BOOL AttachToIMC(BOOL bAttach);
public:
	BOOL IsDefaultIme(void);
};

