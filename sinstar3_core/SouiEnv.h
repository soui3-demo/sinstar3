#pragma once

class CSouiEnv : public SSingleton<CSouiEnv>
{
public:
	CSouiEnv(HINSTANCE hInst);
	~CSouiEnv(void);

private:
	SApplication	* m_theApp;
	CAutoRefPtr<IObjRef> m_imgDecoder;
	CAutoRefPtr<IObjRef> m_render;
};
