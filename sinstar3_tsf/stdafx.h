#pragma once

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
#define ENABLE_LANGUAGEBAR
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <ole2.h>
#include <olectl.h>
#include <assert.h>
#include <tchar.h>
#include <msctf.h>
#include <atlcomcli.h>

#include "../core_loader/core_loader.h"
#include "../include/unknown.h"
#include "../include/reg.h"

#include "Globals.h"

#include "../include/common_def.h"
#include "../include/TextService-i.h"
#include "IMEMan.h"

#define XLOG_FUNCTION 
#define BD_VERIFY(x) _ASSERT(x)
#define BD_VERIFY_HR(hr) _ASSERT(SUCCEEDED(hr))
#define BD_VERIFY_RET(x,ret) _ASSERT(x);if(!x) ret
