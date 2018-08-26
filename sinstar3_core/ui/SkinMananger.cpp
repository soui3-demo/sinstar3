#include "StdAfx.h"
#include "SkinMananger.h"
#include <helper/SMenu.h>
#include <helper/mybuffer.h>
#include "../SouiEnv.h"

CSkinMananger::CSkinMananger()
{
}

CSkinMananger::~CSkinMananger(void)
{
}

int CSkinMananger::InitSkinMenu(SMenuEx *hMenu, const SStringT &strSkinPath, int nStartId, const SStringT &strCurSkin)
{
	WIN32_FIND_DATA findData;
	HANDLE hFind = FindFirstFile(strSkinPath + _T("\\*.*"), &findData);
	if (hFind != INVALID_HANDLE_VALUE)
	{		
		do {
			if (findData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				if (_tcscmp(findData.cFileName, _T(".")) != 0 && _tcscmp(findData.cFileName, _T("..")) != 0)
				{					
					if (hMenu->InsertMenu(R.id.skin_def, MF_POPUP| MF_BYCOMMAND, ++nStartId, findData.cFileName))
					{
						nStartId = InitSkinMenu(hMenu->GetSubMenu(nStartId), strSkinPath + _T("\\") + findData.cFileName, nStartId, strCurSkin);
					}
				}
			}
			else
			{
				TCHAR szExt[100];
				_tsplitpath(findData.cFileName, NULL, NULL, NULL, szExt);
				if (_tcsicmp(szExt, _T(".sskn")) == 0)
				{
					nStartId++;
					SStringT strFullPath = strSkinPath + _T("\\") + findData.cFileName;
					m_mapSkin[nStartId] = strFullPath;
					SStringT strDesc = ExtractSkinInfo(strFullPath);

					BOOL bInsertSucess=hMenu->InsertMenu(-1, MF_BYPOSITION, nStartId, strDesc);
					
					if (bInsertSucess&&(strFullPath == strCurSkin))
					{
						hMenu->CheckMenuItem(nStartId, MF_BYCOMMAND | MF_CHECKED);
					}
				}
			}
		} while (FindNextFile(hFind, &findData));
		FindClose(hFind);
	}
	return nStartId;
}

SOUI::SStringT CSkinMananger::ExtractSkinInfo(SStringT strSkinPath)
{
	IResProvider *pResProvider = NULL;
	{
		CCurrentDirSet curDirSet(CSouiEnv::getSingletonPtr()->SouiDllDir());
		CSouiEnv::getSingleton().theComMgr()->CreateResProvider_ZIP((IObjRef**)&pResProvider);
	}
	ZIPRES_PARAM param;
	param.ZipFile(NULL, strSkinPath);
	pResProvider->Init((WPARAM)&param,0);
	int nSize = (int)pResProvider->GetRawBufferSize(_T("uidef"),_T("xml_init"));
	CMyBuffer<char> buffer(nSize);
	pResProvider->GetRawBuffer(_T("uidef"),_T("xml_init"),buffer,nSize);
	pResProvider->Release();

	pugi::xml_document xmlDoc;
	xmlDoc.load_buffer_inplace(buffer,nSize);
	pugi::xml_node xmlDesc = xmlDoc.child(L"uidef").child(L"desc");
	SStringW strDesc = SStringW(xmlDesc.attribute(L"name").value())+L"["+xmlDesc.attribute(L"author").value()+L"]";
	return S_CW2T(strDesc);
}

SStringT CSkinMananger::SkinPathFromID(int nSkinID) const
{
	SStringT strSkinPath;
	if (const SMap<int, SStringT>::CPair * p = m_mapSkin.Lookup(nSkinID))
	{
		strSkinPath = p->m_value;
	}
	return strSkinPath;
}

CPoint CSkinMananger::ExtractSkinOffset(IResProvider * pResProvider)
{
	int nSize = (int)pResProvider->GetRawBufferSize(_T("uidef"), _T("xml_init"));

	CMyBuffer<char> buffer(nSize);
	pResProvider->GetRawBuffer(_T("uidef"), _T("xml_init"), buffer, nSize);

	pugi::xml_document xmlDoc;
	xmlDoc.load_buffer_inplace(buffer, nSize);
	pugi::xml_node xmlOffset = xmlDoc.child(L"uidef").child(L"offset");
	CPoint pt;
	pt.x = xmlOffset.attribute(L"x").as_int();
	pt.y = xmlOffset.attribute(L"y").as_int();
	return pt;
}
