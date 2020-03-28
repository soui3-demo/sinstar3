#pragma once
class CUtils
{
public:
	CUtils();
	~CUtils();

	static void SoundPlay(LPCTSTR pszSound);
	static int GetClipboardText(HWND hWnd, WCHAR *pszBuf, int nBufSize);
	static int GB2GIB5(LPCWSTR szBuf, int nStrLen, WCHAR * szBig5, int nOutLen);
	static BOOL CmdExecute(BYTE * pszBuf);
};

