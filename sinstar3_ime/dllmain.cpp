// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "uiwnd.h"
#include <ShlObj.h>

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
		g_hInst=hModule;
		{
			TCHAR szPath[MAX_PATH];
			CRegKey reg;
			LONG ret = reg.Open(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\SetoutSoft\\sinstar3"),KEY_READ|KEY_WOW64_64KEY);
			if(ret == ERROR_SUCCESS)
			{
				ULONG len = MAX_PATH;
				reg.QueryStringValue(_T("path_client"),szPath,&len);
				reg.Close();
			}
#ifdef _WIN64
			_tcscat(szPath,_T("\x64"));
#endif
			CBaiduIMEModule::GetInstance().SetBaiduJP3Path(szPath);			
		}
		CUiWnd::RegisterClass(hModule);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		CUiWnd::UnregisterClass(hModule);
		break;
	}
	return TRUE;
}

STDAPI DllUnregisterServer(void)
{
	TCHAR szPath[MAX_PATH];
	GetModuleFileName(g_hInst,szPath,MAX_PATH);
	TCHAR *p=_tcsrchr(szPath,_T('\\'));
	
	return FALSE;
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
	GetModuleFileName(g_hInst,szPath,MAX_PATH);
	TCHAR szSysDir[MAX_PATH];
	GetSystemDir(szSysDir);
	//确保当前IME位于系统目录
	int nLen = _tcslen(szSysDir);
	if(_tcsnicmp(szSysDir,szPath,nLen)!=0) 
	{
		return E_FAIL;
	}
	// register this service's profile with the tsf
	if (!ImmInstallIME(szPath, PRODUCT_NAME))
	{
		DWORD dwErr=GetLastError();
		TCHAR szErr[100];
		_stprintf(szErr,_T("ImmInstallIME Failed! error code=0x%08x"),dwErr);
		MessageBox(NULL,szErr,_T("failed"),MB_OK);
		return E_FAIL;
	}

	return S_OK;
}
