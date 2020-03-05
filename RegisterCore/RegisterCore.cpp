// RegisterCore.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "RegisterCore.h"
#pragma comment(lib,"Version.lib")

TCHAR g_szPath[MAX_PATH] = { 0 };	//程序启动位置

const TCHAR * KFiles[] =
{
	_T("program\\isserver3.exe"),
	_T("program\\sinstar3_ime.dll"),
	_T("program\\x64\\sinstar3_ime.dll"),
	_T("program\\sinstar3_tsf.dll"),
	_T("program\\x64\\sinstar3_tsf.dll"),
};


struct CopyInfo {
	const TCHAR *pszSrc;
	const TCHAR *pszDest;
	bool   bReg;
};

const CopyInfo KSrcX86Files[] =
{
	{ _T("program\\sinstar3_ime.dll"),_T("sinstar3_ime.ime"),false },
	{ _T("program\\sinstar3_tsf.dll"),_T("sinstar3_tsf.dll"),true },
};

const CopyInfo KSrcX64Files[] =
{
	{ _T("program\\x64\\sinstar3_ime.dll"),_T("sinstar3_ime.ime"),true },
	{ _T("program\\x64\\sinstar3_tsf.dll"),_T("sinstar3_tsf.dll"),true },
};

struct UpdateInfo
{
	LPCTSTR pszFile;
	bool	bClean;	//true- delete at first.
};

const UpdateInfo KUpdateInfo[] =
{
	{_T("program"),true},
	{_T("defskin"),true },
	{ _T("sound"),true },
	{_T("skins"),false},
	{ _T("tools"),false },	
	{_T("register.exe"),false },
	{_T("RegisterCore.dll"),false },
	{_T("license.rtf"),false },
	{_T("使用说明.txt"),false },
};

