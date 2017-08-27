#pragma once
#include "../include/module_ref.h"

class CClassFactory;

class CTsfModule : public CModuleRef
{
public:
	CTsfModule(HINSTANCE hInst, LPCTSTR pszDataPath);
	~CTsfModule(void);

	LPCTSTR GetDataPath() const{return m_szDataPath;}

	virtual void OnInit();

	virtual void OnFinalRelease();

	void BuildClassFactory();

	HRESULT GetClassObject(REFCLSID rclsid, REFIID riid, void **ppvObj);
protected:

	CClassFactory *m_classFactory;

	TCHAR	m_szDataPath[MAX_PATH];
};


extern CTsfModule *theModule;