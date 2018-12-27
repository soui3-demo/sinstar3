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
LRESULT HandleFun(SIpcObject* pIpcObj,UINT uMsg, CParamStream &ps){ \
	LRESULT nRet = 0;\
	bool bHandled = false; \

#define FUN_HANDLER(x,fun) \
	if(!bHandled && uMsg == x::FUN_ID) \
	{\
		x param; \
		param.FromStream4Input(ps);\
		nRet = fun(pIpcObj,param); \
		CParamStream psOut(ps.GetBuffer(),true);\
		param.ToStream4Output(psOut);\
		bHandled = true;\
	}

#define FUN_END \
	return nRet; \
}

enum {
	FUN_ID_LOGIN = 100,
	FUN_ID_LOGOUT,
	FUN_ID_START,
};

class SIpcObject;
struct IIpcClient : IObjRef
{
	virtual LRESULT HandleFun(SIpcObject *pIpcObj,UINT uFunID, CParamStream & ps) = 0;
};

class SIpcObject
{
public:
	SIpcObject();
	~SIpcObject(void);

public:
	int Init(HWND hWnd,UINT uBufSize=4096);
	void Uninit();

	LRESULT Login(HWND hSvr);

	LRESULT Logout(HWND hSvr);

	LRESULT CallFun(HWND hSvr, FunParams_Base * pParam);

public:
	static void GetMemMapFileByObjectID(HWND hWnd,TCHAR *szName);

	CShareMemBuffer * GetBuffer() { return &m_buffer; }

	BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult);

protected:
	virtual HWND GetIpcObjectID() = 0;
	virtual HRESULT CreateIpcClient(HWND hClient, IIpcClient ** ppClient) const {
		return E_NOTIMPL;
	}

	LRESULT HandleFunction(HWND hClient, UINT uMsgID);
	LRESULT OnLogin(HWND hClient);
	LRESULT OnLogout(HWND hClient);
	LRESULT OnClientMsg(UINT uMsg,WPARAM wp,LPARAM lp);

	CShareMemBuffer	m_buffer;
	struct ClientData {
		ClientData():pClient(NULL),pBuf(NULL){}
		~ClientData()
		{
			if (pBuf) delete pBuf;
			if (pClient) pClient->Release();
		}

		IIpcClient		* pClient;
		CShareMemBuffer * pBuf;
	};
	std::map<HWND, ClientData *> m_mapClients;

};


