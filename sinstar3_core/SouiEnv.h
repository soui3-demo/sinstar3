#pragma once

class CSouiEnv : public SSingleton<CSouiEnv>
{
public:
	CSouiEnv(HINSTANCE hInst);
	~CSouiEnv(void);

private:
	SApplication	* m_theApp;
	SComMgr			* m_pComMgr;
};
