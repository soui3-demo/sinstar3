// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <ShlObj.h>
#include "ImeModule.h"
#include "ImeMgr.h"
#include "../sinstar3_proxy/SinstarProxy.h"

#undef ENABLE_VLD
#ifdef ENABLE_VLD
#include <vld.h>
#endif //ENABLE_VLD

CImeModule * theModule=NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
#ifdef _DEBUG
			_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF);
#endif
			TCHAR szPathSvr[MAX_PATH] = { 0 },szPathClient[MAX_PATH]={0};
			CRegKey reg;
			LONG ret = reg.Open(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\SetoutSoft\\sinstar3"),KEY_READ|KEY_WOW64_64KEY);
			if(ret == ERROR_SUCCESS)
			{
				ULONG len = MAX_PATH;
				reg.QueryStringValue(_T("path_svr"),szPathSvr,&len);
				len = MAX_PATH;
				reg.QueryStringValue(_T("path_client"),szPathClient,&len);
				reg.Close();
			}

			//check for black list
			TCHAR szBlacklist[MAX_PATH];
			_stprintf(szBlacklist,_T("%s\\data\\blacklist.txt"),szPathClient);
			if(CSinstarProxy::isInBlackList(szBlacklist))
				return FALSE;

			theModule = new CImeModule(hModule,szPathSvr,szPathClient);
		}
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		delete theModule;
		break;
	}
	return TRUE;
}

STDAPI DllUnregisterServer(void)
{
	TCHAR szPath[MAX_PATH];
	GetModuleFileName(theModule->GetModule(),szPath,MAX_PATH);
	TCHAR *p=_tcsrchr(szPath,_T('\\'));
	
	return ImeMgr::RemoveIME(p+1)?S_OK:E_UNEXPECTED;
}

void GetSystemDir(LPTSTR szPath)
{
#ifdef _WIN64
	SHGetSpecialFolderPath(NULL,szPath,CSIDL_SYSTEM,FALSE);
#else
	SHGetSpecialFolderPath(NULL,szPath,CSIDL_SYSTEMX86,FALSE);
#endif
}

STDAPI DllRegisterServer(void)
{
	TCHAR szPath[MAX_PATH];
	GetModuleFileName(theModule->GetModule(),szPath,MAX_PATH);
	TCHAR szSysDir[MAX_PATH];
	GetSystemDir(szSysDir);
	// register this service's profile with the tsf
	if (!ImmInstallIME(szPath, PRODUCT_TNAMEVER))
	{
		DWORD dwErr=GetLastError();
		TCHAR szErr[100];
		_stprintf(szErr,_T("ImmInstallIME Failed! error code=0x%08x"),dwErr);
		MessageBox(NULL,szErr,_T("failed"),MB_OK);
		return E_FAIL;
	}

	return S_OK;
}
