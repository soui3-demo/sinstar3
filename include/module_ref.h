#pragma once

#include "autolock.h"

class CModuleRef
{
public:
	CModuleRef::CModuleRef(HINSTANCE hInst):m_cRef(0),m_hInst(hInst)
	{
		InitializeCriticalSection(&m_cs);
	}

	virtual CModuleRef::~CModuleRef()
	{
		DeleteCriticalSection(&m_cs);
	}

	LONG AddRef()
	{
		CAutoLock lock(&m_cs);

		if(m_cRef == 0)
		{
			OnInit();
		}

		return ++m_cRef;
	}

	LONG Release()
	{
		CAutoLock lock(&m_cs);
		LONG ret = --m_cRef;
		if(ret == 0)
		{
			OnFinalRelease();
		}
		return ret;
	}

	LONG GetRefCount()
	{
		CAutoLock lock(&m_cs);
		return m_cRef;
	}

	HINSTANCE GetModule()
	{
		return m_hInst;
	}
protected:
	virtual void OnInit(){}
	virtual void OnFinalRelease(){}

protected:
	LONG m_cRef;
	HINSTANCE m_hInst;
	CRITICAL_SECTION m_cs;
};

