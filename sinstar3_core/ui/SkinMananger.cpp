#include "StdAfx.h"
#include "SkinMananger.h"
#include <helper/SMenu.h>
#include <helper/mybuffer.h>
#include "../SouiEnv.h"

CSkinMananger::CSkinMananger(void)
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
						hMenu->GetMenuItem(nStartId)->SetAttribute(L"check",L"1");
					}
				}
			}
		} while (FindNextFile(hFind, &findData));
		FindClose(hFind);
	}
	return nStartId;
}

BOOL CSkinMananger::SetSkin(int nSkinId)
{
	SStringT strSkinPath;
	if(SMap<int,SStringT>::CPair * p =m_mapSkin.Lookup(nSkinId))
	{
		strSkinPath = p->m_value;
	}
	if(strSkinPath == CDataCenter::getSingletonPtr()->GetData().m_strSkin)
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

		if(!CDataCenter::getSingletonPtr()->GetData().m_strSkin.IsEmpty())
		{//清除正在使用的外置皮肤。
			IResProvider *pLastRes = SApplication::getSingleton().GetTailResProvider();
			SApplication::getSingleton().RemoveResProvider(pLastRes);
			IUiDefInfo *pUiDefInfo = SUiDef::getSingleton().GetUiDef();

			SStylePoolMgr::getSingleton().PopStylePool(pUiDefInfo->GetStylePool());
		}

		SApplication::getSingleton().AddResProvider(pResProvider,NULL);
		SUiDef::getSingleton().SetUiDef(pUiDef);
		pUiDef->Release();

	}else if(!CDataCenter::getSingletonPtr()->GetData().m_strSkin.IsEmpty())
	{//清除正在使用的外置皮肤。
		IResProvider *pLastRes = SApplication::getSingleton().GetTailResProvider();
		SApplication::getSingleton().RemoveResProvider(pLastRes);
		IUiDefInfo *pUiDefInfo = SUiDef::getSingleton().GetUiDef();

		SStylePoolMgr::getSingleton().PopStylePool(pUiDefInfo->GetStylePool());
	}

	CDataCenter::getSingletonPtr()->GetData().m_strSkin = strSkinPath;

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
