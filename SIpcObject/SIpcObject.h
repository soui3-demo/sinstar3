#pragma once
#include <interface/SIpcObj-i.h>
#include <unknown/obj-ref-impl.hpp>
#include <map>
#include "ShareMemBuffer.h"

#ifdef _LIB
#define SIPC_API
#define SIPC_COM_C 
#else
#ifdef BUILD_SIPC
#define SIPC_API __declspec(dllexport)
#else
#define SIPC_API __declspec(dllimport)
#define SIPC_COM_C  EXTERN_C
#endif
#endif


namespace SOUI
{
	class SIpcHandle : public TObjRefImpl<IIpcHandle>
	{
	public:
		SIpcHandle();
		virtual ~SIpcHandle();

	public:

		virtual void SetIpcConnection(IIpcConnection *pConn) {
			m_pConn = pConn;
		}

		virtual IIpcConnection * GetIpcConnection() const {
			return m_pConn;
		}

		virtual LRESULT OnMessage(ULONG_PTR idLocal, UINT uMsg, WPARAM wp, LPARAM lp, BOOL &bHandled);

		virtual HRESULT ConnectTo(ULONG_PTR idLocal, ULONG_PTR idRemote);

		virtual HRESULT Disconnect();

		virtual bool CallFun(IFunParams * pParam) const;

		virtual ULONG_PTR GetLocalId() const {
			return (ULONG_PTR)m_hRemoteId;
		}

		virtual ULONG_PTR GetRemoteId() const {
			return (ULONG_PTR)m_hRemoteId;
		}

		virtual IShareBuffer * GetSendBuffer() override
		{
			return &m_SendBuf;
		}

		virtual IShareBuffer * GetRecvBuffer()  override
		{
			return &m_RecvBuf;
		}

		virtual BOOL InitShareBuf(ULONG_PTR idLocal, ULONG_PTR idRemote, UINT nBufSize, void* pSa);
	protected:
		HWND	m_hLocalId;
		mutable CShareMemBuffer	m_SendBuf;
		HWND	m_hRemoteId;
		CShareMemBuffer	m_RecvBuf;

		IIpcConnection * m_pConn;
	};


	class SIpcServer : public TObjRefImpl<IIpcServer>
	{
	public:
		SIpcServer();


	public:
		// 通过 TObjRefImpl 继承
		virtual HRESULT Init(ULONG_PTR idSvr, IIpcSvrCallback * pCallback) override;
		virtual void CheckConnectivity() override;
		virtual LRESULT OnMessage(ULONG_PTR idLocal, UINT uMsg, WPARAM wp, LPARAM lp,BOOL &bHandled) override;

	private:
		LRESULT OnConnect(HWND hClient);
		LRESULT OnDisconnect(HWND hClient);
		LRESULT OnClientMsg(UINT uMsg, WPARAM wp, LPARAM lp);
	private:
		WNDPROC			  m_prevWndProc;
		IIpcSvrCallback * m_pCallback;
		HWND			  m_hSvr;
		std::map<HWND, IIpcConnection *> m_mapClients;
	};

	class SIpcFactory : public TObjRefImpl<IIpcFactory>
	{
	public:
		// 通过 TObjRefImpl 继承
		virtual HRESULT CreateIpcServer(IIpcServer ** ppServer) override;
		virtual HRESULT CreateIpcHandle(IIpcHandle ** ppHandle) override;
	};
	namespace IPC
	{
		SIPC_COM_C BOOL SIPC_API SCreateInstance(IObjRef **ppIpcFactory);
	}
}

