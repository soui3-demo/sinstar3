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
#include "../include/version.h"
#include "../include/TextService-i.h"

#include "TsfModule.h"
#include "Globals.h"
#include "sinstar3_guids.h"

#define ASSERT_HR(hr) _ASSERT(SUCCEEDED(hr))
#define ASSERT_RET(x,ret) _ASSERT(x);if(!x) ret

