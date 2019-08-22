#include "stdafx.h"
#include "Utils.h"
#include <ShellAPI.h>
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
	if (g_SettingsG->nSoundAlert == 1)
	{
		SStringT strPath = SStringT().Format(_T("%s\\sound\\%s.wav"), CDataCenter::getSingletonPtr()->GetDataPath(), pszSound);
		PlaySound(strPath, NULL, SND_ASYNC | SND_FILENAME);
	}
	else if (g_SettingsG->nSoundAlert == 2)
	{
		MessageBeep(1000);
	}
}

//----------------------------------------------------------------------------
// GB2312码转GBK码
// 中华人民共和国 --> 中華人民共和國
int  CUtils::GB2GIB5(LPCTSTR szBuf, int nBufLen, TCHAR *szBig5, int nOutLen)
{
	DWORD wLCID = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_CHINESE_PRC);
	return LCMapString(wLCID, LCMAP_TRADITIONAL_CHINESE, szBuf, nBufLen*sizeof(TCHAR), szBig5, nOutLen * sizeof(TCHAR));
}

BOOL CUtils::CmdExecute(BYTE * pszBuf)
{
	UINT_PTR uRet = FALSE;
	char *pParam = NULL;
	char *pCmd = (char*)pszBuf + pszBuf[2] + 3 + 1;
	if (pCmd[0] == '\"')
	{
		pParam = strstr(pCmd, "\" ");
		if (pParam) { pParam[1] = 0; pParam += 2; }
	}
	else
	{
		pParam = strstr(pCmd, " ");
		if (pParam) pParam[0] = 0, pParam++;
	}
	uRet = (UINT_PTR)ShellExecuteA(NULL, "open", pCmd, pParam, NULL, SW_SHOWDEFAULT);
	if (uRet <= 32) uRet = (UINT_PTR)ShellExecuteA(NULL, "explorer", pCmd, NULL, NULL, SW_SHOWDEFAULT);
	return uRet>32;
}

