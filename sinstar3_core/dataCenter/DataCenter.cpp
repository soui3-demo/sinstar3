#include "StdAfx.h"
#include "DataCenter.h"

namespace SOUI
{
	template<>
	CDataCenter * SSingleton<CDataCenter>::ms_Singleton = NULL;

	CDataCenter::CDataCenter(void)
	{
		InitializeCriticalSection(&m_cs);

		m_compInfo.cWild = 'z';
		m_compInfo.strCompName = _T("Îå±Ê86");
	}

	CDataCenter::~CDataCenter(void)
	{
		DeleteCriticalSection(&m_cs);
	}

	void CDataCenter::Lock()
	{
		EnterCriticalSection(&m_cs);
	}

	void CDataCenter::Unlock()
	{
		LeaveCriticalSection(&m_cs);
	}

	CDataCenter::AutoLocker CDataCenter::GetAutoLockerInstance()
	{
		return AutoLocker(getSingletonPtr());
	}
}


