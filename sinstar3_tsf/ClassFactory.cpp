#include "stdafx.h"
#include "ClassFactory.h"


CClassFactory::CClassFactory(REFCLSID rclsid, HRESULT (*pfnCreateInstance)(IUnknown *, REFIID , void **)) : _rclsid(rclsid)
{
	_pfnCreateInstance = pfnCreateInstance;

#ifdef ENABLE_LOG
	TCHAR szPathInstall[MAX_PATH] = { 0 };
	CRegKey reg;
	LONG ret = reg.Open(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\SetoutSoft\\sinstar3"),KEY_READ|KEY_WOW64_64KEY);
	if(ret == ERROR_SUCCESS)
	{
		ULONG len = MAX_PATH;
		reg.QueryStringValue(_T("path_client"), szPathInstall, &len);
		reg.Close();
	}
	new SOUI::SLogMgr(szPathInstall);
#endif
}

CClassFactory::~CClassFactory()
{
#ifdef ENABLE_LOG
	delete SOUI::SLogMgr::getSingletonPtr();
#endif
}

//+---------------------------------------------------------------------------
//
//  CClassFactory::QueryInterface
//
//----------------------------------------------------------------------------

STDAPI CClassFactory::QueryInterface(REFIID riid, void **ppvObj)
{
	if (IsEqualIID(riid, IID_IClassFactory) || IsEqualIID(riid, IID_IUnknown))
	{
		*ppvObj = this;
		theModule->AddRef();
		return NOERROR;
	}
	*ppvObj = NULL;
	return E_NOINTERFACE;
}

//+---------------------------------------------------------------------------
//
//  CClassFactory::AddRef
//
//----------------------------------------------------------------------------

STDAPI_(ULONG) CClassFactory::AddRef()
{
	return theModule->AddRef();
}

//+---------------------------------------------------------------------------
//
//  CClassFactory::Release
//
//----------------------------------------------------------------------------

STDAPI_(ULONG) CClassFactory::Release()
{
	return theModule->Release();
}

//+---------------------------------------------------------------------------
//
//  CClassFactory::CreateInstance
//
//----------------------------------------------------------------------------

STDAPI CClassFactory::CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppvObj)
{
	return _pfnCreateInstance(pUnkOuter, riid, ppvObj);
}

//+---------------------------------------------------------------------------
//
//  CClassFactory::LockServer
//
//----------------------------------------------------------------------------

STDAPI CClassFactory::LockServer(BOOL fLock)
{
	if (fLock)
	{
		theModule->AddRef();
	}
	else
	{
		theModule->Release();
	}

	return S_OK;
}
