// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "uiwnd.h"
#include <ShlObj.h>
#include "ImeModule.h"
#include "ImeMgr.h"

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
#ifdef _DEBUG
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF);
#endif
		{
			TCHAR szPath[MAX_PATH] = { 0 };
			CRegKey reg;
			LONG ret = reg.Open(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\SetoutSoft\\sinstar3"),KEY_READ|KEY_WOW64_64KEY);
			if(ret == ERROR_SUCCESS)
			{
				ULONG len = MAX_PATH;
				reg.QueryStringValue(_T("path_client"),szPath,&len);
				reg.Close();
			}
			theModule = new CImeModule(hModule,szPath);
			CCoreLoader::GetInstance().SetCorePath(szPath);			
		}
		CUiWnd::RegisterClass(hModule);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		CUiWnd::UnregisterClass(hModule);
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
