#include "stdafx.h"
#include "sinstar3_tsf.h"
#include "../sinstar3_proxy/SinstarProxy.h"

CTsfModule	*theModule = NULL;

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID pvReserved)
{
	switch (dwReason)
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
			theModule = new CTsfModule(hInstance,szPathSvr,szPathClient);
		}
		break;

	case DLL_PROCESS_DETACH:
		{
			delete theModule;
			theModule = NULL;
		}
		break;

	case DLL_THREAD_ATTACH:
		{
			SLOG_INFO(L"DLL_THREAD_ATTACH");
		}
		break;

	case DLL_THREAD_DETACH:
		{
			SLOG_INFO(L"DLL_THREAD_DETACH");
		}
		break;
	}

    return TRUE;
}
