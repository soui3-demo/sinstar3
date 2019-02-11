#include "stdafx.h"
#include "ShellTypeReg.h"

CShellTypeReg::CShellTypeReg(void)
{
}

CShellTypeReg::~CShellTypeReg(void)
{
}

BOOL CShellTypeReg::RegShellType(const tstring &strType,const tstring & strFileTypeID, const tstring & strFileTypeName,const tstring & strOpenFmt, int iIcon)
{
	{//key type id and name.
		if(::RegSetValue(HKEY_CLASSES_ROOT, strFileTypeID.c_str(), REG_SZ,
			strFileTypeName.c_str(), strFileTypeName.length() * sizeof(TCHAR)) != ERROR_SUCCESS)
			return FALSE;
	}
	{//set type ext.
		if(::RegSetValue(HKEY_CLASSES_ROOT, strType.c_str(), REG_SZ,
			strFileTypeID.c_str(), strFileTypeID.length() * sizeof(TCHAR)) != ERROR_SUCCESS)
			return FALSE;

	}
	{
		TCHAR szPath[MAX_PATH],szShortPath[MAX_PATH];
		::GetModuleFileName(NULL,szPath,MAX_PATH);
		::GetShortPathName(szPath,szShortPath,MAX_PATH);

		CRegKey key;
		if(ERROR_SUCCESS!=key.Create(HKEY_CLASSES_ROOT,strFileTypeID.c_str()))
			return FALSE;

		//set icon
		TCHAR szKey[MAX_PATH],szTmp[MAX_PATH];
		_stprintf(szTmp,_T("%s,%d"),szShortPath,iIcon);
		_stprintf(szKey,_T("%s\\DefaultIcon"),strFileTypeID.c_str());

		{
			CRegKey key;
			key.Create(HKEY_CLASSES_ROOT,szKey);
			key.SetStringValue(NULL,szTmp);
		}

		//set shell open cmd
		_stprintf(szTmp,strOpenFmt.c_str(),szShortPath);
		_stprintf(szKey,_T("%s\\shell\\open\\command"),strFileTypeID.c_str());
		{
			CRegKey key;
			key.Create(HKEY_CLASSES_ROOT,szKey);
			key.SetStringValue(NULL,szTmp);
		}
		key.Close();
	}
	return TRUE;
}

BOOL CShellTypeReg::UnregShellType(const tstring &strType)
{
	CRegKey key;
	if(ERROR_SUCCESS != key.Open(HKEY_CLASSES_ROOT,strType.c_str()))
		return FALSE;
	TCHAR szTypeID[200];
	ULONG nLen = 200;
	if(ERROR_SUCCESS != key.QueryStringValue(NULL,szTypeID,&nLen))
		return FALSE;
	key.Close();
	if(ERROR_SUCCESS != key.Open(HKEY_CLASSES_ROOT,NULL))
		return FALSE;
	key.RecurseDeleteKey(strType.c_str());
	key.RecurseDeleteKey(szTypeID);
	return TRUE;
}
