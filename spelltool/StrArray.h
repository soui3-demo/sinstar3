// StrArray.h: interface for the CStrArray class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STRARRAY_H__9F745161_BBA5_4006_8CA4_3F201B316DED__INCLUDED_)
#define AFX_STRARRAY_H__9F745161_BBA5_4006_8CA4_3F201B316DED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "sarray.hpp"

typedef struct tagSTRING
{
	char *pszBuf;
	int   nLen;
}STRING;

class CStrArray : public CSArray<STRING>  
{
public:
	int AddString(const char *pszString,int nLen=-1);
	void RemoveAll();
	CStrArray();
	virtual ~CStrArray();
};

typedef struct tagWSTRING
{
	WCHAR *pszBuf;
	int   nLen;
}WSTRING;

class CWStrArray : public CSArray<WSTRING>  
{
public:
	int AddString(const WCHAR *pszString,int nLen=-1);
	void RemoveAll();
	CWStrArray();
	virtual ~CWStrArray();
};
#endif // !defined(AFX_STRARRAY_H__9F745161_BBA5_4006_8CA4_3F201B316DED__INCLUDED_)
