#pragma once
class CUtils
{
public:
	CUtils();
	~CUtils();

	static void SoundPlay(LPCTSTR pszSound);
	static int GB2GIB5(TCHAR * szBuf, int nStrLen);
	static int GetClipboardText(HWND hWnd, WCHAR *pszBuf, int nBufSize);
	static BOOL ChangeWindowMessageFilter(UINT message, DWORD dwFlag);
};

