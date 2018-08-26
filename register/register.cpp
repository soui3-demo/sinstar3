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
#include "../helper/helper.h"
#include "../include/version.h"
#include "../include/reg.h"

#define MAX_STRLEN		100

#define NAME_MAPFILE _T("_setoutsoft comm map file")
#define SINSTAR3_CORE	_T("sinstar3_core.dll")
#define SINSTARE_IME	_T("sinstar3_ime")

TCHAR g_szPath[MAX_PATH]={0};	//程序启动位置
HINSTANCE g_hInst = 0;



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
	ret = reg.Open(HKEY_CURRENT_USER, _T("SOFTWARE\\SetoutSoft"), KEY_WRITE | KEY_WOW64_64KEY);
	if (ret == ERROR_SUCCESS)
	{
		reg.RecurseDeleteKey(_T("sinstar3"));
		reg.Close();
	}


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

BOOL Sinstar_Install(LPCTSTR pszImeName)
{
	TCHAR szSysPath[MAX_PATH];
	TCHAR szPath1[300],szPath2[300];
	TCHAR szSysWow64[MAX_PATH]={0};

	GetSystemDirectory(szSysPath, MAX_PATH);
	GetSysWow64Dir(szSysWow64, MAX_PATH);
	
	//step1:验证文件有效性
	_stprintf(szPath1,_T("%s\\program\\%s.dll"),g_szPath, pszImeName);
	if(GetFileAttributes(szPath1)==0xFFFFFFFF)
	{
		TCHAR szMsg[100];
		_stprintf(szMsg,_T("输入法目录下没有找到输入法文件：%s"), pszImeName);
		MessageBox(GetActiveWindow(),szMsg,_T("提示"),MB_OK|MB_ICONSTOP);
		return FALSE;
	}

	TCHAR szSvrExe[256] = { 0 }, szSvrData[256] = { 0 };
	_stprintf(szSvrExe, _T("%s\\program\\isserver3.exe"), g_szPath);
	_stprintf(szSvrData,_T("%s\\server"),g_szPath);
	if(GetFileAttributes(szSvrExe)==0xFFFFFFFF)
	{
		TCHAR szMsg[100];
		_stprintf(szMsg,_T("服务器目录下没有找到isserver3.exe"));
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
		_stprintf(szPath1,_T("%s\\program\\x64\\%s.dll"),g_szPath, pszImeName);
		_stprintf(szPath2, _T("%s\\%s.ime"), szSysPath, pszImeName);
		MyCopyFile(szPath1, szPath2);
		//复制32位版本到wow64目录
		_stprintf(szPath1,_T("%s\\program\\%s.dll"),g_szPath, pszImeName);
		_stprintf(szPath2, _T("%s\\%s.ime"), szSysWow64, pszImeName);
		MyCopyFile(szPath1, szPath2);
	}else
	{
		_stprintf(szPath1, _T("%s\\program\\%s.dll"), g_szPath, pszImeName);
		_stprintf(szPath2, _T("%s\\%s.ime"), szSysPath, pszImeName);
		MyCopyFile(szPath1, szPath2);
	}
	//step6:安装输入法
	_stprintf(szPath1,_T("%s\\%s.ime"),szSysPath, pszImeName);
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

#define IME_NAME _T("sinstar3_ime")

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
			HICON hIcon=LoadIcon(g_hInst,MAKEINTRESOURCE(IDI_FLAG));
			SendMessage(hwndDlg, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
			SendMessage(hwndDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
			Helper_CenterWindow(hwndDlg,0);

			TCHAR szLicenseFile[MAX_PATH];
			_stprintf(szLicenseFile, _T("%s\\license.txt"), g_szPath);
			FILE *f = _tfopen(szLicenseFile, _T("rb"));
			if (!f)
			{
				MessageBox(hwndDlg, _T("当前目录下没有找到licnese.txt,请保证安装包完整"), _T("错误"), MB_OK | MB_ICONSTOP);
				PostQuitMessage(1);
				return 0;
			}
			BYTE utf16[2];
			fread(utf16, 1, 2, f);
			if (utf16[0] != 0xff || utf16[1] != 0xfe)
			{
				MessageBox(hwndDlg, _T("license.txt非法,请保证安装包完整"), _T("错误"), MB_OK | MB_ICONSTOP);
				PostQuitMessage(1);
				return 0;
			}
			fseek(f, 0, SEEK_END);
			int nLen = ftell(f);
			wchar_t *pLisence = new wchar_t[nLen / 2];//
			fseek(f, 2, SEEK_SET);
			fread(pLisence, 1, nLen -2, f);
			fclose(f);
			pLisence[nLen / 2 - 1] = 0;
			SetDlgItemTextW(hwndDlg, IDC_LISENCE, pLisence);
			delete[]pLisence;
			CheckDlgButton(hwndDlg, IDC_AGREE, 0);
			EnableWindow(GetDlgItem(hwndDlg, IDC_INSTALL), FALSE);
			EnableWindow(GetDlgItem(hwndDlg, IDC_UPDATE), FALSE);
			EnableWindow(GetDlgItem(hwndDlg, IDC_UNINSTALL), FALSE);
		}
		break;
	case WM_COMMAND:
		if(HIWORD(wParam)==BN_CLICKED)
		{
			switch(LOWORD(wParam))
			{
			case IDC_AGREE:
				{
					BOOL bAgree = IsDlgButtonChecked(hwndDlg, IDC_AGREE);
					EnableWindow(GetDlgItem(hwndDlg, IDC_INSTALL), bAgree);
					EnableWindow(GetDlgItem(hwndDlg, IDC_UPDATE), bAgree);
					EnableWindow(GetDlgItem(hwndDlg, IDC_UNINSTALL), bAgree);
				}
				break;
			case IDC_INSTALL:
				{
					EndDialog(hwndDlg,Sinstar_Install(IME_NAME));
				}
				break;
			case IDC_UNINSTALL:
				{
					EndDialog(hwndDlg,Sinstar_Uninstall(IME_NAME));
				}
				break;
			case IDC_UPDATE:
				{
					EndDialog(hwndDlg,Sinstar_Update(IME_NAME));
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

	nRet = DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_OPTION), GetActiveWindow(), DlgProc_Option, NULL);

	return nRet;
}

