#include "stdafx.h"
#include "CAppBarUtils.h"

CAppBarUtils* g_AppBar = NULL;

CAppBarUtils::CAppBarUtils() : m_uMsgAppBar(0) , bFullScreen(false){
	m_uMsgAppBar = RegisterWindowMessage(L"my_appbar_callback_msg");
	Create(L"my_appbar_callback_window", 0, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL);

	abd.cbSize = sizeof(APPBARDATA);
	abd.hWnd = m_hWnd;
	abd.uCallbackMessage = m_uMsgAppBar;
	::SHAppBarMessage(ABM_NEW, &abd);
}

CAppBarUtils::~CAppBarUtils() {
	::SHAppBarMessage(ABM_REMOVE, &abd);
	DestroyWindow();
}

BOOL CAppBarUtils::IsFullScreen()
{
	return bFullScreen;
}

void CAppBarUtils::OnAppBarCallBack(UINT msgid, BOOL bFull)
{
	switch (msgid)
	{
		case ABN_FULLSCREENAPP:
			{
				bFullScreen=bFull;
			}break;
	}
}
