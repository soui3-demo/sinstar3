#include "StdAfx.h"
#include "SkinMananger.h"
#include <helper/SMenu.h>
#include <helper/mybuffer.h>
#include <resprovider-zip/zipresprovider-param.h>
#include "../SouiEnv.h"

CSkinMananger::CSkinMananger(void)
{
	SNotifyCenter::getSingletonPtr()->addEvent(EVENTID(EventSetSkin));
}

CSkinMananger::~CSkinMananger(void)
{
}

int CSkinMananger::InitSkinMenu(HMENU hMenu, const SStringT &strSkinPath,int nStartId)
{
	SMenu menu(hMenu);

	WIN32_FIND_DATA findData;
	HANDLE hFind = FindFirstFile(strSkinPath+_T("\\*.*"),&findData);
	if(hFind != INVALID_HANDLE_VALUE)
	{
		do{
			if(findData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				if(_tcscmp(findData.cFileName,_T("."))!=0 && _tcscmp(findData.cFileName,_T(".."))!=0)
				{
					HMENU hSubMenu = CreatePopupMenu();
					menu.AppendMenu(MF_POPUP,(UINT_PTR)hSubMenu,findData.cFileName,0);
					nStartId=InitSkinMenu(hSubMenu,strSkinPath+_T("\\")+findData.cFileName,nStartId);
				}
			}else
			{
				TCHAR szExt[100];
				_tsplitpath(findData.cFileName,NULL,NULL,NULL,szExt);
				if(_tcsicmp(szExt,_T(".sskn"))==0)
				{
					SStringT strFullPath = strSkinPath+_T("\\")+findData.cFileName;
					m_mapSkin[nStartId] = strFullPath;
					SStringT strDesc = ExtractSkinInfo(strFullPath);
					menu.AppendMenu(0,nStartId++,strDesc,0);
				}
			}
		}while(FindNextFile(hFind,&findData));
		FindClose(hFind);
	}
	menu.Detach();

	return nStartId;
}


BOOL CSkinMananger::SetSkin(int nSkinId)
{
	if(SMap<int,SStringT>::CPair * p =m_mapSkin.Lookup(nSkinId))
	{
		IResProvider *pResProvider=NULL;
		CSouiEnv::getSingleton().theComMgr()->CreateResProvider_ZIP((IObjRef**)&pResProvider);
		ZIPRES_PARAM param;
		param.ZipFile(GETRENDERFACTORY, p->m_value);
		pResProvider->Init((WPARAM)&param,0);

		SApplication::getSingleton().AddResProvider(pResProvider);

		//on skin changed
		EventSetSkin *pEvt = new EventSetSkin(this);
		SNotifyCenter::getSingletonPtr()->FireEventAsync(pEvt);

		pResProvider->Release();

		return TRUE;
	}else
	{
		return FALSE;
	}
}

SOUI::SStringT CSkinMananger::ExtractSkinInfo(SStringT strSkinPath)
{
	IResProvider *pResProvider=NULL;
	CSouiEnv::getSingleton().theComMgr()->CreateResProvider_ZIP((IObjRef**)&pResProvider);
	ZIPRES_PARAM param;
	param.ZipFile(NULL, strSkinPath);
	pResProvider->Init((WPARAM)&param,0);
	int nSize = pResProvider->GetRawBufferSize(_T("uidef"),_T("xml_init"));
	CMyBuffer<char> buffer(nSize);
	pResProvider->GetRawBuffer(_T("uidef"),_T("xml_init"),buffer,nSize);
	pResProvider->Release();

	pugi::xml_document xmlDoc;
	xmlDoc.load_buffer_inplace(buffer,nSize);
	pugi::xml_node xmlDesc = xmlDoc.child(L"uidef").child(L"desc");
	SStringW strDesc = SStringW(xmlDesc.attribute(L"name").value())+L"["+xmlDesc.attribute(L"author").value()+L"]";
	return S_CW2T(strDesc);
}
