// Windows Í·ÎÄ¼þ:
#include <windows.h>
#include <tchar.h>

#include "core_loader.h"
#include <crtdbg.h>
#include "../include/autolock.h"

CCoreLoader::CCoreLoader(void)
{
	m_szPath[0]=0;
	InitializeCriticalSection(&m_cs);
}

CCoreLoader::~CCoreLoader(void)
{
	DeleteCriticalSection(&m_cs);
}

void CCoreLoader::SetBaiduJP3Path(LPCTSTR pszPath)
{
	_tcscpy(m_szPath,pszPath);
	_tcscat(m_szPath,_T("\\sinstar3_core.dll"));
}

ISinstar *CCoreLoader::Sinstar3_Create(ITextService *pTxtSvr,HINSTANCE hInst)
{
	CAutoLock lock(&m_cs);
	BOOL bLoad=FALSE;
	HMODULE hBaiduJP3=GetModuleHandle(m_szPath);
	if(!hBaiduJP3)
	{
		hBaiduJP3=LoadLibrary(m_szPath);
		bLoad=TRUE;
	}
	if(!hBaiduJP3) 
	{
		return NULL;
	}
	FUN_Sinstar3_Create funCreate=(FUN_Sinstar3_Create)GetProcAddress(hBaiduJP3,"Sinstar3_Create");
	FUN_Sinstar3_Delete funDelete=(FUN_Sinstar3_Delete)GetProcAddress(hBaiduJP3,"Sinstar3_Delete");
	FUN_Sinstar3_CanUnloadNow funCanUnloadNow=(FUN_Sinstar3_CanUnloadNow)GetProcAddress(hBaiduJP3,"Sinstar3_CanUnloadNow");
	if(!funCreate || !funDelete || !funCanUnloadNow)
	{
		if(bLoad) FreeLibrary(hBaiduJP3);
		return NULL;
	}
	ISinstar *pRet = funCreate(pTxtSvr,hBaiduJP3);
	return pRet;
}

BOOL CCoreLoader::Sinstar3_Delete(ISinstar * pBaiduJP3)
{
	CAutoLock lock(&m_cs);

	HMODULE hBaiduJP3=pBaiduJP3->GetModule();
	FUN_Sinstar3_Delete funDelete=(FUN_Sinstar3_Delete)GetProcAddress(hBaiduJP3,"Sinstar3_Delete");
	FUN_Sinstar3_CanUnloadNow funCanUnloadNow=(FUN_Sinstar3_CanUnloadNow)GetProcAddress(hBaiduJP3,"Sinstar3_CanUnloadNow");
	_ASSERT(funDelete && funCanUnloadNow);
	funDelete(pBaiduJP3);
	if(funCanUnloadNow()==S_OK)
	{
		FreeLibrary(hBaiduJP3);
	}
	return TRUE;
}

BOOL CCoreLoader::BaiduJP3_OpenConfig(HWND hParent)
{
	BOOL bNeedFree=FALSE;
	HMODULE	hBaiduJP3=LoadLibrary(m_szPath);
	if(!hBaiduJP3) return FALSE;
	BOOL bRet=FALSE;
	FUN_Sinstar3_Config funConfig=(FUN_Sinstar3_Config)GetProcAddress(hBaiduJP3,"Sinstar3_Config");
	if(funConfig) bRet=funConfig(hParent);
	FreeLibrary(hBaiduJP3);
	return bRet;
}

CCoreLoader & CCoreLoader::GetInstance()
{
	static CCoreLoader thiz;
	return thiz;
}
