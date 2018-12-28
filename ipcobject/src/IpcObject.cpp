#include "stdafx.h"
#include "IpcObject.h"
#include <assert.h>

SIpcServer::SIpcServer()
{
}

SIpcServer::~SIpcServer(void)
{
	std::map<HWND, SIpcConnection *>::iterator it = m_mapClients.begin();
	while (it != m_mapClients.end())
	{
		delete it->second;
		it++;
	}
	m_mapClients.clear();
}

BOOL SIpcServer::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	if (UM_CALL_FUN != uMsg) return FALSE;
	lResult = OnClientMsg(uMsg, wParam, lParam);
	return TRUE;
}

LRESULT SIpcServer::OnClientMsg(UINT uMsg,WPARAM wp,LPARAM lp)
{
	if (wp == FUN_ID_CONNECT)
		return OnConnect((HWND)lp);
	else if (wp == FUN_ID_DISCONNECT)
		return OnDisconnect((HWND)lp);
	HWND hClient = (HWND)lp;
	std::map<HWND, SIpcConnection*>::iterator it = m_mapClients.find(hClient);
	if (it == m_mapClients.end())
		return 0;
	CParamStream ps(it->second->GetRemoteBuffer(), false);
	return it->second->HandleFun((UINT)wp, ps)?1:0;
}

LRESULT SIpcServer::OnConnect(HWND hClient)
{
	if (m_mapClients.find(hClient) != m_mapClients.end()) return 0;

	SIpcConnection * pConnection=NULL;
	HRESULT hr = CreateConnection(&pConnection);
	if (hr != S_OK || !pConnection) goto error;
	pConnection->SetLocalId(GetSvrId(), GetBufSize());
	pConnection->SetRemoteId(hClient);

	m_mapClients[hClient] = pConnection;
	return 1;
error:
	delete pConnection;
	return 0;
}

LRESULT SIpcServer::OnDisconnect(HWND hClient)
{
	if (m_mapClients.find(hClient) == m_mapClients.end())
		return 0;

	SIpcConnection *pClient = m_mapClients[hClient];
	delete pClient;
	m_mapClients.erase(hClient);
	return 1;
}

//////////////////////////////////////////////////////////////
SIpcConnection::SIpcConnection():m_hLocalId(NULL),m_hRemoteId(NULL)
{
}

LRESULT SIpcConnection::ConnectTo(HWND hRemote)
{
	if (m_hRemoteId != NULL)
		return 1;
	assert(m_hLocalId != NULL);

	LRESULT lRet = ::SendMessage(hRemote,UM_CALL_FUN, FUN_ID_CONNECT, (LPARAM)m_hLocalId);
	if (lRet == 0)
	{
		return 0;
	}
	TCHAR szName[100];
	GetMemMapFileByObjectID(hRemote, szName);
	m_RemoteBuf.OpenMemFile(szName, 0);
	m_hRemoteId = hRemote;
	return lRet;
}

LRESULT SIpcConnection::Disconnect()
{
	if (m_hRemoteId == NULL)
		return 1;
	assert(m_hRemoteId != NULL);
	::SendMessage(m_hRemoteId, UM_CALL_FUN, FUN_ID_DISCONNECT, (LPARAM)m_hLocalId);
	m_hRemoteId = NULL;
	m_RemoteBuf.Close();
	return 0;
}

LRESULT SIpcConnection::CallFun(FunParams_Base * pParam) const
{
	if (m_hRemoteId == NULL)
		return 0;

	CParamStream ps(&m_localBuf, true);
	pParam->ToStream4Input(ps);
	LRESULT lRet = SendMessage(m_hRemoteId,UM_CALL_FUN, pParam->GetID(), (LPARAM)m_hLocalId);
	if (lRet != 0)
	{
		CParamStream ps2(&m_localBuf, false);
		pParam->FromStream4Output(ps2);
	}
	return lRet;
}

BOOL SIpcConnection::SetRemoteId(HWND hRemote)
{
	assert(m_hRemoteId == NULL);
	TCHAR szName[100];
	GetMemMapFileByObjectID(hRemote, szName);
	if (!m_RemoteBuf.OpenMemFile(szName))
	{
		return FALSE;
	}
	m_hRemoteId = hRemote;
	return TRUE;
}

BOOL SIpcConnection::SetLocalId(HWND hLocal,UINT uBufSize)
{
	assert(m_hLocalId == NULL);
	TCHAR szName[100];
	GetMemMapFileByObjectID(hLocal, szName);
	if (!m_localBuf.OpenMemFile(szName, uBufSize))
	{
		return FALSE;
	}
	m_hLocalId = hLocal;
	return TRUE;
}


BOOL SIpcConnection::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	if (UM_CALL_FUN != uMsg)
	{
		return FALSE;
	}
	HWND hRemote = (HWND)lParam;
	if (hRemote != m_hRemoteId)
	{
		return FALSE;
	}
	CParamStream ps(GetRemoteBuffer(), false);
	lResult = HandleFun((UINT)wParam, ps)?1:0;
	return TRUE;
}