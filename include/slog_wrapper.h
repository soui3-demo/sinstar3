#pragma once

#ifdef DLL_CORE
#   define SOUI_EXP __declspec(dllimport)
#else
#   define SOUI_EXP
#endif

#include <interface/slog-i.h>

#ifndef GETLOGMGR
#define GETLOGMGR() NULL
#endif
#include <helper/slog.h>

