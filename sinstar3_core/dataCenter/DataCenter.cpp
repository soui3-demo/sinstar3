#include "StdAfx.h"
#include "DataCenter.h"

namespace SOUI
{
	template<>
	CDataCenter * SSingleton<CDataCenter>::ms_Singleton = NULL;

	CDataCenter::CDataCenter(void)
	{
		InitializeCriticalSection(&m_cs);

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

	CMyData::CMyData()
	{
		m_reg.Create(HKEY_CURRENT_USER,L"Software\\Setoutsoft\\sinstar3",NULL,REG_OPTION_NON_VOLATILE,KEY_WRITE|KEY_READ|KEY_WOW64_64KEY,0,NULL);

		m_compInfo.cWild = 'z';
		m_compInfo.strCompName = _T("Îå±Ê86");

		TCHAR szSkin[MAX_PATH]={0};
		ULONG nSize = MAX_PATH;
		m_reg.QueryStringValue(_T("skin"),szSkin,&nSize);
		m_strSkin = szSkin;

		DWORD dwPos=-1;
		m_reg.QueryDWORDValue(_T("status_pos"),dwPos);
		m_ptStatus = CPoint(GET_X_LPARAM(dwPos),GET_Y_LPARAM(dwPos));

	}

	CMyData::~CMyData()
	{
		m_reg.SetDWORDValue(_T("status_pos"),MAKELPARAM(m_ptStatus.x,m_ptStatus.y));
		m_reg.SetStringValue(_T("skin"),m_strSkin);
		m_reg.Close();
	}

}


