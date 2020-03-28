// GroupManager.cpp: implementation of the CGroupManager class.
//	分组管理模块
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GroupManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGroupManager::CGroupManager()
{

}

CGroupManager::~CGroupManager()
{
	Free();
}

char CGroupManager::AddGroup(LPCWSTR pszName,LPCWSTR pszEditor,LPCWSTR pszRemark)
{
	if(m_arrGroup.GetSize()==0x7F) return -1;
	if(wcslen(pszName)>=50 || wcslen(pszEditor)>=50 || wcslen(pszRemark)>=200) return -1;
	GROUPINFO gi={0};
	wcscpy(gi.szName,pszName);
	wcscpy(gi.szEditor,pszEditor);
	wcscpy(gi.szRemark,pszRemark);
	gi.dwCount=0;
	return (char)m_arrGroup.Add(gi);
}

BOOL CGroupManager::Write(FILE *f)
{
	BYTE byGroups=m_arrGroup.GetSize();
	fwrite(&byGroups,1,1,f);
	for(BYTE i=0;i<byGroups;i++)
	{
		fwrite(m_arrGroup.GetPtAt(i),sizeof(GROUPINFO),1,f);
	}
	return TRUE;
}

BOOL CGroupManager::Read(FILE *f)
{
	BYTE byGroups=0;
	fread(&byGroups,1,1,f);
	GROUPINFO gi;
	for(BYTE i=0;i<byGroups;i++)
	{
		fread(&gi,sizeof(GROUPINFO),1,f);
		m_arrGroup.Add(gi);
	}	
	return TRUE;
}

DWORD CGroupManager::GetCount()
{
	DWORD dwRet=0;
	for(int i=0;i<m_arrGroup.GetSize();i++)
		dwRet+=m_arrGroup.GetPtAt(i)->dwCount;
	return dwRet;
}

void CGroupManager::Free()
{
	m_arrGroup.RemoveAll();
}

GROUPINFO * Group_Read(FILE *f,char *pcGroups)
{
	ASSERT(pcGroups);
	fread(pcGroups,1,1,f);
	GROUPINFO *pgi=(GROUPINFO*)malloc(sizeof(GROUPINFO)*(*pcGroups));
	if(!pgi) return NULL;
	fread(pgi,sizeof(GROUPINFO),*pcGroups,f);
	return pgi;
}

void		Group_Write(FILE *f,GROUPINFO *pGroup,char cGroups)
{
	fwrite(&cGroups,1,1,f);
	fwrite(pGroup,sizeof(GROUPINFO),cGroups,f);
}