BOOL Is64OS()
{
	SYSTEM_INFO si;
	GetNativeSystemInfo(&si);

	if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
		si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

static TCHAR s_LastMsg[500]=_T("ok");//error msg;

void Sinstar_SetErrMsg(LPCTSTR szMsg)
{
	_tcscpy_s(s_LastMsg,500*sizeof(TCHAR),szMsg);
}

LPCWSTR RC_API  Sinstar_GetErrMsgW(){
	return s_LastMsg;
}

LPCSTR RC_API  Sinstar_GetErrMsgA(){
	static char szErrMsg[1000];
	WideCharToMultiByte(CP_ACP,0,s_LastMsg,-1,szErrMsg,1000,NULL,NULL);
	return szErrMsg;
}

void RC_API  Sinstar_InitW(LPCWSTR pszPath)
{
	wcscpy_s(g_szPath,MAX_PATH*sizeof(TCHAR),pszPath);
}

void RC_API  Sinstar_InitA(LPCSTR pszPath)
{
	MultiByteToWideChar(CP_ACP,0,pszPath,strlen(pszPath),g_szPath,MAX_PATH);
}

BOOL RC_API  Sinstar_GetInstallDir(LPWSTR  pszPath,int nSize)
{
	CRegKey reg;
	LONG ret = reg.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\SetoutSoft\\sinstar3"), KEY_READ | KEY_WOW64_64KEY);
	if (ret != ERROR_SUCCESS)
	{
		return FALSE;
	}

	ULONG nLen = nSize;
	reg.QueryStringValue(_T("path_client"),pszPath,&nLen);
	reg.Close();
	return TRUE;
}

BOOL RC_API  Sinstar_ShowCaller(BOOL bOrg)
{
	TCHAR szPath[MAX_PATH];
	STARTUPINFO si = { sizeof(STARTUPINFO),0 };
	PROCESS_INFORMATION pi = { 0 };
	if(bOrg)
	{
		TCHAR szOriPath[MAX_PATH];
		if(!Sinstar_GetInstallDir(szOriPath,MAX_PATH))
			return FALSE;
		_stprintf(szPath, _T("%s\\program\\findcallerUI.exe sinstar3_ime.ime|sinstar3_tsf.dll"), szOriPath);
	}else
	{
		_stprintf(szPath, _T("%s\\program\\findcallerUI.exe sinstar3_ime.ime|sinstar3_tsf.dll"), g_szPath);
	}
	if (CreateProcess(NULL, szPath, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		return TRUE;
	}
	else
	{
		Sinstar_SetErrMsg(_T("启动FindCallerUI失败"));
		return FALSE;
	}
}

DWORD CallRegsvr32(LPCTSTR pszFileName, BOOL bReg)
{
	STARTUPINFO         si = { 0 };
	PROCESS_INFORMATION pi;
	DWORD dwWaitRet = -1;
	si.cb = sizeof(si);
	si.dwFlags = 0;
	si.wShowWindow = 0;

	TCHAR szSysDir[MAX_PATH];
	GetSystemDirectory(szSysDir, MAX_PATH);

	TCHAR szCmd[1000];
	_stprintf(szCmd, _T("regsvr32 /s %s %s"), bReg ? _T("") : _T("/u"), pszFileName);
	//以后台启动方式创建服务器进程
	if (!CreateProcess(NULL, szCmd, NULL, NULL, FALSE, CREATE_NEW_PROCESS_GROUP, NULL, szSysDir, &si, &pi))
	{
		return FALSE;
	}
	WaitForSingleObject(pi.hProcess, INFINITE);
	GetExitCodeProcess(pi.hProcess, &dwWaitRet);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return dwWaitRet;
}

typedef UINT
(RC_API
 *FunGetSystemWow64Directory)(
 LPTSTR lpBuffer,
 UINT uSize
 );


BOOL GetSysWow64Dir(LPTSTR pszDir, int nLen)
{
	if (!Is64OS())
		return FALSE;
	HMODULE hMod = LoadLibrary(_T("Kernel32.dll"));
#ifdef _UNICODE
	FunGetSystemWow64Directory fGetSystemWow64Directory = (FunGetSystemWow64Directory)GetProcAddress(hMod, "GetSystemWow64DirectoryW");
#else
	FunGetSystemWow64Directory fGetSystemWow64Directory = (FunGetSystemWow64Directory)GetProcAddress(hMod, "GetSystemWow64DirectoryA");
#endif
	if (!fGetSystemWow64Directory) return FALSE;
	fGetSystemWow64Directory(pszDir, nLen);
	FreeLibrary(hMod);
	return TRUE;
}


BOOL RC_API  Sinstar_IsRunning()
{
	HANDLE hMutex = CreateMutex(NULL, FALSE, SINSTAR3_MUTEX);
	BOOL bRet = GetLastError() == ERROR_ALREADY_EXISTS;
	CloseHandle(hMutex);
	return bRet;
}

BOOL Sinstar_Unreg(LPCTSTR sysWow64,LPCTSTR szSysPath,BOOL bDelFile)
{
	TCHAR szPath[1000];
	//删除输入法文件
	if (Is64OS())
	{
		for (int i = 0; i<ARRAYSIZE(KSrcX64Files); i++)
		{
			_stprintf(szPath, _T("%s\\%s"), szSysPath, KSrcX64Files[i].pszDest);
			if (KSrcX64Files[i].bReg && 0 != CallRegsvr32(szPath, FALSE))
			{
				return FALSE;
			}
			if(bDelFile) DeleteFile(szPath);
		}

		for (int i = 0; i<ARRAYSIZE(KSrcX86Files); i++)
		{
			_stprintf(szPath, _T("%s\\%s"), sysWow64, KSrcX86Files[i].pszDest);
			if (KSrcX86Files[i].bReg && 0 != CallRegsvr32(szPath, FALSE))
			{
				return FALSE;
			}
			if(bDelFile) DeleteFile(szPath);
		}
	}
	else
	{
		for (int i = 0; i<ARRAYSIZE(KSrcX86Files); i++)
		{
			_stprintf(szPath, _T("%s\\%s"), szSysPath, KSrcX86Files[i].pszDest);
			if (0 != CallRegsvr32(szPath, FALSE))
			{
				return FALSE;
			}
			if(bDelFile) DeleteFile(szPath);
		}
	}
	return TRUE;
}

BOOL Sinstar_Reg(LPCTSTR szSysWow64,LPCTSTR szSysPath)
{

	TCHAR szPath[1000];
	//step6:安装输入法
	if (Is64OS())
	{
		for (int i = 0; i<ARRAYSIZE(KSrcX86Files); i++)
		{
			if (!KSrcX86Files[i].bReg) continue;
			_stprintf(szPath, _T("%s\\%s"), szSysWow64, KSrcX86Files[i].pszDest);
			if (0 != CallRegsvr32(szPath, TRUE))
			{
				return FALSE;
			}
		}

		for (int i = 0; i<ARRAYSIZE(KSrcX64Files); i++)
		{
			if (!KSrcX64Files[i].bReg) continue;
			_stprintf(szPath, _T("%s\\%s"), szSysPath, KSrcX64Files[i].pszDest);
			if (0 != CallRegsvr32(szPath, TRUE))
			{
				return FALSE;
			}
		}
	}
	else
	{
		for (int i = 0; i<ARRAYSIZE(KSrcX86Files); i++)
		{
			_stprintf(szPath, _T("%s\\%s"), szSysPath, KSrcX86Files[i].pszDest);
			if (0 != CallRegsvr32(szPath, TRUE))
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}

BOOL Sinstar_UpdateIME()
{
	TCHAR szPath1[MAX_PATH], szPath2[MAX_PATH], szSysPath[MAX_PATH], szSysWow64[MAX_PATH];
	::GetSystemDirectory(szSysPath, MAX_PATH);
	GetSysWow64Dir(szSysWow64, MAX_PATH);

	if(Sinstar_IsRunning())
	{
		Sinstar_SetErrMsg(_T("输入法正在使用，不能更新"));
		return FALSE;
	}

	BOOL bCopied = TRUE;
	if (Is64OS())
	{
		//copy x64 files to system dir
		for (int i = 0; i<ARRAYSIZE(KSrcX64Files) && bCopied; i++)
		{
			_stprintf(szPath1, _T("%s\\%s"), g_szPath, KSrcX64Files[i].pszSrc);
			_stprintf(szPath2, _T("%s\\%s"), szSysPath, KSrcX64Files[i].pszDest);
			bCopied = CopyFile(szPath1, szPath2, FALSE);
		}

		//copy x86 files to wow64 dir.
		for (int i = 0; i<ARRAYSIZE(KSrcX86Files) && bCopied; i++)
		{
			_stprintf(szPath1, _T("%s\\%s"), g_szPath, KSrcX86Files[i].pszSrc);
			_stprintf(szPath2, _T("%s\\%s"), szSysWow64, KSrcX86Files[i].pszDest);
			bCopied = CopyFile(szPath1, szPath2, FALSE);
		}
	}
	else
	{
		//copy x86 files to system dir
		for (int i = 0; i<ARRAYSIZE(KSrcX86Files) && bCopied; i++)
		{
			_stprintf(szPath1, _T("%s\\%s"), g_szPath, KSrcX86Files[i].pszSrc);
			_stprintf(szPath2, _T("%s\\%s"), szSysPath, KSrcX86Files[i].pszDest);
			bCopied = CopyFile(szPath1, szPath2, FALSE);
		}
	}
	if(!bCopied)
	{
		Sinstar_SetErrMsg(_T("复制文件到系统目录失败！"));
	}
	return bCopied;
}
BOOL RC_API  Sinstar_PEVersionW(LPCWSTR pszFileName,  int wVers[4])
{
	return Sinstar_PEVersion2W(pszFileName,wVers+0,wVers+1,wVers+2,wVers+3);
}

BOOL RC_API  Sinstar_PEVersion2W(LPCWSTR pszFileName,int *v1,int *v2,int *v3,int *v4)
{
	DWORD dwResHandle;
	void *pBuf;
	BOOL bRet = FALSE;
	DWORD dwVerInfoSize = GetFileVersionInfoSizeW(pszFileName, &dwResHandle);
	if (!dwVerInfoSize) return FALSE;
	pBuf = malloc(dwVerInfoSize);
	GetFileVersionInfo(pszFileName, dwResHandle, dwVerInfoSize, pBuf);
	UINT nVersionLen;
	VS_FIXEDFILEINFO *pstFileVersion;
	if (VerQueryValueW(pBuf, _T("\\"), (void**)&pstFileVersion, &nVersionLen) && nVersionLen >= sizeof(VS_FIXEDFILEINFO))
	{
		*v4 = LOWORD(pstFileVersion->dwFileVersionLS);
		*v3 = HIWORD(pstFileVersion->dwFileVersionLS);
		*v2 = LOWORD(pstFileVersion->dwFileVersionMS);
		*v1 = HIWORD(pstFileVersion->dwFileVersionMS);
	}
	free(pBuf);
	return TRUE;
}

BOOL RC_API  Sinstar_PEVersion2A(LPCSTR pszFileName,int *v1,int *v2,int *v3,int *v4)
{
	WCHAR szPath[MAX_PATH]={0};
	MultiByteToWideChar(CP_ACP,0,pszFileName,strlen(pszFileName),szPath,MAX_PATH);
	return Sinstar_PEVersion2W(szPath,v1,v2,v3,v4);
}

BOOL RC_API  Sinstar_PEVersionA(LPCSTR pszFileName,  int wVers[4])
{
	return Sinstar_PEVersion2A(pszFileName,wVers+0,wVers+1,wVers+2,wVers+3);
}

BOOL RC_API  Sinstar_GetCurrentVer2(int *v1,int *v2,int *v3,int *v4)
{
	WCHAR szPath[MAX_PATH];
	if(!Sinstar_GetInstallDir(szPath,MAX_PATH))
		return FALSE;

	TCHAR szSvrPath[MAX_PATH];
	_stprintf(szSvrPath,_T("%s\\program\\isserver3.exe"),szPath);
	return Sinstar_PEVersion2W(szSvrPath,v1,v2,v3,v4);
}

BOOL RC_API  Sinstar_GetCurrentVer(int wVers[4])
{
	return Sinstar_GetCurrentVer2(wVers+0,wVers+1,wVers+2,wVers+3);
}

BOOL RC_API  Sinstar_Update()
{	
	if(Sinstar_IsRunning())
	{
		Sinstar_SetErrMsg(_T("输入法正在使用，不能安装"));
		return FALSE;
	}

	CRegKey reg;
	LONG ret = reg.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\SetoutSoft\\sinstar3"), KEY_READ | KEY_WOW64_64KEY);
	if (ret != ERROR_SUCCESS)
	{
		return Sinstar_Install();
	}

	//退出服务器
	Sinstar_QuitServer();


	TCHAR szPath[MAX_PATH];
	ULONG nLen = MAX_PATH;
	reg.QueryStringValue(_T("path_client"),szPath,&nLen);
	reg.Close();

	if(_tcsicmp(szPath,g_szPath)!=0)
	{
		for (int i = 0; i < ARRAYSIZE(KUpdateInfo); i++)
		{
			TCHAR szDest[MAX_PATH]={0};
			TCHAR szSour[MAX_PATH]={0};
			_stprintf(szSour, _T("%s\\%s"), g_szPath, KUpdateInfo[i].pszFile);
			if(KUpdateInfo[i].bClean)
			{
				memset(szDest,0,sizeof(szDest));
				_stprintf(szDest, _T("%s\\%s"), szPath,KUpdateInfo[i].pszFile);
				//delete old program dir
				SHFILEOPSTRUCT fileOp = { 0 };
				fileOp.wFunc = FO_DELETE;
				fileOp.pFrom = szDest;
				fileOp.fFlags = FOF_NOCONFIRMATION | FOF_SILENT | FOF_ALLOWUNDO;
				int nRet = SHFileOperation(&fileOp);
			}
			{
				memset(szDest,0,sizeof(szDest));
				_stprintf(szDest, _T("%s"), szPath);
				//copy file to old path.
				SHFILEOPSTRUCT fileOp = { 0 };
				fileOp.wFunc = FO_COPY;
				fileOp.pFrom = szSour;
				fileOp.pTo = szDest;
				fileOp.fFlags = FOF_NOCONFIRMATION | FOF_SILENT|FOF_NOCOPYSECURITYATTRIBS|FOF_NOCONFIRMMKDIR ;
				int nRet = SHFileOperation(&fileOp);
			}
		}
	}

	Sinstar_UpdateIME();

	//step8:reg ime file type.
	TCHAR szSvrCmd[MAX_PATH];
	_stprintf(szSvrCmd,_T("%s\\program\\isserver3.exe"),szPath);
	ShellExecute(NULL, _T("open"), szSvrCmd, _T("-reg"), NULL, 0);
	
	return TRUE;
}

BOOL RC_API  Sinstar_Uninstall()
{
	TCHAR szSysPath[MAX_PATH], sysWow64[MAX_PATH];
	::GetSystemDirectory(szSysPath, MAX_PATH);
	GetSysWow64Dir(sysWow64, MAX_PATH);
	if(Sinstar_IsRunning())
	{
		Sinstar_SetErrMsg(_T("输入法正在使用，不能卸载"));
		return FALSE;
	}

	if(!Sinstar_Unreg(sysWow64,szSysPath,TRUE))
	{
		Sinstar_SetErrMsg(_T("反注册输入法接口失败"));
		return FALSE;
	}

	//delete reg
	CRegKey reg;
	LONG ret = reg.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\SetoutSoft"), KEY_WRITE | KEY_READ | KEY_WOW64_64KEY);
	if (ret != ERROR_SUCCESS)
	{
		Sinstar_SetErrMsg(_T("打开注册表失败"));
		return FALSE;
	}

	TCHAR szClient[MAX_PATH];
	ULONG uSize = MAX_PATH;
	reg.QueryStringValue(_T("path_client"), szClient, &uSize);

	reg.RecurseDeleteKey(_T("sinstar3"));
	reg.Close();


	//退出服务器
	Sinstar_QuitServer();

	//step8:reg ime file type.
	TCHAR szRegCmd[MAX_PATH];
	_stprintf(szRegCmd, _T("%s\\program\\isserver3.exe"), szClient);
	ShellExecute(NULL, _T("open"), szRegCmd, _T("-unreg"), NULL, 0);

	return TRUE;
}


void DeleteFileEx(LPCTSTR pszPath)
{
	TCHAR szTmpFile[MAX_PATH*2];
	for(int i=0;;i++)
	{
		_stprintf(szTmpFile,_T("%s.bak.%d"),pszPath,i);
		if(GetFileAttributes(szTmpFile)==INVALID_FILE_ATTRIBUTES)
			break;
	}
	MoveFile(pszPath,szTmpFile);
	MoveFileEx(szTmpFile, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
}

BOOL RC_API  Sinstar_ForceUninstall()
{
	TCHAR szSysPath[MAX_PATH], sysWow64[MAX_PATH];
	TCHAR szPath[MAX_PATH];
	::GetSystemDirectory(szSysPath, MAX_PATH);
	GetSysWow64Dir(sysWow64, MAX_PATH);
	if(!Sinstar_IsRunning())
	{
		return Sinstar_Uninstall();
	}

	//删除输入法文件
	if (Is64OS())
	{
		for (int i = 0; i<ARRAYSIZE(KSrcX64Files); i++)
		{
			_stprintf(szPath, _T("%s\\%s"), szSysPath, KSrcX64Files[i].pszDest);
			if (KSrcX64Files[i].bReg && 0 != CallRegsvr32(szPath, FALSE))
			{
				Sinstar_SetErrMsg(_T("反注册输入法模块失败!"));
				return FALSE;
			}

			DeleteFileEx(szPath);
		}

		for (int i = 0; i<ARRAYSIZE(KSrcX86Files); i++)
		{
			_stprintf(szPath, _T("%s\\%s"), sysWow64, KSrcX86Files[i].pszDest);
			if (KSrcX86Files[i].bReg && 0 != CallRegsvr32(szPath, FALSE))
			{
				Sinstar_SetErrMsg(_T("反注册输入法模块失败!"));
				return FALSE;
			}
			DeleteFileEx(szPath);
		}
	}
	else
	{
		for (int i = 0; i<ARRAYSIZE(KSrcX86Files); i++)
		{
			_stprintf(szPath, _T("%s\\%s"), szSysPath, KSrcX86Files[i].pszDest);
			if (0 != CallRegsvr32(szPath, FALSE))
			{
				Sinstar_SetErrMsg(_T("反注册输入法模块失败!"));
				return FALSE;
			}
			DeleteFileEx(szPath);
		}
	}

	//delete reg
	CRegKey reg;
	LONG ret = reg.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\SetoutSoft"), KEY_WRITE | KEY_READ | KEY_WOW64_64KEY);
	if (ret != ERROR_SUCCESS)
	{
		Sinstar_SetErrMsg(_T("打开注册表失败!"));
		return FALSE;
	}

	TCHAR szClient[MAX_PATH];
	ULONG uSize = MAX_PATH;
	reg.QueryStringValue(_T("path_client"), szClient, &uSize);

	reg.RecurseDeleteKey(_T("sinstar3"));
	reg.Close();


	//step8:reg ime file type.
	TCHAR szRegCmd[MAX_PATH];
	_stprintf(szRegCmd, _T("%s\\program\\isserver3.exe"), g_szPath);
	ShellExecute(NULL, _T("open"), szRegCmd, _T("-unreg"), NULL, 0);

	return TRUE;
}

BOOL MyCopyFile(LPCTSTR pszSour, LPCTSTR pszDest)
{
	TCHAR szSour[512] = { 0 }, szDest[512] = { 0 };
	_tcscpy(szSour, pszSour);
	_tcscpy(szDest, pszDest);

	SHFILEOPSTRUCT fileOp = { 0 };
	fileOp.wFunc = FO_COPY;
	fileOp.pFrom = szSour;
	fileOp.pTo = szDest;
	fileOp.fFlags = FOF_NOCONFIRMATION | FOF_SILENT | FOF_FILESONLY;
	int nRet = SHFileOperation(&fileOp);
	return nRet == 0;
}

void RC_API Sinstar_QuitServer()
{
	HWND hWndSvr = FindWindow(NULL, SINSTAR3_SERVER_HWND);
	if (IsWindow(hWndSvr))
	{
		DWORD dwProcID = 0;
		GetWindowThreadProcessId(hWndSvr, &dwProcID);
		PostMessage(hWndSvr, WM_QUIT, 0, 0);
		if (dwProcID != 0)
			WaitForSingleObject((HANDLE)dwProcID, INFINITE);
		else
			Sleep(500);
	}
}

BOOL RC_API  Sinstar_Install()
{
	TCHAR szSysPath[MAX_PATH];
	TCHAR szSysWow64[MAX_PATH] = { 0 };

	GetSystemDirectory(szSysPath, MAX_PATH);
	GetSysWow64Dir(szSysWow64, MAX_PATH);

	if(Sinstar_IsRunning())
	{
		Sinstar_SetErrMsg( _T("输入法正在使用，不能安装"));
		return FALSE;
	}

	//step2 copy files.
	if (!Sinstar_UpdateIME())
	{
		return FALSE;
	}

	TCHAR szSvrExe[256] = { 0 }, szSvrData[256] = { 0 };
	_stprintf(szSvrExe, _T("%s\\program\\isserver3.exe"), g_szPath);
	_stprintf(szSvrData, _T("%s\\server"), g_szPath);

	//step3: write reg
	CRegKey reg;
	LONG ret = reg.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\SetoutSoft\\sinstar3"), KEY_WRITE | KEY_WOW64_64KEY);
	if (ret != ERROR_SUCCESS)
	{
		ret = reg.Create(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\SetoutSoft\\sinstar3"), 0, 0, KEY_WRITE | KEY_WOW64_64KEY);
		if (ret != ERROR_SUCCESS)
		{
			Sinstar_SetErrMsg(_T("打开注册表失败"));
			return FALSE;
		}
	}
	{
		reg.SetStringValue(_T("path_client"), g_szPath);
		reg.SetStringValue(_T("path_svr"), szSvrExe);
		reg.SetStringValue(_T("path_svr_data"), szSvrData);
		reg.Close();
	}

	if(!Sinstar_Reg(szSysWow64,szSysPath))
	{
		Sinstar_SetErrMsg(_T("注册失败"));
		return FALSE;
	}
	//step8:reg ime file type.
	ShellExecute(NULL, _T("open"), szSvrExe, _T("-reg"), NULL, 0);

	return TRUE;
}


BOOL RC_API  Sinstar_CheckFiles()
{
	for (int i = 0; i<ARRAYSIZE(KFiles); i++)
	{
		TCHAR szBuf[1000];
		_stprintf(szBuf, _T("%s\\%s"),g_szPath, KFiles[i]);
		if (GetFileAttributes(szBuf) == INVALID_FILE_ATTRIBUTES)
		{
			_stprintf(szBuf, _T("安装目录文件[%s]没有找到，请保证安装包完整！"), KFiles[i]);
			Sinstar_SetErrMsg(szBuf);
			return 0;
		}
	}
	return TRUE;
}
