#include "StdAfx.h"
#include "Utils.h"

CUtils::CUtils(void)
{
}

CUtils::~CUtils(void)
{
}

BOOL CUtils::CopyCompFormWStr2AStr(char * pDst,const WCHAR * pSour, int nLen)
{
	if(nLen == -1)
		nLen = wcslen(pSour);
	for(int i=0;i<nLen;i++)
	{
		if(!CWordID::IsAlphabit(pSour[i])) return FALSE;
		pDst[i]=(char)pSour[i];
	}
	pDst[nLen]=0;
	return TRUE;
}

void CUtils::CopyCompFromAStr2WStr(WCHAR *pDst,const char *pSour,int nLen)
{
	if(nLen == -1)
		nLen = strlen(pSour);
	for(int i=0;i<nLen;i++)
	{
		ASSERT(pSour[i]>0);
		pDst[i]=(WCHAR)pSour[i];
	}
	pDst[nLen]=0;
}
