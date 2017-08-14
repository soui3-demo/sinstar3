#pragma once

class CSouiEnv : public SSingleton<CSouiEnv>
{
public:
	CSouiEnv(HINSTANCE hInst);
	~CSouiEnv(void);

	SApplication * theApp() {return m_theApp;}
private:
	SApplication	* m_theApp;
	SComMgr	*		  m_pComMgr;
};
