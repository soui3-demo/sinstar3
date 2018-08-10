// sinstar3_core.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "Sinstar3Impl.h"
#include "../include/autolock.h"
#include "../include/version.h"
#include "SouiEnv.h"
#include "Minidump.h"
#include "dataCenter/DataCenter.h"


EXTERN_C SINSTAR3_API  ISinstar* Sinstar3_Create(ITextService* pTxtSvr)
{
	CSinstar3Impl*  pRet= new CUnknownImpl<CSinstar3Impl>(pTxtSvr);
	return pRet;
}

EXTERN_C SINSTAR3_API  void Sinstar3_Delete(ISinstar* pSinstar3)
{
	pSinstar3->Release();
}

EXTERN_C SINSTAR3_API HRESULT Sinstar3_CanUnloadNow()
{
	return theModule->GetRefCount()==0;
}

EXTERN_C SINSTAR3_API BOOL Sinstar3_Config(HWND hWnd)
{
	return FALSE;
}


EXTERN_C SINSTAR3_API void Sinstar3_SetHostInfo(HostInfo *pHostInfo)
{
	theModule->SetLogStateListener(pHostInfo->pLogStateListener);
}


CSinstar3Core::CSinstar3Core(HINSTANCE hInst):CModuleRef(hInst),m_pLogStateListener(NULL), m_hMutex(0)
{
	m_hMutex = CreateMutex(NULL, FALSE, SINSTAR3_MUTEX);
	TCHAR szPath[MAX_PATH];
	CRegKey reg;
	LONG ret = reg.Open(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\SetoutSoft\\sinstar3"),KEY_READ|KEY_WOW64_64KEY);
	if(ret == ERROR_SUCCESS)
	{
		ULONG len = MAX_PATH;
		reg.QueryStringValue(_T("path_client"),szPath,&len);
		reg.Close();
	}
	m_strDataPath = szPath;
	m_strConfig = m_strDataPath + _T("\\data\\") + KSettingINI;
	if(0 == g_nRefCount++)
	{//the first time
		g_SettingsG.Load(m_strConfig);

	}
	new CDataCenter(m_strDataPath);
}

CSinstar3Core::~CSinstar3Core()
{
	delete CDataCenter::getSingletonPtr();
	g_SettingsG.Save(m_strConfig);
	g_nRefCount --;
	CloseHandle(m_hMutex);
}

void CSinstar3Core::OnInit()
{
	CMinidump::Enable();
	new CSouiEnv(GetModule(),GetDataPath());
	if(m_pLogStateListener)
	{
		m_pLogStateListener->OnLogMgrReady(CSouiEnv::getSingleton().theApp()->GetLogManager());
	}
}

void CSinstar3Core::OnFinalRelease()
{
	if(m_pLogStateListener)
	{
		m_pLogStateListener->OnLogMgrFree();
	}
	delete CSouiEnv::getSingletonPtr();
}

void CSinstar3Core::SetLogStateListener(ILogStateListener *pListener)
{
	m_pLogStateListener = pListener;
	if(CSouiEnv::getSingletonPtr())
	{
		m_pLogStateListener->OnLogMgrReady(CSouiEnv::getSingleton().theApp()->GetLogManager());
	}
}
