#pragma once

#ifdef DLL_CORE
#   define SOUI_EXP __declspec(dllimport)
#else
#   define SOUI_EXP
#endif

#include <interface/slog-i.h>

#define GETLOGMGR() NULL
#include <helper/slog.h>

