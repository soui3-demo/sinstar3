// ForeignLangMap.h: interface for the CForeignLangMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FOREIGNLANGMAP_H__B61838E0_0F93_486E_8D95_0F88CA8467AE__INCLUDED_)
#define AFX_FOREIGNLANGMAP_H__B61838E0_0F93_486E_8D95_0F88CA8467AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "groupmanager.h"
#include "slist.hpp"
#include <list>
#include "../include/iscore-i.h"
typedef struct tagFLMHEAD
{
	DWORD dwFlBodySize;
	DWORD dwFlIndexSize;
	DWORD dwCnBodySize;
	DWORD dwCnIndexSize;
}FLMHEAD;

typedef void (*FunProgCallback)(PROGTYPE uType,DWORD dwData,LPARAM lParam);

#define SSID_FL	'LF'
#define SVFL_MAJOR1 2020
#define SVFL_MINOR1	1

class CFLMEditor  :public CGroupManager
{
	typedef struct tagCNITEM
	{
		WCHAR *pszCnBuf;		//中文词条
		int	  nFLCount;		//相匹配的外文词条数
		union
		{
			SLISTPOS *pPos;		//相匹配的外文词条位置
			int		 *pIndex;
		};
	}CNITEM;

	typedef struct tagFLITEM
	{
		WCHAR *pszFLBuf;
		char  cGroup;
		WCHAR *pszAddition;
		int   nCnCount;
		union
		{
			SLISTPOS *pPos;		//相匹配的外文词条位置
			int		 *pIndex;
		};
	}FLITEM;
public:
	BOOL Save(LPCTSTR pszFileName);
	BOOL Load(LPCTSTR pszFileName);
	void Free();
	BOOL AddItem(char cGroup,LPCWSTR pszFLBuf,LPCWSTR pszAddition,CStringArray *pArrCnBuf);
	void SetInfo(LPCWSTR pszName,LPCWSTR pszAddition,LPCWSTR pszAddFont);
	void SetProgCallBack(FunProgCallback funCb,LPARAM cbParam)
	{
		m_funProgCB=funCb;
		m_cbParam=cbParam;
	}
	
	CFLMEditor();
	virtual ~CFLMEditor();

	CSList<CNITEM> m_lstCnItem;
	CSList<FLITEM> m_lstFLItem;
	FLMINFO			m_flmInfo;
protected:
	FunProgCallback	m_funProgCB;
	LPARAM			m_cbParam;		//回调Param
};

typedef struct tagFOREIGNINFO{
	WCHAR szKey[100];
	WCHAR szAddition[100];
}FOREIGNINFO;

typedef struct tagCHINESEINFO
{
	WCHAR szKey[100];
	int  nLen;
}CHINESEINFO;

typedef struct tagQUERYHEAD
{
	LPCWSTR pszHead;
	int  nLen;
	BOOL bPart;
}QUERYHEAD;

#endif // !defined(AFX_FOREIGNLANGMAP_H__B61838E0_0F93_486E_8D95_0F88CA8467AE__INCLUDED_)
