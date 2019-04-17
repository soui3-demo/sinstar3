// FindCaller.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "resource.h"
#include "FindCaller.h"
#include "dllcaller.h"
#include "fc_const.h"

// 全局变量: 
HINSTANCE hInst;                                // 当前实例

// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

typedef BOOL (WINAPI *FunWow64DisableWow64FsRedirection)(
	PVOID *OldValue
	);

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpstrCmdLine*/, int nCmdShow)
{
    // 初始化全局字符串
    MyRegisterClass(hInstance);

    // 执行应用程序初始化: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    MSG msg;
    // 主消息循环: 
    while (GetMessage(&msg, NULL, 0, 0))
    {
         DispatchMessage(&msg);
    }

    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = 0;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInst, MAKEINTRESOURCE(IDI_FINDCALLER));
	wcex.hIconSm = 0;
	wcex.hCursor        = 0;
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = KFindCallerClass;

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(KFindCallerClass, KTitle, 0,
      0, 0, 0, 0, HWND_MESSAGE, NULL, hInstance, NULL);
   return hWnd != 0;
}

LRESULT OnCopyData(HWND hWnd, WPARAM wp, LPARAM lp)
{
	LRESULT lRet = -1;
	HWND hSender = (HWND)wp;
	COPYDATASTRUCT * pCds = (COPYDATASTRUCT*)lp;
	switch (pCds->dwData)
	{
	case FC_REFRESH:
		{
			LPCTSTR pszDllName = (LPCTSTR)pCds->lpData;
			BYTE *pBuf = GetCaller(pszDllName);

			COPYDATASTRUCT cds = { 0 };
			cds.dwData = ACK_REFRESH;
			if (pBuf)
			{
				memcpy(&cds.cbData, pBuf, sizeof(DWORD));
				cds.lpData = pBuf + sizeof(DWORD);
				SendMessage(hSender, WM_COPYDATA, (WPARAM)hWnd, (LPARAM)&cds);
				FreeCallerInfo(pBuf);
			}
			else
			{
				SendMessage(hSender, WM_COPYDATA, (WPARAM)hWnd, (LPARAM)&cds);
			}
			lRet = 0;
		}
		break;
	case FC_KILLPID:
		if (pCds->cbData == sizeof(DWORD))
		{
			DWORD pid = 0;
			memcpy(&pid, pCds->lpData, sizeof(DWORD));
			BOOL bOK = Kill_Process(pid);
			lRet = bOK ? 0 : 1;
		}
		break;
	case FC_ACTIVATE:
		if (pCds->cbData == sizeof(DWORD))
		{
			DWORD pid = 0;
			memcpy(&pid, pCds->lpData, sizeof(DWORD));
			HWND hWnd=0;
			DWORD dwThreadID=0;
			GetProcessInfo(pid,hWnd,dwThreadID);
			DWORD dwCurId = GetCurrentThreadId();
			AttachThreadInput(dwCurId,dwThreadID,TRUE);
			if (IsIconic(hWnd))
			{
				ShowWindow(hWnd, SW_RESTORE);
			}
			SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			AttachThreadInput(dwCurId, dwThreadID, FALSE);
			lRet = 0;
		}
		break;
	case FC_QUIT:
		{
			PostQuitMessage(0);
			lRet = 0;
		}
		break;
	}
	return lRet;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_COPYDATA:
		return OnCopyData(hWnd,wParam,lParam);
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

