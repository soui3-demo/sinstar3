// GroupManager.h: interface for the CGroupManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GROUPMANAGER_H__E28B98C3_204D_4502_8F0D_9CEA3EAC86A4__INCLUDED_)
#define AFX_GROUPMANAGER_H__E28B98C3_204D_4502_8F0D_9CEA3EAC86A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

typedef struct tagGROUPINFO
{//组信息
	char szName[50];	//名称
	char szEditor[50];	//编辑
	char szRemark[200];	//备注
	DWORD dwCount;		//词数
	BOOL  bValid;		//有效标志
}GROUPINFO,*PGROUPINFO;

class CGroupManager  
{
public:
	CGroupManager();
	virtual ~CGroupManager();

	char AddGroup(LPCSTR pszName,LPCSTR pszEditor,LPCSTR pszRemark);
	void Free();
	BOOL Read(FILE *f);
	BOOL Write(FILE *f);
	DWORD GetCount();
	const std::vector<GROUPINFO> & GetGroup() const{ return m_arrGroup;}
protected:
	std::vector<GROUPINFO>	m_arrGroup;

};

GROUPINFO * Group_Read(FILE *f,char *pcGroups);
void		Group_Write(FILE *f,GROUPINFO *pGroup,char cGroups);
#endif // !defined(AFX_GROUPMANAGER_H__E28B98C3_204D_4502_8F0D_9CEA3EAC86A4__INCLUDED_)
