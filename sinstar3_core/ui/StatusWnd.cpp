#include "StdAfx.h"
#include "StatusWnd.h"
#include <helper/SMenu.h>
#include <resprovider-zip/zipresprovider-param.h>

#define CMD_MENU_FIRST	 220
namespace SOUI
{
	CStatusWnd::CStatusWnd(void):CImeWnd(UIRES.LAYOUT.wnd_status_bar)
	{
	}

	CStatusWnd::~CStatusWnd(void)
	{
	}

	int CStatusWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		int nRet = __super::OnCreate(lpCreateStruct);
		if(nRet != 0) return nRet;

		CRect rcWorkArea;
		SystemParametersInfo(SPI_GETWORKAREA,0,&rcWorkArea,0);
		SetWindowPos(HWND_TOPMOST,rcWorkArea.right-GetWindowRect().Width(),rcWorkArea.bottom-GetWindowRect().Height(),0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
		return 0;
	}

	void CStatusWnd::OnRButtonUp(UINT nFlags,CPoint pt)
	{
		SMenu menu;
		menu.LoadMenu(UIRES.smenu.context);
		ClientToScreen(&pt);
		int nRet = menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_BOTTOMALIGN|TPM_RETURNCMD,pt.x,pt.y,m_hWnd);
		if(nRet>=CMD_MENU_FIRST && nRet <=CMD_MENU_FIRST+1000)
		{//select menu
			if(SMap<int,SStringT>::CPair * p =m_mapSkin.Lookup(nRet))
			{
				SStringT skinPath = p->m_value;
				SComMgr comMgr;
				IResProvider *pResProvider=NULL;
				comMgr.CreateResProvider_ZIP((IObjRef**)&pResProvider);
				ZIPRES_PARAM param;
				param.ZipFile(GETRENDERFACTORY, skinPath);
				pResProvider->Init((WPARAM)&param,0);
				
				//SApplication::getSingleton().AddResProvider(pResProvider);

				//on skin changed

				pResProvider->Release();
			}
		}
	}

	void CStatusWnd::OnInitMenuPopup(HMENU menuPopup, UINT nIndex, BOOL bSysMenu)
	{
		if(GetMenuContextHelpId(menuPopup)==2)
		{
			m_mapSkin.RemoveAll();
			InitSkinMenu(menuPopup,theModule->GetDataPath()+_T("\\skins"),CMD_MENU_FIRST);
		}
	}

	int CStatusWnd::InitSkinMenu(HMENU hMenu, const SStringT &strSkinPath,int nStartId)
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
					TCHAR szName[200],szExt[100];
					_tsplitpath(findData.cFileName,NULL,NULL,szName,szExt);
					if(_tcsicmp(szExt,_T(".sskn"))==0)
					{
						m_mapSkin[nStartId] = strSkinPath+_T("\\")+findData.cFileName;
						menu.AppendMenu(0,nStartId++,szName,0);
					}
				}
			}while(FindNextFile(hFind,&findData));
			FindClose(hFind);
		}
		menu.Detach();

		return nStartId;
	}

}
