#pragma once
#include "../include/module_ref.h"

class CClassFactory;
class CTsfModule : public CModuleRef
{
public:
	CTsfModule(HINSTANCE hInst, LPCTSTR pszSvrPath,LPCTSTR pszInstallPath);
	~CTsfModule(void);

	LPCTSTR GetSvrPath() const{return m_szSvrPath;}
	LPCTSTR GetInstallPath() const{return m_szInstallPath;}

	virtual void OnInit();

	virtual void OnFinalRelease();

	void BuildClassFactory();

	HRESULT GetClassObject(REFCLSID rclsid, REFIID riid, void **ppvObj);
protected:

	CClassFactory *m_classFactory;

	TCHAR	m_szSvrPath[MAX_PATH];
	TCHAR	m_szInstallPath[MAX_PATH];
	HANDLE	m_hMutex;
};


extern CTsfModule *theModule;