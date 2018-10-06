#pragma once
#include "../include/sinstar-i.h"

class CCoreLoader
{
public:
	static CCoreLoader & GetInstance();

	ISinstar *Sinstar3_Create(ITextService *pTxtSvr, HWND hParentWnd=NULL);
	BOOL Sinstar3_Delete(ISinstar * pSinstar3);
	BOOL Sinstar3_OpenConfig(HWND hParent);
	void Sinstar3_SetHostInfo(HostInfo *pHostInfo);
	void SetCorePath(LPCTSTR pszPath);
private:
	CCoreLoader(void);
	~CCoreLoader(void);

	TCHAR	m_szPath[MAX_PATH];
	CRITICAL_SECTION	m_cs;
};
