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

BYTE CGroupManager::AddGroup(LPCWSTR pszName,LPCWSTR pszEditor,LPCWSTR pszRemark)
{
	if(m_arrGroup.size()==0x7F) return -1;
	if(wcslen(pszName)>=50 || wcslen(pszEditor)>=50 || wcslen(pszRemark)>=200) return -1;
	GROUPINFO gi={0};
	wcscpy(gi.szName,pszName);
	wcscpy(gi.szEditor,pszEditor);
	wcscpy(gi.szRemark,pszRemark);
	gi.dwCount=0;
	m_arrGroup.push_back(gi);
	return (BYTE)m_arrGroup.size()-1;
}

BOOL CGroupManager::Write(FILE *f)
{
	BYTE byGroups=m_arrGroup.size();
	fwrite(&byGroups,1,1,f);
	fwrite(&m_arrGroup[0],sizeof(GROUPINFO),byGroups,f);
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
		m_arrGroup.push_back(gi);
	}	
	return TRUE;
}

DWORD CGroupManager::GetCount()
{
	DWORD dwRet=0;
	for(int i=0;i<m_arrGroup.size();i++)
		dwRet+=m_arrGroup[i].dwCount;
	return dwRet;
}

void CGroupManager::Free()
{
	m_arrGroup.clear();
}

BOOL CGroupManager::SetGroup(BYTE iGroup,const GROUPINFO & groupInfo)
{
	if(iGroup>=m_arrGroup.size())
		return FALSE;
	DWORD dwCount = m_arrGroup[iGroup].dwCount;
	m_arrGroup[iGroup]=groupInfo;
	m_arrGroup[iGroup].dwCount = dwCount;
	return TRUE;
}

BOOL CGroupManager::ValidGroup(BYTE iGroup,BOOL bValid)
{
	if(iGroup>=m_arrGroup.size())
		return FALSE;
	m_arrGroup[iGroup].bValid = bValid;
	return TRUE;
}

GROUPINFO * Group_Read(FILE *f,char *pcGroups)
{
	SASSERT(pcGroups);
	fread(pcGroups,1,1,f);
	GROUPINFO *pgi=(GROUPINFO*)malloc(sizeof(GROUPINFO)*(*pcGroups));
	if(!pgi) return NULL;
	fread(pgi,sizeof(GROUPINFO),*pcGroups,f);
	return pgi;
}

void Group_Write(FILE *f,GROUPINFO *pGroup,char cGroups)
{
	fwrite(&cGroups,1,1,f);
	fwrite(pGroup,sizeof(GROUPINFO),cGroups,f);
}