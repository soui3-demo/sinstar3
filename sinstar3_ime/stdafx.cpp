#include "stdafx.h"

// TODO: 在 STDAFX.H 中
// 引用任何所需的附加头文件，而不是在此文件中引用
DWORD g_dwSystemInfoFlags;
HINSTANCE g_hInst;

#define MAX_TRACE	1024
void Helper_Trace(LPCTSTR pszFormat,...)
{
#ifdef _DEBUG
	static int nIdx=0;
	nIdx++;
	va_list args; 
	TCHAR buf[MAX_TRACE+1];
	TCHAR szFmt[200];
	if(pszFormat[0]=='\n')
	{
		_stprintf_s(szFmt,_T("\n%d\t:%s"),nIdx,pszFormat+1);
	}else
	{
		_stprintf_s(szFmt,_T("\n%d\t:%s"),nIdx,pszFormat);
	}

	va_start( args, pszFormat );
	_vsntprintf(buf,MAX_TRACE, szFmt,args);
	va_end (args);
	buf[MAX_TRACE]=0;
	OutputDebugString(buf);
#ifdef LOGFILE
	{
		FILE *f=_tfopen(LOGFILE,_T("a+"));
		if(f)
		{
			_ftprintf(f,buf);
			fclose(f);
		}
	}
#endif
#endif
}

