#pragma once
#include <shellapi.h>

#define MESSAGE_APPBAR(msg, func) \
if(uMsg == msg) \
{ \
	SetMsgHandled(TRUE);\
	func((UINT)wParam,(BOOL)lParam); \
	if(IsMsgHandled()) \
		return TRUE; \
}	

class CAppBarUtils :public SNativeWnd
{
public:
	CAppBarUtils();	
	~CAppBarUtils();
	BOOL IsFullScreen();
protected:
	void OnAppBarCallBack(UINT msgid, BOOL bFull);

	BEGIN_MSG_MAP_EX(CAppBarUtils)		
		MESSAGE_APPBAR(m_uMsgAppBar, OnAppBarCallBack)
		CHAIN_MSG_MAP(SNativeWnd)
	END_MSG_MAP()
private:
	UINT m_uMsgAppBar;
	APPBARDATA abd;
	BOOL bFullScreen;
};

extern CAppBarUtils* g_AppBar;