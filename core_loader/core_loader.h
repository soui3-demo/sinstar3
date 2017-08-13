#pragma once
#include "../include/sinstar-i.h"

class CCoreLoader
{
public:
	static CCoreLoader & GetInstance();

	ISinstar *Sinstar3_Create(ITextService *pTxtSvr,HINSTANCE hInst);
	BOOL Sinstar3_Delete(ISinstar * pSinstar3);
	BOOL Sinstar3_OpenConfig(HWND hParent);
	void SetCorePath(LPCTSTR pszPath);
private:
	CCoreLoader(void);
	~CCoreLoader(void);

	TCHAR	m_szPath[MAX_PATH];
	CRITICAL_SECTION	m_cs;
};
