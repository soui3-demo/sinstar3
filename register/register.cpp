// register.cpp : Defines the entry point for the application.
//

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS

#include "register.h"
#include <stdio.h>
#include <imm.h>
#include <TCHAR.h>
#include <ShellAPI.h>
#pragma comment(lib,"shell32.lib")
#pragma comment(lib,"imm32.lib")
#include <winver.h>
#pragma comment(lib,"version.lib")

#include "../include/version.h"
#include "../include/reg.h"

#define MAX_STRLEN		100

#define NAME_MAPFILE _T("_setoutsoft comm map file")
#define SINSTAR3_CORE	_T("sinstar3_core.dll")
#define SINSTARE_IME	_T("sinstar3_ime")

TCHAR g_szPath[MAX_PATH]={0};	//程序启动位置

//定义一个结构，以便初始化对话框
typedef struct tagDLGPARAM
{
	HINSTANCE hInst;	//程序实例
	TCHAR szIme[MAX_PATH];	//输入法文件名
	TCHAR szName[100];	//安装到注册表中的名称
}DLGPARAM;


//获取一个PE文件的version + ProductName +  FileDescription
BOOL Helper_PEVersion(LPCTSTR pszFileName,DWORD *pdwVer,TCHAR *pszName ,TCHAR *pszDesc)
{
	DWORD dwResHandle;
	LPTSTR lszVer = NULL; 
	UINT  cchVer = 0;  
	void *pBuf;
	BOOL bRet=FALSE;
	DWORD dwVerInfoSize=GetFileVersionInfoSize((TCHAR*)pszFileName,&dwResHandle);
	if(!dwVerInfoSize) return FALSE;
	pBuf=malloc(dwVerInfoSize);
	GetFileVersionInfo((TCHAR*)pszFileName, dwResHandle, dwVerInfoSize, pBuf); 
	if(pdwVer)
	{
		UINT nVersionLen;
		VS_FIXEDFILEINFO *pstFileVersion;
		if(VerQueryValue(pBuf, _T("\\"), (void**)&pstFileVersion, &nVersionLen) && nVersionLen>=sizeof(VS_FIXEDFILEINFO) )
		{
			*pdwVer=MAKELONG(MAKEWORD(LOWORD(pstFileVersion->dwFileVersionLS),HIWORD(pstFileVersion->dwFileVersionLS)),
				MAKEWORD(LOWORD(pstFileVersion->dwFileVersionMS),HIWORD(pstFileVersion->dwFileVersionMS)));
		}
	}

	bRet = VerQueryValue(pBuf, _T("\\VarFileInfo\\Translation"), (void**)&lszVer, &cchVer); 
	if(bRet&&cchVer==4)
	{
		TCHAR szLangCode[20]={0};
		TCHAR szSection[256]; 
		BOOL bOK;
		WORD * pwVer = (WORD*)lszVer;
		_stprintf(szLangCode,_T("%04x"),pwVer[0]);
		_stprintf(szLangCode+4,_T("%04x"), pwVer[1]);
		if(pszName)
		{
			_stprintf(szSection,_T("\\StringFileInfo\\%s\\ProductName"),szLangCode);
			bOK= VerQueryValue(pBuf, szSection, (void**)&lszVer, &cchVer); 
			if(bOK) _tcscpy(pszName,lszVer);
		}
		if(pszDesc)
		{
			_stprintf(szSection,_T("\\StringFileInfo\\%s\\FileDescription"),szLangCode);
			bOK = VerQueryValue(pBuf, szSection, (void**)&lszVer, &cchVer); 
			if(bOK) _tcscpy(pszDesc,lszVer);
		}
	}
	free(pBuf);
	return bRet;
}
//获取文件的版本号
void Helper_VersionString(DWORD dwVer,TCHAR *pszBuf)
{
	_stprintf(pszBuf,_T("%u.%u.%u.%u"),(dwVer>>24)&0xFF,(dwVer>>16)&0xFF,(dwVer>>8)&0xFF,dwVer&0xFF);
}

