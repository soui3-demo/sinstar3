#include "stdafx.h"
#include "ClassFactory.h"

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
