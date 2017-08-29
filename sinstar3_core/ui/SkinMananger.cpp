#include "StdAfx.h"
#include "SkinMananger.h"
#include <helper/SMenu.h>
#include <helper/mybuffer.h>
#include "../SouiEnv.h"

CSkinMananger::CSkinMananger(void)
{
	SNotifyCenter::getSingletonPtr()->addEvent(EVENTID(EventSetSkin));
}

CSkinMananger::~CSkinMananger(void)
{
	SNotifyCenter::getSingletonPtr()->removeEvent(EventSetSkin::EventID);
}

int CSkinMananger::InitSkinMenu(HMENU hMenu, const SStringT &strSkinPath,int nStartId,const SStringT &strCurSkin)
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
					nStartId=InitSkinMenu(hSubMenu,strSkinPath+_T("\\")+findData.cFileName,nStartId,strCurSkin);
				}
			}else
			{
				TCHAR szExt[100];
				_tsplitpath(findData.cFileName,NULL,NULL,NULL,szExt);
				if(_tcsicmp(szExt,_T(".sskn"))==0)
				{
					nStartId++;
					SStringT strFullPath = strSkinPath+_T("\\")+findData.cFileName;
					m_mapSkin[nStartId] = strFullPath;
					SStringT strDesc = ExtractSkinInfo(strFullPath);
					menu.AppendMenu(0,nStartId,strDesc,0);
					if(strFullPath == strCurSkin)
					{
						CheckMenuItem(menu.m_hMenu,nStartId,MF_CHECKED|MF_BYCOMMAND);
					}
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
	SStringT strSkinPath;
	if(SMap<int,SStringT>::CPair * p =m_mapSkin.Lookup(nSkinId))
	{
		strSkinPath = p->m_value;
	}
	if(strSkinPath == CDataCenter::GetAutoLockerInstance()->GetData().m_strSkin)
	{
		return TRUE;
	}

	if(!strSkinPath.IsEmpty())
	{
		CAutoRefPtr<IResProvider> pResProvider;
		CSouiEnv::getSingleton().theComMgr()->CreateResProvider_ZIP((IObjRef**)&pResProvider);
		ZIPRES_PARAM param;
		param.ZipFile(GETRENDERFACTORY, strSkinPath);
		if(!pResProvider->Init((WPARAM)&param,0))
			return FALSE;

		IUiDefInfo * pUiDef = SUiDef::getSingleton().CreateUiDefInfo(pResProvider,_T("uidef:xml_init"));
		if(pUiDef->GetSkinPool())
		{//不允许皮肤中存在全局的skin数据
			pUiDef->Release();
			return FALSE;
		}

		if(!CDataCenter::GetAutoLockerInstance()->GetData().m_strSkin.IsEmpty())
		{//清除正在使用的外置皮肤。
			IResProvider *pLastRes = SApplication::getSingleton().GetTailResProvider();
			SApplication::getSingleton().RemoveResProvider(pLastRes);
			IUiDefInfo *pUiDefInfo = SUiDef::getSingleton().GetUiDef();

			SStylePoolMgr::getSingleton().PopStylePool(pUiDefInfo->GetStylePool());
		}

		SApplication::getSingleton().AddResProvider(pResProvider,NULL);
		SUiDef::getSingleton().SetUiDef(pUiDef);
		pUiDef->Release();

	}else if(!CDataCenter::GetAutoLockerInstance()->GetData().m_strSkin.IsEmpty())
	{//清除正在使用的外置皮肤。
		IResProvider *pLastRes = SApplication::getSingleton().GetTailResProvider();
		SApplication::getSingleton().RemoveResProvider(pLastRes);
		IUiDefInfo *pUiDefInfo = SUiDef::getSingleton().GetUiDef();

		SStylePoolMgr::getSingleton().PopStylePool(pUiDefInfo->GetStylePool());
	}

	CDataCenter::GetAutoLockerInstance()->GetData().m_strSkin = strSkinPath;

	//notify skin changed
	EventSetSkin *pEvt = new EventSetSkin(this);
	SNotifyCenter::getSingletonPtr()->FireEventAsync(pEvt);
	pEvt->Release();
	return TRUE;
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