#include <AclAPI.h>
BOOL Helper_SetFileACL(LPCTSTR pszPath)
{
	HANDLE hDir = CreateFile(pszPath,READ_CONTROL|WRITE_DAC,0,NULL,OPEN_EXISTING,FILE_FLAG_BACKUP_SEMANTICS,NULL);
	if(hDir == INVALID_HANDLE_VALUE)
		return FALSE; 

	ACL* pOldDACL;
	SECURITY_DESCRIPTOR* pSD = NULL;
	GetSecurityInfo(hDir, SE_FILE_OBJECT , DACL_SECURITY_INFORMATION,NULL, NULL, &pOldDACL, NULL, (void**)&pSD);

	PSID pSid = NULL;
	SID_IDENTIFIER_AUTHORITY authNt = SECURITY_NT_AUTHORITY;
	AllocateAndInitializeSid(&authNt,2,SECURITY_BUILTIN_DOMAIN_RID,DOMAIN_ALIAS_RID_USERS,0,0,0,0,0,0,&pSid);

	EXPLICIT_ACCESS ea={0};
	ea.grfAccessMode = GRANT_ACCESS;
	ea.grfAccessPermissions = GENERIC_ALL;
	ea.grfInheritance = CONTAINER_INHERIT_ACE|OBJECT_INHERIT_ACE;
	ea.Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea.Trustee.ptstrName = (LPTSTR)pSid;

	ACL* pNewDACL = 0;
	DWORD err = SetEntriesInAcl(1,&ea,pOldDACL,&pNewDACL);

	if(pNewDACL)
		SetSecurityInfo(hDir,SE_FILE_OBJECT,DACL_SECURITY_INFORMATION,NULL, NULL, pNewDACL, NULL);

	FreeSid(pSid);
	LocalFree(pNewDACL);
	LocalFree(pSD);
	LocalFree(pOldDACL);
	CloseHandle(hDir);

	return TRUE;

}


