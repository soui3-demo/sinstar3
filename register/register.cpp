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

#include <accctrl.h>
#include <aclapi.h>


#define MAX_STRLEN		100

#define NAME_MAPFILE _T("_setoutsoft comm map file")

TCHAR g_szPath[MAX_PATH]={0};	//程序启动位置
HINSTANCE g_hInst = 0;

const TCHAR * KFiles[]=
{
	_T("program\\isserver3.exe"),
	_T("program\\sinstar3_ime.dll"),
	_T("program\\x64\\sinstar3_ime.dll"),
	_T("program\\sinstar3_tsf.dll"),
	_T("program\\x64\\sinstar3_tsf.dll"),
};

struct CopyInfo{
	TCHAR *pszSrc;
	TCHAR *pszDest;
	bool   bReg;
};

const CopyInfo KSrcX86Files[]=
{
	{_T("program\\sinstar3_ime.dll"),_T("sinstar3_ime.ime"),true},
	{_T("program\\sinstar3_tsf.dll"),_T("sinstar3_tsf.dll"),true},
};

const CopyInfo KSrcX64Files[]=
{
	{_T("program\\x64\\sinstar3_ime.dll"),_T("sinstar3_ime.ime"),false},
	{_T("program\\x64\\sinstar3_tsf.dll"),_T("sinstar3_tsf.dll"),true},
};


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


