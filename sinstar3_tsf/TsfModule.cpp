#include "stdafx.h"
#include "TsfModule.h"
#include "ClassFactory.h"
#include "sinstar3_tsf.h"

CTsfModule::CTsfModule(HINSTANCE hInst):CModuleRef(hInst),m_classFactory(NULL)
{
}

CTsfModule::~CTsfModule(void)
{
}

void CTsfModule::OnInit()
{
}

void CTsfModule::OnFinalRelease()
{
	if(m_classFactory)
		delete m_classFactory;
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
