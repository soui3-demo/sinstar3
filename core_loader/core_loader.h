#pragma once
#include "../include/sinstar-i.h"

class CCoreLoader
{
public:
	static CCoreLoader & GetInstance();

	ISinstar *Sinstar3_Create(ITextService *pTxtSvr,HINSTANCE hInst);
	BOOL Sinstar3_Delete(ISinstar * pBaiduJP3);
	BOOL BaiduJP3_OpenConfig(HWND hParent);
	void SetBaiduJP3Path(LPCTSTR pszPath);
private:
	CCoreLoader(void);
	~CCoreLoader(void);

	TCHAR	m_szPath[MAX_PATH];
	CRITICAL_SECTION	m_cs;
};
