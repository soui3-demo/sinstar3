#pragma once

#include <Sddl.h>
#include <AccCtrl.h>

#ifdef __cplusplus
extern "C"{
#endif//__cplusplus

extern char * g_pszLogFile;
extern BOOL	g_bRecordLog;
BOOL Helper_ProcessHasLoadDll(const TCHAR* dllname);
BOOL Helper_SetFileACL(LPCTSTR pszPath);
BOOL Helper_SetFileACLEx(LPCTSTR pszPath, BOOL bSubFile);
BOOL Helper_SetObjectToLowIntegrity(HANDLE hObject, SE_OBJECT_TYPE type);

SECURITY_ATTRIBUTES * Helper_BuildLowIntegritySA();
void Helper_FreeSa(SECURITY_ATTRIBUTES *psa);

void Helper_TraceW(wchar_t * pszFormat,...);
void Helper_TraceA(char * pszFormat,...);

void Helper_CenterWindow(HWND hWnd,UINT uFlag);

BOOL Helper_ParseIniLine(LPCSTR pszLine, LPSTR pszKey, LPSTR pszData);

#pragma comment(lib,"version.lib")
void Helper_VersionString(DWORD dwVer,TCHAR *pszBuf);

BOOL Helper_PEVersion(LPCTSTR pszFileName, DWORD *pdwVer, TCHAR *pszName, TCHAR *pszDesc);

bool Helper_IsWin8orLater();

BOOL Helper_ParseIniLine(LPCSTR pszLine,LPSTR pszKey,LPSTR pszData);
void Helper_WriteProfileSection(LPCSTR pszSection,LPCSTR pszKeys,LPCSTR pszIniFile);

DWORD Helper_GetProfileSectionString(
  LPCSTR lpKeyName,        // points to key name
  LPCSTR lpDefault,        // points to default string
  LPSTR lpReturnedString,  // points to destination buffer
  DWORD nSize,              // size of destination buffer
  LPCSTR lpSectionData     // points to initialization data string list
);

int Helper_GetProfileSectionInt( 
  LPCSTR lpKeyName,        // points to key name
  int nDefault,				// return value if key name is not found
  LPCSTR lpSectionData     // points to initialization data string list
);

BOOL WritePrivateProfileInt(
  LPCTSTR lpAppName,  // pointer to section name
  LPCTSTR lpKeyName,  // pointer to key name
  int nKeyValue,   // value to write
  LPCTSTR lpFileName  // pointer to initialization filename
);

BOOL Helper_IsLayerSupport();

#ifndef MSGFLT_ADD
#define MSGFLT_ADD 1
#define MSGFLT_REMOVE 2
#endif//MSGFLT_ADD
BOOL Helper_ChangeWindowMessageFilter(UINT message, DWORD dwFlag);

#ifdef __cplusplus
}
#endif//__cplusplus
