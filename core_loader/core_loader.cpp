// Windows Í·ÎÄ¼þ:
#include <windows.h>
#include <tchar.h>

#include "core_loader.h"
#include <crtdbg.h>


CBaiduIMEModule::CBaiduIMEModule(void)
{
	m_szPath[0]=0;
}

CBaiduIMEModule::~CBaiduIMEModule(void)
{
}

void CBaiduIMEModule::SetBaiduJP3Path(LPCTSTR pszPath)
{
	_tcscpy(m_szPath,pszPath);
	_tcscat(m_szPath,_T("\\sinstar3_core.dll"));
}

ISinstar *CBaiduIMEModule::BaiduJP3_Create(CTextService *pTxtSvr,HINSTANCE hInst)
{
	BOOL bLoad=FALSE;
	HMODULE hBaiduJP3=GetModuleHandle(m_szPath);
	if(!hBaiduJP3)
	{
		hBaiduJP3=LoadLibrary(m_szPath);
		bLoad=TRUE;
	}
	if(!hBaiduJP3) return NULL;
	FUN_BaiduJP3_Create funCreate=(FUN_BaiduJP3_Create)GetProcAddress(hBaiduJP3,"BaiduJP3_Create");
	FUN_BaiduJP3_Delete funDelete=(FUN_BaiduJP3_Delete)GetProcAddress(hBaiduJP3,"BaiduJP3_Delete");
	FUN_BaiduJP3_CanUnloadNow funCanUnloadNow=(FUN_BaiduJP3_CanUnloadNow)GetProcAddress(hBaiduJP3,"BaiduJP3_CanUnloadNow");
	if(!funCreate || !funDelete || !funCanUnloadNow)
	{
		if(bLoad) FreeLibrary(hBaiduJP3);
		return NULL;
	}
	ISinstar *pRet = funCreate(pTxtSvr,hInst);
	return pRet;
}

BOOL CBaiduIMEModule::BaiduJP3_Delete(ISinstar * pBaiduJP3)
{
	HMODULE hBaiduJP3=pBaiduJP3->GetModule();
	FUN_BaiduJP3_Delete funDelete=(FUN_BaiduJP3_Delete)GetProcAddress(hBaiduJP3,"BaiduJP3_Delete");
	FUN_BaiduJP3_CanUnloadNow funCanUnloadNow=(FUN_BaiduJP3_CanUnloadNow)GetProcAddress(hBaiduJP3,"BaiduJP3_CanUnloadNow");
	_ASSERT(funDelete && funCanUnloadNow);
	funDelete(pBaiduJP3);
	if(funCanUnloadNow()==S_OK)
	{
		FreeLibrary(hBaiduJP3);
	}
	return TRUE;
}

BOOL CBaiduIMEModule::BaiduJP3_OpenConfig(HWND hParent)
{
	BOOL bNeedFree=FALSE;
	HMODULE	hBaiduJP3=LoadLibrary(m_szPath);
	if(!hBaiduJP3) return FALSE;
	BOOL bRet=FALSE;
	FUN_BaiduJP3_Config funConfig=(FUN_BaiduJP3_Config)GetProcAddress(hBaiduJP3,"BaiduJP3_Config");
	if(funConfig) bRet=funConfig(hParent);
	FreeLibrary(hBaiduJP3);
	return bRet;
}

CBaiduIMEModule & CBaiduIMEModule::GetInstance()
{
	static CBaiduIMEModule thiz;
	return thiz;
}
