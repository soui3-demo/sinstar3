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
		it->second->Release();
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
	if (!pConnection->OpenShareBuffer(GetSvrId(), hClient, GetBufSize()))
		goto error;
	m_mapClients[hClient] = pConnection;
	return 1;
error:
	if (pConnection)
	{
		pConnection->Release();
	}
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

void SIpcServer::CheckConnectivity()
{
	std::map<HWND, SIpcConnection *>::iterator it = m_mapClients.begin();
	while(it != m_mapClients.end())
	{
		if(!::IsWindow(it->first))
		{
			delete it->second;
			it = m_mapClients.erase(it);
		}else
		{
			it++;
		}
	}
}

//////////////////////////////////////////////////////////////
SIpcConnection::SIpcConnection():m_hLocalId(NULL),m_hRemoteId(NULL)
{
}

LRESULT SIpcConnection::ConnectTo(HWND hLocal,HWND hRemote)
{
	if (m_hLocalId != NULL)
		return 1;
	if (m_hRemoteId != NULL)
		return 2;

	LRESULT lRet = ::SendMessage(hRemote,UM_CALL_FUN, FUN_ID_CONNECT, (LPARAM)hLocal);
	if (lRet == 0)
	{
		return 0;
	}
	OpenShareBuffer(hLocal, hRemote, 0);
	return lRet==1?0:3;
}

LRESULT SIpcConnection::Disconnect()
{
	if (m_hLocalId == NULL)
		return 1;
	if (m_hRemoteId == NULL)
		return 2;
	::SendMessage(m_hRemoteId, UM_CALL_FUN, FUN_ID_DISCONNECT, (LPARAM)m_hLocalId);
	m_hRemoteId = NULL;
	m_RemoteBuf.Close();
	m_hLocalId = NULL;
	m_localBuf.Close();
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

BOOL SIpcConnection::OpenShareBuffer(HWND hLocal, HWND hRemote, DWORD uBufSize)
{
	assert(m_hRemoteId == NULL);
	assert(m_hLocalId == NULL);
	
	TCHAR szName[MAX_PATH];
	GetMemMapFileByObjectID(hLocal,hRemote, szName);
	if (!m_localBuf.OpenMemFile(szName, uBufSize))
	{
		return FALSE;
	}
	m_hLocalId = hLocal;

	GetMemMapFileByObjectID(hRemote,hLocal, szName);

	if (!m_RemoteBuf.OpenMemFile(szName, uBufSize))
	{
		return FALSE;
	}
	m_hRemoteId = hRemote;

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

void SIpcConnection::GetMemMapFileByObjectID(HWND hFrom, HWND hTo, TCHAR szName[MAX_PATH])
{
	_stprintf(szName, _T("sistart3_share_buffer_8085395F-E2FA-4F96-8BD0-FE5D7412CD22_%08x_2_%08x"), (DWORD)(((LPARAM)hFrom)&0xffffffff), (DWORD)(((LPARAM)hTo) & 0xffffffff));
}
