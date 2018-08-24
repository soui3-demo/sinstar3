#pragma once

#include <tchar.h>

class CMinidump
{
public:
    static void Enable();
	static void SetDumpFile(LPCWSTR pszDumpFile);
};

