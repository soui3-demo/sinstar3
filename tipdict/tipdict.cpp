// tipdict.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "tipdict.h"
#include <string>
using namespace std;

//query dict demo
//output buffer can be soui layout xml or plain text.
TIPDICT_API int TipDict(LPCWSTR pszKey,int nKeyLen,LPWSTR pszBuf,int nBufLen)
{
	if(nBufLen<1000)
		return 1000;
	static const wchar_t * KFmt=L"<text size=\"-1,-1\" colorText=\"#ff0000\">点击搜索:%s</text>";
	return swprintf_s(pszBuf,nBufLen,KFmt,pszKey);
}
