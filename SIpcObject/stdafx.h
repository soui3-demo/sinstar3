// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//  are changed infrequently
//

#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS
#include "..\targetver.h"
#include <Windows.h>
#include <assert.h>
#include <tchar.h>
#include "../SLog4Z/log4z.h"

#define GETLOGMGR() (SOUI::SLogMgr::getSingletonPtr()?SOUI::SLogMgr::getSingletonPtr()->GetLogMgr():NULL)
#include "../include/slog_wrapper.h"

#define LOG_FILTER "sinstar3_ipc"
#include <helper/slogdef.h>
