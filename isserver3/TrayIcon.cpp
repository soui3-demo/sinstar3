//////////////////////////////////////////////////////////////////////////
//  Class Name: CTrayIcon
// Description:show ShellNotifyIcon
//     Creator: 359501300
//     Version: 2017.9.11 - 2.0 - Create
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "trayicon.h"

namespace SOUI
{
#define TIMERID_ANI_LOGO	100
#define IDC_LOGOTRAY		1324	//TRAY ID

	CTrayIcon::CTrayIcon():m_iIcon(0), m_bVisible(FALSE)
	{
		memset(&m_nid, 0, sizeof(m_nid));
		for (int i = 0; i <= ANI_ICON_NUM; i++)
		{
			m_AniLogo[i] = LOADICON2(SStringT().Format(_T("icon_logo%d"), i + 1));
		}
	}

	CTrayIcon::~CTrayIcon() {
		for (int i = 0; i < ANI_ICON_NUM; i++)
		{
			DestroyIcon(m_AniLogo[i]);
		}
	}


	void CTrayIcon::Init(HWND hOwner, LPCTSTR szTip)
	{	
		m_nid.cbSize = sizeof(NOTIFYICONDATA);
		m_nid.hWnd = hOwner;
		m_nid.uID = IDC_LOGOTRAY;
		m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		m_nid.uCallbackMessage = UM_TRAYNOTIFY;
		m_nid.hIcon = m_AniLogo[0];
		_tcscpy_s(m_nid.szTip, szTip);
	}

	BOOL CTrayIcon::Show()
	{
		m_nid.hIcon = m_AniLogo[0];
		m_bVisible = TRUE;
		return Shell_NotifyIcon(NIM_ADD, &m_nid);
	}

	BOOL CTrayIcon::Hide()
	{
		m_bVisible = FALSE;
		return Shell_NotifyIcon(NIM_DELETE, &m_nid);
	}

	void CTrayIcon::AnimateIcon()
	{
		if (!m_bVisible) return;
		m_iIcon = 0;
		SetTimer(m_nid.hWnd, TIMERID_ANI_LOGO, 50, NULL);
		OnTimer(TIMERID_ANI_LOGO);
	}

	void CTrayIcon::OnTimer(UINT_PTR uID)
	{
		if (uID == TIMERID_ANI_LOGO)
		{
			if (++m_iIcon == ANI_ICON_NUM)
			{
				::KillTimer(m_nid.hWnd, uID);
				m_iIcon = 0;
			}
			m_nid.hIcon = m_AniLogo[m_iIcon];
			Shell_NotifyIcon(NIM_MODIFY, &m_nid);
		}
		else
		{
			SetMsgHandled(FALSE);
		}
	}



}