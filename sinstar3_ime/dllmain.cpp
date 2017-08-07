// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "uiwnd.h"


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
			LONG ret = reg.Open(HKEY_CURRENT_USER,_T("SOFTWARE\\SetoutSoft\\sinstar3"),KEY_READ);
			if(ret == ERROR_SUCCESS)
			{
				ULONG len = MAX_PATH;
				reg.QueryStringValue(_T("path_client"),szPath,&len);
				reg.Close();
			}
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

STDAPI DllRegisterServer(void)
{
	TCHAR szPath[MAX_PATH];
	GetModuleFileName(g_hInst,szPath,MAX_PATH);
	TCHAR szSysDir[MAX_PATH];
	int nLen=GetSystemDirectory(szSysDir,MAX_PATH);
	//确保当前IME位于系统目录
	if(_tcsnicmp(szSysDir,szPath,nLen)!=0) return E_FAIL;
	// register this service's profile with the tsf
	if (!ImmInstallIME(szPath, PRODUCT_NAME))
	{
		DWORD dwErr=GetLastError();
		return E_FAIL;
	}

	return S_OK;
}
