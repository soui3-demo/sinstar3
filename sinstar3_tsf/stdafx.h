#pragma once

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif//_CRT_SECURE_NO_WARNINGS

#define ENABLE_LANGUAGEBAR

#include <windows.h>
#include <ole2.h>
#include <olectl.h>
#include <assert.h>
#include <tchar.h>
#include <msctf.h>
#include <atl.mini/SComCli.h>

#include "../core_loader/core_loader.h"
#include "../include/unknown.h"
#include "../include/reg.h"
#include "../include/version.h"
#include "../include/TextService-i.h"
#include "../include/sinstar3_guids.h"
#include "../include/slog_wrapper.h"

#define LOG_FILTER "sinstar3_tsf"
#include <helper/slog-def.h>

#include "TsfModule.h"
#include "Globals.h"

#define SASSERT_HR(hr) SASSERT(SUCCEEDED(hr))
#define SASSERT_RET(x,ret) SASSERT(x);if(!x) ret

#ifndef ARRAYSIZE
#define ARRAYSIZE(a) (sizeof(a)/sizeof(a[0]))
#endif

#define TEXTSERVICE_LANGID			MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED)
