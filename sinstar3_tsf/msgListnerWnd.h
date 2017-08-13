#pragma once

class CMsgListnerWnd
{
public:
	CMsgListnerWnd() : m_hWnd(0)
	{
	}

	virtual ~CMsgListnerWnd()
	{
	}

	
	LRESULT SendMessage(UINT uMsg,WPARAM wParam,LPARAM lParam)
	{
		return ::SendMessage(m_hWnd,uMsg,wParam,lParam);
	}

	LRESULT PostMessage(UINT uMsg,WPARAM wParam,LPARAM lParam)
	{
		return ::PostMessage(m_hWnd,uMsg,wParam,lParam);
	}


	HWND CreateMsgListner(HINSTANCE hInst)
	{
		return ::CreateWindowEx(0,MSGLISTNERWNDCLASS,NULL,WS_POPUP|WS_DISABLED,0,0,0,0,NULL,0,hInst,this);
	}
	
	BOOL DestroyMsgListner()
	{
		return ::DestroyWindow(m_hWnd);
	}
	static BOOL RegisterWnd(HINSTANCE hInstance)
	{
		WNDCLASSEX wc;

		wc.cbSize         = sizeof(WNDCLASSEX);
		wc.style          = 0;
		wc.lpfnWndProc    = _MsgListnerWndProc;
		wc.cbClsExtra     = 0;
		wc.cbWndExtra     = 0;
		wc.hInstance      = (HINSTANCE)hInstance;
		wc.hCursor        = LoadCursor( NULL, IDC_ARROW );
		wc.hIcon          = NULL;
		wc.lpszMenuName   = (LPTSTR)NULL;
		wc.lpszClassName  = MSGLISTNERWNDCLASS;
		wc.hbrBackground  = NULL;
		wc.hIconSm        = NULL;

		return RegisterClassEx( (LPWNDCLASSEX)&wc );
	}
	
	static void UnregisterWnd(HINSTANCE hInstance)
	{
		UnregisterClass(MSGLISTNERWNDCLASS,hInstance);
	}
public:

	HWND m_hWnd;
	
protected:
	virtual LRESULT MsgListnerWndProc(UINT uMsg,WPARAM wParam,LPARAM lParam)
	{
		return ::DefWindowProc(m_hWnd,uMsg,wParam,lParam);
	}
	static LRESULT CALLBACK _MsgListnerWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
	{
		CMsgListnerWnd* pWnd = NULL;
		if(uMsg==WM_CREATE)
		{
			pWnd=(CMsgListnerWnd*)((LPCREATESTRUCT)lParam)->lpCreateParams;
			pWnd->m_hWnd=hWnd;
			SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)pWnd);
		}else
		{
			pWnd=(CMsgListnerWnd*)GetWindowLongPtr(hWnd,GWLP_USERDATA);
		}
		if(0 == pWnd)
		{
			return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
		}

		LRESULT lRet=pWnd->MsgListnerWndProc(uMsg, wParam, lParam);
		if(uMsg==WM_NCDESTROY)
		{
			pWnd->m_hWnd=NULL;
		}
		return lRet;
	}
};






