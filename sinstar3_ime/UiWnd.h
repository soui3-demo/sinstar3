#pragma once

#include "../sinstar3_proxy/simplewnd.h"
#include "../sinstar3_proxy/SinstarProxy.h"

class CUiWnd :
	public CSimpleWnd,
	public ITextService
{
	enum{
		TID_INIT = 100,
	};
public:
	CUiWnd(void);
	~CUiWnd(void);

	static BOOL RegisterClass(HINSTANCE hInstance);
	static void UnregisterClass(HINSTANCE hInstance);

public:
	BOOL IsDefaultIme(void);
	void SetFocus(BOOL bFocus);
public:
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
	LRESULT OnAsyncFocus();
protected:
	virtual void OnReconnReady();

public:
	//ITextService
	virtual BOOL InputStringW(LPCWSTR pszBuf, int nLen) override;
	virtual BOOL IsCompositing() const override;
	virtual void StartComposition(UINT64 imeContext) override;
	virtual void ReplaceSelCompositionW(UINT64 imeContext,int nLeft,int nRight,const WCHAR *wszComp,int nLen) override;
	virtual void UpdateResultAndCompositionStringW(UINT64 imeContext,const WCHAR *wszResultStr,int nResStrLen,const WCHAR *wszCompStr,int nCompStrLen) override;
	virtual void EndComposition(UINT64 imeContext) override;
	virtual void  SetConversionMode(EInputMethod mode) override;
	virtual EInputMethod GetConversionMode() override;
	virtual void SetOpenStatus(BOOL bOpen)  override;
	virtual BOOL GetOpenStatus() const override;
	virtual DWORD GetActiveWnd() const override;

public:
	CSinstarProxy * m_pSinstar3;
	CImeContext *m_pCurContext;

protected:
	void StartComposition(CImeContext * imeContext);
	void ReplaceSelCompositionW(CImeContext * imeContext,int nLeft,int nRight,const WCHAR *wszComp,int nLen);
	void UpdateResultAndCompositionStringW(CImeContext * imeContext,const WCHAR *wszResultStr,int nResStrLen,const WCHAR *wszCompStr,int nCompStrLen);
	void EndComposition(CImeContext * imeContext);
	CImeContext* GetImeContext() const;
	void   ReleaseImeContext(CImeContext * imeContext) const;
	void SetOpenStatus(CImeContext * imeContext,BOOL bOpen);
	BOOL GetOpenStatus(CImeContext * imeContext) const;

	void _SyncFocus();

	BOOL		m_bCompositing;

	int        m_nFontHei;
	HFONT		m_fntComp;
	BOOL		m_bActivate;
	BOOL		m_bHasFocus;
private:
	BOOL _InitSinstar3();
	BOOL _UninitSinstar3();
	BOOL IsIMEMessage(UINT message);
	POINT GetAbsPos(HWND hWnd,DWORD dwStyle,POINT ptCur,RECT rc);
	BOOL AttachToIMC(BOOL bAttach);
};

