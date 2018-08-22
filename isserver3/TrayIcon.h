/**
 * Copyright (C) 2014-2050 359501300
 * All rights reserved.
 *
 * @file       CTrayIcon.h
 * @brief
 * @version    v2.0
 * @author     359501300
 * @date       2017-09-11
 *
 * Describe
 */
#pragma once
#include <ShellAPI.h>

 /**
  * @class      CTrayIcon
  * @brief      ShellNotifyIcon
  *
  * Describe    ShellNotifyIcon
  */
namespace SOUI
{
#define UM_TRAYNOTIFY	(WM_USER+600)
#define ANI_ICON_NUM	4
	class CTrayIcon
	{
	public:
		/**
		 * CTrayIcon::CTrayIcon
		 * @brief    构造函数
		 *
		 * Describe  构造函数
		 */
		CTrayIcon();

		/**
		 * CTrayIcon::~CTrayIcon
		 * @brief    析构函数
		 *
		 * Describe  析构函数
		 */
		~CTrayIcon();

		void Init(HWND hOwner, LPCTSTR szTip);

		BOOL Show();

		BOOL Hide();

		void AnimateIcon();
	protected:
		void OnTimer(UINT_PTR uID);
		BEGIN_MSG_MAP_EX(CTrayIcon)
			MSG_WM_TIMER(OnTimer)
		END_MSG_MAP()

	protected:

		NOTIFYICONDATA m_nid;

		HICON	 m_AniLogo[ANI_ICON_NUM];
		int		 m_iIcon;

		BOOL	 m_bVisible;
	};
}