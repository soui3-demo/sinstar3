// sinstar3_core.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "Sinstar3Impl.h"
#include "../include/autolock.h"
#include "SouiEnv.h"

CSinstar3Core	*theCore = NULL;


EXTERN_C BAIDUJP3_API  ISinstar* BaiduJP3_Create(ITextService* pTxtSvr,HINSTANCE hInst)
{
	CSinstar3Impl*  pRet= new CUnknownImpl<CSinstar3Impl>(pTxtSvr,hInst);
	return pRet;
}

EXTERN_C BAIDUJP3_API  void BaiduJP3_Delete(ISinstar* pBaiduJP3)
{
	pBaiduJP3->Release();
}

EXTERN_C BAIDUJP3_API HRESULT BaiduJP3_CanUnloadNow()
{
	return theCore->GetRefCount()==0;
}

EXTERN_C BAIDUJP3_API BOOL BaiduJP3_Config(HWND hWnd)
{
	return FALSE;
}


CSinstar3Core::CSinstar3Core(HINSTANCE hInst):m_cRef(0)
{
	InitializeCriticalSection(&m_cs);
	new CSouiEnv(hInst);
}

CSinstar3Core::~CSinstar3Core()
{
	delete CSouiEnv::getSingletonPtr();
	DeleteCriticalSection(&m_cs);
}

LONG CSinstar3Core::AddRef()
{
	CAutoLock lock(&m_cs);

	if(m_cRef == 0)
	{

	}

	return ++m_cRef;
}

LONG CSinstar3Core::Release()
{
	CAutoLock lock(&m_cs);
	LONG ret = --m_cRef;
	if(ret == 0)
	{

	}
	return ret;
}

LONG CSinstar3Core::GetRefCount()
{
	CAutoLock lock(&m_cs);
	return m_cRef;
}
