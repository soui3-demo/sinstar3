#include "stdafx.h"
#include "sinstar3_tsf.h"

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID pvReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
#ifdef _DEBUG
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF);
#endif
		g_hInst=hInstance;
		{
			if (!InitializeCriticalSectionAndSpinCount(&g_cs, 0))
				return FALSE;

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
			_tcscat(szPath,_T("\\x64"));
#endif
			CBaiduIMEModule::GetInstance().SetBaiduJP3Path(szPath);			

			CMsgListnerWnd::RegisterWnd(g_hInst);
		}
		break;

	case DLL_PROCESS_DETACH:
		{
			Helper_Trace(L"DLL_PROCESS_DETACH");
			CMsgListnerWnd::UnregisterWnd(g_hInst);

			DeleteCriticalSection(&g_cs);
			
		}
		break;

	case DLL_THREAD_ATTACH:
		{
			Helper_Trace(L"DLL_THREAD_ATTACH");
		}
		break;

	case DLL_THREAD_DETACH:
		{
			Helper_Trace(L"DLL_THREAD_DETACH");
		}
		break;
	}

    return TRUE;
}
