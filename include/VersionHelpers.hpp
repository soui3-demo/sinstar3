#pragma once

#ifndef NTSTATUS
typedef __success(return >= 0) LONG NTSTATUS;
#endif

#define VERSIONHELPERAPI inline bool

#define _WIN32_WINNT_NT4            0x0400
#define _WIN32_WINNT_WIN2K          0x0500
#define _WIN32_WINNT_WINXP          0x0501
#define _WIN32_WINNT_WS03           0x0502
#define _WIN32_WINNT_WIN6           0x0600
#define _WIN32_WINNT_VISTA          0x0600
#define _WIN32_WINNT_WS08           0x0600
#define _WIN32_WINNT_LONGHORN       0x0600
#define _WIN32_WINNT_WIN7           0x0601
#define _WIN32_WINNT_WIN8           0x0602
#define _WIN32_WINNT_WINBLUE        0x0603
#define _WIN32_WINNT_WIN10          0x0A00

typedef NTSTATUS(NTAPI* fnRtlGetVersion)(PRTL_OSVERSIONINFOEXW lpVersionInformation);

enum eVerShort
{
	WinUnsupported, // Unsupported OS 
	WinXP,          // Windows XP
	Win7,           // Windows 7
	Win8,           // Windows 8
	Win8Point1,     // Windows 8.1
	Win10,          // Windows 10
	Win10AU,        // Windows 10 Anniversary update
	Win10CU,        // Windows 10 Creators update
	Win10FC,        // Windows 10 Fall Creators update
};

struct WinVersion
{
	eVerShort ver;
	RTL_OSVERSIONINFOEXW native;

	WinVersion()
	{
		native.dwOSVersionInfoSize = sizeof(native);
		fnRtlGetVersion RtlGetVersion = (fnRtlGetVersion)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "RtlGetVersion");
		if (RtlGetVersion)
			RtlGetVersion(&native);

		if (native.dwMajorVersion != 0)
		{
			DWORD fullver = (native.dwMajorVersion << 8) | native.dwMinorVersion;
			switch (fullver)
			{
			case _WIN32_WINNT_WIN10:
				if (native.dwBuildNumber >= 16299)
					ver = Win10FC;
				else if (native.dwBuildNumber >= 15063)
					ver = Win10CU;
				else if (native.dwBuildNumber >= 14393)
					ver = Win10AU;
				else if (native.dwBuildNumber >= 10586)
					ver = Win10;
				break;

			case _WIN32_WINNT_WINBLUE:
				ver = Win8Point1;
				break;

			case _WIN32_WINNT_WIN8:
				ver = Win8;
				break;

			case _WIN32_WINNT_WIN7:
				ver = Win7;
				break;

			case _WIN32_WINNT_WINXP:
				ver = WinXP;
				break;

			default:
				ver = WinUnsupported;
			}
		}
	}
};

inline WinVersion& WinVer()
{
	static WinVersion g_WinVer;
	return g_WinVer;
}


VERSIONHELPERAPI
IsWindowsVersionOrGreater(WORD wMajorVersion, WORD wMinorVersion, WORD wServicePackMajor, DWORD dwBuild)
{
	WinVersion& g_WinVer = WinVer();
	if (g_WinVer.native.dwMajorVersion != 0)
	{
		if (g_WinVer.native.dwMajorVersion > wMajorVersion)
			return true;
		else if (g_WinVer.native.dwMajorVersion < wMajorVersion)
			return false;

		if (g_WinVer.native.dwMinorVersion > wMinorVersion)
			return true;
		else if (g_WinVer.native.dwMinorVersion < wMinorVersion)
			return false;

		if (g_WinVer.native.wServicePackMajor > wServicePackMajor)
			return true;
		else if (g_WinVer.native.wServicePackMajor < wServicePackMajor)
			return false;

		if (g_WinVer.native.dwBuildNumber >= dwBuild)
			return true;
	}

	return false;
}

VERSIONHELPERAPI
IsWindowsXPOrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINXP), LOBYTE(_WIN32_WINNT_WINXP), 0, 0);
}

VERSIONHELPERAPI
IsWindowsXPSP1OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINXP), LOBYTE(_WIN32_WINNT_WINXP), 1, 0);
}

VERSIONHELPERAPI
IsWindowsXPSP2OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINXP), LOBYTE(_WIN32_WINNT_WINXP), 2, 0);
}

VERSIONHELPERAPI
IsWindowsXPSP3OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINXP), LOBYTE(_WIN32_WINNT_WINXP), 3, 0);
}

VERSIONHELPERAPI
IsWindowsVistaOrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_VISTA), LOBYTE(_WIN32_WINNT_VISTA), 0, 0);
}

VERSIONHELPERAPI
IsWindowsVistaSP1OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_VISTA), LOBYTE(_WIN32_WINNT_VISTA), 1, 0);
}

VERSIONHELPERAPI
IsWindowsVistaSP2OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_VISTA), LOBYTE(_WIN32_WINNT_VISTA), 2, 0);
}

VERSIONHELPERAPI
IsWindows7OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN7), LOBYTE(_WIN32_WINNT_WIN7), 0, 0);
}

VERSIONHELPERAPI
IsWindows7SP1OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN7), LOBYTE(_WIN32_WINNT_WIN7), 1, 0);
}

VERSIONHELPERAPI
IsWindows8OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN8), LOBYTE(_WIN32_WINNT_WIN8), 0, 0);
}

VERSIONHELPERAPI
IsWindows8Point1OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINBLUE), LOBYTE(_WIN32_WINNT_WINBLUE), 0, 0);
}

VERSIONHELPERAPI
IsWindows10OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN10), LOBYTE(_WIN32_WINNT_WIN10), 0, 0);
}

VERSIONHELPERAPI
IsWindows10AnniversaryOrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN10), LOBYTE(_WIN32_WINNT_WIN10), 0, 14393);
}

VERSIONHELPERAPI
IsWindows10CreatorsOrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN10), LOBYTE(_WIN32_WINNT_WIN10), 0, 15063);
}

VERSIONHELPERAPI
IsWindows10FallCreatorsOrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN10), LOBYTE(_WIN32_WINNT_WIN10), 0, 16299);
}

VERSIONHELPERAPI
IsWindowsServer()
{
	OSVERSIONINFOEXW osvi = { sizeof(osvi), 0, 0, 0, 0,{ 0 }, 0, 0, 0, VER_NT_WORKSTATION };
	DWORDLONG        const dwlConditionMask = VerSetConditionMask(0, VER_PRODUCT_TYPE, VER_EQUAL);

	return !VerifyVersionInfoW(&osvi, VER_PRODUCT_TYPE, dwlConditionMask);
}
