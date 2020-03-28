#pragma once

class CUtils
{
public:
	CUtils(void);
	~CUtils(void);


	static BOOL CopyCompFormWStr2AStr(char * pDst,const WCHAR * pSour, int nLen=-1);
	static void CopyCompFromAStr2WStr(WCHAR *pDst,const char *pSour,int nLen=-1);
};
