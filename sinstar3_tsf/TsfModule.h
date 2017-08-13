#pragma once
#include "../include/module_ref.h"

class CClassFactory;

class CTsfModule : public CModuleRef
{
public:
	CTsfModule(HINSTANCE hInst);
	~CTsfModule(void);

	virtual void OnInit();

	virtual void OnFinalRelease();

	void BuildClassFactory();

	HRESULT GetClassObject(REFCLSID rclsid, REFIID riid, void **ppvObj);
protected:

	CClassFactory *m_classFactory;
};


extern CTsfModule *theModule;