// StrArray.cpp: implementation of the CStrArray class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StrArray.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStrArray::CStrArray()
{

}

CStrArray::~CStrArray()
{
	RemoveAll();
}

void CStrArray::RemoveAll()
{
	for(int i=0;i<GetSize();i++)
	{
		STRING *p=GetPtAt(i);
		if(p->pszBuf) free(p->pszBuf);
	}
	CSArray<STRING>::RemoveAll();
}

int CStrArray::AddString(const char *pszString, int nLen)
{
	if(nLen==-1) nLen=strlen(pszString);
	STRING str;
	str.pszBuf=(char*)malloc(nLen+1);
	strncpy(str.pszBuf,pszString,nLen);
	str.pszBuf[nLen]=0;
	str.nLen=nLen;
	return Add(str);
}


/////////////////////////////////////////////////////////////
CWStrArray::CWStrArray()
{

}

CWStrArray::~CWStrArray()
{
	RemoveAll();
}

void CWStrArray::RemoveAll()
{
	for(int i=0;i<GetSize();i++)
	{
		WSTRING *p=GetPtAt(i);
		if(p->pszBuf) free(p->pszBuf);
	}
	CSArray<WSTRING>::RemoveAll();
}

int CWStrArray::AddString(const WCHAR *pszString, int nLen)
{
	if(nLen==-1) nLen=wcslen(pszString);
	WSTRING str;
	str.pszBuf=(WCHAR*)malloc((nLen+1)*2);
	wcsncpy(str.pszBuf,pszString,nLen);
	str.pszBuf[nLen]=0;
	str.nLen=nLen;
	return Add(str);
}
