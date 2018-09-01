#pragma warning (disable:4996)
#include <windows.h>
#include <stdarg.h>
#include <wingdi.h>
#include <stdio.h>
#include <tchar.h>
#include <AclAPI.h>

#include "helper.h"

char * g_pszLogFile=NULL;
BOOL	g_bRecordLog=TRUE;

BOOL Helper_SetFileACL(LPCTSTR pszPath)
{
	HANDLE hDir = CreateFile(pszPath, READ_CONTROL | WRITE_DAC, 0, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (hDir == INVALID_HANDLE_VALUE)
		return FALSE;

	ACL* pOldDACL;
	SECURITY_DESCRIPTOR* pSD = NULL;
	GetSecurityInfo(hDir, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, &pOldDACL, NULL, (void**)&pSD);

	PSID pSid = NULL;
	SID_IDENTIFIER_AUTHORITY authNt = SECURITY_NT_AUTHORITY;
	AllocateAndInitializeSid(&authNt, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_USERS, 0, 0, 0, 0, 0, 0, &pSid);

	EXPLICIT_ACCESS ea = { 0 };
	ea.grfAccessMode = GRANT_ACCESS;
	ea.grfAccessPermissions = GENERIC_ALL;
	ea.grfInheritance = CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE;
	ea.Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea.Trustee.ptstrName = (LPTSTR)pSid;

	ACL* pNewDACL = 0;
	DWORD err = SetEntriesInAcl(1, &ea, pOldDACL, &pNewDACL);

	if (pNewDACL)
		SetSecurityInfo(hDir, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, pNewDACL, NULL);

	FreeSid(pSid);
	LocalFree(pNewDACL);
	LocalFree(pSD);
	LocalFree(pOldDACL);
	CloseHandle(hDir);

	return TRUE;

}

LPCWSTR LOW_INTEGRITY_SDDL_SACL_W = L"S:(ML;;NW;;;LW)";
#define LABEL_SECURITY_INFORMATION (0x00000010L)
BOOL Helper_SetObjectToLowIntegrity(HANDLE hObject, SE_OBJECT_TYPE type)
{
	BOOL bRet = FALSE;
	DWORD dwErr = ERROR_SUCCESS;
	PSECURITY_DESCRIPTOR pSD = NULL;
	PACL pSacl = NULL;
	BOOL fSaclPresent = FALSE;
	BOOL fSaclDefaulted = FALSE;
	if (ConvertStringSecurityDescriptorToSecurityDescriptorW(
		LOW_INTEGRITY_SDDL_SACL_W, SDDL_REVISION_1, &pSD, NULL))
	{
		if (GetSecurityDescriptorSacl(
			pSD, &fSaclPresent, &pSacl, &fSaclDefaulted))
		{
			dwErr = SetSecurityInfo(hObject, type, LABEL_SECURITY_INFORMATION, NULL, NULL, NULL, pSacl);

			bRet = (ERROR_SUCCESS == dwErr);
		}

		LocalFree(pSD);
	}

	return bRet;
}

BOOL Helper_SetFileACLEx(LPCTSTR pszPath, BOOL bSubFile)
{
	SECURITY_DESCRIPTOR* pSD = NULL;

	PSID pSid = NULL;
	SID_IDENTIFIER_AUTHORITY authNt = SECURITY_NT_AUTHORITY;
	AllocateAndInitializeSid(&authNt, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_USERS, 0, 0, 0, 0, 0, 0, &pSid);

	EXPLICIT_ACCESS ea = { 0 };
	ea.grfAccessMode = GRANT_ACCESS;
	ea.grfAccessPermissions = GENERIC_ALL;
	ea.grfInheritance = CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE;
	ea.Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea.Trustee.ptstrName = (LPTSTR)pSid;

	ACL* pNewDACL = 0;
	DWORD err = SetEntriesInAcl(1, &ea, NULL, &pNewDACL);

	if (pNewDACL)
	{
		HANDLE hFile = CreateFile(pszPath, READ_CONTROL | WRITE_DAC, 0, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
		if (hFile != INVALID_HANDLE_VALUE) SetSecurityInfo(hFile, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, pNewDACL, NULL);
		CloseHandle(hFile);
		if (bSubFile)
		{
			WIN32_FIND_DATA wfd = { 0 };
			TCHAR szPath[MAX_PATH], szFilter[MAX_PATH];
			_stprintf(szFilter, _T("%s\\*.*"), pszPath);
			HANDLE hFind = FindFirstFile(szFilter, &wfd);
			if (hFind)
			{
				do
				{
					if (!(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
					{
						_stprintf(szPath, _T("%s\\%s"), pszPath, wfd.cFileName);
						HANDLE hFile = CreateFile(szPath, READ_CONTROL | WRITE_DAC, 0, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
						if (hFile != INVALID_HANDLE_VALUE) SetSecurityInfo(hFile, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, pNewDACL, NULL);
						CloseHandle(hFile);
					}
				} while (FindNextFile(hFind, &wfd));
				FindClose(hFind);
			}
		}
	}
	FreeSid(pSid);
	LocalFree(pNewDACL);
	LocalFree(pSD);

	return TRUE;

}

void Helper_TraceA(char * pszFormat,...)
{
	if(g_bRecordLog)
	{
		va_list args; 
		char buf[1024];
		va_start( args, pszFormat );
		_vsnprintf(buf,sizeof(buf)-1, pszFormat,args);
		va_end (args);
		buf[sizeof(buf)-1]=0;
		OutputDebugStringA(buf);
		if(g_pszLogFile)
		{
			FILE *f=fopen(g_pszLogFile,"a+");
			if(f)
			{
				fprintf(f,buf);
				fclose(f);
			}
		}
	}
}


void Helper_TraceW(wchar_t * pszFormat, ...)
{
	if (g_bRecordLog)
	{
		va_list args;
		wchar_t buf[1024];
		va_start(args, pszFormat);
		_vsnwprintf(buf, sizeof(buf)/2 - 1, pszFormat, args);
		va_end(args);
		buf[sizeof(buf) - 1] = 0;
		OutputDebugStringW(buf);
		if (g_pszLogFile)
		{
			FILE *f = fopen(g_pszLogFile, "a+");
			if (f)
			{
				fwprintf(f, buf);
				fclose(f);
			}
		}
	}
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

//获取文件的版本号
void Helper_VersionString(DWORD dwVer,TCHAR *pszBuf)
{
	_stprintf(pszBuf,_T("%u.%u.%u.%u"),(dwVer>>24)&0xFF,(dwVer>>16)&0xFF,(dwVer>>8)&0xFF,dwVer&0xFF);
}

//获取一个PE文件的version + ProductName +  FileDescription
BOOL Helper_PEVersion(LPCTSTR pszFileName, DWORD *pdwVer, TCHAR *pszName, TCHAR *pszDesc)
{
	DWORD dwResHandle;
	LPSTR lszVer = NULL;
	UINT  cchVer = 0;
	void *pBuf;
	BOOL bRet = FALSE;
	DWORD dwVerInfoSize = GetFileVersionInfoSize(pszFileName, &dwResHandle);
	if (!dwVerInfoSize) return FALSE;
	pBuf = malloc(dwVerInfoSize);
	GetFileVersionInfo(pszFileName, dwResHandle, dwVerInfoSize, pBuf);
	if (pdwVer)
	{
		UINT nVersionLen;
		VS_FIXEDFILEINFO *pstFileVersion;
		if (VerQueryValue(pBuf, _T("\\"), (void**)&pstFileVersion, &nVersionLen) && nVersionLen >= sizeof(VS_FIXEDFILEINFO))
		{
			*pdwVer = MAKELONG(MAKEWORD(LOWORD(pstFileVersion->dwFileVersionLS), HIWORD(pstFileVersion->dwFileVersionLS)),
				MAKEWORD(LOWORD(pstFileVersion->dwFileVersionMS), HIWORD(pstFileVersion->dwFileVersionMS)));
		}
	}

	bRet = VerQueryValue(pBuf, _T("\\VarFileInfo\\Translation"), (void**)&lszVer, &cchVer);
	if (bRet&&cchVer == 4)
	{
		TCHAR szLangCode[20] = { 0 };
		TCHAR szSection[256];
		BOOL bOK;
		_stprintf(szLangCode, _T("%04x"), *(WORD*)lszVer);
		_stprintf(szLangCode + 4, _T("%04x"), *(WORD*)(lszVer + 2));
		if (pszName)
		{
			_stprintf(szSection, _T("\\StringFileInfo\\%s\\ProductName"), szLangCode);
			bOK = VerQueryValue(pBuf, szSection, (void**)&lszVer, &cchVer);
			if (bOK) _tcscpy(pszName, (LPCTSTR)lszVer);
		}
		if (pszDesc)
		{
			_stprintf(szSection, _T("\\StringFileInfo\\%s\\FileDescription"), szLangCode);
			bOK = VerQueryValue(pBuf, szSection, (void**)&lszVer, &cchVer);
			if (bOK) _tcscpy(pszDesc, (LPCTSTR)lszVer);
		}
	}
	free(pBuf);
	return bRet;
}

//滑过空格及制表符
static char * StrSkipSpace(const char *pszBuf)
{
	char *p=(char*)pszBuf;
	while(*p)
	{
		switch(*p)
		{
		case 0x20:
		case 0x09:
			p++;
		default:
			return p;
		}
	}
	return p;
}

BOOL Helper_ParseIniLine(LPCSTR pszLine,LPSTR pszKey,LPSTR pszData)
{
	int i=0;
	char *p;
	while(pszLine[i])
	{
		if(pszLine[i]&0x80)
			i+=2;
		else if(pszLine[i]==0x20 || pszLine[i]==0x09 || pszLine[i]=='=')
			break;
		else
			i++;
	}
	if(!pszLine[i])	return FALSE;
	p=StrSkipSpace(pszLine+i);
	if(!p || p[0]!='=') return FALSE;
	strncpy(pszKey,pszLine,i);
	pszKey[i]=0;
	p=StrSkipSpace(p+1);
	strcpy(pszData,p);
	return TRUE;
}

void Helper_WriteProfileSection(LPCSTR pszSection,LPCSTR pszKeys,LPCSTR pszIniFile)
{
	char szKey[100],szData[500];
	char *p=(char*)pszKeys;
	while(p[0])
	{
		if(Helper_ParseIniLine(p,szKey,szData)) WritePrivateProfileStringA(pszSection,szKey,szData,pszIniFile);
		p+=strlen(p)+1;
	}
}

DWORD Helper_GetProfileSectionString(
  LPCSTR lpKeyName,        // points to key name
  LPCSTR lpDefault,        // points to default string
  LPSTR lpReturnedString,  // points to destination buffer
  DWORD nSize,              // size of destination buffer
  LPCSTR lpSectionData     // points to initialization data string list
)
{
	LPCSTR p=lpSectionData;
	if(p)
	{
		int nKeyLen=strlen(lpKeyName);
		while(*p)
		{
			if(strncmp(lpKeyName,p,nKeyLen)==0)
			{
				p=StrSkipSpace(p+nKeyLen);
				if(p[0]=='=')
				{
					UINT nLen=0;
					p=StrSkipSpace(p+1);
					nLen=strlen(p);
					if(nLen>=nSize) return nSize-2;
					strcpy(lpReturnedString,p);
					return nLen;
				}
			}
			p+=strlen(p)+1;
		}
	}
	if(lpDefault)
		strcpy(lpReturnedString,lpDefault);
	else
		strcpy(lpReturnedString,"");
	return strlen(lpReturnedString);
}

int Helper_GetProfileSectionInt( 
  LPCSTR lpKeyName,        // points to key name
  int nDefault,				// return value if key name is not found
  LPCSTR lpSectionData     // points to initialization data string list
)
{
	char szBuf[50];
	int nRet=Helper_GetProfileSectionString(lpKeyName,NULL,szBuf,50,lpSectionData);
	if(nRet==0||nRet==48) return nDefault;
	return atoi(szBuf);
}

BOOL WritePrivateProfileInt(
  LPCSTR lpAppName,  // pointer to section name
  LPCSTR lpKeyName,  // pointer to key name
  int nKeyValue,   // value to write
  LPCSTR lpFileName  // pointer to initialization filename
)
{
	char szBuf[100];
	sprintf(szBuf,"%d",nKeyValue);
	return WritePrivateProfileStringA(lpAppName,lpKeyName,szBuf,lpFileName);
}

typedef BOOL (WINAPI *FunSetLayeredWindowAttributes)(HWND,COLORREF,BYTE,DWORD);

void Helper_SetWindowLayerAttributes(HWND hWnd,BYTE byTrans)
{

	HMODULE hUser32 = GetModuleHandle(_T("user32.dll")); 
	if(hUser32)
	{
		FunSetLayeredWindowAttributes funSetLayer=(FunSetLayeredWindowAttributes)GetProcAddress(hUser32,"SetLayeredWindowAttributes"); 
		funSetLayer(hWnd,0,byTrans,0x02);
	}
}

typedef BOOL (WINAPI *FunChangeMsgFilter)(UINT,DWORD);

BOOL Helper_ChangeWindowMessageFilter(UINT message, DWORD dwFlag)
{
	HMODULE hUser32=GetModuleHandle(_T("user32.dll"));
	if(hUser32)
	{
		FunChangeMsgFilter fMsgFilter=(FunChangeMsgFilter)GetProcAddress(hUser32,"ChangeWindowMessageFilter");
		if(!fMsgFilter) return FALSE;
		return fMsgFilter(message,dwFlag);
	}else
	{
		return FALSE;
	}
}

BOOL Helper_IsLayerSupport()
{
	OSVERSIONINFO     ver; 
	ver.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	GetVersionEx(&ver);
	return ver.dwMajorVersion>=5;
}

