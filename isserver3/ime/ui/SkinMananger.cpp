#include "StdAfx.h"
#include "SkinMananger.h"
#include <helper/SMenu.h>
#include <helper/SAutoBuf.h>

CSkinMananger::CSkinMananger():m_nMaxCtxID(R.id.menu_skin)
{
}

CSkinMananger::~CSkinMananger(void)
{
}

int CSkinMananger::InitSkinMenu(HMENU hMenu, const SStringT &strSkinPath, int nStartID, const SStringT &strCurSkin)
{
	WIN32_FIND_DATA findData;
	HANDLE hFind = FindFirstFile(strSkinPath + _T("\\*.*"), &findData);
	SMenu smenu(hMenu);
	int nID = nStartID;
	//enum sub folder
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do {
			if (findData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				if (_tcscmp(findData.cFileName, _T(".")) != 0 && _tcscmp(findData.cFileName, _T("..")) != 0)
				{					
					HMENU hSubMenu = CreatePopupMenu();
					if (smenu.AppendMenu(MF_STRING|MF_POPUP ,(UINT_PTR)hSubMenu , findData.cFileName))
					{
						m_nMaxCtxID += 1000;
						SetMenuContextHelpId(hSubMenu,m_nMaxCtxID);
						SStringT strSubSkinDir = strSkinPath + _T("\\") + findData.cFileName;
						m_mapCtxId2Path[m_nMaxCtxID] = strSubSkinDir;
					}else
					{
						DestroyMenu(hSubMenu);
					}
				}
			}
		} while (FindNextFile(hFind, &findData));
		FindClose(hFind);
	}

	//enum skins.
	hFind = FindFirstFile(strSkinPath + _T("\\*.sskn"), &findData);
	if (hFind != INVALID_HANDLE_VALUE)
	{		
		do {

			nID++;
			SStringT strFullPath = strSkinPath + _T("\\") + findData.cFileName;
			m_mapSkin[nID] = strFullPath;
			//SLOG_INFO("skin "<<strFullPath<<" id is "<<nID);
			SStringT strDesc;
			if(ExtractSkinInfo(strFullPath,strDesc))
			{
				BOOL bInsertSucess= smenu.AppendMenu( MF_STRING , nID, strDesc);

				if (bInsertSucess&&(strFullPath == strCurSkin))
				{
					CheckMenuItem(hMenu,nID, MF_BYCOMMAND | MF_CHECKED);
				}
			}
		} while (FindNextFile(hFind, &findData));
		FindClose(hFind);
	}
	return nID;
}

static const int KMinVer[4]={0,0,0,0};
static const int KMaxVer[4]={3,0,0,0};

bool CSkinMananger::ExtractSkinInfo(SStringT strSkinPath,SStringW &strDesc)
{
	IResProvider *pResProvider = NULL;
	g_ComMgr2->CreateResProvider_ZIP((IObjRef**)&pResProvider);
	ZIPRES_PARAM param;
	param.ZipFile(NULL, strSkinPath);
	pResProvider->Init((WPARAM)&param,0);
	int nSize = (int)pResProvider->GetRawBufferSize(_T("uidef"),_T("xml_init"));
	SAutoBuf buffer(nSize);
	pResProvider->GetRawBuffer(_T("uidef"),_T("xml_init"),buffer,nSize);
	pResProvider->Release();

	pugi::xml_document xmlDoc;
	xmlDoc.load_buffer_inplace(buffer,nSize);

	pugi::xml_node xmlDesc = xmlDoc.child(L"uidef").child(L"desc");
	SStringW strVer = xmlDesc.attribute(L"version").as_string(L"1.0");
	SStringWList lstVer;
	SplitString(strVer,L'.',lstVer);
	int nVer[4]={0};
	bool bValid = true;
	for(size_t i =0;i<lstVer.GetCount() && i<4;i++)
	{
		nVer[i]=_wtoi(lstVer[i]);
		if(nVer[i]<KMinVer[i])
		{
			bValid=false;
			break;
		}
		if(nVer[i]<KMaxVer[i])
			break;
		if(nVer[i]>KMaxVer[i])
		{
			bValid=false;
			break;
		}
	}
	if(!bValid)
		return false;

	strDesc = S_CW2T(SStringW(xmlDesc.attribute(L"name").value())+L":"+xmlDesc.attribute(L"version").value()+L"["+xmlDesc.attribute(L"author").value()+L"]");
	return true;
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


SOUI::SStringT CSkinMananger::SkinPathFromCtxID(int nCtxID) const
{
	SStringT strSkinPath;
	if (const SMap<int, SStringT>::CPair * p = m_mapCtxId2Path.Lookup(nCtxID))
	{
		strSkinPath = p->m_value;
	}
	return strSkinPath;
}

CPoint CSkinMananger::ExtractSkinOffset(IResProvider * pResProvider)
{
	int nSize = (int)pResProvider->GetRawBufferSize(_T("uidef"), _T("xml_init"));

	SAutoBuf buffer(nSize);
	pResProvider->GetRawBuffer(_T("uidef"), _T("xml_init"), buffer, nSize);

	pugi::xml_document xmlDoc;
	xmlDoc.load_buffer_inplace(buffer, nSize);
	pugi::xml_node xmlOffset = xmlDoc.child(L"uidef").child(L"offset");
	CPoint pt;
	pt.x = xmlOffset.attribute(L"x").as_int();
	pt.y = xmlOffset.attribute(L"y").as_int();
	return pt;
}

void CSkinMananger::ClearMap()
{
	m_mapSkin.RemoveAll();
	m_mapCtxId2Path.RemoveAll();
	m_nMaxCtxID = R.id.menu_skin;
}