void ShowCaller()
{
	TCHAR szPath[MAX_PATH];
	STARTUPINFO si={sizeof(STARTUPINFO),0};
	PROCESS_INFORMATION pi={0};
	_stprintf(szPath,_T("%s\\program\\findcallerUI.exe sinstar3_ime.ime|sinstar3_tsf.dll"), g_szPath);
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

BOOL Sinstar_Update(bool bSilent)
{
	TCHAR szPath1[MAX_PATH],szPath2[MAX_PATH],szSysPath[MAX_PATH],szSysWow64[MAX_PATH];
	::GetSystemDirectory(szSysPath,MAX_PATH);
	GetSysWow64Dir(szSysWow64, MAX_PATH);

	HANDLE hMutex = CreateMutex(NULL, FALSE, SINSTAR3_MUTEX);
	if(GetLastError()==ERROR_ALREADY_EXISTS)
	{
		CloseHandle(hMutex);
		if (!bSilent && MessageBox(GetActiveWindow(), _T("输入法正在使用，不能更新，查看哪些程序在使用吗？"), _T("提示"), MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
		{
			ShowCaller();
		}
		return 0;
	}
	CloseHandle(hMutex);
	
	BOOL bCopied = TRUE;
	if (Is64OS())
	{
		//copy x64 files to system dir
		for(int i=0;i<ARRAYSIZE(KSrcX64Files) && bCopied;i++)
		{
			_stprintf(szPath1, _T("%s\\%s"), g_szPath, KSrcX64Files[i].pszSrc);
			_stprintf(szPath2, _T("%s\\%s"), szSysPath, KSrcX64Files[i].pszDest);
			bCopied = CopyFile(szPath1, szPath2, FALSE);
		}

		//copy x86 files to wow64 dir.
		for(int i=0;i<ARRAYSIZE(KSrcX86Files) && bCopied;i++)
		{
			_stprintf(szPath1, _T("%s\\%s"), g_szPath, KSrcX86Files[i].pszSrc);
			_stprintf(szPath2, _T("%s\\%s"), szSysWow64, KSrcX86Files[i].pszDest);
			bCopied = CopyFile(szPath1, szPath2, FALSE);
		}
	}
	else
	{
		//copy x86 files to system dir
		for(int i=0;i<ARRAYSIZE(KSrcX86Files) && bCopied;i++)
		{
			_stprintf(szPath1, _T("%s\\%s"), g_szPath, KSrcX86Files[i].pszSrc);
			_stprintf(szPath2, _T("%s\\%s"), szSysPath, KSrcX86Files[i].pszDest);
			bCopied = CopyFile(szPath1, szPath2, FALSE);
		}
	}
	if(!bSilent)
	{
		if (!bCopied)
		{
			MessageBox(GetActiveWindow(), _T("Copy File Failed"), _T("update"), MB_OK);
		}
		else
		{
			MessageBox(GetActiveWindow(), _T("更新成功！"), _T("install"), MB_OK | MB_ICONINFORMATION);
		}
	}
	return bCopied;
}

BOOL Sinstar_Uninstall()
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
			ShowCaller();
		}
		return 0;
	}
	CloseHandle(hMutex);
	if(MessageBox(GetActiveWindow(),_T("确定卸载吗？"),_T("提示"),MB_OKCANCEL|MB_ICONQUESTION)==IDCANCEL) 
		return 0;

	//删除输入法文件
	if (Is64OS())
	{
		for(int i=0;i<ARRAYSIZE(KSrcX64Files);i++)
		{
			_stprintf(szPath, _T("%s\\%s.ime"), szSysPath, KSrcX64Files[i].pszDest);
			if (KSrcX64Files[i].bReg && 0 != CallRegsvr32(szPath, FALSE))
			{
				MessageBox(GetActiveWindow(), _T("卸载程序失败"), _T("提示"), MB_OK | MB_ICONSTOP);
				return 0;
			}
			DeleteFile(szPath);
		}

		for(int i=0;i<ARRAYSIZE(KSrcX86Files);i++)
		{
			_stprintf(szPath, _T("%s\\%s.ime"), sysWow64, KSrcX86Files[i].pszDest);
			if (KSrcX86Files[i].bReg && 0 != CallRegsvr32(szPath, FALSE))
			{
				MessageBox(GetActiveWindow(), _T("卸载程序失败"), _T("提示"), MB_OK | MB_ICONSTOP);
				return 0;
			}
			DeleteFile(szPath);
		}
	}
	else
	{
		for(int i=0;i<ARRAYSIZE(KSrcX86Files);i++)
		{
			_stprintf(szPath, _T("%s\\%s.ime"), szSysPath, KSrcX86Files[i].pszDest);
			if (KSrcX86Files[i].bReg && 0 != CallRegsvr32(szPath, FALSE))
			{
				MessageBox(GetActiveWindow(), _T("卸载程序失败"), _T("提示"), MB_OK | MB_ICONSTOP);
				return 0;
			}
			DeleteFile(szPath);
		}
	}

	//delete reg
	CRegKey reg;
	LONG ret = reg.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\SetoutSoft"), KEY_WRITE |KEY_READ | KEY_WOW64_64KEY);
	if (ret != ERROR_SUCCESS)
	{
		MessageBox(GetActiveWindow(), _T("打开注册表失败"), _T("uninstall"), MB_OK | MB_ICONSTOP);
		return 0;
	}

	TCHAR szClient[MAX_PATH];
	ULONG uSize = MAX_PATH;
	reg.QueryStringValue(_T("path_client"),szClient,&uSize);

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
	MessageBox(GetActiveWindow(), _T("卸载成功！"), _T("install"), MB_OK | MB_ICONINFORMATION);

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

DWORD AddAceToObjectsSecurityDescriptor(
	LPTSTR pszObjName,          // name of object
	SE_OBJECT_TYPE ObjectType,  // type of object
	LPTSTR pszTrustee,          // trustee for new ACE
	TRUSTEE_FORM TrusteeForm,   // format of trustee structure
	DWORD dwAccessRights,       // access mask for new ACE
	ACCESS_MODE AccessMode,     // type of ACE
	DWORD dwInheritance         // inheritance flags for new ACE
)
{
	DWORD dwRes = 0;
	PACL pOldDACL = NULL, pNewDACL = NULL;
	PSECURITY_DESCRIPTOR pSD = NULL;
	EXPLICIT_ACCESS ea;

	if (NULL == pszObjName)
		return ERROR_INVALID_PARAMETER;

	// Get a pointer to the existing DACL.

	dwRes = GetNamedSecurityInfo(pszObjName, ObjectType,
		DACL_SECURITY_INFORMATION,
		NULL, NULL, &pOldDACL, NULL, &pSD);
	if (ERROR_SUCCESS != dwRes) {
		printf("GetNamedSecurityInfo Error %u\n", dwRes);
		goto Cleanup;
	}

	// Initialize an EXPLICIT_ACCESS structure for the new ACE. 

	ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));
	ea.grfAccessPermissions = dwAccessRights;
	ea.grfAccessMode = AccessMode;
	ea.grfInheritance = dwInheritance;
	ea.Trustee.TrusteeForm = TrusteeForm;
	ea.Trustee.ptstrName = pszTrustee;

	// Create a new ACL that merges the new ACE
	// into the existing DACL.

	dwRes = SetEntriesInAcl(1, &ea, pOldDACL, &pNewDACL);
	if (ERROR_SUCCESS != dwRes) {
		printf("SetEntriesInAcl Error %u\n", dwRes);
		goto Cleanup;
	}

	// Attach the new ACL as the object's DACL.

	dwRes = SetNamedSecurityInfo(pszObjName, ObjectType,
		DACL_SECURITY_INFORMATION,
		NULL, NULL, pNewDACL, NULL);
	if (ERROR_SUCCESS != dwRes) {
		printf("SetNamedSecurityInfo Error %u\n", dwRes);
		goto Cleanup;
	}

Cleanup:

	if (pSD != NULL)
		LocalFree((HLOCAL)pSD);
	if (pNewDACL != NULL)
		LocalFree((HLOCAL)pNewDACL);

	return dwRes;
}


