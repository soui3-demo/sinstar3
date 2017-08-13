#pragma once

//+---------------------------------------------------------------------------
//
//  CClassFactory declaration with IClassFactory Interface
//
//----------------------------------------------------------------------------

class CClassFactory : public IClassFactory
{
public:
	// IUnknown methods
	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObj);
	STDMETHODIMP_(ULONG) AddRef(void);
	STDMETHODIMP_(ULONG) Release(void);

	// IClassFactory methods
	STDMETHODIMP CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppvObj);
	STDMETHODIMP LockServer(BOOL fLock);

	// Constructor
	CClassFactory(REFCLSID rclsid, HRESULT (*pfnCreateInstance)(IUnknown *, REFIID , void **))
		: _rclsid(rclsid)
	{
		_pfnCreateInstance = pfnCreateInstance;
	}

public:
	REFCLSID _rclsid;
	HRESULT (*_pfnCreateInstance)(IUnknown *pUnkOuter, REFIID riid, void **ppvObj);
};