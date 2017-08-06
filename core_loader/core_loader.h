#pragma once
#include "../include/sinstar-i.h"

class CBaiduIMEModule
{
public:
	static CBaiduIMEModule & GetInstance();

	ISinstar *BaiduJP3_Create(ITextService *pTxtSvr,HINSTANCE hInst);
	BOOL BaiduJP3_Delete(ISinstar * pBaiduJP3);
	BOOL BaiduJP3_OpenConfig(HWND hParent);
	void SetBaiduJP3Path(LPCTSTR pszPath);
private:
	CBaiduIMEModule(void);
	~CBaiduIMEModule(void);

	TCHAR	m_szPath[MAX_PATH];
};
