#pragma once

#define  CLS_SINSTAR3_IME_WND _T("sinstar3_uiwnd")	//the class name must like this. otherwise the ime module maybe crash!!!
#define  UM_GETPROCPATH  (WM_USER+5000)
#define  CDT_RET_PROCPATH (2134)
struct PROC_INFO
{
	DWORD dwProcID;
	TCHAR szPath[MAX_PATH];
};

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