BOOL Helper_SetFileACLEx(LPCTSTR pszPath,BOOL bSubFile)
{
	SECURITY_DESCRIPTOR* pSD = NULL;

	PSID pSid = NULL;
	SID_IDENTIFIER_AUTHORITY authNt = SECURITY_NT_AUTHORITY;
	AllocateAndInitializeSid(&authNt,2,SECURITY_BUILTIN_DOMAIN_RID,DOMAIN_ALIAS_RID_USERS,0,0,0,0,0,0,&pSid);

	EXPLICIT_ACCESS ea={0};
	ea.grfAccessMode = GRANT_ACCESS;
	ea.grfAccessPermissions = GENERIC_ALL;
	ea.grfInheritance = CONTAINER_INHERIT_ACE|OBJECT_INHERIT_ACE;
	ea.Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea.Trustee.ptstrName = (LPTSTR)pSid;

	ACL* pNewDACL = 0;
	DWORD err = SetEntriesInAcl(1,&ea,NULL,&pNewDACL);

	if(pNewDACL)
	{
		HANDLE hFile = CreateFile(pszPath,READ_CONTROL|WRITE_DAC,0,NULL,OPEN_EXISTING,FILE_FLAG_BACKUP_SEMANTICS,NULL);
		if(hFile != INVALID_HANDLE_VALUE) SetSecurityInfo(hFile,SE_FILE_OBJECT,DACL_SECURITY_INFORMATION,NULL, NULL, pNewDACL, NULL);
		CloseHandle(hFile);
		if(bSubFile)
		{
			WIN32_FIND_DATA wfd={0};
			TCHAR szPath[MAX_PATH],szFilter[MAX_PATH];
			_stprintf(szFilter,_T("%s\\*.*"),pszPath);
			HANDLE hFind=FindFirstFile(szFilter,&wfd);
			if(hFind)
			{
				do 
				{
					if(!(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
					{
						_stprintf(szPath,_T("%s\\%s"),pszPath,wfd.cFileName);
						HANDLE hFile = CreateFile(szPath,READ_CONTROL|WRITE_DAC,0,NULL,OPEN_EXISTING,FILE_FLAG_BACKUP_SEMANTICS,NULL);
						if(hFile != INVALID_HANDLE_VALUE) SetSecurityInfo(hFile,SE_FILE_OBJECT,DACL_SECURITY_INFORMATION,NULL, NULL, pNewDACL, NULL);
						CloseHandle(hFile);
					}
				} while (FindNextFile(hFind,&wfd));
				FindClose(hFind);
			}
		}
	}
	FreeSid(pSid);
	LocalFree(pNewDACL);
	LocalFree(pSD);

	return TRUE;

}

BOOL Is64OS()
{
	SYSTEM_INFO si;
	GetNativeSystemInfo(&si);

	if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
		si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
} 



//从输入文件中获取文件属性
BOOL QueryImeProp(TCHAR *pszIme,TCHAR *pszImeProp)
{
	TCHAR szImeFile[MAX_PATH] = { 0 }, szName[100] = { 0 };
	_stprintf(szImeFile,_T("%s\\%s"),g_szPath,pszIme);
	DWORD dwVer = 0;
	if(!Helper_PEVersion(szImeFile,&dwVer,NULL,szName)) return FALSE;
	BYTE byVer[4];
	memcpy(byVer, &dwVer, 4);
	_stprintf(pszImeProp,_T("%s%d.%d"),szName,UINT(byVer[3]),UINT(byVer[2]));
	return TRUE;
}

void VerDot2Space(TCHAR *pszVer)
{
	while(*pszVer)
	{
		if(*pszVer == '.') *pszVer=0x20;
		pszVer++;
	}
}

BOOL CheckVersion(TCHAR *pszVerNew,TCHAR *pszVerCur)
{
	DWORD dwVerCur=0,dwVerNew=0;
	int byVer1,byVer2,byVer3,byVer4;
	VerDot2Space(pszVerNew);
	VerDot2Space(pszVerCur);
	_stscanf(pszVerCur,_T("%u %u %u %u"),&byVer1,&byVer2,&byVer3,&byVer4);
	dwVerCur=MAKELONG(MAKEWORD(byVer4,byVer3),MAKEWORD(byVer2,byVer1));
	_stscanf(pszVerNew,_T("%u %u %u %u"),&byVer1,&byVer2,&byVer3,&byVer4);
	dwVerNew=MAKELONG(MAKEWORD(byVer4,byVer3),MAKEWORD(byVer2,byVer1));
	return (dwVerCur<dwVerNew);
}

void ShowCaller(LPCTSTR pszImeCore)
{
	TCHAR szPath[MAX_PATH];
	STARTUPINFO si={sizeof(STARTUPINFO),0};
	PROCESS_INFORMATION pi={0};
	_stprintf(szPath,_T("%s\\program\\findcallerUI.exe %s"), g_szPath, pszImeCore);
	if(CreateProcess(NULL,szPath,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi))
	{
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}else
	{
		MessageBox(GetActiveWindow(),_T("启动FindCallerUI失败"),_T("提示"),MB_OK|MB_ICONSTOP);
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
(WINAPI
	*FunGetSystemWow64Directory)(
		LPTSTR lpBuffer,
		UINT uSize
		);


BOOL GetSysWow64Dir(LPTSTR pszDir,int nLen)
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

BOOL Sinstar_Update(LPCTSTR pszIme)
{
	TCHAR szPath1[MAX_PATH],szPath2[MAX_PATH],szSysPath[MAX_PATH],szSysWow64[MAX_PATH];
	::GetSystemDirectory(szSysPath,MAX_PATH);
	GetSysWow64Dir(szSysWow64, MAX_PATH);

	HANDLE hMutex = CreateMutex(NULL, FALSE, SINSTAR3_MUTEX);
	if(GetLastError()==ERROR_ALREADY_EXISTS)
	{
		CloseHandle(hMutex);
		if (MessageBox(GetActiveWindow(), _T("输入法正在使用，不能更新，查看哪些程序在使用吗？"), _T("提示"), MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
		{
			ShowCaller(SINSTAR3_CORE);
		}
		return 0;
	}
	CloseHandle(hMutex);
	
	BOOL bCopied = FALSE;
	if (Is64OS())
	{
		_stprintf(szPath1, _T("%s\\program\\x64\\%s.dll"), g_szPath, pszIme);
		_stprintf(szPath2, _T("%s\\%s.ime"), szSysPath, pszIme);
		bCopied = CopyFile(szPath1, szPath2, FALSE);

		if (bCopied)
		{
			_stprintf(szPath1, _T("%s\\program\\%s.dll"), g_szPath, pszIme);
			_stprintf(szPath2, _T("%s\\%s.ime"), szSysPath, pszIme);
			bCopied=CopyFile(szPath1, szPath2, FALSE);
		}
	}
	else
	{
		_stprintf(szPath1, _T("%s\\program\\%s.dll"), g_szPath, pszIme);
		_stprintf(szPath2, _T("%s\\%s.ime"), szSysPath, pszIme);
		bCopied = CopyFile(szPath1, szPath2, FALSE);
	}
	if (!bCopied)
	{
		MessageBox(GetActiveWindow(), _T("Copy File Failed"), _T("update"), MB_OK);
	}
	return bCopied;
}

BOOL Sinstar_Uninstall(LPCTSTR pszIme)
{
	TCHAR szSysPath[MAX_PATH],sysWow64[MAX_PATH];
	TCHAR szPath[MAX_PATH];
	::GetSystemDirectory(szSysPath,MAX_PATH);
	GetSysWow64Dir(sysWow64, MAX_PATH);

	HANDLE hMutex = CreateMutex(NULL, FALSE, SINSTAR3_MUTEX);
	if(!hMutex)
	{
		MessageBox(GetActiveWindow(),_T("mutex create error"),_T("?"),MB_OK);
		return 0;
	}
	if(GetLastError()==ERROR_ALREADY_EXISTS)
	{
		CloseHandle(hMutex);
		if (MessageBox(GetActiveWindow(), _T("输入法正在使用，不能卸载，查看哪些程序在使用吗？"), _T("提示"), MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
		{
			ShowCaller(SINSTAR3_CORE);
		}
		return 0;
	}
	CloseHandle(hMutex);
	if(MessageBox(GetActiveWindow(),_T("确定卸载吗？"),_T("提示"),MB_OKCANCEL|MB_ICONQUESTION)==IDCANCEL) 
		return 0;


	_stprintf(szPath,_T("%s\\%s.ime"),szSysPath,pszIme);
	if (0 != CallRegsvr32(szPath, FALSE))
	{
		MessageBox(GetActiveWindow(), _T("卸载程序失败"), _T("提示"), MB_OK | MB_ICONSTOP);
		return 0;
	}

	//删除输入法文件
	if (Is64OS())
	{
		_stprintf(szPath, _T("%s\\%s.ime"), szSysPath, pszIme);
		DeleteFile(szPath);
		_stprintf(szPath, _T("%s\\%s.ime"), sysWow64, pszIme);
		DeleteFile(szPath);
	}
	else
	{
		_stprintf(szPath, _T("%s\\%s.ime"), szSysPath, pszIme);
		DeleteFile(szPath);
	}

	//delete reg
	CRegKey reg;
	LONG ret = reg.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\SetoutSoft"), KEY_WRITE | KEY_WOW64_64KEY);
	if (ret != ERROR_SUCCESS)
	{
		MessageBox(GetActiveWindow(), _T("打开注册表失败"), _T("uninstall"), MB_OK | MB_ICONSTOP);
		return 0;
	}
	reg.RecurseDeleteKey(_T("sinstar3"));
	reg.Close();

	//退出服务器
	HANDLE hMapData = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, NAME_MAPFILE);
	if (hMapData)
	{
		HWND hWndSvr = 0;
		LPVOID pData = MapViewOfFile(hMapData, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
		memcpy(&hWndSvr, pData, sizeof(HWND));
		UnmapViewOfFile(pData);
		CloseHandle(hMapData);
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
	return TRUE;
}


BOOL MyCopyFile(LPCTSTR pszSour,LPCTSTR pszDest)
{
	TCHAR szSour[512]={0},szDest[512]={0};
	_tcscpy(szSour,pszSour);
	_tcscpy(szDest,pszDest);

	SHFILEOPSTRUCT fileOp={0};
	fileOp.wFunc=FO_COPY;
	fileOp.pFrom=szSour;
	fileOp.pTo=szDest;
	fileOp.fFlags=FOF_NOCONFIRMATION|FOF_SILENT|FOF_FILESONLY;
	int nRet=SHFileOperation(&fileOp);
	return nRet==0;
}

BOOL Sinstar_Install(LPCTSTR pszImeName,LPCTSTR pszIme)
{
	TCHAR szSysPath[MAX_PATH];
	TCHAR szPath1[300],szPath2[300];
	TCHAR szSysWow64[MAX_PATH]={0};

	GetSystemDirectory(szSysPath, MAX_PATH);
	GetSysWow64Dir(szSysWow64, MAX_PATH);
	
	//step1:验证文件有效性
	_stprintf(szPath1,_T("%s\\program\\%s.dll"),g_szPath, pszIme);
	if(GetFileAttributes(szPath1)==0xFFFFFFFF)
	{
		TCHAR szMsg[100];
		_stprintf(szMsg,_T("输入法目录下没有找到输入法文件：%s"),pszIme);
		MessageBox(GetActiveWindow(),szMsg,_T("提示"),MB_OK|MB_ICONSTOP);
		return FALSE;
	}

	TCHAR szSvrExe[256] = { 0 }, szSvrData[256] = { 0 };
	_stprintf(szSvrExe, _T("%s\\server\\isserver.exe"), g_szPath);
	_stprintf(szSvrData,_T("%s\\server"),g_szPath);
	if(GetFileAttributes(szSvrExe)==0xFFFFFFFF)
	{
		TCHAR szMsg[100];
		_stprintf(szMsg,_T("服务器目录下没有找到isserver.exe"));
		MessageBox(GetActiveWindow(),szMsg,_T("提示"),MB_OK|MB_ICONSTOP);
		return FALSE;
	}
	
	//step2: check for running.
	HANDLE hMutex = CreateMutex(NULL, FALSE, SINSTAR3_MUTEX);
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(hMutex);
		if (MessageBox(GetActiveWindow(), _T("输入法正在使用，不能安装，查看哪些程序在使用吗？"), _T("提示"), MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
		{
			ShowCaller(SINSTAR3_CORE);
		}
		return 0;
	}
	CloseHandle(hMutex);


	//step3: write reg
	CRegKey reg;
	LONG ret = reg.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\SetoutSoft\\sinstar3"), KEY_WRITE | KEY_WOW64_64KEY);
	if (ret != ERROR_SUCCESS)
	{
		ret = reg.Create(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\SetoutSoft\\sinstar3"), 0, 0, KEY_WRITE | KEY_WOW64_64KEY);
		if (ret != ERROR_SUCCESS)
		{
			MessageBox(GetActiveWindow(), _T("打开注册表失败"), _T("uninstall"), MB_OK | MB_ICONSTOP);
			return 0;
		}
	}
	{
		reg.SetStringValue(_T("path_client"), g_szPath);
		reg.SetStringValue(_T("path_svr"), szSvrExe);
		reg.SetStringValue(_T("path_svr_data"), szSvrData);
		reg.Close();
	}

	//step4:修改2.x的服务器路径
	_stprintf(szPath1, _T("%s\\sinstar2.ini"), szSysPath);
	if (GetFileAttributes(szPath1) != INVALID_FILE_ATTRIBUTES)
	{
		WritePrivateProfileString(_T("server"), _T("exe"), szSvrExe, szPath1);
		WritePrivateProfileString(_T("server"), _T("data"), szSvrData, szPath1);
		if (Is64OS())
		{
			_stprintf(szPath1, _T("%s\\sinstar2.ini"), szSysWow64);
			WritePrivateProfileString(_T("server"), _T("exe"), szSvrExe, szPath1);
			WritePrivateProfileString(_T("server"), _T("data"), szSvrData, szPath1);
		}
	}

	//step5: copy ime file
	if(Is64OS())
	{
		//复制64位版本到系统目录
		_stprintf(szPath1,_T("%s\\program\\x64\\%s.dll"),g_szPath,pszIme);
		_stprintf(szPath2, _T("%s\\%s.ime"), szSysPath, pszIme);
		MyCopyFile(szPath1, szPath2);
		//复制32位版本到wow64目录
		_stprintf(szPath1,_T("%s\\program\\%s.dll"),g_szPath,pszIme);
		_stprintf(szPath2, _T("%s\\%s.ime"), szSysWow64, pszIme);
		MyCopyFile(szPath1, szPath2);
	}else
	{
		_stprintf(szPath1, _T("%s\\program\\%s.dll"), g_szPath, pszIme);
		_stprintf(szPath2, _T("%s\\%s.ime"), szSysPath, pszIme);
		MyCopyFile(szPath1, szPath2);
	}
	//step6:安装输入法
	_stprintf(szPath1,_T("%s\\%s.ime"),szSysPath,pszIme);
	if (0 != CallRegsvr32(szPath1, TRUE))
	{
		MessageBox(GetActiveWindow(), _T("注册失败"), _T("install"), MB_OK | MB_ICONSTOP);
		return 0;
	}

	//step7:修改配置文件属性
	_stprintf(szPath1,_T("%s\\data\\config.ini"),g_szPath);
	Helper_SetFileACL(szPath1);
	Helper_SetFileACLEx(szSvrData,TRUE);

	return TRUE;
}

void Helper_CenterWindow(HWND hWnd,UINT uFlag)
{
	RECT rc,screenrc;
	int x,y;
	SystemParametersInfo(SPI_GETWORKAREA,
		0,
		&screenrc,
		0);
	GetWindowRect(hWnd,&rc);
	x=(screenrc.right-rc.right+rc.left)/2;
	y=(screenrc.bottom-rc.bottom+rc.top)/2;
	SetWindowPos(hWnd,NULL,x,y,0,0,SWP_NOSIZE|SWP_NOZORDER|uFlag);
}


BOOL CALLBACK DlgProc_Option(
  HWND hwndDlg,  // handle to dialog box
  UINT uMsg,     // message
  WPARAM wParam, // first message parameter
  LPARAM lParam  // second message parameter
)
{
	static BOOL bCancel=FALSE;
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			DLGPARAM *pdlgParam=(DLGPARAM*)lParam;
			SetDlgItemText(hwndDlg,IDC_NAME,pdlgParam->szName);
			HICON hIcon=LoadIcon(pdlgParam->hInst,MAKEINTRESOURCE(IDI_FLAG));
			SendMessage(hwndDlg, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
			SendMessage(hwndDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
			Helper_CenterWindow(hwndDlg,0);
		}
		break;
	case WM_COMMAND:
		if(HIWORD(wParam)==BN_CLICKED)
		{
			switch(LOWORD(wParam))
			{
			case IDC_INSTALL:
				{
					TCHAR szIme[]=_T("sinstar3_ime"),szName[100];
					GetDlgItemText(hwndDlg,IDC_NAME,szName,100);
					EndDialog(hwndDlg,Sinstar_Install(szName,szIme));
				}
				break;
			case IDC_UNINSTALL:
				{
					TCHAR szIme[] = _T("sinstar3_ime");
					EndDialog(hwndDlg,Sinstar_Uninstall(szIme));
				}
				break;
			case IDC_UPDATE:
				{
					TCHAR szIme[] = _T("sinstar3_ime");
					EndDialog(hwndDlg,Sinstar_Update(szIme));
				}
				break;
			case IDCANCEL:
				bCancel=TRUE;
				EndDialog(hwndDlg,IDCANCEL);
				break;
			}
		}
		break;
	}
	return FALSE;
}

typedef BOOL (WINAPI *FunWow64DisableWow64FsRedirection)(
	PVOID *OldValue
);


int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
	int nRet=0;
	DLGPARAM dlgParam={hInstance,0};
	//获得程序启动目录
	TCHAR szFilePath[MAX_PATH];
	int nLen=GetModuleFileName(NULL,szFilePath,MAX_PATH);
	while(szFilePath[nLen]!='\\') nLen--;
	szFilePath[nLen]=0;
	_tcscpy(g_szPath,szFilePath);

	HMODULE hMod = LoadLibrary(_T("Kernel32.dll"));
	FunWow64DisableWow64FsRedirection fWow64DisableWow64FsRedirection = (FunWow64DisableWow64FsRedirection)GetProcAddress(hMod, "Wow64DisableWow64FsRedirection");
	if (fWow64DisableWow64FsRedirection)
	{
		PVOID pData=NULL;
		fWow64DisableWow64FsRedirection(&pData);
	}
	FreeLibrary(hMod);

	TCHAR szImeCorePath[MAX_PATH];
	_stprintf(szImeCorePath, _T("program\\%s"), SINSTAR3_CORE);
	if(!QueryImeProp(szImeCorePath,dlgParam.szName))
	{
		MessageBox(GetActiveWindow(),_T("查询输入法文件信息失败"),_T("提示"),MB_OK|MB_ICONSTOP);
		return 1;
	}

	_tcscpy(dlgParam.szIme, SINSTARE_IME);
	if(__argc==1)
	{
		nRet=DialogBoxParam(hInstance,MAKEINTRESOURCE(IDD_OPTION),GetActiveWindow(),DlgProc_Option,(LPARAM)&dlgParam);
	}else if(__argc==2 && (__targv[1][0]==_T('/') || __targv[1][0]==_T('-')))
	{
		switch(__targv[1][1])
		{
		case _T('i'):case _T('I'):
			nRet=Sinstar_Install(dlgParam.szName,dlgParam.szIme);
			break;
		case _T('u'):case _T('U'):
			nRet=Sinstar_Uninstall(dlgParam.szIme);
			break;
		case _T('c'):case _T('C'):
			nRet=Sinstar_Update(dlgParam.szIme);
			break;
		default:
			MessageBox(GetActiveWindow(),_T("无效的参数"),_T("提示"),MB_OK|MB_ICONSTOP);
			nRet=1;
			break;
		}
	}
	return nRet;
}

