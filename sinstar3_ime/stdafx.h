// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
#define ENABLE_LANGUAGEBAR
#define _CRT_NON_CONFORMING_SWPRINTFS
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif//_CRT_SECURE_NO_WARNINGS

// Windows 头文件:
#define NOIME
#include <windows.h>
#include <tchar.h>
#include <crtdbg.h>

#include "imm.h"
#pragma comment(lib,"imm32.lib")

#include "imeContext.h"

#ifndef SASSERT
#include <assert.h>
#define SASSERT(x) assert(x)
#endif

#include "../include/version.h"
#include "../include/sinstar-i.h"
#include "../include/reg.h"
#include "../include/slog_wrapper.h"

#define LOG_FILTER "sinstar3_ime"
#include <helper/slog-def.h>

#include "../core_loader/core_loader.h"
#include "ImeModule.h"

