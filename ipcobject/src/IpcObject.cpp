#include "stdafx.h"
#include "IpcObject.h"

SIpcObject::SIpcObject()
{
}

SIpcObject::~SIpcObject(void)
{
	Uninit();
}	

LRESULT SIpcObject::OnClientMsg(UINT uMsg,WPARAM wp,LPARAM lp)
{
	if (wp == FUN_ID_LOGIN)
		return OnLogin((HWND)lp);
	else if (wp == FUN_ID_LOGOUT)
		return OnLogout((HWND)lp);
	return HandleReq((HWND)lp,(UINT)wp);
}


LRESULT SIpcObject::HandleFunction(HWND hClient, UINT uMsgID)
{
	if (m_mapClients.find(hClient) == m_mapClients.end())
		return 0;
	CShareMemBuffer *pData = m_mapClients[hClient];
	CParamStream ps(pData, false);
	return HandleFun(hClient, uMsgID, ps);
}

LRESULT SIpcObject::OnLogin(HWND hClient)
{
	if (m_mapClients.find(hClient) != m_mapClients.end()) return 0;

	CShareMemBuffer *pMemMapFile = new CShareMemBuffer;
	TCHAR szName[100];
	GetMemMapFileByObjectID(hClient, szName);
	if (!pMemMapFile->OpenMemFile(szName, 1 << 12))
	{
		delete pMemMapFile;
		return 0;
	}
	m_mapClients[hClient] = pMemMapFile;
	return 1;
}

LRESULT SIpcObject::OnLogout(HWND hClient)
{
	if (m_mapClients.find(hClient) == m_mapClients.end())
		return 0;

	CShareMemBuffer *pMemMapFile = m_mapClients[hClient];
	delete pMemMapFile;
	m_mapClients.erase(hClient);
	return 1;
}

void SIpcObject::GetMemMapFileByObjectID(HWND hWnd,TCHAR *szName)
{
	_stprintf(szName,_T("ipc_client_%08x"),(DWORD)hWnd);
}

int SIpcObject::Init(HWND hWnd,UINT uBufSize)
{
	TCHAR szName[100];
	GetMemMapFileByObjectID(hWnd,szName);
	m_buffer.OpenMemFile(szName, uBufSize);
	return 0;
}


void SIpcObject::Uninit()
{
	m_buffer.Close();
	std::map<HWND, CShareMemBuffer*>::iterator it = m_mapClients.begin();
	while (it != m_mapClients.end())
	{
		delete it->second;
		it++;
	}
	m_mapClients.clear();
}

BOOL SIpcObject::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	if(UM_CALL_FUN != uMsg) return FALSE;
	lResult = OnClientMsg(uMsg, wParam, lParam);
	return TRUE;
}

LRESULT SIpcObject::Login(HWND hSvr)
{
	LRESULT lRet = ::SendMessage(hSvr,UM_CALL_FUN, FUN_ID_LOGIN, (LPARAM)GetIpcObjectID());
	if (lRet == 0) return 0;
	OnLogin(hSvr);
	return lRet;
}

LRESULT SIpcObject::Logout(HWND hSvr)
{
	LRESULT lRet = ::SendMessage(hSvr, UM_CALL_FUN, FUN_ID_LOGOUT, (LPARAM)GetIpcObjectID());
	if (lRet == 0) return 0;
	OnLogout(hSvr);
	return lRet;
}

LRESULT SIpcObject::CallFun(HWND hSvr, FunParams_Base * pParam)
{
	CParamStream ps(GetBuffer(), true);
	pParam->ToStream4Input(ps);
	LRESULT lRet = SendMessage(hSvr,UM_CALL_FUN, pParam->GetID(), (LPARAM)GetIpcObjectID());
	if (lRet != 0)
	{
		CParamStream ps2(GetBuffer(), false);
		pParam->FromStream4Output(ps2);
	}
	return lRet;
}
