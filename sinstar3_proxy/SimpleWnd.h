#pragma once

#define  CLS_SINSTAR3_IME_WND	_T("cls_sinstar3_ime_{8C2BE781-F91D-4c1a-9F98-427C7719E100}")
class CSimpleWnd
{
public:
	CSimpleWnd(void);
	virtual ~CSimpleWnd(void);

	HWND Create(HINSTANCE hInst);
	BOOL Destroy();
	LRESULT SendMessage(UINT uMsg,WPARAM wp,LPARAM lp){return ::SendMessage(m_hWnd,uMsg,wp,lp);}
	BOOL PostMessage(UINT uMsg,WPARAM wp,LPARAM lp){return ::PostMessage(m_hWnd,uMsg,wp,lp);}
	BOOL ShowWindow(int nCmdShow) {return ::ShowWindow(m_hWnd,nCmdShow);}

	virtual LRESULT WindowProc(UINT uMsg,WPARAM wParam,LPARAM lParam);
	static BOOL RegisterWndClass(HINSTANCE hInstance);
	static void UnregisterWndClass(HINSTANCE hInstance);

	void Attach(HWND hWnd);
	void Detatch();

	HWND m_hWnd;

private:
	static LRESULT CALLBACK SimpleWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
};
