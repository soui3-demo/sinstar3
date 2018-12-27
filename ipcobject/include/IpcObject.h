#pragma once


#define UM_CALL_FUN (WM_USER+1000)

#include "paramstream.hpp"
#include <map>
#include <unknown/obj-ref-i.h>

struct FunParams_Base
{
	virtual UINT GetID() = 0;
	virtual void ToStream4Input(CParamStream &  ps) {}
	virtual void FromStream4Input(CParamStream &  ps) {}
	virtual void ToStream4Output(CParamStream &  ps) {}
	virtual void FromStream4Output(CParamStream &  ps) {}
	virtual std::string toString() const { return "not implemented!"; }
};

#define FUN_BEGIN \
bool HandleFun(UINT uMsg, CParamStream &ps){ \
	bool bHandled = false; \

#define FUN_HANDLER(x,fun) \
	if(!bHandled && uMsg == x::FUN_ID) \
	{\
		x param; \
		param.FromStream4Input(ps);\
		fun(param); \
		CParamStream psOut(ps.GetBuffer(),true);\
		param.ToStream4Output(psOut);\
		bHandled = true;\
	}

#define FUN_END \
	return bHandled; \
}

enum {
	FUN_ID_CONNECT = 100,
	FUN_ID_DISCONNECT,
	FUN_ID_START,
};

class SIpcConnection : IObjRef
{
	friend class SIpcServer;
public:
	SIpcConnection();

	LRESULT ConnectTo(HWND hRemote);

	LRESULT Disconnect();

	BOOL SetRemoteId(HWND hRemote);

	BOOL SetLocalId(HWND hLocal, UINT uBufSize);

	LRESULT CallFun(FunParams_Base * pParam) const;
public:
	CShareMemBuffer * GetLocalBuffer() { return &m_localBuf; }
	CShareMemBuffer * GetRemoteBuffer() { return &m_RemoteBuf; }
	BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult);

	static void GetMemMapFileByObjectID(HWND hWnd, TCHAR *szName) {
		_stprintf(szName, _T("ipc_client_%08x"), (DWORD)hWnd);
	}

protected:
	virtual bool HandleFun(UINT uFunID, CParamStream & ps) = 0;

protected:
	HWND	m_hLocalId;
	mutable CShareMemBuffer	m_localBuf;
	HWND	m_hRemoteId;
	CShareMemBuffer	m_RemoteBuf;
};

class SIpcServer
{
public:
	SIpcServer();
	~SIpcServer(void);
public:

	BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult);

protected:
	LRESULT OnConnect(HWND hClient);
	LRESULT OnDisconnect(HWND hClient);
	LRESULT OnClientMsg(UINT uMsg, WPARAM wp, LPARAM lp);

protected:
	virtual UINT GetBufSize() const { return 1 << 12; }
	virtual HWND GetSvrId() = 0;
	virtual HRESULT CreateConnection(SIpcConnection ** ppConnection) const {
		return E_NOTIMPL;
	}

	std::map<HWND, SIpcConnection *> m_mapClients;
};


