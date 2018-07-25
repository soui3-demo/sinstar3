#include "stdafx.h"
#include "Utils.h"
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")


CUtils::CUtils()
{
}


CUtils::~CUtils()
{
}


int CUtils::GetClipboardText(HWND hWnd, WCHAR *pszBuf, int nBufSize)
{
	int nRet = 0;
	if (OpenClipboard(hWnd))
	{//剪贴板数据优先
		HGLOBAL hglb = GetClipboardData(CF_UNICODETEXT);
		if (hglb)
		{
			LPWSTR lpstr = (LPWSTR)GlobalLock(hglb);
			SStringW str = lpstr;
			str.TrimBlank();
			if (str.GetLength() < nBufSize)
			{
				wcscpy(pszBuf, str);
				nRet = str.GetLength();
			}
			GlobalUnlock(hglb);
		}
		CloseClipboard();
	}
	return nRet;
}


void CUtils::SoundPlay(LPCTSTR pszSound)
{
	if (g_SettingsG.nSoundAlert == 1)
	{
		SStringT strPath = SStringT().Format(_T("%s\\sound\\%s.wav"), theModule->GetDataPath(), pszSound);
		PlaySound(strPath, NULL, SND_ASYNC | SND_FILENAME);
	}
	else if (g_SettingsG.nSoundAlert == 2)
	{
		MessageBeep(1000);
	}
}

//----------------------------------------------------------------------------
// GB2312码转GBK码
// 中华人民共和国 --> 中華人民共和國
int  CUtils::GB2GIB5(TCHAR *szBuf, int nStrLen)
{
	int nRet = 0;
	if (nStrLen == -1) nStrLen = (int)_tcslen(szBuf);
	if (nStrLen>0)
	{
		DWORD wLCID = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_CHINESE_PRC);
		int nReturn = LCMapString(wLCID, LCMAP_TRADITIONAL_CHINESE, szBuf, nStrLen, NULL, 0);
		if (nReturn)
		{
			TCHAR *pcBuf = (TCHAR*)malloc((nReturn + 1)*sizeof(TCHAR));
			wLCID = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_CHINESE_PRC);
			LCMapString(wLCID, LCMAP_TRADITIONAL_CHINESE, szBuf, nReturn, pcBuf, nReturn + 1);
			if (nReturn <= nStrLen)
			{
				_tcsncpy(szBuf, pcBuf, nReturn);
				nRet = nReturn;
			}
			free(pcBuf);
		}
	}
	return nRet;
}

typedef BOOL(WINAPI *FunChangeMsgFilter)(UINT, DWORD);

BOOL CUtils::ChangeWindowMessageFilter(UINT message, DWORD dwFlag)
{
	HMODULE hUser32 = GetModuleHandle(_T("user32.dll"));
	if (hUser32)
	{
		FunChangeMsgFilter fMsgFilter = (FunChangeMsgFilter)GetProcAddress(hUser32, "ChangeWindowMessageFilter");
		if (!fMsgFilter) return FALSE;
		return fMsgFilter(message, dwFlag);
	}
	else
	{
		return FALSE;
	}
}

