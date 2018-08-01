#pragma once

#define SINSTART3_WNDCLASS _T("sinstar3_wndclass")

class CSouiEnv : public SSingleton<CSouiEnv>
{
public:
	CSouiEnv(HINSTANCE hInst);
	~CSouiEnv(void);

	SApplication * theApp() {return m_theApp;}
	SComMgr * theComMgr() {return m_pComMgr;}
private:
	SApplication	* m_theApp;
	SComMgr	*		  m_pComMgr;
};
