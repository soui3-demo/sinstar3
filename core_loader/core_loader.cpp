// Windows Í·ÎÄ¼þ:
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif//_CRT_SECURE_NO_WARNINGS

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

void CCoreLoader::SetCorePath(LPCTSTR pszPath)
{
	_tcscpy(m_szPath,pszPath);
	_tcscat(m_szPath,_T("\\sinstar3_core.dll"));
}

ISinstar *CCoreLoader::Sinstar3_Create(ITextService *pTxtSvr)
{
	CAutoLock lock(&m_cs);
	BOOL bLoad=FALSE;
	HMODULE hCoreModule=GetModuleHandle(m_szPath);
	if(!hCoreModule)
	{
		hCoreModule=LoadLibrary(m_szPath);
		bLoad=TRUE;
	}
	if(!hCoreModule) 
	{
		DWORD dwErr=GetLastError();
		return NULL;
	}
	FUN_Sinstar3_Create funCreate=(FUN_Sinstar3_Create)GetProcAddress(hCoreModule,"Sinstar3_Create");
	FUN_Sinstar3_Delete funDelete=(FUN_Sinstar3_Delete)GetProcAddress(hCoreModule,"Sinstar3_Delete");
	FUN_Sinstar3_CanUnloadNow funCanUnloadNow=(FUN_Sinstar3_CanUnloadNow)GetProcAddress(hCoreModule,"Sinstar3_CanUnloadNow");
	if(!funCreate || !funDelete || !funCanUnloadNow)
	{
		if(bLoad) FreeLibrary(hCoreModule);
		return NULL;
	}
	ISinstar *pRet = funCreate(pTxtSvr);
	return pRet;
}

BOOL CCoreLoader::Sinstar3_Delete(ISinstar * pSinstar3)
{
	CAutoLock lock(&m_cs);
	HMODULE hCoreModule=GetModuleHandle(m_szPath);
	if(!hCoreModule) return FALSE;
	FUN_Sinstar3_Delete funDelete=(FUN_Sinstar3_Delete)GetProcAddress(hCoreModule,"Sinstar3_Delete");
	FUN_Sinstar3_CanUnloadNow funCanUnloadNow=(FUN_Sinstar3_CanUnloadNow)GetProcAddress(hCoreModule,"Sinstar3_CanUnloadNow");
	_ASSERT(funDelete && funCanUnloadNow);
	funDelete(pSinstar3);
	if(funCanUnloadNow()==S_OK)
	{
		FreeLibrary(hCoreModule);
	}
	return TRUE;
}

BOOL CCoreLoader::Sinstar3_OpenConfig(HWND hParent)
{
	BOOL bNeedFree=FALSE;
	HMODULE	hCoreModule=LoadLibrary(m_szPath);
	if(!hCoreModule) return FALSE;
	BOOL bRet=FALSE;
	FUN_Sinstar3_Config funConfig=(FUN_Sinstar3_Config)GetProcAddress(hCoreModule,"Sinstar3_Config");
	if(funConfig) bRet=funConfig(hParent);
	FreeLibrary(hCoreModule);
	return bRet;
}


void CCoreLoader::Sinstar3_SetHostInfo(HostInfo *pHostInfo)
{
	CAutoLock lock(&m_cs);

	HMODULE	hCoreModule=LoadLibrary(m_szPath);
	if(!hCoreModule) return;
	BOOL bRet=FALSE;
	FUN_Sinstar3_SetHostInfo funSetHostInfo=(FUN_Sinstar3_SetHostInfo)GetProcAddress(hCoreModule,"Sinstar3_SetHostInfo");
	if(funSetHostInfo) funSetHostInfo(pHostInfo);
	FreeLibrary(hCoreModule);

}

CCoreLoader & CCoreLoader::GetInstance()
{
	static CCoreLoader thiz;
	return thiz;
}
