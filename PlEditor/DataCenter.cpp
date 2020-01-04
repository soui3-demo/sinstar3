#include "StdAfx.h"
#include "DataCenter.h"
#include "../include/reg.h"
#include <helper/SFunctor.hpp>

namespace TASKLOOP
{
	SOUI_COM_C BOOL SCreateInstance(IObjRef **ppTaskLoop);
}

CDataCenter::CDataCenter(void):m_bReady(true)
{
	m_plEditor.SetProgCallBack(this);
	SNotifyCenter::getSingletonPtr()->addEvent(EVENTID(EventProgStart));
	SNotifyCenter::getSingletonPtr()->addEvent(EVENTID(EventProgRun));
	SNotifyCenter::getSingletonPtr()->addEvent(EVENTID(EventProgEnd));

	TASKLOOP::SCreateInstance((SOUI::IObjRef**)&m_taskLoop);
	m_taskLoop->start("pleditor_taskloop");
}

CDataCenter::~CDataCenter(void)
{
	m_taskLoop->stop();
}

void CDataCenter::OnLoadSysPL()
{
	TCHAR szSvrPath[MAX_PATH];
	CRegKey reg;
	LONG ret = reg.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\SetoutSoft\\sinstar3"), KEY_READ | KEY_WOW64_64KEY);
	if (ret != ERROR_SUCCESS)
	{
		return ;
	}
	ULONG len = MAX_PATH;
	reg.QueryStringValue(_T("path_svr_data"), szSvrPath, &len);
	reg.Close();

	TCHAR szPath[MAX_PATH];
	_stprintf(szPath,_T("%s\\default.spl"),szSvrPath);
	m_plEditor.Load(szPath);
}

void CDataCenter::LoadSysPL()
{
	STaskHelper::post(m_taskLoop,this,&CDataCenter::OnLoadSysPL,false);
}

void CDataCenter::OnStart(DWORD dwMax)
{
	SAutoLock autoLock(m_cs);
	m_bReady = false;

	EventProgStart *e = new EventProgStart(NULL);
	e->dwMax = dwMax;
	SNotifyCenter::getSingletonPtr()->FireEventAsync(e);
	e->Release();
}

void CDataCenter::OnProg(DWORD dwData,DWORD dwMax)
{
	EventProgRun *e = new EventProgRun(NULL);
	e->dwValue=dwData;
	e->dwMax = dwMax;
	SNotifyCenter::getSingletonPtr()->FireEventAsync(e);
	e->Release();
}

void CDataCenter::OnEnd()
{
	SAutoLock autoLock(m_cs);
	m_bReady = true;

	EventProgEnd *e = new EventProgEnd(NULL);
	SNotifyCenter::getSingletonPtr()->FireEventAsync(e);
	e->Release();

}

std::vector<GROUPINFO>  CDataCenter::GetGroup() const
{
	SAutoLock autoLock(m_cs);
	return m_plEditor.GetGroup();
}

void CDataCenter::LoadPL(LPCTSTR pszName)
{
	STaskHelper::post(m_taskLoop,this,&CDataCenter::OnLoadPL,pszName,false);	
}

void CDataCenter::OnLoadPL(const std::tstring &name)
{
	m_plEditor.Load(name.c_str());
}

void CDataCenter::SavePL(LPCTSTR pszName)
{
	m_plEditor.Save(pszName);
}

void CDataCenter::Import2Group(LPCTSTR pszFile,BYTE byRateMin, BYTE byRateMax,BYTE iGroup/*=-1*/)
{
	STaskHelper::post(m_taskLoop,this,&CDataCenter::OnImport2Group,pszFile,byRateMin,byRateMax,iGroup,false);
}

void CDataCenter::OnImport2Group(const std::tstring &strFile,BYTE byMin, BYTE byMax,BYTE iGroup/*=-1*/)
{
	m_plEditor.Import2Group(strFile.c_str(),byMin,byMax,iGroup);
}

void CDataCenter::AddGroup(GROUPINFO groupInfo)
{
	SAutoLock autoLock(m_cs);
	if(!m_bReady)
		return;
	m_plEditor.AddGroup(groupInfo.szName,groupInfo.szEditor,groupInfo.szRemark);
}