BOOL Sinstar_Install()
{
	TCHAR szSysPath[MAX_PATH];
	TCHAR szSysWow64[MAX_PATH]={0};

	GetSystemDirectory(szSysPath, MAX_PATH);
	GetSysWow64Dir(szSysWow64, MAX_PATH);
	
	//step1: check for running.
	HANDLE hMutex = CreateMutex(NULL, FALSE, SINSTAR3_MUTEX);
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(hMutex);
		if (MessageBox(GetActiveWindow(), _T("输入法正在使用，不能安装，查看哪些程序在使用吗？"), _T("提示"), MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
		{
			ShowCaller();
		}
		return FALSE;
	}
	CloseHandle(hMutex);

	//step2 copy files.
	if(!Sinstar_Update(true))
	{
		MessageBox(GetActiveWindow(), _T("复制文件失败!"), _T("提示"), MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	TCHAR szSvrExe[256] = { 0 }, szSvrData[256] = { 0 };
	_stprintf(szSvrExe, _T("%s\\program\\isserver3.exe"), g_szPath);
	_stprintf(szSvrData,_T("%s\\server"),g_szPath);

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


	TCHAR szPath[1000];
	//step6:安装输入法
	if(Is64OS())
	{
		for(int i=0;i<ARRAYSIZE(KSrcX86Files);i++)
		{
			if(!KSrcX86Files[i].bReg) continue;
			_stprintf(szPath,_T("%s\\%s"),szSysWow64,KSrcX86Files[i].pszDest);
			if (0 != CallRegsvr32(szPath, TRUE))
			{
				MessageBox(GetActiveWindow(), _T("注册失败"), _T("install"), MB_OK | MB_ICONSTOP);
				return FALSE;
			}
		}

		for(int i=0;i<ARRAYSIZE(KSrcX64Files);i++)
		{
			if(!KSrcX64Files[i].bReg) continue;
			_stprintf(szPath,_T("%s\\%s"),szSysPath,KSrcX64Files[i].pszDest);
			if (0 != CallRegsvr32(szPath, TRUE))
			{
				MessageBox(GetActiveWindow(), _T("注册失败"), _T("install"), MB_OK | MB_ICONSTOP);
				return FALSE;
			}
		}
	}else
	{
		for(int i=0;i<ARRAYSIZE(KSrcX86Files);i++)
		{
			if(!KSrcX86Files[i].bReg) continue;
			_stprintf(szPath,_T("%s\\%s"),szSysPath,KSrcX86Files[i].pszDest);
			if (0 != CallRegsvr32(szPath, TRUE))
			{
				MessageBox(GetActiveWindow(), _T("注册失败"), _T("install"), MB_OK | MB_ICONSTOP);
				return FALSE;
			}
		}
	}

	//step8:修改配置文件属性
	_stprintf(szPath,_T("%s\\data\\config.ini"),g_szPath);
	Helper_SetFileACL(szPath);
	Helper_SetFileACLEx(szSvrData,TRUE);

	TCHAR szUser[]=_T("ALL APPLICATION PACKAGES");
	DWORD dc = AddAceToObjectsSecurityDescriptor(g_szPath, SE_FILE_OBJECT, szUser, TRUSTEE_IS_NAME,
		WRITE_DAC | GENERIC_ALL, SET_ACCESS, SUB_CONTAINERS_AND_OBJECTS_INHERIT);

	MessageBox(GetActiveWindow(), _T("安装成功！"), _T("install"), MB_OK | MB_ICONINFORMATION);

	return TRUE;
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
			HICON hIcon=LoadIcon(g_hInst,MAKEINTRESOURCE(IDI_FLAG));
			SendMessage(hwndDlg, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
			SendMessage(hwndDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
			Helper_CenterWindow(hwndDlg,0);

			for(int i=0;i<ARRAYSIZE(KFiles);i++)
			{
				TCHAR szBuf[1000];
				_stprintf(szBuf,_T("%s\\%s"),g_szPath,KFiles[i]);
				if(GetFileAttributes(szBuf)==INVALID_FILE_ATTRIBUTES)
				{
					_stprintf(szBuf,_T("安装目录文件[%s]没有找到，请保证安装包完整！"),KFiles[i]);
					MessageBox(hwndDlg, szBuf, _T("错误"), MB_OK | MB_ICONSTOP);
					PostQuitMessage(1);
					return 0;
				}
			}
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
					EndDialog(hwndDlg,Sinstar_Install());
				}
				break;
			case IDC_UNINSTALL:
				{
					EndDialog(hwndDlg,Sinstar_Uninstall());
				}
				break;
			case IDC_UPDATE:
				{
					EndDialog(hwndDlg,Sinstar_Update(false));
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

