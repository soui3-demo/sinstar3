#include "stdafx.h"
#include "TsfModule.h"
#include "ClassFactory.h"
#include "sinstar3_tsf.h"
#include "../sinstar3_proxy/SimpleWnd.h"
#include "../helper/helper.h"

CTsfModule::CTsfModule(HINSTANCE hInst, LPCTSTR pszSvrPath):CModuleRef(hInst),m_classFactory(NULL)
{
	CSimpleWnd::RegisterWndClass(hInst);
	_tcscpy(m_szSvrPath, pszSvrPath);
	SECURITY_ATTRIBUTES * psa = Helper_BuildLowIntegritySA();
	m_hMutex = CreateMutex(psa, FALSE, SINSTAR3_MUTEX);
	Helper_FreeSa(psa);
	if (!m_hMutex && GetLastError() == ERROR_ACCESS_DENIED)
	{
		m_hMutex = OpenMutex(SYNCHRONIZE, FALSE, SINSTAR3_MUTEX);
	}
}

CTsfModule::~CTsfModule(void)
{
	if(m_hMutex) CloseHandle(m_hMutex);
	CSimpleWnd::UnregisterWndClass(GetModule());
}

void CTsfModule::OnInit()
{
}

void CTsfModule::OnFinalRelease()
{
	if(m_classFactory)
	{
		delete m_classFactory;
		m_classFactory = NULL;
	}
}

void CTsfModule::BuildClassFactory()
{
	CAutoLock autoLock(&m_cs);
	if(m_classFactory == NULL)
		m_classFactory = new CClassFactory(c_clsidSinstar3TSF, CSinstar3Tsf::CreateInstance);

}

HRESULT CTsfModule::GetClassObject(REFCLSID rclsid, REFIID riid, void **ppvObj)
{
	if (IsEqualIID(riid, IID_IClassFactory) ||
		IsEqualIID(riid, IID_IUnknown))
	{
		if (m_classFactory && IsEqualGUID(rclsid, m_classFactory->_rclsid))
		{
			*ppvObj = (void *)m_classFactory;
			AddRef();    // class factory holds DLL ref count
			return NOERROR;
		}
	}
	*ppvObj = NULL;

	return CLASS_E_CLASSNOTAVAILABLE;

}
