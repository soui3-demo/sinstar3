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
//			CImeUtil::Comm_GetInstallPath(szPath,MAX_PATH);

#if defined(_WIN64)
			//PathAppend( szPath, _T("x64"));
			_tcscat_s( szPath, _T("\\x64"));			
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
	//return CIMEMan::RemoveIME(p+1)?S_OK:E_FAIL;
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
		//DllUnregisterServer(); // 不能删除，原来正在使用的时候可能安装失败。
		return E_FAIL;
	}

	return S_OK;
}
