// dllcaller.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "dllcaller.h"
#include "sarray.hpp"
#include <tlhelp32.h>
#include <stdlib.h> 

BOOL ProcessModuleDetect(DWORD dwProcID,LPCTSTR pszModuleName,LPCTSTR pszProcFile,LPTSTR pszProcPath)
{
	BOOL bRet=FALSE;
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me32;

	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcID);

	if (hModuleSnap == INVALID_HANDLE_VALUE)
	{
		return (FALSE);
	}

	me32.dwSize = sizeof(MODULEENTRY32);

	if (!Module32First (hModuleSnap, &me32))
	{
		return (FALSE);
	}

	BOOL bFillPath = FALSE;
	do
	{
		if(0==lstrcmpi(pszProcFile,me32.szModule)) 
		{
			_tcscpy(pszProcPath,me32.szExePath);
			bFillPath = TRUE;
			if(bRet) break;
		}
		if (0==lstrcmpi(pszModuleName, me32.szModule)) 
		{
			bRet=TRUE;
			if(bFillPath) break;
		}
	} while (Module32Next(hModuleSnap, &me32));

	CloseHandle(hModuleSnap);

	return bRet;
}

 BYTE * GetCaller(LPCTSTR pszDllPath)
{
	CSArray<CALLERINFO> arrCaller;
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;
	CALLERINFO ci;
	TCHAR szProcPath[MAX_PATH];
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return (NULL);
	}

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);
		return (NULL);
	}

	do
	{
		szProcPath[0]=0;
		if(pe32.th32ProcessID!=0 && ProcessModuleDetect(pe32.th32ProcessID,pszDllPath,pe32.szExeFile,szProcPath))
		{
			_tcscpy(ci.szFile,pe32.szExeFile);
			_tcscpy(ci.szPath,szProcPath);
			ci.dwProcID=pe32.th32ProcessID;
			arrCaller.Add(ci);
		}
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);

	int nProcessCount=arrCaller.GetSize();
	if(nProcessCount==0) return NULL;

	DWORD nBufLen = sizeof(nProcessCount);
	for (int i = 0; i < nProcessCount; i++)
	{
		PCALLERINFO pi = arrCaller.GetPtAt(i);
		nBufLen += sizeof(pi->dwProcID);
		short sLen = (short)_tcslen(pi->szFile);
		nBufLen += sizeof(short);
		nBufLen += sLen * sizeof(TCHAR);
		sLen = (short)_tcslen(pi->szPath);
		nBufLen += sizeof(short);
		nBufLen += sLen * sizeof(TCHAR);
	}
	
	BYTE * pBuf = (BYTE*)malloc(nBufLen+sizeof(DWORD));

	BYTE *p = pBuf;
	memcpy(pBuf, &nBufLen, sizeof(nBufLen));
	p += sizeof(nBufLen);

	memcpy(p, &nProcessCount, sizeof(nProcessCount));
	p += sizeof(nProcessCount);
	for (int i = 0; i < nProcessCount; i++)
	{
		PCALLERINFO pi = arrCaller.GetPtAt(i);
		memcpy(p, &pi->dwProcID, sizeof(pi->dwProcID));
		p += sizeof(pi->dwProcID);
		short sLen = (short)_tcslen(pi->szFile);
		memcpy(p, &sLen, sizeof(short));
		p += sizeof(short);
		memcpy(p, pi->szFile, sLen * sizeof(TCHAR));
		p += sLen * sizeof(TCHAR);

		sLen = (short)_tcslen(pi->szPath);
		memcpy(p, &sLen, sizeof(short));
		p += sizeof(short);
		memcpy(p, pi->szPath, sLen * sizeof(TCHAR));
		p += sLen * sizeof(TCHAR);
	}
	return pBuf;
}

 void FreeCallerInfo(BYTE * pci)
{
	if(pci)	free(pci);
}

